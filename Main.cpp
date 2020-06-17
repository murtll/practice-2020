//#include <Windows.h>
#include <Windows.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

#define KEY_DOWN(key) (GetAsyncKeyState(key) & 0x8000)
#define MAX_CORNERS 50

double integral = 0;
std::string path = "D:\\practice\\practice\\vizdoom";
auto game = std::make_unique<vizdoom::DoomGame>();
const unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;
auto image = cv::Mat(480, 640, CV_8UC3);
auto ClasteredImage = cv::Mat(480, 640, CV_8UC1);
//auto ClasterCenterImage = cv::Mat(480, 640, CV_8UC1);
//struct enemy
//{
//	e
//};

//void runTask2(int episode)
//{
//	try
//	{
//		game->loadConfig(path + "\\scenarios\\task2.cfg");
//		game->setWindowVisible(true);
//		game->setRenderWeapon(true);
//		game->init();
//	}
//	catch (std::exception & e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	cv::Mat clusters;
//	std::vector<double> actions = {0,0,40,0};
//	auto image = cv::Mat(480, 640, CV_8UC3);
//	auto GrayImage = cv::Mat(480, 640, CV_8UC1);
//	//action[0] = {1,0,0};
//	/*actions[0] = { 1,0,0 };
//	actions[1] = { 0,1,0 };
//	actions[2] = { 0,0,1 };
//	actions[3] = { 0,0,0 };*/
//	//action[1]
//	//action[2]
//	for (auto i = 0; i < episode; i++)
//	{
//		game->newEpisode();
//		std::cout << "Episode #" << i + 1 << std::endl;
//
//		while (!game->isEpisodeFinished())
//		{
//			
//			//ClasteredImage = 0;
//			int center = 0;
//			int k = 0;
//			//cout << "Time: " << game->getEpisodeTime() << endl;
//			const auto& gameState = game->getState();
//			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
//			//vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
//			//cv::extractChannel(image, GrayImage, 0);
//			//cv::threshold(GrayImage, GrayImage,	130, 255, cv::THRESH_BINARY);
//			//game->makeAction(actions[2]);
//			//image.data = screenBuf->data();
//			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
//			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
//
//			//std::vector<cv::Point2f> clasterArr;
//
//			std::vector<cv::Point2f> centers;
//			std::vector<cv::Point2f> points(0);
//			for (int x = 0; x < 640; x++)
//			{
//				for (int y = 0; y < 480; y++)
//				{
//					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
//					{
//						//greyscale.at<unsigned char>(y, x) = 255;
//						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
//						points.push_back(cv::Point2f(x, y));
//						center += x;
//						k++;
//					}
//					else
//					{
//						//greyscale.at<unsigned char>(y, x) = 0;
//					}
//				}
//			}
//
//			//greyscale.convertTo(greyscale, CV_32F);
//
//			//cv::Mat samples = greyscale.reshape(1, greyscale.total());
//			/*int count = 0;
//			for (int x = 0; x < 640; x++)
//			{
//				for (int y = 0; y < 480; y++)
//				{
//					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
//					{
//						points[count] = cv::Point2f(x, y);
//						count++;
//					}
//				}
//			}*/
//			cv::kmeans(points, 1, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);
//
//			//centers = centers.reshape(1, 0);
//			//clusters = clusters.reshape(1, greyscale.rows);
//
//			//greyscale.convertTo(greyscale, CV_8UC3);
//			
//			///*goodFeaturesToTrack(GrayImage, corners, 25, 0.01, 2);
//			//for (int i = 0; i < corners.size(); i++)
//			//{
//			//	cv::circle(image, Point(corners[i].x, corners[i].y), 10, Scalar(255, 255, 255), -1, 8, 0);
//			//	center += corners[i].y;
//			//	k++;
//			//}*/
//			if (k != 0)
//			{
//				center /= k;
//			}
//			//circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
//			cv::Point c = centers[0];
//			cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);
//			////cout << endl;
//			////waitKey(100);
//			////cout << image.at<uchar>(245, 340) << endl;
//			////cout << intensity.val[2] << endl;
//			imshow("Game", image);
//			double tmp = (center - 320)*0.15;
//			actions = { 0, 0, tmp, 0 };
//			if (abs(center - 320) < 30)
//			{
//				actions = { 0,0,0,1 };
//			}
//
//			game->makeAction(actions);
//				
//			cv::waitKey(sleepTime);
//		}
//		std::cout << game->getTotalReward() << std::endl;
//	}
//}

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
	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	//auto clusters = cv::Mat(480, 640, CV_8UC3);
	//auto centers = cv::Mat(480, 640, CV_8UC3);

	cv::Mat clusters;

	actions[0] = { 1,0,0 };
	actions[1] = { 0,1,0 };
	actions[2] = { 0,0,1 };
	actions[3] = { 0,0,0 };
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
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						greyscale.at<unsigned char>(y, x) = 255;
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
						points.push_back(cv::Point2f(x, y));
						center += x;
						k++;
					}
					else
					{
						greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}

			greyscale.convertTo(greyscale, CV_32F);
			
			cv::Mat samples = greyscale.reshape(1, greyscale.total());
			/*int count = 0;
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						points[count] = cv::Point2f(x, y);
						count++;
					}
				}
			}*/
			cv::kmeans(points, 1, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

			//centers = centers.reshape(1, 0);
			//clusters = clusters.reshape(1, greyscale.rows);

			greyscale.convertTo(greyscale, CV_8UC3);

			/*cvKMeans2(&greyscale, 1, &clusters, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));
			cvZero(&clusters);

			for (i = 0; i < 5; i++) {
				CvPoint2D32f pt = ((CvPoint2D32f*) greyscale.data)[i];
				circle(clusters, cvPointFrom32f(pt), 2, cvScalar(0, 255, 0), CV_FILLED);
			}*/

			/*goodFeaturesToTrack(GrayImage, corners, 25, 0.01, 2);
			//for (int i = 0; i < corners.size(); i++)
			//{
			//	cv::circle(image, Point(corners[i].x, corners[i].y), 10, Scalar(255, 255, 255), -1, 8, 0);
			//	center += corners[i].y;
			//	k++;
			//}*/
			/*if (k != 0)
			{
				center /= k;
			}*/

			//circle(image, cv::Point(center, 245), 1, cv::Scalar(0, 0, 255), -1, 10, 0);
			//circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);

			//cv::Point2f c = centers.at<cv::Point2f>(0);
			cv::Point c = centers[0];
			cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);

			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			if ((centers[0].x - 320) > 40)
			{
				game->makeAction(actions[1]);
			}
			else if ((centers[0].x - 320) < -40)
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

		//std::cout << clusters.rows << " " << clusters.cols << std::endl;
		//std::cout << centers << std::endl;
	}
}


