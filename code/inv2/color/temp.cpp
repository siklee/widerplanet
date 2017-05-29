/*
Mat pro_hist(const char *img_url, Mat& src)
{
int i, j, k, bin, ihist[3][256];
double hist[3][256];
double maxc[3];
int maxloc = 0;
int th = 50;

maxc[0] = maxc[1] = maxc[2] = 0;
Mat tmp;
tmp.create(src.rows, src.cols, CV_8UC3);

tmp = Scalar::all(0);



int cutrows = src.rows * 0.3;
int cutcols = src.cols * 0.3;


for (j = 0; j<3; j++)
for (i = 0; i < 256; i++)
{
ihist[j][i] = 0;
}

for (k = 0; k < 3; k++)
{
for (j = cutrows; j < src.rows - cutrows; j++)
{
for (i = cutcols; i < src.cols - cutcols; i++)
{
bin = src.at<Vec3b>(j, i)[k];
ihist[k][bin]++;
}

for (i = 0; i < 256; i++)
{
hist[k][i] = ihist[k][i] / (double)(src.rows * src.cols);
if (maxc[k]<hist[k][i])
{
maxc[k] = hist[k][i];
maxloc = i;
}
}
}
}

for (j = 0; j <src.rows; j++)
{
for (i = 0; i < src.cols; i++)
{
if ((src.at<Vec3b>(j, i)[0] >= maxc[0] - th && src.at<Vec3b>(j, i)[0] <= maxc[0] + th) &&
(src.at<Vec3b>(j, i)[1] >= maxc[1] - th && src.at<Vec3b>(j, i)[1] <= maxc[1] + th) &&
(src.at<Vec3b>(j, i)[2] >= maxc[2] - th && src.at<Vec3b>(j, i)[2] <= maxc[2] + th))
tmp.at<uchar>(j, i) = 255;
else
tmp.at<uchar>(j, i) = 0;
}
}


Mat barb = draw_histogram_gray(hist[0]);
namedWindow("È÷½ºÅä±×·¥b");
imshow("È÷½ºÅä±×·¥b", barb);


Mat barg = draw_histogram_gray(hist[1]);
namedWindow("È÷½ºÅä±×·¥g");
imshow("È÷½ºÅä±×·¥g", barg);


Mat barr = draw_histogram_gray(hist[2]);
namedWindow("È÷½ºÅä±×·¥r");
imshow("È÷½ºÅä±×·¥r", barr);

show_img("hahaha", tmp);
return src;
}

Mat draw_histogram_gray(double hist[])
{
double vmax = -1.0;
Mat himg(256, 256, CV_8U, Scalar(255));

for (int i = 0; i < 256; i++) vmax = (vmax > hist[i]) ? vmax : hist[i];

for (int i = 0; i < 256; i++) {
Point p1(i, 256), p2(i, 256 - (int)((hist[i] / vmax)*(0.8 * 256)));
line(himg, p1, p2, Scalar(0));
}

return himg;
}





void pro_equalize(const char *img_url, Mat& src)
{

char imageoriname[100];
char imageequname[100];
Mat equalized;

sprintf(imageoriname, "ori %s", img_url);
sprintf(imageequname, "equ %s", img_url);

src.copyTo(equalized);
equalized = equalize_hist(equalized);
equalized = kovac(equalized);
equalized = skin_color(equalized);


if (cal_img(img_url, equalized) == false)
{
show_img(imageoriname, src);
show_img(imageequname, equalized);
}
}


void pro_sharpen(const char *img_url, Mat& src)
{

char imageoriname[100];
char imagegauname[100];
Mat img;

sprintf(imageoriname, "ori %s", img_url);
sprintf(imagegauname, "sha %s", img_url);

src.copyTo(img);

Mat blurred; double sigma = 1, threshold = 5, amount = 1;
GaussianBlur(img, blurred, Size(), sigma, sigma);
Mat lowContrastMask = abs(img - blurred) < threshold;
Mat sharpened = img*(1 + amount) + blurred*(-amount);
img.copyTo(sharpened, lowContrastMask);

sharpened = kovac(sharpened);
sharpened = skin_color(sharpened);
//show_img(img_url, img);
//if (cal_img(img_url, img) == false)
//{

show_img(imagegauname, sharpened);
show_img(imageoriname, src);
//show_img(imagegauname, img);
//}


}

void pro_gaussian(const char *img_url, Mat& src)
{

char imageoriname[100];
char imagegauname[100];
Mat img;

sprintf(imageoriname, "ori %s", img_url);
sprintf(imagegauname, "gau %s", img_url);

src.copyTo(img);


GaussianBlur(img, img, Size(3, 3), 0, 0);

img = kovac(img);
img = skin_color(img);
//show_img(img_url, img);
//if (cal_img(img_url, img) == false)
//{

show_img(imagegauname, img);
show_img(imageoriname, src);
//show_img(imagegauname, img);
//}


}

void pro_normal(const char *img_url, Mat& src)
{

char imageoriname[100];
Mat img;

sprintf(imageoriname, "ori %s", img_url);

src.copyTo(img);
img = kovac(img);
img = skin_color(img);
//show_img(img_url, img);
//if (cal_img(img_url, img) == false)
//{
show_img(imageoriname, src);
show_img(img_url, img);
//}


}

void pro_canny(const char *img_url, Mat& src)
{

char imageoriname[100];
char imagecaname[100];
Mat img;

Mat src_gray;
Mat detected_edges;

sprintf(imageoriname, "ori %s", img_url);
sprintf(imagecaname, "ca %s", img_url);

img.create(src.size(), src.type());

cvtColor(src, src_gray, CV_BGR2GRAY);                        //rgb¸¦ ±×·¹ÀÌ ¿µ»óÀ¸·Î


blur(src_gray, detected_edges, Size(3, 3));
Canny(detected_edges, detected_edges, 10, 30, 3);

img = Scalar::all(0);

src.copyTo(img, detected_edges);



show_img(imagecaname, img);
show_img(imageoriname, src);

}

void pro_canny2(const char *img_url, Mat& src)
{


char imagecaname[100];
char imageoriname[100];
Mat img;
Mat black;
black = Scalar::all(0);

sprintf(imageoriname, "ori %s", img_url);
sprintf(imagecaname, "ca %s", img_url);

src.copyTo(img);
img = kovac(img);
img = skin_color(img);
//show_img(img_url, img);

Canny(img, img, 40, 120, 3);

src.copyTo(black, img);


show_img(imageoriname, src);
show_img(imagecaname, black);
}


Mat equalize_hist(Mat& src)
{
Mat channel[3];
split(src, channel);

equalizeHist(channel[0], channel[0]);
equalizeHist(channel[1], channel[1]);
equalizeHist(channel[2], channel[2]);

merge(channel, 3, src);

return src;
}

*/

