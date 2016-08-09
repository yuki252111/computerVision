#include <math.h>  
#include <complex> 
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv/cv.h"
#include <iostream>
using namespace cv;
using namespace std;

// ������ 
#define PI 3.1415926535 
/////////////////////////////////FFT
/*************************************************************************
*
* \�������ƣ�
*   FFT_1D()
*
* \�������:
*   complex<double> * pCTData - ָ��ʱ�����ݵ�ָ�룬�������Ҫ�任������
*   complex<double> * pCFData - ָ��Ƶ�����ݵ�ָ�룬����ľ����任������
*   nLevel ������Ҷ�任�����㷨�ļ�����2��������
*
* \����ֵ:
*   ��
*
* \˵��:
*   һά���ٸ���Ҷ�任��
*
*************************************************************************
*/

void FFT_1D(complex<double> * pCTData, complex<double> * pCFData, int nLevel)
{
	// ѭ�����Ʊ��� 
	int i;
	int     j;
	int     k;

	// ����Ҷ�任���� 
	int nCount = 0;

	// ���㸵��Ҷ�任���� 
	nCount = (int)pow(2.0, nLevel);

	// ĳһ���ĳ��� 
	int nBtFlyLen;
	nBtFlyLen = 0;

	// �任ϵ���ĽǶ� ��2 * PI * i / nCount 
	double dAngle;

	complex<double> *pCW;

	// �����ڴ棬�洢����Ҷ�仯��Ҫ��ϵ���� 
	pCW = new complex<double>[nCount / 2];

	// ���㸵��Ҷ�任��ϵ�� 
	for (i = 0; i < nCount / 2; i++)
	{
		dAngle = -2 * PI * i / nCount;
		pCW[i] = complex<double>(cos(dAngle), sin(dAngle));
	}

	// �任��Ҫ�Ĺ����ռ� 
	complex<double> *pCWork1, *pCWork2;

	// ���乤���ռ� 
	pCWork1 = new complex<double>[nCount];
	pCWork2 = new complex<double>[nCount];

	// ��ʱ���� 
	complex<double> *pCTmp;

	// ��ʼ����д������ 
	memcpy(pCWork1, pCTData, sizeof(complex<double>) * nCount);

	// ��ʱ���� 
	int nInter;
	nInter = 0;

	// �����㷨���п��ٸ���Ҷ�任 
	for (k = 0; k < nLevel; k++)
	{
		for (j = 0; j < (int)pow(2.0, k); j++)
		{
			//���㳤�� 
			nBtFlyLen = (int)pow(2.0, (nLevel - k));

			//�������ţ���Ȩ���� 
			for (i = 0; i < nBtFlyLen / 2; i++)
			{
				nInter = j * nBtFlyLen;
				pCWork2[i + nInter] = pCWork1[i + nInter] + pCWork1[i + nInter + nBtFlyLen / 2];
				pCWork2[i + nInter + nBtFlyLen / 2] = (pCWork1[i + nInter] - pCWork1[i + nInter + nBtFlyLen / 2]) * pCW[(int)(i * pow(2.0, k))];
			}
		}

		// ���� pCWork1��pCWork2������ 
		pCTmp = pCWork1;
		pCWork1 = pCWork2;
		pCWork2 = pCTmp;
	}

	// �������� 
	for (j = 0; j < nCount; j++)
	{
		nInter = 0;
		for (i = 0; i < nLevel; i++)
		{
			if (j&(1 << i))
			{
				nInter += 1 << (nLevel - i - 1);
			}
		}
		pCFData[j] = pCWork1[nInter];
	}

	// �ͷ��ڴ�ռ� 
	delete pCW;
	delete pCWork1;
	delete pCWork2;
	pCW = NULL;
	pCWork1 = NULL;
	pCWork2 = NULL;
}

