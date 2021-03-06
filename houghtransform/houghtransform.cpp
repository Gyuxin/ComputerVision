
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <iostream>
using namespace cv;
using namespace std;

int edgeThresh = 306;

int main(int argc, const char** argv)
{	
	Mat dst,cdst;
	Mat src = imread("track.jpg", 1);
	Canny(src, dst, edgeThresh, edgeThresh*3, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);

	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(src, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
	}

	imshow("detected line", src);

	// Wait for a key stroke; the same function arranges events processing
	waitKey(0);

	return 0;
}
