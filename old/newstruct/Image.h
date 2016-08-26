#pragma once

#include "ResourceManager.h"
#include "EngineUtil.h"
#include "FreeImage.h"

class Image{
public:
	string name;//file name
	string filepath;

	enum ImageFormat
	{
		IM_JPEG,
		IM_PNG,
		IM_TGA,
		IM_BMP,
		IM_HDR,
		IM_UNKNOWN
	};

	enum ImagePixelFormat
	{
		UNSIGNED_BYTE,
		UNSIGNED_SHORT,
		UNSIGNED_INT,
		UNSIGNED_INT_24_8,
		UNSIGNED_CHAR_8_8_8,
		UNSIGNED_CHAR_8_8_8_8,
		UNSIGNED_CHAR_10_10_10_2,
		FLOAT_32,
		FLOAT_32_32_32,
		FLOAT_32_32_32_32,
		FLOAT_16_16_16_16,
		FLOAT_16_16_16,
		UNSIGNED_INT_16_16_16_16
	};
	ImageFormat mImageformat = IM_UNKNOWN;

	ImagePixelFormat mPixelformat;

	unsigned int width=0, height=0;
	unsigned int bpp=0;
	void * pdata=NULL;

	bool bgra = false;
	Image* LoadImage(string filepath);
	Image* CreateImageFromData(ImagePixelFormat format,int width,int height,void* data,string name="");
	bool m_valid = false;
	void test();
private:
	void readDataBytes(FIBITMAP * fib, void*& dst);
	ImageFormat getImageFormat(FREE_IMAGE_FORMAT fm);
	void ConvertBetweenBGRandRGB(unsigned char* input, int pixel_width,
		int pixel_height,int bytes_per_pixel);

};






class ImageManager :public as_ResourceManager<Image*>{
public:
	static ImageManager& getInstance()
	{
		static ImageManager    instance;
		return instance;
	}

	Image* CreateImageFromFile(string filepath);
	vector<Image*> CreateImageListFromFiles(vector<string> paths);
private:
	ImageManager(){
	}

	ImageManager(ImageManager const&);              // Don't Implement.
	void operator=(ImageManager const&); // Don't implement
};