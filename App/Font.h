#pragma once
#include <memory>
#include <string>
#include <vector>
namespace jck
{

	

class Font
{
private:
	std::vector<std::vector<float>> data_;
	friend std::shared_ptr<Font> make_font(std::string, int, int);
public:
	const int width;
	const int height;
	Font(int width, int height, int length);
	~Font(void);
	auto data() -> decltype(Font::data_) const { return this->data_; };
};

/*
	Attempts to construct a Font from a given font path.
	Returns null if an error has occured.
	Otherwise returns a shared_ptr to a Font.
*/
std::shared_ptr<Font> make_font(std::string fontPath, int width, int height);
std::shared_ptr<Font> make_font(std::string fontPath, int size);

}