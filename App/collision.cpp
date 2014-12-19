#include "collision.h"

bool isColliding(glm::vec2 point, glm::vec2 rectangle, glm::vec2 size)
{
	auto dist = rectangle - point;

	dist.x = abs(dist.x);
	dist.y = abs(dist.y);

	return dist.x <= size.x / 2.0f && dist.y <= size.y / 2.0f;
}