/*
uchar **input_R, **input_G, **input_B;
uchar **nor_R, **nor_G, **nor_B;


RGBSum = (int **)calloc(SizeY, sizeof(int *));
for (i = 0; i<SizeY; i++) {
RGBSum[i] = (int *)calloc(SizeX, sizeof(int));
}

input_R = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
input_R[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}
input_G = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
input_G[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}
input_B = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
input_B[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}
nor_R = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
nor_R[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}
nor_G = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
nor_G[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}
nor_B = (uchar **)calloc(SizeY, sizeof(uchar *));
for (i = 0; i<SizeY; i++) {
nor_B[i] = (uchar *)calloc(SizeX, sizeof(uchar));
}



for (i = 0; i < SizeY; i++)
{
for (j = 0; j < SizeX; j++)
{
RGBSum[i][j] = (int)(input_R[i][j] + input_G[i][j] + input_B[i][j]);

if (RGBSum[i][j] > 10)
{
nor_R[i][j] = (255 * input_R[i][j]) / RGBSum[i][j];
nor_G[i][j] = (255 * input_G[i][j]) / RGBSum[i][j];
nor_B[i][j] = (255 * input_B[i][j]) / RGBSum[i][j];
}

if (RGBSum[i][j] <= 10)
{
nor_R[i][j] = 0;
nor_G[i][j] = 0;
nor_B[i][j] = 0;
}
}
}
*/


/*
Vec3b lower = Vec3b(120,140, 180); //B, G, R
Vec3b upper = Vec3b(225, 240, 255);
Vec3b with = Vec3b(255, 255, 255);

inRange(image, lower, upper, skin);
image.setTo(with, skin);
*/

