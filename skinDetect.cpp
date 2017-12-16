//#define _CRT_SECURE_NO_WARNINGS
//#include<iostream>
//#include<opencv2/core/core.hpp>
//#include<opencv2/opencv.hpp>
//#include<fstream>
//#include<string>
//#include<stdlib.h>
//
//using namespace std;
//using namespace cv;
//int readCamera(double threshold);
//int readImage(std::vector<String> &testimagePath, double threshold);
//int getFileName(String imagePathDir, std::vector<String> &imagePath);
//int train(std::vector<String> &imagePath, std::vector<String> &maskPath);
//
//cv::Vec3f skinMeanValue, skinVarianceValue;
//cv::Vec3f nonSkinMeanValue, nonSkinVarianceValue;
//
//int main()
//{
//	int ret;
//	double threshold = 0.5;
//	std::vector<String> imagePath, maskPath,testImagePath; // std::string in opencv2.4, but cv::String in 3.0
//	String imagePathDir = "image\\";
//	String maskPathDir = "mask\\";
//	String testImagePathDir = "testImage";
//	//String outputPath = "data-10557011\\knowLedge.txt";
//	getFileName(imagePathDir, imagePath);
//	getFileName(maskPathDir, maskPath);
//	train(imagePath, maskPath); //train 消除註解即可訓練
//
//	
//	imagePath.clear();
//	maskPath.clear();
//
//
//	getFileName(testImagePathDir, testImagePath);
//	ret = readImage(testImagePath, threshold); //照片test
//	testImagePath.clear();
//
//
//	
//	return 0;
//}
//
//
//int train(std::vector<String> &imagePath, std::vector<String> &maskPath){
//	long skinNum = 0, nonSkinNum = 0;
//	skinMeanValue = Vec3f::all(0);
//	skinVarianceValue = Vec3f::all(0);
//	nonSkinMeanValue = Vec3f::all(0);
//	nonSkinVarianceValue = Vec3f::all(0);
//
//	uchar *p_image;
//	uchar *p_mask;
//	for (int m = 0; m < imagePath.size(); m++){
//		Mat image = imread(imagePath[m], CV_LOAD_IMAGE_COLOR);
//		Mat mask = imread(maskPath[m], CV_LOAD_IMAGE_GRAYSCALE);
//
//		p_image = image.ptr<uchar>();
//		p_mask = image.ptr<uchar>();
//
//		//imshow("image", image);
//		//imshow("mask", mask);
//		//waitKey();
//
//		//for (int y = 0; y < image.rows; y++)
//		//{
//		//	for (int x = 0; x < image.cols; x++)
//		//	{
//
//		//	}
//		//}
//
//		for (int j = 0; j < image.rows; j++, p_image += image.step, p_mask += mask.step)
//		{
//			for (int i = 0; i < image.cols; i++)
//			{
//				if (p_mask[i] >= 250)
//				{
//					skinNum++;
//					skinMeanValue.val[0] += p_image[i * 3] / 255.0;
//					skinMeanValue.val[1] += p_image[i * 3 + 1] / 255.0;
//					skinMeanValue.val[2] += p_image[i * 3 + 2] / 255.0;
//
//					//meanValue += image.at<Vec3b>(j, i) / 255;
//				}
//				else
//				{
//					nonSkinNum++;
//					nonSkinMeanValue.val[0] += p_image[i * 3] / 255.0;
//					nonSkinMeanValue.val[1] += p_image[i * 3 + 1] / 255.0;
//					nonSkinMeanValue.val[2] += p_image[i * 3 + 2] / 255.0;
//				}
//			}
//		}
//
//	}
//	skinMeanValue /= skinNum;
//	nonSkinMeanValue /= nonSkinNum;
//
//	for (int m = 0; m < imagePath.size(); m++){
//		Mat image = imread(imagePath[m], CV_LOAD_IMAGE_COLOR);
//		Mat mask = imread(maskPath[m], CV_LOAD_IMAGE_GRAYSCALE);
//
//		p_image = image.ptr<uchar>();
//		p_mask = image.ptr<uchar>();
//
//		for (int j = 0; j < image.rows; j++, p_image += image.step, p_mask += mask.step)
//		{
//			for (int i = 0; i < image.cols; i++)
//			{
//				if (p_mask[i] >= 250)
//				{	
//					skinVarianceValue.val[0] += pow(p_image[i * 3] / 255.0 - skinMeanValue[0], 2);
//					skinVarianceValue.val[1] += pow(p_image[i * 3 + 1] / 255.0 - skinMeanValue[1], 2);
//					skinVarianceValue.val[2] += pow(p_image[i * 3 + 2] / 255.0 - skinMeanValue[2], 2);
//
//					//varianceValue.val[0] += pow((float)image.at<Vec3b>(j,i)[0] / 255 - meanValue[0], 2);
//					//varianceValue.val[1] += pow((float)image.at<Vec3b>(j,i)[1] / 255 - meanValue[1], 2);
//					//varianceValue.val[2] += pow((float)image.at<Vec3b>(j,i)[2] / 255 - meanValue[2], 2);
//				}
//
//				else
//				{
//					nonSkinVarianceValue.val[0] += pow(p_image[i * 3] / 255.0 - nonSkinMeanValue[0], 2);
//					nonSkinVarianceValue.val[1] += pow(p_image[i * 3 + 1] / 255.0 - nonSkinMeanValue[1], 2);
//					nonSkinVarianceValue.val[2] += pow(p_image[i * 3 + 2] / 255.0 - nonSkinMeanValue[2], 2);
//				}
//			}
//		}
//	}
//	skinVarianceValue /= skinNum;
//	nonSkinVarianceValue /= nonSkinNum;
//
//
//	std::cout <<"skin_mean: "<< skinMeanValue << endl;
//	std::cout <<"skin_var: "<< skinVarianceValue << endl;
//	std::cout <<"non_skin_mean: "<< nonSkinMeanValue << endl;
//	std::cout << "non_skin_var: "<<nonSkinVarianceValue << endl;
//	return 0;
//}
//
//
//int getFileName(String imagePathDir, std::vector<String> &imagePath){
//
//	cv::glob(imagePathDir, imagePath, false);
//	return 0;
//}
//
//int readCamera(double threshold){
//	cv::Mat frame;
//	cv::VideoCapture cap(0);
//
//	if (!cap.isOpened())
//		//如果沒有開啟成功，則離開程式
//	{
//		return -1;
//	}
//	while (true)
//	{
//		if (!cap.read(frame))
//			break;
//		cv::Mat src = cv::Mat(frame);
//		uchar* p_testImage = src.ptr<uchar>();
//
//		imshow("origin", frame);
//
//		for (int j = 0; j < src.rows; j++, p_testImage += src.step)
//		{
//			for (int i = 0; i < src.cols; i++)
//			{
//				float skinProb = 0;
//				skinProb  = pow((p_testImage[3 * i    ] / 255 - skinMeanValue[0]), 2) / 2 / skinVarianceValue[0];
//				skinProb += pow((p_testImage[3 * i + 2] / 255 - skinMeanValue[1]), 2) / 2 / skinVarianceValue[1];
//				skinProb += pow((p_testImage[3 * i + 2] / 255 - skinMeanValue[2]), 2) / 2 / skinVarianceValue[2];
//
//				skinProb = 1 / exp(skinProb);
//
//				float nonSkinProb = 0;
//				nonSkinProb = pow((p_testImage[3 * i] / 255 - nonSkinMeanValue[0]), 2) / 2 / nonSkinVarianceValue[0];
//				nonSkinProb += pow((p_testImage[3 * i + 2] / 255 - nonSkinMeanValue[1]), 2) / 2 / nonSkinVarianceValue[1];
//				nonSkinProb += pow((p_testImage[3 * i + 2] / 255 - nonSkinMeanValue[2]), 2) / 2 / nonSkinVarianceValue[2];
//
//				nonSkinProb = 1 / exp(nonSkinProb);
//
//				if (skinProb > nonSkinProb)
//				{
//					p_testImage[3 * i] = 255;
//					p_testImage[3 * i + 1] = 255;
//					p_testImage[3 * i + 2] = 255;
//				}
//				else
//				{
//					p_testImage[3 * i] = 0;
//					p_testImage[3 * i + 1] = 0;
//					p_testImage[3 * i + 2] = 0;
//				}
//			}
//		}
//
//
//		cv::imshow("window", src);
//		cv::waitKey(30);
//	}
//	return 0;
//}
//
//int readImage(std::vector<String> &testimagePath, double threshold){
//	for (int i = 0; i < testimagePath.size(); i++){
//		Mat testImage = imread(testimagePath[i], CV_LOAD_IMAGE_COLOR);
//
//		imshow("Display window2", testImage);
//
//		uchar* p_testImage = testImage.ptr<uchar>();
//
//		for (int j = 0; j < testImage.rows; j++, p_testImage += testImage.step)
//		{
//			for (int i = 0; i < testImage.cols; i++)
//			{
//				float prob = 0;
//				prob += pow((p_testImage[3 * i    ] / 255 - skinMeanValue[0]), 2) / 2 / skinVarianceValue[0];
//				prob += pow((p_testImage[3 * i + 2] / 255 - skinMeanValue[1]), 2) / 2 / skinVarianceValue[1];
//				prob += pow((p_testImage[3 * i + 2] / 255 - skinMeanValue[2]), 2) / 2 / skinVarianceValue[2];
//
//				prob = 1 / exp(prob);
//
//				if (prob > threshold)
//				{
//					p_testImage[3 * i    ] = 255;
//					p_testImage[3 * i + 1] = 255;
//					p_testImage[3 * i + 2] = 255;
//				}
//				else
//				{
//					p_testImage[3 * i] = 0;
//					p_testImage[3 * i + 1] = 0;
//					p_testImage[3 * i + 2] = 0;
//				}
//			}
//		}
//
//		imshow("Display window", testImage);
//		waitKey(0);
//	}
//	return 0;
//}
//
//
