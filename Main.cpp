#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

#define COLOR_RED cv::Scalar(0, 0, 255)
#define COLOR_GREEN cv::Scalar(0, 255, 0)
#define COLOR_BLUE cv::Scalar(255, 0, 0)
#define COLOR_YELLOW cv::Scalar(0, 255, 255)
#define COLOR_VIOLET cv::Scalar(255, 0, 255)
#define COLOR_LIGHT_BLUE cv::Scalar(255, 255, 0)
#define LEFT 1
#define RIGHT -1
#define NONE 0

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
			
			int reward = game->makeAction(actions);

			std::cout << reward << std::endl;

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
							//greyscale.at<unsigned char>(y, x) = 255;
							points.push_back(cv::Point2f(x, y));
						}
						else
						{
							//greyscale.at<unsigned char>(y, x) = 0;
						}
					}
				}

				greyscale.convertTo(greyscale, CV_32F);

				//cv::Mat samples = cv::Mat(points).reshape(1, points.size());

				int K = 5;
				//K = K > 0 ? K : 1;
				if (points.size() > K)
				{
					cv::kmeans(points, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 10), 3, cv::KMEANS_RANDOM_CENTERS, centers);
				}
				else
				{
					game->makeAction({ 0, 0, 180, 1 });
				}

				greyscale.convertTo(greyscale, CV_8UC1);

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
				//imshow("Greyscale", greyscale);
				cv::moveWindow("Game", 60, 20);
				//cv::moveWindow("Greyscale", 710, 20);

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

				cv::waitKey(1);
			}
			std::cout << game->getTotalReward() << std::endl;
			total_reward += game->getTotalReward();
		}

}

//returns count of points in cv::Mat that satisfy the requirements
int thresholdImagePart(cv::Mat input, /*cv::Mat output,*/ cv::Point start/*, cv::Point end*/) {

	cv::Mat output = cv::Mat(130, 320, CV_8UC1);

	//assert(start_x + output.cols <= input.cols);

	int points_count = 0;

	for (int i = 0; i < output.cols; i++)
	{
		for (int j = 0; j < output.rows; j++)
		{
			if (std::abs(input.at<cv::Vec3b>(j + start.y, i + start.x)[0] - 50) < 10
			&& std::abs(input.at<cv::Vec3b>(j + start.y, i + start.x)[1] - 50) < 10
			&& std::abs(input.at<cv::Vec3b>(j + start.y, i + start.x)[2] - 50) < 10)
			{
				//output.at<unsigned char>(j, i) = 255;
				points_count++;
			}
			else
			{
				//output.at<unsigned char>(j, i) = 0;
			}
		}
	}

	return points_count;
}

