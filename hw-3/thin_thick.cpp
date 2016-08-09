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
int getThreshold(int** p, int height, int width) //��ȡ��ֵ
{
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)  //����ֱ��ͼ
	{
		for (int j = 0; j < width; j++)
			histogram[p[i][j]]++;
	}

	long size = height*width;
	int threshold = 0;
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���
	double w0 = 0, w1 = 0; //ǰ���ͱ�����ռ����ͼ��ı���
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�
	double variance = 0; //�����䷽��

	double u = 0;
	double maxVariance = 0;
	for (int i = 1; i < 256; i++) //һ�α���ÿ������
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		w0 = 0;
		w1 = 0;
		for (int j = 0; j <= i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];
		}

		u0 = (double)sum0 / cnt0;
		w0 = (double)cnt0 / size;

		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}

		u1 = (double)sum1 / cnt1;
		w1 = 1 - w0; // (double)cnt1 / size;

		u = u0 * w0 + u1 * w1; //ͼ���ƽ���Ҷ�

		//variance =  w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
		variance = w0 * w1 *  (u0 - u1) * (u0 - u1);
		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}
	return threshold;
}
void hitOrMiss(int**p, int height, int width, int** strucEle, int originX, int originY, int row, int col)//hitOrMiss����
{
	//ԭ��1.�ṹԪ��ȫΪ1�ĵ���ԭͼ����
	//      2.�ṹԪ��ȫΪ0�ĵ�Ҫô�ڽ��⣬Ҫôԭͼ���Ӧ��ҲΪ0
	//�������������ĵ��ֵΪ1������Ϊ0
	vector<pair<int, int>>allOne;  //�ṹԪ��Ϊ1�ĵ�
	vector<pair<int, int>>allZero; //�ṹԪ��Ϊ0�ĵ�
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (strucEle[i][j] == 1)
				allOne.push_back(make_pair(i, j));
			if (strucEle[i][j] == 0)
				allZero.push_back(make_pair(i, j));
		}
	}
	//���Ƹ���
	int** copy = new int*[height];
	for (int i = 0; i < height; i++)
		copy[i] = new int[width];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			copy[i][j] = p[i][j];

	bool one = false;
	bool zero = false;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			one = true; zero = true;
			for (int k = 0; k < allOne.size(); k++)
			{
				//�ж�Ϊ1�ĵ��Ƿ�ȫ��ͼ����
				int x = allOne[k].first - originX + i;
				int y = allOne[k].second - originY + j;
				if (x < 0 || x >= height || y < 0 || y >= width)//�е��ڽ���
				{
					one = false;
					break;
				}
				else
				{
					if (copy[x][y] == 0)    //ԭͼ���Ӧ��Ϊ0
					{
						one = false;
						break;
					}
				}
			}
			for (int k = 0; k < allZero.size(); k++)
			{
				//�жϽṹԪ��ȫΪ0�ĵ�Ҫô�ڽ��⣬Ҫôԭͼ���Ӧ��ҲΪ0
				int x = allZero[k].first - originX + i;
				int y = allZero[k].second - originY + j;

				if (!(x < 0 || x >= height || y < 0 || y >= width) && copy[x][y] != 0)
				{//���ڽ��ڵ��ǵ�ǰԭͼ���Ӧ�㲻��0
					zero = false;
					break;
				}
			}
			if (one == true && zero == true) //����1��2����
				p[i][j] = 1;
			else
				p[i][j] = 0;

		}
	}
}
void perThin(int**p, int height, int width, int** strucEle, int originX, int originY, int row, int col)//һ��thin����
{
	//���ƶ�ά����
	int** copy = new int*[height];
	for (int i = 0; i < height; i++)
		copy[i] = new int[width];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			copy[i][j] = p[i][j];
	//����hitOrMiss�任
	hitOrMiss(copy, height, width, strucEle, originX, originY, row, col);
	//��ԭ�����ȥ�仯������飬��Ϊthin����
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = p[i][j] - copy[i][j];
}
void allThin(int** p, int height, int width,int maxCount=1)  //�������е�thin����
{
	//�ȹ���8���ṹԪ��
	int originX, originY;
	int** p1 = new int*[3];
	for (int i = 0; i < 3; i++)
		p1[i] = new int[3];

	originX = 1; originY = 1;  //�ṹԪ�ص�ԭ��
	for (int count = 0; count < maxCount; count++) //�ظ�����100��
	{
		//��һ���ṹԪ�أ���������thin����
		p1[0][0] = 0; p1[0][1] = 0; p1[0][2] = 0;
		p1[1][0] = -1; p1[1][1] = 1; p1[1][2] = -1;
		p1[2][0] = 1; p1[2][1] = 1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//�ڶ����ṹԪ�أ���������thin����
		p1[0][0] = -1; p1[0][1] = 0; p1[0][2] = 0;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = 1; p1[2][1] = 1; p1[2][2] = -1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//�������ṹԪ�أ���������thin����
		p1[0][0] = 1; p1[0][1] = -1; p1[0][2] = 0;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = 1; p1[2][1] = -1; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//���ĸ��ṹԪ�أ���������thin����
		p1[0][0] = 1; p1[0][1] = 1; p1[0][2] = -1;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = -1; p1[2][1] = 0; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//������ṹԪ�أ���������thin����
		p1[0][0] = 1; p1[0][1] = 1; p1[0][2] = 1;
		p1[1][0] = -1; p1[1][1] = 1; p1[1][2] = -1;
		p1[2][0] = 0; p1[2][1] = 0; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//�������ṹԪ�أ���������thin����
		p1[0][0] = -1; p1[0][1] = 1; p1[0][2] = 1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = 0; p1[2][1] = 0; p1[2][2] = -1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//���߸��ṹԪ�أ���������thin����
		p1[0][0] = 0; p1[0][1] = -1; p1[0][2] = 1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = 0; p1[2][1] = -1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//�ڰ˸��ṹԪ�أ���������thin����
		p1[0][0] = 0; p1[0][1] = 0; p1[0][2] = -1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = -1; p1[2][1] = 1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
	}
}
IplImage* thin(IplImage* rgb,int maxCount=1)          //����ͼ��������thin����
{
	IplImage* thinImage = cvCreateImage(cvGetSize(rgb), rgb->depth, rgb->nChannels);
	cvCopy(rgb, thinImage);

	IplImage* grey = RGB2GRAY(rgb);             //ת�ɻҶ�

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
	int threshold = getThreshold(p, height, width);  //��ȡ��ֵ
	for (int i = 0; i < height; i++)                //��ֵ����ά����
	{
		for (int j = 0; j < width; j++)
		{
			if (p[i][j]>threshold)
				p[i][j] = 1;
			else
				p[i][j] = 0;
		}
	}
	allThin(p, height, width,maxCount);              //�������е�thin����
	//��thin�����õ���ͼ������ɫ��ʾ
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)thinImage->imageData + i*thinImage->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (p[i][j] == 1)                  //�ж���ͼ��
			{
				*(ptr + 3 * j + 0) = 0;
				*(ptr + 3 * j + 1) = 255;
				*(ptr + 3 * j + 2) = 0;
			}
		}
	}
	return thinImage;
}
IplImage* thick(IplImage* rgb,int maxCount=1)
{
	IplImage* thickImage = cvCreateImage(cvGetSize(rgb), rgb->depth, rgb->nChannels);
	cvCopy(rgb, thickImage);

	IplImage* grey = RGB2GRAY(rgb);             //ת�ɻҶ�

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
	int threshold = getThreshold(p, height, width);  //��ȡ��ֵ
	for (int i = 0; i < height; i++)                //��ֵ����ά����
	{
		for (int j = 0; j < width; j++)
		{
			if (p[i][j]>threshold)
				p[i][j] = 1;
			else
				p[i][j] = 0;
		}
	}
	//ȡ����
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = 1 - p[i][j];

	allThin(p, height, width, maxCount);              //�Բ����������е�thin����
	//��thin�����õ���ͼ������ɫ��ʾ
	//�ٴ�ȡ�������õ�thick
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = 1 - p[i][j];
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)thickImage->imageData + i*thickImage->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (p[i][j] == 1)                  //�ж���ͼ��
			{
				*(ptr + 3 * j + 0) = 0;
				*(ptr + 3 * j + 1) = 255;
				*(ptr + 3 * j + 2) = 0;
			}
		}
	}
	return thickImage;
}
int main()
{
	IplImage* rgb = cvLoadImage("potato.png");  //��ȡͼ��
	IplImage* thinImage = thin(rgb,3);
	IplImage* thickImage = thick(rgb,3);
	//��ʾͼ��
	cvShowImage("RGB", rgb);
	cvShowImage("THIN", thinImage);
	cvShowImage("THICK", thickImage);
	cvWaitKey(0);

}