#include "edge.h"


edge::edge(vec2d origin, vec2d end):origin_(origin), end_(end)
{
}


edge::~edge(void)
{
}

// even though this errors, it works just fine
// ignore it
auto edge::length() const -> decltype(vec2d().length()) 
{
	return this->offset().length();
}

vec2d edge::intersect(edge e) const
{
	// use parameterized calculations to find intersection point
	auto u = (this->origin() - e.origin()).cross(this->offset()) / e.offset().cross(this->offset());
	return e.offset() * u + e.origin();
};

bool edge::contains(vec2d v) const
{
	// find the difference in the origin and the point being tested
	auto diff = v - this->origin();
	// find the scale along offset that the point is
	auto t = diff.length() /  this->offset().length();
	// make sure diff is in the same direction as offset (or at one of the base points)
	// and make sure diff lies somewhere with in offset
	return t == 0 || (diff.normalize() == this->offset().normalize() && t >= 0 && t <= 1);

}