void runTask4(int episodes) {
	try
	{
		game->loadConfig(path + "\\scenarios\\task4.cfg");
		game->setWindowVisible(false);
		//game->setLabelsBufferEnabled(true);
		//game->setRenderWeapon(true);
		game->init();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	std::vector<double> actions = { 0,0,0,0 };

	double integral = 0;
	double integral1 = 0;
	int wall_points = 25000;
	cv::Point max_cluster_center = cv::Point(320, 0);

	auto image = cv::Mat(480, 640, CV_8UC3);

	cv::Mat grenade_templ = cv::imread("sprites\\Pickups\\bon1b0.png");

	cv::Mat clusters;

	for (auto a = 0; a < episodes; a++)
	{
		game->newEpisode();
		std::cout << "Episode #" << a + 1 << std::endl;

		while (!game->isEpisodeFinished())
		{

			auto greyscale_right = cv::Mat(130, 320, CV_8UC1);
			auto greyscale_left = cv::Mat(130, 320, CV_8UC1);

			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());

			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);

			for (int x = 0; x < 640; x++)
			{
				for (int y = 0; y < 400; y++)
				{
					if (int(image.at<cv::Vec3b>(y, x)[1]) > 160
						&& int(image.at<cv::Vec3b>(y, x)[0]) > 90
						&& int(image.at<cv::Vec3b>(y, x)[2]) > 90
						&& int(image.at<cv::Vec3b>(y, x)[0]) < 150
						&& int(image.at<cv::Vec3b>(y, x)[2]) < 150)
					{
						points.push_back(cv::Point2f(x, y));
					}
				}
			}

			int points_left = (points.size() < 1) ? thresholdImagePart(image, /*greyscale_left,*/ cv::Point(0, 130)) : 0;
			int points_right = (points.size() < 1) ? thresholdImagePart(image, /*greyscale_right,*/ cv::Point(320, 130)) : 0;

			//std::cout << "Right: " << points_right << std::endl;
			//std::cout << "Left: " << points_left << std::endl;

			if (points_left > wall_points && points_right > wall_points)
			{
				game->makeAction({ 0, 0, 80, 1 });
			}
			if (points_left > wall_points)
			{
				game->makeAction({ 0, 0, 20, 1 });
			}
			else if (points_right > wall_points)
			{
				game->makeAction({ 0, 0, -20, 1 });
			}

			int K = 3;
			if (points.size() > K)
			{
				cv::kmeans(points, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 10), 3, cv::KMEANS_RANDOM_CENTERS, centers);
			}
			else if (points.size() > 0)
			{
				K = 1;
				cv::kmeans(points, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 100, 10), 3, cv::KMEANS_RANDOM_CENTERS, centers);
			}

			max_cluster_center.x = 320;

			if (centers.size() > 0)
			{
				for (int i = 0; i < centers.size(); i++)
				{
					if (centers[i].y - max_cluster_center.y > 30 
						|| (centers[i].y > max_cluster_center.y && std::abs(centers[i].x - max_cluster_center.x) < 30)) 
					{
						max_cluster_center = centers[i];
					}
				}
			}
			else
			{
				max_cluster_center.y = 0;
			}

			int tmp = 0;
			for (int i = 0; i < centers.size(); i++)
			{
				tmp = std::max(tmp, (int)centers[i].y);
			}
			max_cluster_center.y = std::min(tmp, max_cluster_center.y);

			greyscale_left.convertTo(greyscale_left, CV_32F);
			greyscale_right.convertTo(greyscale_right, CV_32F);
			greyscale_left.convertTo(greyscale_left, CV_8UC1);
			greyscale_right.convertTo(greyscale_right, CV_8UC1);

			if (points_left - points_right > 40000 && points.size() == 0)
			{
				actions = { 0, 0, 40, 1 };
			}
			else if (points_right - points_left > 40000 && points.size() == 0)
			{
				actions = { 0, 0, -40, 1 };
			}

			for (int i = 0; i < centers.size(); i++)
			{
				cv::Point c = centers[i];

				//cv::circle(image, c, 5, COLOR_BLUE, -1, 8);
				//cv::circle(image, c, 40, COLOR_BLUE);
				cv::putText(image, "AID", cv::Point(c.x - 20, c.y - 30), 1, 1, COLOR_GREEN);
				cv::rectangle(image, cv::Point(c.x - 20, c.y - 30), cv::Point(c.x + 40, c.y + 30), COLOR_GREEN, 1);
			}

			//cv::circle(image, max_cluster_center, 5, COLOR_GREEN, -1, 8);
			//cv::circle(image, max_cluster_center, 40, COLOR_GREEN);
			//cv::putText(image, "AID", cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), 1, 1, COLOR_GREEN);
			//cv::rectangle(image, cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), cv::Point(max_cluster_center.x + 40, max_cluster_center.y + 30), COLOR_GREEN, -1);

			cv::Mat result;

			int result_cols = image.cols - grenade_templ.cols + 1;
			int result_rows = grenade_templ.rows - grenade_templ.rows + 1;

			result.create(result_rows, result_cols, CV_32FC1);

			cv::matchTemplate(image, grenade_templ, result, 4);
			//cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

			double minVal, maxVal;
			cv::Point minLoc, maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

			cv::putText(image, "grenade", maxLoc, 1, 1, cv::Scalar(0, 0, maxVal * 0.000028));
			//cv::putText(image, "grenade visor", cv::Point(140, 200), 1, 1, COLOR_YELLOW);

			rectangle(image, maxLoc, cv::Point(maxLoc.x + grenade_templ.cols, maxLoc.y + grenade_templ.rows), cv::Scalar(0, 0, maxVal * 0.000028));
			//rectangle(image, cv::Point(80, 200), cv::Point(560, 400), COLOR_YELLOW);

			double err = max_cluster_center.x - 320;
			if (points.size() > 0)
			{
				double p = err * 0.2;
				integral = integral + err * 0.01;
				double u = p + integral;
				actions = { 0, 0, u, 1 };
			}

			if (abs(err) < 20) actions = { 0,0,0,1 };

			//TODO (DO GRENADE FINDING BETTER)

			if (maxLoc.x > 80 && maxLoc.x < 560 && maxLoc.y > 200 && maxLoc.y < 400
				&& maxVal >= 9100000
				&& maxLoc.y > max_cluster_center.y
				//&& !(game->getGameVariable(vizdoom::HEALTH) > 40 && std::abs(max_cluster_center.x - maxLoc.x) < 20 || std::abs(max_cluster_center.y - maxLoc.y) < 20)
				//&& (std::abs(max_cluster_center.x - maxLoc.x) > 50 || std::abs(max_cluster_center.y - maxLoc.y) > 50)
				)
			{

				if (maxLoc.x <= 300)
				{
					if (points_right > wall_points + 20000)
					{
						double err1 = maxLoc.x - 600;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						//std::cout << "Tic: " << gameState->tic << " : on my way woody left and wall right -> turn left" << std::endl;
					}
					else {
						double err1 = maxLoc.x - 40;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						//std::cout << "Tic: " << gameState->tic << " : on my way woody left -> turn right" << std::endl;
					}
				}
				else
				{
					if (points_left > wall_points + 20000)
					{
						double err1 = maxLoc.x - 40;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						//std::cout << "Tic: " << gameState->tic << " : on my way woody right and wall left -> turn right" << std::endl;
					}
					else {
						double err1 = maxLoc.x - 600;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						//std::cout << "Tic: " << gameState->tic << " : on my way woody right -> turn left" << std::endl;
					}
				}

			}

			game->makeAction(actions);

			cv::imshow("Game", image);
			cv::moveWindow("Game", 60, 20);
			//cv::imshow("Left", greyscale_left);
			//cv::moveWindow("Left", 710, 20);
			//cv::imshow("Right", greyscale_right);
			//cv::moveWindow("Right", 1030, 20);

			cv::waitKey(1);
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
	catch (std::exception& e)
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
		total_reward += game->getTotalReward();

		//std::cout << clusters.rows << " " << clusters.cols << std::endl;
		//std::cout << centers << std::endl;
	}
}

