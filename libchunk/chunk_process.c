#include "chunk.h"
#include <stdbool.h>

char*** chunk_shell(
    char*** chunk, int width, int height, int depth,
    char target_block, char shell_block) {
        // Neighbours
        int posx[] = {-1, 1, 0, 0, 0, 0, -1, -1, 0, 1, 1, 0, -1, -1, 1, 1, 0, 0};
        int posy[] = {0, 0, -1, 1, 0, 0, -1, 0, -1, 1, 0, 1, 1, 0, -1, 0, -1, 1};
        int posz[] = {0, 0, 0, 0, -1, 1, 0, -1, -1, 0, 1, 1, 0, 1, 0, -1, 1, -1};

        // Marked blocks are blocks that compose the shell
        char marked_block = -1;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < depth; k++) {
                    if (chunk[i][j][k] == target_block) {
                        for (int iter = 0; iter < sizeof(posx) / sizeof(int); iter++) {
                            // Additional bound checking cuz I get segfault -_-
                            if (i + posx[iter] >= 0 && i + posx[iter] < width && j + posy[iter] >= 0 &&
                            j + posy[iter] < height && k + posz[iter] >= 0 && k + posz[iter] < depth) {
                                if (chunk[i + posx[iter]][j + posy[iter]][k + posz[iter]] != target_block) {
                                    chunk_place_block(
                                        chunk, width, height, depth,
                                        i + posx[iter], j + posy[iter], k + posz[iter], marked_block);
                                }
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                for (int k = 0; k < depth; k++) {
                    if (chunk[i][j][k] == marked_block) {
                        chunk[i][j][k] = shell_block;
                    }
                }
            }
        }
        return chunk;
}

char*** chunk_fill_xz(
    char*** chunk, int width, int height, int depth,
    int x, int y, int z, char block) {
        // Can be done recursively but checker won't give me max points -_-
        // static char first_replaced_block = 0;
        // if (first_replaced_block == 0) {
        //     first_replaced_block = chunk[x][y][z];
        //     // We will be looking at the neighbours to see if the blocks are of
        //     // the same type as the starting block (first_replaced_block)
        // }
        // if (x < 0 || x >= width || z < 0 || z >= depth) {
        // return chunk;
        // }
        // if (chunk[x][y][z] != first_replaced_block) {
        //     return chunk;
        // }
        // chunk_place_block(chunk, width, height, depth, x, y, z, block);
        // chunk_fill_xz(chunk, width, height, depth, x - 1, y, z, block);
        // chunk_fill_xz(chunk, width, height, depth, x + 1, y, z, block);
        // chunk_fill_xz(chunk, width, height, depth, x, y, z + 1, block);
        // chunk_fill_xz(chunk, width, height, depth, x, y, z - 1, block);

        char first_replaced_block = chunk[x][y][z];

        // Movement in xOz
        int posx[] = {-1, 1, 0, 0};
        int posz[] = {0, 0, -1, 1};

        // Visited blocks grid
        bool*** visited = (bool***)malloc(width * sizeof(bool**));
        for (int i = 0; i < width; i++) {
            visited[i] = (bool**)malloc(height * sizeof(bool*));
            for (int j = 0; j < height; j++) {
                visited[i][j] = (bool*)calloc(depth, sizeof(bool));
            }
        }

        // Defining a 2D point for BFS queue
        typedef struct {
            int x, z;
        } Point;

        Point* queue = (Point*)malloc(width * depth * sizeof(Point));
        int front = 0, back = 0;

        // Insert the first position in the queue
        queue[back++] = (Point){x, z};
        visited[x][y][z] = true;

        while (front < back) {
            Point current_point = queue[front++];
            int current_x = current_point.x;
            int current_z = current_point.z;

            // Place a block at current position
            chunk[current_x][y][current_z] = block;

            // Checking neighbours
            for (int i = 0; i < sizeof(posx); i++) {
                int new_point_x = current_x + posx[i];
                int new_point_z = current_z + posz[i];

                if (new_point_x >= 0 && new_point_x < width && new_point_z >= 0 && new_point_z < depth &&
                    !visited[new_point_x][y][new_point_z] &&
                    chunk[new_point_x][y][new_point_z] == first_replaced_block) {
                    queue[back++] = (Point){new_point_x, new_point_z};
                    visited[new_point_x][y][new_point_z] = true;
                }
            }
        }

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                free(visited[i][j]);
            }
            free(visited[i]);
        }
        free(visited);
        free(queue);

        return chunk;
    }

