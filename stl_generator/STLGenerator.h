#ifndef STLGENERATOR_H
#define STLGENERATOR_H

#define DEBUG(...) printf(__VA_ARGS__)

#define CSV_MAX_LINE_LENGTH 128
#define CSV_FACES_STRING_LENGTH 60
#define CSV_COLUMNS 5
#define CSV_METADATA_FIELDS 3

#define MAX_BLOCK_ID_LENGTH 50

#define COORDS_NUM_DIRECTIONS 6

#define FILE_CHUNK_SIZE 4096

/* Face identifier definitions */

#define BLOCK_FACE_PLUS_X_SHIFT 0
#define BLOCK_FACE_MINUS_X_SHIFT 1
#define BLOCK_FACE_PLUS_Y_SHIFT 2
#define BLOCK_FACE_MINUS_Y_SHIFT 3
#define BLOCK_FACE_PLUS_Z_SHIFT 4
#define BLOCK_FACE_MINUS_Z_SHIFT 5
#define BLOCK_FACE_INVALID_SHIFT 6

#define MC_EAST_FACE_SHIFT BLOCK_FACE_PLUS_X_SHIFT
#define MC_WEST_FACE_SHIFT BLOCK_FACE_MINUS_X_SHIFT 
#define MC_SOUTH_FACE_SHIFT BLOCK_FACE_PLUS_Y_SHIFT
#define MC_NORTH_FACE_SHIFT BLOCK_FACE_MINUS_Y_SHIFT
#define MC_DOWN_FACE_SHIFT BLOCK_FACE_PLUS_Z_SHIFT 4
#define MC_UP_FACE_SHIFT BLOCK_FACE_MINUS_Z_SHIFT 5
#define MC_INVALID_FACE_SHIFT BLOCK_FACE_INVALID_SHIFT 6

#define BLOCK_FACE_PLUS_X (0x1 << BLOCK_FACE_PLUS_X_SHIFT)
#define BLOCK_FACE_MINUS_X (0x1 << BLOCK_FACE_MINUS_X_SHIFT)
#define BLOCK_FACE_PLUS_Y (0x1 << BLOCK_FACE_PLUS_Y_SHIFT)
#define BLOCK_FACE_MINUS_Y (0x1 << BLOCK_FACE_MINUS_Y_SHIFT)
#define BLOCK_FACE_PLUS_Z (0x1 << BLOCK_FACE_PLUS_Z_SHIFT)
#define BLOCK_FACE_MINUS_Z (0x1 << BLOCK_FACE_MINUS_Z_SHIFT)
#define BLOCK_FACE_INVALID (0x1 << BLOCK_FACE_INVALID_SHIFT)

#define MC_EAST_FACE (0x1 << MC_EAST_FACE_SHIFT)
#define MC_WEST_FACE (0x1 << MC_WEST_FACE_SHIFT)
#define MC_SOUTH_FACE (0x1 << MC_SOUTH_FACE_SHIFT)
#define MC_NORTH_FACE (0x1 << MC_NORTH_FACE_SHIFT)
#define MC_DOWN_FACE (0x1 << MC_DOWN_FACE_SHIFT)
#define MC_UP_FACE (0x1 << MC_UP_FACE_SHIFT)
#define MC_INVALID_FACE (0x1 << MC_INVALID_FACE_SHIFT)

#define BLOCK_TYPE_BLOCK 0
#define BLOCK_TYPE_STAIRS_BOT 1
#define BLOCK_TYPE_STAIRS_TOP 2
#define BLOCK_TYPE_SLAB_BOT 3
#define BLOCK_TYPE_SLAB_TOP 4

#define ROTATION_AXIS_X 0
#define ROTATION_AXIS_Y 1
#define ROTATION_AXIS_Z 2

/* Structure to hold information about a block's position and faces */
typedef struct {
    char blockName[MAX_BLOCK_ID_LENGTH];
    int x;
    int y; 
    int z;
    unsigned char faces;
    char type;
} block_t;

/* Structure defining a block linked list node */
typedef struct blockNode{
     block_t info;
     struct blockNode* next;
} blockNode_t;

/* This function takes a file path to a csv file as input 
    and returns a linked list of block data */
blockNode_t* ingestFile(char* filepath);

/** extract the block data from a CSV line, store it at the block pointer.
    returns the number of columns successfully processed */
int extractBlockData(block_t* block, char* csvLine);

/* Get the renderable faces for each block, returns the number of faces populated */
int calculateRenderedFaces(blockNode_t * list);

/* convert csv faces string to bitstring */
unsigned char getFaces(char* string);

/* generate an STL file from a list of blocks */
int generateSTLFromList(blockNode_t * blockList, char* fileName);

/* Encode the stair direction into the faces field */
unsigned char stairDirectionToFaces(char* string);


#endif