#include "halfedge.h"
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <GL/glut.h>
#include <pthread.h>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    deleteHalfEdge();
}

void Mesh::generateHalfEdge(char* path)
{
    std::ifstream infile(path);
    
    if(!infile.is_open())
        printf("ERROR: File not open\n");
    
    std::string line;


    std::vector<v3> tempV;
    std::vector<v2> tempUV;
    std::vector<v3> tempN;
    std::vector<std::string> tempF;

    int step = 0;
    
    while(std::getline(infile, line))
    {
        if(line.size() < 1);
        else
        {

            std::stringstream sst(line.c_str());

            std::string split;
        
            std::vector<std::string> splits;
                    
            if(line.at(0) == 'v' && line.at(1) == ' ')
            {
            
                while(std::getline(sst, split, ' '))
                {
                    splits.push_back(split);
                }

                v3 p(std::stof(splits[1]), std::stof(splits[2]), std::stof(splits[3]));

                tempV.push_back(p);
            }
            if(line.at(0) == 'v' && line.at(1) == 't')
            {
            
                while(std::getline(sst, split, ' '))
                {
                    splits.push_back(split);
                }

                v2 p(std::stof(splits[1]), std::stof(splits[2]));

                tempUV.push_back(p);
            }
            if(line.at(0) == 'v' && line.at(1) == 'n')
            {
            
                while(std::getline(sst, split, ' '))
                {
                    splits.push_back(split);
                }

                v3 p(std::stof(splits[1]), std::stof(splits[2]), std::stof(splits[3]));

                tempN.push_back(p);
            }
            else if(line.at(0) == 'f')
            {

                step = 0;
                while(std::getline(sst, split, ' '))
                {
                    if(split.at(0) != 'f')
                    {
                        tempF.push_back(split);

                        step++;
                    }
                }    
            }
        }
    }
    


    int vNm = tempV.size();
    vertPool.pre_allocate_elements(vNm);

    for(int i = 0; i < vNm; i++)
    {
        *(vertPool.new_element()) = tempV[i];
    }
  
    int edgeCount = tempF.size();
    int faceCount = edgeCount/step;
  
    //Interpret vertices, uvs, normals and faces as Half-edges.

    facePool.pre_allocate_elements(faceCount);

    for(int i = 0; i < faceCount; i++)
        facePool.new_element();
    
    edgePool.pre_allocate_elements(edgeCount);
    
    for(int i = 0; i < edgeCount; i++)
        edgePool.new_element();
    
    for(int i = 0; i < faceCount*step; i+=step)
    {
        for(int j = 0; j < step - 1; j++)
        {
            edgePool[i + j]->next = edgePool[i + j + 1];
        }
        edgePool[i + step - 1]->next = edgePool[i];
    }
           
    for(int i = 0; i < faceCount; i++)
    {
        int attr = 0;

        int *vIndex = new int[step];
        int *uvIndex = new int[step];
        int *nIndex = new int[step];

        for(int j = 0; j < step; j++)
        {
            attr = 0;
            std::stringstream f(tempF[i*step+j]);
            std::string split;

	  
            std::vector<std::string> splits;

            while(std::getline(f, split, '/'))
            {
                splits.push_back(split);
            }

            switch(splits.size())
            {
            case(3):
                nIndex[j] = std::stoi(splits[2]) - 1;
                attr++;
            case(2):
                uvIndex[j] = std::stoi(splits[1]) - 1;
                attr++;
            case(1):
                vIndex[j] = std::stoi(splits[0]) - 1;
                break;
            default:
                break;
            }
            
        }

        facePool[i]->edge = edgePool[step*i];
        
        if(attr == 2)
        {
            facePool[i]->edge->uv = tempUV[uvIndex[0]];
            facePool[i]->edge->norm = tempN[nIndex[0]];
        }
        
        HEEdge* temp = facePool[i]->edge;
        temp->vert = vertPool[vIndex[0]];

        for(int j = 1; j < step; j++)
        {
            temp = temp->next;
            temp->vert = vertPool[vIndex[j]];
            if(attr == 2)
            {
                temp->uv = tempUV[uvIndex[j]];
                temp->norm = tempN[nIndex[j]];
            }
        }

        delete[] vIndex;
        delete[] uvIndex;
        delete[] nIndex;

    }
    
    for(int i = 0; i < faceCount*step; i++)
    {

        edgePool[i]->vert->edge = edgePool[i];

        for(int j = 0; j < faceCount*step; j++)
        {
            if(edgePool[i]->vert == edgePool[j]->next->vert && edgePool[j]->vert == edgePool[i]->next->vert)
                edgePool[i]->pair = edgePool[j];
        }
    }

    for(int i = 0; i < faceCount; i++)
    {
        HEEdge *temp = facePool[i]->edge->next;

        while(true)
        {
            temp->face = facePool[i];

            if(temp == facePool[i]->edge)
                break;
            temp = temp->next;
        }
    }
    
    fCount = faceCount;
    eStep = step;
    vCount = vNm;
  
}

