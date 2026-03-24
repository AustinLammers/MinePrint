#include "STLInterpreter.h"



int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid arguments\n\tUsage: stl_interpret <filename>\n");
    }

    FILE* stlFile = fopen(argv[1], "r");

    if(!stlFile) {
        printf("could not open file");
        return -1;
    }

    int numTriangles = readHeader(stlFile);

    printf("reading %d triangles of size %d bytes...\n", numTriangles, sizeof(triangle_t));

    triangle_t bytes = {0};
    int trianglesRead = 0;

    while (fread(&bytes, sizeof(bytes), 1, stlFile) > 0) {
        trianglesRead++;

        printf("triangle %d:\n", trianglesRead);
        printf("\tbytes:\n\t\t");
        printBytes(bytes);
        printf("\tnormal:\n\t\tx: %f\n\t\ty: %f\n\t\tz: %f\n", bytes.normalVec[0], bytes.normalVec[1], bytes.normalVec[2]);
        for (int i = 0; i < STL_NUM_TRIANGLE_VERTICIES; i++) {
            printf("\tvertex %d:\n\t\tx: %f\n\t\ty: %f\n\t\tz: %f\n", i, bytes.verticies[i][0], bytes.verticies[i][1], bytes.verticies[i][2]);
        }
    }



    if(trianglesRead != numTriangles) {
        printf("invalid number of triangle read, possible file corruption:\n\texpected: %d\n\tactual: %d\n", numTriangles, trianglesRead);
    }


    return trianglesRead;

}

int readHeader(FILE* file) {
    fseek(file, STL_FILE_HEADER_SIZE, SEEK_SET);

    unsigned int num = 0;

    fread(&num, sizeof(num), 1, file);

    return num;
}

void printBytes(triangle_t bytes) {

    char* buf = (char*) &bytes;
    printf("{ ");
    for (int i = 0; i < sizeof(bytes); i++) {
        printf("0x%hhx, ", buf[i]);
    }
    printf("\b\b }\n");
}