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
//-------------------------------------------
int** pcanny = NULL;   //ԭͼ��
//--------��ȡԭͼ��------------
void readImage(IplImage* src,int height,int width)
{
	uchar* ptr;
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)src->imageData + i*src->widthStep;
		for (int j = 0; j < width; j++)
		{
			pcanny[i][j] = (int)*(ptr + j);
		}
	}
}
//-----------------------------------------------
//-----�����ݶȷ�ֵ�ͷ���---------------
/////    P[i,j]=(S[i,j+1]-S[i,j]+S[i+1,j+1]-S[i+1,j])/2     /////  
/////    Q[i,j]=(S[i,j]-S[i+1,j]+S[i,j+1]-S[i+1,j+1])/2     /////  

double** P = NULL;    //x����ƫ����
double** Q = NULL;   //y����ƫ����
int** M =NULL;          //�ݶȷ�ֵ  
double** Theta = NULL;   //�ݶȷ���  

void calgradient(int height,int width)
{
	//����xyƫ����
	for (int i = 0; i < height - 1; i++)
	{
		for (int j = 0; j < width - 1; j++)
		{
			P[i][j] = (double)(pcanny[i][min(j + 1, width - 1)] - pcanny[i][j] + pcanny[min(i + 1, height - 1)][min(j + 1, width - 1)] - pcanny[min(i + 1,height - 1)][j]) / 2.0;
			Q[i][j] = (double)(pcanny[i][j] - pcanny[min(i + 1, height - 1)][j] + pcanny[i][min(j + 1, width - 1)] - pcanny[min(i + 1, height - 1)][min(j + 1, width - 1)]) / 2.0;
		}
	}
	//�����ֵ�ͷ���
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			M[i][j] = (int)sqrt(P[i][j] * P[i][j] + Q[i][j] * Q[i][j]);
			Theta[i][j] = atan2(Q[i][j], P[i][j])*57.3;		//����ת�Ƕ�
			if (Theta[i][j] < 0)
				Theta[i][j] += 360;			//ת��0-360֮��
		}
	}
}
//---------------------------------------------------------
//------------�Ǽ���ֵ����------------------------------
int ** N = NULL;												//�Ǽ���ֵ���ƽ��  
int g1 = 0, g2 = 0, g3 = 0, g4 = 0;                            //���ڽ��в�ֵ���õ������ص�����ֵ  
double dTmp1 = 0.0, dTmp2 = 0.0;                           //�������������ص��ֵ�õ��ĻҶ�����  
double dWeight = 0.0;                                    //��ֵ��Ȩ��  

