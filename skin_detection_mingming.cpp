#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

#include<opencv2/opencv.hpp>//OpenCV 3.2
using namespace cv;

#include<algorithm>
#include<vector>

struct pixel_t
{
	float b, g, r;
};

struct bayes_t
{
	pixel_t mean;
	pixel_t variance;
};

//#define CHECK_AGAIN
//#define DEBUG_INFO
void check_img_isPair(vector<String> &img_list, vector<String> &img_mask_list);
void output_mat_information(Mat &img);
pair<pixel_t, pixel_t> compute_Color_mean(Mat img, Mat &img_binary_mask, int mask_threshold = 128);
pair<pixel_t, pixel_t> compute_skin_nonSkinColor_mean(vector<String> &img_list, vector<String> &img_mask_list);





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
	
	bayes_t skin_color_bayes, non_skin_color_bayes;
	
	/*---統計所有圖片膚色的mean---*/
	pair<pixel_t, pixel_t> skin_nonSkin_mean_pair = compute_skin_nonSkinColor_mean(img_list, img_mask_list);
	skin_color_bayes.mean = skin_nonSkin_mean_pair.first;
	non_skin_color_bayes.mean = skin_nonSkin_mean_pair.second;
	
	/*---統計所有圖片膚色的variance---*/



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

void output_mat_information(Mat &img)
{
	cout << "rows: " << img.rows << endl;
	cout << "cols: " << img.cols << endl;
	cout << "channels: " << img.channels() << endl;
	return;
}


/*--計算單張圖的膚色非膚色的Mean--*/
pair<pixel_t, pixel_t> compute_Color_mean(Mat img, Mat &img_binary_mask, int mask_threshold)
{
	pixel_t skin_color_mean = { 0.0, 0.0, 0.0 };
	pixel_t non_skin_color_mean = { 0.0, 0.0, 0.0 };
	int skin_color_pixel_counts = 0;
	int non_skin_color_pixel_counts = 0;
	//int mask_threshold = 128;//剛剛check過數值都是255, 保險起見用128


	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			int r, g, b;
			int mask_color;

			mask_color = img_binary_mask.at<uchar>(y, x);

			b = img.at<Vec3b>(y, x)[0];
			g = img.at<Vec3b>(y, x)[1];
			r = img.at<Vec3b>(y, x)[2];

			if (mask_color < mask_threshold)//Non-skin color
			{
				non_skin_color_mean.b += b;
				non_skin_color_mean.g += g;
				non_skin_color_mean.r += r;
				non_skin_color_pixel_counts++;
			}
			else
			{
				skin_color_mean.b += b;
				skin_color_mean.g += g;
				skin_color_mean.r += r;
				skin_color_pixel_counts++;
			}
		}
	}

	//有些圖片沒有膚色
	if (skin_color_pixel_counts != 0)
	{
		skin_color_mean.b /= float(skin_color_pixel_counts);
		skin_color_mean.g /= float(skin_color_pixel_counts);
		skin_color_mean.r /= float(skin_color_pixel_counts);
	}

	if (non_skin_color_pixel_counts != 0)
	{
		non_skin_color_mean.b /= float(non_skin_color_pixel_counts);
		non_skin_color_mean.g /= float(non_skin_color_pixel_counts);
		non_skin_color_mean.r /= float(non_skin_color_pixel_counts);
	}

#ifdef DEBUG_INFO
	cout << "SkinColor_mean: (b, g, r): " << skin_color_mean.b << " " << skin_color_mean.g << " " << skin_color_mean.r << endl;
	cout << "Non_SkinColor_mean: (b, g, r): " << non_skin_color_mean.b << " " << non_skin_color_mean.g << " " << non_skin_color_mean.r << endl;
#endif

	return make_pair(skin_color_mean, non_skin_color_mean);
}

/*---計算所有圖片統計過後的skin_mean, non_skin_mean---*/
pair<pixel_t, pixel_t> compute_skin_nonSkinColor_mean(vector<String> &img_list, vector<String> &img_mask_list)
{
	pixel_t bayes_skin_color_mean = {0.0, 0.0, 0.0};
	pixel_t bayes_non_skin_color_mean = {0.0, 0.0, 0.0};
	vector<pair<pixel_t, pixel_t>> img_skin_nonSkinMean_List;
	for (int i = 0; i < img_list.size(); i++)
	{
		Mat img = imread(img_list[i], CV_LOAD_IMAGE_COLOR);
		Mat mask = imread(img_mask_list[i], CV_LOAD_IMAGE_GRAYSCALE);

		pair<pixel_t, pixel_t> skin_nonSkinMean_Pair = compute_Color_mean(img, mask);
		pixel_t skin_color = skin_nonSkinMean_Pair.first;

		if (skin_color.b == 0 && skin_color.g == 0 && skin_color.r == 0)
		{
			cout << "This images doesn't contain any skin_color" << endl;
			cout << img_list[i] << endl;
			continue;
		}

		img_skin_nonSkinMean_List.push_back(skin_nonSkinMean_Pair);
	}

	for (int i = 0; i < img_skin_nonSkinMean_List.size(); i++)
	{
		pair<pixel_t, pixel_t> one_pair = img_skin_nonSkinMean_List[i];
		pixel_t skin_color_mean = one_pair.first;
		pixel_t non_skin_color_mean = one_pair.second;

		bayes_skin_color_mean.b += skin_color_mean.b;
		bayes_skin_color_mean.g += skin_color_mean.g;
		bayes_skin_color_mean.r += skin_color_mean.r;

		bayes_non_skin_color_mean.b += non_skin_color_mean.b;
		bayes_non_skin_color_mean.g += non_skin_color_mean.g;
		bayes_non_skin_color_mean.r += non_skin_color_mean.r;
	}

	int img_counts = img_skin_nonSkinMean_List.size();
	bayes_skin_color_mean.b /= (float)img_counts;
	bayes_skin_color_mean.g /= (float)img_counts;
	bayes_skin_color_mean.r /= (float)img_counts;

	bayes_non_skin_color_mean.b /= (float)img_counts;
	bayes_non_skin_color_mean.g /= (float)img_counts;
	bayes_non_skin_color_mean.r /= (float)img_counts;

	return make_pair(bayes_skin_color_mean, bayes_non_skin_color_mean);
}