// CharEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <windows.h>
#include <chrono>

using namespace std;

typedef std::chrono::high_resolution_clock Clock;

const int width = 316;//316 fullscreen
const int height = (int)((float)width / 1.8f);//320

const int yResolution = (int)((float)width * 0.27f);//320
int Pixels[width][yResolution];

void CursorGoTo(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(handle, c);
}


void Print()
{
	CursorGoTo(0, 0);
    string row;
	for (int y = 0; y < yResolution; y++)
	{
        row = "";
		for (int x = 0; x < width; x++)
		{
			if (Pixels[x][y] != 0)
			{
				row += "#";
			}
			else
			{
				row += " ";
			}
            Pixels[x][y] = 0;
		}
		cout << row << endl;
		//Sleep(10);
	}
}
int halfOfWidth = width * 0.5f;
int halfOfHeight = height * 0.5f;
float yResize = (float)height / (float)width;
void setPixel(int x, int y)
{
    x = halfOfWidth + x;
    y = (halfOfHeight - y) * yResize;
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        Pixels[x][y] = 1;
    }
}


void drawHLine(int x1, int y1, int x2)
{
	int s;
	for (int j = 0; j < abs(x1 - x2) + 1; j++)
	{
		s = x1 + j * ((x1 > x2) ? -1 : 1);
		if (s >= 0 && s < width)
		{
			Pixels[s][y1] = 1;
		}
	}
}

void drawVLine(int x1, int y1, int y2)
{
	int s;
	for (int j = 0; j < abs(y1 - y2) + 1; j++)
	{
		s = y1 + j * ((y1 > y2) ? -1 : 1);
		if (s >= 0 && s < height)
		{
			Pixels[x1][s] = 1;
		}
	}
}

void drawLine(int x1, int y1, int x2, int y2)//(10, 10, 20, 10)
{
    int lx2 = x2 - x1;
    int ly2 = y2 - y1;
    float interpolation = 0;
    float step;
    int xFlip = (lx2 > 0) ? 1 : -1;
    int yFlip = (ly2 > 0) ? 1 : -1;
    lx2 = abs(lx2);
    ly2 = abs(ly2);

    if (lx2 > ly2)
    {
        step = (float)ly2 / (float)lx2;
        while (interpolation < lx2)
        {
            setPixel(interpolation * xFlip + x1, interpolation * step * yFlip + y1);
            interpolation++;
        }
    }
    else
    {
        step = (float)lx2 / (float)ly2;
        while (interpolation < ly2)
        {
            setPixel(interpolation * step * xFlip + x1, interpolation * yFlip + y1);
            interpolation++;
        }
    }
}



float x = 0;
float y = 0;
float z = 0;
float xAngle = 0;
float yAngle = 0;
float zAngle = 0;
int klawisz = 0;
bool nowe = true;


int dist = 8;
int fov = 80;
float xtmp;
float ytmp;
float ztmp;
float returnf;
float returns;

/*void MdrawLine(int xf, int yf, int xs, int ys)
{
    float pos1x;
    float pos1y;
    float pos2x;
    float pos2y;
    xf = 0.5f * xResolution + xf;
    yf = 0.5f * yResolution + yf;
    xs = 0.5f * xResolution + xs;
    ys = 0.5f * yResolution + ys;
    if (0 > xf || 0 > yf || xf > xResolution || yf > yResolution)
    {
        pos1x = xs;
        pos1y = ys;
        pos2x = xf;
        pos2y = yf;
    }
    else
    {
        pos1x = xf;
        pos1y = yf;
        pos2x = xs;
        pos2y = ys;
    }
    float heightFactor;
    if (0 > pos2y && !(0 > pos1y && 0 > pos2y))
    {
        heightFactor = pos1y / abs(pos1y - pos2y);
        pos2x = pos1x - (pos1x - pos2x) * heightFactor;
        pos2y = 0;
    }
    if (0 > pos2x && !(0 > pos1x && 0 > pos2x))
    {
        heightFactor = pos1x / abs(pos1x - pos2x);
        pos2y = pos1y - (pos1y - pos2y) * heightFactor;
        pos2x = 0;
    }
    if (pos2y > yResolution && !(pos1y > yResolution && pos2y > yResolution))
    {
        heightFactor = (yResolution - pos1y) / abs(pos1y - pos2y);
        pos2x = pos1x - (pos1x - pos2x) * heightFactor;
        pos2y = yResolution;
    }
    if (pos2x > xResolution && !(pos1x > xResolution && pos2x > xResolution))
    {
        heightFactor = (xResolution - pos1x) / abs(pos1x - pos2x);
        pos2y = pos1y - (pos1y - pos2y) * heightFactor;
        pos2x = xResolution;
    }
    if ((pos1x > 0 && xResolution > pos1x && pos1y > 0 && yResolution > pos1y) || (pos2x > 0 && xResolution > pos2x && pos2y > 0 && yResolution > pos2y))
    {
        drawLine((int)pos1x, (int)pos1y, (int)pos2x, (int)pos2y);
    }
}*/

