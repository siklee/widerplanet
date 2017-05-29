/*************************************************************************************
*                                                                                    *
*                     kookmin university capstone project wider planet               *
*                        professor yoon sang min     team ms dj                      *
*                                                                                    *
**************************************************************************************/
//input format name \t purl \t iurl \t c1 \t c2 \t c3 \t c4
// opencvgif,opencv2413,curl7.52.1

//BGR variance ,standard deviation B 13 G 12  R 18  R : 96, 98 G : 81, 83
//Ycrcb variance ,standard deviation   y20 cr15 cb 15  Y:186,188 Cr: 145, 147 Cb: 110, 112
//HLS variance ,standard deviation  h 36 l 63 s 43     H: 10, 12 L: 184,186   S 86 , 88

#include "detect_skin.hpp"


int main()
{
	Mat image;                           //image
	char imagename[1000];                //url address
	int select; //switch

	// vec[0]          vec[1]  vec[2]   vec[3]   vec[4]   vec[5]   vec[6]
	// product name    purl    iurl     c1       c2        c3      c4

	DIR *dirp;
	struct dirent *dp;

	clock_t begin, end;

	int result = 1;

	ifstream file;
	file.open("input.txt");                              // input
	ofstream ofile;
	ofile.open("output.txt");                            // output
	ofstream tfile;
	tfile.open("unsafe.txt");                            // only unsafe name
	ofstream ffile;
	ffile.open("safe.txt");                              // only sage name
	

	init();   // make gaussian normal distribution
	
	while (1)
	{
		cout << "1.file input 2.URL input 3.save image(url)" << endl;
		cin >> select;

		switch (select)
		{		
		case 1:    // file input
    		begin = clock();
			dirp = opendir("../image/");  //search file in image folder
			if (dirp == NULL)
			{
				printf("There were no files.\n");
				break;
			}
			while ((dp = readdir(dirp)) != NULL)
			{
				if (strstr(dp->d_name, "gif"))     //gif image
				{
					sprintf(imagename, "../image/%s", dp->d_name);
					image = loadGIF(imagename);
					count_img++;
				}
				else if (strstr(dp->d_name, "jpg") || strstr(dp->d_name, "png"))    //jpg image
				{
					sprintf(imagename, "../image/%s", dp->d_name);
					image = imread(imagename, CV_LOAD_IMAGE_COLOR);
					count_img++;
				}
				else
				{
					continue;
				}
				
				if (!image.data)                              // Check for invalid input
				{
					cout << "Could not open or find the image" << std::endl;
					continue;
				}

				pro_detect(dp->d_name, image, ofile,tfile,ffile, 3);    //detect skin color
			}
			closedir(dirp);
			cout << count_fal / (count_cor + count_fal) * 100 << "%" << endl;
			cout << "unsafe:" << count_fal << "total:" << (count_cor + count_fal) << endl;
			end = clock(); 
    		cout<<"execute time : "<<((end-begin)/CLOCKS_PER_SEC)<<endl;
			waitKey(0);

			file.close();
			ofile.close();
			return 0;		
		case 2:    // URL input
    		begin = clock();
			system("mkdir \"../temp\"");  //make folder 
			while (1)
			{

				vector<string> vec;
				String line;
				file.getline(imagename, 1000);
				if(file.fail())
					break;

				line = imagename;

				vec = split(line, '\t');

				strcpy(imagename,vec[2].c_str());

				if (strstr(imagename, "gif"))              // gif image
				{
					image = curlgifImg(imagename, "../temp/%d.gif");   //load image 
					count_img++;
				} 
				else if (strstr(imagename, "jpg") || strstr(imagename, "png"))   // jpg and png image
				{
					image = curlImg(imagename);                      //load image
					count_img++;
				}												
				else
				{
					cout << "Could not open or find the image" << std::endl;
					cout << imagename << std::endl;
					continue;
				}

				if (!image.data)                              // Check for invalid input
				{
					cout << "Could not open or find the image" << std::endl;
					cout << imagename << std::endl;
					continue;
					
				}
				pro_detect(imagename, image, ofile,tfile,ffile,3);          //detect skin color
				image.release();

			}
			system("rm -r \"../temp\"");
			cout << count_fal / (count_cor + count_fal) * 100 << "%" << endl;
			cout << "false:" << count_fal << "total:" << (count_cor + count_fal) << endl;
			end = clock(); 
    		cout<<"수행시간 : "<<((end-begin)/CLOCKS_PER_SEC)<<endl;
			waitKey(0);    

			file.close();
			ofile.close();
			return 0;
		case 3:    //save image(url)
			while (1)
			{
				vector<string> vec;
				String line;
				file.getline(imagename, 1000);

				if (file.fail())
					break;
				
				line = imagename;
				vec = split(line, '\t');

				strcpy(imagename, vec[2].c_str());

				if (strstr(imagename, "gif"))              // gif image
				{
					image = curlgifImg(imagename, "../image/%d.gif");    //load image and save
					count_img++;
				}
				else if (strstr(imagename, "jpg"))      //jpg image
				{
					image = curlImg(imagename);                      //load image
					save_img(imagename,"../image/%d.jpg",image);     //save image
					count_img++;
				}
				else if(strstr(imagename, "png"))            //png image
				{
					image = curlImg(imagename);                      //load image 
					save_img(imagename,"../image/%d.png",image);     //save image
					count_img++;
				}
				else
				{
					cout << "Could not open or find the image" << std::endl;
					cout << imagename << std::endl;
					continue;
				}


				if (!image.data)                              // Check for invalid input
				{
					cout << "Could not open or find the image" << std::endl;
					cout << imagename << std::endl;
					continue;
				}
			}

			file.close();
			ofile.close();
			return 0;
		default:
			break;
		}
	}
}