void nonMaxSuppression(int height, int width)
{
	//�߽��ʼ��
	for (int i = 0; i < width; i++)
	{
		N[0][i] = 0;
		N[height - 1][i] = 0;
	}
	for (int i = 0; i < height; i++)
	{
		N[i][0] = 0;
		N[i][width - 1] = 0;
	}
	//�Ǽ���ֵ����
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			if (M[i][j] == 0)
				N[i][j] = 0;
			else
			{
				////////�����ж��������������Ȼ����������ֵ///////  
			   ////////////////////��һ�����///////////////////////  
		       /////////       g1  g2                  /////////////  
			     /////////           C                   /////////////  
		        /////////           g3  g4              /////////////  
		        /////////////////////////////////////////////////////  
				if (((Theta[i][j] >= 90) && (Theta[i][j] < 135)) || ((Theta[i][j] >= 270) && (Theta[i][j] < 315)))
				{
					g1 = M[i - 1][j - 1];
					g2 = M[i - 1][j];
					g3 = M[i + 1][j];
					g4 = M[i + 1][j + 1];
					dWeight = abs(P[i][j]) / abs(Q[i][j]);   //������  
					dTmp1 = g1*dWeight + g2*(1 - dWeight);
					dTmp2 = g4*dWeight + g3*(1 - dWeight);
				}
				////////////////////�ڶ������///////////////////////  
				 /////////       g1                      /////////////  
			     /////////       g2  C   g3              /////////////  
					  /////////               g4              /////////////  
		       /////////////////////////////////////////////////////  
				else if (((Theta[i][j] >= 135) && (Theta[i][j] < 180)) || ((Theta[i][j] >= 315) && (Theta[i][j] < 360)))
				{
					g1 = M[i - 1][j - 1];
					g2 = M[i][j - 1];
					g3 = M[i][j + 1];
					g4 = M[i + 1][j + 1];
					dWeight = abs(Q[i][j]) / abs(P[i][j]);   //����  
					dTmp1 = g2*dWeight + g1*(1 - dWeight);
				    dTmp2 = g4*dWeight + g3*(1 - dWeight);
				}
				////////////////////���������///////////////////////  
				  /////////           g1  g2              /////////////  
				  /////////           C                   /////////////  
			 /////////       g4  g3                  /////////////  
		  /////////////////////////////////////////////////////  
			  else if (((Theta[i][j] >= 45) && (Theta[i][j]<90)) ||((Theta[i][j] >= 225) && (Theta[i][j]<270)))
			   {
				   g1 = M[i - 1][j];
				   g2 = M[i - 1][j + 1];
				   g3 = M[i + 1][j];
				   g4 = M[i+1][j-1];
				   dWeight = abs(P[i][j]) / abs(Q[i][j]);   //������  
				   dTmp1 = g2*dWeight + g1*(1 - dWeight);
				   dTmp2 = g3*dWeight + g4*(1 - dWeight);
				}
			     ////////////////////���������///////////////////////  
						  /////////               g1              /////////////  
			          /////////       g4  C   g2              /////////////  
			          /////////       g3                      /////////////  
			          /////////////////////////////////////////////////////  
		         else if (((Theta[i][j] >= 0) && (Theta[i][j]<45)) ||  ((Theta[i][j] >= 180) && (Theta[i][j]<225)))
				 {
					 g1 = M[i - 1][j + 1];
					 g2 = M[i][j + 1];
					 g3 = M[i + 1][j - 1];
					 g4 = M[i][j - 1];
					 dWeight = abs(Q[i][j]) / abs(P[i][j]);   //����  
					 dTmp1 = g1*dWeight + g2*(1 - dWeight);
				     dTmp2 = g3*dWeight + g4*(1 - dWeight);
		        }
			  if (M[i][j] >= dTmp1&&M[i][j] >= dTmp2)
				  N[i][j] = 1;
			  else
				  N[i][j] = 0;
			}
		}
	}
}
//---------------------------------------------------------------
//------------˫��ֵ���---------------------------------------
void around(int** N , int i, int j,int thr)        //������Ե������򣬲�ȡ����ֵ���
{
	int xNum[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int yNum[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	int xx, yy;
	for (int k = 0; k<8; k++)
	{
		xx = i + xNum[k];
		yy = j + yNum[k];
		if (N[xx][yy] == 1 && M[xx][yy] >= thr)
		{
			N[xx][yy] = 255;
			around(N, xx, yy, thr);                //����ĵ�
		}
   }
}
void twoThresholdCheck(int height, int width)
{
	int histogram[400];  //��ֱֵ��ͼ
	for (int i = 0; i < 400; i++)
		histogram[i] = 0;
	//��ȡ�߽��ѡ�ݶȷ�ֵ��ֱ��ͼ
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (N[i][j]==1)
				histogram[M[i][j]]++;
	//��ȡǱ�ڱ�Ե��ĸ����ܺͺ����ֵ
	int max = 0;
	int sum = histogram[0];
	for (int i = 1; i < 400; i++)
	{
		if (histogram[i] != 0)
			max = i;
		sum += histogram[i];
	}
	//��ȡ����ֵ�͵���ֵ
	double  dRatHigh = 0.79;
	double  dRatLow = 0.5;
	double  dThrHigh;											//����ֵ
	double  dThrLow;											//����ֵ
	int bound = (int)(dRatHigh * sum);      //����Ŀ����bound����ǰ��Ϊ����ֵ
	sum = 0;
	for (int i = 0; i < max; i++)
	{
		sum += histogram[i];
		if (sum >= bound)
		{
			dThrHigh = i;                                     //����ֵ
			break;
		}
	}
	dThrLow = (int)(dThrHigh*dRatLow);    //����ֵΪ����ֵ�İٷֱ�
	//�Ӻ�ѡ����ѡ����Ե��
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (N[i][j] == 1 && M[i][j] >= dThrHigh)          //����ֵ���
			{
				N[i][j] = 255;
				around(N, i, j, dThrLow);                           //�ж�8����ĵ�
			}
		}
	}
}
//��ȡ��Ե
IplImage*  getEdge(IplImage* gaussianImg,int height,int width)
{
	IplImage* dst = cvCreateImage(cvGetSize(gaussianImg), IPL_DEPTH_8U, gaussianImg->nChannels);
	uchar* ptr;
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)dst->imageData + i*dst->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (N[i][j] == 255)														//�Ƿ�Ϊ��Ե��
				*(ptr + j) = 255;
			else
				*(ptr + j) = 0;
		}
	}
	return dst;
}
int main()
{
	IplImage* srcImg = cvLoadImage("canny.png");
	IplImage* greyImg = RGB2GRAY(srcImg);
	IplImage* gaussianImg = cvCreateImage(cvGetSize(greyImg), IPL_DEPTH_8U, greyImg->nChannels);
	cvSmooth(greyImg, gaussianImg, CV_GAUSSIAN, 7, greyImg->nChannels);		//  Gaussƽ���˲����˴�СΪ7x7   

	int height = srcImg->height;
	int width = srcImg->width;
	//������ά����
	P = new double*[height];
	Q = new double*[height];
	Theta = new double*[height];
	M = new int*[height];
	pcanny = new int*[height];
	N = new int*[height];
	for (int i = 0; i < height; i++)
	{
		P[i] = new double[width];
		Q[i] = new double[width];
		Theta[i] = new double[width];
		M[i] = new int[width];
		pcanny[i] = new int[width];
		N[i] = new int[width];
	}
	//��ȡԭͼ��
	readImage(gaussianImg,height,width);
	//�����ݶȷ�ֵ�ͷ���
	calgradient(height,width);
	//�Ǽ���ֵ����
	nonMaxSuppression(height, width);
	//˫��ֵ���
	twoThresholdCheck(height, width);
	//��ȡ�߽�
	IplImage* dst = getEdge(gaussianImg,height,width);

	cvShowImage("SRC", srcImg);
	cvShowImage("EDGE", dst);
	cvWaitKey(0);
}