/*
Mat efyun(Mat& src)                            //¶óº§¸µ ÇÔ¼ö
{
	int label = 2;
	int i, j;
	int count;
	Mat mark;

	src.copyTo(mark);

	int th = mark.rows*mark.cols*0.025;


	for (i = 1; i <mark.rows - 2; i++)
	{
		for (j = 1; j < mark.cols - 2; j++)
		{
			if (mark.at<Vec3b>(i, j)[0] == 255 &&
				mark.at<Vec3b>(i, j)[1] == 255 &&
				mark.at<Vec3b>(i, j)[2] == 255)
			{
				mark.at<Vec3b>(i, j)[0] = 1;
				mark.at<Vec3b>(i, j)[1] = 1;
				mark.at<Vec3b>(i, j)[2] = 1;
			}
		}
	}
	for (j = 0; j < mark.cols - 2; j++)
	{
		mark.at<Vec3b>(0, j)[0] = 0;
		mark.at<Vec3b>(0, j)[1] = 0;
		mark.at<Vec3b>(0, j)[2] = 0;
		mark.at<Vec3b>(mark.rows - 1, j)[0] = 0;
		mark.at<Vec3b>(mark.rows - 1, j)[1] = 0;
		mark.at<Vec3b>(mark.rows - 1, j)[2] = 0;
	}
	for (j = 0; j < mark.rows - 2; j++)
	{
		mark.at<Vec3b>(j, 0)[0] = 0;
		mark.at<Vec3b>(j, 0)[1] = 0;
		mark.at<Vec3b>(j, 0)[2] = 0;
		mark.at<Vec3b>(0, mark.cols - 1)[0] = 0;
		mark.at<Vec3b>(0, mark.cols - 1)[1] = 0;
		mark.at<Vec3b>(0, mark.cols - 1)[2] = 0;
	}



	for (i = 1; i <mark.rows - 2; i++)
	{
		for (j = 1; j < mark.cols - 2; j++)
		{
			if (mark.at<Vec3b>(i, j)[0] == 1 &&
				mark.at<Vec3b>(i, j)[1] == 1 &&
				mark.at<Vec3b>(i, j)[2] == 1)
			{
				count = eff_flood_fill(mark, i, j, label, 1);
				if (count > th)
					eff_flood_fill(mark, i, j, 255, label);
				else
					eff_flood_fill(mark, i, j, 0, label);
			}
		}
	}

	return mark;
}


int eff_flood_fill(Mat& src, int j, int i, int label, int search)                   //floodfill
{
	std::queue<xy> Queue;
	int left;
	int right;
	int c;
	int count = 0;

	xy temp;
	temp.y = j;
	temp.x = i;

	Queue.push(temp);

	while (!Queue.empty())
	{
		xy num = Queue.front();
		Queue.pop();

		if (src.at<Vec3b>(num.y, num.x)[0] == search &&
			src.at<Vec3b>(num.y, num.x)[1] == search &&
			src.at<Vec3b>(num.y, num.x)[2] == search)
		{
			left = num.x;
			right = num.x;

			while (src.at<Vec3b>(num.y, left - 1)[0] == search &&
				src.at<Vec3b>(num.y, left - 1)[1] == search &&
				src.at<Vec3b>(num.y, left - 1)[2] == search)
				left--;
			while (src.at<Vec3b>(num.y, right + 1)[0] == search &&
				src.at<Vec3b>(num.y, right + 1)[1] == search &&
				src.at<Vec3b>(num.y, right + 1)[2] == search)
				right++;
			for (c = left; c <= right; c++)
			{
				src.at<Vec3b>(num.y, c)[0] = label;
				src.at<Vec3b>(num.y, c)[1] = label;
				src.at<Vec3b>(num.y, c)[2] = label;
				count++;


				if (src.at<Vec3b>(num.y - 1, c)[0] == search &&
					src.at<Vec3b>(num.y - 1, c)[1] == search &&
					src.at<Vec3b>(num.y - 1, c)[2] == search &&
					(c == left || (src.at<Vec3b>(num.y - 1, c - 1)[0] != search &&
						src.at<Vec3b>(num.y - 1, c - 1)[0] != search &&
						src.at<Vec3b>(num.y - 1, c - 1)[0] != search)
						)
					)
				{
					xy temp2;
					temp2.y = num.y - 1;
					temp2.x = c;
					Queue.push(temp2);
				}
				if (src.at<Vec3b>(num.y + 1, c)[0] == search &&
					src.at<Vec3b>(num.y + 1, c)[1] == search &&
					src.at<Vec3b>(num.y + 1, c)[2] == search &&
					(c == left || (src.at<Vec3b>(num.y - 1, c - 1)[0] != search &&
						src.at<Vec3b>(num.y + 1, c - 1)[0] != search &&
						src.at<Vec3b>(num.y + 1, c - 1)[0] != search)
						)
					)
				{
					xy temp2;
					temp2.y = num.y + 1;
					temp2.x = c;
					Queue.push(temp2);
				}
			}
		}
	}

	return count;
}
*/



