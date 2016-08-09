#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
using namespace cv;
using namespace std;

IplImage* RGB2GRAY(IplImage* src)
{
	//����һ��Դͼ��һ����IplImageָ��
	IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	//ɫ�ʿռ�ת����ת������ΪCV_BGR2GRAY
	cvCvtColor(src, dst, CV_BGR2GRAY);
	return dst;

}
IplImage* histogramEqual(IplImage* src) //��ȡ��ֵ
{
	IplImage* grey = RGB2GRAY(src);             //ת�ɻҶ�
	IplImage* dst = cvCreateImage(cvGetSize(grey), grey->depth, grey->nChannels); //����Ŀ��ͼ��
	
	//���Ҷȶ����λ����
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
	//����ֱ��ͼ
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
	//�����ۻ�ֱ��ͼ��ΪLUT
	long long sum = 0;
	long long sumHistogram[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		sum += histogram[i];
		sumHistogram[i] = sum;
	}
	//ת��ͼ��
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
	IplImage* src = cvLoadImage("einstein.png");  //��ȡͼ��
	IplImage* dst = histogramEqual(src);  //����ֱ��ͼ���⻯�ķ���
	
	cvShowImage("SRC", src);
	cvShowImage("DST", dst);
	cvWaitKey(0);
}