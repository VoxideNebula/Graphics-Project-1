#ifndef _CSCI441_OBJ_LOADER_H_
#define _CSCI441_OBJ_LOADER_H_

#include <csci441/vector4.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstring>

// Adapted from tutorial code found here http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

template <typename V, typename I>
void loadObj(V& coords, I& indices, const std::string& modelPath) {
    std::vector<unsiged int> vertexIndices, uvIndices, normalIndices;
    std::vector<Vector4> temp_vertices;
    std::vector<Vector4> temp_uvs;
    std::vector<Vector4> temp_normals;

    // Convert the std::string to a char array. coded with help from https://www.techiedelight.com/convert-string-char-array-cpp/
    char path[modelPath.size() + 1];
    strcpy(path, modelPath.c_str());

    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
    }

    while(1){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF){
            break; // EOF = End Of File. Quit the loop.
        }
        // if our line is vertex, store it as a Vector4 in the temp_vertices array
        if ( strcmp( lineHeader, "v" ) == 0 ){
            float x, y, z;
            fscanf(file, "%f %f %f\n", &x, &y, &z );
            Vector4 vertex = Vector4(x,y,z);
            temp_vertices.push_back(vertex);
        }
        else if ( strcmp( lineHeader, "vt" ) == 0 ){
            float u, v;
            fscanf(file, "%f %f\n", &u, &v );
            Vector4 textureUV = Vector4(u,v);
            temp_uvs.push_back(textureUV);
        }
        // if our line is a vertex normal, store it as a Vector4 in the temp_vertices array
        else if ( strcmp( lineHeader, "vn" ) == 0 ){
            float x, y, z;
            fscanf(file, "%f %f %f\n", &x, &y, &z );
            Vector4 vertexNorm = Vector4(x,y,z);
            temp_vertices.push_back(vertexNorm);
        }
        // if our line is vertex, store it as a Vector4 in the temp_vertices array
        else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("Obj can't be read by our simple parser : ( Try exporting with other options\n");
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }
}


#endif
