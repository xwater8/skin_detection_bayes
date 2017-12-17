#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

#include<opencv2/opencv.hpp>//OpenCV 3.2
using namespace cv;

#include<algorithm>
#include<vector>
#include<cmath>
struct bayes_t
{
	Vec3f mean;
	Vec3f variance;
};

#define USE_YCRCB

//#define CHECK_AGAIN
//#define DEBUG_INFO
void check_img_isPair(vector<String> &img_list, vector<String> &img_mask_list);
void output_mat_information(Mat &img);
pair<Vec3f, Vec3f> compute_Color_mean(Mat img, Mat &img_binary_mask, int mask_threshold = 128);
pair<Vec3f, Vec3f> compute_skin_nonSkinColor_mean(vector<String> &img_list, vector<String> &img_mask_list);
Vec3f Vec3f_variance(Vec3f &Xi, Vec3f &mean);
pair<Vec3f, Vec3f> compute_Color_Variance(Mat img, Mat &img_binary_mask, Vec3f &skin_color_mean, Vec3f &non_skin_color_mean, int mask_threshold);
pair<Vec3f, Vec3f> compute_skin_nonSkinColor_Variance(vector<String> &img_list, vector<String> &img_mask_list, Vec3f &skin_color_mean, Vec3f &non_skin_color_mean);

double normal_distribution(double value, double u, double variance);
double color_normal_distribution(Vec3b &color, Vec3f &mean, Vec3f &variance);
bool isSkin(Vec3b &color, bayes_t &skin_color_bayes, bayes_t &non_skin_color_bayes);

double normal_distribution(double value, double u, double variance)
{
	double pi = 3.14;
	double stand_deviation = sqrt(variance);
	double denominator = 1 / (stand_deviation * sqrt(2 * pi));
	double in_exp_pow = -1 * pow(value-u, 2) / (2 * variance);
	double numerator = exp(in_exp_pow);

	return numerator * denominator;
}

double color_normal_distribution(Vec3b &color, Vec3f &mean, Vec3f &variance)
{
	double probability = 1.0;
	for (int i = 0; i < 3; i++)
	{
		probability *= normal_distribution(color[i], mean[i], variance[i]);
	}

	return probability;
}

bool isSkin(Vec3b &color, bayes_t &skin_color_bayes, bayes_t &non_skin_color_bayes)
{
	double skin_probability = color_normal_distribution(color, skin_color_bayes.mean, skin_color_bayes.variance);
	double non_skin_probability = color_normal_distribution(color, non_skin_color_bayes.mean, non_skin_color_bayes.variance);

	if (skin_probability >= non_skin_probability)
	{
		return true;
	}

	return false;
}