//void runTask1(int episode)
//{
//	try
//	{
//		game->loadConfig(path + "\\scenarios\\task1.cfg");
//		game->setWindowVisible(true);
//		game->setRenderWeapon(true);
//		game->init();
//	}
//	catch (std::exception & e)
//	{
//		std::cout << e.what() << std::endl;
//	}
//	std::vector<double> actions[4];
//	auto image = cv::Mat(480, 640, CV_8UC3);
//	auto GrayImage = cv::Mat(480, 640, CV_8UC1);
//	//action[0] = {1,0,0};
//	actions[0] = { 1,0,0 };
//	actions[1] = { 0,1,0 };
//	actions[2] = { 0,0,1 };
//	actions[3] = { 0,0,0 };
//	//action[1]
//	//action[2]
//	for (auto i = 0; i < episode; i++)
//	{
//		game->newEpisode();
//		std::cout << "Episode #" << i + 1 << std::endl;
//
//		while (!game->isEpisodeFinished())
//		{
//			ClasteredImage = 0;
//			//std:vector<cv::Point2f> 
//			int center = 0;
//			int k = 0;
//			//cout << "Time: " << game->getEpisodeTime() << endl;
//			const auto& gameState = game->getState();
//			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
//			//vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
//			//cv::extractChannel(image, GrayImage, 0);
//			//cv::threshold(GrayImage, GrayImage,	130, 255, cv::THRESH_BINARY);
//			//game->makeAction(actions[2]);
//			//image.data = screenBuf->data();
//			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
//			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
//			//cv::kmeans()
//			std::vector<cv::Point> clasters;
//
//			for (int x = 0; x < 640; x++)
//			{
//				for (int y = 0; y < 480; y++)
//				{
//					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
//					{
//						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
//						ClasteredImage.at<unsigned char>(y, x) = 255;
//						center += x;
//						k++;
//					}
//				}
//				//	//cout << int(image.at<vec3b>(245, x)[2]) << " ";
//			}
//			//cvKMeans2(ClasteredImage,1);
//			///*goodFeaturesToTrack(GrayImage, corners, 25, 0.01, 2);
//			//for (int i = 0; i < corners.size(); i++)
//			//{
//			//	cv::circle(image, Point(corners[i].x, corners[i].y), 10, Scalar(255, 255, 255), -1, 8, 0);
//			//	center += corners[i].y;
//			//	k++;
//			//}*/
//			cv::Mat labels;
//			cv::Mat centers(1, 1, CV_8UC1);
//			cv::kmeans(ClasteredImage, 1, labels,
//				cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),3,cv::KMEANS_PP_CENTERS, centers);
//			if (k != 0)
//			{
//				center /= k;
//			}
//			circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);
//			////cout << endl;
//			////waitKey(100);
//			////cout << image.at<uchar>(245, 340) << endl;
//			////cout << intensity.val[2] << endl;
//			imshow("ClasteredImage", ClasteredImage);
//			imshow("Game", image);
//			if ((center - 320) > 30)
//			{
//				game->makeAction(actions[1]);
//			}
//			else if ((center - 320) < -30)
//			{
//				game->makeAction(actions[0]);
//			}
//			else
//			{
//				game->makeAction(actions[2]);
//			}
//			/*if (KEY_DOWN('A'))
//			{
//				game->makeAction(actions[1]);
//			}
//			else if (KEY_DOWN('D'))
//			{
//				game->makeAction(actions[0]);
//			}
//			else if (KEY_DOWN('K'))
//			{
//				game->makeAction(actions[2]);
//			}*/
//			//if (game->getEpisodeTime() < 70 || game->getEpisodeTime() > 200)game->makeAction(actions[0]);
//			//cvKMeans2(,1,)
//			//else game->makeAction(actions[1]);
//			//game->makeAction(actions[2]);
//		//cv::imshow("game", GrayImage);
//			cv::waitKey(sleepTime);
//		}
//		std::cout << game->getTotalReward() << std::endl;
//	}
//}
void runTask2(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task2.cfg");
		game->setWindowVisible(true);
		game->setRenderWeapon(true);
		game->init();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	std::vector<double> actions = {0,0,0,0};

	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	//auto clusters = cv::Mat(480, 640, CV_8UC3);
	//auto centers = cv::Mat(480, 640, CV_8UC3);

	cv::Mat clusters;

	
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
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						greyscale.at<unsigned char>(y, x) = 255;
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
						points.push_back(cv::Point2f(x, y));
						center += x;
						k++;
					}
					else
					{
						greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}

			greyscale.convertTo(greyscale, CV_32F);
			
			cv::Mat samples = greyscale.reshape(1, greyscale.total());
			/*int count = 0;
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						points[count] = cv::Point2f(x, y);
						count++;
					}
				}
			}*/
			cv::kmeans(points, 1, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

			//centers = centers.reshape(1, 0);
			//clusters = clusters.reshape(1, greyscale.rows);

			greyscale.convertTo(greyscale, CV_8UC3);

			/*cvKMeans2(&greyscale, 1, &clusters, cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0));
			cvZero(&clusters);

			for (i = 0; i < 5; i++) {
				CvPoint2D32f pt = ((CvPoint2D32f*) greyscale.data)[i];
				circle(clusters, cvPointFrom32f(pt), 2, cvScalar(0, 255, 0), CV_FILLED);
			}*/

			/*goodFeaturesToTrack(GrayImage, corners, 25, 0.01, 2);
			//for (int i = 0; i < corners.size(); i++)
			//{
			//	cv::circle(image, Point(corners[i].x, corners[i].y), 10, Scalar(255, 255, 255), -1, 8, 0);
			//	center += corners[i].y;
			//	k++;
			//}*/
			/*if (k != 0)
			{
				center /= k;
			}*/

			//circle(image, cv::Point(center, 245), 1, cv::Scalar(0, 0, 255), -1, 10, 0);
			//circle(image, cv::Point(center, 245), 3, cv::Scalar(0, 0, 255), -1, 8, 0);

			//cv::Point2f c = centers.at<cv::Point2f>(0);
			cv::Point c = centers[0];
			cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);

			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			double err = centers[0].x - 320;
			double p = err * 0.2;
			integral = integral + err * 0.01;
			//double i += err * 0.01;
			double u = p + integral;
			actions = { 0, 0, u, 0 };
			if (abs(centers[0].x - 320) < 40)
			{
				actions = { 0,0,0,1 };
			}
			
			game->makeAction(actions);
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

		//std::cout << clusters.rows << " " << clusters.cols << std::endl;
		//std::cout << centers << std::endl;
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
	runTask2(episodes);
	//===============================

	game->close();
}







/*if ((center - 320) > 30)
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
				}*/
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
			//cv::imshow("game", GrayImage);*/