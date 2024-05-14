 
目录
高级语言程序设计大作业实验报告	2
一.	作业题目	2
二.	开发软件	2
三.	课题要求	2
四.	主要流程	2
五.	收获	4
 

高级语言程序设计大作业实验报告


	作业题目
一款无双割草小游戏
	开发软件
Visual Studio 2022
使用了EasyX绘图库
	课题要求
	面向对象。
	通过函数与类减少代码重复。
	使游戏尽可能流畅。
	主要流程
	实现思路：
	通过定义玩家，敌人，图集，动画等类，使用类的封装特性来将复杂的事物封装成一个整体，方便使用。从游戏的数据读取，处理数据，绘制画面这三个大方向出发分别进行相应流程的编写。将游戏分成两个部分：开始界面与游戏进行界面分别处理。使用EasyX库中的ExMessage类对象进行输入操作的读取，得到ExMessage类对象后针对不同的目的分别编写相应函数来处理信息，从而进行游戏画面的绘制。比如，在玩家类中，
class Player
{
public:
	const int FRAME_WIDTH = 80;		
	const int FRAME_HEIGHT = 80;	

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

	void Draw(int delta)
	{
		
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
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
	const POINT& GetPosion()const
	{
		return position;
	}
private:	
	const int SHADOW_WIDTH = 32;	
	const int SPEED = 3;			
private:	
	Animation* anim_left;
	Animation* anim_right;
	IMAGE img_shadow;
	POINT position = { 500,500 };
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;

};
通过ProcessEvent函数来对接收到的输入进行处理，然后move函数可以更新玩家的位置，而通过draw来在画面中绘制玩家。
	使用到的技巧：
	运用GetTickCount函数来获得每次循环的运行时间,由视觉残留知识,1秒钟内绘制画面一定次数就可以让游戏显得连贯，从而我们可以不必频繁运行循环,通过sleep来让循环暂停,从而大幅减少cpu占用.
	运用渲染缓冲区，通过BeginBatchDraw，FlushBatchDraw，FlushBatchDraw函数以及cleardevice函数,使绘制先在缓冲区进行再将缓冲区切换到显示区域，从而使绘制过程不可见，使得画面不会闪烁
	运用享元的设计模式，只需在最开始加载图片材质，运行过程中新建enemy对象时不需要再从磁盘中读取图片材质，从而加快游戏主循环运行速度，减少游戏的卡顿感。比如：
class Animation
{
public:
	Animation(Atlas* atlas,int interval)
	{
		anim_atlas = atlas;
		interval_ms = interval;
	}
	
	~Animation() = default;
	
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
	int timer = 0;    
	int idx_frame = 0; 
	int interval_ms = 0;

private:
	Atlas* anim_atlas;
};
通过使用Atlas类型的指针，需要同一批图片作为其动画的物品就可以指向同一个Atlas对象，从而不必为每一个物品单独加载图片，而只需要加载一次，在绘制时用同一个素材绘制多遍即可。
	通过将位移向量化，来保持玩家和敌人沿各个方向移动时速度相同，而不会在沿斜线走时速度变成正常的\sqrt2倍。
\ 
	收获
初步学习了游戏的制作流程，设计模式。对C++语言有了更进一步的了解，对类的封装特性有了更深的体会，学会使用类来表现某一种事物的特性与行为，并使代码组织得更有逻辑性，不显得混乱。