/*
// Á¤±ÔºÐÆ÷¸¦ ±â¹ÝÀ¸·ÎÇÑ ÇÇºÎ»ö ÃßÃâ
Mat make_gaussian(Mat src, int lr, int hr, int lg, int hg, int dr, int dg, int th)
{
	uchar *Lut_norR, *Lut_norG;
	int i, j;
	Mat img;
	src.copyTo(img);

	Lut_norR = (uchar *)calloc(256, sizeof(uchar));
	Lut_norG = (uchar *)calloc(256, sizeof(uchar));


	Gaussian1D(Lut_norR, lr, hr, dr*dr, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norG, lg, hg, dg*dg, 256);  //Gaussian1D(Lut_norG, Skin_low_mG, Skin_high_mG, SDev_G*SDev_G, 256);


	for (j = 0; j < img.rows; j++)
	{
		for (i = 0; i < img.cols; i++)
		{
			uchar skin_tmp = Lut_norR[img.at<Vec3b>(j, i)[2]] * Lut_norG[img.at<Vec3b>(j, i)[1]] / 255;
			if (skin_tmp> th)
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 255;
			}
			else
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}

	return img;
}


Mat make_gaussian_ycrcb(Mat src, int ly, int hy, int lr, int hr, int lb, int hb, int dy, int dr, int db, int th)   //ycbcr
{
	uchar *Lut_norR, *Lut_norB, *Lut_norY;
	int i, j;
	Mat img;
	src.copyTo(img);


	Lut_norY = (uchar *)calloc(256, sizeof(uchar));
	Lut_norR = (uchar *)calloc(256, sizeof(uchar));
	Lut_norB = (uchar *)calloc(256, sizeof(uchar));

	Gaussian1D(Lut_norY, ly, hy, dy*dy, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norR, lr, hr, dr*dr, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norB, lb, hb, db*db, 256);  //Gaussian1D(Lut_norG, Skin_low_mG, Skin_high_mG, SDev_G*SDev_G, 256);


	for (j = 0; j < img.rows; j++)
	{
		for (i = 0; i < img.cols; i++)
		{
			uchar skin_tmp = Lut_norY[img.at<Vec3b>(j, i)[0]] * Lut_norR[img.at<Vec3b>(j, i)[1]] * Lut_norB[img.at<Vec3b>(j, i)[2]] / (255 * 255);
			if (skin_tmp> th)
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 255;
			}
			else
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}
	return img;
}


Mat make_gaussian_hls(Mat src, int lh, int hh, int ll, int hl, int ls, int hs, int dh, int dl, int ds, int th)
{
	uchar *Lut_norH, *Lut_norL, *Lut_norS;
	int i, j;
	Mat img;
	src.copyTo(img);

	Lut_norH = (uchar *)calloc(256, sizeof(uchar));
	Lut_norL = (uchar *)calloc(256, sizeof(uchar));
	Lut_norS = (uchar *)calloc(256, sizeof(uchar));


	Gaussian1D(Lut_norH, lh, hh, dh*dh, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);
	Gaussian1D(Lut_norL, ll, hl, dl*dl, 256);  //Gaussian1D(Lut_norG, Skin_low_mG, Skin_high_mG, SDev_G*SDev_G, 256);
	Gaussian1D(Lut_norS, ls, hs, ds*ds, 256);  //Gaussian1D(Lut_norR, Skin_low_mR, Skin_high_mR, SDev_R*SDev_R, 256);


	for (j = 0; j < img.rows; j++)
	{
		for (i = 0; i < img.cols; i++)
		{
			//uchar skin_tmp = a[img.at<Vec3b>(j, i)[2]] * b[img.at<Vec3b>(j, i)[1]] / 255;
			uchar skin_tmp = Lut_norS[img.at<Vec3b>(j, i)[2]] * Lut_norL[img.at<Vec3b>(j, i)[1]] * Lut_norH[img.at<Vec3b>(j, i)[0]] / (255 * 255);
			if (skin_tmp> th)
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 255;
			}
			else
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}
	return img;
}

*/