/********

         SUBDIVIDE FUNCTIONS
         
********/

HEVert* Mesh::facePoint(HEFace *face)
{

    if(face->avg != nullptr)
        return face->avg;

    HEVert *fp = vertPool.new_element();
    
    HEEdge* temp = face->edge->next;

    fp->pos = v3(0, 0, 0);
    
    while(true)
    {

        fp->pos = fp->pos + temp->vert->pos;

        if(temp == face->edge)
            break;
        temp = temp->next;
    }

    fp->pos = fp->pos * 0.25f;
    
    face->avg = fp;
    return face->avg;
}

HEVert* Mesh::edgePoint(HEEdge *edge)
{

    if(edge->ePt != nullptr)
        return edge->ePt;

    if(edge->pair->ePt != nullptr)
        return edge->pair->ePt;

    HEVert *newV = vertPool.new_element();
    
    HEVert *fp1 = facePoint(edge->face);
    HEVert *fp2 = facePoint(edge->pair->face);
    
    newV->pos = (edge->vert->pos + edge->next->vert->pos + fp1->pos + fp2->pos) * 0.25f;

    edge->ePt = newV;
    
    return edge->ePt;
}

v3 Mesh::avgPoint(HEEdge *edge)
{
    v3 ap;
    
    ap = (edge->vert->pos + edge->next->vert->pos) * 0.5f;

    return ap;
}

void Mesh::splitEdge(HEEdge *edge)
{

    if(edge->split)
        return;

    edge->split = true;
    edge->pair->split = true;
    
    HEEdge *newEdge = edgePool.new_element();
    HEEdge *newPairEdge = edgePool.new_element();
    
    newEdge->split = true;
    newPairEdge->split = true;

    newEdge->uv = (edge->uv + edge->next->uv) * 0.5f;
    newPairEdge->uv = (edge->pair->uv + edge->pair->next->uv) * 0.5f;
    
    HEVert *v = edgePoint(edge);
    
    newEdge->next = edge->next;
    newEdge->pair = edge->pair;
    newEdge->vert = v;
    newEdge->vert->edge = newEdge;
    newEdge->face = edge->face;
    
    newPairEdge->next = edge->pair->next;
    newPairEdge->pair = edge->pair->pair;
    newPairEdge->vert = v;
    newPairEdge->vert->edge = newPairEdge;
    newPairEdge->face = edge->pair->face;
    
    edge->pair->pair = newEdge;
    edge->pair->next = newPairEdge;
    edge->pair = newPairEdge;
    edge->next = newEdge;

    edge->ePt = nullptr;
    newEdge->pair->ePt = nullptr;


}

void Mesh::splitEdges(HEEdge *edge)
{

    HEEdge *temp;
    if(edge->next->split)
        temp = edge->next->next;
    else
        temp = edge->next;

    while(true)
    {
        splitEdge(temp);

        if(temp == edge)
            break;

        while(temp != edge && temp->split)
            temp = temp->next;

    }
}

