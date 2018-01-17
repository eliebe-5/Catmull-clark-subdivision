struct HEVert;
struct HEFace;
#include <vector>
#include <memorypool.h>
#include <string.h>

struct v2
{
    float x;
    float y;

    inline v2(){};

    inline v2(float p1, float p2)
    {
        x = p1;
        y = p2;
    }

       
    inline v2(const v2& a)
    {
        memcpy(&x, &(a.x), sizeof(float)*2);
    }
    
    inline bool operator==(const v2& one) const
    {
        if(x == one.x && y == one.y)
            return true;
        return false;
    }

    inline v2 operator+(const v2& one) const
    {
        return v2(this->x + one.x, this->y + one.y);
    }
    inline v2 operator*(const float& mult) const
    {
        return v2(this->x * mult, this->y * mult);
    }
};

struct v3
{
    float x;
    float y;
    float z;

    inline v3(){};

    inline v3(float p1, float p2, float p3)
    {
        x = p1;
        y = p2;
        z = p3;
    }
    
    inline v3(const v3& a)
    {
        memcpy(&x, &(a.x), sizeof(float)*3);
    }

    friend v3 operator+(const v3&, const v3&);

    friend v3 operator-(const v3&, const v3&);
    
    /*inline v3 operator-(const v3& one) const
    {
        return v3(this->x - one.x, this->y - one.y, this->z - one.z);
    }*/
    
    inline v3 operator*(const float& mult) const
    {
        return v3(this->x * mult, this->y * mult, this->z * mult);
    }
    
    inline v3 operator*(const v3& cross) const
    {
        v3 r(
            this->y*cross.z - cross.y*this->z,
            this->z*cross.x - cross.z*this->x,
            this->x*cross.y - cross.x*this->y
            );
        return r;
    }

    inline v3 operator/(const float& div) const
    {
        float divm = 1/div;
        v3(this->x *divm, this->y *divm, this->z *divm);
    }

    inline float operator[](const int& i) const
    {
        if(i == 0)
            return x;
        if(i == 1)
            return y;
        return z;
    }
  
};

inline v3 operator+(const v3& one, const v3& two)
{
    return v3(two.x + one.x, two.y + one.y, two.z + one.z);
}

inline v3 operator-(const v3& one, const v3& two)
{
    return v3(one.x - two.x, one.y - two.y, one.z - two.z);
}


struct HEEdge
{
    HEVert* vert; //Vertex at the end of the half-edge
    HEEdge* pair; //Oppositely oriented adjacent half-edge
    HEFace* face; //Face the half-edge borders
    HEEdge* next; //next half-edge around the face
    HEVert *ePt;
    v2 uv;
    v3 norm;

    bool split;
    
    HEEdge()
    {
        split = false;
        vert = nullptr;
        pair = nullptr;
        face = nullptr;
        next = nullptr;
        ePt = nullptr;
    };
};

struct HEVert
{
    v3 pos;

    HEVert()
    {
        edge = 0;
        vNew = 0;
    };
    HEVert(v3 v)
    {
        pos = v;
        edge = nullptr;
        vNew = nullptr;
    };

    HEEdge* edge; //One of the half-edges emantating from the vertex
    HEVert *vNew;

    //bool operator==(HEVert& one)
    //{
    //    if(one.pos == pos)
    //        return true;
    //    return false;
    //};

};

struct HEFace
{
    HEEdge* edge; //One of the half-edges bordering the face

    HEVert *avg;
    
    HEFace()
    {
        edge = nullptr;
        avg = nullptr;
    };
};

class Mesh
{
public://private:
    Memorypool<HEEdge> edgePool;
    Memorypool<HEVert> vertPool;
    Memorypool<HEFace> facePool;
    int fCount;
    int eStep;
    int vCount;
//public:
    Mesh();
    ~Mesh();
    void generateHalfEdge(char*);
    v3* getVB();
    v2* getUV();
    v3* getNormals();
    int VBLen(){ return fCount*eStep; };
    int ePerF(){ return eStep; };

    HEVert* facePoint(HEFace *face);
    HEVert* edgePoint(HEEdge *edge);
    v3 avgPoint(HEEdge *edge);
    void splitEdge(HEEdge *edge);
    void splitEdges(HEEdge *edge);
    v3 newVertex(HEVert *vert);
    void subdivide();
    void fuckmeup();
    void deleteHalfEdge();
};