/*************************************************************************
*
* \�������ƣ�
*   FFT_2D()
*
* \�������:
*   complex<double> * pCTData - ͼ������
*   int    nWidth - ���ݿ��
*   int    nHeight - ���ݸ߶�
*   complex<double> * pCFData - ����Ҷ�任��Ľ��
*
* \����ֵ:
*   ��
*
* \˵��:
*   ��ά����Ҷ�任��
*
************************************************************************
*/
void FFT_2D(complex<double> * pSpaceData, complex<double> * pFrequencyData, int nWidthFFT, int nHeightFFT)
{
	// ѭ�����Ʊ��� 
	int x;
	int y;

	// x��y�����У������ϵĵ������� 
	int nXLev = (int)(log((double)nWidthFFT) / log(2.0) + 0.5);
	int nYLev = (int)(log((double)nHeightFFT) / log(2.0) + 0.5);

	for (y = 0; y < nHeightFFT; ++y)
	{
		// x������п��ٸ���Ҷ�任 
		FFT_1D(&pSpaceData[nWidthFFT * y], &pFrequencyData[nWidthFFT * y], nXLev);
	}

	// pCFData��Ŀǰ�洢��pCTData�����б任�Ľ�� 
	// Ϊ��ֱ������FFT_1D����Ҫ��pCFData�Ķ�ά����ת�ã���һ������FFT_1D���� 

	// ����Ҷ�б任��ʵ�����൱�ڶ��н��и���Ҷ�任�� 
	for (y = 0; y < nHeightFFT; ++y)
	{
		for (x = 0; x < nWidthFFT; ++x)
		{
			pSpaceData[nHeightFFT * x + y] = pFrequencyData[nWidthFFT * y + x];
		}
	}

	for (x = 0; x < nWidthFFT; ++x)
	{
		// ��x������п��ٸ���Ҷ�任��ʵ�����൱�ڶ�ԭ����ͼ�����ݽ����з���� 
		// ����Ҷ�任 
		FFT_1D(&pSpaceData[x * nHeightFFT], &pFrequencyData[x * nHeightFFT], nYLev);
	}

	// pCFData��Ŀǰ�洢��pCTData������ά����Ҷ�任�Ľ��������Ϊ�˷����з��� 
	// �ĸ���Ҷ�任�����������ת�ã����ڰѽ��ת�û��� 
	for (y = 0; y < nHeightFFT; y++)
	{
		for (x = 0; x < nWidthFFT; x++)
		{
			pSpaceData[nHeightFFT * x + y] = pFrequencyData[nWidthFFT * y + x];
		}
	}

	memcpy(pFrequencyData, pSpaceData, sizeof(complex<double>) * nHeightFFT * nWidthFFT);
}

/***********************************************************************
* \�������ƣ�
*    IFFT_1D()
*
* \�������:
*   complex<double> * pCTData - ָ��ʱ�����ݵ�ָ�룬�������Ҫ���任������
*   complex<double> * pCFData - ָ��Ƶ�����ݵ�ָ�룬����ľ������任������
*   nLevel ������Ҷ�任�����㷨�ļ�����2��������
*
* \����ֵ:
*   ��
*
* \˵��:
*   һά���ٸ���Ҷ���任��
*
************************************************************************
*/
void IFFT_1D(complex<double> * pCFData, complex<double> * pCTData, int nLevel)
{
	// ѭ�����Ʊ��� 
	int i;

	// ����Ҷ���任���� 
	int nCount;

	// ���㸵��Ҷ�任���� 
	nCount = (int)pow(2.0, nLevel);

	// �任��Ҫ�Ĺ����ռ� 
	complex<double> *pCWork;

	// ���乤���ռ� 
	pCWork = new complex<double>[nCount];

	// ����Ҫ���任������д�빤���ռ�pCWork 
	memcpy(pCWork, pCFData, sizeof(complex<double>) * nCount);

	// Ϊ�����ø���Ҷ���任,���԰Ѹ���ҶƵ�������ȡ���� 
	// Ȼ��ֱ���������任�����������Ǹ���Ҷ���任����Ĺ��� 
	for (i = 0; i < nCount; i++)
	{
		pCWork[i] = complex<double>(pCWork[i].real(), -pCWork[i].imag());
	}

	// ���ÿ��ٸ���Ҷ�任ʵ�ַ��任������洢��pCTData�� 
	FFT_1D(pCWork, pCTData, nLevel);

	// ��ʱ���Ĺ��������ս�� 
	// ���ݸ���Ҷ�任ԭ�����������ķ�����õĽ����ʵ�ʵ�ʱ������ 
	// ���һ��ϵ��nCount 
	for (i = 0; i < nCount; i++)
	{
		pCTData[i] =
			complex<double>(pCTData[i].real() / nCount, -pCTData[i].imag() / nCount);
	}

	// �ͷ��ڴ� 
	delete pCWork;
	pCWork = NULL;
}

