#include "valsalva_class.h"
#include <chrono>


valsalva::valsalva(const string & modelfile)
{
    cout << "start initalize " << "\n"; 
	outNames.clear();
	net = cv::dnn::readNetFromTensorflow(modelfile);
	outNames.push_back(net.getUnconnectedOutLayersNames()[2]);  //seg
}

void valsalva::getSeg(const cv::Mat& inputImage, vector<cv::Point>& outputSegContour)
{
    cout << "seg start" << endl;
    cv::Size dsize = cv::Size((int) inputImage.cols, (int) inputImage.rows);
                                        
	cv::Mat inputBlob = cv::dnn::blobFromImage(inputImage, 1.0 / 255, cv::Size(256, 256), cv::Scalar(), false, false);
	net.setInput(inputBlob);
    
	vector<cv::Mat> outs;
    auto startTime = chrono::high_resolution_clock::now();                                                       //outputs
	net.forward(outs, outNames);
    cout << "seg cost time: " << (chrono::high_resolution_clock::now() - startTime).count()/1e6 << "ms" << endl;
        
	vector<cv::Mat> out_images;
	cv::Mat output_img;
	cv::dnn::imagesFromBlob(outs[0], out_images);
    cv::resize(out_images[0], output_img, dsize, 2);
    output_img.convertTo(output_img, CV_8UC1, 255.0);
                                                    //seg out
    cv::Mat seg_thresh;
	vector<vector<cv::Point>> seg_contours;
    cv::threshold(output_img, seg_thresh, 125, 255, cv::THRESH_BINARY);
    cv::findContours(seg_thresh, seg_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_L1);
	
	double area = 0, maxArea = 0;
	size_t maxSegArea_index = 0;
    for(size_t idx = 0; idx < seg_contours.size(); idx++)
    {   
        area = cv::contourArea(seg_contours[idx]);         
        if ( area > maxArea)
        {                                   //find max connected area
            maxSegArea_index = idx;
            maxArea = area;
        }                                
    }
	outputSegContour = seg_contours[maxSegArea_index];
    //cv::drawContours(inputImage, seg_contours, maxSegArea_index, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);  //draw predict seg

    cout << "seg finish " <<endl;
}


