// FisheyeCam.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

cv::Point2f SelectPoint(cv::Mat image)
{
	std::string windowName = "SelectPoint";
	cv::Point2f point = cv::Point2f((float)image.size().width/2, (float)image.size().height/2);
	int key = 0;
	do {
		switch (key)
		{
		case 119: // w
			point.y -= 2;
			break;
		case 115: // s
			point.y += 2;
			break;
		case 97: // a
			point.x -= 2;
			break;
		case 100: // d
			point.x += 2;
			break;
		default:
			break;
		}
		cv::Mat temp = image.clone();
		cv::circle(temp, point, 3, cv::Scalar(255,0,0), 2);
		cv::imshow(windowName, temp);
		key = cv::waitKey();

	} while (key != 27); // esc

	cv::destroyWindow(windowName);
	return point;
}

void CreateUndistortMap(cv::Mat K, cv::Mat D, cv::Size size, cv::Mat& map_x, cv::Mat& map_y)
{
	cv::fisheye::initUndistortRectifyMap(K, D, cv::noArray(), K, size, CV_32FC1, map_x, map_y);
}

cv::Mat Undistort(cv::Mat image, cv::Mat map_x, cv::Mat map_y)
{
	cv::Mat undistortImg;
	cv::remap(image, undistortImg, map_x, map_y, CV_INTER_LINEAR);
	return undistortImg;
}

// Distort point using undistort map
cv::Point2f DistortPoint(cv::Point2f point, cv::Mat map_x, cv::Mat map_y)
{
	float distorted_x = map_x.at<float>((int)point.y, (int)point.x);
	float distorted_y = map_y.at<float>((int)point.y, (int)point.x);
	return cv::Point2f(distorted_x, distorted_y);
}

void Test_DistortionUndistortionMapping()
{
	// load fisheye image
	cv::Mat distortImg = cv::imread("..\\images\\FisheyeRaw.jpg", CV_LOAD_IMAGE_COLOR);
	cv::resize(distortImg, distortImg, distortImg.size() / 2);
	cv::imshow("distort image", distortImg);

	// camera matrix
	cv::Mat camK = cv::Mat::eye(3, 3, CV_64FC1);
	camK.at<double>(0, 0) = 560;
	camK.at<double>(1, 1) = 560;
	camK.at<double>(0, 2) = 0;
	camK.at<double>(1, 2) = 0;

	// camera distortion
	cv::Mat camD = cv::Mat::zeros(4, 1, CV_64FC1);
	// camD.at<double>(0) = 0;
	// camD.at<double>(1) = 0;
	// camD.at<double>(2) = 0;
	// camD.at<double>(3) = 0;

	// create undistort map
	cv::Mat map_x, map_y;
	CreateUndistortMap(camK, camD, distortImg.size(), map_x, map_y);

	// undistort image
	cv::Mat undistortImg = Undistort(distortImg, map_x, map_y);
	cv::imshow("undistort image", undistortImg);
	cv::waitKey();
	cv::destroyAllWindows();

	// add point to undistort image
	std::vector<cv::Point2f> undistort_points;
	undistort_points.push_back(cv::Point2f(194, 122));
	undistort_points.push_back(cv::Point2f(284, 194));
	undistort_points.push_back(cv::Point2f(270, 118));
	undistort_points.push_back(cv::Point2f(162, 262));
	undistort_points.push_back(cv::Point2f(90, 338));
	undistort_points.push_back(cv::Point2f(46, 160));
	undistort_points.push_back(cv::Point2f(488, 366));
	//while (1)
	//{
	//	cv::Point2f selected_point = SelectPoint(undistortImg);
	//	undistort_points.push_back(selected_point);
	//}

	for (int i = 0; i < undistort_points.size(); i++)
	{
		cv::circle(undistortImg, undistort_points[i], 3, cv::Scalar(255, 0, 0), 2);
	}
	cv::imshow("points on undistort image", undistortImg);

	// distort point and add to distort image
	std::vector<cv::Point2f> distort_points;
	for (int i = 0; i < undistort_points.size(); i++)
	{
		cv::Point2f distort_pt = DistortPoint(undistort_points[i], map_x, map_y);
		distort_points.push_back(distort_pt);
		cv::circle(distortImg, distort_pt, 3, cv::Scalar(255, 0, 0), 2);
	}
	cv::imshow("points mapped from undistort point", distortImg);

	cv::waitKey();
	cv::destroyAllWindows();
}

int main()
{
	Test_DistortionUndistortionMapping();
}

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