/*************************************************
Function:       IFFT_2D
Description:    ��ά����Ҷ��任
input:
*   complex<double> * pSpaceData		- ͼ�����ݵĸ�����ʽ���ռ���
*   int    nWidthFFT						- ���ݿ��
*   int    nHeightFFT						- ���ݸ߶�
output:
*   complex<double> * pFrequencyData	- ����Ҷ�任��Ľ����Ƶ����
Return:
Others:	 nWidthFFT��nHeightFFTҪ����2������
*************************************************/
void IFFT_2D(complex<double> *pFrequencyData, int nWidthFFT, int nHeightFFT, complex<double> *pSpaceData)
{
	// ѭ�����Ʊ���
	int	x;
	int	y;

	// ����x��y�����ϵĵ�������
	int	nXLev = (int)(log((double)nWidthFFT) / log(2.0) + 0.5);
	int	nYLev = (int)(log((double)nHeightFFT) / log(2.0) + 0.5);

	// 1���з�����п��ٸ���Ҷ��任	
	for (y = 0; y < nHeightFFT; ++y)
	{
		IFFT_1D(&pFrequencyData[nWidthFFT * y], &pSpaceData[nWidthFFT * y], nXLev);
	}

	// 2��ת��
	for (y = 0; y<nHeightFFT; ++y)
	{
		for (x = 0; x<nWidthFFT; ++x)
			pFrequencyData[nHeightFFT * x + y] = pSpaceData[nWidthFFT * y + x];
	}

	// 3���з�����п��ٸ���Ҷ��任��	
	for (x = 0; x<nWidthFFT; ++x)
	{
		IFFT_1D(&pFrequencyData[nHeightFFT * x], &pSpaceData[nHeightFFT * x], nYLev);
	}

	// 4��ת�û���
	for (y = 0; y<nHeightFFT; ++y)
	{
		for (x = 0; x<nWidthFFT; ++x)
			pFrequencyData[nHeightFFT * x + y] = pSpaceData[nWidthFFT * y + x];
	}

	// 5�����ռ���ֵ
	memcpy(pSpaceData, pFrequencyData, sizeof(complex<double>) * nHeightFFT * nWidthFFT);
}

