#pragma once
#include "assert.h"
#include "vec2d.h"
class edge
{
public:
	edge(vec2d = vec2d(), vec2d = vec2d());
	~edge(void);

	auto length() const -> decltype(vec2d().length());
	
	vec2d intersect(edge) const;

	vec2d& origin() { return this->origin_; };
	vec2d origin() const { return this->origin_; };
	vec2d offset() const { return this->end_ - this->origin_; };
	vec2d& end() { return this->end_; };
	vec2d end() const { return this->end_; };

	bool contains(vec2d) const;
private:
	vec2d origin_;
	vec2d end_;
};

