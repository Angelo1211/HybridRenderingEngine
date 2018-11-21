#ifndef MPRIMITIVES_H
#define MPRIMITIVES_H

/*
AUTHOR       : Angel Ortiz (angelo12 AT vt DOT edu)
PROJECT      : Hybrid Rendering Engine 
LICENSE      : This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
DATE	     : 2018-11-21
PURPOSE      : Creation and initialization of basic mesh primitives that are common in rendering.
*/

//All primitives share a very simple interface and only vary by their setup implementation
//and their number of vertices. This class generally should never be called and a new struct should be
//built if you want to define a new primitive type.
struct Primitive{
    Primitive(unsigned int numVertex) : numVertices(numVertex) {};
    void draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0);
    virtual void setup() = 0;
    unsigned int VAO, VBO;
    const unsigned int numVertices;
};

//Mostly used for screen space or render to texture stuff
struct Quad : public Primitive{
    Quad() : Primitive(6) {};
    void draw(const unsigned int readTex1 = 0, const unsigned int readTex2 = 0, const unsigned int readTex3 = 0);
    void setup();
};

//Used in cubemap rendering
struct Cube : public Primitive{
    Cube() : Primitive(36) {};
    void setup();
};

#endif