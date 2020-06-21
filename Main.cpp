#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

double total_reward = 0;
std::string path = "..\\vizdoom";
auto game = std::make_unique<vizdoom::DoomGame>();
const unsigned int sleepTime = 1000 / vizdoom::DEFAULT_TICRATE;

void runTask1(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task1.cfg");
		//game->setLabelsBufferEnabled(true);
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
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());

			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						greyscale.at<unsigned char>(y, x) = 255;
						points.push_back(cv::Point2f(x, y));
					}
					else
					{
						greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}

			greyscale.convertTo(greyscale, CV_32F);
			
			cv::Mat samples = greyscale.reshape(1, greyscale.total());

			cv::kmeans(points, 1, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

			greyscale.convertTo(greyscale, CV_8UC3);

			for (int i = 0; i < centers.size(); i++)
			{
				cv::Point c = centers[i];

				cv::circle(image, c, 5, cv::Scalar(0, 0, 255), -1, 8);
				cv::rectangle(image, cv::Rect(c.x - 25, c.y - 25, 50, 50), cv::Scalar(0, 0, 255));
			}

			for (int i = 0; i < points.size(); i++)
			{
				cv::circle(image, points[i], 2, cv::Scalar(0, 255, 0));
			}

			imshow("Game", image);
			imshow("Greyscale", greyscale);
			cv::moveWindow("Game", 60, 20);
			cv::moveWindow("Greyscale", 710, 20);

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

			char c = cv::waitKey(sleepTime);

			//if 'ESC'
			if (c == 27) break;
		}
		std::cout << game->getTotalReward() << std::endl;
		total_reward += game->getTotalReward();
	}
}

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

	double integral = 0;

	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale = cv::Mat(480, 640, CV_8UC1);

	cv::Mat clusters;
	
	for (auto i = 0; i < episode; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		while (!game->isEpisodeFinished())
		{
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());

			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 480; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
					{
						greyscale.at<unsigned char>(y, x) = 255;
						points.push_back(cv::Point2f(x, y));
					}
					else
					{
						greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}

			greyscale.convertTo(greyscale, CV_32F);
			
			cv::Mat samples = greyscale.reshape(1, greyscale.total());
			
			cv::kmeans(points, 1, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

			greyscale.convertTo(greyscale, CV_8UC3);
			
			for (int i = 0; i < centers.size(); i++)
			{
				cv::Point c = centers[i];

				cv::circle(image, c, 5, cv::Scalar(0, 0, 255), -1, 8);
				cv::rectangle(image, cv::Rect(c.x - 25, c.y - 25, 50, 50), cv::Scalar(0, 0, 255));
			}

			for (int i = 0; i < points.size(); i++)
			{
				cv::circle(image, points[i], 2, cv::Scalar(0, 255, 0));
			}

			
			imshow("Game", image);
			imshow("Greyscale", greyscale);
			cv::moveWindow("Game", 60, 20);
			cv::moveWindow("Greyscale", 710, 20);

			double err = centers[0].x - 320;
			double p = err * 0.2;
			integral = integral + err * 0.01;
			double u = p + integral;
			actions = { 0, 0, u, 0 };
			if (abs(centers[0].x - 320) < 40)
			{
				actions = { 0,0,0,1 };
			}
			
			game->makeAction(actions);
			
			cv::waitKey(sleepTime);
		}
		std::cout << game->getTotalReward() << std::endl;
		total_reward += game->getTotalReward();
	}
}

