/*
   Copyright (C) 2017, Richard e Collins.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <stdint.h>
#include <linux/fb.h>

/**
 * @brief The define USE_FREETYPEFONTS allows users of this lib to disable freetype support to reduce code size and dependencies.
 * Make sure you have freetype dev installed. sudo apt install libfreetype6-dev
 * Also add /usr/include/freetype2 to your build paths. The include macros need this.
 */
#ifdef USE_FREETYPEFONTS
	#include <freetype2/ft2build.h>
	#include FT_FREETYPE_H
#endif

namespace FBIO{	// Using a namespace to try to prevent name clashes as my class name is kind of obvious. :)
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Represents the linux frame buffer display.
 * Is able to deal with and abstract out the various pixel formats. 
 */
class FrameBuffer
{
public:
	/*
		Creates and opens a FrameBuffer object.
		If the OS does not support the frame buffer driver or there is some other error,
		this function will return NULL.
		Set pVerbose to true to get debugging information as the object is created.
	*/
	static FrameBuffer* Open(bool pVerbose = false);

	~FrameBuffer();

	/**
	 * @brief Get the setting for Verbose debug output.
	 * 
	 * @return true 
	 * @return false 
	 */
	bool GetVerbose()const{return mVerbose;}

	/*
		Returns the width of the frame buffer.
	*/
	int GetWidth()const{return mWidth;}

	/*
		Returns the height of the frame buffer.
	*/
	int GetHeight()const{return mHeight;}

	/*
		Writes a single pixel with the passed red, green and blue values. 0 -> 255, 0 being off 255 being full on.
	*/
	void WritePixel(int pX,int pY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	/*
		Clears the entrie screen.
	*/
	void ClearScreen(uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	/* 
		Expects source to be 24bit, three 8 bit bytes in R G B order.
		IE pSourcePixels[0] is red, pSourcePixels[1] is green and pSourcePixels[2] is blue.
		Renders the image to pX,pY without scaling. Most basic blit.
	*/
	void BlitRGB24(const uint8_t* pSourcePixels,int pX,int pY,int pSourceWidth,int pSourceHeight);
	
	/* 
		Expects source to be 24bit, three 8 bit bytes in R G B order.
		IE pSourcePixels[0] is red, pSourcePixels[1] is green and pSourcePixels[2] is blue.
		Renders the image to pX,pY from pSourceX,pSourceY in the source without scaling.
		pSourceStride is the byte size of one scan line in the source data.
		Allows sub rect render of the source image.
	*/
	void BlitRGB24(const uint8_t* pSourcePixels,int pX,int pY,int pWidth,int pHeight,int pSourceX,int pSourceY,int pSourceStride);

	/*
		Draws a horizontal line.
	*/
	void DrawLineH(int pFromX,int pFromY,int pToX,uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	/*
		Draws a vertical line.
	*/
	void DrawLineV(int pFromX,int pFromY,int pToY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	/*
		Draws an arbitrary line.
		Will take a short cut if the line is horizontal or vertical.
	*/
	void DrawLine(int pFromX,int pFromY,int pToX,int pToY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	/*
		Draws a circle using the Midpoint algorithm.
		https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
	*/
	void DrawCircle(int pCenterX,int pCenterY,int pRadius,uint8_t pRed,uint8_t pGreen,uint8_t pBlue,bool pFilled = false);

	/*
		Draws a rectangle with the passed in RGB values either filled or not.
	*/
	void DrawRectangle(int pFromX,int pFromY,int pToX,int pToY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue,bool pFilled = false);


	/*
		Converts from HSV to RGB.
		Very handy for creating colour palettes.
		See:- (thanks David H)
			https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both

	*/
	void HSV2RGB(float H,float S, float V,uint8_t &rRed,uint8_t &rGreen, uint8_t &rBlue)const;

private:
	FrameBuffer(int pFile,uint8_t* pFrameBuffer,struct fb_fix_screeninfo pFixInfo,struct fb_var_screeninfo pScreenInfo,bool pVerbose);

	/*
		Draws an arbitrary line.
		Using Bresenham's line algorithm
		https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	*/
	void DrawLineBresenham(int pFromX,int pFromY,int pToX,int pToY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

	const int mWidth,mHeight;
	const int mStride;// Num bytes between each line.
	const int mPixelSize;	// The byte count of each pixel. So to move in the x by one pixel.
	const int mFrameBufferFile;
	const int mFrameBufferSize;
	const bool mVerbose;
	const struct fb_var_screeninfo mVariableScreenInfo;
	uint8_t* mFrameBuffer;
};

/**
 * @brief This class is for a fast but low quality pixel font.
 * The base size of the font is 8 by 13 pixels. Any scaling is very crude.
 * No fancy antialising here. But is very fast.
 */
class PixelFont
{
public:
	PixelFont(int pPixelSize = 1);
	~PixelFont();

	int GetCharWidth()const{return 8*mPixelSize;}
	int GetCharHeight()const{return 13*mPixelSize;}

	// These render with the passed in colour, does not change the pen colour.
	void DrawChar(FrameBuffer* pDest,int pX,int pY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue,int pChar)const;
	void Print(FrameBuffer* pDest,int pX,int pY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue,const char* pText)const;
	void Printf(FrameBuffer* pDest,int pX,int pY,uint8_t pRed,uint8_t pGreen,uint8_t pBlue,const char* pFmt,...)const;

	// These use current pen. Just a way to reduce the number of args you need to use for a property that does not change that much.
	void Print(FrameBuffer* pDest,int pX,int pY,const char* pText)const;
	void Printf(FrameBuffer* pDest,int pX,int pY,const char* pFmt,...)const;
	
	void SetPenColour(uint8_t pRed,uint8_t pGreen,uint8_t pBlue);
	void SetPixelSize(int pPixelSize);
private:

	int mPixelSize;

	struct
	{
		uint8_t r,g,b;
	}mPenColour;
};

#ifdef USE_FREETYPEFONTS
/**
 * @brief This adds functionality for rendering free type fonts into the framebuffer object.
 * 
 */
class FreeTypeFont
{
public:
	FreeTypeFont(const char* pFontName,int pPixelHeight = 40,bool pVerbose = false);
	~FreeTypeFont();

	bool GetOK()const{return mOK;}
	int GetCharWidth()const{return 8;}
	int GetCharHeight()const{return 13;}

	// These render with the passed in colour, does not change the pen colour.
	int DrawChar(FrameBuffer* pDest,int pX,int pY,char pChar)const;
	void Print(FrameBuffer* pDest,int pX,int pY,const char* pText)const;
	void Printf(FrameBuffer* pDest,int pX,int pY,const char* pFmt,...)const;
	
	void SetPenColour(uint8_t pRed,uint8_t pGreen,uint8_t pBlue);
	void SetBackgroundColour(uint8_t pRed,uint8_t pGreen,uint8_t pBlue);

private:

	/**
	 * @brief Rebuilds the mBlended table from the pen and background colours.
	 */
	void RecomputeBlendTable();

	struct
	{// These are 32bit values to
		uint8_t r,g,b;
	// mBlended is a precomputed lookup for blend from background to pen colour to speed things up a bit.
	}mPenColour,mBackgroundColour,mBlended[256];

	const bool mVerbose;
	FT_Face mFace;
	bool mOK;

	/**
	 * @brief This is for the free type font support.
	 */
	static FT_Library mFreetype;
	static int mFreetypeRefCount;

};
#endif //#ifdef USE_FREETYPEFONTS

///////////////////////////////////////////////////////////////////////////////////////////////////////////
};//namespace FBIO
	
#endif //FRAME_BUFFER_H
