#include<graphics.h>
#include<string>	
#include<vector>


inline void putimage_alpha(int x, int y, IMAGE* img);

//动画...定义成一个类,因为loadimage只能加载单张图像
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
	
	//播放动画
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

class Player
{
public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));	
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);
	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage&msg)
	{

	}

	void Move()
	{
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		//c++里^是位运算o,想要算幂的话,要么用pow,要么老老实实乘吧
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(PLAYER_SPEED * normalized_x);
			position.y += (int)(PLAYER_SPEED * normalized_y);

		}

		if (position.x < 0)position.x = 0;
		if (position.y < 0)position.y = 0;
		if (position.x + PLAYER_WIDTH > WINDOW_WIDTH)position.x = WINDOW_WIDTH - PLAYER_WIDTH;
		if (position.y + PLAYER_HEIGHT > WINDOW_HEIGHT)position.y = WINDOW_HEIGHT - PLAYER_HEIGHT;


	}

	void Draw(int delta)
	{

	}

	
private:	//常量

	const int PLAYER_WIDTH = 80;	//角色的宽
	const int PLAYER_HEIGHT = 80;	//角色的高
	const int SHADOW_WIDTH = 32;	//阴影的宽
	const int PLAYER_SPEED = 3;	//角色速度



private:	//变量
	Animation* anim_left;
	Animation* anim_right;
	IMAGE img_shadow;
	POINT position = { 500,500 };
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

};




//窗口参数
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;




IMAGE img_background;




#pragma comment(lib,"MSIMG32.LIB")
//透明部分变成透明而不会绘制出黑的
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });

}

//绘制玩家及阴影
void DrawPlayer(int delta, int dir_x)
{
	//使阴影水平居中
	int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);
	//使阴影竖直方向在脚底下
	int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;
	putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

	static bool facing_left = false;
	 
	if (dir_x < 0){	facing_left = true;}
	else if (dir_x > 0)	{facing_left = false;}

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


*
		//绘制
		cleardevice();

		putimage(0, 0, &img_background);
		DrawPlayer(1000 / 144, is_move_right - is_move_left);
		   
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