void runTask3(int episodes) {
		try
		{
			game->loadConfig(path + "\\scenarios\\task3.cfg");
			game->setWindowVisible(true);
			game->setLabelsBufferEnabled(true);
			game->setRenderWeapon(true);
			game->init();
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		std::vector<double> actions = { 0,0,0,0 };

		double integral = 0;

		auto image = cv::Mat(480, 640, CV_8UC3);
		auto greyscale = cv::Mat(480, 640, CV_8UC1);

		cv::Mat clusters;

		for (auto i = 0; i < episodes; i++)
		{
			game->newEpisode();
			std::cout << "Episode #" << i + 1 << std::endl;

			while (!game->isEpisodeFinished())
			{
				const auto& gameState = game->getState();
				std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());

				std::vector<cv::Point2f> centers;
				std::vector<cv::Point2f> points(0);
				for (int x = 0; x < 640; x++)
				{
					for (int y = 0; y < 400; y++)
					{
						if (int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[0]) < 50)
						{
							greyscale.at<unsigned char>(y, x) = 255;
							points.push_back(cv::Point2f(x, y));
						}
						else
						{
							greyscale.at<unsigned char>(y, x) = 0;
						}
					}
				}

				greyscale.convertTo(greyscale, CV_32F);

				cv::Mat samples = cv::Mat(points).reshape(1, points.size());

				int K = 5;
				//K = K > 0 ? K : 1;
				if (points.size() > K)
				{
					cv::kmeans(samples, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 10), 3, cv::KMEANS_RANDOM_CENTERS, centers);
				}
				else
				{
					game->makeAction({ 0, 0, 180, 1 });
				}

				greyscale.convertTo(greyscale, CV_8UC3);

				std::vector<int> cluster_counts(K, 0);

				for (int y = 0; y < clusters.total(); y++)
				{
					int cluster_number = clusters.at<int>(y);
					cluster_counts[cluster_number]++;
				}

				cv::Point max_cluster_center;
				int tmp = 0;

				for (int i = 0; i < cluster_counts.size(); i++)
				{
					if (cluster_counts[i] > tmp) {
						if (centers.size() > 0)
						{
							tmp = cluster_counts[i];
							max_cluster_center = centers[i];
						}
						else
						{
							game->makeAction({ 0, 0, 180, 0 });
						}
					}
				}

				for (int i = 0; i < centers.size(); i++)
				{
					cv::Point c = centers[i];

					cv::circle(image, c, 5, cv::Scalar(0, 0, 255), -1, 8);
					cv::rectangle(image, cv::Rect(c.x - 25, c.y - 25, 50, 50), cv::Scalar(0, 0, 255));
				}

				cv::circle(image, max_cluster_center, 5, cv::Scalar(255, 255, 0), -1, 8);
				cv::rectangle(image, cv::Rect(max_cluster_center.x - 25, max_cluster_center.y - 25, 50, 50), cv::Scalar(255, 255, 0));

				for (int i = 0; i < points.size(); i++)
				{
					cv::circle(image, points[i], 2, cv::Scalar(0, 255, 0));
				}


				imshow("Game", image);
				imshow("Greyscale", greyscale);
				cv::moveWindow("Game", 60, 20);
				cv::moveWindow("Greyscale", 710, 20);

				double err = max_cluster_center.x - 320;
				double p = err * 0.2;
				integral = integral + err * 0.01;
				double u = p + integral;
				actions = { 0, 0, u, 1 };
				if (abs(err) < 60)
				{
					actions = { 0,0,0,1 };
				}

				game->makeAction(actions);

				cv::waitKey(sleepTime);
			}
			std::cout << game->getTotalReward() << std::endl;
			total_reward += game->getTotalReward();
		}

}

