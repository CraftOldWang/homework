#include<graphics.h>
#include<string>	
#include<vector>

//���ڲ���
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

inline void putimage_alpha(int x, int y, IMAGE* img);



//����...�����һ����,��Ϊloadimageֻ�ܼ��ص���ͼ��
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
	
	//���Ŷ���
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
	int timer = 0;     //��ʱ��
	int idx_frame = 0;//����֡����
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

	//�����while������switch,���ʲô����?
	//�����switch����Ҫdefault��????Ϊʲô����enemyд��?
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
		//c++��^��λ����o,��Ҫ���ݵĻ�,Ҫô��pow,Ҫô����ʵʵ�˰�
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

	//������Ҽ���Ӱ
	void Draw(int delta)
	{
		//ʹ��Ӱˮƽ����
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		//ʹ��Ӱ��ֱ�����ڽŵ���
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

	POINT GetPosion()const
	{
		return position;
	}

	
private:	//����

	const int FRAME_WIDTH = 80;		//��ɫ�Ŀ�
	const int FRAME_HEIGHT = 80;	//��ɫ�ĸ�
	const int SHADOW_WIDTH = 32;	//��Ӱ�Ŀ�
	const int SPEED = 3;			//��ɫ�ٶ�



private:	//����
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
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);


		//�����߽�
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
	
		return false;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		
		return false;
	}

	void Move(const Player& player)
	{
		int dir_x = player.GetPosion().x - position.x;
		int dir_y = player.GetPosion().y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		//c++��^��λ����o,��Ҫ���ݵĻ�,Ҫô��pow,Ҫô����ʵʵ�˰�
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
	

	//���Ƶ��˼���Ӱ
	void Draw(int delta)
	{
		//ʹ��Ӱˮƽ����
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		//ʹ��Ӱ��ֱ�����ڽŵ���;�Ƚϰ�,����Ҫ-35������-8
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);


		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);

	}


private:	//����
	 
	const int FRAME_WIDTH = 80;	//���˵Ŀ�
	const int FRAME_HEIGHT = 80;	//���˵ĸ�
	const int SHADOW_WIDTH = 48;	//��Ӱ�Ŀ�
	const int SPEED = 2;		//�����ٶ�



private:	//����
	Animation* anim_left;
	Animation* anim_right;
	IMAGE img_shadow;
	POINT position = { 0,0 };
	bool facing_left = false;


};



#pragma comment(lib,"MSIMG32.LIB")
//ʹ��͸�����ֱ��͸����������Ƴ��ڵ�
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });

}


void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0)
		enemy_list.push_back(new Enemy());

}



int main()
{

	initgraph(1280, 720);

	bool running = true;

	Player player;
	ExMessage msg;
	IMAGE img_background;
	std::vector<Enemy*> enemy_list;
	
	loadimage(&img_background, _T("img/background.png"));

	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
	
		//��ȡ����
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}


		//��������
		player.Move();
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
			enemy->Move(player);





		//����
		cleardevice();
		//�ƺ�˭�Ȼ���˭�ͻᱻ����Ƶĸ���,������ص��Ļ�

		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list)
			enemy->Draw(1000/144);



		   
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