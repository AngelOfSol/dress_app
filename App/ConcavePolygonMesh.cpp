#include "ConcavePolygonMesh.h"


ConcavePolygonMesh::ConcavePolygonMesh(void)
{
}


ConcavePolygonMesh::~ConcavePolygonMesh(void)
{
}


void ConcavePolygonMesh::buildMesh(void)
{
	// if the mesh has been built before
	// let opengl know it can clear out previous data
	this->deleteBuffers();
	
	// grab new spots for vertex data

	this->genBuffers();

	this->bindBuffers();
	

	// push the points into the array
	int i = 0;
	for(auto ele : this->vertexData_)
	{
		this->arrayData_.push_back(ele.x);
		this->arrayData_.push_back(ele.y);
		this->arrayData_.push_back((float)i / (float)this->vertexData_.size());
		i++;
	}
	
	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->arrayData_.size(), this->arrayData_.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 2 floats long starting at 0
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(2*sizeof(float)));

	
	/**
		data array looks like the following:
			x0,y0,x1,y1,....xn,yn
	*/
	
	
	// enable the attributes
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	built = true;
}