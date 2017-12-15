#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

#include<opencv2/opencv.hpp>//OpenCV 3.2
using namespace cv;

#include<algorithm>
#include<vector>

//#define DEBUG
#define CHECK_AGAIN

void check_img_isPair(vector<String> &img_list, vector<String> &img_mask_list);


int main()
{
	string img_path="image";
	string img_mask_path="mask";
	vector<String>img_list;
	vector<String>img_mask_list;

	glob(img_path, img_list, false);
	glob(img_mask_path, img_mask_list, false);

#ifdef CHECK_AGAIN
	//Check our image can compare to its mask
	check_img_isPair(img_list, img_mask_list);
#endif
	




	return 0;
}



/*--Have checked--*/
void check_img_isPair(vector<String> &img_list, vector<String> &img_mask_list)
{
	for (int i = 0; i < img_list.size(); i++)
	{
		Mat img = imread(img_list[i], CV_LOAD_IMAGE_COLOR);
		Mat mask = imread(img_mask_list[i], CV_LOAD_IMAGE_GRAYSCALE);

		imshow("img", img);
		imshow("mask", mask);
		waitKey(0);
	}
}