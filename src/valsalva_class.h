#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#ifdef DLL1_API
#else
#define DLL1_API /*_declspec(dllimport)*/
#endif


using namespace std;

class DLL1_API valsalva
{                       
public:
    valsalva(const string& modelfile);                                                                          //model
	void getSeg(const cv::Mat &inputImage, vector<cv::Point> &outputSegContour);
private:
	cv::dnn::Net net;
	vector<string> outNames;
};
