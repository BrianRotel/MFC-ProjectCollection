#include<windows.h>
#include<stdio.h>
#include<math.h>
#pragma comment(lib,"user32.lib") 
#pragma comment(lib,"gdi32.lib")
const double pi = 3.14159265;

void ClearWind1(HDC hdc, int SCREEN_WIDTH, int SCREEN_HEIGHT);
int main1(int argc, char *argv[])
{
	TCHAR title[256];//控制台程序标题
	GetConsoleTitle(title, 256);//获取控制台标题
	//先通过FindWindow函数借助标题获取窗口句柄HWND，之后再利用GetDC函数借助HWND获取HDC
	HWND hWnd = FindWindow(0, title);
	HDC hdc = GetDC(hWnd);//HDC是设备描述表句柄（/获取屏幕显示DC）

	int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN); //屏幕宽度   
	int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN); //屏幕高度  

	double rho, theta, x, y;//极坐标和直角坐标
	HPEN hPen1, hPen2, hPen3;
	hPen1 = CreatePen(PS_SOLID, 0, RGB(255, 155, 0));//添加画笔属性
	hPen2 = CreatePen(PS_SOLID, 0, RGB(0, 255, 0));//添加画笔属性
	hPen3 = CreatePen(PS_SOLID, 0, RGB(0, 0, 255));//添加画笔属性

	for (int j = 0; j<1000; j++){
		ClearWind1(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);//清屏	
		for (int i = 0; i<20000; i++)
		{
			theta = -pi + pi / 10000.0*i;
			rho = 1 + cos(7 * (theta + j / 100.0)) + pow(1.5*(sin(3 * (theta + j / 100.0))), 2);
			x = 400 + 50 * rho*cos(theta);
			y = 300 - 50 * rho*sin(theta);

			//三支不同色的笔
			if (theta<(-pi / 3.0)){
				SelectObject(hdc, hPen1);//画笔绑定到句柄
			}
			if (theta>-pi / 3.0)
			{
				SelectObject(hdc, hPen2);//画笔绑定到句柄
			}
			if (theta>pi / 3.0)
			{
				SelectObject(hdc, hPen3);//画笔绑定到句柄
			}
			MoveToEx(hdc, 400, 300, NULL); //笔移到起点
			LineTo(hdc, (int)x, (int)y); //画线作图
		}
		Sleep(10);//延时 ms
	}
	return 0;
}

void ClearWind1(HDC hdc, int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	HPEN hPen;
	HBRUSH hBrush;
	hBrush = CreateSolidBrush(RGB(0, 128, 128));
	hPen = CreatePen(PS_SOLID, 0, RGB(0, 128, 128));
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	Rectangle(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

