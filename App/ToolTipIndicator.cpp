#include "ToolTipIndicator.h"


ToolTipIndicator::ToolTipIndicator(void)
{
}


ToolTipIndicator::~ToolTipIndicator(void)
{
}

void ToolTipIndicator::buildMesh(void)
{
	// if the mesh has been built before
	// let opengl know it can clear out previous data
	this->deleteBuffers();
	
	// grab new spots for vertex data

	this->genBuffers();

	this->bindBuffers();
	

	// push the points into the array
	for(auto ele : this->vertexData_)
	{
		this->arrayData_.push_back(ele.pos.x);
		this->arrayData_.push_back(ele.pos.y);
		this->arrayData_.push_back(ele.pos.z);
	}
	
	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->arrayData_.size(), this->arrayData_.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 2 floats long starting at 0
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);

	// enable the attributes
	
	glEnableVertexAttribArray(0);

	built = true;
}
