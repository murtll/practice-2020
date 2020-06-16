#include <windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

using namespace vizdoom;
using namespace cv;
using namespace std;


int main()
{
	DoomGame* game = new DoomGame();
	try
	{
		string path = "C:\\practice\\vizdoom";
		game->setViZDoomPath(path + "\\vizdoom.exe");
		game->setDoomGamePath(path + "\\freedoom2.wad");
		game->loadConfig(path + "\\scenarios\\basic.cfg");
		game->setWindowVisible(true);
		game->setRenderWeapon(true);
		game->init();
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}

	//namedWindow("Control window", WINDOW_AUTOSIZE);
	//auto image = Mat(480, 640, CV_8UC3);

	unsigned int sleepTime = 1000 / DEFAULT_TICRATE;

	auto episodes = 10;

	std::vector<double> actions[3];
	actions[0] = { 1, 0, 0 };
	actions[1] = { 0, 1, 0 };
	actions[2] = { 0, 0, 1 };

	for (auto i = 0; i < episodes; i++)
	{
		game->newEpisode();
		cout << "Episode #" << i + 1 << endl;

		while (!game->isEpisodeFinished())
		{
			auto gamestate = game->getState();

			auto screenBuf = gamestate->screenBuffer;

			//image.data = screenBuf->data();
			//imshow("Control Window", screenBuf->data());

			for (auto i = 0; i < 30; i++)
			{
				game->makeAction(actions[2]);
				game->makeAction(actions[0]);
				//waitKey(sleepTime);
				Sleep(sleepTime);
			}

			for (auto i = 0; i < 60; i++)
			{
				game->makeAction(actions[2]);
				game->makeAction(actions[1]);
				//waitKey(sleepTime);
				Sleep(sleepTime);
			}

		}

		cout << game->getTotalReward() << endl;

	}

	game->close();
	delete game;
}