void runTask5V3(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task5.cfg");
		game->setWindowVisible(true);
		game->setRenderWeapon(true);
		game->init();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

	std::vector<double> actions = { 0,0,0,0 };

	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale = cv::Mat(480, 640, CV_8UC1);
	auto connectedComponentsImage = cv::Mat(480, 640, CV_8UC1);
	//auto clusters = cv::Mat(480, 640, CV_8UC3);
	//auto centers = cv::Mat(480, 640, CV_8UC3);

	cv::Mat clusters;

	std::vector<cv::Point> lastCentersPosition(5);
	std::vector<bool> justCreated(5);

	for (int clust = 0; clust < 5; clust++)
	{
		justCreated[clust] = false;
	}
	for (auto i = 0; i < episode; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;
		int lastAction = 0;
		int ultraLastAction = 0;
		int time = 0;
		int centerOld = 320;
		for (int x = 0; x < 640; x++)
		{
			for (int y = 0; y < 480; y++)
			{
				connectedComponentsImage.at<unsigned char>(y, x) = 0;
			}
		}
		std::vector<std::vector<double>> koff(5, std::vector<double>(2));
		bool avoidObj = false;
		int avoidCounter = 0;
		while (!game->isEpisodeFinished())
		{
			int center = 0;
			int k = 0;
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
			vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			cv::extractChannel(image, greyscale, 1);
			cv::threshold(greyscale, greyscale, 130, 255, cv::THRESH_BINARY);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 380; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[1]) > 150 && int(image.at<cv::Vec3b>(y, x)[0]) > 130)
					{
						connectedComponentsImage.at<unsigned char>(y, x) = 255;
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 0), 1, 8, 0);
						points.push_back(cv::Point2f(x, y));

						//center += x;
						//k++;
					}
					else
					{
						connectedComponentsImage.at<unsigned char>(y, x) = 0;
					}
				}
			}

			for (int x = 0; x < 640; x++)
			{
				for (int y = 175; y < 200; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[2]) > 200 && int(image.at<cv::Vec3b>(y, x)[1]) < 10)
					{
						//greyscale.at<unsigned char>(y, x) = 255;
						circle(image, cv::Point(x, y), 1, cv::Scalar(0, 255, 255), 1, 8, 0);
						//points.push_back(cv::Point2f(x, y));
						center += x;
						k++;
					}
					else
					{
						//greyscale.at<unsigned char>(y, x) = 0;
					}
				}
			}


			if (k != 0)center /= k;
			circle(image, cv::Point(center, 245), 1, cv::Scalar(0, 0, 0), -1, 10, 0);
			circle(image, cv::Point(centerOld, 245), 3, cv::Scalar(255, 255, 255), -1, 8, 0);
			if (abs(center - centerOld) > 80)
			{
				center = centerOld;
			}
			centerOld = center;

			//time = game->getEpisodeTime();
			greyscale.convertTo(greyscale, CV_32F);
			//std::cout << image.at<cv::Vec3b>(50, 320) << std::endl;
			//cv::Mat samples = greyscale.reshape(1, greyscale.total());
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
			if (points.size() > 5)cv::kmeans(points, 5, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_PP_CENTERS, centers);

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
			if (points.size() > 5)
			{
				cv::Point c = centers[0];
				cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);
			}
			std::vector<cv::Point> trueCenters;
			if (points.size() > 5)
			{
				for (int clust1 = 0; clust1 < centers.size(); clust1++)
				{
					if (centers[clust1].x != 1337)
					{
						cv::Point baseCenter;
						baseCenter.x = centers[clust1].x;
						baseCenter.y = centers[clust1].y;
						int mergedCenters = 1;
						for (int clust2 = clust1; clust2 < centers.size(); clust2++)
						{
							if (abs(centers[clust1].x - centers[clust2].x) < 40 && abs(centers[clust1].y - centers[clust2].y) < 40 && clust1 != clust2)
							{

								baseCenter.x += centers[clust2].x;
								baseCenter.y += centers[clust2].y;
								centers[clust2].x = 1337;
								mergedCenters++;

							}
						}
						baseCenter.x /= mergedCenters;
						baseCenter.y /= mergedCenters;
						trueCenters.push_back(baseCenter);
					}
				}
			}

			for (int clust = 0; clust < trueCenters.size(); clust++)
			{
				if (justCreated[clust] == true)
				{
					if (trueCenters[clust].y > lastCentersPosition[clust].y)
					{
						if (lastCentersPosition[clust].x - trueCenters[clust].x)
						{
							koff[clust][0] = (lastCentersPosition[clust].y - trueCenters[clust].y) / (lastCentersPosition[clust].x - trueCenters[clust].x);
						}
						else
						{
							if (!justCreated[clust])koff[clust][0] = 0;
						}
						koff[clust][1] = lastCentersPosition[clust].y - koff[clust][0] * lastCentersPosition[clust].x;
					}
				}
			}
			int tmpX = 0;
			for (int clust = 0; clust < trueCenters.size(); clust++)
			{
				if (justCreated[clust] == true)
				{
					if (trueCenters[clust].y > lastCentersPosition[clust].y)
					{
						if (koff[clust][0] != 0)
						{
							int tmpY = 479 - trueCenters[clust].y;
							tmpX = (tmpY - koff[clust][1]) / koff[clust][0];
						}
					}
				}
				else
				{
					justCreated[clust] = true;
				}
			}
			for (int clust = 0; clust < trueCenters.size(); clust++)
			{
				lastCentersPosition[clust] = trueCenters[clust];
			}
			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			for (int clust = 0; clust < trueCenters.size(); clust++)
			{
				if (koff[clust][0] != 0)
				{
					line(image, cv::Point(trueCenters[clust].x, trueCenters[clust].y), cv::Point(tmpX, 479), cv::Scalar(100, 255, 100), 2);
				}
			}
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			imshow("Threshold", greyscale);
			if (avoidObj == false)
			{
				if (points.size() > 5)
				{
					for (int fireballs = 0; fireballs < trueCenters.size(); fireballs++)
					{
						if (trueCenters[fireballs].y > 230)
						{
							int  traectX, traectY = 479 - trueCenters[fireballs].y;
							traectX = (traectY - koff[fireballs][1]) / koff[fireballs][0];
							if (trueCenters[fireballs].x - 320 > 0)
							{
								if (center - centers[fireballs].x < 0)
								{
									ultraLastAction = LEFT;
									time = game->getEpisodeTime();
								}
								else
								{
									ultraLastAction = RIGHT;
									time = game->getEpisodeTime();
								}
							}
							else if (trueCenters[fireballs].x - 320 < 0)
							{
								if (center - centers[fireballs].x > 0)
								{
									ultraLastAction = RIGHT;
									time = game->getEpisodeTime();
								}
								else
								{
									ultraLastAction = LEFT;
									time = game->getEpisodeTime();
								}
							}
							else
							{
								actions = { 0,0,0,0 };
							}
						}
					}
				}
				else
				{
					if (center - 320 > 70)
					{
						actions = { 0,1,0,0 };
					}
					else if (center - 320 < -70)
					{
						actions = { 1,0,0,0 };
					}
					else
					{
						actions = { 0,0,0,0 };
					}
				}
			}
			if (ultraLastAction == NONE)
			{
				game->makeAction(actions);
			}
			else
			{
				avoidObj = true;
				if (ultraLastAction == RIGHT)
				{
					actions = { 0,1,0,0 };
					game->makeAction(actions);
				}
				else
				{
					actions = { 1,0,0,0 };
					game->makeAction(actions);
				}
				if (game->getEpisodeTime() - time > 8)
				{
					avoidObj = false;
					ultraLastAction = NONE;
				}
			}
			//clusters.

			//lastAction = 
			//centerOld = centers[0].x;
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
	runTask3(episodes);
	//===============================

	std::cout << std::endl << "Rewards average: " << total_reward / episodes << std::endl;

	game->close();
}