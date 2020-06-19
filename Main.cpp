#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

#define COLOR_RED cv::Scalar(0, 0, 255)
#define COLOR_GREEN cv::Scalar(0, 255, 0)
#define COLOR_BLUE cv::Scalar(255, 0, 0)
#define COLOR_YELLOW cv::Scalar(0, 255, 255)
#define COLOR_VIOLET cv::Scalar(255, 255, 0)

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

int thresholdImagePart(cv::Mat input, cv::Mat output, int start_x) {

	assert(start_x + output.cols <= input.cols);

	int points_count = 0;

	for (int i = 0; i < output.cols; i++)
	{
		for (int j = 0; j < output.rows; j++)
		{
			if (std::abs(input.at<cv::Vec3b>(j, i + start_x)[0] - 50) < 10
			&& std::abs(input.at<cv::Vec3b>(j, i + start_x)[1] - 50) < 10
			&& std::abs(input.at<cv::Vec3b>(j, i + start_x)[2] - 50) < 10)
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
	cv::Point max_cluster_center = cv::Point(320, 0);

	auto image = cv::Mat(480, 640, CV_8UC3);
	auto greyscale_right = cv::Mat(400, 320, CV_8UC1);
	auto greyscale_left = cv::Mat(400, 320, CV_8UC1);

	cv::Mat grenade_templ = cv::imread("sprites\\Pickups\\bon1a0.png");

	cv::Mat clusters;

	for (auto a = 0; a < episodes; a++)
	{
		game->newEpisode();
		std::cout << "Episode #" << a + 1 << std::endl;

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

			int points_left = thresholdImagePart(image, greyscale_left, 0);
			int points_right = thresholdImagePart(image, greyscale_right, 320);

			if (points_left > 80000 && points_right > 80000)
			{
				game->makeAction({ 0, 0, 200, 1 });
			}
			else if (points_left > 80000)
			{
				game->makeAction({ 0, 0, 45, 1 });
			}
			else if (points_right > 80000)
			{
				game->makeAction({ 0, 0, -45, 1 });
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
					if (centers[i].y - max_cluster_center.y > 30) max_cluster_center = centers[i];
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
			greyscale_right.convertTo(greyscale_left, CV_32F);
			greyscale_left.convertTo(greyscale_left, CV_8UC1);
			greyscale_right.convertTo(greyscale_right, CV_8UC1);

			cv::Mat result;

			int result_cols = image.cols - grenade_templ.cols + 1;
			int result_rows = grenade_templ.rows - grenade_templ.rows + 1;

			result.create(result_rows, result_cols, CV_32FC1);

			cv::matchTemplate(image, grenade_templ, result, 4);
			cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

			bool on_my_way_woody = false;

			//TODO (DO GRENADE FINDING BETTER)

			if (maxLoc.x > 180 && maxLoc.x < 460 && maxLoc.y > 160 && (max_cluster_center.x - maxLoc.x > 60 || max_cluster_center.y - maxLoc.y > 60))
			{
				on_my_way_woody = true;
				std::cout << "on my way woody" << std::endl;
			}

			rectangle(image, maxLoc, cv::Point(maxLoc.x + grenade_templ.cols, maxLoc.y + grenade_templ.rows), COLOR_RED, 1, 8, 0);
			rectangle(image, cv::Point(180, 160), cv::Point(460, 480), COLOR_YELLOW, 1, 8, 0);

			double err = max_cluster_center.x - 320;
			double p = err * 0.2;
			integral = integral + err * 0.01;
			double u = p + integral;
			actions = { 0, 0, u, 1 };

			if (on_my_way_woody)
			{
				game->makeAction({0, 0, 50, 1});
				actions[2] -= 50;
			}

			if (abs(err) < 30) game->makeAction({ 0,0,0,1 });
			else game->makeAction(actions);

			for (int i = 0; i < centers.size(); i++)
			{
				cv::Point c = centers[i];

				cv::circle(image, c, 5, COLOR_BLUE, -1, 8);
				cv::circle(image, c, 40, COLOR_BLUE);
			}

			cv::circle(image, max_cluster_center, 5, COLOR_GREEN, -1, 8);
			cv::circle(image, max_cluster_center, 40, COLOR_GREEN);

			//for (int i = 0; i < points.size(); i++)
			//{
				//cv::circle(image, points[i], 2, cv::Scalar(0, 255, 0));
			//}

			//cv::imshow("Res", result);
			//cv::imshow("Tmpl", grenade_templ);
			imshow("Game", image);
			cv::moveWindow("Game", 60, 20);
			//imshow("Greyscale right", greyscale_right);
			//imshow("Greyscale left", greyscale_left);
			//cv::moveWindow("Greyscale left", 710, 20);
			//cv::moveWindow("Greyscale right", 1030, 20);

			cv::waitKey(1);
		}
		std::cout << game->getTotalReward() << std::endl;
		total_reward += game->getTotalReward();
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
	runTask4(episodes);
	//===============================

	std::cout << std::endl << "Rewards average: " << total_reward / episodes << std::endl;

	game->close();
}