void main()
{
	IplImage* src = cvLoadImage("proof.png");  //����ԭͼ��
	int m_nPicWidth = src->width;
	int m_nPicHeight = src->height;
	uchar* m_pImageTempBuffer = new uchar[m_nPicHeight*m_nPicWidth * 4];
	uchar* ptr;
	for (int i = 0; i < m_nPicHeight; i++)
	{
		ptr = (uchar*)src->imageData + i*src->widthStep;
		for (int j = 0; j < m_nPicWidth; j++)
		{
			m_pImageTempBuffer[i*m_nPicWidth*4 + 4*j + 0] = *(ptr+3*j+0);
			m_pImageTempBuffer[i*m_nPicWidth*4 + 4 * j + 1] = *(ptr + 3 * j + 1);
			m_pImageTempBuffer[i*m_nPicWidth*4 + 4 * j + 2] = *(ptr + 3 * j + 2);
			m_pImageTempBuffer[i*m_nPicWidth * 4 + 4 * j + 3] = 0;
		}
	}
	//��ʾԭͼ
	cvShowImage("src", src);
	//����Ҷ�任�Ŀ�Ⱥ͸߶�
	int nHeightFFT;
	int nWidthFFT;

	/*���㸵��Ҷ�任�Ŀ�Ⱥ͸߶ȣ���2������*/
	double dTemp = log((double)m_nPicWidth) / log(2.0);
	dTemp = ceil(dTemp);//���ش��ڻ��ߵ���ָ�����ʽ����С����
	dTemp = pow(2.0, dTemp);
	nWidthFFT = (int)dTemp;

	dTemp = log((double)m_nPicHeight) / log(2.0);
	dTemp = ceil(dTemp);
	dTemp = pow(2.0, dTemp);
	nHeightFFT = (int)dTemp;

	//ָ��������ݵ�ָ��
	complex<double>* pSpaceDataB, *pSpaceDataG, *pSpaceDataR;
	//ָ��Ƶ�����ݵ�ָ��
	complex<double>* pFrequencyData1, *pFrequencyData2, *pFrequencyData3;

	//����洢�ռ�
	pSpaceDataB = new complex<double>[nWidthFFT * nHeightFFT];
	pSpaceDataG = new complex<double>[nWidthFFT * nHeightFFT];
	pSpaceDataR = new complex<double>[nWidthFFT * nHeightFFT];
	pFrequencyData1 = new complex<double>[nWidthFFT * nHeightFFT];
	pFrequencyData2 = new complex<double>[nWidthFFT * nHeightFFT];
	pFrequencyData3 = new complex<double>[nWidthFFT * nHeightFFT];

	int x = 0;
	int y = 0;
	int pixel = 0;

	/*��ʼ����������*/
	for (y = 0; y<nHeightFFT; ++y)
	{
		for (x = 0; x<nWidthFFT; ++x)
		{
			pSpaceDataB[y*nWidthFFT + x] = complex<double>(0, 0);
			pSpaceDataG[y*nWidthFFT + x] = complex<double>(0, 0);
			pSpaceDataR[y*nWidthFFT + x] = complex<double>(0, 0);
		}
	}

	//��ͼ�����ݴ���pSpaceData
	for (y = 0; y<m_nPicHeight; ++y)
	{
		for (x = 0; x<m_nPicWidth; ++x, pixel += 4)
		{
			//�������pow(-1, x+y)��Ƶ����������
			double transform = pow(-1, x + y);
			pSpaceDataB[y*nWidthFFT + x] = complex<double>(m_pImageTempBuffer[pixel] * transform, 0);
			pSpaceDataG[y*nWidthFFT + x] = complex<double>(m_pImageTempBuffer[pixel + 1] * transform, 0);
			pSpaceDataR[y*nWidthFFT + x] = complex<double>(m_pImageTempBuffer[pixel + 2] * transform, 0);
		}
	}
	FFT_2D(pSpaceDataB, pFrequencyData1, nWidthFFT, nHeightFFT);
	FFT_2D(pSpaceDataG, pFrequencyData2, nWidthFFT, nHeightFFT);
	FFT_2D(pSpaceDataR, pFrequencyData3, nWidthFFT, nHeightFFT);

	pixel = 0;
	for (y = 0; y<m_nPicHeight; ++y)
	{
		for (x = 0; x<m_nPicWidth; ++x, pixel += 4)
		{
			//ȡģ
			dTemp = pFrequencyData1[y*nWidthFFT + x].real() * pFrequencyData1[y*nWidthFFT + x].real() + pFrequencyData1[y*nWidthFFT + x].imag()*pFrequencyData1[y*nWidthFFT + x].imag();
			dTemp = sqrt(dTemp) / (double)m_nPicWidth;//��������N
			//ͼ������С��255
			if (dTemp > 255) dTemp = 255;
			m_pImageTempBuffer[pixel] = (uchar)dTemp;

			dTemp = pFrequencyData2[y*nWidthFFT + x].real() * pFrequencyData2[y*nWidthFFT + x].real() + pFrequencyData2[y*nWidthFFT + x].imag()*pFrequencyData2[y*nWidthFFT + x].imag();
			dTemp = sqrt(dTemp) / (double)m_nPicWidth;
			if (dTemp > 255) dTemp = 255;
			m_pImageTempBuffer[pixel + 1] = (uchar)dTemp;

			dTemp = pFrequencyData3[y*nWidthFFT + x].real() * pFrequencyData3[y*nWidthFFT + x].real() + pFrequencyData3[y*nWidthFFT + x].imag()*pFrequencyData3[y*nWidthFFT + x].imag();
			dTemp = sqrt(dTemp) / (double)m_nPicWidth;
			if (dTemp > 255) dTemp = 255;
			m_pImageTempBuffer[pixel + 2] = (uchar)dTemp;
		}
	}
	ptr = NULL; pixel = 0;
	IplImage* fft_img = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	for (y = 0; y<m_nPicHeight; ++y)
	{
		ptr = (uchar*)fft_img->imageData + y*fft_img->widthStep;
		for (x = 0; x<m_nPicWidth; ++x, pixel += 4)
		{
			*(ptr + 3 * x + 0) = (uchar)m_pImageTempBuffer[pixel + 0];
			*(ptr + 3 * x + 1) = (uchar)m_pImageTempBuffer[pixel + 1];
			*(ptr + 3 * x + 2) = (uchar)m_pImageTempBuffer[pixel+ 2];
		}
	}
	cvShowImage("fft", fft_img);
	//����Ҷ���任
	for (y = 0; y < nHeightFFT; ++y)
	{
		for (x = 0; x < nWidthFFT; ++x)
		{
			pSpaceDataB[y*nWidthFFT + x] = complex<double>(0, 0);
			pSpaceDataG[y*nWidthFFT + x] = complex<double>(0, 0);
			pSpaceDataR[y*nWidthFFT + x] = complex<double>(0, 0);
		}
	}

	//���ٸ���Ҷ�任
	IFFT_2D(pFrequencyData1, nWidthFFT, nHeightFFT, pSpaceDataB);
	IFFT_2D(pFrequencyData2, nWidthFFT, nHeightFFT, pSpaceDataG);
	IFFT_2D(pFrequencyData3, nWidthFFT, nHeightFFT, pSpaceDataR);

	pixel = 0;
	for (y = 0; y < m_nPicHeight; ++y)
	{
		for (x = 0; x < m_nPicWidth; ++x, pixel += 4)
		{
			dTemp = pSpaceDataB[y * nWidthFFT + x].real() * pSpaceDataB[y * nWidthFFT + x].real() +
				pSpaceDataB[y * nWidthFFT + x].imag() * pSpaceDataB[y * nWidthFFT + x].imag();
			dTemp = sqrt(dTemp);
			if (dTemp > 255)dTemp = 255;
			m_pImageTempBuffer[pixel] = (uchar)dTemp;

			dTemp = pSpaceDataG[y * nWidthFFT + x].real() * pSpaceDataG[y * nWidthFFT + x].real() +
				pSpaceDataG[y * nWidthFFT + x].imag() * pSpaceDataG[y * nWidthFFT + x].imag();
			dTemp = sqrt(dTemp);
			if (dTemp > 255)dTemp = 255;
			m_pImageTempBuffer[pixel + 1] = (uchar)dTemp;

			dTemp = pSpaceDataR[y * nWidthFFT + x].real() * pSpaceDataR[y * nWidthFFT + x].real() +
				pSpaceDataR[y * nWidthFFT + x].imag() * pSpaceDataR[y * nWidthFFT + x].imag();
			dTemp = sqrt(dTemp);
			if (dTemp > 255)dTemp = 255;
			m_pImageTempBuffer[pixel + 2] = (uchar)dTemp;
		}
	}
	ptr = NULL; pixel = 0;
	IplImage* ifft_img = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	for (y = 0; y<m_nPicHeight; ++y)
	{
		ptr = (uchar*)ifft_img->imageData + y*ifft_img->widthStep;
		for (x = 0; x<m_nPicWidth; ++x, pixel += 4)
		{
			*(ptr + 3 * x + 0) = (uchar)m_pImageTempBuffer[pixel + 0];
			*(ptr + 3 * x + 1) = (uchar)m_pImageTempBuffer[pixel + 1];
			*(ptr + 3 * x + 2) = (uchar)m_pImageTempBuffer[pixel + 2];
		}
	}
	cvShowImage("Ifft", ifft_img);
	//�ͷ��ڴ�
	delete[]pSpaceDataB;
	delete[]pSpaceDataG;
	delete[]pSpaceDataR;
	delete[]pFrequencyData1;
	delete[]pFrequencyData2;
	delete[]pFrequencyData3;
	pSpaceDataB = NULL;
	pSpaceDataG = NULL;
	pSpaceDataR = NULL;
	pFrequencyData1 = NULL;
	pFrequencyData2 = NULL;
	pFrequencyData3 = NULL;
	
	cvWaitKey(0);
}
