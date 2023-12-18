#include <iostream>
#include<cstdio>
#include<graphics.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include<string>
#include<cstring>
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
	unsigned int f;
};

struct bullet {
	int line;
	int x;
	int f;
	bool ex;
};

IMAGE ima_bg;									//背景
IMAGE ima_p_1;							//植物：豌豆射手
IMAGE ima_z_1[22];							//僵尸，走
IMAGE ima_z_2[22];							//僵尸，吃
IMAGE ima_z_1_0;
IMAGE ima_s_1;
MOUSEMSG mou;
bool mou_p;
plant_place loca_p[6][15];   //5行6列，前行后列
int zombie_num;
zombie a[10001];
bullet b[100001];
zombie z0;
plant_place p0;
unsigned int fps;
unsigned int fps_z;
unsigned long long time_c;
int bullet_num;
int zombie_line[10];
int zombie_line_num[10];

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
void update_c();
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

void update_shoot();

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
	//char file[255] = { 0 }; 
	//char  file_add[255] = "res\\Zombies\\Zombie\\";
	//int ag={0,1,2,3,4,5,6,7,8,9,10,11,12,}
	//string s = "res\\Zombies\\Zombie\\ % c.png";
	//for (int i = 0; i <= 21; i++)
	{
		//file_add[22] = i + '0';
		//strcpy(file_add + 22, ".png");
		//loadimage(&ima_z_1[i], (LPCTSTR)file_add);
		loadimage(&ima_z_1[1], _T("res\\Zombies\\Zombie\\1.png")); 
		loadimage(&ima_z_1[2], _T("res\\Zombies\\Zombie\\2.png"));
		loadimage(&ima_z_1[3], _T("res\\Zombies\\Zombie\\3.png"));
		loadimage(&ima_z_1[4], _T("res\\Zombies\\Zombie\\4.png"));
		loadimage(&ima_z_1[5], _T("res\\Zombies\\Zombie\\5.png"));
		loadimage(&ima_z_1[6], _T("res\\Zombies\\Zombie\\6.png"));
		loadimage(&ima_z_1[7], _T("res\\Zombies\\Zombie\\7.png"));
		loadimage(&ima_z_1[8], _T("res\\Zombies\\Zombie\\8.png"));
		loadimage(&ima_z_1[9], _T("res\\Zombies\\Zombie\\9.png"));
		loadimage(&ima_z_1[10], _T("res\\Zombies\\Zombie\\10.png"));
		loadimage(&ima_z_1[11], _T("res\\Zombies\\Zombie\\11.png"));
		loadimage(&ima_z_1[12], _T("res\\Zombies\\Zombie\\12.png"));
		loadimage(&ima_z_1[13], _T("res\\Zombies\\Zombie\\13.png"));
		loadimage(&ima_z_1[14], _T("res\\Zombies\\Zombie\\14.png"));
		loadimage(&ima_z_1[15], _T("res\\Zombies\\Zombie\\15.png"));
		loadimage(&ima_z_1[16], _T("res\\Zombies\\Zombie\\16.png"));
		loadimage(&ima_z_1[17], _T("res\\Zombies\\Zombie\\17.png"));
		loadimage(&ima_z_1[18], _T("res\\Zombies\\Zombie\\18.png"));
		loadimage(&ima_z_1[19], _T("res\\Zombies\\Zombie\\19.png"));
		loadimage(&ima_z_1[20], _T("res\\Zombies\\Zombie\\20.png"));

		loadimage(&ima_z_2[1], _T("res\\zm_eat\\1.png"));
		loadimage(&ima_z_2[2], _T("res\\zm_eat\\2.png"));
		loadimage(&ima_z_2[3], _T("res\\zm_eat\\3.png"));
		loadimage(&ima_z_2[4], _T("res\\zm_eat\\4.png"));
		loadimage(&ima_z_2[5], _T("res\\zm_eat\\5.png"));
		loadimage(&ima_z_2[6], _T("res\\zm_eat\\6.png"));
		loadimage(&ima_z_2[7], _T("res\\zm_eat\\7.png"));
		loadimage(&ima_z_2[8], _T("res\\zm_eat\\8.png"));
		loadimage(&ima_z_2[9], _T("res\\zm_eat\\9.png"));
		loadimage(&ima_z_2[10], _T("res\\zm_eat\\10.png"));
		loadimage(&ima_z_2[11], _T("res\\zm_eat\\11.png"));
		loadimage(&ima_z_2[12], _T("res\\zm_eat\\12.png"));
		loadimage(&ima_z_2[13], _T("res\\zm_eat\\13.png"));
		loadimage(&ima_z_2[14], _T("res\\zm_eat\\14.png"));
		loadimage(&ima_z_2[15], _T("res\\zm_eat\\15.png"));
		loadimage(&ima_z_2[16], _T("res\\zm_eat\\16.png"));
		loadimage(&ima_z_2[17], _T("res\\zm_eat\\17.png"));
		loadimage(&ima_z_2[18], _T("res\\zm_eat\\18.png"));
		loadimage(&ima_z_2[19], _T("res\\zm_eat\\19.png"));
		loadimage(&ima_z_2[20], _T("res\\zm_eat\\20.png"));
	//	//sprintf_s(name, sizeof(name), "res\\Plants\\Peashooter\\%d.gif", i-1);
	}
	loadimage(&ima_s_1, _T("res\\bullets\\PeaNormal\\PeaNormal_0.png"));
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
		//for (int i = 1; i <= 5; i++)
		//{
		//	for (int j = 1; j <= 6; j++)
		//	{
		//		if (loca_p[i][j].ex)
		//			drawAlpha(&ima_p_1, 185 + 80 * j, 100 * i);  //植物边界
		//	}
		//}
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
	update_c();
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
			if (!zombie_line[a[i].line])
			{
				zombie_line[a[i].line] = a[i].x;
				zombie_line_num[a[i].line] = i;
			}
			if (a[i].x < zombie_line[a[i].line])
			{
				zombie_line[a[i].line] = a[i].x;
				zombie_line_num[a[i].line] = i;
			}
			//zombie_line[a[i].line] = min(a[i].x, zombie_line[a[i].line]);
			//更新这一列僵尸存在状态，供给豌豆生成

			if (a[i].state==1)
			{
				//走
				drawAlpha(&ima_z_1[a[i].f % 20 + 1], a[i].x, a[i].line * 100 - 73);
				if (!(fps % 50))
					a[i].x -= 2;
			}
			if (a[i].state == 2)
			{
				//吃
				drawAlpha(&ima_z_2[a[i].f % 20 + 1], a[i].x, a[i].line * 100 - 73);
				loca_p[a[i].line][(a[i].x - 120) / 80].hp -= 1;
			}

			if (!(fps % 50))
				a[i].f++;
			if (a[i].x < 100)
				a[i].state = 0;
			if (a[i].hp < 0)
			{
				a[i].state = 0;   //后期考虑加死亡动画
				zombie_line[a[i].line] = 0;
			}
				
		}
	}
	//if (GetTickCount() - time_c>10)
		fps ++;
	//time_c= GetTickCount();
	//Sleep(50);
}