void runTask7(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task7.cfg");
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
	//auto clusters = cv::Mat(480, 640, CV_8UC3);
	//auto centers = cv::Mat(480, 640, CV_8UC3);

	cv::Mat clusters;


	for (auto i = 0; i < episode; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;

		double integral = 0;

		while (!game->isEpisodeFinished())
		{
			int center = 0;
			int k = 0;
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
			vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			cv::extractChannel(image, greyscale, 0);
			cv::threshold(greyscale, greyscale,	175, 255, cv::THRESH_BINARY);
			imshow("LOL", greyscale);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 290; x < 350; x++)
			{
				for (int y = 0; y < 300; y++)
				{
					if ((int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[2]) < 150 && int(image.at<cv::Vec3b>(y, x)[0]) < 50 && int(image.at<cv::Vec3b>(y, x)[0]) > 30) || (int(image.at<cv::Vec3b>(y, x)[0]) > 150 && int(image.at<cv::Vec3b>(y, x)[0]) < 200 && int(image.at<cv::Vec3b>(y, x)[1]) > 100))
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
			/*cv::rectangle(image,))*/
			greyscale.convertTo(greyscale, CV_32F);
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
			if(points.size() > 1)cv::kmeans(points, 2, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

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
			if (points.size() > 1) 
			{
				cv::Point c = centers[0];
				cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);
			}

			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			bool shoot = false;
			if (points.size() > 1) 
			{
				for (int clust = 0; clust < centers.size(); clust++)
				{
					if (centers[clust].y > 240)
					{
						double err = centers[clust].x - 320;
						double p = err * 0.1;
						integral = integral + err * 0.005;
						//double i += err * 0.01;
						double u = p + integral;
						actions = { 0, 0, u, 0 };
						if (abs(centers[clust].x - 320) < 10)
						{
							actions = { 0,0,0,1 };
							shoot = true;
						}
					}
					else if (shoot != true)
					{
						//actions = { 1,0,0,0 };
					}
				}
			}
			else
			{
				actions = { 0,0,-10,0 };
			}
			//clusters.
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

void runTask7V2(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task7.cfg");
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
	//auto clusters = cv::Mat(480, 640, CV_8UC3);
	//auto centers = cv::Mat(480, 640, CV_8UC3);

	cv::Mat clusters;

	for (auto i = 0; i < episode; i++)
	{
		game->newEpisode();
		std::cout << "Episode #" << i + 1 << std::endl;
		double integral = 0;

		while (!game->isEpisodeFinished())
		{
			int center = 0;
			int k = 0;
			const auto& gameState = game->getState();
			std::memcpy(image.data, gameState->screenBuffer->data(), gameState->screenBuffer->size());
			vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			cv::extractChannel(image, greyscale, 0);
			cv::threshold(greyscale, greyscale,	175, 255, cv::THRESH_BINARY);
			imshow("LOL", greyscale);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 290; x < 350; x++)
			{
				for (int y = 0; y < 300; y++)
				{
					if ((int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[2]) < 150 && int(image.at<cv::Vec3b>(y, x)[0]) < 50 && int(image.at<cv::Vec3b>(y, x)[0]) > 30) || (int(image.at<cv::Vec3b>(y, x)[0]) > 150 && int(image.at<cv::Vec3b>(y, x)[0]) < 200 && int(image.at<cv::Vec3b>(y, x)[1]) > 100))
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
			/*cv::rectangle(image,))*/
			greyscale.convertTo(greyscale, CV_32F);
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
			if(points.size() > 1)cv::kmeans(points, 2, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

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
			if (points.size() > 1) 
			{
				cv::Point c = centers[0];
				cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);
			}

			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			bool shoot = false;
			if (points.size() > 1) 
			{
				for (int clust = 0; clust < centers.size(); clust++)
				{
					if (centers[clust].y > 240)
					{
						double err = centers[clust].x - 320;
						double p = err * 0.1;
						integral = integral + err * 0.005;
						//double i += err * 0.01;
						double u = p + integral;
						actions = { 0, 0, u, 0 };
						if (abs(centers[clust].x - 320) < 10)
						{
							actions = { 0,0,0,1 };
							shoot = true;
						}
					}
					else if (shoot != true)
					{
						//actions = { 1,0,0,0 };
					}
				}
			}
			else
			{
				actions = { 0,0,-10,0 };
			}
			//clusters.
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

void runTask7V3(int episode)
{
	try
	{
		game->loadConfig(path + "\\scenarios\\task7.cfg");
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
			vizdoom::BufferPtr screenBuf = gameState->screenBuffer;
			cv::extractChannel(image, greyscale, 0);
			cv::threshold(greyscale, greyscale,	175, 255, cv::THRESH_BINARY);
			imshow("LOL", greyscale);
			//game->makeAction(actions[2]);
			//image.data = screenBuf->data();
			//cvtColor(image, GrayImage, cv::COLOR_BGR2GRAY);
			//line(image, Point(0, 245), Point(639, 245), Scalar(0, 0, 200));
			std::vector<cv::Point2f> centers;
			std::vector<cv::Point2f> points(0);
			for (int x = 290; x < 350; x++)
			{
				for (int y = 0; y < 300; y++)
				{
					if ((int(image.at<cv::Vec3b>(y, x)[2]) > 130 && int(image.at<cv::Vec3b>(y, x)[2]) < 150 && int(image.at<cv::Vec3b>(y, x)[0]) < 50 && int(image.at<cv::Vec3b>(y, x)[0]) > 30) || (int(image.at<cv::Vec3b>(y, x)[0]) > 140 && int(image.at<cv::Vec3b>(y, x)[0]) < 200 && int(image.at<cv::Vec3b>(y, x)[1]) > 100))
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
			/*cv::rectangle(image,))*/
			greyscale.convertTo(greyscale, CV_32F);
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
			if(points.size() > 1)cv::kmeans(points, 2, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1.0), 3, cv::KMEANS_RANDOM_CENTERS, centers);

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
			if (points.size() > 1) 
			{
				cv::Point c = centers[0];
				cv::circle(image, c, 5, cv::Scalar(255, 0, 255), -1, 8);
			}

			/*cv::Point sdsaf = centers[1];
			cv::circle(image, sdsaf, 5, cv::Scalar(255, 255, 255), -1, 8);*/
			////cout << endl;
			////waitKey(100);
			////cout << image.at<uchar>(245, 340) << endl;
			////cout << intensity.val[2] << endl;
			//imshow("Clusters", clusters);
			imshow("Game", image);
			bool shoot = false;
			if (points.size() > 1) 
			{
				for (int clust = 0; clust < centers.size(); clust++)
				{
					if (centers[clust].y > 235)
					{
						double err = centers[clust].x - 320;
						double p = err * 0.1;
						integral = integral + err * 0.005;
						//double i += err * 0.01;
						double u = p + integral;
						actions = { 0, 0, u, 0 };
						if (abs(centers[clust].x - 320) < 3)
						{
							actions = { 0,0,0,1 };
							shoot = true;
						}
					}
					else if (shoot != true)
					{
						//actions = { 1,0,0,0 };
					}
				}
			}
			else
			{
				actions = { 0,0,-15,0 };
			}
			//clusters.
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
	
	auto episodes = 10;
	//===============================
	runTask7V2(episodes);
	//===============================

	std::cout << std::endl << "Rewards average: " << total_reward / episodes << std::endl;

	game->close();
}