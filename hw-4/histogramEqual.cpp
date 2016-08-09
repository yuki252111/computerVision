#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
using namespace cv;
using namespace std;

IplImage* RGB2GRAY(IplImage* src)
{
	//创建一个源图像一样的IplImage指针
	IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	//色彩空间转换，转换类型为CV_BGR2GRAY
	cvCvtColor(src, dst, CV_BGR2GRAY);
	return dst;

}
IplImage* histogramEqual(IplImage* src) //获取阈值
{
	IplImage* grey = RGB2GRAY(src);             //转成灰度
	IplImage* dst = cvCreateImage(cvGetSize(grey), grey->depth, grey->nChannels); //创建目标图像
	
	//将灰度读入二位数组
	int width = grey->width;
	int height = grey->height;
	int **p = new int*[height];
	for (int i = 0; i < height; i++)
		p[i] = new int[width];
	uchar* ptr;
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)grey->imageData + i*grey->widthStep;
		for (int j = 0; j < width; j++)
		{
			p[i][j] = (int)*(ptr + j);
		}
	}
	//建立直方图
	long long histogram[256] = { 0 };
	int max = 0;
	for (int i = 0; i < height; i++)  
	{
		for (int j = 0; j < width; j++)
		{
			histogram[p[i][j]]++;
			if (p[i][j]>max)
				max = p[i][j];
		}
	}
	//建立累积直方图作为LUT
	long long sum = 0;
	long long sumHistogram[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		sum += histogram[i];
		sumHistogram[i] = sum;
	}
	//转化图像
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)dst->imageData + i*dst->widthStep;
		for (int j = 0; j < width; j++)
		{
				*(ptr + j) = sumHistogram[p[i][j]] * max / (height*width);
		}
	}
	return dst;
}
int main()
{
	IplImage* src = cvLoadImage("einstein.png");  //读取图像
	IplImage* dst = histogramEqual(src);  //调用直方图均衡化的方法
	
	cvShowImage("SRC", src);
	cvShowImage("DST", dst);
	cvWaitKey(0);
}