void update_p()
{
	for (int i = 1; i <= 5; i++)
	{
		for (int j = 1; j <= 6; j++)
		{
			if (loca_p[i][j].ex)
			{
				drawAlpha(&ima_p_1, 185 + 80 * j, 100 * i);  //植物边界
				if (loca_p[i][j].hp <= 0)
				{
					loca_p[i][j].ex = 0;
				}
				if (zombie_line[i])
				if (!(fps % 100))
				{
					bullet_num++;
					b[bullet_num].ex = 1;
					b[bullet_num].line = i;
					b[bullet_num].x = 225 + 80 * j;
				}
			}
		}
	}
	//for (int i = 1; i <= 5; i++)
	//{
	//	for (int j = 1; j <= 10; j++)
	//	{
	//		printf("%d ", loca_p[i][j].ex);
	//	}
	//	printf("\n");

	//}
	update_shoot();
	//update_p();
	//update_z();
	//drawAlpha(&ima_s_1, 100 * 2, 500);
}

void update_shoot()
{
	for (int i = bullet_num>300 ? bullet_num - 300 : 1; i <= bullet_num; i++)
	{
		
		if (b[i].ex)
		{
			drawAlpha(&ima_s_1,b[i].x, 100 * b[i].line);  //子弹打印
			b[i].x++;
			if (b[i].x > 1000)
			{
				b[i].ex = 0;
			}
		}
	}
}

void update_c()
{
	for (int i = bullet_num > 300 ? bullet_num - 300 : 1; i <= bullet_num; i++)
	{
		if (b[i].ex)
		{
			if (b[i].x -50 > zombie_line[b[i].line])
			{
				b[i].ex = 0;
				a[zombie_line_num[b[i].line]].hp -= 20;
			}
		}
	}

	for (int i = zombie_num; i >= 1; i--)
	{
		if (loca_p[a[i].line][(a[i].x-120) / 80].ex)
		{
			//printf("%d   %d  %d\n",  a[i].x, a[i].x / 80, loca_p[a[i].line][(a[i].x) / 80].ex);
			a[i].state = 2;   
		}
		else {
			a[i].state = 1;
		}
	}

}

void initialize()
{
	z0 = { 0,900,100000,10,1,0};
	p0 = { 2000,1,1 };
	mou_p = 0;
	srand(time(NULL));												//初始化随机种
	time_c = GetTickCount();

	for (int ii = 1; ii <= 10; ii++)
	{
		loca_p[rand()%6][rand()%7] = p0;
		//loca_p[1][2] = p0;
	}
	//loca_p[3][4]= p0;
	
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