Mat img_skin_detection(Mat &img, bayes_t &skin_color_bayes, bayes_t &non_skin_color_bayes)
{
	Mat result_img = img.clone();
	Vec3b black(0, 0, 0);

#ifdef USE_YCRCB
	cvtColor(img, img, CV_BGR2YCrCb);
#endif

	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			Vec3b color = img.at<Vec3b>(y, x);

			//非skin的pixle變成黑色
			if (isSkin(color, skin_color_bayes, non_skin_color_bayes) == false)
			{
				result_img.at<Vec3b>(y,x) = black;
			}

		}
	}

	//imshow("Skin_detection_Results", result_img);
	//waitKey();

	return result_img;
}


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
	pair<Vec3f, Vec3f> skin_nonSkin_mean_pair = compute_skin_nonSkinColor_mean(img_list, img_mask_list);
	skin_color_bayes.mean = skin_nonSkin_mean_pair.first;
	non_skin_color_bayes.mean = skin_nonSkin_mean_pair.second;

	/*---統計所有圖片膚色的Variance---*/
	pair<Vec3f, Vec3f> skin_nonSkin_Variance_pair = compute_skin_nonSkinColor_Variance(img_list, img_mask_list, skin_color_bayes.mean, non_skin_color_bayes.mean);
	skin_color_bayes.variance = skin_nonSkin_Variance_pair.first;
	non_skin_color_bayes.variance = skin_nonSkin_Variance_pair.second;


	/*---Start to Test---*/
	string test_path = "testImage";
	vector<String> test_img_list;
	glob(test_path, test_img_list);

	for (int i = 0; i < test_img_list.size(); i++)
	{
		Mat img = imread(test_img_list[i], CV_LOAD_IMAGE_COLOR);
		Mat skin_detect_img;
		char output_fileName[100];
		sprintf(output_fileName, "output_%d.jpg", i);
		if (img.empty() == true)
		{
			cout << "Can't open the img: " << img_list[i] << endl;
			continue;
		}

		skin_detect_img = img_skin_detection(img, skin_color_bayes, non_skin_color_bayes);
		imshow("skin_detection", skin_detect_img);
		waitKey(10);
		imwrite(string(output_fileName), skin_detect_img);
	}

	//Released memory

	img_list.clear();
	img_mask_list.clear();
	test_img_list.clear();

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
pair<Vec3f, Vec3f> compute_Color_mean(Mat img, Mat &img_binary_mask, int mask_threshold)
{
	Vec3f skin_color_mean = { 0.0, 0.0, 0.0 };
	Vec3f non_skin_color_mean = { 0.0, 0.0, 0.0 };
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

			Vec3b color = img.at<Vec3b>(y, x);
			

			if (mask_color < mask_threshold)//Non-skin color
			{
				non_skin_color_mean += color;				
				non_skin_color_pixel_counts++;
			}
			else
			{
				skin_color_mean += color;				
				skin_color_pixel_counts++;
			}
		}
	}

	//有些圖片沒有膚色
	if (skin_color_pixel_counts != 0)
	{
		skin_color_mean /= float(skin_color_pixel_counts);		
	}

	if (non_skin_color_pixel_counts != 0)
	{
		non_skin_color_mean /= float(non_skin_color_pixel_counts);		
	}

#ifdef DEBUG_INFO
	cout << "SkinColor_mean: (b, g, r): " << skin_color_mean.b << " " << skin_color_mean.g << " " << skin_color_mean.r << endl;
	cout << "Non_SkinColor_mean: (b, g, r): " << non_skin_color_mean.b << " " << non_skin_color_mean.g << " " << non_skin_color_mean.r << endl;
#endif

	return make_pair(skin_color_mean, non_skin_color_mean);
}

/*---計算所有圖片統計過後的skin_mean, non_skin_mean---*/
pair<Vec3f, Vec3f> compute_skin_nonSkinColor_mean(vector<String> &img_list, vector<String> &img_mask_list)
{
	Vec3f bayes_skin_color_mean = {0.0, 0.0, 0.0};
	Vec3f bayes_non_skin_color_mean = {0.0, 0.0, 0.0};
	vector<pair<Vec3f, Vec3f>> img_skin_nonSkinMean_List;
	for (int i = 0; i < img_list.size(); i++)
	{
		Mat img = imread(img_list[i], CV_LOAD_IMAGE_COLOR);
		Mat mask = imread(img_mask_list[i], CV_LOAD_IMAGE_GRAYSCALE);

		//ycrcb
#ifdef USE_YCRCB
		cvtColor(img, img, CV_BGR2YCrCb);
#endif
		pair<Vec3f, Vec3f> skin_nonSkinMean_Pair = compute_Color_mean(img, mask);
		Vec3f skin_color = skin_nonSkinMean_Pair.first;

		if (skin_color[0] == 0 && skin_color[1] == 0 && skin_color[2] == 0)
		{
			cout << "This images doesn't contain any skin_color" << endl;
			cout << img_list[i] << endl;
			continue;
		}

		img_skin_nonSkinMean_List.push_back(skin_nonSkinMean_Pair);
	}

	for (int i = 0; i < img_skin_nonSkinMean_List.size(); i++)
	{
		pair<Vec3f, Vec3f> one_pair = img_skin_nonSkinMean_List[i];
		Vec3f skin_color_mean = one_pair.first;
		Vec3f non_skin_color_mean = one_pair.second;

		bayes_skin_color_mean += skin_color_mean;
		
		bayes_non_skin_color_mean += non_skin_color_mean;
		
	}

	int img_counts = img_skin_nonSkinMean_List.size();
	bayes_skin_color_mean /= (float)img_counts;
	
	bayes_non_skin_color_mean /= (float)img_counts;	

	return make_pair(bayes_skin_color_mean, bayes_non_skin_color_mean);
}


