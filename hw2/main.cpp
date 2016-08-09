#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

IplImage* RGB2GRAY(IplImage* src) //��RGBת��Ϊ�Ҷ�ͼ��
{
		      //����һ��Դͼ��һ����IplImageָ��
			  IplImage* dst = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
		    //ɫ�ʿռ�ת����ת������ΪCV_BGR2GRAY
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
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���  
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�  

	double maxSimilarity = 0;  //���similarity
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
		u0 = (double)sum0 / cnt0;  //������ֵ
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1; //ǰ����ֵ
		for (int l = 0; l < width - 1; l++)
		{
			for (int k = 0; k < height - 1; k++)
			{
				if (p[k][l] > i)
					similarity += exp(-c*abs(p[k][l] - u1));//��ȡ1/n����Ĳ����ܺͣ����ڲ��ı��С�����Բ���
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
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���  
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�  

	double minIndex = -1;  //��Сindex
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
		u0 = (double)sum0 / cnt0;         //������ֵ
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1;        //ǰ����ֵ
		for (int k = 0; k < height - 1; k++)
		{
			for (int l = 0; l < width - 1; l++)
			{
				if (p[k][l] > i) //��ȡ2/sqrt��n)���沿�ֵ���ͣ����ڲ��ܸı��С�����Բ���
					Index += (min(exp(-0.6931*c*abs(p[k][l] - u1)), 1 - exp(-0.6931*c*abs(p[k][l] - u1))))*
					(min(exp(-0.6931*c*abs(p[k][l] - u1)), 1 - exp(-0.6931*c*abs(p[k][l] - u1))));
				else
					Index += (min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0))))*
					(min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0))));
			}
		}
		Index = sqrt(Index);
		if (minIndex == -1)  //��һ��
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
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���  
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�  

	double minIndex = -1;    //��Сindex
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
		u0 = (double)sum0 / cnt0;   //������ֵ
		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1; //ǰ����ֵ
		for (int k = 0; k < height - 1; k++)
		{
			for (int l = 0; l < width - 1; l++)
			{
				if (p[k][l] > i) //��ȡ��ʽ2/n�������ͣ�����2/n����ı��С�����Բ���
					Index += min(exp(-0.6931*c*abs(p[k][l] - u1)),1- exp(-0.6931*c*abs(p[k][l] - u1)));
				else
					Index += min(exp(-0.6931*c*abs(p[k][l] - u0)), 1 - exp(-0.6931*c*abs(p[k][l] - u0)));
			}
		}
		if (minIndex == -1)   //��һ��
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
	long sum0 = 0, sum1 = 0; //�洢ǰ���ĻҶ��ܺͺͱ����Ҷ��ܺ�  
	long cnt0 = 0, cnt1 = 0; //ǰ�����ܸ����ͱ������ܸ���  
	double u0 = 0, u1 = 0;  //ǰ���ͱ�����ƽ���Ҷ�  
	
	double maxComp = 0;    //���comp

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
		u0 = (double)sum0 / cnt0;          //������ֵ
		for (int j = i+1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		u1 = (double)sum1 / cnt1;         //ǰ����ֵ
		double au = 0;
		double pu = 0;
		double pu1 = 0;
		double pu2 = 0;
		for (int k = 0; k < height; k++)
		{
			for (int l= 0; l < width; l++)
			{
				if (p[k][l] > i)
					au += exp(-c*abs(p[k][l] - u1)); //Uij=exp(-c*abs(p[k][l] - u1))����ȡau
				else
					au += exp(-c*abs(p[k][l] - u0));
			}
		}
		for (int k = 0; k < height; k++)           //��ȡpu��һ����
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
		for (int l = 0; l < width; l++)              //��ȡpu�ڶ�����
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
		pu = pu1 + pu2;        //��ȡpu
		if (au / (pu*pu) > maxComp)
		{
			threshold = i;
			maxComp = au / (pu*pu);
		}
		cout << i << endl;
	}
	return threshold;
}
int getThreshold(int** p,int height,int width)  //ostu�㷨��ʵ��
{
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)           //����ֱ��ͼ
	{
		for (int j = 0; j < width; j++)
			histogram[p[i][j]]++;
	}

	long size = height*width;
	int threshold=0;
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
		
		for (int j = i+1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}
		
	    u1 = (double)sum1 / cnt1;
		w1 = 1 - w0; // (double)cnt1 / size;  
		
        u = u0 * w0 + u1 * w1; //ͼ���ƽ���Ҷ�  
		
		       //variance =  w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);  
	    variance = w0 * w1 *  (u0 - u1) * (u0 - u1); //��䷽��
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

	IplImage* src = cvLoadImage("potato.png");  //����ԭͼ��
	IplImage* grey=RGB2GRAY(src);               //ת�ɻҶ�ͼ
	IplImage* bin = cvCreateImage(cvGetSize(grey), IPL_DEPTH_8U, 1);
	//��ͼ�����ݼ��ؽ���λ����
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
	int threshold = fuzzySimilarity(p, height, width); //4.3 Using fuzzy similarity ��ʵ��

	//int threshold = fuzzyIndexQuad(p, height, width); //4.2 Using indices of fuzziness��ʵ��
														//����quadratic index of fuzziness
	//int threshold = fuzzyIndexLinear(p, height, width);//4.2 Using indices of fuzziness��ʵ��
														//����linear index of fuzziness
	//int threshold = fuzzyComp(p,height,width);       //4.1. Using fuzzy compactness��ʵ��

	//int threshold = getThreshold(p, height, width);  //ostu��ʵ��
	cvThreshold(grey, bin, threshold, 255, CV_THRESH_BINARY);  //������ֵ���Ҷ�ͼת�ɶ�ֵͼ

	cvShowImage("RGB", src);
	//cvShowImage("Gray", greyDst);
	cvShowImage("Bin", bin);
	cvWaitKey(0);
}