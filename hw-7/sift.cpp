#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include<opencv2/nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>
#include<vector>
#include<iostream>
using namespace std;
using namespace cv;



int main()
{
	//从文件中读入图像
	Mat img_1 = imread("class.png");
	Mat img_2 = imread("class2.png");
	//如果读入图像失败
	if (img_1.empty() || img_2.empty())
	{
		cout << "load image error" << endl;
		return -1;
	}
	//显示图像
	imshow("src image 1", img_1);
	imshow("src image 2", img_2);
	//第一步，用SIFT算子检测关键点
	SiftFeatureDetector detector;//构造函数采用内部默认的
	std::vector<KeyPoint> keypoints_1, keypoints_2;//构造2个专门由点组成的点向量用来存储特征点

	detector.detect(img_1, keypoints_1);//将img_1图像中检测到的特征点存储起来放在keypoints_1中
	detector.detect(img_2, keypoints_2);//同理

	//在图像中画出特征点
	Mat img_keypoints_1, img_keypoints_2;

	drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);//在内存中画出特征点
	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	imshow("sift_keypoints_1", img_keypoints_1);//显示特征点
	imshow("sift_keypoints_2", img_keypoints_2);

	//计算特征向量
	SiftDescriptorExtractor extractor;//定义描述子对象
	Mat descriptors_1, descriptors_2;//存放特征向量的矩阵

	extractor.compute(img_1, keypoints_1, descriptors_1);//计算特征向量
	extractor.compute(img_2, keypoints_2, descriptors_2);

	//用burte force进行匹配特征向量
	BruteForceMatcher<L2<float>>matcher;//定义一个burte force matcher对象
	vector<DMatch>matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	//绘制匹配线段
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);//将匹配出来的结果放入内存img_matches中

	//显示匹配线段
	imshow("sift_Matches", img_matches);//显示的标题为Matches
	cvWaitKey(0);
}