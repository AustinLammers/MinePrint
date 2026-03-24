#ifndef STLGENERATOR_H
#define STLGENERATOR_H

#define DEBUG(...) printf(__VA_ARGS__)

#define CSV_MAX_LINE_LENGTH 128
#define CSV_COLUMNS 5

/* Face identifier definitions */
#define BLOCK_FACE_PLUS_X (0x1 << 0)
#define BLOCK_FACE_MINUS_X (0x1 << 1)
#define BLOCK_FACE_PLUS_Y (0x1 << 2)
#define BLOCK_FACE_MINUS_Y (0x1 << 3)
#define BLOCK_FACE_PLUS_Z (0x1 << 4)
#define BLOCK_FACE_MINUS_Z (0x1 << 5)
#define BLOCK_FACE_INVALID (0x3 << 6)

/* Structure to hold information about a block's position and faces */
typedef struct {
    int x;
    int y; 
    int z;
    char faces;
} block_t;

/* Structure defining a block linked list node */
typedef struct {
     block_t info;
     blockNode_t * next;
} blockNode_t;

/* This function takes a file path to a csv file as input 
    and returns a linked list of block data */
blockNode_t* ingestFile(char* filepath);

/** extract the block data from a CSV line, store it at the block pointer.
    returns the number of columns successfully processed */
int extractBlockData(block_t* block, char* csvLine);

/* Get the renderable faces for each block, returns the number of faces populated */
int calculateRenderedFaces(blockNode_t * list);


#endif