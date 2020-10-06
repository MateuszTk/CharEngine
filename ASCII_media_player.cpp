#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <math.h>
#include <Windows.h>

using namespace cv;
using namespace std;

struct Color
{
	int R;
	int G;
	int B;
};

HANDLE handle;
void CursorGoTo(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(handle, c);
}

int main()
{
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	bool mode = false;// true - image; false - video
	char shades[] = { ' ', '.', ',', ':', ';', '!', '+', '*', '?','#', '%' ,'@' };
	int shadesCount = sizeof(shades) / sizeof(shades[0]);
	const int yTargetResolution = 68;

	Mat image;
	VideoCapture cap;
	

	if (mode)
	{
		image = imread("F:\\obrazy\\test.png", IMREAD_COLOR);
		if (!image.data)
		{
			std::cout << "Could not open or find the image" << std::endl;
			return -1;
		}
	}
	else
	{
		if (cap.open("F:\\wideo\\Valerian.and.the.City.of.a.Thousand.Planets.2017.PL.BDRip.XviD-KiT(1).avi"))
		{  
			std::cout << "ok";
		}
		if (!cap.isOpened())
		{
			std::cout << "Error opening video" << endl;

			return -1;
		}
	}

	while (true)
	{
		cap >> image;
		if (mode || image.empty())
		{
			break;
		}
		int imWidth = image.cols;
		int imHeight = image.rows;
		int channels = image.channels();
		//cout << imWidth << "" << imHeight << " " << channels << endl;
		uchar* pixelPtr = image.data;
		string pixels = "";
		float yStep = (float)imHeight / (float)yTargetResolution;
		int xAdaptiveResolution = (int)((float)imWidth / (float)imHeight * (float)yTargetResolution * 1.94f);
		float xStep = (float)imWidth / (float)xAdaptiveResolution;

		Color pixelColor;
		int xCord;
		int yCord;
		CursorGoTo(0, 0);
		for (int y = 0; y < yTargetResolution - 1; y++)
		{
			pixels = "";
			for (int x = 0; x < xAdaptiveResolution - 1; x++)
			{
				xCord = (int)((float)x * xStep);
				yCord = (int)((float)y * yStep);
				pixelColor.B = pixelPtr[yCord * imWidth * channels + xCord * channels];
				pixelColor.G = pixelPtr[yCord * imWidth * channels + xCord * channels + 1];
				pixelColor.R = pixelPtr[yCord * imWidth * channels + xCord * channels + 2];
				pixels += shades[(int)(round(max(pixelColor.R, max(pixelColor.G, pixelColor.B)) / 255.0f * (float)(shadesCount - 1)))];
			}
			std::cout << pixels + '\n';
		}
		Sleep(14);
	}

	cap.release();
	return 0;
}







