#include <stdio.h>
#include "math.h"
#define PIE 3.1415926535898
#define i(x) cplx_imult(x)
#ifndef _EPS_
#define EPS 1e-10
#endif


//******************************************************************//
//																	//
//					cplxc.h定义了复数结构体							//
//																	//
//******************************************************************//

struct complex {
	double re,im;//复数的实部和虚部
	int NaN;//表示此数值是否有效的标志位
};

struct double_x {
	double x;//实数的数值
	int NaN;//表示此数值是否有效的标志位
};

//******************************************************************//
//																	//
//					cplxc.h定义了复数域常用的函数					//
//						具体函数见下面目录							//
//																	//
//******************************************************************//
/*******************************************************************\
	double_x cplx_abs ( complex a )				复数求模		|a|

	double_x cplx_agl ( complex a )				复数求辐角		Arg(a)

	complex cplx_imult( complex a )				复数乘以i		i*a

	complex cplx_conj ( complex a )				复数取共轭		conj(a)

	complex cplx_mult ( complex a , complex b )	复数相乘		a*b

	complex cplx_dvic ( complex a , complex b )	复数相除		a/b

	complex cplx_exp ( complex a )				复数域e的a次方	exp(a)

	complex cplx_log ( complex a )				复数域自然对数	Ln(a)

	complex cplx_sin (complex a)				复数正弦		sin(a)

	complex cplx_cos (complex a)				复数余弦		cos(a)

	complex cplx_tan (complex a)				复数正切		tan(a)

	complex cplx_add ( complex a , complex b )	复数相加		a+b

	complex cplx_sub ( complex a , complex b )	复数相减		a-b

	complex cplx_pow ( complex a , int n )		复数的整数次方	a^n

	void cplx_print ( complex a )				打印一个复数
\*******************************************************************/
/*>>>>>>>>>>>>>>>>>>>>>>>>>函数代码<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

double_x cplx_abs ( complex a ) {
	double_x z;
	z.x = sqrt(a.re*a.re + a.im*a.im);
	z.NaN = a.NaN;
	return z;
}
/////////////////////////////////////////////////////////////////////
double_x cplx_agl ( complex a ) {
	double_x z,t;
	t = cplx_abs(a);
	if(t.x == 0)
		z.NaN = 0;
	else {
		z.NaN = a.NaN;
		if(a.re > 0)
			z.x = atan(a.im/a.re);
		else if(a.re = 0 && a.im > 0)
			z.x = PIE/2;
		else if(a.re < 0 && a.im >0)
			z.x = atan(a.im/a.re)+PIE;
		else if(a.re < 0 && a.im <0)
			z.x = atan(a.im/a.re)-PIE;
		else if(a.re == 0 && a.im < 0)
			z.x = -PIE/2;
		else if(a.re < 0 && a.im == 0)
			z.x = PIE;
		else {
			z.x = 0;
			z.NaN = 0;
		}
	}
	return z;
}
////////////////////////////////////////////////////////////////////
complex cplx_imult(complex a) {
	complex z;
	z.re = -a.im;
	z.im = a.re;
	z.NaN = a.NaN;
	return z;
}
////////////////////////////////////////////////////////////////////
complex cplx_conj ( complex a ) {
	a.im = -a.im;
	return a;
}
////////////////////////////////////////////////////////////////////
complex cplx_mult ( complex a , complex b ) {
	complex z;
	z.re = a.re*b.re - a.im*b.im;
	z.im = a.re*b.im + a.im*b.re;
	z.NaN = a.NaN;
	return z;
}
////////////////////////////////////////////////////////////////////
complex cplx_dvic ( complex a , complex b ) {
	complex z={0,0,1};
	z.NaN = a.NaN*b.NaN;
	if(cplx_abs(b).x == 0)
		z.NaN = 0;
	else {
		z = cplx_mult(a,cplx_conj(b));
		z.im = z.im/(b.re*b.re + b.im*b.im);
		z.re = z.re/(b.re*b.re + b.im*b.im);
	}
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_exp ( complex a ) {
	complex z;
	z.re = exp(a.re)*cos(a.im);
	z.im = exp(a.re)*sin(a.im);
	z.NaN = a.NaN;
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_log ( complex a ) {
	complex z;
	if(a.im == 0 && a.re == 0) {
		z.im=0;
		z.re=0;
		z.NaN=1;
	}
	else {
		z.re = (double)log(cplx_abs(a).x);
		z.im = cplx_agl(a).x;
		z.NaN = a.NaN;
	}
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_add ( complex a , complex b ) {
	complex z;
	z.re = a.re + b.re;
	z.im = a.im + b.im;
	z.NaN = b.NaN*a.NaN;
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_sub ( complex a , complex b ) {
	complex z;
	z.re = a.re - b.re;
	z.im = a.im - b.im;
	z.NaN = b.NaN*a.NaN;
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_cos (complex a) {
	complex na,x,y,z;
	na.im = -a.im;
	na.re = -a.re;
	na.NaN = a.NaN;
	x = cplx_exp(i(a));
	y = cplx_exp(i(na));
	z = cplx_add(x,y);
	z.im = z.im/2.0;
	z.re = z.re/2.0;
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_sin (complex a) {
	complex na,x,y,z;
	na.im = -a.im;
	na.re = -a.re;
	na.NaN = a.NaN;
	x = cplx_exp(i(na));
	y = cplx_exp(i(a));
	z = cplx_sub(x,y);
	z = i(z);
	z.im = z.im/2.0;
	z.re = z.re/2.0;
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_tan (complex a) {
	complex z;
	z = cplx_dvic(cplx_sin(a),cplx_cos(a));
	return z;
}
///////////////////////////////////////////////////////////////////
complex cplx_pow ( complex a , int n ) {
	complex z,n_a;
	n_a.im = (double)n*(cplx_log(a).im);
	n_a.re = (double)n*(cplx_log(a).re);
	n_a.NaN = cplx_log(a).NaN;
	z = cplx_exp(n_a);
	return z;
}
///////////////////////////////////////////////////////////////////
void cplx_print ( complex a ) {
	if(a.im <= EPS && -a.im <= EPS) {
		a.im = 0;
	}
	if(a.re <= EPS && -a.re <= EPS) {
		a.re = 0;
	}
			//对小于EPS的数值进行截断
	if(a.NaN == 0)
		printf("\t NaN");
	else {		
		if(a.re==0) {
			if(a.im==0)
				printf("%12.4f",0);
			else if(a.im==1)
				printf("\t\t\t   i");
			else if(a.im==-1)
				printf("\t\t\t  -i");
			else
				printf("\t       %12.4fi",a.im);
		}
		else
			if(a.im==0)
				printf("%12.4f",a.re);
			else if(a.im>0 && a.im!=1)
				printf("%12.4f  +%12.4fi",a.re,a.im);
			else if(a.im<0 && a.im!=-1)
				printf("%12.4f  -%12.4fi",a.re,-a.im);
			else if(a.im ==1) 
				printf("%12.4f+i",a.re);
			else if(a.im == -1)
				printf("%12.4f-i",a.re);
			
			else
				printf("error!");
	}
}
///////////////////////////////////////////////////////////////////