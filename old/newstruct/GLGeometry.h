#pragma once
#include "Geometry.h"
#include "OPENGL_Include.h"

struct GL_GPUVertexData{
	GLuint vao = -1;
	vector<GLuint> vbo;
	GLuint elementbuffer = -1;
	int indices_size = 0;
	GL_GPUVertexData(){
		vbo = vector<GLuint>(10, -1);
	}
};

class GLGeometry :public Geometry{
public:
	GLGeometry(){}
	//interface
	virtual void Create(as_Geometry* geo, as_InputLayout* inputlayout);
	virtual void Update(as_Geometry* geo){}
	virtual bool IsValid(){ return m_bResourceValid; }

	//help func
	void Draw();
private:
	GL_GPUVertexData gldata;
};

class GLGeometryFactory:public GeometryFactory{
public:
	virtual Geometry * create();
	virtual Geometry * create(as_Geometry* geo, as_InputLayout* inputlayout);
};