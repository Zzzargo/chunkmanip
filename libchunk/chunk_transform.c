#include "chunk.h"
#include <stdbool.h>

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
            for (j = 0; j < height; j++) {
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

        // Checking if there are interdependent objects
        // Two objects are interdependent if one object is at the same time above and below the other (or vice versa)
        // If there are interdependent objects, these will be treated as one object
        int *interdependent_objects_dict = calloc(__SHRT_MAX__, sizeof(int));
        for (int objectID = 0; objectID < object_count; objectID++) {
            int mark1 = objectID + BLOCK_STONE + 1;
            for (int objectID_2 = 0; objectID_2 < object_count; objectID_2++) {
                int mark2 = objectID_2 + BLOCK_STONE + 1;
                if (objectID != objectID_2) {
                    for (i = 0; i < width; i++) {
                        for (j = 0; j < height; j++) {
                            for (k = 0; k < depth; k++) {
                                if (j + 1 < height && j - 1 >= 0) {
                                    if (objects[i][j][k] == mark1) {
                                        if (objects[i][j + 1][k] == mark2 &&
                                        objects[i][j - 1][k] == mark2) {
                                            // Object 1 is above object 2 and object 2 is above object 1
                                            // Both objects will be treated as the same
                                            interdependent_objects_dict[mark1] = mark1;
                                            interdependent_objects_dict[mark2] = mark1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // Now we will apply gravity to each object
        // We will do this until no object can fall anymore


        bool gravity_done = false;
        while (!gravity_done) {
            gravity_done = true;
            // Applying gravity to each object
            for (int objectID = 0; objectID < object_count; objectID++) {
                // Let's see if the object can fall
                int dep_object = 0;
                int objectID_2 = 0;
                int mark1 = objectID + BLOCK_STONE + 1;
                bool interdependent = false;
                bool can_fall = true;
                for (i = 0; i < width && can_fall; i++) {
                    for (j = 0; j < height && can_fall; j++) {
                        for (k = 0; k < depth && can_fall; k++) {
                            if (objects[i][j][k] == mark1) {  // Part of this object
                                if (interdependent_objects_dict[mark1] == mark1) {
                                    interdependent = true;
                                    // Find the object it is interdependent with
                                    for (objectID_2 = 0; objectID_2 < object_count; objectID_2++) {
                                        int mark2 = objectID_2 + BLOCK_STONE + 1;
                                        if (interdependent_objects_dict[mark2] == mark1 && objectID_2 != objectID) {
                                            dep_object = objectID_2;
                                            bool obj2_can_fall = true;
                                            //  We need to also check if the dependent object can fall
                                            for (int x = 0; x < width; x++) {
                                                for (int y = 0; y < height; y++) {
                                                    for (int z = 0; z < depth; z++) {
                                                        if (objects[x][y][z] == mark2) {
                                                            if (y - 1 < 0 || (objects[x][y - 1][z] != 0 &&
                                                            interdependent_objects_dict[objects[x][y - 1][z]] !=
                                                            interdependent_objects_dict[mark1] &&
                                                            interdependent_objects_dict[objects[x][y - 1][z]] !=
                                                            interdependent_objects_dict[mark2])) {
                                                                obj2_can_fall = false;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            if (j - 1 < 0 || (objects[i][j - 1][k] != 0 &&
                                            interdependent_objects_dict[objects[i][j - 1][k]] !=
                                            interdependent_objects_dict[mark1] &&
                                            interdependent_objects_dict[objects[i][j - 1][k]] !=
                                            interdependent_objects_dict[mark2]) || !obj2_can_fall) {
                                                // Floor reached or the block below is not air or
                                                // a block of the same object union
                                                can_fall = false;
                                                break;
                                            }
                                        }
                                    }
                                } else {
                                    if (j - 1 < 0 || (objects[i][j - 1][k] != 0
                                    && objects[i][j - 1][k] != mark1)) {
                                        // Floor reached or the block below is not air or a block of the same object
                                        can_fall = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if (can_fall) {
                    gravity_done = false;
                    for (i = 0; i < width; i++) {
                        for (j = 1; j < height; j++) {
                            for (k = 0; k < depth; k++) {
                                if (interdependent) {
                                    if (interdependent_objects_dict[objects[i][j][k]] == mark1 || objects[i][j][k] ==
                                    dep_object + BLOCK_STONE + 1) {
                                        // Part of the union of objects
                                        // Dropping the entire object by a block
                                        objects[i][j - 1][k] = objects[i][j][k];
                                        objects[i][j][k] = 0;
                                    }
                                } else {
                                    if (objects[i][j][k] == mark1) {  // Part of this object
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
        }
        *new_height = 0;
        for (i = 0; i < width; i++) {
            for (j = 0; j < height; j++) {
                for (k = 0; k < depth; k++) {
                    if (objects[i][j][k] > 0) {  // Not air
                        // Decoding the object to the initial block type
                        for (int objectID = 0; objectID < object_count; objectID++) {
                            int mark = objectID + BLOCK_STONE + 1;  // Decoding 'key'
                            if (objects[i][j][k] == mark) {  // Find the object the block belongs to
                                objects[i][j][k] = objects_blocks_dict[mark];
                            }
                        }
                        if (j > *new_height - 1) {
                            *new_height = j + 1;
                        }
                    }
                }
            }
        }

        // Build the chunk with the new height
        for (i = 0; i < width; i++) {
            for (j = height - 1; j >= 0; j--) {
                free(chunk[i][j]);
            }
            free(chunk[i]);
        }
        free(chunk);

        chunk = malloc(width * sizeof(char **));
        for (i = 0; i < width; i++) {
            chunk[i] = malloc(*new_height * sizeof(char *));
            for (j = 0; j < *new_height; j++) {
                chunk[i][j] = malloc(depth * sizeof(char));
            }
        }

        for (i = 0; i < width; i++) {
            for (j = 0; j < *new_height; j++) {
                for (k = 0; k < depth; k++) {
                    chunk[i][j][k] = (char)objects[i][j][k];
                }
            }
        }

        // Free the slaves
        for (i = 0; i < width; i++) {
            for (j = height - 1; j >= 0; j--) {
                free(objects[i][j]);
            }
            free(objects[i]);
        }
        free(objects);
        free(objects_blocks_dict);

        return chunk;
    }
