#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
#include    <string>

/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

#define RGBA_DIMEMSION	4	/* RGBA -> 4D color */
#define RGB_DIMEMSION	3	/* RGB -> 3D color */
#define COLOR_LIMIT		4095	/* Clamping the color value into 0~4095. */

GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;

	int resolution = 0;
	resolution = xres * yres;
	int frameBufferDepth = (RGB_DIMEMSION) * resolution;	// Add 1 as the Z 
	framebuffer = new char[frameBufferDepth];
	pixelbuffer = new GzPixel[resolution];
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete[] framebuffer;
	delete[] pixelbuffer;
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	GzPixel defaultPixel = { 880, 880, 880, 1, MAXINT };

	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		pixelbuffer[i] = defaultPixel;
		framebuffer[RGB_DIMEMSION * i] = (char)880;
		framebuffer[RGB_DIMEMSION * i + 1] = (char)880;
		framebuffer[RGB_DIMEMSION * i + 2] = (char)880;
		//framebuffer[RGB_DIMEMSION * i + 3] = (char)MAXINT;	// initialize Z.
	}
	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		if (z < pixelbuffer[index].z) {
			GzPixel currentPixel = { r, g, b, a, z };
			pixelbuffer[index] = currentPixel;
		}
	}
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	if (i >= 0 && i < xres && j >= 0 && j < yres) {
		int index = ARRAY(i, j);
		GzPixel currentPixel = pixelbuffer[index];
		*r = currentPixel.red;
		*g = currentPixel.green;
		*b = currentPixel.blue;
		*a = currentPixel.alpha;
		*z = currentPixel.z;
	}
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);
	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		GzPixel currentPixel = pixelbuffer[i];

		GzIntensity gotRed = currentPixel.red;
		GzIntensity gotGreen = currentPixel.green;
		GzIntensity gotBlue = currentPixel.blue;
		if (currentPixel.red < 0)
			gotRed = 0;
		if (currentPixel.red > COLOR_LIMIT)
			gotRed = COLOR_LIMIT;
		if (currentPixel.green < 0)
			gotGreen = 0;
		if (currentPixel.green > COLOR_LIMIT)
			gotGreen = COLOR_LIMIT;
		if (currentPixel.blue < 0)
			gotBlue = 0;
		if (currentPixel.blue > COLOR_LIMIT)
			gotBlue = COLOR_LIMIT;

		GzIntensity red2 = gotRed >> 4;
		char redValue = (char)(red2 & 0xFF);
		GzIntensity green2 = gotGreen >> 4;
		char greenValue = (char)(green2 & 0xFF);
		GzIntensity blue2 = gotBlue >> 4;
		char blueValue = (char)(blue2 & 0xFF);

		char color[3];
		color[0] = redValue;
		color[1] = greenValue;
		color[2] = blueValue;
		fwrite(color, 1, 3, outfile);
	}
	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	int resolution = xres * yres;
	for (int i = 0; i < resolution; i++) {
		GzPixel currentPixel = pixelbuffer[i];

		GzIntensity gotRed = currentPixel.red;
		GzIntensity gotGreen = currentPixel.green;
		GzIntensity gotBlue = currentPixel.blue;
		GzDepth gotZ = currentPixel.z;
		if (currentPixel.red < 0)
			gotRed = 0;
		if (currentPixel.red > COLOR_LIMIT)
			gotRed = COLOR_LIMIT;
		if (currentPixel.green < 0)
			gotGreen = 0;
		if (currentPixel.green > COLOR_LIMIT)
			gotGreen = COLOR_LIMIT;
		if (currentPixel.blue < 0)
			gotBlue = 0;
		if (currentPixel.blue > COLOR_LIMIT)
			gotBlue = COLOR_LIMIT;

		GzIntensity red2 = gotRed >> 4;
		char redValue = (char)(red2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 2] = redValue;

		GzIntensity green2 = gotGreen >> 4;
		char greenValue = (char)(green2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i + 1] = greenValue;

		GzIntensity blue2 = gotBlue >> 4;
		char blueValue = (char)(blue2 & 0xFF);
		framebuffer[RGB_DIMEMSION * i] = blueValue;

		//framebuffer[RGB_DIMEMSION * i + 3] = (char)gotZ;
	}
	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList) 
{
/* HW 2.1
-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
-- In later homeworks set shaders, interpolaters, texture maps, and lights
*/
	if (numAttributes == 1) {
		GzToken token = nameList[0];
		float* color = (float*)valueList[0];
		flatcolor[0] = color[0];
		flatcolor[1] = color[1];
		flatcolor[2] = color[2];
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken *nameList, GzPointer *valueList) 
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	if (numParts == 1) {
		GzCoord* verticesPointer = (GzCoord*)valueList[0];
		GzCoord vertices[3], sortedVertices[3];
		
		// V1:
		vertices[0][0] = verticesPointer[0][0];
		vertices[0][1] = verticesPointer[0][1];
		vertices[0][2] = verticesPointer[0][2];
		// V2:
		vertices[1][0] = verticesPointer[1][0];
		vertices[1][1] = verticesPointer[1][1];
		vertices[1][2] = verticesPointer[1][2];
		// V3:
		vertices[2][0] = verticesPointer[2][0];
		vertices[2][1] = verticesPointer[2][1];
		vertices[2][2] = verticesPointer[2][2];
		
		if (vertices[0][1] > vertices[1][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[0][i], vertices[1][i]);
			}
		}
		if (vertices[0][1] > vertices[2][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[0][i], vertices[2][i]);
			}
		}
		if (vertices[1][1] > vertices[2][1]) {
			for (int i = 0; i < 3; i++) {
				std::swap(vertices[1][i], vertices[2][i]);
			}
		}

		//sorted by Y. determine final order by middle-Y & special cases.
		if ((int)(vertices[0][1] + 0.5) == (int) (vertices[1][1] + 0.5)) {
			if (vertices[0][0] > vertices[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vertices[1][i], vertices[2][i]);
				}
			}
		}
		else if ((int) (vertices[1][1] + 0.5) == (int) (vertices[2][1] + 0.5)) {
			if (vertices[2][0] > vertices[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vertices[1][i], vertices[2][i]);
				}
			}
		}
		else {
			float slopeOfSideEdge, middleX;
			if ((int)(vertices[0][0] + 0.5) == (int)(vertices[2][0] + 0.5)) {
				middleX = vertices[0][0];
			}
			else {
				slopeOfSideEdge = (vertices[0][1] - vertices[2][1]) / (vertices[0][0] - vertices[2][0]);
				middleX = (vertices[1][1] - vertices[0][1]) / slopeOfSideEdge + vertices[0][0];
			}

			if (middleX > vertices[1][0]) {
				for (int i = 0; i < 3; i++) {
					std::swap(vertices[1][i], vertices[2][i]);
				}
			}
		}
		//sorted as CW. 3 edges: 1-2, 2-3, 3-1.
		float deltaX12, deltaY12, deltaX23, deltaY23, deltaX31, deltaY31;
		float A12, B12, C12, A23, B23, C23, A31, B31, C31;
		
		deltaX12 = vertices[1][0] - vertices[0][0];
		deltaY12 = vertices[1][1] - vertices[0][1];
		deltaX23 = vertices[2][0] - vertices[1][0];
		deltaY23 = vertices[2][1] - vertices[1][1];
		deltaX31 = vertices[0][0] - vertices[2][0];
		deltaY31 = vertices[0][1] - vertices[2][1];
		
		A12 = deltaY12;
		B12 = -1.0f * deltaX12;
		C12 = deltaX12 * vertices[0][1] - deltaY12 * vertices[0][0];
		A23 = deltaY23;
		B23 = -1.0f * deltaX23;
		C23 = deltaX23 * vertices[1][1] - deltaY23 * vertices[1][0];
		A31 = deltaY31;
		B31 = -1.0f * deltaX31;
		C31 = deltaX31 * vertices[2][1] - deltaY31 * vertices[2][0];
		// Get the current plane to interpolate Z:
		float X1 = vertices[1][0] - vertices[0][0];
		float Y1 = vertices[1][1] - vertices[0][1];
		float Z1 = vertices[1][2] - vertices[0][2];
		float X2 = vertices[2][0] - vertices[0][0];
		float Y2 = vertices[2][1] - vertices[0][1];
		float Z2 = vertices[2][2] - vertices[0][2];
		float planeA = (Y1 * Z2) - (Z1 * Y2);
		float planeB = -((X1 * Z2) - (Z1 * X2));
		float planeC = (X1 * Y2) - (Y1 * X2);
		float planeD = -1.0f * (planeA * vertices[0][0] + planeB * vertices[0][1] + planeC * vertices[0][2]);

		// Get Bounding Box:
		float minX = min(min(vertices[0][0], vertices[1][0]), vertices[2][0]);
		float maxX = max(max(vertices[0][0], vertices[1][0]), vertices[2][0]);
		float minY = min(min(vertices[0][1], vertices[1][1]), vertices[2][1]);
		float maxY = max(max(vertices[0][1], vertices[1][1]), vertices[2][1]);
		int minXPixel = (int)(minX + 0.5);
		int maxXPixel = (int)(maxX + 0.5);
		int minYPixel = (int)(minY + 0.5);
		int maxYPixel = (int)(maxY + 0.5);
		// Start Rasterization:
		for (int i = minXPixel; i <= maxXPixel; i++) {
			for (int j = minYPixel; j <= maxYPixel; j++) {
				//int currentIndex = ARRAY(i, j);
				float LEE12 = A12 * (float)i + B12 * (float)j + C12;
				float LEE23 = A23 * (float)i + B23 * (float)j + C23;
				float LEE31 = A31 * (float)i + B31 * (float)j + C31;

				if ((LEE12 > 0 && LEE23 > 0 && LEE31 > 0 && planeC != 0) 
					|| (LEE12 < 0 && LEE23 < 0 && LEE31 < 0 && planeC != 0)
					|| LEE12 == 0 || LEE23 == 0 || LEE31 == 0) { // Any pixel inside or on the 3 edges.
					float interpolatedZ = -1.0f * (planeA * (float)i + planeB * (float)j + planeD) / planeC;
					int currentZ = (int)(interpolatedZ + 0.5);
					GzIntensity redIntensity = ctoi(flatcolor[0]);
					GzIntensity greenIntensity = ctoi(flatcolor[1]);
					GzIntensity blueIntensity = ctoi(flatcolor[2]);
					// Call GzPut to push the pixel to pixelbuffer.
					GzPut(i, j, redIntensity, greenIntensity, blueIntensity, 1, currentZ);
				}
			}
		}
	}
	return GZ_SUCCESS;
}

