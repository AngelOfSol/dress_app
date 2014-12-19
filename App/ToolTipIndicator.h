#pragma once
#include "Mesh.h"
#include "glm_includes.h"

struct ttVertex
{
	glm::vec3 pos;
};

class ToolTipIndicator
	: public Mesh<ttVertex>
{
public:
	ToolTipIndicator(void);
	~ToolTipIndicator(void);
	void buildMesh() override;

};

