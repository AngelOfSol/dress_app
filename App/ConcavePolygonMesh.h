#pragma once
#include "Mesh.h"
#include "Vec2D.h"
class ConcavePolygonMesh : public Mesh<vec2d>
{
public:
	ConcavePolygonMesh(void);
	~ConcavePolygonMesh(void);

	void buildMesh();
};