/*
Mat normal_rgb(Mat& src)
{
	Mat img;
	int i, j;

	src.copyTo(img);

	//R = rgb.at<Vec3b>(j, i)[2];
	for (j = 0; j < img.rows; j++)
	{
		for (i = 0; i < img.cols; i++)
		{
			int R, G, B;
			int RGB_sum;
			R = img.at<Vec3b>(j, i)[2];
			G = img.at<Vec3b>(j, i)[1];
			B = img.at<Vec3b>(j, i)[0];
			RGB_sum = R + G + B;

			if (RGB_sum > 10)
			{
				img.at<Vec3b>(j, i)[2] = (255 * R) / RGB_sum;
				img.at<Vec3b>(j, i)[1] = (255 * G) / RGB_sum;
				img.at<Vec3b>(j, i)[0] = (255 * B) / RGB_sum;
			}
			else
			{
				img.at<Vec3b>(j, i)[2] = 0;
				img.at<Vec3b>(j, i)[1] = 0;
				img.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}
	return img;
}
*/

/*
Mat ycrcb_detect_pointer(Mat& src)                                           //ycrcb ÄÃ·¯¿µ¿ªÀ» ÀÌ¿ëÇÑ ±¸ºÐ
{
	Mat img;
	Mat skin;
	int row, col;
	src.copyTo(img);

	cvtColor(img, img, CV_BGR2YCrCb);                               //ycrcb º¯È¯

	int R, G, B;

	for (row = 0; row < img.rows; ++row)
	{
		uchar *ptr = img.ptr(row);
		for (col = 0; col < img.cols; col++)
		{
			uchar * uc_pixel = ptr;
			B = uc_pixel[0];
			G = uc_pixel[1];
			R = uc_pixel[2];
			ptr += 3;

			if ((R >= 77 && R <= 127) && (G >= 133 && G <= 173) && (B >= 0 && B <= 255))
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 255;
			}
			else
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 0;
			}
		}
	}

	//inRange(img, Scalar(0, 133, 77), Scalar(255, 173, 127), skin);

	return img;
}
*/
/*
//1.È÷½ºÅä±×·¥ ºÐÆ÷(ÆÄÀÏ)
		case 1:
			init();                                         //È÷½ºÅä±×·¥À» ¸¸µé±â À§ÇÑ µ¿ÀûÇÒ´ç
			while (1)
			{
				sprintf(imagename, "C:/Users/user/Desktop/¼÷Á¦/capstone/ÀÚ·á/input/%d.jpg", count_img);
				image = imread(imagename, CV_LOAD_IMAGE_COLOR);
				if (!image.data)                              // Check for invalid input
				{
					count_img++;
					if (count_img >= 930)
					{
						break;
					}
					continue;
				}
				//cvtColor(image, image, CV_BGR2HLS); // CV_BGR2YCrCb,CV_BGR2HLS
				image = normal_rgb_pointer(image);
				resize(image, image, Size(320, 320));
				size_sum += image.rows * image.cols;

				Mat channel[3];
				split(image, channel);
				sumhist(channel[0], 0);
				sumhist(channel[1], 1);
				sumhist(channel[2], 2);

				count_img++;
			}
			show_hist("b", 256, 0);
			show_hist("g", 256, 1);
			show_hist("r", 256, 2);
			waitKey(0);
			break;
			*/




