#pragma once
#include "glm_includes.h"
#include "mesh.h"
struct aVertex
{
	aVertex(glm::vec3 pos, glm::vec4 xAxisColor, glm::vec4 yAxisColor, glm::vec4 zAxisColor, float radius):
		pos(pos), 
		xAxisColor(xAxisColor), 
		yAxisColor(yAxisColor), 
		zAxisColor(zAxisColor), 
		radius(radius)
	{
	};

	glm::vec3 pos;
	glm::vec4 zAxisColor;
	glm::vec4 xAxisColor;
	glm::vec4 yAxisColor;
	float radius;
};
class AxisMesh :
	public Mesh<aVertex>
{
public:
	AxisMesh(void);
	~AxisMesh(void);

	virtual void buildMesh();
};

