#pragma once

#include "Mesh.h"
#include "typedefs.h"
#include "Vec2D.h"
#include <algorithm>

struct cVertex
{
	template <class InputType1, class InputType2>
	cVertex(InputType1 position, InputType2 rad):pos(position), radius(rad)
	{
	};
	cVertex(vec2d v):pos(v.x), radius(v.y)
	{
	};

	int_frac pos;
	int_frac radius;

	bool operator ==(const cVertex& rhs)
	{
		return rhs.pos == this->pos && rhs.radius == this->radius;
	}
};

class CylindricalMesh : public Mesh<cVertex>
{
public:
	CylindricalMesh(void);
	~CylindricalMesh(void);

	void buildMesh();

};

