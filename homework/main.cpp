#include<graphics.h>
#include<string>	
#include<vector>

//窗口参数
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int BUTTON_WIDTH = 192;
const int BUTTON_HEIGHT = 75;

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")

bool is_game_started = false;
bool running = true;

//使得透明部分变成透明而不会绘制出黑色
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });

}



class Atlas
{
public:
	Atlas(LPCTSTR path, int num)
	{

		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	}

	~Atlas()
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}

	}
	
public:
	std::vector<IMAGE*> frame_list;
};

Atlas* atlas_player_left;
Atlas* atlas_player_right;
Atlas* atlas_enemy_left;
Atlas* atlas_enemy_right;


//动画...定义成一个类,因为loadimage只能加载单张图像
class Animation
{
public:
	Animation(Atlas* atlas,int interval)
	{
		anim_atlas = atlas;
		interval_ms = interval;
	}
	
	~Animation() = default;
	
	//播放动画
	void Play(int x, int y, int delta)
	{
		timer += delta;
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}

		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}
private:
	int timer = 0;     //计时器
	int idx_frame = 0;//动画帧索引
	int interval_ms = 0;

private:
	Atlas* anim_atlas;
};

class Player
{
public:
	const int FRAME_WIDTH = 80;		//角色的宽
	const int FRAME_HEIGHT = 80;	//角色的高

public:
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));	
		anim_left = new Animation(atlas_player_left,45);
		anim_right = new Animation(atlas_player_right,45);
	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	//这里把while换成了switch,会出什么事吗?
	//这里的switch不需要default吗????为什么下面enemy写了?
	void ProcessEvent(const ExMessage&msg)
	{
		switch(msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
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
			break;
			
		case WM_KEYUP:
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
			break;
		}

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
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);

		}

		if (position.x < 0)position.x = 0;
		if (position.y < 0)position.y = 0;
		if (position.x + FRAME_WIDTH > WINDOW_WIDTH)position.x = WINDOW_WIDTH - FRAME_WIDTH;
		if (position.y + FRAME_HEIGHT > WINDOW_HEIGHT)position.y = WINDOW_HEIGHT - FRAME_HEIGHT;


	}

	//绘制玩家及阴影
	void Draw(int delta)
	{
		//使阴影水平居中
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		//使阴影竖直方向在脚底下
		int pos_shadow_y = position.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);

		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0) 
			facing_left = true; 
		else if (dir_x > 0) 
			facing_left = false; 

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);
		
	}

	const POINT& GetPosion()const//前面的const &减少拷贝制造的资源浪费同时防止position在外面被修改,后面的const防止position在这个函数内被修改
	{
		return position;
	}

	
private:	//常量

	
	const int SHADOW_WIDTH = 32;	//阴影的宽
	const int SPEED = 3;			//角色速度



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

class Bullet
{
public:
	POINT position = { 0,0 };

public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}


	
private:
	const int RADIUS = 10;


};

class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(atlas_enemy_left, 45);
		anim_right = new Animation(atlas_enemy_right, 45);


		//出生边界
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right,

		};

		SpawnEdge edge = (SpawnEdge)(rand() % 4);

		switch (edge)
		{
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = - FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x =WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		default:
			break;
		}

	}

	~Enemy()
	{
		delete anim_left;
		delete anim_right;
	}

	bool CheckBulletCollision(const Bullet& bullet)
	{
		bool is_overlap_x = (bullet.position.x >= position.x) && (bullet.position.x <= (position.x + FRAME_WIDTH));
		bool is_overlap_y = (bullet.position.y >= position.y) && (bullet.position.y <= (position.y + FRAME_HEIGHT));
		return is_overlap_x&&is_overlap_y;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		POINT check_position = { position.x + FRAME_WIDTH / 2,	position.y + FRAME_HEIGHT / 2 };
		bool is_overlap_x = (check_position.x >= player.GetPosion().x) && (check_position.x <= (player.GetPosion().x + FRAME_WIDTH));
		bool is_overlap_y = (check_position.y >= player.GetPosion().y )&& (check_position.y <= (player.GetPosion().y + FRAME_HEIGHT));
		return is_overlap_x && is_overlap_y;
	}

	void Hurt()
	{
		alive = false;
		
	}

	bool CheckAlive()
	{
		return alive;
	}


	void Move(const Player& player)
	{
		int dir_x = player.GetPosion().x - position.x;
		int dir_y = player.GetPosion().y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		//c++里^是位运算o,想要算幂的话,要么用pow,要么老老实实乘吧
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);

		}

		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;

	}

	//绘制敌人及阴影
	void Draw(int delta)
	{
		//使阴影水平居中
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		//使阴影竖直方向在脚底下;比较矮,所以要-35而不是-8
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);


		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);

	}


