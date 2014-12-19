#include "CylindricalMesh.h"


CylindricalMesh::CylindricalMesh(void)
{
}


CylindricalMesh::~CylindricalMesh(void)
{
}

void CylindricalMesh::buildMesh(void)
{
	// if the mesh has been built before
	// let opengl know it can clear out previous data
	this->deleteBuffers();
	
	// grab new spots for vertex data

	this->genBuffers();

	this->bindBuffers();
	
	// sort the vertices by xOrder, going from least to greatest
	// note: stable sort mantains insertion order, allowing for vertices placed on the same x position
	//		to be remembered in insertion order.
	/*
	std::stable_sort(this->vertexData_.begin(), this->vertexData_.end(), [] (const cVertex& lhs, const cVertex& rhs)
		{
			return lhs.pos < rhs.pos;
		}
	);*/

	// push the x position, and blank yz positions followed by the radius into the data aray

	for(auto iter = this->vertexData_.begin(); iter != this->vertexData_.end(); iter++)
	{
		this->arrayData_.push_back(iter->pos);
		this->arrayData_.push_back(0.0f);
		this->arrayData_.push_back(0.0f);
		this->arrayData_.push_back(iter->radius);


	}
	
	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->arrayData_.size(), this->arrayData_.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 3 floats long starting at 0

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// let opengl know the second attribute is radius at 1 float long starting at the 3rd index

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3*sizeof(float)));
	
	/**
		data array looks like the following:
			x0,y0,z0,r0,x1,y1,z1,r1,....xn,yn,zn,rn
	*/
	
	
	// enable the attributes

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	built = true;
}