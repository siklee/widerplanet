#pragma once
#define CURL_STATICLIB

#include "detect_skin.hpp"
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <queue>
#include "opencvgif.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bloblabeling.h"
#include <sys/types.h>
#include <dirent.h>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#pragma comment(lib,"wldap32.lib")
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib,"libcurld.lib")
#else
#pragma comment(lib,"libcurl.lib")
#endif


#define Skin_low_Y 186
#define Skin_high_Y 188
#define Skin_Dev_Y 1600
#define Skin_low_CR 145
#define Skin_high_CR 147
#define Skin_Dew_CR 100
#define Skin_low_CB 110
#define Skin_high_CB 112
#define Skin_Dev_CB 100

#define Fix_row 250
#define Fix_col 330

using namespace std;
using namespace cv;


bool cal_img(const char *img_url, Mat& src);
void show_img(const char *img_url, Mat& src);
void pro_detect(const char *img_name, Mat& src, ofstream& ofile,ofstream& tfile,ofstream& ffile,int th);              //skin 추출함수 
Mat cropimg(Mat& src);
void Gaussian1D(unsigned char *GD, double mean_a, double mean_b, double var, int num);
void init();
Mat curlgifImg(const char *img_url, const char *path);
void save_img(const char *img_url, const char *path, Mat& src);

Mat make_gaussian_ycrcb(Mat src, int th);   //ycbcr
Mat label(Mat inpitImg, int threshold);

int count_img = 0;                           // 전체 이미지 개수
float count_cor = 0;                         // 걸러야 되는 이미지
float count_fal = 0;                         // 거르지 말아야할 이미지 
int low_red, high_red, low_green, high_green;
uchar *Lut_norCR, *Lut_norCB, *Lut_norY;

typedef struct {
	int x;
	int y;
}xy;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	vector<uchar> *stream = (vector<uchar>*)userdata;
	size_t count = size * nmemb;
	stream->insert(stream->end(), ptr, ptr + count);
	return count;
}

size_t write_data2(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

//function to retrieve the image as cv::Mat data type
cv::Mat curlImg(const char *img_url, int timeout = 10)
{
	vector<uchar> stream;
	CURL *curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, img_url); //the img url
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // pass the writefunction
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr to the writefunction
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout); // timeout if curl_easy hangs, 
	CURLcode res = curl_easy_perform(curl); // start curl
	curl_easy_cleanup(curl); // cleanup
	return imdecode(stream, -1); // 'keep-as-is'
}

cv::Mat curlgifImg(const char *img_url, const char *path)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char imagefilename[100];
	sprintf(imagefilename, path, count_img);
	cout << count_img << " " << img_url << endl;
	curl = curl_easy_init();
	if (curl)
	{
		fp = fopen(imagefilename, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, img_url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	Mat image = loadGIF(imagefilename);

	return image;
}

void init()   // make gaussian normal distribution
{
	Lut_norY = (uchar *)calloc(256, sizeof(uchar));
	Lut_norCR = (uchar *)calloc(256, sizeof(uchar));
	Lut_norCB = (uchar *)calloc(256, sizeof(uchar));
	
	Gaussian1D(Lut_norY, Skin_low_Y, Skin_high_Y, Skin_Dev_Y, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norCR, Skin_low_CR, Skin_high_CR, Skin_Dew_CR, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norCB, Skin_low_CB, Skin_high_CB, Skin_Dev_CB, 256);  //Gaussian1D(Lut_norG, Skin_low_mG, Skin_high_mG, SDev_G*SDev_G, 256);

}

//img = make_gaussian_pointer(img, 96, 98, 81, 83, 18, 12,th);//img = make_gaussian(img, 97, 101, 77, 101, 12, 10);
//image = make_gaussian_ycrcb_pointer(image,186,188, 145, 147, 110, 112, 40, 10, 10,25); //(img,186,188, 145, 147, 110, 112, 40, 10, 10)
//image = make_gaussian_hls_pointer(image, 10, 12, 184, 186, 86, 88, 36, 63, 43, 91);

//Mat make_gaussian_pointer(Mat src, int lr, int hr, int lg, int hg, int dr, int dg, int th)
//Mat make_gaussian_ycrcb_pointer(Mat src, int ly, int hy, int lr, int hr, int lb, int hb, int dy, int dr, int db, int th)   //ycbcr
//Mat make_gaussian_hls_pointer(Mat src, , int lh, int hh, int ll, int hl, int ls, int hs, int dh, int dl, int ds, int th)


//  split input text to 
// vec[0]          vec[1]  vec[2]   vec[3]   vec[4]   vec[5]   vec[6]
// product name    purl    iurl     c1       c2        c3      c4
template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}
