//use VC15-3.2

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm> 
#include <list>


#define WARPED_XSIZE 200
#define WARPED_YSIZE 300

using namespace cv;
using namespace std;

Mat src; Mat src_gray, warped_result;
Mat speed_80, speed_40;
int canny_thresh = 150;


#define VERY_LARGE_VALUE 100000

#define NO_MATCH    0
#define STOP_SIGN            1
#define SPEED_LIMIT_40_SIGN  2
#define SPEED_LIMIT_80_SIGN  3

int main(int argc, char** argv)
{
	int sign_recog_result = NO_MATCH;
	speed_40 = imread("speed_40.bmp", 0);
	speed_80 = imread("speed_80.bmp", 0);

	// you run your program on these three examples (uncomment the two lines below)
	//string sign_name = "stop4";
	//string sign_name = "speedsign12";
	//string sign_name = "speedsign13";
	//string sign_name = "speedsign14";
	//string sign_name = "speedsign3";
	//string sign_name = "speedsign4";
	string sign_name = "speedsign5";
	string final_sign_input_name = sign_name + ".jpg";
	string final_sign_output_name = sign_name + "_result" + ".jpg";

	/// Load source image and convert it to gray
	src = imread(final_sign_input_name, 1);

	/// Convert image to gray and blur it
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));
	warped_result = Mat(Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());
	// here you add the code to do the recognition, and set the variable 
	// sign_recog_result to one of STOP_SIGN, SPEED_LIMIT_40_SIGN, SPEED_LIMIT_80_SIGN, or NO_MATCH
	Mat src_canny;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Point> result;
	Canny(src_gray,src_canny, canny_thresh, canny_thresh * 2, 3);
	findContours(src_canny, contours, hierarchy ,RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));
	double temp;
	double largestArea = 0;
	vector<Point> largestContour;

	for (int i = 0; i < contours.size();i++) {
		approxPolyDP(Mat(contours[i]), result, arcLength(Mat(contours[i]),true)*0.04, true);
		temp = contourArea(result, false);
		if (temp > largestArea) {
			largestContour = result;
			largestArea = temp;
		}
	}

	if (largestContour.size() == 8) {
		sign_recog_result = STOP_SIGN;
	}
	else if (largestContour.size() == 4) {
		Point2f newRectangle[4];
		double maxX, maxY;			//to make sure (0,0) is the top left point
		if ((largestContour[1].x + largestContour[1].y) > (largestContour[0].x + largestContour[0].y)) {
			newRectangle[0] = largestContour[0];
			newRectangle[1] = largestContour[3];
			newRectangle[2] = largestContour[2];
			newRectangle[3] = largestContour[1];
			maxX = max((largestContour[3].x - largestContour[0].x), (largestContour[2].x - largestContour[1].x));
			maxY = max((largestContour[1].y - largestContour[0].y), (largestContour[2].y - largestContour[3].y));
		}
		else
		{
			newRectangle[0] = largestContour[1];
			newRectangle[1] = largestContour[0];
			newRectangle[2] = largestContour[3];
			newRectangle[3] = largestContour[2];
			maxX = max((largestContour[0].x - largestContour[1].x), (largestContour[3].x - largestContour[2].x));
			maxY = max((largestContour[3].y - largestContour[0].y), (largestContour[2].y - largestContour[1].y));
		}

		Mat max, warpIm;
		max.rows = maxY;
		max.cols = maxX;
		warpIm.rows = WARPED_YSIZE;
		warpIm.cols = WARPED_XSIZE;

		Point2f perspect[4];
		perspect[0] = Point2f(0, 0);
		perspect[1] = Point2f(maxX - 1, 0);
		perspect[2] = Point2f(maxX - 1, maxY - 1);
		perspect[3] = Point2f(0, maxY - 1);

		Mat m = getPerspectiveTransform(newRectangle, perspect);
		Mat afterWarp, afterResize;
		warpPerspective(src_gray, afterWarp, m, max.size(), 1, 0, 0);
		resize(afterWarp, afterResize, warpIm.size(), 0, 0, 1);

		Mat result_40,result_80; 
		matchTemplate(afterResize, speed_40, result_40, CV_TM_CCOEFF_NORMED);
		matchTemplate(afterResize, speed_80, result_80, CV_TM_CCOEFF_NORMED);

		if (result_40.at<float>(0, 0) < 0.2&&result_80.at<float>(0, 0) < 0.2) {			//some images have a black part at bottom which affect matchtemplate
			Point2f perspect2[4];
			perspect2[0] = Point2f(0, 0);
			perspect2[1] = Point2f(WARPED_XSIZE, 0);
			perspect2[2] = Point2f(WARPED_XSIZE, WARPED_YSIZE +60);				//removing the black part by modifying the perspect matrix size
			perspect2[3] = Point2f(0, WARPED_YSIZE +60);
			warpPerspective(src_gray, afterWarp, getPerspectiveTransform(newRectangle, perspect2), warpIm.size(), 1, 0, 0);
			matchTemplate(afterWarp, speed_40, result_40, CV_TM_CCOEFF_NORMED);
			matchTemplate(afterWarp, speed_80, result_80, CV_TM_CCOEFF_NORMED);
			if (result_40.at<float>(0, 0) > result_80.at<float>(0, 0)) 
				sign_recog_result = SPEED_LIMIT_40_SIGN;
			else
				sign_recog_result = SPEED_LIMIT_80_SIGN;
		}
		else if (result_40.at<float>(0, 0) > 0.7) {
			sign_recog_result = SPEED_LIMIT_40_SIGN;
		}
		else if (result_80.at<float>(0, 0) > 0.5) {
			sign_recog_result = SPEED_LIMIT_80_SIGN;
			}
		else
				sign_recog_result = NO_MATCH;
		
	}
	else
		sign_recog_result = NO_MATCH;

	string text;
	if (sign_recog_result == SPEED_LIMIT_40_SIGN) text = "Speed 40";
	else if (sign_recog_result == SPEED_LIMIT_80_SIGN) text = "Speed 80";
	else if (sign_recog_result == STOP_SIGN) text = "Stop";
	else if (sign_recog_result == NO_MATCH) text = "Fail";

	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 2;
	int thickness = 3;
	cv::Point textOrg(10, 130);
	cv::putText(src, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

	/// Create Window
	const char* source_window = "Result";
	namedWindow(source_window, WINDOW_AUTOSIZE);
	imshow(source_window, src);
	imwrite(final_sign_output_name, src);
	
	waitKey(0);

	return(0);
}
