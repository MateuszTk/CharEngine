#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Helper.h"
#include "Renderer.h"

#ifdef SDL
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // SDL

using namespace std;

namespace objLoader
{
    Mat loadImage(string& filename)
    {
#ifdef SDL
        Mat image;
        // Standard parameters:
        //    int *x                 -- outputs image width in pixels
        //    int *y                 -- outputs image height in pixels
        //    int *channels_in_file  -- outputs # of image components in image file
        //    int desired_channels   -- if non-zero, # of image components requested in result
        image.data = stbi_load(filename.data(), &(image.cols), &(image.rows), &(image.channels_), 0);
        if(image.data)  image.empty_ = false;
        return image;   
#endif // SDL
      
#ifdef OPENCV
        return imread(filename, IMREAD_COLOR);
#endif // OPENCV
    }

    float getNext(int* start, string* line, char breakpoint = ' ')
    {
        while ((*line)[*start] != breakpoint) { (*start)++; }
        (*start)++;
        return atof(line->c_str() + *start);
    }

    std::string directoryOf(const std::string& fname)
    {
        size_t pos = fname.find_last_of("\\/");
        return (std::string::npos == pos) ? "" : fname.substr(0, pos);
    }

    //returns loaded actors names
	std::vector<std::string> LoadObj(string path, string actorName = "", Vector3 position = Vector3(0, 0, 0), ActorType type = ActorType::Common)
	{

        std::string local = directoryOf( path );

        string line;
        std::vector<std::string> names;

        //loading materials
        vector<Material> l_materials;
        ifstream file(path + ".mtl");
        if (file.is_open())
        {
            Material mat;
            mat.name = "defaultxyz";
            mat.textureId = 0;

            while (getline(file, line))
            {
                if (line[0] == 'n')
                {
                    if (mat.name != "defaultxyz")
                    {
                        l_materials.push_back(mat);
                    }
                    //mat.texture->textureData.release();
                    mat.textureId = 0;
                    mat.name = line.substr(7, line.length() - 1);
                }
                else if(line[0] == 'K')
                {
                    if (line[1] == 'd')
                    {
                        int xyz = 0;
                        mat.color.R = (uchar)(getNext(&xyz, &line) * 255.0f);
                        mat.color.G = (uchar)(getNext(&xyz, &line) * 255.0f);
                        mat.color.B = (uchar)(getNext(&xyz, &line) * 255.0f);

                        //cout << (int)mat.color.R << " " << (int)mat.color.G << " " << (int)mat.color.B << '\n';
                    }
                }
                else if (line[0] == 'd')
                {
                    int start = 0;
                    mat.transparency = 1.0f - getNext(&start, &line);
                }
                else if (line[0] == 'm' && line[5] =='d')
                {
                    std::string texturePath = line.substr(7);

                    //checking if texture was not loaded before
                    bool t = true;
                    auto end = std::end(textures);
                    for (auto tex = std::begin(textures); tex != end; ++tex)
                    {
                        if (tex->texturePath == texturePath)
                        {
                            mat.textureId = tex - textures.begin();
                            t = false;
                            break;
                        }
                    }

                    if (t)
                    {

                        // very hacky image loading :concern:
                        bool success = true;
                        
                        // try as a absolute path
                        Mat texture = loadImage(texturePath);
                        if (!texture.data)
                        {
                            // try as a relative path
                            std::string localTexturePath = local + "/" + texturePath;

                            texture = loadImage(localTexturePath);
                            if( !texture.data ) 
                            {
            
                                // print some useful info
                                std::cout << "Could not open or find the image!" << std::endl;
                                std::cout << " * Tried: " << texturePath << std::endl;
                                std::cout << " * Tried: " << localTexturePath << std::endl;
                                success = false;
                            } 
                        }

                        if( success )
                        {
                            Texture tx;
                            tx.textureData = texture;
                            tx.texturePath = texturePath;
                            textures.push_back(tx);
                            mat.textureId = textures.size() - 1;
                        }
                    }
                }
            }
            l_materials.push_back(mat);
            file.close();
        }
        else cout << "Unable to load " << path + ".mtl";

        //loading geometry
        ifstream fileo(path + ".obj");
        if (fileo.is_open())
        {
            Actor act;
            *act.getActorType() = type;
            *(act.getName()) = "defaultxyz";
            Vector3 vertex = Vector3(0,0,0);
            pTriangle tri;
            Vertex vert;
            vector<Vertex>* vertices;
            vector<Vector2> uvs;
            int xyz = 0;
            int vertCount = 0;

            while (getline(fileo, line))
            {
                switch (line[0])
                {
                case 'o':
                    *(act.getPosition()) = position;
                    if (*(act.getName()) != "defaultxyz")
                    {
                        actors.push_back(act);
                        names.push_back(*(act.getName()));
                    }
                    act.getTriangles()->clear();
                    vertCount += act.getVertices()->size();
                    act.getVertices()->clear();
                    *(act.getName()) = actorName + "_" + line.substr(2, line.length() - 1);
                    break;

                case 'v':
                    if (line[1] == ' ')
                    {
                        xyz = 0;
                        vert.raw.x = getNext(&xyz, &line);
                        vert.raw.y = getNext(&xyz, &line);
                        vert.raw.z = getNext(&xyz, &line);
                        vert.transformed.x = 0;
                        vert.transformed.y = 0;
                        vert.transformed.z = 0;
                        act.getVertices()->push_back(vert);
                        //cout << vertex.x << " " << vertex.y << " " << vertex.z << '\n';
                    }
                    else if(line[1] == 't')
                    {
                        xyz = 0;
                        Vector2 v = Vector2(0, 0);
                        v.x = getNext(&xyz, &line);
                        v.y = 1.0f - getNext(&xyz, &line);
                        uvs.push_back(v);
                    }
                    break;

                case 'f':
                    xyz = 0;
                    vertex.x = getNext(&xyz, &line);
                    tri.v2 = vertex.x - 1 - vertCount;
                    vertex.x = getNext(&xyz, &line, '/');
                    tri.uv2 = uvs[vertex.x - 1];

                    vertex.y = getNext(&xyz, &line);
                    tri.v1 = vertex.y - 1 - vertCount;
                    vertex.y = getNext(&xyz, &line, '/');
                    tri.uv1 = uvs[vertex.y - 1];

                    vertex.z = getNext(&xyz, &line);
                    tri.v3 = vertex.z - 1 - vertCount;
                    vertex.z = getNext(&xyz, &line, '/');
                    tri.uv3 = uvs[vertex.z - 1];
                    

                    act.getTriangles()->push_back(tri);
                    //cout << vertex.x << " " << vertex.y << " " << vertex.z << '\n';
                    break;

                case 'u':
                    //matching the material to the actor
                    std::string name = line.substr(7, line.length() - 1);
                    for (Material mat : l_materials)
                    {
                        if (mat.name == name)
                        {
                            act.getMaterial()->name = name;
                            *(act.getMaterial()) = mat;
                        }
                    }
                    break;
                }
            }
            actors.push_back(act);
            names.push_back(*(act.getName()));
            fileo.close();
        }
        else cout << "Unable to load " << path + ".obj";

        return names;
	}
}
