#include "AxisMesh.h"


AxisMesh::AxisMesh(void):Mesh(6)
{
}


AxisMesh::~AxisMesh(void)
{
}

void AxisMesh::buildMesh()
{

	this->deleteBuffers();

	this->genBuffers();

	this->bindBuffers();

	// push the x position, and blank yz positions followed by the radius into the data aray

	for(auto iter = this->vertexData_.begin(); iter != this->vertexData_.end(); iter++)
	{
		auto pushOrigin = [this, &iter] () {

			this->arrayData_.push_back(iter->pos.x);
			this->arrayData_.push_back(iter->pos.y);
			this->arrayData_.push_back(iter->pos.z);
		};

		auto pushOffset = [this, &iter] (float x, float y, float z) {

			this->arrayData_.push_back(iter->pos.x + x);
			this->arrayData_.push_back(iter->pos.y + y);
			this->arrayData_.push_back(iter->pos.z + z);
		};

		auto pushColor = [this] (const glm::vec4& color) {
			this->arrayData_.push_back(color.r);
			this->arrayData_.push_back(color.g);
			this->arrayData_.push_back(color.b);
			this->arrayData_.push_back(color.a);
		};
		
		// create an x axis

		pushOrigin();
		pushColor(iter->xAxisColor);
		pushOffset(iter->radius, 0.0f, 0.0f);
		pushColor(iter->xAxisColor);

		// create a y axis

		pushOrigin();
		pushColor(iter->yAxisColor);
		pushOffset(0.0f, iter->radius, 0.0f);
		pushColor(iter->yAxisColor);

		// create a z axis

		pushOrigin();
		pushColor(iter->zAxisColor);
		pushOffset(0.0f, 0.0f, iter->radius);
		pushColor(iter->zAxisColor);


	}
	
	// tell opengl to copy the data

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->arrayData_.size(), this->arrayData_.data(), GL_STATIC_DRAW);

	// let opengl know the first attribute is the position at 3 floats long starting at 0

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

	// let opengl know the second attribute is color at 4 float long starting at the 3rd index

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3*sizeof(float)));
	
	/**
		data array looks like the following:
			x0,y0,z0,r0,g0,b0,a0,x1,y1,z1,r1,g1,b1,a1,....xn,yn,zn,rn,gn,bn,an

	*/
	
	
	// enable the attributes

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	built = true;
}