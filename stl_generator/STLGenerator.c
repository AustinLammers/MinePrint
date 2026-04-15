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

        if (cols != CSV_COLUMNS + CSV_METADATA_FIELDS && cols != -1) {
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
    char shapeString[CSV_FACES_STRING_LENGTH] = {0};
    char facingString[CSV_FACES_STRING_LENGTH] = {0};
    char halfString[CSV_FACES_STRING_LENGTH] = {0};

    int status = sscanf(csvLine, "%[^,],%d,%d,%d,%s", block->blockName, &block->x, &block->y, &block->z, facesString);

    char* metadata = strstr(facesString, "||");
    metadata += 2;
    DEBUG("meta: '%s'\n", metadata);

    int metaStatus = sscanf(metadata, "shape:%[^|]|facing:%[^|]|half:%s", shapeString, facingString, halfString);

    status += metaStatus;

    if (status != CSV_COLUMNS + CSV_METADATA_FIELDS) {
        printf("failed to extract data from line '%s'\n\textracted %d cols\n\tmetadata: '%s'\n\tshape: '%s'\n\tfacing: '%s'\n\thalf: '%s'\n", csvLine, status, metadata, shapeString, facingString, halfString);
        return status;
    }

    if (strncmp("minecraft:air", block->blockName, sizeof(block->blockName)) == 0) {
        return -1;
    }

    if (strncmp("SLAB_BOTTOM", shapeString, sizeof(shapeString)) == 0) {
        block->type = BLOCK_TYPE_SLAB_BOT;
    }
    else if (strncmp("SLAB_TOP", shapeString, sizeof(shapeString)) == 0) {
        block->type = BLOCK_TYPE_SLAB_TOP;
    }
    else if (strncmp("STAIR", shapeString, sizeof(shapeString)) == 0) {
        if(strncmp("top", halfString, CSV_FACES_STRING_LENGTH) == 0) {
            block->type = BLOCK_TYPE_STAIRS_TOP;
        }
        else {
            block->type = BLOCK_TYPE_STAIRS_BOT;
        }

        block->faces = stairDirectionToFaces(facingString);
    }
    else {
        block->type = BLOCK_TYPE_BLOCK;
        block->faces = getFaces(facesString);
    }

    
    DEBUG("\tfaces: %d\n", block->faces);
    return status;

}

unsigned char getFaces(char* string) {
    char facesArr[COORDS_NUM_DIRECTIONS] = {0};

    int status = sscanf(string, "D:%hhd|U:%hhd|N:%hhd|S:%hhd|W:%hhd|E:%hhd|", &facesArr[0], &facesArr[1], &facesArr[2], &facesArr[3], &facesArr[4], &facesArr[5]);

    return (facesArr[0] << BLOCK_FACE_PLUS_Z_SHIFT) |
            (facesArr[1] << BLOCK_FACE_MINUS_Z_SHIFT) |
            (facesArr[2] << BLOCK_FACE_MINUS_Y_SHIFT) |
            (facesArr[3] << BLOCK_FACE_PLUS_Y_SHIFT) |
            (facesArr[4] << BLOCK_FACE_MINUS_X_SHIFT) |
            (facesArr[5] << BLOCK_FACE_PLUS_X_SHIFT);
}

