#ifndef _cplxc_h_ 
#define _cplxc_h_ 
#include <stdlib.h>
#endif
#define NULL 0
#define L 1024
#define BITWIDTH 32
//#define DIMM 1024*1024

//******************************************************************//
//																	//
//					���뺯��	���ص���ֵ							//
//																	//
//		int n :	������ֵ											//
//		int depth: ����λ��											//
//		����ֵ��Ϊ-1˵���������									//
//																	//
//******************************************************************//

int revs( int n , int depth ) {
	if(n>=(int)pow(2,depth))
		return -1;
	int temp[BITWIDTH],j=0,m,k;
	for(k=0;k<depth;k++) {
		m = depth-k-1;
		temp[m] = (n>>k) & 1;
	}
	for(k=0;k<depth;k++)
		j = j+(int)(temp[k]*(int)pow(2,k));
	return j;
}

//******************************************************************//
//																	//
//							ת�ú���								//
//																	//
//		complex * A : Ҫת�õľ���									//
//		int dim : ����A��������������Ҫ������������ȣ�				//
//		����ֵ��Ϊ-1˵���������									//
//																	//
//******************************************************************//

int trsp(complex * A, int dim) {
	if(dim>L)
		return 0;
	else {
		int i=0,j=0,num,n;
		num = dim*dim;
		complex* pt = (complex*)malloc(num*sizeof(complex));
		for(n=0;n<num;++n) {
			pt[n] = A[n];
		}
		for(n=0,i=0;i < dim;i++) {
			for(j=0;j < dim;j++) {
				A[n++] = pt[j*dim+i];
			}
		}
		free(pt);
		return 1;
	}
}

//******************************************************************//
//																	//
//				���ٸ���Ҷ�㷨		����DFT����						//
//																	//
//	complex * Sdata :ԭ�����׵�ַ									//
//	complex * Idata :Ŀ�������׵�ַ									//
//	int w :���������������(2^w��)									//
//																	//
//******************************************************************//

int DFT( complex * Sdata , complex * Idata , int w ) {
	int length = (int)pow(2,w);
	if(length > L)
		return 0;
	else {
		const complex Z = {0,-2.0*PIE/length,1};
		const complex Wn = cplx_exp(Z);
		complex temp={0,0,1},Wnk;
		complex *pd=Sdata;
		int n,k;
		for(k=0;k<length;k++) {
			temp.im = 0;
			temp.re = 0;
			for(n=0,pd=Sdata;n<length;n++,pd++) {
				Wnk = cplx_pow(Wn,n*k);
				temp = cplx_add(temp,cplx_mult(*pd,Wnk));
			}
			Idata[k] = temp;
		}
		return 1;
	}
}

//******************************************************************//
//																	//
//				���ٸ���Ҷ�㷨		���ڵ���ͼ						//
//																	//
//	complex * data :�����׵�ַ										//
//	int w :���������������(2^w��)									//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

int FFT( complex * data , int w ) {
	int length = (int)pow(2,w);
	if(length > L)
		return 0;
	else {
		complex Dtmp[L],Wnk[L/2],t={0,0,1},Wn;
		complex * pD1 = Dtmp , * pD2 = data;
		int length = (int)pow(2,w);
		int n,k,m,offset,dis;
		const complex Z = {0,-2.0*PIE,1};
		for(n=0;n<length;n++)
			Dtmp[n] = data[revs(n,w)];
		for(k=0;k<w;k++) {
			t.im = Z.im/(double)pow(2,k+1);
			Wn = cplx_exp(t);
			for(n=0;n<(double)pow(2,k);n++)
				Wnk[n] = cplx_pow(Wn,n);
			offset = (int)pow(2,k+1);
			dis = (int)pow(2,k);
			for(n=0;n<(int)pow(2,w-k-1);n++) {	
				for(m=0;m<(int)pow(2,k);m++) {
					*(pD2+n*offset+m) = cplx_add(*(pD1+n*offset+m),
						cplx_mult(*(pD1+n*offset+dis+m),Wnk[m]));
					*(pD2+n*offset+dis+m) = cplx_sub(*(pD1+n*offset+m),
						cplx_mult(*(pD1+n*offset+dis+m),Wnk[m]));
				}
			}
			for(n=0;n<length;n++)
				Dtmp[n] = data[n];
		}
		return 1;
	}
}

//******************************************************************//
//																	//
//				���ٶ�ά����Ҷ�㷨		����DFT						//
//																	//
//	���þ���ת��													//
//	complex * data :��������׵�ַ									//
//	int w :�������߳�(2^w��)										//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

int DFT2( complex * data , int w ) {
	if((int)pow(2,w)>L) {
		return 0;
	}
	else {
		complex * pl;
		complex* pt = (complex*)malloc((int)pow(2,w)*sizeof(complex));
		int k,n;
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[(int)(k*(int)pow(2,w))]);
			DFT(pl,pt,w);
			for(n=0;n<pow(2,w);n++) {
				pl[n] = pt[n];
			}
		}
		pl=&data[0];
		trsp(data,(int)pow(2,w));
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[k*(int)pow(2,w)]);
			DFT(pl,pt,w);
			for(n=0;n<pow(2,w);n++) {
				pl[n] = pt[n];
			}
		}
		free(pt);
		return 1;
	}
}

