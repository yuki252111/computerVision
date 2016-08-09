#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

IplImage* RGB2GRAY(IplImage* src) //将RGB转化为灰度图像
{
		      //创建一个源图像一样的IplImage指针
			  IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		    //色彩空间转换，转换类型为CV_BGR2GRAY
		      cvCvtColor(src, dst, CV_BGR2GRAY);
			  return dst;
	
 }
int fuzzySimilarity(int **p, int height, int width)
{
	float histogram[256] = { 0 };
	int maxGrey = 0; int minGrey = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histogram[p[i][j]]++;
			if (p[i][j]>maxGrey)
				maxGrey = p[i][j];
			if (p[i][j] < minGrey)
				minGrey = p[i][j];
		}
	}
	long size = height*width;
	double c = 1.0 / (maxGrey - minGrey);
	int threshold = 0;
	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  

	double maxSimilarity = 0;  //最大similarity
	double similarity = 0;;
	for (int i = 0; i < 256; i++)
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		u0 = 0;
		u1 = 0;
		similarity = 0;
		for (int j = 0; j <= i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];

		}
		u0 = (double)sum0 / cnt0;  //背景均值
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1; //前景均值
		for (int l = 0; l < width - 1; l++)
		{
			for (int k = 0; k < height - 1; k++)
			{
				if (p[k][l] > i)
					similarity += exp(-c*abs(p[k][l] - u1));//获取1/n后面的部分总和，由于不改变大小，所以不乘
				else
					similarity += exp(-c*abs(p[k][l] - u0));
			}
		}
		if (similarity >maxSimilarity)
		{
			maxSimilarity = similarity;
			threshold = i;
		}
		cout << i << endl;
	}
	return threshold;
}
int fuzzyIndexQuad(int** p, int height, int width)
{
	float histogram[256] = { 0 };
	int maxGrey = 0; int minGrey = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histogram[p[i][j]]++;
			if (p[i][j]>maxGrey)
				maxGrey = p[i][j];
			if (p[i][j] < minGrey)
				minGrey = p[i][j];
		}
	}
	long size = height*width;
	double c = 1.0 / (maxGrey - minGrey);
	int threshold = 0;
	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  

	double minIndex = -1;  //最小index
	double Index = 0;                 
	for (int i = 0; i < 256; i++)
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		u0 = 0;
		u1 = 0;
		Index = 0;
		for (int j = 0; j <= i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];

		}
		u0 = (double)sum0 / cnt0;         //背景均值
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1;        //前景均值
		for (int k = 0; k < height - 1; k++)
		{
			for (int l = 0; l < width - 1; l++)
			{
				if (p[k][l] > i) //获取2/sqrt（n)后面部分的求和，由于不能改变大小，所以不乘
					Index += (min(exp(-0.6931*c*abs(p[k][l] - u1)), 1 - exp(-0.6931*c*abs(p[k][l] - u1))))*
					(min(exp(-0.6931*c*abs(p[k][l] - u1)), 1 - exp(-0.6931*c*abs(p[k][l] - u1))));
				else
					Index += (min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0))))*
					(min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0))));
			}
		}
		Index = sqrt(Index);
		if (minIndex == -1)  //第一次
		{
			minIndex = Index;
			threshold = i;
		}
		else
		{
			if (Index < minIndex)
			{
				minIndex = Index;
				threshold = i;
			}
		}
		cout << i << endl;
	}
	return threshold;
}
int fuzzyIndexLinear(int** p, int height, int width)
{
	float histogram[256] = { 0 };
	int maxGrey = 0; int minGrey = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histogram[p[i][j]]++;
			if (p[i][j]>maxGrey)
				maxGrey = p[i][j];
			if (p[i][j] < minGrey)
				minGrey = p[i][j];
		}
	}
	long size = height*width;
	double c = 1.0 / (maxGrey - minGrey);
	int threshold = 0;
	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  

	double minIndex = -1;    //最小index
	double Index = 0;
	for (int i = 0; i < 256; i++)
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		u0 = 0;
		u1 = 0;
		Index = 0;
		for (int j = 0; j <= i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];

		}
		u0 = (double)sum0 / cnt0;   //背景均值
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1; //前景均值
		for (int k = 0; k < height - 1; k++)
		{
			for (int l = 0; l < width - 1; l++)
			{
				if (p[k][l] > i) //获取公式2/n后面的求和，由于2/n不会改变大小，所以不乘
					Index += min(exp(-0.6931*c*abs(p[k][l] - u1)),1- exp(-0.6931*c*abs(p[k][l] - u1)));
				else
					Index += min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0)));
			}
		}
		if (minIndex == -1)   //第一次
		{
			minIndex = Index;
			threshold = i;
		}
		else
		{
			if (Index < minIndex)
			{
				minIndex = Index;
				threshold = i;
			}
		}
		cout << i << endl;
	}
	return threshold;
}
int fuzzyComp(int** p, int height, int width)  
{
	float histogram[256] = { 0 };
	int maxGrey = 0; int minGrey = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			histogram[p[i][j]]++;
			if (p[i][j]>maxGrey)
				maxGrey = p[i][j];
			if (p[i][j] < minGrey)
				minGrey = p[i][j];
		}
	}
	long size = height*width;
	double c = 1.0 / (maxGrey - minGrey);
	int threshold = 0;
	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  
	
	double maxComp = 0;    //最大comp

	for (int i = 0; i < 256; i++)
	{
		sum0 = 0;
		sum1 = 0;
		cnt0 = 0;
		cnt1 = 0;
		u0 = 0;
		u1 = 0;
		for (int j = 0; j <= i; j++)
		{
			cnt0 += histogram[j];
			sum0 += j * histogram[j];

		}
		u0 = (double)sum0 / cnt0;          //背景均值
		for (int j = i+1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1;         //前景均值
		double au = 0;
		double pu = 0;
		double pu1 = 0;
		double pu2 = 0;
		for (int k = 0; k < height; k++)
		{
			for (int l= 0; l < width; l++)
			{
				if (p[k][l] > i)
					au += exp(-c*abs(p[k][l] - u1)); //Uij=exp(-c*abs(p[k][l] - u1))，获取au
				else
					au += exp(-c*abs(p[k][l] - u0));
			}
		}
		for (int k = 0; k < height; k++)           //获取pu第一部分
		{
			for (int l = 0; l < width - 1; l++)
			{
				if (p[k][l] > i)
				{
					if (p[k][l + 1]>i)
						pu1 += abs(exp(-c*abs(p[k][l] - u1)) - exp(-c*abs(p[k][l+1] - u1)));
					else
						pu1 += abs(exp(-c*abs(p[k][l] - u1)) - exp(-c*abs(p[k][l+1] - u0)));
				}
				else
				{
					if (p[k][l + 1]>i)
						pu1 += abs(exp(-c*abs(p[k][l] - u0)) - exp(-c*abs(p[k][l + 1] - u1)));
					else
						pu1 += abs(exp(-c*abs(p[k][l] - u0)) - exp(-c*abs(p[k][l + 1] - u0)));
				}
			}
		}
		for (int l = 0; l < width; l++)              //获取pu第二部分
		{
			for (int k = 0; k < height - 1; k++)
			{
				if (p[k][l] > i)
				{
					if (p[k+1][l]>i)
						pu2 += abs(exp(-c*abs(p[k][l] - u1)) - exp(-c*abs(p[k+1][l] - u1)));
					else
						pu2 += abs(exp(-c*abs(p[k][l] - u1)) - exp(-c*abs(p[k+1][l] - u0)));
				}
				else
				{
					if (p[k+1][l]>i)
						pu2 += abs(exp(-c*abs(p[k][l] - u0)) - exp(-c*abs(p[k+1][l] - u1)));
					else
						pu2 += abs(exp(-c*abs(p[k][l] - u0)) - exp(-c*abs(p[k+1][l] - u0)));
				}
			}
		}
		pu = pu1 + pu2;        //获取pu
		if (au / (pu*pu) > maxComp)
		{
			threshold = i;
			maxComp = au / (pu*pu);
		}
		cout << i << endl;
	}
	return threshold;
}
int getThreshold(int** p,int height,int width)  //ostu算法的实现
{
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)           //建立直方图
	{
		for (int j = 0; j < width; j++)
			histogram[p[i][j]]++;
	}

	long size = height*width;
	int threshold=0;
	long sum0 = 0, sum1 = 0; //存储前景的灰度总和和背景灰度总和  
	long cnt0 = 0, cnt1 = 0; //前景的总个数和背景的总个数  
	double w0 = 0, w1 = 0; //前景和背景所占整幅图像的比例  
	double u0 = 0, u1 = 0;  //前景和背景的平均灰度  
	double variance = 0; //最大类间方差  
	 
	double u = 0;
	double maxVariance = 0;
	for (int i = 1; i < 256; i++) //一次遍历每个像素  
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
		
		for (int j = i+1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		
	    u1 = (double)sum1 / cnt1;
		w1 = 1 - w0; // (double)cnt1 / size;  
		
        u = u0 * w0 + u1 * w1; //图像的平均灰度  
		
		       //variance =  w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);  
	    variance = w0 * w1 *  (u0 - u1) * (u0 - u1); //类间方差
		if (variance > maxVariance)
		{
			maxVariance = variance;
            threshold = i;
		 }
	}
	return threshold;
}
int main()
{

	IplImage* src = cvLoadImage("potato.png");  //加载原图像
	IplImage* grey=RGB2GRAY(src);               //转成灰度图
	IplImage* bin = cvCreateImage(cvGetSize(grey), IPL_DEPTH_8U, 1);
	//将图像数据加载进二位数组
	int width = grey->width;
	int height = grey->height;
	int **p = new int*[height];
	for (int i = 0; i < height; i++)
		p[i] = new int[width];
	uchar* ptr;
	for (int i = 0; i<height; i++)
	{
		ptr = (uchar*)grey->imageData + i*grey->widthStep;
		for (int j = 0; j<width; j++)
		{
			p[i][j] = (int)*(ptr + j);
		}
	}
	int threshold = fuzzySimilarity(p, height, width); //4.3 Using fuzzy similarity 的实现

	//int threshold = fuzzyIndexQuad(p, height, width); //4.2 Using indices of fuzziness的实现
														//采用quadratic index of fuzziness
	//int threshold = fuzzyIndexLinear(p, height, width);//4.2 Using indices of fuzziness的实现
														//采用linear index of fuzziness
	//int threshold = fuzzyComp(p,height,width);       //4.1. Using fuzzy compactness的实现

	//int threshold = getThreshold(p, height, width);  //ostu的实现
	cvThreshold(grey, bin, threshold, 255, CV_THRESH_BINARY);  //根据阈值将灰度图转成二值图

	cvShowImage("RGB", src);
	//cvShowImage("Gray", greyDst);
	cvShowImage("Bin", bin);
	cvWaitKey(0);
}