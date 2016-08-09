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
//-------------------------------------------
int** pcanny = NULL;   //原图像
//--------读取原图像------------
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
//-----计算梯度幅值和方向---------------
/////    P[i,j]=(S[i,j+1]-S[i,j]+S[i+1,j+1]-S[i+1,j])/2     /////  
/////    Q[i,j]=(S[i,j]-S[i+1,j]+S[i,j+1]-S[i+1,j+1])/2     /////  

double** P = NULL;    //x方向偏导数
double** Q = NULL;   //y方向偏导数
int** M =NULL;          //梯度幅值  
double** Theta = NULL;   //梯度方向  

void calgradient(int height,int width)
{
	//计算xy偏导数
	for (int i = 0; i < height - 1; i++)
	{
		for (int j = 0; j < width - 1; j++)
		{
			P[i][j] = (double)(pcanny[i][min(j + 1, width - 1)] - pcanny[i][j] + pcanny[min(i + 1, height - 1)][min(j + 1, width - 1)] - pcanny[min(i + 1,height - 1)][j]) / 2.0;
			Q[i][j] = (double)(pcanny[i][j] - pcanny[min(i + 1, height - 1)][j] + pcanny[i][min(j + 1, width - 1)] - pcanny[min(i + 1, height - 1)][min(j + 1, width - 1)]) / 2.0;
		}
	}
	//计算幅值和方向
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			M[i][j] = (int)sqrt(P[i][j] * P[i][j] + Q[i][j] * Q[i][j]);
			Theta[i][j] = atan2(Q[i][j], P[i][j])*57.3;		//弧度转角度
			if (Theta[i][j] < 0)
				Theta[i][j] += 360;			//转成0-360之间
		}
	}
}
//---------------------------------------------------------
//------------非极大值抑制------------------------------
int ** N = NULL;												//非极大值抑制结果  
int g1 = 0, g2 = 0, g3 = 0, g4 = 0;                            //用于进行插值，得到亚像素点坐标值  
double dTmp1 = 0.0, dTmp2 = 0.0;                           //保存两个亚像素点插值得到的灰度数据  
double dWeight = 0.0;                                    //插值的权重  

void nonMaxSuppression(int height, int width)
{
	//边界初始化
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
	//非极大值抑制
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			if (M[i][j] == 0)
				N[i][j] = 0;
			else
			{
				////////首先判断属于那种情况，然后根据情况插值///////  
			   ////////////////////第一种情况///////////////////////  
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
					dWeight = abs(P[i][j]) / abs(Q[i][j]);   //反正切  
					dTmp1 = g1*dWeight + g2*(1 - dWeight);
					dTmp2 = g4*dWeight + g3*(1 - dWeight);
				}
				////////////////////第二种情况///////////////////////  
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
					dWeight = abs(Q[i][j]) / abs(P[i][j]);   //正切  
					dTmp1 = g2*dWeight + g1*(1 - dWeight);
				    dTmp2 = g4*dWeight + g3*(1 - dWeight);
				}
				////////////////////第三种情况///////////////////////  
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
				   dWeight = abs(P[i][j]) / abs(Q[i][j]);   //反正切  
				   dTmp1 = g2*dWeight + g1*(1 - dWeight);
				   dTmp2 = g3*dWeight + g4*(1 - dWeight);
				}
			     ////////////////////第四种情况///////////////////////  
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
					 dWeight = abs(Q[i][j]) / abs(P[i][j]);   //正切  
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
//------------双阈值检测---------------------------------------
void around(int** N , int i, int j,int thr)        //遍历边缘点的邻域，采取低阈值检测
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
			around(N, xx, yy, thr);                //邻域的点
		}
   }
}
void twoThresholdCheck(int height, int width)
{
	int histogram[400];  //幅值直方图
	for (int i = 0; i < 400; i++)
		histogram[i] = 0;
	//获取边界候选梯度幅值的直方图
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			if (N[i][j]==1)
				histogram[M[i][j]]++;
	//获取潜在边缘点的个数总和和最大值
	int max = 0;
	int sum = histogram[0];
	for (int i = 1; i < 400; i++)
	{
		if (histogram[i] != 0)
			max = i;
		sum += histogram[i];
	}
	//获取高阈值和低阈值
	double  dRatHigh = 0.79;
	double  dRatLow = 0.5;
	double  dThrHigh;											//高阈值
	double  dThrLow;											//低阈值
	int bound = (int)(dRatHigh * sum);      //当数目大于bound，则当前作为高阈值
	sum = 0;
	for (int i = 0; i < max; i++)
	{
		sum += histogram[i];
		if (sum >= bound)
		{
			dThrHigh = i;                                     //高阈值
			break;
		}
	}
	dThrLow = (int)(dThrHigh*dRatLow);    //低阈值为高阈值的百分比
	//从候选点中选出边缘点
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (N[i][j] == 1 && M[i][j] >= dThrHigh)          //高阈值检测
			{
				N[i][j] = 255;
				around(N, i, j, dThrLow);                           //判断8邻域的点
			}
		}
	}
}
//获取边缘
IplImage*  getEdge(IplImage* gaussianImg,int height,int width)
{
	IplImage* dst = cvCreateImage(cvGetSize(gaussianImg), IPL_DEPTH_8U, gaussianImg->nChannels);
	uchar* ptr;
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)dst->imageData + i*dst->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (N[i][j] == 255)														//是否为边缘点
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
	cvSmooth(greyImg, gaussianImg, CV_GAUSSIAN, 7, greyImg->nChannels);		//  Gauss平滑滤波，核大小为7x7   

	int height = srcImg->height;
	int width = srcImg->width;
	//创建二维数组
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
	//读取原图像
	readImage(gaussianImg,height,width);
	//计算梯度幅值和方向
	calgradient(height,width);
	//非极大值抑制
	nonMaxSuppression(height, width);
	//双阈值检测
	twoThresholdCheck(height, width);
	//获取边界
	IplImage* dst = getEdge(gaussianImg,height,width);

	cvShowImage("SRC", srcImg);
	cvShowImage("EDGE", dst);
	cvWaitKey(0);
}