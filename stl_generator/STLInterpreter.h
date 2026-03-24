#ifndef _STL_INTERPRETER_
#define _STL_INTERPRETER_

#include <stdio.h>
#include <stdlib.h>

#define STL_FILE_HEADER_SIZE 80
#define STL_NUM_TRIANGLE_VERTICIES 3
#define STL_NUM_VERTEX_DIMENSIONS 3

#pragma pack(1)
typedef struct {
    float normalVec[3];
    float verticies[3][3];
    unsigned short attrByteCount;
} triangle_t;
#pragma pack()

//prints out all the triangles found in the file
int printTriangleVerticiesFromFile(char* filename);

//strips 80 byte header and returns the number of triangles in the file
int readHeader(FILE* file);

//prints the bytes of the triangle struct
void printBytes(triangle_t bytes);




#endif