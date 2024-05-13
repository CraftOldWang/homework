#include<graphics.h>
#include<string>	

int idx_current_anim = 0;		

const int PLAYER_ANIM_NUM = 6;

const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];
IMAGE img_player_right[PLAYER_ANIM_NUM];

POINT player_pos = { 500,500 };

#pragma comment(lib,"MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });

}

void LoadAnimation()
{
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());

	}
}



int main()
{

	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;
	IMAGE img_background;

	LoadAnimation();
	loadimage(&img_background, _T("img/background.png"));



	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
	
		//读取数据
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					player_pos.y -= PLAYER_SPEED;
					break;
				case VK_DOWN:
					player_pos.y += PLAYER_SPEED;
					break;
				case VK_LEFT:
					player_pos.x -= PLAYER_SPEED;
					break;
				case VK_RIGHT:
					player_pos.x += PLAYER_SPEED;
					break;

				}
			}



		}


		//处理数据
		static int counter = 0;

		if (++counter % 5 == 0)
		{
			idx_current_anim++;
		}

		//使动画循环播放
		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;
		//???
		//if (idx_current_anim == 6)
		//{
		//	idx_current_anim = 0;
		//}




		//绘制
		cleardevice();

		putimage(0, 0, &img_background);
		putimage_alpha(player_pos.x,player_pos.y, &img_player_left[idx_current_anim]);

		FlushBatchDraw();




		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}

	EndBatchDraw();

	return 0;
}