private:	//常量...大概要有常量的字母全部大写的习惯
	 
	const int FRAME_WIDTH = 80;	//敌人的宽
	const int FRAME_HEIGHT = 80;	//敌人的高
	const int SHADOW_WIDTH = 48;	//阴影的宽
	const int SPEED = 2;		//敌人速度



private:	//变量
	Animation* anim_left;
	Animation* anim_right;
	IMAGE img_shadow;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;

};

class Button
{
public:
	Button(RECT rect,LPCTSTR path_img_idle,LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
	{
		region = rect;

		loadimage(&img_idle, path_img_idle);
		loadimage(&img_hovered, path_img_hovered);
		loadimage(&img_pushed, path_img_pushed);

	}
	
	~Button() = default;

	//检测鼠标进入按钮所在矩形
	bool CheckCursorHit(int x,int y)
	{
		bool is_overlap_x = (x >= region.left) && (x <= region.right);
		bool is_overlap_y = (y >= region.top) && (y <= region.bottom);//注意y轴正方向是向下,故top反而比bottom小
		return is_overlap_x && is_overlap_y;
	}

	void ProcessEvent(const ExMessage& msg)
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (status == Status::Idle && CheckCursorHit(msg.x, msg.y))
				status = Status::Hovered;
			else if (status == Status::Hovered && !CheckCursorHit(msg.x, msg.y))
				status = Status::Idle;
			break;

		case WM_LBUTTONDOWN:
			if (CheckCursorHit(msg.x, msg.y))
				status = Status::Pushed;
			break;

		case WM_LBUTTONUP:
			if (status == Status::Pushed)
				OnClick();
			break;
		default:	//不写default有什么风险吗?这里面似乎有个地方没写.
			break;
			
		}
	}

	void Draw()
	{
		switch (status)
		{
		case Status::Idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::Hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::Pushed:
			putimage(region.left, region.top, &img_pushed);
			break;

		}
	}

protected:
	virtual void OnClick() = 0;

private:
	enum class Status
	{
		Idle = 0,
		Hovered ,
		Pushed,
	};
private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::Idle;
};


//开始游戏按钮
class StartGameButton :public Button
{
public:
	StartGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button( rect, path_img_idle, path_img_hovered, path_img_pushed){}

	~StartGameButton() = default;
protected:
	void OnClick()
	{
		is_game_started = true;

		mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	}



};


//退出游戏按钮
class QuitGameButton :public Button
{
public:
	QuitGameButton(RECT rect, LPCTSTR path_img_idle, LPCTSTR path_img_hovered, LPCTSTR path_img_pushed)
		:Button( rect, path_img_idle, path_img_hovered, path_img_pushed) {}

		~QuitGameButton() = default;
protected:
	void OnClick()
	{
		running = false;
	}
};



void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());

}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045;		//径向速度
	const double TANGENT_SPEED = 0.0055;	//切向速度
	double radian_interval = 2 * 3.14159 / bullet_list.size();	//子弹弧度间隔.
	POINT player_position = player.GetPosion();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	for (size_t i = 0; i < bullet_list.size();i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
		bullet_list[i].position.x = player_position.x + player.FRAME_WIDTH / 2 + (int)(radius * sin(radian));
		bullet_list[i].position.y = player_position.y + player.FRAME_HEIGHT / 2 + (int)(radius * cos(radian));

	}


}

void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text,_T("当前玩家得分: %d" ), score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}