void rotate2(float x, float y, float angle)
{
    returnf = x * cos(angle) + y * sin(angle);
    returns = y * cos(angle) - x * sin(angle);
}

void rotate(float x, float y, float z)
{
    rotate2(x, y, zAngle);
    xtmp = returnf;
    ytmp = returns;
    rotate2(xtmp, z, yAngle);
    xtmp = returnf;
    ztmp = returns;
    rotate2(ztmp, ytmp, xAngle);
    ztmp = returnf;
    ytmp = returns;
}

void tdLine(float x, float y, float z, float x2, float y2, float z2)
{
    rotate(x, y, z);
    float xf = xtmp;
    float yf = ytmp;
    float zf = ztmp;
    float multiplier = 0.5f * width / (((float)zf + (float)dist) * (float)tan(fov / 2));
    xf = xf * multiplier;
    yf = yf * multiplier;
    rotate(x2, y2, z2);
    float xs = xtmp;
    float ys = ytmp;
    float zs = ztmp;
    multiplier = 0.5f * width / (((float)zs + (float)dist) * (float)tan(fov / 2));
    xs = xs * multiplier;
    ys = ys * multiplier;
    drawLine(xf, yf, xs, ys);
}

void drawCube(float xpos, float ypos, float zpos)
{
    tdLine(-1 + xpos, -1 + ypos, 1 + zpos, 1 + xpos, -1 + ypos, 1 + zpos);
    tdLine(-1 + xpos, 1 + ypos, 1 + zpos, 1 + xpos, 1 + ypos, 1 + zpos);
    tdLine(1 + xpos, -1 + ypos, 1 + zpos, 1 + xpos, 1 + ypos, 1 + zpos);
    tdLine(-1 + xpos, -1 + ypos, 1 + zpos, -1 + xpos, 1 + ypos, 1 + zpos);
    tdLine(-1 + xpos, -1 + ypos, 1 + zpos, -1 + xpos, -1 + ypos, -1 + zpos);
    tdLine(-1 + xpos, -1 + ypos, -1 + zpos, 1 + xpos, -1 + ypos, -1 + zpos);
    tdLine(1 + xpos, -1 + ypos, -1 + zpos, 1 + xpos, -1 + ypos, 1 + zpos);
    tdLine(1 + xpos, 1 + ypos, -1 + zpos, 1 + xpos, 1 + ypos, 1 + zpos);
    tdLine(-1 + xpos, 1 + ypos, -1 + zpos, -1 + xpos, 1 + ypos, 1 + zpos);
    tdLine(-1 + xpos, 1 + ypos, -1 + zpos, -1 + xpos, -1 + ypos, -1 + zpos);
    tdLine(1 + xpos, 1 + ypos, -1 + zpos, 1 + xpos, -1 + ypos, -1 + zpos);
    tdLine(-1 + xpos, 1 + ypos, -1 + zpos, 1 + xpos, 1 + ypos, -1 + zpos);
}

void drawPlane(int xSize, int zSize, float x, float z)
{
    int ypos = -1;
    for (int xpos = -xSize / 2; xpos < xSize; xpos += 2)
    {
        // int zcop;
        for (int zpos = -zSize / 2; zpos < zSize; zpos += 2)
        {
            tdLine(-1 + xpos + x, -1 + ypos, 1 + zpos + z, -1 + xpos + x, -1 + ypos, -1 + zpos + z);
            tdLine(-1 + xpos + x, -1 + ypos, 1 + zpos + z, 1 + xpos + x, -1 + ypos, 1 + zpos + z);
            //tdLine(-1 + xpos, -1 + ypos, -1 + zpos, 1 + xpos, -1 + ypos, -1 + zpos);
            //  tdLine(1 + xpos, -1 + ypos, -1 + zpos, 1 + xpos, -1 + ypos, 1 + zpos);
            // zcop = zpos;
        }
        tdLine(-1 + xpos + x, -1 + ypos, 3 - zSize + z, 1 + xpos + x, -1 + ypos, 3 - zSize + z);
        // tdLine(1 + xpos, -1 + ypos, -1 + zcop, 1 + xpos, -1 + ypos, 1 + zcop);
    }
}





int main()
{
    while (true)
    {
        auto t1 = Clock::now();
        if (true)
        {
            drawCube(x, y, z);
            
            POINT p;
            if (GetCursorPos(&p))
            {
                yAngle = p.x / 500.0f;
                xAngle = p.y / 500.0f;
            }

            Print();
            //Sleep(100);
        }
        auto t2 = Clock::now();
        std::cout << 1000.0f / std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << " FPS" << std::endl;
    }
}