/*
static void getThreashold(int, void*)
{

}

void trackbar(const char *img_url, Mat& src)
{
	Mat img;
	src.copyTo(img);

	namedWindow("result", WINDOW_AUTOSIZE);


	createTrackbar("low red", "result", &low_red, 255, getThreashold);
	setTrackbarPos("low red", "result", 85);


	createTrackbar("high red", "result", &high_red, 255, getThreashold);
	setTrackbarPos("high red", "result", 85);


	createTrackbar("low green", "result", &low_green, 255, getThreashold);
	setTrackbarPos("low green", "result", 85);


	createTrackbar("high green", "result", &high_green, 255, getThreashold);
	setTrackbarPos("high green", "result", 85);

	while (1)
	{

		src.copyTo(img);
		low_red = getTrackbarPos("low red", "result");
		high_red = getTrackbarPos("high red", "result");
		low_green = getTrackbarPos("low green", "result");
		high_green = getTrackbarPos("high green", "result");

		img = make_gaussian_pointer(normal_rgb_pointer(img), low_red, high_red, low_green, high_green, 10, 10, 140);

		imshow("result", img);
		int key = waitKey(1);
		if (key == 'q')break;
	}
}

void sumhist(Mat& channel, int chnum)
{
	int i, j, bin;
	for (j = 0; j < channel.rows; j++)
	{
		for (i = 0; i < channel.cols; i++)
		{
			bin = channel.at<uchar>(j, i);
			ihist[chnum][bin]++;
		}
	}
}

void show_hist(const char *channel, int range, int chnum)
{
	int i, j;
	double max, sum, var, mean;
	int maxloc;

	max = sum = var = mean = 0;
	maxloc = 0;

	char filename[50];

	for (i = 1; i < range; i++)
	{
		hist[chnum][i] = ihist[chnum][i] / (double)(size_sum - (ihist[chnum][0]));
	}

	for (i = 1; i < range; i++)
	{
		if (max < hist[chnum][i])
		{
			max = hist[chnum][i];
			maxloc = i;
		}
	}

	for (i = 1; i < range; i++)
	{
		mean += (hist[chnum][i] * i);
	}


	for (i = 1; i < 256; i++)
	{
		sum += hist[chnum][i] * i * i;
	}


	for (j = 0; j < 3; j++)
	{
		var = sum - (mean * mean);
	}

	cout << "mean :" << mean << "\tvar :" << var << "\tmax :" << maxloc << endl;

	sprintf(filename, "C:/Users/user/Desktop/¼÷Á¦/capstone/ÀÚ·á/%s.txt", channel);
	ofstream histout;
	histout.open(filename);

	for (i = 0; i < 256; i++)
	{
		histout << i << "\t" << ihist[chnum][i] << endl;
	}

	Mat temp = draw_histogram(hist[chnum]);
	namedWindow(channel);
	imshow(channel, temp);
	sprintf(filename, "C:/Users/user/Desktop/¼÷Á¦/capstone/ÀÚ·á/%s.jpg", channel);
	save_img(channel, filename, temp);
}



//È÷½ºÅä±×·¥ Ãâ·Â
Mat draw_histogram(double hist[])
{
	double vmax = -1.0;
	Mat himg(256, 256, CV_8U, Scalar(255));

	for (int i = 0; i < 256; i++) vmax = (vmax > hist[i]) ? vmax : hist[i];

	for (int i = 0; i < 256; i++) {
		Point p1(i, 256), p2(i, 256 - (int)((hist[i] / vmax)*(0.8 * 256)));
		line(himg, p1, p2, Scalar(0));
	}

	return himg;
}
*/


