#include "stdio.h"
#include "stdlib.h"
#include "STLGenerator.h"
#include "resources/STLTemplates.h"
#include "string.h"

int main (int argc, char** argv) {

    if (argc < 3) {
        printf("usage: stlgenerate <inputFile> <outputFile>\n");
        return -1;
    }

    blockNode_t* blockList = ingestFile(argv[1]);

    int status = generateSTLFromList(blockList, argv[2]);

    return status;

}

blockNode_t* ingestFile(char* filepath) {

    FILE * file = fopen(filepath, "r");
    if (!file) {
        printf("We couldn't find '%s'", filepath);
        return NULL;
    }

    blockNode_t * list = NULL;

    int blocksCreated = 0;
    int currline = 1;
    int lineLength = 0;
    char line[CSV_MAX_LINE_LENGTH] = {0};

    //skip first line
    fscanf(file, "%[^\n]", &line[0]);
    //move past \n
    fseek(file, 1, SEEK_CUR);

    while ((lineLength = fscanf(file, "%[^\n]", &line[0])) > 0) {
        fseek(file, 1, SEEK_CUR);
        currline++;
        blockNode_t * block = (blockNode_t *) malloc(sizeof(blockNode_t));

        int cols = extractBlockData(&block->info, line);

        if (cols != CSV_COLUMNS && cols != -1) {
            printf("error processing line %d in file\n", currline); 
            printf("\tproc'd errant number of cols: %d\n", cols);
            DEBUG("\tline=%s\n", line);
            continue;
        }
        else if (cols == -1) {
            DEBUG("\tignoring air block on line %d\n", currline);
            continue;
        }

        //sucessfully got block data, add it to the list
        if (blocksCreated++ == 0) {
            block->next = NULL;
            list = block;
        }
        else {
            block->next = list;
            list = block;
        }

    }

    fclose(file);

    printf("***FILE PROCESSING FINISHED***\n\tsucessfully extracted %d blocks\n", blocksCreated);
    return list;
}

int extractBlockData(block_t* block, char* csvLine) {
    if(strlen(csvLine) <= CSV_COLUMNS) { //simple check to see if our row makes any sense
        return 0;
    }

    DEBUG("extracting data from line '%s'\n", csvLine);

    char facesString[CSV_FACES_STRING_LENGTH] = {0};
    int status = sscanf(csvLine, "%[^,],%d,%d,%d,%s", block->blockName, &block->x, &block->y, &block->z, facesString);

    if (status != CSV_COLUMNS) {
        printf("failed to extract data from line '%s'\n\textracted %d cols\n", csvLine, status);
        return status;
    }

    if (strncmp("minecraft:air", block->blockName, sizeof(block->blockName)) == 0) {
        return -1;
    }

    block->faces = facesToChar(facesString);
    DEBUG("\tfaces: %d\n", block->faces);
    return status;

}

unsigned char facesToChar(char* string) {
    char facesArr[COORDS_NUM_DIRECTIONS] = {0};

    int status = sscanf(string, "D:%hhd|U:%hhd|N:%hhd|S:%hhd|W:%hhd|E:%hhd", &facesArr[0], &facesArr[1], &facesArr[2], &facesArr[3], &facesArr[4], &facesArr[5]);

    return (facesArr[0] << BLOCK_FACE_PLUS_Z_SHIFT) |
            (facesArr[1] << BLOCK_FACE_MINUS_Z_SHIFT) |
            (facesArr[2] << BLOCK_FACE_MINUS_Y_SHIFT) |
            (facesArr[3] << BLOCK_FACE_PLUS_Y_SHIFT) |
            (facesArr[4] << BLOCK_FACE_MINUS_X_SHIFT) |
            (facesArr[5] << BLOCK_FACE_PLUS_X_SHIFT);
}

void adjustFacePosition(triangle_t* face, block_t blockInfo ){
    for (int i = 0; i < STL_TRIANGLES_PER_FACE; i++) {
                for(int j = 0; j < STL_NUM_TRIANGLE_VERTICIES; j++) {
                    face[i].verticies[j][0] += blockInfo.x * STL_EDGE_LENGTH;
                    face[i].verticies[j][1] += blockInfo.y * STL_EDGE_LENGTH;
                    face[i].verticies[j][2] += blockInfo.z * STL_EDGE_LENGTH;
                }
            }
}

void renderFace(block_t blockInfo, const char* template, FILE* file) {
    triangle_t face[2] = {0};
    memcpy(face, template, sizeof(face));
    adjustFacePosition(face, blockInfo);
    fwrite((void*) face, sizeof(char), sizeof(face), file);
}

int generateSTLFromList(blockNode_t * blockList, char* fileName) {

    blockNode_t * current = blockList;

    FILE* file = fopen(fileName, "wb+");
    FILE* tempFile = fopen("stlgen_temp_file", "wb+");

    if (file == NULL || tempFile == NULL) {
        return -1;
    }

    char header[STL_HEADER_LENGTH] = {0};
    int bytes = fwrite(header, sizeof(char), STL_HEADER_LENGTH, file);

    if (bytes != 80) {
        fclose(file);
        return -1;
    }

    unsigned int faceCount = 0;

    while (current != NULL) {
        printf("rendering faces for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);

        if ((current->info.faces & BLOCK_FACE_PLUS_X) != 0) {
            DEBUG("\trendering plus x face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_PLUS_X_FACE_BIN, tempFile);
            faceCount++;
        }

        if ((current->info.faces & BLOCK_FACE_MINUS_X) != 0) {
            DEBUG("\trendering minus x face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_MINUS_X_FACE_BIN, tempFile);
            faceCount++;
        }

        if ((current->info.faces & BLOCK_FACE_PLUS_Y) != 0) {
            DEBUG("\trendering plus y face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_PLUS_Y_FACE_BIN, tempFile);
            faceCount++;
        }

        if ((current->info.faces & BLOCK_FACE_MINUS_Y) != 0) {
            DEBUG("\trendering minus y face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_MINUS_Y_FACE_BIN, tempFile);
            faceCount++;
        }

        if ((current->info.faces & BLOCK_FACE_PLUS_Z) != 0) {
            DEBUG("\trendering plus z face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_PLUS_Z_FACE_BIN, tempFile);
            faceCount++;
        }

        if ((current->info.faces & BLOCK_FACE_MINUS_Z) != 0) {
            DEBUG("\trendering minus z face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            renderFace(current->info, STL_MINUS_Z_FACE_BIN, tempFile);
            faceCount++;
        }

        current = current->next;
 
    }

    faceCount *= 2;

    fwrite(&faceCount, sizeof(faceCount), 1, file);

    int bytesRead = 0;
    char fileChunk [FILE_CHUNK_SIZE] = {0};
    fseek(tempFile, 0, SEEK_SET);
    while ((bytesRead = fread(fileChunk, sizeof(char), FILE_CHUNK_SIZE, tempFile)) > 0) {
        fwrite(fileChunk, sizeof(char), bytesRead, file);
    }

    fclose(file);
    fclose(tempFile);

    remove("stlgen_temp_file");

    return 0;
    
}
