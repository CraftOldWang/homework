#include<graphics.h>


int main()
{
	initgraph(1280, 720);

	bool running = true;

	ExMessage msg;

	BeginBatchDraw();

	while (running)
	{

		while (peekmessage(&msg))
		{

		}


		cleardevice();
		FlushBatchDraw();

	}

	EndBatchDraw();

	return 0;
}