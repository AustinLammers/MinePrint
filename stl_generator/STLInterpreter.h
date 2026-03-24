#ifndef _STL_INTERPRETER_
#define _STL_INTERPRETER_

#include <stdio.h>
#include <stdlib.h>

#define STL_FILE_HEADER_SIZE 80
#define STL_NUM_TRIANGLE_VERTICIES 3
#define STL_NUM_VERTEX_DIMENSIONS 3

//prints out all the triangles found in the file
int printTriangleVerticiesFromFile(char* filename);

//strips 80 byte header and returns the number of triangles in the file
int readHeader(FILE* file);

typedef struct {
    float normalVec[3];
    float verticies[3][3];
    unsigned short attrByteCount;
} triangle_t;


#endif