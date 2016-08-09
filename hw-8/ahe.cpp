#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <string>
#include <vector>
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
//��ֱ��ͼ
void fillHistogram(vector<vector<int>>& histograms, int row, int col, IplImage* src,int height,int width,int perHeight,int perWidth)
{
	//��ȡֱ��ͼ
	long long histogram[256] = { 0 };
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int tmp = cvGet2D(src, i + row*perHeight, j + col*perWidth).val[0];
			histogram[tmp]++;
		}
	}
	//��ȡ�ۼ�ֱ��ͼ
	long long sum = 0;
	long long sumHistogram[256] = { 0 };
	for (int i = 0; i < 256; i++)
	{
		sum += histogram[i];
		sumHistogram[i] = sum;
	}
	//����vector
	int index = row * 8 + col;
	for (int i = 0; i < 256; i++)
		histograms[index].push_back(sumHistogram[i]);
}
//ת��ֱ��ͼ
int convert(IplImage* src, vector<vector<int>>& histograms, vector<pair<int, int>>&centers, vector<pair<int, int>>&sizes,
	int row, int col, int perHeight, int perWidth)
{

	int leftTopRow = centers[0].first;         //��һ�����ĵ�
	int leftTopCol = centers[0].second;
	int rightButtonRow = centers[centers.size() - 1].first;   //���һ�����ĵ�
	int rightButtonCol = centers[centers.size() - 1].second;
	if (row <= leftTopRow)
	{
		if (col<=leftTopCol)        //���Ͻ�
		{
			int tmp = cvGet2D(src, row, col).val[0];
			return histograms[0][tmp] * 255 / (sizes[0].first*sizes[0].second);
		}
		else if (col>=rightButtonCol)  //���Ͻ�
		{
			int tmp = cvGet2D(src, row, col).val[0];
			return histograms[7][tmp] * 255 / (sizes[7].first*sizes[7].second);
		}
		else                    //���� �м�
		{
			int i = 0;
			for (; i < 8; i++)
			{
				if (col<centers[i].second)
					break;
			}
			int c1 = i-1; int c2 = i;
			double s = ((float)(col - centers[c1].second)) / ((float)(centers[c2].second - centers[c1].second));
			int tmp = cvGet2D(src, row, col).val[0];
			int f00 = histograms[c1][tmp] * 255 / (sizes[c1].first*sizes[c1].second);
			int f01 = histograms[c2][tmp] * 255 / (sizes[c2].first*sizes[c2].second);
			double result = (1 - s)*f00 + s*f01;
			return (int)result;
		}
	}
	else if (row>=rightButtonRow)      
	{
		if (col<=leftTopCol)            //���½�
		{
			int tmp = cvGet2D(src, row, col).val[0];
			return histograms[56][tmp] * 255 / (sizes[56].first*sizes[56].second);
		}
		else if (col>=rightButtonCol)  //���½�
		{
			int tmp = cvGet2D(src, row, col).val[0];
			return histograms[63][tmp] * 255 / (sizes[63].first*sizes[63].second);
		}
		else                  //���� �м�
		{
			int i = 56;
			for (; i < 64; i++)
			{
				if (col<centers[i].second)
					break;
			}
			int c1 = i - 1; int c2 = i;
			double s = ((float)(col - centers[c1].second)) / ((float)(centers[c2].second - centers[c1].second));
			int tmp = cvGet2D(src, row, col).val[0];
			int f00 = histograms[c1][tmp] * 255 / (sizes[c1].first*sizes[c1].second);
			int f01 = histograms[c2][tmp] * 255 / (sizes[c2].first*sizes[c2].second);
			double result = (1 - s)*f00 + s*f01;
			return (int)result;
		}
	}
	else
	{
		if (col<=leftTopCol)                 //��� �м�
		{
			int i = 0;
			for (; i <= 56; i += 8)
			{
				if (row<centers[i].first)
					break;
			}
			int c1 = i - 8; int c2 = i;
			double t = ((float)(row - centers[c1].first)) / ((float)(centers[c2].first - centers[c1].first));
			int tmp = cvGet2D(src, row, col).val[0];
			int f00 = histograms[c1][tmp] * 255 / (sizes[c1].first*sizes[c1].second);
			int f01 = histograms[c2][tmp] * 255 / (sizes[c2].first*sizes[c2].second);
			double result = (1 - t)*f00 + t*f01;
			return (int)result;
		}
		else if (col>=rightButtonCol)                  //�ұ��м�
		{
			int i = 7;
			for (; i <= 63; i += 8)
			{
				if (row<centers[i].first)
					break;
			}
			int c1 = i - 8; int c2 = i;
			double t = ((float)(row - centers[c1].first)) / ((float)(centers[c2].first - centers[c1].first));
			int tmp = cvGet2D(src, row, col).val[0];
			int f00 = histograms[c1][tmp] * 255 / (sizes[c1].first*sizes[c1].second);
			int f01 = histograms[c2][tmp] * 255 / (sizes[c2].first*sizes[c2].second);
			double result = (1 - t)*f00 + t*f01;
			return (int)result;
		}
		else                                      //���м�
		{
			int i = 9;
			for (; i < 64; i++)
			{
				if (row < centers[i].first&&col <= centers[i].second&&
					row >= centers[i - 9].first&&col >= centers[i - 9].second&&
					row >= centers[i - 8].first&&col <= centers[i - 8].second&&
					row < centers[i - 1].first&&col >= centers[i - 1].second)
					break;
			}
			int c1 = i - 9; int c2 = i - 8; int c3 = i - 1; int c4 = i;
			double t = ((float)(row - centers[c1].first)) / ((float)(centers[c3].first - centers[c1].first));
			double s = ((float)(col - centers[c1].second)) / ((float)(centers[c2].second - centers[c1].second));
			int tmp = cvGet2D(src, row, col).val[0];
			int f00 = histograms[c1][tmp] * 255 / (sizes[c1].first*sizes[c1].second);
			int f01 = histograms[c2][tmp] * 255 / (sizes[c2].first*sizes[c2].second);
			int f10 = histograms[c3][tmp] * 255 / (sizes[c3].first*sizes[c3].second);
			int f11= histograms[c4][tmp] * 255 / (sizes[c4].first*sizes[c4].second);
			double result = (1 - s)*(1 - t)*f00 + s*(1 - t)*f01 + t*(1 - s)*f10 + s*t*f11;
			return (int)result;
		}
	}
}
IplImage* adaptiveHistogramEqual(IplImage* src) //��ȡ��ֵ
{
	IplImage* grey = RGB2GRAY(src);             //ת�ɻҶ�
	IplImage* dst = cvCreateImage(cvGetSize(grey), grey->depth, grey->nChannels); //����Ŀ��ͼ��

	int height = grey->height;
	int width = grey->width;
	int perHeight = height / 8;           //��ȡÿ������ĸ߶�
	int perWidth = width / 8;             //��ȡÿ������Ŀ��
	vector<vector<int>>histograms(64);            //ÿ�������ֱ��ͼ
	vector < pair<int, int>>centers;                    //ÿ����������ĵ�λ��
	vector<pair<int,int>>sizes;                           //ÿ������Ĵ�С
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (i == 7)
			{
				if (j == 7)
				{
					fillHistogram(histograms, i, j, grey, height - perHeight * 7,width-perWidth*7,perHeight,perWidth);  //��ֱ��ͼ
					centers.push_back(make_pair(perHeight*i + (height - perHeight * 7) / 2, perWidth*j + (width - perWidth * 7) / 2));  //�����ĵ�
					sizes.push_back(make_pair(height - perHeight * 7, width - perWidth * 7));               //���С
				}
				else
				{
					fillHistogram(histograms, i, j, grey, height - perHeight * 7, perWidth,perHeight,perWidth);
					centers.push_back(make_pair(perHeight*i + (height - perHeight * 7) / 2, perWidth*j + perWidth / 2));
					sizes.push_back(make_pair(height - perHeight * 7, perWidth));
				}
			}
			else
			{
				if (j == 7)
				{
					fillHistogram(histograms, i, j, grey, perHeight, width - perWidth * 7,perHeight,perWidth);
					centers.push_back(make_pair(perHeight*i + perHeight / 2, perWidth*j + (width - perWidth * 7) / 2));
					sizes.push_back(make_pair(perHeight, width - perWidth * 7));
				}
				else
				{
					fillHistogram(histograms, i, j, grey, perHeight, perWidth,perHeight,perWidth);
					centers.push_back(make_pair(perHeight*i +perHeight / 2, perWidth*j + perWidth / 2));
					sizes.push_back(make_pair(perHeight, perWidth));
				}
			}
		}
	}
	for (int i = 0; i < height; i++)
	{
		uchar* ptr = (uchar*)dst->imageData + i*dst->widthStep;
		for (int j = 0; j < width; j++)
		{
			*(ptr + j) = convert(grey, histograms,centers, sizes,i, j,perHeight,perWidth);              //����ͼ����
		}
	}
	return dst;
}
int main()
{
	cout << "input image name: ";
	string imgName;
	cin >> imgName;
	IplImage* src = cvLoadImage(imgName.c_str());  //��ȡͼ��
	IplImage* dst = adaptiveHistogramEqual(src);  //��������Ӧֱ��ͼ���⻯�ķ���
	cvShowImage("SRC", src);
	cvShowImage("DST", dst);
	cvWaitKey(0);
}