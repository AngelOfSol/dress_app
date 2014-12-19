#include "polygon.h"
#include <iostream>
#include "math.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/polygon.hpp>
polygon::polygon(void)
{
}


polygon::~polygon(void)
{
}

bool polygon::contains(vec2d vertex) const
{
	// check to see if we have points
	if(this->vertexes.size() > 0)
	{
		// see if we are more than a point
		if(this->vertexes.size() > 1)
		{
			// see if we are more than an edge
			if(this->vertexes.size() > 2)
			{
				int windCount = 0;
				edge ray(vertex, vertex + vec2d(1, 0));

				for(unsigned int i = 0; i < this->vertexes.size(); i++)
				{
					edge side(this->vertexes[i], this->vertexes[(i + 1) % this->vertexes.size()]);

					if(side.contains(vertex))
						return true;

					if(side.offset().y != 0)
					{
						auto intersect = ray.intersect(side);
						if(side.contains(intersect) && intersect.x >= ray.origin().x)
						{
							windCount += (int)(side.offset().y / abs(side.offset().y));
						}
					}
				}
				return windCount != 0;
			} else
			{
				// we're an edge so compare along edginess
				edge thisEdge(this->vertexes[0], this->vertexes[1]);
				return thisEdge.contains(vertex);
			}
		} else
		{
			// we're a vertex so compare as vertices
			return vertex == this->vertexes[0];
		}
	}  else
	{
		return false;
	}
}
std::vector<polygon> polygon::from(const ClipperLib::Paths& paths, base_int maxDenom)
{
	std::vector<polygon> ret;
	for(auto iter = paths.begin(); iter != paths.end(); iter++)
	{
		ret.push_back(polygon());
		auto& polyRef = ret.back();
		for(auto point = iter->begin(); point != iter->end(); point++)
		{
			polyRef.vertexes.push_back(vec2d(int_frac(point->X, maxDenom), int_frac(point->Y, maxDenom)));
		}
	}
	return ret;
}
polygon polygon::createUnion(const std::vector<polygon>& data)
{
	base_int maxDenom = 1;
	for(auto poly : data)
	{
		maxDenom = lcm(maxDenom, poly.highestDenominator());
	}

	maxDenom = std::min(maxDenom, base_int(1000000000L));
	//std::cout << "-----" << std::endl;
	//std::cout << maxDenom << std::endl;
	polygon base = data[0];
		
	for(unsigned int i = 1; i < data.size(); i++)
	{
		ClipperLib::Clipper clipper;
		clipper.AddPath(base.path(maxDenom), ClipperLib::PolyType::ptSubject, true);
		clipper.AddPath(data[i].path(maxDenom), ClipperLib::PolyType::ptClip, true);
		
		ClipperLib::Paths returnedPaths;
		clipper.Execute(ClipperLib::ClipType::ctUnion, returnedPaths, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

		base = polygon::from(returnedPaths, maxDenom)[0];

	}

	return base;
}

ClipperLib::Path polygon::path(base_int denom) const
{
	ClipperLib::Path ret;
	for(auto v : this->vertexes)
	{
		ret.push_back(ClipperLib::IntPoint((ClipperLib::cInt)(v.x.numerator() * (denom / v.x.denominator())), (ClipperLib::cInt)(v.y.numerator() * (denom / v.y.denominator()))));
	}
	return ret;
}
base_int polygon::highestDenominator() const
{
	std::vector<base_int> denominators;

	for(auto v : this->vertexes)
	{
		denominators.push_back(v.x.denominator());
		denominators.push_back(v.y.denominator());
	}
	auto ret = lcm_of(denominators.begin(), denominators.end());
	return ret;
}
std::vector<polygon> polygon::subtract(const polygon& subtrahend) const
{
	ClipperLib::Clipper poly;

	auto maxDenom = lcm(this->highestDenominator(), subtrahend.highestDenominator());
	
	poly.AddPath(this->path(maxDenom), ClipperLib::PolyType::ptSubject, true);
	poly.AddPath(subtrahend.path(maxDenom), ClipperLib::PolyType::ptClip, true);

	ClipperLib::Paths returnedPaths;

	poly.Execute(ClipperLib::ClipType::ctDifference, returnedPaths);
	
	return polygon::from(returnedPaths, maxDenom);
}
polygon polygon::sweep(const std::vector<vec2d>& path) const
{

	std::vector<polygon> subSweeps;
	for(auto start = path.begin(), end = start + 1; end != path.end(); start++, end = start + 1)
	{

		polygon startPoly;
		polygon endPoly;
		for(auto v : this->vertexes)
		{
			startPoly.vertexes.push_back(v + *start);
			endPoly.vertexes.push_back(v + *end);
		}
		
		auto wrapPoint = [this] (int i) { return (i + this->vertexes.size()) % this->vertexes.size(); };

		std::vector<polygon> interiors;
		interiors.push_back(startPoly);
		interiors.push_back(endPoly);

		for(unsigned int i = 0; i < startPoly.vertexes.size(); i++)
		{
			polygon temp;
			
			temp.vertexes.push_back(startPoly.vertexes[i]);
			temp.vertexes.push_back(startPoly.vertexes[wrapPoint(i + 1)]);
			temp.vertexes.push_back(endPoly.vertexes[wrapPoint(i + 1)]);
			temp.vertexes.push_back(endPoly.vertexes[i]);

			interiors.push_back(temp);
		}
		
		subSweeps.push_back(polygon::createUnion(interiors));
	}

	return polygon::createUnion(subSweeps);
}
