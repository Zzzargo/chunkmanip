#include "chunk.h"
#include <stdbool.h>

#define RUN_MAX_LENGTH 4095
#define MAX_NUM_ON_5BITS 31
#define TWO_BYTES 2
#define BLOCK_TYPE_SHIFT 6
#define SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN 32
#define FIRST_BYTE_OF_TWO_BYTES_SHIFT 8
#define SECOND_BYTE_OF_TWO_BYTES_RUN_AND 255
#define TWO_BYTES_RUN_FIRST_BYTE_LENGTH_GETTER 15

unsigned char* chunk_encode(
    char*** chunk, int width, int height, int depth,
    int* length) {
        // run-length encoding (RLE) with bit operations
        // bb are bits that tell the run's block type
        // n wil represent the length of the run in binary
        int i = 0, j = 0, k = 0;
        *length = 0;
        int curr_run_length = 0;
        char curr_run_block = BLOCK_STONE + 1;  // The first value should be different
        unsigned char *tmp_output = calloc(width * height * depth, sizeof(char));
        bool last_block = false;
        // Order: y -> z -> x
        for (j = 0; j < height; j++) {
            for (k = 0; k < depth; k++) {
                for (i = 0; i < width; i++) {
                    if (j == height - 1 && k == depth - 1 && i == width - 1) {
                        // If it's the last block, check the run made so far
                        last_block = true;
                    }
                    if (chunk[i][j][k] != curr_run_block || curr_run_length == RUN_MAX_LENGTH) {
                        // If the current block is different from the previous block
                        // or the current run has reached the maximum length
                        // Starting a new run and writing the previous run to the output
                        if (j == 0 && k == 0 && i == 0) {
                            // If it's the first block, don't write anything because
                            // there is no run to write

                            curr_run_length = 1;
                            curr_run_block = chunk[i][j][k];
                            (*length)++;
                            continue;
                        }


                        if (curr_run_length == RUN_MAX_LENGTH) {
                            // If the current run has reached the maximum length, write it to the output
                            // It will be written as two bytes: bb10nnnn nnnnnnnn
                            tmp_output[*length - 1] = ((curr_run_block <<
                            BLOCK_TYPE_SHIFT) | SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN) |
                            ((unsigned char)(RUN_MAX_LENGTH >> FIRST_BYTE_OF_TWO_BYTES_SHIFT));
                            tmp_output[*length] = (unsigned char)(RUN_MAX_LENGTH & SECOND_BYTE_OF_TWO_BYTES_RUN_AND);
                            (*length) += TWO_BYTES;
                        } else {
                            // If the current run has ended
                            // Write the previous run to the output

                            if (curr_run_length <= MAX_NUM_ON_5BITS) {
                                // If the current run has less than 32 blocks
                                // It will be written as a single byte
                                // Its form will be bb0nnnnn

                                tmp_output[*length - 1] = curr_run_block << BLOCK_TYPE_SHIFT;
                                tmp_output[*length - 1] |= curr_run_length;
                                (*length)++;
                            } else {
                                // If the current run has 32 <= length < 4096
                                // It will be written as two bytes: bb10nnnn nnnnnnnn

                                tmp_output[*length - 1] = ((curr_run_block <<
                                BLOCK_TYPE_SHIFT) | SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN) |
                                ((unsigned char)(curr_run_length >> FIRST_BYTE_OF_TWO_BYTES_SHIFT));
                                tmp_output[*length] = (unsigned char)(curr_run_length &
                                SECOND_BYTE_OF_TWO_BYTES_RUN_AND);
                                (*length) += TWO_BYTES;
                            }
                        }


                        // Start a new run
                        curr_run_length = 1;
                        curr_run_block = chunk[i][j][k];
                    } else {
                        // In the same run
                        curr_run_length++;
                    }
                    if (last_block) {
                        // Force check
                        if (curr_run_length == RUN_MAX_LENGTH) {
                            // If the current run has reached the maximum length, write it to the output
                            // It will be written as two bytes: bb10nnnn nnnnnnnn
                            tmp_output[*length - 1] = ((curr_run_block <<
                            BLOCK_TYPE_SHIFT) | SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN) |
                            (unsigned char)(RUN_MAX_LENGTH >> FIRST_BYTE_OF_TWO_BYTES_SHIFT);
                            tmp_output[*length] = (unsigned char)(RUN_MAX_LENGTH & SECOND_BYTE_OF_TWO_BYTES_RUN_AND);
                            (*length)++;
                        } else {
                            if (curr_run_length <= MAX_NUM_ON_5BITS) {
                                // If the current run has less than 32 blocks
                                // It will be written as a single byte
                                // Its form will be bb0nnnnn

                                // bit operations; || I want 01000100
                                tmp_output[*length - 1] = curr_run_block << BLOCK_TYPE_SHIFT;
                                tmp_output[*length - 1] |= curr_run_length;
                                // printf("%u\n", tmp_output[(*length) - 1]);
                                // int test = 0;
                            } else {
                                // If the current run has 32 <= length < 4096
                                // It will be written as two bytes: bb10nnnn nnnnnnnn

                                tmp_output[*length - 1] = ((curr_run_block <<
                                BLOCK_TYPE_SHIFT) | SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN) |
                                ((unsigned char)(curr_run_length >> FIRST_BYTE_OF_TWO_BYTES_SHIFT));
                                tmp_output[*length] = (unsigned char)(curr_run_length &
                                SECOND_BYTE_OF_TWO_BYTES_RUN_AND);
                                (*length)++;
                            }
                        }
                    }
                }
            }
        }
        // Building the real output - without the zeroes at the end
        unsigned char* output = calloc(*length, sizeof(unsigned char));
        for (i = 0; i < *length; i++) {
            output[i] = tmp_output[i];
        }
        free(tmp_output);
        return output;
}

