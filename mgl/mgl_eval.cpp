/* mgl_eval.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
//-----------------------------------------------------------------------------
#include <time.h>
#include <math.h>
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define	_USE_MATH_DEFINES
#include <float.h>
const unsigned long mgl_nan[2] = {0xffffffff, 0x7fffffff};
#define NANd	(*(double*)mgl_nan)
#define NANf	(*(float*)&(mgl_nan[1]))
#define NAN		NANf
#define isnan	_isnan
#define isfinite	_finite
#define copysignf	_copysign
#define chdir	_chdir
#endif

#ifndef NO_GSL
#include <gsl/gsl_sf.h>
#endif

#include "mgl/mgl_eval.h"
#include <stdio.h>
//-----------------------------------------------------------------------------
//	��������� ��� ������������� ���������
enum{
EQ_NUM=0,	// a variable substitution
EQ_RND,		// random number
EQ_A,		// numeric constant
// ���������� �������
EQ_LT,		// comparison x<y	!!! MUST BE FIRST 2-PLACE FUNCTION
EQ_GT,		// comparison x>y
EQ_EQ,		// comparison x=y
EQ_OR,		// comparison x|y
EQ_AND,		// comparison x&y
EQ_ADD,		// addition x+y
EQ_SUB,		// substraction x-y
EQ_MUL,		// multiplication x*y
EQ_DIV,		// division x/y
EQ_IPOW,	// power x^n for integer n
EQ_POW,		// power x^y
EQ_MOD,		// x modulo y
EQ_LOG,		// logarithm of x on base a, log_a(x) = ln(x)/ln(a)
// ���������� ����. �������
EQ_BESJ,	// regular cylindrical Bessel function of fractional order
EQ_BESY,	// irregular cylindrical Bessel function of fractional order
EQ_BESI,	// regular modified Bessel function of fractional order
EQ_BESK,	// irregular modified Bessel function of fractional order
EQ_ELE,		// elliptic integral E(\phi,k) = \int_0^\phi dt   \sqrt((1 - k^2 \sin^2(t)))
EQ_ELF,		// elliptic integral F(\phi,k) = \int_0^\phi dt 1/\sqrt((1 - k^2 \sin^2(t)))
EQ_LP,		// Legendre polynomial P_l(x), (|x|<=1, l>=0) !!! MUST BE LAST 2-PLACE FUNCTION
// ����������� �������
EQ_SIN,		// sine function \sin(x).
EQ_COS,		// cosine function \cos(x).
EQ_TAN,		// tangent function \tan(x).
EQ_ASIN,	// inverse sine function \sin(x).
EQ_ACOS,	// inverse cosine function \sin(x).
EQ_ATAN,	// inverse tangent function \tan(x).
EQ_SINH,	// hyperbolic sine function \sin(x).
EQ_COSH,	// hyperbolic cosine function \sin(x).
EQ_TANH,	// hyperbolic tangent function \tan(x).
EQ_ASINH,	// inverse hyperbolic sine function \sin(x).
EQ_ACOSH,	// inverse hyperbolic cosine function \sin(x).
EQ_ATANH,	// inverse hyperbolic tangent function \tan(x).
EQ_SQRT,	// square root function \sqrt(x)
EQ_EXP,		// exponential function \exp(x)
EQ_LN,		// logarithm of x, ln(x)
EQ_LG,		// decimal logarithm of x, lg(x) = ln(x)/ln(10)
EQ_SIGN,	// sign of number
EQ_STEP,	// step function
EQ_ABS,		// absolute value of x
// ����������� ���� �������
EQ_LI2,		// dilogarithm for a real argument Li2(x) = - \Re \int_0^x ds \log(1-s)/s.
EQ_ELLE,	// complete elliptic integral is denoted by E(k) = E(\pi/2, k).
EQ_ELLK,	// complete elliptic integral is denoted by K(k) = F(\pi/2, k).
EQ_AI,		// Airy function Ai(x)
EQ_BI,		// Airy function Bi(x)
EQ_ERF,		// error function erf(x) = (2/\sqrt(\pi)) \int_0^x dt \exp(-t^2).
EQ_EI3,		// exponential integral Ei_3(x) = \int_0^x dt \exp(-t^3) for x >= 0.
EQ_EI,		// exponential integral Ei(x),  Ei(x) := - PV(\int_{-x}^\infty dt \exp(-t)/t), where PV denotes the principal value of the integral.
EQ_E1,		// exponential integral E_1(x), E_1(x) := Re \int_1^\infty dt \exp(-xt)/t.
EQ_E2,		// exponential integral E_2(x), E_2(x) := Re \int_1^\infty dt \exp(-xt)/t^2.
EQ_SI,		// Sine integral Si(x) = \int_0^x dt \sin(t)/t.
EQ_CI,		// Cosine integral Ci(x) = \int_0^x dt \cos(t)/t.
EQ_GAMMA,	// Gamma function \Gamma(x) = \int_0^\infty dt  t^{x-1} \exp(-t)
EQ_PSI,		// digamma function \psi(x) = \Gamma'(x)/\Gamma(x) for general x, x \ne 0.
EQ_W0,		// principal branch of the Lambert W function, W_0(x). Functions W(x), are defined to be solutions of the equation W\exp(W) = x.
EQ_W1,		// secondary real-valued branch of the Lambert W function, W_{-1}(x). Functions W(x), are defined to be solutions of the equation W\exp(W) = x.
EQ_SINC,		// compute \sinc(x) = \sin(\pi x) / (\pi x) for any value of x.
EQ_ZETA,		// Riemann zeta function \zeta(s) = \sum_{k=1}^\infty k^{-s}for arbitrary s, s \ne 1.
EQ_ETA,		// eta function \eta(s) = (1-2^{1-s}) \zeta(s) for arbitrary s.
			// !!! MUST BE LAST 1-PLACE FUNCTION
// ������������� �������
EQ_SN,		// Jacobian elliptic function sn(u|m)
EQ_SC,		// Jacobian elliptic function sn(u|m)/cn(u|m)
EQ_SD,		// Jacobian elliptic function sn(u|m)/dn(u|m)
EQ_NS,		// Jacobian elliptic function 1/sn(u|m)
EQ_NC,		// Jacobian elliptic function 1/cn(u|m)
EQ_ND,		// Jacobian elliptic function 1/dn(u|m)
EQ_CN,		// Jacobian elliptic function cn(u|m)
EQ_CS,		// Jacobian elliptic function cn(u|m)/sn(u|m)
EQ_CD,		// Jacobian elliptic function cn(u|m)/dn(u|m)
EQ_DN,		// Jacobian elliptic function dn(u|m)
EQ_DS,		// Jacobian elliptic function dn(u|m)/sn(u|m)
EQ_DC,		// Jacobian elliptic function dn(u|m)/cn(u|m)
			// MUST BE LAST ELLIPTIC FUNCTION
// ������
EQ_EN,
EQ_CL,		// Clausen function
};
//-----------------------------------------------------------------------------
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+(IM1-1)/NTAB)
#define RNMX (1.0 - 1.2e-7)
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif
double mgl_rnd()
{
	static long idum=0;
	long j;
	long k;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];
	double temp;
	if(idum==0)
		idum = -(long)(time(NULL));
	if (idum <= 0) { 				// Initialize.
		if (-(idum) < 1) idum=1;	// Be sure to prevent idum = 0.
		else idum = -(idum);
		idum2=(idum);
		for (j=NTAB+7;j>=0;j--) {	// Load the shuffle table (after 8 warm-ups).
			k=(idum)/IQ1;
			idum=IA1*(idum-k*IQ1)-k*IR1;
			if (idum < 0) idum += IM1;
			if (j < NTAB) iv[j] = idum;
		}
		iy=iv[0];
	}
	k=(idum)/IQ1; 					// Start here when not initializing.
	idum=IA1*(idum-k*IQ1)-k*IR1;	// Compute idum=(IA1*idum) % IM1 without
									// over ows by Schrage's method.
	if (idum < 0) idum += IM1;
	k=idum2/IQ2;
	idum2=IA2*(idum2-k*IQ2)-k*IR2;	// Compute idum2=(IA2*idum) % IM2 likewise.
	if (idum2 < 0) idum2 += IM2;
	j=iy/NDIV; 						// Will be in the range 0..NTAB-1.
	iy=iv[j]-idum2;					// Here idum is shufted, idum and idum2 are
									// combined to generate output.
	iv[j] = idum;
	if (iy < 1) iy += IM1-1;
	if ((temp=AM*iy) > RNMX)	// Because users don't expect endpoint values.
		return RNMX;
	else return temp;
}
//-----------------------------------------------------------------------------
double ipow_mgl(double x,int n);
int mglFormula::Error=0;
float mglFormula::a1['z'-'a'+1];
bool mglCheck(char *str,int n);
int mglFindInText(char *str,const char *lst);
//-----------------------------------------------------------------------------
void strtrim_mgl(char *str)
{
	char *c = strdup(str);
	unsigned long n=strlen(str);
	long k;
	for(k=0;k<long(strlen(str));k++)	// ������� ��������� �������
		if(str[k]>' ')	break;
	strcpy(c,&(str[k]));
	n = strlen(c);
	for(k=n-1;k>=0;k--)	// ������� ��������� �������
		if(c[k]>' ')		break;
	c[k+1] = 0;
	strcpy(str,c);	free(c);
}
//-----------------------------------------------------------------------------
void strlwr_mgl(char *str)
{
	for(long k=0;k<(long)strlen(str);k++)	// ������� ��������� �������
		str[k] = (str[k]>='A' && str[k]<='Z') ? str[k]+'a'-'A' : str[k];
}
//-----------------------------------------------------------------------------
// ���������� �������
mglFormula::~mglFormula()
{
	if(Left) delete Left;
	if(Right) delete Right;
}
//-----------------------------------------------------------------------------
// ����������� ������� (������������� ���������� � "�����������" �������)
mglFormula::mglFormula(const char *string)
{
	Error=0;
	Left=Right=0;
	Res=0; Kod=0;
	if(!string)	{	Kod = EQ_NUM;	Res = 0;	return;	}
//printf("%s\n",string);	fflush(stdout);
	char *str = new char[strlen(string)+1];
	strcpy(str,string);
	static char Buf[2048];
	long n,len;
	strtrim_mgl(str);
	strlwr_mgl(str);
	len=strlen(str);
	if(str[0]==0) {	delete []str;	return;	}
	if(str[0]=='(' && mglCheck(&(str[1]),len-2))	// ���� ��� ��������� � ������, �� �������  ��
	{
		strcpy(Buf,str+1);
		len-=2;	Buf[len]=0;
		strcpy(str,Buf);
	}
	len=strlen(str);
	n=mglFindInText(str,"&|");				// ������� ��������� - ��������, ���������
	if(n>=0)
	{
		if(str[n]=='|') Kod=EQ_OR;	else Kod=EQ_AND;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"<>=");				// ������� ��������� - ��������, ���������
	if(n>=0)
	{
		if(str[n]=='<') Kod=EQ_LT;
		else if(str[n]=='>') Kod=EQ_GT;
		else Kod=EQ_EQ;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"+-");				// ������� ��������� - ��������, ���������
	if(n>=0)
	{
		if(str[n]=='+') Kod=EQ_ADD; else Kod=EQ_SUB;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"*/");				// ������� ��������� - ���������, �������
	if(n>=0)
	{
		if(str[n]=='*') Kod=EQ_MUL; else Kod=EQ_DIV;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}
	n=mglFindInText(str,"^");				// ������� ��������� - ���������� � �������
	if(n>=0)
	{
		Kod=EQ_IPOW;
		strcpy(Buf,str); Buf[n]=0;
		Left=new mglFormula(Buf);
		Right=new mglFormula(Buf+n+1);
		delete []str;
		return;
	}

	for(n=0;n<len;n++)	if(str[n]=='(')	break;
	if(n>=len)							// ��� ����� ��� ����������
	{
		Kod = EQ_NUM;
//		Left = Right = 0;
		if(str[1]==0 && str[0]>='a' && str[0]<='z')	// ��������� ���������
		{	Kod=EQ_A;	Res = str[0]-'a';	}
		else if(!strcmp(str,"rnd")) Kod=EQ_RND;
		else if(!strcmp(str,"pi")) Res=M_PI;
		else Res=atof(str);				// ��� �����
	}
	else
	{
		char name[128];
		strcpy(name,str);
//		strcpy(Buf,str);
		name[n]=0;
//		len-=n;
		memcpy(Buf,&(str[n+1]),len-n);
		len=strlen(Buf);
		Buf[--len]=0;
		if(!strcmp(name,"sin")) Kod=EQ_SIN;
		else if(!strcmp(name,"step")) Kod=EQ_STEP;
		else if(!strcmp(name,"sign")) Kod=EQ_SIGN;
		else if(!strcmp(name,"cos")) Kod=EQ_COS;
		else if(!strcmp(name,"tg")) Kod=EQ_TAN;
		else if(!strcmp(name,"tan")) Kod=EQ_TAN;
		else if(!strcmp(name,"asin")) Kod=EQ_ASIN;
		else if(!strcmp(name,"acos")) Kod=EQ_ACOS;
		else if(!strcmp(name,"atan")) Kod=EQ_ATAN;
		else if(!strcmp(name,"sinh")) Kod=EQ_SINH;
		else if(!strcmp(name,"cosh")) Kod=EQ_COSH;
		else if(!strcmp(name,"tanh")) Kod=EQ_TANH;
		else if(!strcmp(name,"sh")) Kod=EQ_SINH;
		else if(!strcmp(name,"ch")) Kod=EQ_COSH;
		else if(!strcmp(name,"th")) Kod=EQ_TANH;
		else if(!strcmp(name,"sqrt")) Kod=EQ_SQRT;
		else if(!strcmp(name,"log")) Kod=EQ_LOG;
		else if(!strcmp(name,"pow")) Kod=EQ_POW;
		else if(!strcmp(name,"exp")) Kod=EQ_EXP;
		else if(!strcmp(name,"lg")) Kod=EQ_LG;
		else if(!strcmp(name,"ln")) Kod=EQ_LN;
		else if(!strcmp(name,"mod")) Kod=EQ_MOD;
		else if(!strcmp(name,"erf")) Kod=EQ_ERF;
		else if(!strcmp(name,"abs")) Kod=EQ_ABS;
		else if(!strcmp(name,"en")) Kod=EQ_EN;
		else if(!strcmp(name,"ci")) Kod=EQ_CI;
		else if(!strcmp(name,"si")) Kod=EQ_SI;
		else if(!strcmp(name,"j")) Kod=EQ_BESJ;
		else if(!strcmp(name,"y")) Kod=EQ_BESY;
		else if(!strcmp(name,"i")) Kod=EQ_BESI;
		else if(!strcmp(name,"k")) Kod=EQ_BESK;
		else if(!strcmp(name,"ee")) Kod=EQ_ELLE;
		else if(!strcmp(name,"ek")) Kod=EQ_ELLK;
		else if(!strcmp(name,"e")) Kod=EQ_ELE;
		else if(!strcmp(name,"f")) Kod=EQ_ELF;
		else if(!strcmp(name,"sn")) Kod=EQ_SN;
		else if(!strcmp(name,"sc")) Kod=EQ_SC;
		else if(!strcmp(name,"sd")) Kod=EQ_SD;
		else if(!strcmp(name,"cn")) Kod=EQ_CN;
		else if(!strcmp(name,"cs")) Kod=EQ_CS;
		else if(!strcmp(name,"cd")) Kod=EQ_CD;
		else if(!strcmp(name,"dn")) Kod=EQ_DN;
		else if(!strcmp(name,"ds")) Kod=EQ_DS;
		else if(!strcmp(name,"dc")) Kod=EQ_DC;
		else if(!strcmp(name,"ns")) Kod=EQ_NS;
		else if(!strcmp(name,"nc")) Kod=EQ_NC;
		else if(!strcmp(name,"nd")) Kod=EQ_ND;
		else if(!strcmp(name,"gamma")) Kod=EQ_GAMMA;
		else if(!strcmp(name,"ai")) Kod=EQ_AI;
		else if(!strcmp(name,"bi")) Kod=EQ_BI;
		else if(!strcmp(name,"cl")) Kod=EQ_CL;
		else if(!strcmp(name,"dilog")) Kod=EQ_LI2;
		else if(!strcmp(name,"li2")) Kod=EQ_LI2;
		else if(!strcmp(name,"ei")) Kod=EQ_EI;
		else if(!strcmp(name,"e1")) Kod=EQ_E1;
		else if(!strcmp(name,"e2")) Kod=EQ_E2;
		else if(!strcmp(name,"w0")) Kod=EQ_W0;
		else if(!strcmp(name,"w1")) Kod=EQ_W1;
		else if(!strcmp(name,"legendre")) Kod=EQ_LP;
		else if(!strcmp(name,"psi")) Kod=EQ_PSI;
		else if(!strcmp(name,"sinc")) Kod=EQ_SINC;
		else if(!strcmp(name,"zeta")) Kod=EQ_ZETA;
		else if(!strcmp(name,"eta")) Kod=EQ_ETA;
		else if(!strcmp(name,"ei3")) Kod=EQ_EI3;
//		else if(!strcmp(name,"a")) Kod=EQ_A;
		else {	delete []str;	return;	}	// ����������� �������
		// � ��� �����
		n=mglFindInText(Buf,",");
		if(n>=0)
		{
			Buf[n]=0;
			Left=new mglFormula(Buf);
			Right=new mglFormula(&(Buf[n+1]));
		}
		else
			Left=new mglFormula(Buf);
	}
	delete []str;
}
//-----------------------------------------------------------------------------
// ���������� �������� �������
float mglFormula::Calc(float x,float y,float t,float u)
{
	Error=0;
	a1['a'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	return CalcIn();
}
//-----------------------------------------------------------------------------
// ���������� �������� �������
float mglFormula::Calc(float x,float y,float t,float u,float v,float w)
{
	Error=0;
	a1['c'-'a'] = a1['w'-'a'] = w;
	a1['b'-'a'] = a1['v'-'a'] = v;
	a1['a'-'a'] = a1['u'-'a'] = u;
	a1['x'-'a'] = a1['r'-'a'] = x;
	a1['y'-'a'] = a1['n'-'a'] = y;
	a1['z'-'a'] = a1['t'-'a'] = t;
	return CalcIn();
}
//-----------------------------------------------------------------------------
// ���������� �������� �������
float mglFormula::Calc(float var['z'-'a'+1])
{
	Error=0;
	for(long i=0;i<='z'-'a';i++)	a1[i] = var[i];
	return CalcIn();
}
//-----------------------------------------------------------------------------
double cand(double a,double b)	{return a&&b?1:0;}
double cor(double a,double b)	{return a||b?1:0;}
double ceq(double a,double b)	{return a==b?1:0;}
double clt(double a,double b)	{return a<b?1:0;}
double cgt(double a,double b)	{return a>b?1:0;}
double add(double a,double b)	{return a+b;}
double sub(double a,double b)	{return a-b;}
double mul(double a,double b)	{return a&&b?a*b:0;}
double div(double a,double b)	{return b?a/b:NAN;}
double ipw(double a,double b)	{return ipow_mgl(a,int(b));}
double llg(double a,double b)	{return log(a)/log(b);}
#ifndef NO_GSL
double gslEllE(double a,double b)	{return gsl_sf_ellint_E(a,b,GSL_PREC_SINGLE);}
double gslEllF(double a,double b)	{return gsl_sf_ellint_F(a,b,GSL_PREC_SINGLE);}
double gslLegP(double a,double b)	{return gsl_sf_legendre_Pl(int(a),b);}
double gslEllEc(double a)	{return gsl_sf_ellint_Ecomp(a,GSL_PREC_SINGLE);}
double gslEllFc(double a)	{return gsl_sf_ellint_Kcomp(a,GSL_PREC_SINGLE);}
double gslAi(double a)	{return gsl_sf_airy_Ai(a,GSL_PREC_SINGLE);}
double gslBi(double a)	{return gsl_sf_airy_Bi(a,GSL_PREC_SINGLE);}
#endif
double sgn(double a)	{return a<0 ? -1 : (a>0 ? 1:0);}
double stp(double a)	{return a<0 ? 0 : 1;}
double mgz1(double)	{return 0;}
double mgz2(double,double)	{return 0;}
#ifdef _MSC_VER
double asinh(double x)	{	return log(x+sqrt(x*x+1));	}
double acosh(double x)	{	return x>1 ? log(x+sqrt(x*x-1)) : NAN;	}
double atanh(double x)	{	return fabs(x)<1 ? log((1+x)/(1-x))/2 : NAN;	}
#endif
//-----------------------------------------------------------------------------
typedef double (*func_1)(double);
typedef double (*func_2)(double, double);
// ���������� �������� ��������� ���������
float mglFormula::CalcIn()
{
	func_2 f2[20] = {clt,cgt,ceq,cor,cand,add,sub,mul,div,ipw,pow,fmod,llg
#ifndef NO_GSL
			,gsl_sf_bessel_Jnu,gsl_sf_bessel_Ynu,
			gsl_sf_bessel_Inu,gsl_sf_bessel_Knu,
			gslEllE,gslEllF,gslLegP
#else
			,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2,mgz2
#endif
		};
	func_1 f1[38] = {sin,cos,tan,asin,acos,atan,sinh,cosh,tanh,
					asinh,acosh,atanh,sqrt,exp,log,log10,sgn,stp,fabs
#ifndef NO_GSL
			,gsl_sf_dilog,gslEllEc,gslEllFc,gslAi,gslBi,gsl_sf_erf,
			gsl_sf_expint_3,gsl_sf_expint_Ei,gsl_sf_expint_E1,gsl_sf_expint_E2,
			gsl_sf_Si,gsl_sf_Ci,gsl_sf_gamma,gsl_sf_psi,gsl_sf_lambert_W0,
			gsl_sf_lambert_Wm1,gsl_sf_sinc,gsl_sf_zeta,gsl_sf_eta
#else
			,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,
			mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1,mgz1
#endif
		};
	if(Error)	return 0;
	if(Kod==EQ_A)	return a1[(int)Res];
	if(Kod==EQ_RND)	return mgl_rnd();
	if(Kod==EQ_NUM) return Res;

	double a = Left->CalcIn();
	if(isnan(a))	return NAN;

	if(Kod<=EQ_LP)
	{
		double b = Right->CalcIn();
		if(isnan(b))	return NAN;
		if(Kod==EQ_POW && fabs(a)<=0)		{ Error=MGL_ERR_LOG; return NAN; }
		if(Kod==EQ_LOG && (a<=0 || b<=0))	{ Error=MGL_ERR_LOG; return NAN; }
		return f2[Kod-EQ_LT](a,b);
	}
	else if(Kod<=EQ_ETA)
	{
		if((Kod==EQ_ACOS || Kod==EQ_ASIN) && fabs(a)>1)	{ Error=MGL_ERR_ARC; return NAN; }
		if((Kod==EQ_LG || Kod==EQ_LN) && fabs(a)<=0)	{ Error=MGL_ERR_LOG; return NAN; }
		if(Kod==EQ_SQRT && a<0) { Error=MGL_ERR_SQRT; return NAN; }
		return f1[Kod-EQ_LP-1](a);
	}
#ifndef NO_GSL
	else if(Kod<=EQ_DC)
	{
		double sn=0,cn=0,dn=0,b = Right->CalcIn();
		if(isnan(b))	return NAN;
		gsl_sf_elljac_e(a,b, &sn, &cn, &dn);
		switch(Kod)
		{
		case EQ_SN:		return sn;
		case EQ_SC:		return sn/cn;
		case EQ_SD:		return sn/dn;
		case EQ_CN:		return cn;
		case EQ_CS:		return cn/sn;
		case EQ_CD:		return cn/dn;
		case EQ_DN:		return dn;
		case EQ_DS:		return dn/sn;
		case EQ_DC:		return dn/cn;
		case EQ_NS:		return 1./sn;
		case EQ_NC:		return 1./cn;
		case EQ_ND:		return 1./dn;
		}
	}
#endif
	return Res;
}
//-----------------------------------------------------------------------------
// �������� ������������ ������
bool mglCheck(char *str,int n)
{
	register long s = 0,i;
	for(i=0;i<n;i++)
	{
		if(str[i]=='(')	s++;
		if(str[i]==')') s--;
		if(s<0)	return false;
	}
	return (s==0) ? true : false;
}
//-----------------------------------------------------------------------------
// ����� ������ �� �������� lst � ������ str
int mglFindInText(char *str,const char *lst)
{
	register long l=0,r=0,i;//,j,len=strlen(lst);
	for(i=strlen(str)-1;i>=0;i--)
	{
		if(str[i]=='(') l++;
		if(str[i]==')') r++;
		if(l==r && strchr(lst,str[i]))	return i;
	}
	return -1;
}
//-----------------------------------------------------------------------------