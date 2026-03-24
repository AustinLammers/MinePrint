#include "stdio.h"
#include "stdlib.h"
#include "STLGenerator.h"
#include "string.h"

int main (int argc, char** argv) {

    if (argc < 2) {
        printf("No input file specified.\n");
        return -1;
    }

}

blockNode_t* ingestFile(char* filepath) {

    FILE * file = fopen(filepath, "r");
    if (!file) {
        printf("We couldn't find '%s'", filepath);
        return NULL;
    }

    blockNode_t * list = NULL;

    int blocksCreated = 0;
    int lineLength = 0;
    char line[CSV_MAX_LINE_LENGTH] = {0};

    while ((lineLength = fscanf(file, "%s", &line[0])) > 0) {
        blockNode_t * block = (blockNode_t *) malloc(sizeof(blockNode_t));
        if (blocksCreated == 0) {
            list = block;
        }

        int cols = extractBlockData(&block->info, line);

        if (cols != CSV_COLUMNS ) {
            printf("error processing line %d in file\n", blocksCreated + 1); 
            printf("\tproc'd errant number of cols: %d\n", cols);
            DEBUG("\tline=%s\n", line);
        }



    }
}

int extractBlockData(block_t* block, char* csvLine) {
    if(strlen(csvLine) <= CSV_COLUMNS) { //simple check to see if our row makes any sense
        return 0;
    }

    return sscanf(csvLine, "%s, %d, %d", &block->x, &block->y, &block->z);

}