int main()
{

	initgraph(1280, 720);

	atlas_player_left = new Atlas(_T("img/player_left_%d.png"), 6);
	atlas_player_right = new Atlas(_T("img/player_right_%d.png"), 6);
	atlas_enemy_left = new Atlas(_T("img/enemy_left_%d.png"), 6);
	atlas_enemy_right = new Atlas(_T("img/enemy_right_%d.png"), 6);


	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);


	



	int score = 0;
	Player player;
	ExMessage msg;
	IMAGE img_menu;
	IMAGE img_background;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(3);
	
	RECT region_btn_start_game, region_btn_quit_game;

	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;

	region_btn_start_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_start_game.right = region_btn_start_game.left + BUTTON_WIDTH;
	region_btn_start_game.top = 430; 
	region_btn_start_game.bottom = region_btn_start_game.top + BUTTON_HEIGHT;
	
	region_btn_quit_game.left = (WINDOW_WIDTH - BUTTON_WIDTH) / 2;
	region_btn_quit_game.right = region_btn_quit_game.left + BUTTON_WIDTH;
	region_btn_quit_game.top = 550;
	region_btn_quit_game.bottom = region_btn_quit_game.top + BUTTON_HEIGHT;

	StartGameButton btn_start_game = StartGameButton(region_btn_start_game,
		_T("img/ui_start_idle.png"), _T("img/ui_start_hovered.png"), _T("img/ui_start_pushed.png"));
	QuitGameButton btn_quit_game = QuitGameButton(region_btn_quit_game,
		_T("img/ui_quit_idle.png"), _T("img/ui_quit_hovered.png"), _T("img/ui_quit_pushed.png"));


	loadimage(&img_menu, _T("img/menu.png"));
	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
	
		//读取数据
		while (peekmessage(&msg))
		{
			if(is_game_started)
				player.ProcessEvent(msg);
			else
			{
				btn_start_game.ProcessEvent(msg);
				btn_quit_game.ProcessEvent(msg);
			}
		}



		if (is_game_started)
		{
			//处理数据
			player.Move();
			UpdateBullets(bullet_list, player);

			TryGenerateEnemy(enemy_list);

			for (Enemy* enemy : enemy_list)
				enemy->Move(player);


			//检测敌人与玩家的碰撞
			for (Enemy* enemy : enemy_list)
			{
				if (enemy->CheckPlayerCollision(player))
				{
					static TCHAR text[128];

					_stprintf_s(text, _T("最终得分: %d !"), score);
					MessageBox(GetHWnd(), text, _T("GAME OVER"), MB_OK);
					running = false;
					break;
				}

			}

			//检测敌人与子弹的碰撞
			for (Enemy* enemy : enemy_list)
			{
				for (const Bullet& bullet : bullet_list)
					if (enemy->CheckBulletCollision(bullet))
					{
						mciSendString(_T("play hit from 0"), NULL, 0, NULL);
						enemy->Hurt();
						score++;
					}
			}

			for (size_t i = 0; i < enemy_list.size(); i++)
			{
				Enemy* enemy = enemy_list[i];
				if (!enemy->CheckAlive())
				{
					std::swap(enemy_list[i], enemy_list.back());
					enemy_list.pop_back();
					delete enemy;
				}
			}

		}


		//绘制
		cleardevice();
		//似乎谁先绘制谁就会被后绘制的覆盖,如果有重叠的话

		if (is_game_started)
		{
			putimage(0, 0, &img_background);
			player.Draw(1000 / 144);

			for (Enemy* enemy : enemy_list)//why这里不可以const而下面bullets的就可以?....这里vector里存的是指针,下面那个存的是Bullet对象)...但还是不懂
				enemy->Draw(1000 / 144);

			for (const Bullet& bullet : bullet_list)
				bullet.Draw();

			DrawPlayerScore(score);
		}
		else
		{
			putimage(0, 0, &img_menu);
			btn_start_game.Draw();
			btn_quit_game.Draw();
		}
		   
		FlushBatchDraw();




		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
	}

	delete atlas_player_left;
	delete atlas_player_right;
	delete atlas_enemy_left;
	delete atlas_enemy_right;


	EndBatchDraw();

	return 0;
}