#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

std::string path = "C:\\practice\\vizdoom";
vizdoom::DoomGame* game = new vizdoom::DoomGame();
unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
auto screenBuf = cv::Mat(480, 640, CV_8UC3);

void run_task_1(int episodes) {

	try
	{
		game->loadConfig(path + "\\scenarios\\task1.cfg");
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	auto greyscale = cv::Mat(480, 640, CV_8UC1);

	std::vector<int> rewards;

	std::vector<double> actions[3];
	actions[0] = { 1, 0, 0 };
	actions[1] = { 0, 1, 0 };
	actions[2] = { 0, 0, 1 };

	for (auto i = 0; i < episodes; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished())
		{
			const auto& gamestate = game->getState();

			std::memcpy(screenBuf.data, gamestate->screenBuffer->data(), gamestate->screenBuffer->size());

			cv::extractChannel(screenBuf, greyscale, 0);

			cv::threshold(greyscale, greyscale, 130, 255, cv::THRESH_BINARY);

			cv::imshow("Control Window", greyscale);
			cv::waitKey(sleepTime);

			//double reward = game->makeAction(actions[2]);
		}

		std::cout << game->getTotalReward() << std::endl;

		rewards.push_back(game->getTotalReward());

	}

	int average = 0;
	for (auto i : rewards) {
		average += i;
	}
	std::cout << average / episodes;
};

int main()
{
	try
	{
		game->setViZDoomPath(path + "\\vizdoom.exe");
		game->setDoomGamePath(path + "\\freedoom2.wad");
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	cv::namedWindow("Control Window", cv::WINDOW_AUTOSIZE);

	auto episodes = 10;

	//======================
	run_task_1(episodes);
	//======================

	game->close();
	delete game;
}