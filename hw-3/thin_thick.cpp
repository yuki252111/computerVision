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
int getThreshold(int** p, int height, int width) //获取阈值
{
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)  //建立直方图
	{
		for (int j = 0; j < width; j++)
			histogram[p[i][j]]++;
	}

	long size = height*width;
	int threshold = 0;
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

		for (int j = i + 1; j <= 255; j++)
		{
			cnt1 += histogram[j];
			sum1 += j * histogram[j];
		}

		u1 = (double)sum1 / cnt1;
		w1 = 1 - w0; // (double)cnt1 / size;

		u = u0 * w0 + u1 * w1; //图像的平均灰度

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
void hitOrMiss(int**p, int height, int width, int** strucEle, int originX, int originY, int row, int col)//hitOrMiss操作
{
	//原理：1.结构元素全为1的点在原图像内
	//      2.结构元素全为0的点要么在界外，要么原图像对应点也为0
	//满足上述条件的点的值为1，否则为0
	vector<pair<int, int>>allOne;  //结构元素为1的点
	vector<pair<int, int>>allZero; //结构元素为0的点
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
	//复制副本
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
				//判断为1的点是否全在图像内
				int x = allOne[k].first - originX + i;
				int y = allOne[k].second - originY + j;
				if (x < 0 || x >= height || y < 0 || y >= width)//有点在界外
				{
					one = false;
					break;
				}
				else
				{
					if (copy[x][y] == 0)    //原图像对应点为0
					{
						one = false;
						break;
					}
				}
			}
			for (int k = 0; k < allZero.size(); k++)
			{
				//判断结构元素全为0的点要么在界外，要么原图像对应点也为0
				int x = allZero[k].first - originX + i;
				int y = allZero[k].second - originY + j;

				if (!(x < 0 || x >= height || y < 0 || y >= width) && copy[x][y] != 0)
				{//点在界内但是当前原图像对应点不是0
					zero = false;
					break;
				}
			}
			if (one == true && zero == true) //满足1，2条件
				p[i][j] = 1;
			else
				p[i][j] = 0;

		}
	}
}
void perThin(int**p, int height, int width, int** strucEle, int originX, int originY, int row, int col)//一个thin操作
{
	//复制二维数组
	int** copy = new int*[height];
	for (int i = 0; i < height; i++)
		copy[i] = new int[width];
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			copy[i][j] = p[i][j];
	//进行hitOrMiss变换
	hitOrMiss(copy, height, width, strucEle, originX, originY, row, col);
	//将原数组减去变化后的数组，即为thin操作
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = p[i][j] - copy[i][j];
}
void allThin(int** p, int height, int width,int maxCount=1)  //进行所有的thin操作
{
	//先构造8个结构元素
	int originX, originY;
	int** p1 = new int*[3];
	for (int i = 0; i < 3; i++)
		p1[i] = new int[3];

	originX = 1; originY = 1;  //结构元素的原点
	for (int count = 0; count < maxCount; count++) //重复进行100次
	{
		//第一个结构元素，用它进行thin操作
		p1[0][0] = 0; p1[0][1] = 0; p1[0][2] = 0;
		p1[1][0] = -1; p1[1][1] = 1; p1[1][2] = -1;
		p1[2][0] = 1; p1[2][1] = 1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第二个结构元素，用它进行thin操作
		p1[0][0] = -1; p1[0][1] = 0; p1[0][2] = 0;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = 1; p1[2][1] = 1; p1[2][2] = -1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第三个结构元素，用它进行thin操作
		p1[0][0] = 1; p1[0][1] = -1; p1[0][2] = 0;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = 1; p1[2][1] = -1; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第四个结构元素，用它进行thin操作
		p1[0][0] = 1; p1[0][1] = 1; p1[0][2] = -1;
		p1[1][0] = 1; p1[1][1] = 1; p1[1][2] = 0;
		p1[2][0] = -1; p1[2][1] = 0; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第五个结构元素，用它进行thin操作
		p1[0][0] = 1; p1[0][1] = 1; p1[0][2] = 1;
		p1[1][0] = -1; p1[1][1] = 1; p1[1][2] = -1;
		p1[2][0] = 0; p1[2][1] = 0; p1[2][2] = 0;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第六个结构元素，用它进行thin操作
		p1[0][0] = -1; p1[0][1] = 1; p1[0][2] = 1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = 0; p1[2][1] = 0; p1[2][2] = -1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第七个结构元素，用它进行thin操作
		p1[0][0] = 0; p1[0][1] = -1; p1[0][2] = 1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = 0; p1[2][1] = -1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
		//第八个结构元素，用它进行thin操作
		p1[0][0] = 0; p1[0][1] = 0; p1[0][2] = -1;
		p1[1][0] = 0; p1[1][1] = 1; p1[1][2] = 1;
		p1[2][0] = -1; p1[2][1] = 1; p1[2][2] = 1;
		perThin(p, height, width, p1, originX, originY, 3, 3);
	}
}
IplImage* thin(IplImage* rgb,int maxCount=1)          //进行图像处理并调用thin操作
{
	IplImage* thinImage = cvCreateImage(cvGetSize(rgb), rgb->depth, rgb->nChannels);
	cvCopy(rgb, thinImage);

	IplImage* grey = RGB2GRAY(rgb);             //转成灰度

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
	int threshold = getThreshold(p, height, width);  //获取阈值
	for (int i = 0; i < height; i++)                //二值化二维数组
	{
		for (int j = 0; j < width; j++)
		{
			if (p[i][j]>threshold)
				p[i][j] = 1;
			else
				p[i][j] = 0;
		}
	}
	allThin(p, height, width,maxCount);              //进行所有的thin操作
	//将thin操作得到的图像用绿色表示
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)thinImage->imageData + i*thinImage->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (p[i][j] == 1)                  //判断是图像
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

	IplImage* grey = RGB2GRAY(rgb);             //转成灰度

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
	int threshold = getThreshold(p, height, width);  //获取阈值
	for (int i = 0; i < height; i++)                //二值化二维数组
	{
		for (int j = 0; j < width; j++)
		{
			if (p[i][j]>threshold)
				p[i][j] = 1;
			else
				p[i][j] = 0;
		}
	}
	//取补集
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = 1 - p[i][j];

	allThin(p, height, width, maxCount);              //对补集进行所有的thin操作
	//将thin操作得到的图像用绿色表示
	//再次取补集，得到thick
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			p[i][j] = 1 - p[i][j];
	for (int i = 0; i < height; i++)
	{
		ptr = (uchar*)thickImage->imageData + i*thickImage->widthStep;
		for (int j = 0; j < width; j++)
		{
			if (p[i][j] == 1)                  //判断是图像
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
	IplImage* rgb = cvLoadImage("potato.png");  //读取图像
	IplImage* thinImage = thin(rgb,3);
	IplImage* thickImage = thick(rgb,3);
	//显示图像
	cvShowImage("RGB", rgb);
	cvShowImage("THIN", thinImage);
	cvShowImage("THICK", thickImage);
	cvWaitKey(0);

}