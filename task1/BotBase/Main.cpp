//#include <Windows.h>
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

#define KEY_DOWN(key) (GetAsyncKeyState(key) & 0x8000)
#define MAX_CORNERS 50


std::string path = "D:\\practice\\practice\\vizdoom";
auto game = std::make_unique<vizdoom::DoomGame>();
const unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
auto image = cv::Mat(480, 640, CV_8UC3);
//struct enemy
//{
//	e
//};

void runTask1(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task1.cfg");
		game->setWindowVisible(true);
		game->setRenderWeapon(true);
		game->init();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
	std::vector<double> actions[4];
	auto image = cv::Mat(480, 640, CV_8UC3);
	auto GrayImage = cv::Mat(480, 640, CV_8UC1);
	//action[0] = {1,0,0};
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
			//cout << "Time: " << game->getEpisodeTime() << endl;
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
			//vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			//cv::extractChannel(image, GrayImage, 0);
			//cv::threshold(GrayImage, GrayImage,	130, 255, cv::THRESH_BINARY);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));

			//vector<Point2f> corners;

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
				//	//cout << int(image.at<vec3b>(245, x)[2]) << " ";
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
}

int main()
{
	try
	{
		
		game->setViZDoomPath(path + "\\vizdoom.exe");
		game->setDoomGamePath(path + "\\freedoom2.wad");
		
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}
	//namedWindow("Control Window", WINDOW_AUTOSIZE);
	
	cv::Mat mask(cv::Size(240, 640), CV_8UC1);
	
	auto episodes = 10;
	//===============================
	runTask1(episodes);
	//===============================
	game->close();
}