//img_url : window name   src : image 
void show_img(const char *img_url, Mat& src)    //  show image use window                                
{
	namedWindow(img_url, WINDOW_AUTOSIZE);      //popup window
	imshow(img_url, src);                       //show image
}

//img_url : image name  path : folder path   Mat&src : image
void save_img(const char *img_url,const char *path, Mat& src)    //save image                                     
{
	char imagecopyname[1000];
	Mat img;

	sprintf(imagecopyname, path, count_img); //combine  image name and path

	cout << count_img << " " << img_url << endl;   //notice save
	imwrite(imagecopyname, src);                 //save image
}

//img_name : image name  src : image
//ofile : output file   tfile : only record unsafe file   ffile : only record safe file   
//th : thrash hold
void pro_detect(const char *img_name, Mat& src, ofstream& ofile,ofstream& tfile,ofstream& ffile,int th)               //skin detection
{
	Mat img;

	img = cropimg(src);
	resize(img, img, Size(Fix_row, Fix_col));
	//ycbcr  variance y20 cr15 cb 15
	cvtColor(img, img, CV_BGR2YCrCb); // convert color rgb to ycbcr
	img = make_gaussian_ycrcb(img, th); //(img,186,188, 145, 147, 110, 112, 40, 10, 10)

	erode(img, img, Mat(), Point(-1, -1), 1);   //delete noise
	dilate(img, img, Mat(), Point(-1, -1), 1);								 

	img = label(img, 600);     //delete small pieces

	if (cal_img(img_name, img) == true)           // calculate skin ratio
	{
		count_fal++;                                                         
		ofile << img_name << "\t" << "unsafe" << endl;
		tfile << img_name << endl;
	}
	else
	{
		count_cor++;                                                         
		ofile << img_name << "\t" << "safe" << endl;
		ffile << img_name << endl;
	}
}



