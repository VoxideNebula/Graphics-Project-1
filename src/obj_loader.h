#ifndef _CSCI441_OBJ_LOADER_H_
#define _CSCI441_OBJ_LOADER_H_

#include <csci441/vector4.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstring>

// This header file adapted from tutorial code found here http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/

template <typename V>
void loadObj(V& coords, const std::string& modelPath) {
    std::vector<unsigned int> uvIndices, normalIndices, vertexIndices;
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

    // Get our data and put it into structures
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
        // if our line is a uv texture coord, store it as a Vector4 in the temp_uvs array
        else if ( strcmp( lineHeader, "vt" ) == 0 ){
            float u, v;
            fscanf(file, "%f %f\n", &u, &v );
            Vector4 textureUV = Vector4(u,v);
            temp_uvs.push_back(textureUV);
        }
        // if our line is a vertex normal, store it as a Vector4 in the temp_normals array
        else if ( strcmp( lineHeader, "vn" ) == 0 ){
            float x, y, z;
            fscanf(file, "%f %f %f\n", &x, &y, &z );
            Vector4 vertexNorm = Vector4(x,y,z);
            temp_normals.push_back(vertexNorm);
        }
        // if our line is a triangle face, store it the vertex indices
        else if ( strcmp( lineHeader, "f" ) == 0 ){
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

    // push all of the data to coords
    for (int i = 0; i < vertexIndices.size(); i++){
        int vertIndex     = vertexIndices.at(i);
        int normalIndex   = normalIndices.at(i);
        Vector4 newVertex = temp_vertices.at(vertIndex-1);
        Vector4 newNormal = temp_normals.at(normalIndex-1);

        // push the vertex values
        coords.push_back(newVertex.x());
        coords.push_back(newVertex.y());
        coords.push_back(newVertex.z());
        // push the color values
        coords.push_back(1.0f);
        coords.push_back(0.0f);
        coords.push_back(0.0f);
        // push the normal values
        coords.push_back(newNormal.x());
        coords.push_back(newNormal.y());
        coords.push_back(newNormal.z());
    }
}


#endif