unsigned char stairDirectionToFaces(char* string) {
    if(strncmp("north", string, CSV_FACES_STRING_LENGTH) == 0) {
        return MC_NORTH_FACE;
    }
    else if (strncmp("east", string, CSV_FACES_STRING_LENGTH) == 0) {
        return MC_EAST_FACE;
    }
    else if (strncmp("south", string, CSV_FACES_STRING_LENGTH) == 0) {
        return MC_SOUTH_FACE;
    }
    else if (strncmp("west", string, CSV_FACES_STRING_LENGTH) == 0) {
        return MC_WEST_FACE;
    }
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

void rotateObj(char* obj, int numTriangles, int axis) {
    triangle_t* triangles = (triangle_t *) obj;

    for(int i = 0; i < numTriangles; i++ ){ 
        //rotate the coordinates 90 degrees clockwise
        float temp = 0;
        for (int j = 0; j < STL_NUM_TRIANGLE_VERTICIES; j++ ){
            switch (axis) {
                case ROTATION_AXIS_X:
                    temp = -1.0 * triangles[i].verticies[j][1];
                    triangles[i].verticies[j][1] = triangles[i].verticies[j][2];
                    triangles[i].verticies[j][2] = temp;

                    // 0.5 used here to eliminate floating point approximation errors
                    if (triangles[i].normalVec[1] > 0.5) {
                        triangles[i].normalVec[1] = 0;
                        triangles[i].normalVec[2] = -1.0;
                    }

                    if (triangles[i].normalVec[1] < -0.5) {
                        triangles[i].normalVec[1] = 0;
                        triangles[i].normalVec[2] = 1.0;
                    }

                    if (triangles[i].normalVec[2] > 0.5) {
                        triangles[i].normalVec[2] = 0;
                        triangles[i].normalVec[1] = -1.0;
                    }

                    if (triangles[i].normalVec[2] < -0.5) {
                        triangles[i].normalVec[2] = 0;
                        triangles[i].normalVec[1] = 1.0;
                    }
                    
                    break;
                case ROTATION_AXIS_Y:
                    temp = -1.0 * triangles[i].verticies[j][2];
                    triangles[i].verticies[j][2] = triangles[i].verticies[j][0];
                    triangles[i].verticies[j][0] = temp;

                    // 0.5 used here to eliminate floating point approximation errors
                    if (triangles[i].normalVec[0] > 0.5) {
                        triangles[i].normalVec[0] = 0;
                        triangles[i].normalVec[2] = -1.0;
                    }

                    if (triangles[i].normalVec[0] < -0.5) {
                        triangles[i].normalVec[0] = 0;
                        triangles[i].normalVec[2] = 1.0;
                    }

                    if (triangles[i].normalVec[2] > 0.5) {
                        triangles[i].normalVec[2] = 0;
                        triangles[i].normalVec[0] = -1.0;
                    }

                    if (triangles[i].normalVec[2] < -0.5) {
                        triangles[i].normalVec[2] = 0;
                        triangles[i].normalVec[0] = 1.0;
                    }

                    break;
                case ROTATION_AXIS_Z:
                    temp = -1.0 * triangles[i].verticies[j][0];
                    triangles[i].verticies[j][0] = triangles[i].verticies[j][1];
                    triangles[i].verticies[j][1] = temp;

                    // 0.5 used here to eliminate floating point approximation errors
                    if (triangles[i].normalVec[0] > 0.5) {
                        triangles[i].normalVec[0] = 0;
                        triangles[i].normalVec[1] = -1.0;
                    }

                    if (triangles[i].normalVec[0] < -0.5) {
                        triangles[i].normalVec[0] = 0;
                        triangles[i].normalVec[1] = 1.0;
                    }

                    if (triangles[i].normalVec[1] > 0.5) {
                        triangles[i].normalVec[1] = 0;
                        triangles[i].normalVec[0] = -1.0;
                    }

                    if (triangles[i].normalVec[1] < -0.5) {
                        triangles[i].normalVec[1] = 0;
                        triangles[i].normalVec[0] = 1.0;
                    }

                    break;
            }        
        }
        

    }
}

void rotateObjMulti(char* obj, int numTriangles, int axis, int rotations) {
    for (int i = 0; i < rotations; i++) {
        rotateObj(obj, numTriangles, axis);
    }

}

void adjustObjPosition(char* obj, block_t blockInfo ){

    triangle_t* triangles = (triangle_t* ) obj;

    if(blockInfo.type == BLOCK_TYPE_STAIRS_TOP || BLOCK_TYPE_STAIRS_BOT) {
        for (int i = 0; i < STL_MC_STAIRS_NUM_TRIANGLES; i++) {
            
            if(blockInfo.faces != BLOCK_TYPE_SLAB_TOP) {
                //TODO: rotate
            }

            if(blockInfo.type == BLOCK_TYPE_STAIRS_BOT) {
                rotateObjMulti(obj, STL_MC_STAIRS_NUM_TRIANGLES, 1, ROTATION_AXIS_X);
            }

            for(int j = 0; j < STL_NUM_TRIANGLE_VERTICIES; j++) {
                triangles[i].verticies[j][0] += blockInfo.x * STL_EDGE_LENGTH;
                triangles[i].verticies[j][1] += blockInfo.y * STL_EDGE_LENGTH;
                triangles[i].verticies[j][2] += blockInfo.z * STL_EDGE_LENGTH;

                
            }
        }    
    }
    else if (blockInfo.type == BLOCK_TYPE_SLAB_TOP || BLOCK_TYPE_SLAB_BOT) {
        for (int i = 0; i < STL_MC_SLAB_NUM_TRIANGLES; i++) {
                    for(int j = 0; j < STL_NUM_TRIANGLE_VERTICIES; j++) {
                        triangles[i].verticies[j][0] += blockInfo.x * STL_EDGE_LENGTH;
                        triangles[i].verticies[j][1] += blockInfo.y * STL_EDGE_LENGTH;
                        triangles[i].verticies[j][2] += blockInfo.z * STL_EDGE_LENGTH;

                        if (blockInfo.type == BLOCK_TYPE_SLAB_TOP) {
                            //move slab up half of a block
                            triangles[i].verticies[j][2] += STL_EDGE_LENGTH/2;
                        }
                    }

                    
        }
    }
}

// renders a full stl objects data into the model
int renderObj(block_t * blockInfo, const char* template, int templateSize, FILE* file) {
    char obj[STL_TEMPLATE_MAX_LENGTH] = {0};
    memcpy(&obj[0], &template[0], templateSize);
    adjustObjPosition(obj, *blockInfo);
    fwrite((void*) obj, sizeof(char), templateSize, file);
    return templateSize / sizeof(triangle_t);
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
        if (current->info.type == BLOCK_TYPE_BLOCK) {
            printf("rendering faces for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
            if ((current->info.faces & BLOCK_FACE_PLUS_X) != 0) {
                DEBUG("\trendering plus x face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_PLUS_X_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }

            if ((current->info.faces & BLOCK_FACE_MINUS_X) != 0) {
                DEBUG("\trendering minus x face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_MINUS_X_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }

            if ((current->info.faces & BLOCK_FACE_PLUS_Y) != 0) {
                DEBUG("\trendering plus y face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_PLUS_Y_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }

            if ((current->info.faces & BLOCK_FACE_MINUS_Y) != 0) {
                DEBUG("\trendering minus y face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_MINUS_Y_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }

            if ((current->info.faces & BLOCK_FACE_PLUS_Z) != 0) {
                DEBUG("\trendering plus z face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_PLUS_Z_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }

            if ((current->info.faces & BLOCK_FACE_MINUS_Z) != 0) {
                DEBUG("\trendering minus z face for block %d,%d,%d\n", current->info.x, current->info.y, current->info.z);
                renderFace(current->info, STL_MINUS_Z_FACE_BIN, tempFile);
                faceCount += STL_TRIANGLES_PER_FACE;
            }
        }
        else if (current->info.type == BLOCK_TYPE_SLAB_BOT || current->info.type == BLOCK_TYPE_SLAB_TOP) {
            printf("rendering slab obj at: %d,%d,%d\n", current->info.x, current->info.y, current->info.z);

            faceCount += renderObj(&current->info, STL_MC_SLAB, sizeof(STL_MC_SLAB),tempFile);
        }
        else if (current->info.type == BLOCK_TYPE_STAIRS_BOT || current->info.type == BLOCK_TYPE_STAIRS_TOP) {
            printf("rendering stairs obj at: %d,%d,%d\n", current->info.x, current->info.y, current->info.z);

            faceCount += renderObj(&current->info, STL_MC_STAIRS, sizeof(STL_MC_STAIRS), tempFile);

        }
        else {
            printf("errant block type found: %d, skipping...\n", current->info.faces);
        }

        current = current->next;
 
    }

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
