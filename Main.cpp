#include <iostream>
#include <opencv2/opencv.hpp>
#include <ViZDoom.h>

#define COLOR_RED cv::Scalar(0, 0, 255)
#define COLOR_GREEN cv::Scalar(0, 255, 0)
#define COLOR_BLUE cv::Scalar(255, 0, 0)
#define COLOR_YELLOW cv::Scalar(0, 255, 255)
#define COLOR_VIOLET cv::Scalar(255, 0, 255)
#define COLOR_LIGHT_BLUE cv::Scalar(255, 255, 0)

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
int thresholdImagePart(cv::Mat input, /*cv::Mat output,*/ int start_x) {

	//assert(start_x + output.cols <= input.cols);

	int points_count = 0;

	for (int i = 0; i < 320 /*output.cols*/; i++)
	{
		for (int j = 0; j < 480 /*output.rows*/; j++)
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

	//when points of wall become more than this number (and bot doesnt see any AIDs),
	//bot turns right or left
	int wall_points = 75000;
	cv::Point max_cluster_center = cv::Point(320, 0);

	auto image = cv::Mat(480, 640, CV_8UC3);

	cv::Mat grenade_templ = cv::imread("sprites\\Pickups\\bon1b0.png");

	cv::Mat clusters;

	for (auto a = 0; a < episodes; a++)
	{
		game->newEpisode();
		std::cout << "Episode #" << a + 1 << std::endl;

		bool was_in_last_tic_grenade_left = false;
		bool was_in_last_tic_grenade_right = false;

		while (!game->isEpisodeFinished())
		{

			auto greyscale_right = cv::Mat(400, 320, CV_8UC1);
			//auto greyscale_left = cv::Mat(400, 320, CV_8UC1);

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

			//getting points of wall in left side
			int points_left = thresholdImagePart(image, /*greyscale_left,*/ 0);
			//getting points of wall in right side
			int points_right = thresholdImagePart(image, /*greyscale_right,*/ 320);

			//std::cout << points_right << std::endl;

			//turning if walls are close to bot and bot doesnt see AIDs
			if (points_left > wall_points && points_right > wall_points && points.size() < 10)
			{
				game->makeAction({ 0, 0, 80, 1 });
			}
			if (points_left > wall_points && points.size() < 10 && !was_in_last_tic_grenade_right)
			{
				game->makeAction({ 0, 0, 20, 1 });
			}
			else if (points_right > wall_points && points.size() < 10 && !was_in_last_tic_grenade_left)
			{
				game->makeAction({ 0, 0, -20, 1 });
			}

			int K = 3;
			if (points.size() > K)
			{
				cv::kmeans(points, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1), 1, cv::KMEANS_RANDOM_CENTERS, centers);
			}
			else if (points.size() > 0)
			{
				K = 1;
				cv::kmeans(points, K, clusters, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 1), 1, cv::KMEANS_RANDOM_CENTERS, centers);
			}

			max_cluster_center.x = 320;
			if (centers.size() > 0)
			{
				for (int i = 0; i < centers.size(); i++)
				{
					if (centers[i].y - max_cluster_center.y > 10) max_cluster_center = centers[i];
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

			//greyscale_left.convertTo(greyscale_left, CV_32F);
			greyscale_right.convertTo(greyscale_right, CV_32F);
			//greyscale_left.convertTo(greyscale_left, CV_8UC1);
			greyscale_right.convertTo(greyscale_right, CV_8UC1);



			//turning if bot doesnt see AIDs and left wall are close to bot
			if (points_left - points_right > 30000 && points.size() == 0)
			{
				actions = { 0, 0, 40, 1 };
			}
			//turning if bot doesnt see AIDs and left wall are close to bot
			else if (points_right - points_left > 30000 && points.size() == 0)
			{
				actions = { 0, 0, -40, 1 };
			}

			//cv::circle(image, max_cluster_center, 5, COLOR_GREEN, -1, 8);
			//cv::circle(image, max_cluster_center, 40, COLOR_GREEN);
			//cv::putText(image, "AID", cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), 1, 1, COLOR_GREEN);
			//cv::rectangle(image, cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), cv::Point(max_cluster_center.x + 40, max_cluster_center.y + 30), COLOR_GREEN, -1);

			cv::Mat result;

			int result_cols = image.cols - grenade_templ.cols + 1;
			int result_rows = grenade_templ.rows - grenade_templ.rows + 1;

			result.create(result_rows, result_cols, CV_32FC1);

			//finding grenade
			cv::matchTemplate(image, grenade_templ, result, 4);

			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

			for (int i = 0; i < centers.size(); i++)
			{
				cv::Point c = centers[i];

				//cv::circle(image, c, 5, COLOR_BLUE, -1, 8);
				//cv::circle(image, c, 40, COLOR_BLUE);
				cv::putText(image, "AID", cv::Point(c.x - 20, c.y - 30), 1, 1, COLOR_GREEN);
				cv::rectangle(image, cv::Point(c.x - 20, c.y - 30), cv::Point(c.x + 40, c.y + 30), COLOR_GREEN, 1);
			}

			cv::putText(image, "MAX AID", cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), 1, 1, COLOR_LIGHT_BLUE);
			cv::rectangle(image, cv::Point(max_cluster_center.x - 20, max_cluster_center.y - 30), cv::Point(max_cluster_center.x + 40, max_cluster_center.y + 30), COLOR_LIGHT_BLUE);

			cv::putText(image, "grenade", maxLoc, 1, 1, cv::Scalar(0, 0, maxVal * 0.0000274));
			cv::putText(image, "grenade visor", cv::Point(140, 200), 1, 1, COLOR_YELLOW);

			//grenade signature becomes more red if it looks more close to grenade
			rectangle(image, maxLoc, cv::Point(maxLoc.x + grenade_templ.cols, maxLoc.y + grenade_templ.rows), cv::Scalar(0, 0, maxVal * 0.0000274));
			rectangle(image, cv::Point(80, 200), cv::Point(560, 400), COLOR_YELLOW);

			//if requirements are met, bot thinks that grenade signature is really dangerous
			if (
				maxLoc.x > 80 && maxLoc.x < 560 && maxLoc.y > 200 && maxLoc.y < 400
				&& maxVal >= 9300000
				&& maxLoc.y > max_cluster_center.y
				//&& !(game->getGameVariable(vizdoom::HEALTH) > 40 && std::abs(max_cluster_center.x - maxLoc.x) < 20 || std::abs(max_cluster_center.y - maxLoc.y) < 20)
				//&& (std::abs(max_cluster_center.x - maxLoc.x) > 50 || std::abs(max_cluster_center.y - maxLoc.y) > 50)
				)
			{

				int aids_left = 0;
				int aids_right = 0;

				for (int i = 0; i < centers.size(); i++)
				{
					if (centers[i].x <= 320) aids_left++;
					else aids_right++;
				}

				if (maxLoc.x <= 320)
				{

					was_in_last_tic_grenade_left = true;
					was_in_last_tic_grenade_right = false;

					if (points_right > wall_points - 5000 || (aids_left != 0 && aids_right == 0))
					{
						double err1 = maxLoc.x - 600;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						std::cout << "Tic: " << gameState->tic << " : on my way woody left and wall right -> turn left" << std::endl;
					}
					else {
						double err1 = maxLoc.x - 40;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						std::cout << "Tic: " << gameState->tic << " : on my way woody left -> turn right" << std::endl;
					}
				}
				else
				{
					was_in_last_tic_grenade_left = false;
					was_in_last_tic_grenade_right = true;
					if (points_left > wall_points - 5000 || (aids_right != 0 && aids_left == 0))
					{
						double err1 = maxLoc.x - 40;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						std::cout << "Tic: " << gameState->tic << " : on my way woody right and wall left -> turn right" << std::endl;
					}
					else {
						double err1 = maxLoc.x - 600;
						double p1 = err1 * 0.2;
						integral1 = integral1 + err1 * 0.01;
						double u1 = p1 + integral1;
						actions = { 0, 0, u1, 1 };
						std::cout << "Tic: " << gameState->tic << " : on my way woody right -> turn left" << std::endl;
					}
				}

			}
			else
			{
				was_in_last_tic_grenade_left = false;
				was_in_last_tic_grenade_right = false;
			}
			
			double err = max_cluster_center.x - 320;
			if (points.size() > 0)
			{
				double p = err * 0.2;
				integral = integral + err * 0.01;
				double u = p + integral;
				actions = { 0, 0, u, 1 };
			}

			if (abs(err) < 20) actions = { 0,0,0,1 };

			game->makeAction(actions);

			//for (int i = 0; i < points.size(); i++)
			//{
				//cv::circle(image, points[i], 2, cv::Scalar(0, 255, 0));
			//}

			//cv::imshow("Res", result);
			//cv::imshow("Tmpl", grenade_templ);
			cv::imshow("Game", image);
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