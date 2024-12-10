#include "chunk.h"
#include <math.h>

char*** chunk_place_block(
    char*** chunk, int width, int height, int depth,
    int x, int y, int z, char block) {
        if (x >= 0 && x < width) {
            if (y >= 0 && y < height) {
                if (z >= 0 && z < depth) {
                    chunk[x][y][z] = block;
                }
            }
        }
        return chunk;
}

char*** chunk_fill_cuboid(
    char*** chunk, int width, int height, int depth,
    int x0, int y0, int z0, int x1, int y1, int z1, char block) {
        if (x0 > x1) {
            int aux = x0;
            x0 = x1;
            x1 = aux;
        }
        if (y0 > y1) {
            int aux = y0;
            y0 = y1;
            y1 = aux;
        }
        if (z0 > z1) {
            int aux = z0;
            z0 = z1;
            z1 = aux;
        }
        for (int i = x0; i <= x1; i++) {
            for (int j = y0; j <= y1; j++) {
                for (int k = z0; k <= z1; k++) {
                    chunk_place_block(chunk, width, height, depth, i, j , k, block);
                }
            }
        }
        return chunk;
}

char*** chunk_fill_sphere(
    char*** chunk, int width, int height, int depth,
    int x, int y, int z, double radius, char block) {
        for (int i = (int)trunc(x - radius); i <= (int)trunc(x + radius); i++) {
            for (int j = (int)trunc(y - radius); j <= (int)trunc(y + radius); j++) {
                for (int k = (int)trunc(z - radius); k <= (int)trunc(z + radius); k++) {
                    if (sqrt(pow((i - x), 2) + pow((j - y), 2) + pow((k - z), 2)) <= radius) {
                        chunk_place_block(chunk, width, height, depth, i, j, k, block);
                    }
                }
            }
        }
        return chunk;
}