/*
Mat kovac(Mat& src)                                                //RGB ÄÃ·¯¿µ¿ªÀ» ÀÌ¿ëÇÑ ±¸ºÐ 
{
	int i, j;
	Mat rgb;

	src.copyTo(rgb);

	for (j = 0; j < rgb.rows; j++)
	{
		for (i = 0; i < rgb.cols; i++)
		{
			int R, G, B;
			R = rgb.at<Vec3b>(j, i)[2];
			G = rgb.at<Vec3b>(j, i)[1];
			B = rgb.at<Vec3b>(j, i)[0];

			if ((R > 95 && G > 40 && B > 20) && ((MAX(MAX(R, G), B) - MIN(MIN(R, G), B)) > 15) && abs(R - G) > 15 && (R > G) && (R > B))
			{
				// rgb.at<Vec3b>(j, i)[2] = rgb.at<Vec3b>(j, i)[1] = rgb.at<Vec3b>(j, i)[0] = 255;
			}
			else
			{
				rgb.at<Vec3b>(j, i)[2] = rgb.at<Vec3b>(j, i)[1] = rgb.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}
	return rgb;
}


Mat hls_detect(Mat& src)                                     //hls ÄÃ·¯¿µ¿ªÀ» ÀÌ¿ëÇÑ ±¸ºÐ 
{

	Mat hlsImg;
	Mat skinImg;

	skinImg = src.clone();

	cvtColor(skinImg, hlsImg, CV_BGR2HLS);                             //hls·Î º¯È¯ 
	vector<Mat> hls_images(3);
	split(hlsImg, hls_images);                                         // Ã¤³Îº°·Î ºÐÇÒ

	for (int row = 0; row < hlsImg.rows; row++)
	{
		for (int col = 0; col < hlsImg.cols; col++)
		{
			uchar H = hlsImg.at<Vec3b>(row, col)[0];
			uchar L = hlsImg.at<Vec3b>(row, col)[1];
			uchar S = hlsImg.at<Vec3b>(row, col)[2];

			double LS_ratio = ((double)L) / ((double)S);
			bool skin_pixel = (S >= 50) && (LS_ratio > 0.5) && (LS_ratio < 3.0) && ((H <= 14) || (H >= 165));

			if (skin_pixel == false)
			{
				skinImg.at<Vec3b>(row, col)[0] = 0;
				skinImg.at<Vec3b>(row, col)[1] = 0;
				skinImg.at<Vec3b>(row, col)[2] = 0;
			}
			else
			{
			}
		}
	}
	return skinImg;
}
*/
/*
void pro_merge(const char *img_url, Mat& src, ofstream& file, ofstream& file2)
{

	char imageoriname[100];
	char imagelaname[100];
	Mat img;
	Mat rgb;
	Mat hls;
	Mat ycrcb;
	Mat merge;
	Mat nor;


	sprintf(imageoriname, "%d ori %s", count_img, img_url);
	sprintf(imagelaname, "%d la %s", count_img, img_url);

	show_img("¿À¸®Áö³¯", src);

	img = cropimg(src);
	show_img("ÀÚ¸¥°Í", img);

	rgb = kovac(img);
	show_img("rgb", rgb);

	hls = hls_detect(img);
	show_img("hls", hls);

	ycrcb = ycrcb_detect(img);
	show_img("ycrcb", ycrcb);

	nor = normalize_rgb(img);
	nor = make_gaussian_rgb(nor, 140);
	show_img("nor", nor);


	show_img("ÇÕÄ£°Í", merge);

	waitKey(0);

	erode(img, img, Mat(), Point(-1, -1), 1);
	dilate(img, img, Mat(), Point(-1, -1), 1);

	if (cal_img(img_url, img) == true)
	{
		count_fal++;

		show_img(imageoriname, src);
		show_img(imagelaname, img);
		file2 << img_url << endl;
	}
	else
	{
		count_cor++;
		file << img_url << endl;
	}
}


Mat ycrcb_detect(Mat& src)                                           //ycrcb ÄÃ·¯¿µ¿ªÀ» ÀÌ¿ëÇÑ ±¸ºÐ
{
	Mat img;
	Mat skin;
	int i, j;
	src.copyTo(img);

	cvtColor(img, img, CV_BGR2YCrCb);                               //ycrcb º¯È¯

	for (j = 0; j < img.rows; j++)
	{
		for (i = 0; i < img.cols; i++)
		{
			int R, G, B;
			R = img.at<Vec3b>(j, i)[2];
			G = img.at<Vec3b>(j, i)[1];
			B = img.at<Vec3b>(j, i)[0];

			if ((R >= 77 && R <= 127) && (G >= 133 && G <= 173) && (B >= 0 && B <= 255))
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 255;
			}
			else
			{
				img.at<Vec3b>(j, i)[2] = img.at<Vec3b>(j, i)[1] = img.at<Vec3b>(j, i)[0] = 0;
			}
		}
	}
	//inRange(img, Scalar(0, 133, 77), Scalar(255, 173, 127), skin);

	return img;
}
*/


