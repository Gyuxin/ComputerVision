#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

vector<Point3f> GenerateObjectPoints();

int main(int argc, char** argv)
{
	vector<Point3f> objectPoints = GenerateObjectPoints();
	vector<Point2f> imagePoints ;
	Mat K(3, 3, DataType<float>::type);
	K.at<float>(0, 0) = -500;
	K.at<float>(1, 0) = 0;
	K.at<float>(2, 0) = 0;
	K.at<float>(0, 1) = 0;
	K.at<float>(1, 1) = -500;
	K.at<float>(2, 1) = 0;
	K.at<float>(0, 2) = 320;
	K.at<float>(1, 2) = 240;
	K.at<float>(2, 2) = 1;
	
	Mat R(3, 1, DataType<float>::type);
	R.at<float>(0,0) = 0;
	R.at<float>(1,0) = 0;
	R.at<float>(2,0) = 0;

	Mat T(3, 1, DataType<float>::type);
	T.at<float>(0, 0) = -170;
	T.at<float>(1, 0) = -105;
	T.at<float>(2, 0) = -70;

	Mat distCoeffs(4, 1, DataType<float>::type);
	distCoeffs.at<float>(0) = 0;
	distCoeffs.at<float>(1) = 0;
	distCoeffs.at<float>(2) = 0;
	distCoeffs.at<float>(3) = 0;
	
	projectPoints(objectPoints, R, T, K, distCoeffs, imagePoints);
	std::cout << "2D point: " << imagePoints  << std::endl;
	std::cout << "Press any button to exit..." << std::endl;
	std::cin.get();

	return 0;
}

vector<Point3f> GenerateObjectPoints()
{
	vector<Point3f> points;
	points.push_back(Point3f(350,220,150));
	return points;
}
