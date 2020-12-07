#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Helper.h"
#include "Renderer.h"

using namespace std;

namespace objLoader
{
    float getNext(int* start, string* line)
    {
        while ((*line)[*start] != ' ') { (*start)++; }
        (*start)++;
        return atof(line->c_str() + *start);
    }

    //returns loaded actors names
	std::vector<std::string> LoadObj(string path)
	{
        string line;
        std::vector<std::string> names;

        //loading materials
        vector<Material> materials;
        ifstream file(path + ".mtl");
        if (file.is_open())
        {
            Material mat;
            mat.name = "defaultxyz";

            while (getline(file, line))
            {
                if (line[0] == 'n')
                {
                    if (mat.name != "defaultxyz")
                    {
                        materials.push_back(mat);
                    }
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
            }
            materials.push_back(mat);
            file.close();
        }
        else cout << "Unable to load " << path + ".mtl";

        //loading geometry
        ifstream fileo(path + ".obj");
        if (fileo.is_open())
        {
            Actor act;
            *(act.getName()) = "defaultxyz";
            vector<Vector3> vertices;
            Vector3 vertex = Vector3(0,0,0);
            Triangle tri;
            int xyz = 0;

            while (getline(fileo, line))
            {
                switch (line[0])
                {
                case 'o':
                    if (*(act.getName()) != "defaultxyz")
                    {
                        actors.push_back(act);
                        names.push_back(*(act.getName()));
                    }
                    act.getGeometry()->clear();
                    *(act.getName()) = line.substr(2, line.length() - 1);
                    break;

                case 'v':
                    xyz = 0;
                    vertex.x = getNext(&xyz, &line);
                    vertex.y = getNext(&xyz, &line);
                    vertex.z = getNext(&xyz, &line);
                    vertices.push_back(vertex);
                    //cout << vertex.x << " " << vertex.y << " " << vertex.z << '\n';
                    break;

                case 'f':
                    xyz = 0;
                    vertex.x = getNext(&xyz, &line);
                    vertex.y = getNext(&xyz, &line);
                    vertex.z = getNext(&xyz, &line);

                    tri.v1 = vertices[vertex.y - 1.0f];
                    tri.v2 = vertices[vertex.x - 1.0f];
                    tri.v3 = vertices[vertex.z - 1.0f];
                    

                    act.getGeometry()->push_back(tri);
                    //cout << vertex.x << " " << vertex.y << " " << vertex.z << '\n';
                    break;

                case 'u':
                    //matching the material to the actor
                    std::string name = line.substr(7, line.length() - 1);
                    for (Material mat : materials)
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