Vec3f Vec3f_variance(Vec3f &Xi, Vec3f &mean)
{
	Vec3f variance;
	for (int i = 0; i < 3; i++)
	{
		variance[i] = pow(Xi[i] - mean[i], 2);
	}

	return variance;
}

pair<Vec3f, Vec3f> compute_Color_Variance(Mat img, Mat &img_binary_mask, Vec3f &skin_color_mean, Vec3f &non_skin_color_mean, int mask_threshold)
{
	Vec3f img_skin_variance = { 0.0, 0.0, 0.0 };
	Vec3f img_non_skin_variance = { 0.0, 0.0, 0.0 };
	int skin_pixel_counts = 0, non_skin_pixel_counts = 0;

	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			int mask_color = img_binary_mask.at<uchar>(y, x);
			Vec3f color = Vec3f(img.at<Vec3b>(y, x));

			if (mask_color < mask_threshold)//non_skin
			{
				img_non_skin_variance += Vec3f_variance(color, non_skin_color_mean);
				non_skin_pixel_counts++;
			}
			else
			{
				img_skin_variance += Vec3f_variance(color, skin_color_mean);
				skin_pixel_counts++;
			}

		}
	}

	//如果這張圖片沒有膚色那就不進行計算
	if (skin_pixel_counts == 0)
	{
		return make_pair(Vec3f(0, 0, 0), Vec3f(0, 0, 0));
	}

	img_non_skin_variance /= (float)non_skin_pixel_counts;
	img_skin_variance /= (float)skin_pixel_counts;

	return make_pair(img_skin_variance, img_non_skin_variance);

}

pair<Vec3f, Vec3f> compute_skin_nonSkinColor_Variance(vector<String> &img_list, vector<String> &img_mask_list, Vec3f &skin_color_mean, Vec3f &non_skin_color_mean)
{

	cout << "Compute Variance Start: " << endl;
	cout << "skin_mean: " << skin_color_mean << endl;
	cout << "non_skin_mean: " << non_skin_color_mean << endl;


	/*---計算Variance---*/
	Vec3f skin_variance = { 0.0, 0.0, 0.0 };
	Vec3f non_skin_variance = { 0.0, 0.0, 0.0 };

	int no_skin_img = 0;
	for (int i = 0; i < img_list.size(); i++)
	{
		Mat img = imread(img_list[i], CV_LOAD_IMAGE_COLOR);
		Mat img_mask = imread(img_mask_list[i], CV_LOAD_IMAGE_GRAYSCALE);

#ifdef USE_YCRCB
		cvtColor(img, img, CV_BGR2YCrCb);
#endif
		if (img.empty() == true)
		{
			cout << "Can't open the img: " << img_list[i] << endl;
		}
		else if (img_mask.empty() == true)
		{
			cout << "Can't open the img_mask: " << img_mask_list[i] << endl;
		}

		pair<Vec3f, Vec3f> img_variance_pair = compute_Color_Variance(img, img_mask, skin_color_mean, non_skin_color_mean, 128);
		Vec3f no_skinColor_img(0, 0, 0);

		if (img_variance_pair.first == no_skinColor_img && img_variance_pair.second == no_skinColor_img)
		{
			cout << "This img no skin: " << img_list[i] << endl;
			no_skin_img++;
			continue;
		}

		skin_variance += img_variance_pair.first;
		non_skin_variance += img_variance_pair.second;


	}

	int img_counts = img_list.size() - no_skin_img;
	non_skin_variance = non_skin_variance / (float)img_counts;
	skin_variance = skin_variance / (float)img_counts;

	cout << "skin_variance: " << skin_variance << endl;
	cout << "non_skin_variance: " << non_skin_variance << endl;

	return make_pair(skin_variance, non_skin_variance);
}