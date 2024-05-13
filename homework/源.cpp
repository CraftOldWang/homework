#include<graphics.h>
#include<string>	
#include<vector>


class Animation
{
public:
	Animation(LPCTSTR path,int num,int interval)
	{
		interval_ms = interval;

		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}
	
	~Animation()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}

	}
	
	void Play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, frame_list[idx_frame]);
	}
private:
	int timer = 0;     //计时器
	int idx_frame = 0;//动画帧索引
	int interval_ms = 0;
	std::vector<IMAGE*> frame_list;
};


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

//void LoadAnimation()
//{
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
//	{
//		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_left[i], path.c_str());
//	}
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
//	{
//		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_right[i], path.c_str());
//
//	}
//}

Animation anim_left_player(_T("img/player_left_%d.png"), 6, 45);
Animation anim_right_player(_T("img/player_right_%d.png"), 6, 45);


void DrawPlayer(int delta, int dir_x)
{
	static bool facing_left = false;
	if (dir_x < 0)
	{
		facing_left = true;
	}
	else if (dir_x > 0)
	{
		facing_left = false;
	}

	if (facing_left)
	{
		anim_left_player.Play(player_pos.x, player_pos.y, delta);

	}
	else
	{
		anim_right_player.Play(player_pos.x, player_pos.y, delta);

	}
}

int main()
{

	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;
	IMAGE img_background;
	IMAGE img_shadow;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

	
	loadimage(&img_background, _T("img/background.png"));
	loadimage(&img_shadow, _T("img/shadow_player.png"));


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
					is_move_up=true;
					break;
				case VK_DOWN:
					is_move_down = true;
					break;
				case VK_LEFT:
					is_move_left = true;
					break;
				case VK_RIGHT:
					is_move_right = true;
					break;

				}
			}
			else if(msg.message == WM_KEYUP)
			{
				switch (msg.vkcode)
				{
				case VK_UP:
					is_move_up = false;
					break;
				case VK_DOWN:
					is_move_down = false;
					break;
				case VK_LEFT:
					is_move_left = false;
					break;
				case VK_RIGHT:
					is_move_right = false;
					break;

				}
			}



		}


		//处理数据
		if (is_move_up == true) { player_pos.y -= PLAYER_SPEED; }
		if (is_move_down == true) { player_pos.y += PLAYER_SPEED; }
		if (is_move_left == true) { player_pos.x -= PLAYER_SPEED; }
		if (is_move_right == true) { player_pos.x += PLAYER_SPEED; }



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