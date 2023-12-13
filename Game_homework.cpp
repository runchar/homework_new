#include <iostream>
#include<cstdio>
#include<graphics.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#define	 W	1200
#define  H  600

//struct mou_m {
//	bool flag;
//};
struct plant_place {
	int hp;
	int cata;
	bool ex;
};
struct zombie {
	int line;
	//int x;
	int x;
	int hp;
	int att;
	int state;                           //行走，吃，死掉
};
IMAGE ima_bg;									//背景
IMAGE ima_p_1;							//植物：豌豆射手
IMAGE ima_z_1[22];							//僵尸，normal
IMAGE ima_z_1_0;
MOUSEMSG mou;
bool mou_p;
plant_place loca_p[6][7];
int zombie_num;
zombie a[10001];
zombie z0;
unsigned int fps;


void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标  //这一段是从网上找的
{
	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标
	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}
//void BeginBatchDraw();
//void FlushBatchDraw();
void start_up();
void initialize();
void get_input();
void update_with_input();
void update_without();
void update_p();
void update_z();
int legal_m_x(int x)
{
	if (mou.x > 955 && (mou.x) < 1100)
	{
		return mou.x;
	}
	if ((mou.x) <= 955 && mou.x < 1100)
		return 955;
	if ((mou.x) >= 1100)
		return 1100;
}
//
int legal_m_y(int y)
{
	//(mou.y - 80) > 50 ? mou.y - 80 : 50
	if (mou.y > 120 && (mou.y) < 520)
	{
		return mou.y;
	}
	if ((mou.y) <= 120)
		return 120;
	if ((mou.y) >= 520)
		return 520;
}
bool judge(int location_judge)
{
	//rectangle(1100, 250, 1170, 350);  //  1: 拖动判定框
	switch (location_judge)
	{
	case 1:
		if (mou.x <= 1170 && mou.y <= 350 && mou.x >= 1100 && mou.y >= 250)
			return 1;
		else
			return 0;
	default:
		return 0;
		break;
	}
}
int main()
{
	start_up();
	while (1)
	{
		BeginBatchDraw();											//双缓冲
		cleardevice();												//清屏
		update_with_input();
		update_without();
		FlushBatchDraw();											//双缓冲
	}
	return 0;
}
void start_up()
{
	char name[128];
	initialize();
	initgraph(W, H);												//确定窗口大小
	//for (int i = 0; i <= 20; i++)
	//{
	//	loadimage(&ima_z_1[i], _T("res\\Zombies\\Zombie\\0.png"),i);
	//	//loadimage(&ima_z_1[1], _T("res\\Zombies\\Zombie\\1.gif"));
	//	//sprintf_s(name, sizeof(name), "res\\Plants\\Peashooter\\%d.gif", i-1);
	//}
	loadimage(&ima_z_1_0, _T("res\\Zombies\\Zombie\\0.gif"));
	loadimage(&ima_bg, _T("res\\Map\\map0.jpg"));
	loadimage(&ima_p_1, _T("res\\Plants\\Peashooter\\1.gif"));     //加载豌豆射手
	loadimage(ima_z_1, _T("res\\Zombies\\Zombie\\2.gif"));
	return;
}
void update_with_input()
{
	get_input();
	if (mou.uMsg == WM_LBUTTONDOWN)
	{
		if (judge(1))			//拖动
		{
			mou_p = 1;
		}
		if (judge(2))           //暂停，存档
		{
			;
		}
	}
	if (mou.uMsg == WM_LBUTTONUP)
	{
		if (mou_p == 1)
		{
			zombie_num++;
			a[zombie_num] = z0;
			a[zombie_num].line = legal_m_y(mou.y) / 100;
			//loca_p[(mou.y - 100) / 100 + 1][(mou.x - 185) / 80].ex = 1;
		}
		mou_p = 0;
	}
	//while (1)
	{
		putimage(0, 0, &ima_bg);
		//Sleep(20);
		for (int i = 1; i <= 5; i++)
		{
			for (int j = 1; j <= 6; j++)
			{
				if (loca_p[i][j].ex)
					drawAlpha(&ima_p_1, 185 + 80 * j, 100 * i);  //植物边界
			}
		}
		//Sleep(5);
	}
	rectangle(1100, 250, 1170, 350);
	drawAlpha(&ima_z_1_0, 1000, 230);
	if (mou_p == 1)
		drawAlpha(&ima_z_1_0, legal_m_x(mou.x) - 90, legal_m_y(mou.y) - 80);
}
void get_input()
{
	if (MouseHit())
		mou = GetMouseMsg();
	//keybord  ...
}

void update_without()
{
	putimage(0, 0, &ima_bg);
	drawAlpha(&ima_z_1_0, 1000, 230);
	//Sleep(20);
	update_p();
	update_z();
}
void update_z()
{
	rectangle(1100, 250, 1170, 350);
	if (mou_p == 1)
		drawAlpha(&ima_z_1_0, legal_m_x(mou.x) - 90, legal_m_y(mou.y) - 80);
	for (int i = 1; i <= zombie_num; i++)
	{
		if (a[i].state)
		{
			drawAlpha(ima_z_1, a[i].x, a[i].line * 100 - 53);
			if (!(fps % 50))
				a[i].x -= 1;
			if (a[i].x < 100)
				a[i].state = 0;
		}
	}
	fps++;
	//Sleep(50);
}
void update_p()
{
	for (int i = 1; i <= 5; i++)
	{
		for (int j = 1; j <= 6; j++)
		{
			if (loca_p[i][j].ex)
				drawAlpha(&ima_p_1, 185 + 80 * j, 100 * i);  //植物边界
		}
	}
	//update_p();
	//update_z();
}

void initialize()
{
	z0 = { 0,900,100,10,1 };
	mou_p = 0;
	srand(time(NULL));												//初始化随机种子
	//185 + 80 * j, 100 * i											//位置
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单
// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件