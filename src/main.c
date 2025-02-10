#include <stdio.h>
#include <stdlib.h>

#include "../libchunk/chunk.h"

void print_chunk(char ***chunk, int width, int height, int depth)
{
    printf("%d\n%d\n%d\n", width, height, depth);

    for (int x = 0; x < width; x++)
    {
        for (int y = height - 1; y >= 0; y--)
        {
            for (int z = 0; z < depth; z++)
                printf("%d ", chunk[x][y][z]);
            printf("\n");
        }
        printf("\n");
    }
}

char ***read_chunk(int *width, int *height, int *depth)
{
    scanf("%d%d%d", width, height, depth);

    char ***chunk = malloc(*width * sizeof *chunk);

    for (int x = 0; x < *width; x++)
    {
        chunk[x] = malloc(*height * sizeof **chunk);
        for (int y = *height - 1; y >= 0; y--)
        {
            chunk[x][y] = malloc(*depth * sizeof ***chunk);

            for (int z = 0; z < *depth; z++)
                scanf("%hhd", &chunk[x][y][z]);
        }
    }

    return chunk;
}

int main(void)
{
    // Test case 2 binary data
    unsigned char test2[] = {0x44, 0xC5, 0x08, 0x41, 0x09};
    
    // Test case 3 binary data
    unsigned char test3[] = {0xC6, 0xA1, 0xF5, 0x45};
    
    // Choose which test to run
    unsigned char* test_data = test2;  // or test3

    int width = 16, height = 16, depth = 20;
    char*** result = chunk_decode(test_data, width, height, depth);
    
    // Free the result
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            free(result[i][j]);
        }
        free(result[i]);
    }
    free(result);
    return 0;
}
