#include "Image.h"




Image* Image::LoadImage(string _filepath)
{
	string fullpath = FileUtil::getSceneFileValidPath(_filepath);
	if (fullpath.empty()) return NULL;
	string purefilename = FileUtil::getPureFileName(fullpath);
	name = purefilename;
	this->filepath = fullpath;

	FREE_IMAGE_FORMAT fileFormat = FIF_UNKNOWN;
	fileFormat = FreeImage_GetFileType(fullpath.c_str());
	if (fileFormat == FIF_UNKNOWN)
	{
		fileFormat = FreeImage_GetFIFFromFilename(fullpath.c_str());
	}
	// make sure image can be reading.
	assert(FreeImage_FIFSupportsReading(fileFormat));
	mImageformat=getImageFormat(fileFormat);
	switch (fileFormat){
	case FIF_BMP:
	case FIF_JPEG:
	case FIF_PNG:
	case FIF_TARGA:
	case FIF_HDR:
	{
					FIBITMAP *fib = FreeImage_Load(fileFormat, _filepath.c_str());
					assert(fib && " FreeImage_Load function failed.");
					width = FreeImage_GetWidth(fib);
					height = FreeImage_GetHeight(fib);
					FREE_IMAGE_TYPE imagetype = FreeImage_GetImageType(fib);
					FREE_IMAGE_COLOR_TYPE colortype = FreeImage_GetColorType(fib);
					bpp = FreeImage_GetBPP(fib);
					switch (imagetype)
					{
					case FIT_UNKNOWN:
					case FIT_COMPLEX:
					case FIT_UINT32:
					case FIT_INT32:
					case FIT_DOUBLE:
					case FIT_UINT16:
					case FIT_INT16:
					case FIT_FLOAT:
					default:
						assert(false && "image type not support.");
						return NULL;
						break;
					case  FIT_RGBF:{
									   switch (bpp)
									   {
									   case 96:{
												   mPixelformat = FLOAT_32_32_32;
												   break;
									   }
									   case 128:{
													mPixelformat = FLOAT_32_32_32_32;
													break;
									   }

									   }
									   readDataBytes(fib, pdata);
					}
					case FIT_BITMAP:
					{

									   if (colortype == FIC_MINISWHITE || colortype == FIC_MINISBLACK)
									   {
										   FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fib);
										   // free old bitmap and replace
										   FreeImage_Unload(fib);
										   fib = newBitmap;
										   // get new formats
										   bpp = FreeImage_GetBPP(fib);
										   colortype = FreeImage_GetColorType(fib);
									   }
									   else if (bpp < 8 || colortype == FIC_PALETTE || colortype == FIC_CMYK)
									   {
										   FIBITMAP* newBitmap = FreeImage_ConvertTo32Bits(fib);
										
										   // free old bitmap and replace
										   FreeImage_Unload(fib);
										   fib = newBitmap;
										   // get new formats
										   bpp = FreeImage_GetBPP(fib);
										   colortype = FreeImage_GetColorType(fib);
									   }
									   readDataBytes(fib, pdata);
									   switch (bpp)
									   {
									   case 24:{
												   mPixelformat = UNSIGNED_CHAR_8_8_8;
												   readDataBytes(fib, pdata);
												   ConvertBetweenBGRandRGB((unsigned char*)pdata, width, height,3);
												   break;
									   }
									   case 32:{
												   mPixelformat = UNSIGNED_CHAR_8_8_8_8;
												   ConvertBetweenBGRandRGB((unsigned char*)pdata, width, height, 4);
												   break;
									   }
									   }
									   

					}break;
					case FIT_RGB16:
					{
									  FIBITMAP * newfib = FreeImage_ConvertToType(fib, FIT_BITMAP);
									  FreeImage_Unload(fib);
									  fib = newfib;
									  bpp = FreeImage_GetBPP(fib);
									  readDataBytes(fib, pdata);
									  mPixelformat = UNSIGNED_CHAR_8_8_8;
									  readDataBytes(fib, pdata);
									  
									  
					}break;
					case FIT_RGBA16:
					{
									   FIBITMAP * newfib = FreeImage_ConvertToType(fib, FIT_BITMAP);
									   FreeImage_Unload(fib);
									   fib = newfib;
									   bpp = FreeImage_GetBPP(fib);
									   mPixelformat = UNSIGNED_CHAR_8_8_8_8;
									   readDataBytes(fib, pdata);
									   
					}break;
					}
					//if (flip_vertical)
					//	FreeImage_FlipVertical(fib);
					
					FreeImage_Unload(fib);
					break;
	}
	default:
	{
			   assert(false && "image not support");
			   break;
	}
	}
	m_valid = true;
	return this;
}

void Image::test()
{
	Image* im1 = LoadImage("default_asserts/3.jpg");
	Image* im2 = LoadImage("default_asserts/2.tga");
	Image* im3 = LoadImage("default_asserts/4.jpg");
	Image* im4 = LoadImage("default_asserts/noisy-texture.png");
	Image* im5 = LoadImage("default_asserts/ennis_c02.hdr");
}

void Image::readDataBytes(FIBITMAP * fib, void*& dst)
{
	unsigned char * srcData = FreeImage_GetBits(fib);
	unsigned int srcPitch = FreeImage_GetPitch(fib);

	unsigned int dstPitch = width * (bpp / 8);

	dst = new unsigned char[height * dstPitch];

	unsigned char * pSrc;
	unsigned char * pDst = (unsigned char *)dst;

	for (size_t y = 0; y < height; y++)
	{
		pSrc = srcData + (y)* srcPitch;
		memcpy(pDst, pSrc, dstPitch);
		pDst += dstPitch;
	}
}

Image::ImageFormat Image::getImageFormat(FREE_IMAGE_FORMAT fm)
{
	ImageFormat f;
	switch (fm)
	{
	case FIF_BMP:
		f = IM_BMP; break;
	case FIF_JPEG:
		f = IM_JPEG; break;
	case FIF_PNG:
		f = IM_PNG; break;
	case FIF_TARGA:
		f = IM_TGA; break;
	case FIF_HDR:
		f = IM_HDR; break;
	default:
		f = IM_UNKNOWN; break;
		break;
	}
	return f;
}

void Image::ConvertBetweenBGRandRGB(unsigned char* input, int pixel_width, int pixel_height,int bytes_per_pixel =3)
{
	int offset = 0;

	for (int y = 0; y < pixel_height; y++) {
		for (int x = 0; x < pixel_width; x++) {
			auto tmp = input[offset];
			input[offset] = input[offset + 2];
			input[offset + 2] = tmp;
			offset += bytes_per_pixel;
		}
	}
}

Image* Image::CreateImageFromData(ImagePixelFormat format, int width, int height, void* data, string name/*=""*/)
{

	this->pdata = data;
	this->width = width;
	this->height = height;
	this->mPixelformat = format;
	return this;
}


Image* ImageManager::CreateImageFromFile(string filepath)
{
	string fullpath = FileUtil::getSceneFileValidPath(filepath);
	if (fullpath.empty()) return NULL;
	string purefilename = FileUtil::getPureFileName(fullpath);

	Image* im = get(purefilename);//以filename作为关键字查找
	if (im == NULL)
	{
		im = new Image;
		im->LoadImage(fullpath);
		add(im);
	}
	return im;
}

vector<Image*> ImageManager::CreateImageListFromFiles(vector<string> paths)
{
	vector<Image*> images;
	for (auto path : paths){
		Image* im = CreateImageFromFile(path);
		assert(im != NULL);
		images.push_back(im);
	}
	return images;
}
