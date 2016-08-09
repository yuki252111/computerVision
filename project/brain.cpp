#include "opencv/cv.h"
#include "opencv/ml.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cxcore.h"
using namespace cv;

#include <iostream>
#include <stdio.h>
using namespace std;
//---------------------
#define  START 67 
#define  END  132
#define ATTRIBUTES_PER_SAMPLE 9
#define NUMBER_OF_CLASSES 2
#define HEIGHT 256
#define WIDTH 256
//----------------LBP算法-----------------------
void LBP(IplImage *src, IplImage *dst){
	int tmp[8] = { 0 };
	CvScalar s;

	IplImage * temp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	uchar *data = (uchar*)src->imageData;
	int step = src->widthStep;

	for (int i = 1; i < src->height - 1; i++){
		for (int j = 1; j < src->width - 1; j++){
			int sum = 0;
			tmp[0] = (data[(i - 1)*step + j - 1] > data[i*step + j]) ? 1 : 0;
			tmp[1] = (data[i*step + (j - 1)] > data[i*step + j]) ? 1 : 0;
			tmp[2] = (data[(i + 1)*step + (j - 1)] > data[i*step + j]) ? 1 : 0;
			tmp[3] = (data[(i + 1)*step + j] > data[i*step + j]) ? 1 : 0;
			tmp[4] = (data[(i + 1)*step + (j + 1)] > data[i*step + j]) ? 1 : 0;
			tmp[5] = (data[i*step + (j + 1)] > data[i*step + j]) ? 1 : 0;
			tmp[6] = (data[(i - 1)*step + (j + 1)] > data[i*step + j]) ? 1 : 0;
			tmp[7] = (data[(i - 1)*step + j] > data[i*step + j]) ? 1 : 0;
			//计算LBP编码  
			s.val[0] = (tmp[0] * 1 + tmp[1] * 2 + tmp[2] * 4 + tmp[3] * 8 + tmp[4] * 16 + tmp[5] * 32 + tmp[6] * 64 + tmp[7] * 128);
			cvSet2D(dst, i, j, s); //写入LBP图像
		}
	}
}
//--------------------
IplImage* RGB2GRAY(IplImage* src)
{
	//创建一个源图像一样的IplImage指针
	IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	//色彩空间转换，转换类型为CV_BGR2GRAY
	cvCvtColor(src, dst, CV_BGR2GRAY);
	return dst;
}
//-------------------------
//-------读取训练数据--------------------
int read_training_data(IplImage*** imgs,IplImage*** tex_imgs,Mat& train_datas,Mat& responses)
{
	int data_line = 0;
	for (int i = 0; i < (END - START + 1); i++)
	{
		for (int j = 0; j <HEIGHT; j++)
		{
			for (int k = 0; k < WIDTH; k++)
			{
				if (cvGet2D(imgs[i][0], j, k).val[0] == 0 && cvGet2D(imgs[i][1], j, k).val[0] &&
					cvGet2D(imgs[i][2], j, k).val[0] == 0 && cvGet2D(imgs[i][3], j, k).val[0] == 0)      //全0数据不读入
					continue;

				train_datas.at<float>(data_line, 0) = (float)cvGet2D(imgs[i][0], j, k).val[0];
				train_datas.at<float>(data_line, 1) = (float)cvGet2D(imgs[i][1], j, k).val[0];
				train_datas.at<float>(data_line, 2) = (float)cvGet2D(imgs[i][2], j, k).val[0];
				train_datas.at<float>(data_line, 3) = (float)cvGet2D(imgs[i][3], j, k).val[0];
				
				train_datas.at<float>(data_line, 4) = (float)cvGet2D(tex_imgs[i][0], j, k).val[0];
				train_datas.at<float>(data_line, 5) = (float)cvGet2D(tex_imgs[i][1], j, k).val[0];
				train_datas.at<float>(data_line, 6) = (float)cvGet2D(tex_imgs[i][2], j, k).val[0];
				train_datas.at<float>(data_line, 7) = (float)cvGet2D(tex_imgs[i][3], j, k).val[0];

				train_datas.at<float>(data_line, 8) = i;

				if (cvGet2D(imgs[i][4], j, k).val[0] == 0)
					responses.at<int>(data_line, 0) = 0;
				else
					responses.at<int>(data_line, 0) = 1;
				data_line++;
			}
		}
	}
	return data_line;
}
int main()
{
	//-----------------------------------读图片------------------------
	IplImage*** imgs = new IplImage**[END - START + 1];
	IplImage*** tex_imgs = new IplImage**[END - START + 1];
	for (int i = 0; i < END - START + 1; i++)
	{
		imgs[i] = new IplImage*[5];
		tex_imgs[i] = new IplImage*[4];
	}
	for (int i = 0; i < END - START + 1; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			imgs[i][j] = NULL;
			tex_imgs[i][j] = NULL;
		}
		imgs[i][4] = NULL;
	}
	//----------------------------------------
	cout << "read image..........." << endl;
	for (int i = START; i <= END; i++)
	{
		char flairname[100], t1name[100], t1cname[100], t2name[100], truthname[100];
		memset(flairname, 0, 100); memset(t1name, 0, 100); memset(t1cname, 0, 100); memset(t2name, 0, 100); memset(truthname, 0, 100);

		sprintf(flairname, "BRATS_HG0005_FLAIR/BRATS_HG0005_FLAIR_%d.png", i);
		sprintf(t1name, "BRATS_HG0005_T1/BRATS_HG0005_T1_%d.png", i);
		sprintf(t1cname, "BRATS_HG0005_T1C/BRATS_HG0005_T1C_%d.png", i);
		sprintf(t2name, "BRATS_HG0005_T2/BRATS_HG0005_T2_%d.png", i);
		sprintf(truthname, "BRATS_HG0005_truth/BRATS_HG0005_truth_%d.png", i);

		IplImage* flair_img = RGB2GRAY(cvLoadImage(flairname));
		IplImage* t1_img = RGB2GRAY(cvLoadImage(t1name));
		IplImage* t1c_img = RGB2GRAY(cvLoadImage(t1cname));
		IplImage* t2_img = RGB2GRAY(cvLoadImage(t2name));
		IplImage* truth_img = RGB2GRAY(cvLoadImage(truthname));

		imgs[i - START][0] = flair_img;
		imgs[i - START][1] = t1_img;
		imgs[i - START][2] = t1c_img;
		imgs[i - START][3] = t2_img;
		imgs[i - START][4] = truth_img;
		//获取纹理图
		IplImage* flair_tex = cvCreateImage(cvGetSize(flair_img), IPL_DEPTH_8U, 1);
		IplImage* t1_tex = cvCreateImage(cvGetSize(t1_img), IPL_DEPTH_8U, 1);
		IplImage* t1c_tex = cvCreateImage(cvGetSize(t1c_img), IPL_DEPTH_8U, 1);
		IplImage* t2_tex = cvCreateImage(cvGetSize(t2_img), IPL_DEPTH_8U, 1);
		LBP(flair_img, flair_tex);
		LBP(t1_img, t1_tex);
		LBP(t1c_img, t1c_tex);
		LBP(t2_img, t2_tex);

		tex_imgs[i - START][0] = flair_tex;
		tex_imgs[i - START][1] =t1_tex;
		tex_imgs[i - START][2] = t1c_tex;
		tex_imgs[i - START][3] = t2_tex;
	}
	//----------------------------------------------------------
	cout << "read training data............" << endl;
	Mat train_datas(HEIGHT*WIDTH*(END - START + 1), ATTRIBUTES_PER_SAMPLE, CV_32FC1);    
	Mat responses(HEIGHT*WIDTH*(END - START + 1), 1, CV_32SC1);
	//---读取训练数据----
	int dataline=read_training_data(imgs,tex_imgs, train_datas, responses);
	Mat _train_datas(dataline, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
	Mat _responses(dataline, 1, CV_32SC1);
	//减少训练数据为dataline个
	for (int i = 0; i < dataline; i++)
	{
		float* float_data = train_datas.ptr<float>(i);
		int* int_data = responses.ptr<int>(i);

		_train_datas.at<float>(i, 0) = float_data[0];
		_train_datas.at<float>(i, 1) = float_data[1];
		_train_datas.at<float>(i, 2) = float_data[2];
		_train_datas.at<float>(i, 3) = float_data[3];

		_train_datas.at<float>(i, 4) = float_data[4];
		_train_datas.at<float>(i, 5) = float_data[5];
		_train_datas.at<float>(i, 6) = float_data[6];
		_train_datas.at<float>(i, 7) = float_data[7];

		_train_datas.at<float>(i, 8) = float_data[8];

		_responses.at<int>(i, 0) = int_data[0];
	}
	//----设置输入类型---
	Mat var_type = Mat(ATTRIBUTES_PER_SAMPLE+1, 1, CV_8U);
	var_type.setTo(Scalar(CV_VAR_NUMERICAL)); // all inputs are numerical  
	var_type.at<uchar>(ATTRIBUTES_PER_SAMPLE, 0) = CV_VAR_CATEGORICAL;
	//---训练数据---
	cout << "training......." << endl;
	float priors[NUMBER_OF_CLASSES] = { 1, 1 };
	CvRTParams params = CvRTParams(25, // max depth  
	                       4, // min sample count  
	                       0, // regression accuracy: N/A here  
                           false, // compute surrogate split, no missing data  
		                   5, // max number of categories (use sub-optimal algorithm for larger numbers)  
	                        priors, // the array of priors  
	                        false,  // calculate variable importance  
		                    3,       // number of variables randomly selected at node and used to find the best split(s).  
							3,  // max number of trees in the forest  
                            0.01f,               // forrest accuracy  
	                        CV_TERMCRIT_ITER | CV_TERMCRIT_EPS // termination cirteria  
		);
	CvRTrees* rtree = new CvRTrees;
	bool train_result = rtree->train(_train_datas, CV_ROW_SAMPLE, _responses,
		Mat(), Mat(), var_type, Mat(), params);
	if (train_result == false)
		cout << "random trees train failed!" << endl;
	
	cout << "predicting.........." << endl;
	//-------预测数据生成图片并存储---------
	for (int k = 0; k < END - START + 1; k++)
	{
		IplImage* img_dst = cvCreateImage(cvGetSize(imgs[k][0]), IPL_DEPTH_8U, 1);
		uchar* ptr;
		for (int i = 0; i <HEIGHT ; i++)
		{
			ptr = (uchar*)img_dst->imageData + i*img_dst->widthStep;
			for (int j = 0; j < WIDTH; j++)
			{//读一行数据
				Mat test_data(1, ATTRIBUTES_PER_SAMPLE, CV_32FC1);
				test_data.at<float>(0, 0) = cvGet2D(imgs[k][0], i, j).val[0];
				test_data.at<float>(0, 1) = cvGet2D(imgs[k][1], i, j).val[0];
				test_data.at<float>(0, 2) = cvGet2D(imgs[k][2], i, j).val[0];
				test_data.at<float>(0, 3) = cvGet2D(imgs[k][3], i, j).val[0];
			
				test_data.at<float>(0, 4) = cvGet2D(tex_imgs[k][0], i, j).val[0];
				test_data.at<float>(0, 5) = cvGet2D(tex_imgs[k][1], i, j).val[0];
				test_data.at<float>(0, 6) = cvGet2D(tex_imgs[k][2], i, j).val[0];
				test_data.at<float>(0, 7) = cvGet2D(tex_imgs[k][3], i, j).val[0];

				test_data.at<float>(0, 8) = k;
				//产生结果
				int result = rtree->predict(test_data, Mat());
				*(ptr + j) = result * 255;
			}
		}
		IplConvKernel* strel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);
		cvErode(img_dst, img_dst, strel, 1);
		//cvDilate(img_dst, img_dst, strel, 1);
		
		cout << "save image  " << k + START << endl;
		char result_name[100];
		memset(result_name, 0, 100);
		sprintf(result_name, "BRATS_HG0005_RESULT/BRATS_HG0005_RESULT_%d.png", k+START);
		cvSaveImage(result_name, img_dst);
	}
	cout << "complete!!!" << endl;
	cvWaitKey(0);
}