//img_url : image name  Mat& src image
bool cal_img(const char *img_url, Mat& src)     // calculate skin ratio
{
	float skinColor = 0;                          //skin pixel
	float normalColor = 0;                        //non skin pixel
	int row,col;
	float similarity;

	for (row = 0; row < src.rows; ++row)
	{
		uchar *ptr = src.ptr(row);
		for (col = 0; col < src.cols; col++)
		{
			if (ptr[col] > 0 )
				skinColor = skinColor + 1;
			else
				normalColor = normalColor + 1;
		}
		ptr += 3;
	}

	similarity = (skinColor / (skinColor + normalColor)) * 100;
	//cout << count_img << ": " << similarity << "% " << img_url << endl;

	if (count_img % 50 == 0)                                 //notice progress
	{
		cout << "***********************************" << endl;
		cout << "count img . . " << count_img << endl;
		cout << count_fal / (count_cor + count_fal) * 100 << "%" << endl;
		cout << "unsafe:" << count_fal << "total:" << (count_cor + count_fal) << endl;
		cout << "***********************************" << endl;
	}
	if (similarity > 13)                          // if skin ratio > 13%  detect unsafe image  
		return true;
	else
		return false;
}


//src : image name
Mat cropimg(Mat& src)                               //crop image
{
	Mat img;
	Mat crop;

	src.copyTo(img);

	int crop_row;
	int crop_col;
	int width;
	int height;

	crop_row = src.rows * 0.15;                                 //crop edge 15%
	crop_col = src.cols * 0.15;

	width = src.rows * 0.7;
	height = src.cols * 0.7;

	Rect rect(crop_col, crop_row, height, width);
	crop = img(rect);
	return crop;
}

//GD : histogram   mean_a : low color range mean_b : high color range    var :  variance  num : color range =256
void Gaussian1D(unsigned char *GD, double mean_a, double mean_b, double var, int num) //make gaussian normal distribution
{

	double temp[256];
	int i;

	for (i = 0; i<num; i++) {
		if (i<mean_a)
			temp[i] = exp(-1.*((double)i - mean_a)*((double)i - mean_a) / var);
		else if ((i >= mean_a) && (i <= mean_b))     // make peak flat
			temp[i] = 1.0;
		else
			temp[i] = exp(-1.*((double)i - mean_b)*((double)i - mean_b) / var);
	}
	double min = 1.0;
	double max = 0.0;

	for (i = 0; i<num; i++) {
		if (temp[i] < min)		min = temp[i];
		if (temp[i] > max)		max = temp[i];
	}

	double mag = max - min;

	for (i = 0; i<num; i++) {
		GD[i] = (unsigned char)(((temp[i] - min) / mag) * 255);
	}

}

//src : image th : threash hold
Mat make_gaussian_ycrcb(Mat src,int th)   //detect skin use gaussian normal distribution
{
	int row,col;
	Mat img;
	src.copyTo(img);

	for (row = 0; row < img.rows; ++row)
	{
		uchar *ptr = img.ptr(row);
		for (col = 0; col < img.cols; col++)
		{
			uchar * uc_pixel = ptr;
			uchar skin_tmp = Lut_norY[uc_pixel[0]] * Lut_norCR[uc_pixel[1]] * Lut_norCB[uc_pixel[2]] / (255 * 255);   //find skin pixel
			if (skin_tmp> th)
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 255;   // make skin pixel white
			}
			else
			{
				uc_pixel[2] = uc_pixel[1] = uc_pixel[0] = 0;     // make skin pixel black
			}
			ptr += 3;
		}
	}
	return img;
}

// inpitImg : image  threshold : threshold
Mat label(Mat inpitImg, int threshold)  // delete small piece
{
	CBlobLabeling blob;

	{
		Mat im;
		cvtColor(inpitImg, im, CV_RGB2GRAY);
		
		IplImage copy;

		copy = im;

		IplImage *trans = &copy;



		blob.SetParam(trans, threshold);

		blob.DoLabeling();
	}

	IplImage* abc = cvCreateImage(cvSize(inpitImg.cols, inpitImg.rows), IPL_DEPTH_8U, 1);
	cvSetZero(abc);

	blob.result(abc, blob.m_nBlobs);

	Mat last = cvarrToMat(abc,true);


	cvReleaseImage(&abc);
	return last;
}
