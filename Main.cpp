#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

std::string path = "C:\\practice\\vizdoom";
vizdoom::DoomGame* game = new vizdoom::DoomGame();
unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
auto screenBuf = cv::Mat(480, 640, CV_8UC3);

//task 2
void run_task_2(int episodes)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task2.cfg");
		game->setWindowVisible(true);
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::vector<double> action = { 0 ,0, 40, 0 };
	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	for (auto i = 0; i < episodes; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished())
		{
			int center = 0;
			int k = 0;

			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());

			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);

						center += x;
						k++;
					}
				}
			}

			if (k != 0)
			{
				center /= k;
			}

			circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
			imshow("Game", image);
			double tmp = (center - 320) * 0.15;
			action = { 0, 0, tmp, 0 };

			if (abs(center - 320) < 30)
			{
				action = { 0,0,0,1 };
			}

			game->makeAction(action);

			cv::waitKey(sleepTime);
		}
		std::cout << game->getTotalReward() << std::endl;
	}
};

//task 1
void runTask1(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task1.cfg");
		game->setWindowVisible(true);
		game->setRenderWeapon(true);
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	std::vector<double> actions[4];
	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	auto clusters = cv::Mat(480, 640, CV_8UC3);

	actions[0] = { 1,0,0 };
	actions[1] = { 0,1,0 };
	actions[2] = { 0,0,1 };
	actions[3] = { 0,0,0 };
	//action[1]
	//action[2]
	for (auto i = 0; i < episode; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished())
		{

			int center = 0;
			int k = 0;
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
			//vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			//cv::extractChannel(image, GrayImage, 0);
			//cv::threshold(GrayImage, GrayImage,	130, 255, cv::THRESH_BINARY);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));

			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						greyscale.at<unsigned char>(y, x) = 255;
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
						center += x;
						k++;
					} 
					else
					{
						greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}

			cvKMeans2(&greyscale, 1, &clusters, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));
			cvZero(&clusters);

			for (i = 0; i < 5; i++) {
				CvPoint2D32f pt = ((CvPoint2D32f*) greyscale.data)[i];
				circle(clusters, cvPointFrom32f(pt), 2, cvScalar(0, 255, 0), CV_FILLED);
			}

			///*goodFeaturesToTrack(GrayImage, corners, 25, 0.01, 2);
			//for (int i = 0; i < corners.size(); i++)
			//{
			//	cv::circle(image, Point(corners[i].x, corners[i].y), 10, Scalar(255, 255, 255), -1, 8, 0);
			//	center += corners[i].y;
			//	k++;
			//}*/
			if (k != 0)
			{
				center /= k;
			}
			circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			imshow("Clusters", clusters);
			imshow("Game", image);
			if ((center - 320) > 30)
			{
				game->makeAction(actions[1]);
			}
			else if ((center - 320) < -30)
			{
				game->makeAction(actions[0]);
			}
			else
			{
				game->makeAction(actions[2]);
			}
			/*if (KEY_DOWN('A'))
			{
				game->makeAction(actions[1]);
			}
			else if (KEY_DOWN('D'))
			{
				game->makeAction(actions[0]);
			}
			else if (KEY_DOWN('K'))
			{
				game->makeAction(actions[2]);
			}*/
			//if (game->getEpisodeTime() < 70 || game->getEpisodeTime() > 200)game->makeAction(actions[0]);
			//else game->makeAction(actions[1]);
			//game->makeAction(actions[2]);
		//cv::imshow("game", GrayImage);
			cv::waitKey(sleepTime);
		}
		std::cout << game->getTotalReward() << std::endl;
	}
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

	auto episodes = 10;

	//======================
	//runTask1(episodes);
	run_task_2(episodes);
	//======================

	game->close();
	delete game;
}