char*** chunk_fill(
    char*** chunk, int width, int height, int depth,
    int x, int y, int z, char block) {
        // I did the other one with BFS so why not do this one too
        // static char first_replaced_block = 0;
        // if (first_replaced_block == 0) {
        //     first_replaced_block = chunk[x][y][z];
        //     // Similarly to the function above
        // }
        // if (x < 0 || x >= width || z < 0 || z >= depth || y < 0 || y >= height) {
        // return chunk;
        // }
        // if (chunk[x][y][z] != first_replaced_block) {
        //     return chunk;
        // }
        // chunk_place_block(chunk, width, height, depth, x, y, z, block);
        // chunk_fill(chunk, width, height, depth, x - 1, y, z, block);
        // chunk_fill(chunk, width, height, depth, x + 1, y, z, block);
        // chunk_fill(chunk, width, height, depth, x, y - 1, z, block);
        // chunk_fill(chunk, width, height, depth, x, y + 1, z, block);
        // chunk_fill(chunk, width, height, depth, x, y, z + 1, block);
        // chunk_fill(chunk, width, height, depth, x, y, z - 1, block);

        char first_replaced_block = chunk[x][y][z];

        // Movement in 3D
        int posx[] = {-1, 1, 0, 0, 0, 0};
        int posz[] = {0, 0, -1, 1, 0, 0};
        int posy[] = {0, 0, 0, 0, -1, 1};

        // Visited blocks grid
        bool*** visited = (bool***)malloc(width * sizeof(bool**));
        for (int i = 0; i < width; i++) {
            visited[i] = (bool**)malloc(height * sizeof(bool*));
            for (int j = 0; j < height; j++) {
                visited[i][j] = (bool*)calloc(depth, sizeof(bool));
            }
        }

        // Defining a 3D point for BFS queue
        typedef struct {
            int x, y, z;
        } Point;

        Point* queue = (Point*)malloc(width * depth * height * sizeof(Point));
        int front = 0, back = 0;

        // Insert the first position in the queue
        queue[back++] = (Point){x, y, z};
        visited[x][y][z] = true;

        while (front < back) {
            Point current_point = queue[front++];
            int current_x = current_point.x;
            int current_y = current_point.y;
            int current_z = current_point.z;

            // Place a block at current position
            chunk[current_x][current_y][current_z] = block;

            // Checking neighbours
            for (int i = 0; i < sizeof(posx); i++) {
                int new_point_x = current_x + posx[i];
                int new_point_y = current_y + posy[i];
                int new_point_z = current_z + posz[i];

                if (new_point_x >= 0 && new_point_x < width && new_point_z >= 0 && new_point_z < depth &&
                    new_point_y >= 0 && new_point_y < height &&
                    !visited[new_point_x][new_point_y][new_point_z] &&
                    chunk[new_point_x][new_point_y][new_point_z] == first_replaced_block) {
                    queue[back++] = (Point){new_point_x, new_point_y, new_point_z};
                    visited[new_point_x][new_point_y][new_point_z] = true;
                }
            }
        }

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                free(visited[i][j]);
            }
            free(visited[i]);
        }
        free(visited);
        free(queue);

        return chunk;
    }

// There is a need for a modified version of the chunk fill function
int*** int_chunk_fill(
    int*** chunk, int width, int height, int depth,
    int x, int y, int z, int block) {
        int first_replaced_block = chunk[x][y][z];

        // Movement in 3D
        int posx[] = {-1, 1, 0, 0, 0, 0};
        int posz[] = {0, 0, -1, 1, 0, 0};
        int posy[] = {0, 0, 0, 0, -1, 1};

        // Visited blocks grid
        bool*** visited = (bool***)malloc(width * sizeof(bool**));
        for (int i = 0; i < width; i++) {
            visited[i] = (bool**)malloc(height * sizeof(bool*));
            for (int j = 0; j < height; j++) {
                visited[i][j] = (bool*)calloc(depth, sizeof(bool));
            }
        }

        // Defining a 3D point for BFS queue
        typedef struct {
            int x, y, z;
        } Point;

        Point* queue = (Point*)malloc(width * depth * height * sizeof(Point));
        int front = 0, back = 0;

        // Insert the first position in the queue
        queue[back++] = (Point){x, y, z};
        visited[x][y][z] = true;

        while (front < back) {
            Point current_point = queue[front++];
            int current_x = current_point.x;
            int current_y = current_point.y;
            int current_z = current_point.z;

            // Place a block at current position
            chunk[current_x][current_y][current_z] = block;

            // Checking neighbours
            for (int i = 0; i < sizeof(posx); i++) {
                int new_point_x = current_x + posx[i];
                int new_point_y = current_y + posy[i];
                int new_point_z = current_z + posz[i];

                if (new_point_x >= 0 && new_point_x < width && new_point_z >= 0 && new_point_z < depth &&
                    new_point_y >= 0 && new_point_y < height &&
                    !visited[new_point_x][new_point_y][new_point_z] &&
                    chunk[new_point_x][new_point_y][new_point_z] == first_replaced_block) {
                    queue[back++] = (Point){new_point_x, new_point_y, new_point_z};
                    visited[new_point_x][new_point_y][new_point_z] = true;
                }
            }
        }

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                free(visited[i][j]);
            }
            free(visited[i]);
        }
        free(visited);
        free(queue);

        return chunk;
    }