/*
Mat merge_detect(Mat& a, Mat& b)                           //°¢°¢ ±¸ºÐÀ» ÇÑ ¿µ»óÀ» ÇÕÄ¡´Â ÇÔ¼ö ÁøÇàÁß
{
	Mat img;
	a.copyTo(img);

	for (int row = 0; row < a.rows; ++row)
	{
		uchar *ptr1 = a.ptr(row);
		uchar *ptr2 = b.ptr(row);
		uchar *ptr4 = img.ptr(row);

		for (int col = 0; col < a.cols; col++)
		{
			if (ptr1[col] > 0 || ptr2[col] > 0)
			{
				ptr4[col] = 255;
			}
			else
				ptr4[col] = 0;
		}
		ptr1 += 3;
		ptr2 += 3;
		ptr4 += 3;
	}

	return img;
}




Mat normalize_rgb(Mat& src)
{
	Mat img;
	int row, col;

	src.copyTo(img);

	int R, G, B;
	int RGB_sum;

	for (row = 0; row < img.rows; ++row)
	{
		uchar *ptr = img.ptr(row);
		for (col = 0; col < img.cols; col++)
		{
			uchar * uc_pixel = ptr;
			B = uc_pixel[0];
			G = uc_pixel[1];
			R = uc_pixel[2];
			RGB_sum = R + G + B;
			ptr += 3;

			//if ((R > 5 && G > 5 && B > 5) && (R < 250 && G < 250 && B < 250))
			if (RGB_sum>310)
			{
				uc_pixel[2] = (255 * R) / RGB_sum;
				uc_pixel[1] = (255 * G) / RGB_sum;
				uc_pixel[0] = (255 * B) / RGB_sum;
			}
			else
			{
				uc_pixel[2] = 0;
				uc_pixel[1] = 0;
				uc_pixel[0] = 0;
			}
		}
	}

	return img;
}

// Á¤±ÔºÐÆ÷¸¦ ±â¹ÝÀ¸·ÎÇÑ ÇÇºÎ»ö ÃßÃâ
Mat make_gaussian_rgb(Mat src, int th)
{
	int row, col;
	Mat img;
	src.copyTo(img);

	for (row = 0; row < img.rows; ++row)
	{
		uchar *ptr = img.ptr(row);
		//uchar *ptr2 = skin_loc.ptr(row);
		for (col = 0; col < img.cols; col++)
		{
			uchar * uc_pixel = ptr;
			//uchar * uc_pixel2 = ptr2;

			uchar skin_tmp = (Lut_norR[uc_pixel[2]] * Lut_norG[uc_pixel[1]]) / (float)255;

			//uchar skin_tmp =( Lut_norR[uc_pixel[2]] * Lut_norG[uc_pixel[1]] * uc_pixel2[0]) / (255*255);
			if (skin_tmp> th)
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 255;
			}
			else
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 0;
			}
			ptr += 3;
			//ptr2 += 3;
		}
	}

	return img;
}




Mat make_gaussian_hls(Mat src, int th)
{
	int row, col;
	Mat img;
	src.copyTo(img);

	for (row = 0; row < img.rows; ++row)
	{
		uchar *ptr = img.ptr(row);
		for (col = 0; col < img.cols; col++)
		{
			uchar * uc_pixel = ptr;
			uchar skin_tmp = Lut_norH[uc_pixel[0]] * Lut_norL[uc_pixel[1]] * Lut_norS[uc_pixel[2]] / (255 * 255);
			if (skin_tmp> th)
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 255;
			}
			else
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 0;
			}
			ptr += 3;
		}
	}
	return img;
}
*/

//image = imread("C:/Users/user/Desktop/¼÷Á¦/capstone/ÀÚ·á/testcase/155.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file(jpg)
	//image =loadGIF("2990_shop1_529603_dburl.gif");                     //Read the file(gif)
	//image = curlImg("http://image.lotte.com/goods/44/09/02/00/1/100020944_1_280.jpg");



/*
void collect_name(char *li,unordered_map<string, int> word, map<string, int> &name)  //collect word from title
{
	char *p_token = NULL;
	char delimit_string[] = "\t, /[]<>()-#|\\+_{}&?';:!~@$%^&*\".";
	p_token = strtok(li, delimit_string);
	while (p_token != NULL){

		if (word.find(p_token) != word.end())	// 상품명과 카테고리에 걸러내고자 하는 단어가 있는 경우를 처리
		{
		}
		else
		{
			//unordered_map<string,int>::iterator finder = name.find(p_token);
			//if(finder != name.end())
			//	finder->second+=1;
			//else
			name.insert(pair<string, int>(p_token, 1));
		}

		p_token = strtok(NULL, delimit_string);
	}
}
*/