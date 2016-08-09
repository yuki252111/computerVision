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
	//���ļ��ж���ͼ��
	Mat img_1 = imread("class.png");
	Mat img_2 = imread("class2.png");
	//�������ͼ��ʧ��
	if (img_1.empty() || img_2.empty())
	{
		cout << "load image error" << endl;
		return -1;
	}
	//��ʾͼ��
	imshow("src image 1", img_1);
	imshow("src image 2", img_2);
	//��һ������SIFT���Ӽ��ؼ���
	SiftFeatureDetector detector;//���캯�������ڲ�Ĭ�ϵ�
	std::vector<KeyPoint> keypoints_1, keypoints_2;//����2��ר���ɵ���ɵĵ����������洢������

	detector.detect(img_1, keypoints_1);//��img_1ͼ���м�⵽��������洢��������keypoints_1��
	detector.detect(img_2, keypoints_2);//ͬ��

	//��ͼ���л���������
	Mat img_keypoints_1, img_keypoints_2;

	drawKeypoints(img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);//���ڴ��л���������
	drawKeypoints(img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

	imshow("sift_keypoints_1", img_keypoints_1);//��ʾ������
	imshow("sift_keypoints_2", img_keypoints_2);

	//������������
	SiftDescriptorExtractor extractor;//���������Ӷ���
	Mat descriptors_1, descriptors_2;//������������ľ���

	extractor.compute(img_1, keypoints_1, descriptors_1);//������������
	extractor.compute(img_2, keypoints_2, descriptors_2);

	//��burte force����ƥ����������
	BruteForceMatcher<L2<float>>matcher;//����һ��burte force matcher����
	vector<DMatch>matches;
	matcher.match(descriptors_1, descriptors_2, matches);

	//����ƥ���߶�
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_matches);//��ƥ������Ľ�������ڴ�img_matches��

	//��ʾƥ���߶�
	imshow("sift_Matches", img_matches);//��ʾ�ı���ΪMatches
	cvWaitKey(0);
}