//******************************************************************//
//																	//
//				���ٶ�ά����Ҷ�㷨		����FFT						//
//																	//
//	���þ���ת��													//
//	complex * data :��������׵�ַ									//
//	int w :�������߳�(2^w��)										//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

int FFT2( complex * data , int w ) {
	if((int)pow(2,w)>L) {
		return 0;
	}
	else {
		complex * pl;
		int k;
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[(int)(k*(int)pow(2,w))]);
			FFT(pl,w);
		}
		pl=&data[0];
		trsp(data,(int)pow(2,w));
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[k*(int)pow(2,w)]);
			FFT(pl,w);
		}
		return 1;
	}
}

//******************************************************************//
//																	//
//				FFTSHIFT	FFT������Ļ�							//
//																	//
//	complex * data :���������׵�ַ									//
//	int w :�������鳤��(2^w��)										//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

int fftshift( complex *data , int w ) {
	if((int)pow(2,w)>L)
		return 0;
	else {
		complex *pt = (complex *)malloc((int)(pow(2,w)*sizeof(complex)));
		int n=0;
		for(n=0;n<(int)pow(2,w-1);n++) {
			*(pt+n) = *(data+(int)pow(2,w-1)+n);
			*(pt+(int)pow(2,w-1)+n) = *(data+n);
		}
		for(n=0;n<(int)pow(2,w);n++)
			data[n] = pt[n];
		free(pt);
		return 1;
	}
}

//******************************************************************//
//																	//
//				FFT2SHIFT	FFT2������Ļ�							//
//		����FFTSHIFT����											//
//																	//
//	complex * data :��������׵�ַ									//
//	int w :�������߳�(2^w��)										//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

int fft2shift( complex * data , int w ) {
	if((int)pow(2,w)>L)
		return 0;
	else {
		complex * pl;
		int k;
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[(int)(k*(int)pow(2,w))]);
			fftshift(pl,w);
		}
		pl=&data[0];
		trsp(data,(int)pow(2,w));
		for(k=0;k<pow(2,w);k++) {
			pl=&(data[k*(int)pow(2,w)]);
			fftshift(pl,w);
		}
		return 1;
	}
}

//******************************************************************//
//																	//
//				findmax	����һ����������ֵ						//
//		���ڻҶ�ֵ����												//
//																	//
//	double * data :���������׵�ַ									//
//	int length :�������鳤��										//
//	����ֵΪ�������ֵ��������ֵȫΪ0�������1�������0����			//
//																	//
//******************************************************************//

double findmax( double * data , int length ) {		
	int n;											
	double max=0;									
	for(n=0;n<=length;n++)							
		max = max>abs(data[n])? max:abs(data[n]);			
	return max==0?1:max;							
}

//******************************************************************//
//																	//
//				average	����һ������ľ���ֵƽ��ֵ					//
//		���ڻҶ�ֵ����												//
//																	//
//	double * data :���������׵�ַ									//
//	int length :�������鳤��										//
//	����ֵΪ0���������												//
//	����ֵΪ1��������ȷ												//
//																	//
//******************************************************************//

double average( double * data , int length ) {		
	int n;											
	double max=0;								
	for(n=0;n<=length;n++)							
		max = max+abs(data[n]);							
	return max==0?1:(max/(double)length);			
}													

//******************************************************************//
//																	//
//				DATA2BMP	���ݽ�����ӻ�							//
//		���ڷ����б任���������������Ϊ0~255�ĻҶ�ֵ				//
//																	//
//	complex * data :���������׵�ַ									//
//	unsigned char * bmp :λͼ��������ռ��׵�ַ						//
//	int w,h :�������ݳߴ�(m*n)										//
//	int den :����ϵ��												//
//	����ֵΪ�������ƽ��ֵ��������ֵȫΪ0�������1�������0����		//
//																	//
//******************************************************************//

int data2bmp( complex * data  , unsigned char * bmp , int w , int h , int den ) {
	if(den == 0)
		return 0;
	else {
		double * tmp = (double*)malloc(w*h*sizeof(double));
		int lineByte=(w+3)/4*4;
		int i=0,j=0,n=0;
		for(i=0;i<w;i++) {
			for(j=0;j<h;j++) {
				*(tmp+(i)*lineByte+(j)) = (cplx_abs(*(data+i*lineByte+j)).x);
			}
		}
		double maxbit = findmax(tmp,w*h);
		double avgbit = average(tmp,w*h);
		for(i=0;i<w;i++) {
			for(j=0;j<h;j++) {
				*(bmp+i*lineByte+j) = 
					(unsigned char)((double)atan(*(tmp+i*lineByte+j)/(double)avgbit/(double)den)*2/PIE*255);
			}
		}
		free(tmp);
		return 0;
	}
}