char*** chunk_decode(
    unsigned char* code, int width, int height, int depth) {
        int i = 0, x = 0, y = 0, z = 0;
        // I made the encoding, now it's time to decode
        // Allocate memory for the chunk to build
        char ***chunk = malloc(width * sizeof *chunk);
        for (x = 0; x < width; x++) {
            chunk[x] = malloc(height * sizeof **chunk);
            for (y = height - 1; y >= 0; y--) {
                chunk[x][y] = malloc(depth * sizeof ***chunk);
            }
        }

        for (i = 0; code[i]; i++) {
            unsigned char byte_type = code[i] & (char)SEPARATOR_FOR_FIRST_BYTE_OF_TWO_BYTES_RUN;
            unsigned char byte_block_type = code[i] >> BLOCK_TYPE_SHIFT;
            if (byte_type) {
                // The byte is the first of a two-byte run
                // We will read two bytes
                unsigned char byte1 = code[i];
                unsigned char byte2 = code[i + 1];
                int run_length = ((int)(byte1 | TWO_BYTES_RUN_FIRST_BYTE_LENGTH_GETTER) <<
                FIRST_BYTE_OF_TWO_BYTES_SHIFT) | (int)(byte2);
                int blocks_placed = 0;
                bool stop = false;
                for (y = 0; y < height && !stop; y++) {
                    for (z = 0; z < depth && !stop; z++) {
                        for (x = 0; x < width && !stop; x++) {
                            if (blocks_placed == run_length) {
                                stop = true;
                                break;
                            }
                            chunk[x][y][z] = (char)byte_block_type;
                            blocks_placed++;
                        }
                    }
                }
                i++;
            } else {
                // We will read one byte
                int run_length = code[i] & MAX_NUM_ON_5BITS;
                int blocks_placed = 0;
                bool stop = false;
                for (y = 0; y < height && !stop; y++) {
                    for (z = 0; z < depth && !stop; z++) {
                        for (x = 0; x < width && !stop; x++) {
                            if (blocks_placed == run_length) {
                                stop = true;
                                break;
                            }
                            chunk[x][y][z] = (char)byte_block_type;
                            blocks_placed++;
                        }
                    }
                }
            }
        }
        return chunk;
    }