v3 Mesh::newVertex(HEVert *vert)
{
    
    HEEdge* temp = vert->edge->pair->next;

    int n = 0;

    v3 avgFace(0, 0, 0);
    v3 avgAvg(0, 0, 0);
    
    while(true)
    {
        n++;
        avgFace = avgFace + edgePoint(temp)->pos;
        avgAvg = avgAvg + avgPoint(temp);

        if(temp == vert->edge)
            break;
        temp = temp->pair->next;
    }

    float ndiv = 1.f/(float)n;
    
    avgFace = avgFace * ndiv;
    avgAvg = avgAvg * ndiv;

    v3 res;
    
    res = (avgFace * ndiv) + ((avgAvg*2) * ndiv) + (vert->pos*(n - 3) * ndiv);
    
    return res;
}

void Mesh::subdivide()
{

    vertPool.pre_allocate_elements(fCount*3);
    edgePool.pre_allocate_elements(fCount*12);
    facePool.pre_allocate_elements(fCount*3);
    
    for(int i = 0; i < fCount; i++)
    {
        facePoint(facePool[i]);
    }

    v3 *vertL = new v3[vCount];
    
    for(int i = 0; i < vCount; i++)
    {
        vertL[i] = newVertex(vertPool[i]);
    }

    
    
    for(int i = 0; i < fCount; i++)
    {
        
        HEVert *fb = facePoint(facePool[i]);

        HEFace *f1 = facePool[i];
        HEFace *f2 = facePool.new_element();
        HEFace *f3 = facePool.new_element();
        HEFace *f4 = facePool.new_element();

        if(i != fCount - 1)
            splitEdges(facePool[i]->edge);

        HEEdge *e11 = facePool[i]->edge->next;
        HEEdge *e12 = e11->next;

        HEEdge *e13 = edgePool.new_element();
        HEEdge *e14 = edgePool.new_element();
        
        f1->edge = e11;

        e13->next = e14;
        e14->next = e11;

        e11->face = f1;
        e12->face = f1;
        e13->face = f1;
        e14->face = f1;
        
        e13->vert = e12->next->vert;
        e14->vert = fb;

        e11->vert->edge = e11;
        e12->vert->edge = e12;
        e13->vert->edge = e13;
        e14->vert->edge = e14;

        HEEdge *e21 = e12->next;
        HEEdge *e22 = e21->next;

        HEEdge *e23 = edgePool.new_element();
        HEEdge *e24 = edgePool.new_element();

        e24->pair = e13;
        e13->pair = e24;
        
        f2->edge = e21;

        e23->next = e24;
        e24->next = e21;

        e21->face = f2;
        e22->face = f2;
        e23->face = f2;
        e24->face = f2;
        
        e23->vert = e22->next->vert;
        e24->vert = fb;
        
        e21->vert->edge = e21;
        e22->vert->edge = e22;
        e23->vert->edge = e23;
        e24->vert->edge = e24;

        HEEdge *e31 = e22->next;
        HEEdge *e32 = e31->next;
        
        HEEdge *e33 = edgePool.new_element();
        HEEdge *e34 = edgePool.new_element();
        
        e34->pair = e23;
        e23->pair = e34;
        
        f3->edge = e31;

        e33->next = e34;
        e34->next = e31;
        
        e31->face = f3;
        e32->face = f3;
        e33->face = f3;
        e34->face = f3;

        e33->vert = e32->next->vert;
        e34->vert = fb;

        e31->vert->edge = e31;
        e32->vert->edge = e32;
        e33->vert->edge = e33;
        e34->vert->edge = e34;
        
        HEEdge *e41 = e32->next;
        HEEdge *e42 = e41->next;
        
        HEEdge *e43 = edgePool.new_element();
        HEEdge *e44 = edgePool.new_element();
        
        e43->pair = e14;
        e14->pair = e43;

        e44->pair = e33;
        e33->pair = e44;
        
        f4->edge = e41;

        e43->next = e44;
        e44->next = e41;

        e41->face = f4;
        e42->face = f4;
        e43->face = f4;
        e44->face = f4;
        
        e43->vert = e42->next->vert;
        e44->vert = fb;

        e41->vert->edge = e41;
        e42->vert->edge = e42;
        e43->vert->edge = e43;
        e44->vert->edge = e44;

        
        v2 fuv;

        fuv = (e12->uv + e22->uv + e32->uv + e42->uv) * 0.25;
        
        e13->uv = e12->next->uv;
        e23->uv = e22->next->uv;
        e33->uv = e32->next->uv;
        e43->uv = e42->next->uv;
        
        e14->uv = fuv;
        e24->uv = fuv;
        e34->uv = fuv;
        e44->uv = fuv;
        
        e12->next = e13;
        e22->next = e23;
        e32->next = e33;
        e42->next = e43;

        e12->split = false;
        e22->split = false;
        e32->split = false;
        e42->split = false;

        e11->split = false;
        e21->split = false;
        e31->split = false;
        e41->split = false;

        v3 normal1 = (e14->vert->pos - e11->vert->pos) * (e14->vert->pos - e13->vert->pos);
        v3 normal2 = (e24->vert->pos - e21->vert->pos) * (e24->vert->pos - e23->vert->pos);
        v3 normal3 = (e34->vert->pos - e31->vert->pos) * (e34->vert->pos - e33->vert->pos);
        v3 normal4 = (e44->vert->pos - e41->vert->pos) * (e44->vert->pos - e43->vert->pos);

        float l1 = sqrt(normal1.x + normal1.y + normal1.z);
        normal1 = normal1 / l1;
        float l2 = sqrt(normal2.x + normal2.y + normal2.z);
        normal2 = normal2 / l2;
        float l3 = sqrt(normal3.x + normal3.y + normal3.z);
        normal3 = normal3 / l3;
        float l4 = sqrt(normal4.x + normal4.y + normal4.z);
        normal4 = normal4 / l4;

        e11->norm = normal1;
        e12->norm = normal1;
        e13->norm = normal1;
        e14->norm = normal1;

        e21->norm = normal2;
        e22->norm = normal2;
        e23->norm = normal2;
        e24->norm = normal2;
        
        e31->norm = normal3;
        e32->norm = normal3;
        e33->norm = normal3;
        e34->norm = normal3;
        
        e41->norm = normal4;
        e42->norm = normal4;
        e43->norm = normal4;
        e44->norm = normal4;
        
        facePool[i]->avg = nullptr;
    }

    for(int i = 0; i < vCount; i++)
    {
        vertPool[i]->pos = vertL[i];
    }

    delete[] vertL;
    
    vCount = vCount + fCount*3;
    fCount = fCount * eStep;
    
}

/************************************************/

v3* Mesh::getVB()
{
    v3* VB = new v3[fCount*eStep];
    for(int i = 0; i < fCount; i++)
    {

        HEEdge *temp = facePool[i]->edge;
        for(int j = 0; j < eStep; j++)
        {
            VB[i*eStep + j] = temp->vert->pos;
            
            temp = temp->next;
        }
    }
    return VB;
}

v2* Mesh::getUV()
{
    v2* UV = new v2[fCount*eStep];
    for(int i = 0; i < fCount; i++)
    {

        HEEdge *temp = facePool[i]->edge;
        for(int j = 0; j < eStep; j++)
        {
            UV[i*eStep + j] = temp->uv;
 
            temp = temp->next;
        }
    }
    return UV;
}

v3* Mesh::getNormals()
{
    v3* N = new v3[fCount*eStep];
    for(int i = 0; i < fCount; i++)
    {

        HEEdge *temp = facePool[i]->edge;
        for(int j = 0; j < eStep; j++)
        {
            N[i*eStep + j] = temp->norm;
            temp = temp->next;
        }
    }
    return N;
}

void Mesh::deleteHalfEdge()
{
}
