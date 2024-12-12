#include "chunk.h"
#include <stdbool.h>
#include <stdlib.h>

char*** chunk_rotate_y(
    char*** chunk, int width, int height, int depth) {
        char ***rotated_chunk = malloc(depth * sizeof *rotated_chunk);
        for (int x = 0; x < depth; x++) {
            rotated_chunk[x] = malloc(height * sizeof **rotated_chunk);
            for (int y = height - 1; y >= 0; y--) {
                rotated_chunk[x][y] = malloc(width * sizeof ***rotated_chunk);
            }
        }
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < depth; k++) {
                    rotated_chunk[depth - k - 1][j][i] = chunk[i][j][k];
                }
            }
        }
        return rotated_chunk;
}

char*** chunk_apply_gravity(
    char*** chunk, int width, int height, int depth, int* new_height) {
        int i = 0, j = 0, k = 0;

        // Creating a copy of the grid to get rid of wrap around caused by CHARMAX
        int ***objects = malloc(width * sizeof *objects);
        for (i = 0; i < width; i++) {
            objects[i] = malloc(height * sizeof **objects);
            for (j = height - 1; j >= 0; j--) {
                objects[i][j] = malloc(depth * sizeof ***objects);
            }
        }

        // Copying the chunk into the objects grid
        for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++) {
                for (k = 0; k < depth; k++) {
                    objects[i][j][k] = chunk[i][j][k];
                }
            }
        }

        // Create dictionary KEY(index) = mark, ITEM(value) = block_type
        int *objects_blocks_dict = calloc(__SHRT_MAX__, sizeof(int));  // Suppose we won't have more objects :)

        int object_count = 0;
        for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++) {
                for (k = 0; k < depth; k++) {
                    if (objects[i][j][k] > BLOCK_AIR && objects[i][j][k] <= BLOCK_STONE) {
                        // Not air and not previously marked
                        int mark = object_count + BLOCK_STONE + 1;  // Numerotating found objects from 4
                        // We don't want 0 as it is the value for BLOCK_AIR
                        objects_blocks_dict[mark] = objects[i][j][k];
                        // Object #mark is composed of chunk[i][j][k] blocks
                        int_chunk_fill(objects, width, height, depth, i, j, k, mark);  // Mark(encrypt) an object
                        object_count++;
                    }
                }
            }
        }
        bool gravity_done = false;
        while (!gravity_done) {
            gravity_done = true;
            // Applying gravity to each object
            for (int objectID = 0; objectID < object_count; objectID++) {
                // Let's see if the object can fall
                bool can_fall = true;
                for (i = 0; i < width; i++) {
                    for (j = 0; j < height; j++) {
                        for (k = 0; k < depth; k++) {
                            if (objects[i][j][k] == objectID + BLOCK_STONE + 1) {  // Part of this object
                                if (j - 1 < 0 || (objects[i][j - 1][k] != 0
                                && objects[i][j - 1][k] != objects[i][j][k])) {
                                    // Floor reached or the block below is not empty
                                    can_fall = false;
                                }
                            }
                        }
                    }
                }
                if (can_fall) {
                    gravity_done = false;
                    for (i = 0; i < width; i++) {
                        for (j = 0; j < height; j++) {
                            for (k = 0; k < depth; k++) {
                                if (objects[i][j][k] == objectID + BLOCK_STONE + 1) {  // Part of this object
                                    // Dropping the entire object by a block
                                    objects[i][j - 1][k] = objects[i][j][k];
                                    objects[i][j][k] = 0;
                                }
                            }
                        }
                    }
                }
            }
        }

        *new_height = 0;
        for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++) {
                for (k = 0; k < depth; k++) {
                    if (objects[i][j][k] > 0) {  // Not air
                        // Decoding the object to the initial block type
                        for (int objectID = 0; objectID < object_count; objectID++) {
                            int mark = (char)(objectID + BLOCK_STONE + 1);  // Decoding 'key'
                            if (objects[i][j][k] == mark) {  // Find the object the block belongs to
                                objects[i][j][k] = objects_blocks_dict[mark];
                            }
                        }
                        if (j > *new_height) {
                            *new_height = j + 1;
                        }
                    }
                }
            }
        }

        // Build the chunk with the new height
        char ***new_chunk = malloc(width * sizeof *new_chunk);

        for (int x = 0; x < width; x++) {
            new_chunk[x] = malloc(*new_height * sizeof **new_chunk);
            for (int y = *new_height - 1; y >= 0; y--) {
                new_chunk[x][y] = malloc(depth * sizeof ***new_chunk);
                for (int z = 0; z < depth; z++) {
                    new_chunk[x][y][z] = (char)(objects[x][y][z]);
                }
            }
        }

        // Free the slaves

        for (i = 0; i < width; i++) {
            for (j = height - 1; j >= 0; j--) {
                free(chunk[i][j]);
            }
            free(chunk[i]);
        }
        free(chunk);

        for (i = 0; i < width; i++) {
            for (j = height - 1; j >= 0; j--) {
                free(objects[i][j]);
            }
            free(objects[i]);
        }
        free(objects);
        return new_chunk;
    }
