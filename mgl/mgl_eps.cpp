/***************************************************************************
 * mgl_eps.cpp is part of Math Graphic Library
 * Copyright (C) 2007 Alexey Balakin <balakin@appl.sci-nnov.ru>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <stdlib.h>
#include <time.h>
#include "mgl/mgl_eps.h"
#include "mgl/mgl_c.h"
#include "mgl/mgl_f.h"
//-----------------------------------------------------------------------------
int mgl_compare_prim(const void *p1, const void *p2)
{
	mglPrim *a1=(mglPrim *)p1, *a2=(mglPrim *)p2;
	if( a1->z < a2->z-0.5 )	return -1;
	if( a1->z > a2->z+0.5 )	return 1;
	if( a1->w < a2->w )	return -1;
	if( a1->w > a2->w )	return 1;
	if( a1->style < a2->style )	return -1;
	if( a1->style > a2->style )	return 1;
	if( a2->type != a1->type )	return a2->type - a1->type;
	if( a1->type==1 && (a1->x[0]!=a2->x[0]) )
		return (a2->x[0]<a1->x[0]) ? 1 : -1;
	if( a1->type==1 )	return (a2->y[0]<a1->y[0]) ? 1 : -1;
	return 0;
}
//-----------------------------------------------------------------------------
mglGraphPS::mglGraphPS(int w,int h) : mglGraphAB(w,h)
{	P = 0;	pMax = pNum = 0;	}
//-----------------------------------------------------------------------------
mglGraphPS::~mglGraphPS()	{	if(P)	delete []P;	}
//-----------------------------------------------------------------------------
void mglGraphPS::Ball(mreal x,mreal y,mreal z,mglColor col,mreal alpha)
{
	if(alpha==0)	return;
	if(alpha<0)	{	alpha = -alpha;	}
	else		{	if(!ScalePoint(x,y,z))	return;	}
	if(!col.Valid())	col = mglColor(1.,0.,0.);
	mreal p[3] = {x,y,z};	PostScale(p,1);
	mglPrim a;		a.m = '.';
	a.x[0] = p[0];	a.y[0] = p[1];	a.z = a.zz[0]=p[2];
	a.c[0] = col.r;	a.c[1] = col.g;	a.c[2] = col.b;
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::ball(mreal *p,mreal *c)
{
	mglPrim a;	PostScale(p,1);		a.m = '.';
	a.x[0] = p[0];	a.y[0] = p[1];	a.z = a.zz[0]=p[2];
	memcpy(a.c,c,3*sizeof(mreal));
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::mark_plot(mreal *p, char type)
{
	mglPrim a;		a.m = type;		a.s = MarkSize;//*175*font_factor;
	a.x[0] = p[0];	a.y[0] = p[1];	a.z = a.zz[0]=p[2];
	memcpy(a.c,CDef,3*sizeof(mreal));
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::line_plot(mreal *p1,mreal *p2,mreal *c1,mreal *c2,bool all)
{
	if((PDef==0 && !all) || (fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01))	return;
	mglPrim a(1);
	mreal pw = fabs(PenWidth);
	a.z = (p1[2]+p2[2])/2;	a.w = pw;
	if(pw>1)	a.z += pw-1;
	a.x[0]=p1[0];	a.y[0]=p1[1];	a.x[1]=p2[0];	a.y[1]=p2[1];
	a.zz[0]=p1[2];	a.zz[1]=p2[2];
	a.c[0]=(c1[0]+c2[0])/2;	a.c[1]=(c1[1]+c2[1])/2;	a.c[2]=(c1[2]+c2[2])/2;
//	a.c[0]=c1[0];	a.c[1]=c1[1];	a.c[2]=c1[2];
	a.SetStyle(all? 0xffff:PDef,int(pPos));
	add_prim(a);
	pPos = fmod(pPos+hypot(p2[0]-p1[0], p2[1]-p1[1])/pw/1.5, 16);
}
//-----------------------------------------------------------------------------
void mglGraphPS::trig_plot(mreal *p3,mreal *p1,mreal *p2,mreal *c3,mreal *c1,mreal *c2)
{
	bool pnt = fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01 &&
				fabs(p1[0]-p3[0])<0.01 && fabs(p1[1]-p3[1])<0.01;
	if(pnt)	return;
	mglPrim a(2);
	a.z = (p1[2]+p2[2]+p3[2])/3;
	a.x[0]=p1[0];	a.y[0]=p1[1];	a.zz[0]=p1[2];
	a.x[1]=p2[0];	a.y[1]=p2[1];	a.zz[1]=p2[2];
	a.x[2]=p3[0];	a.y[2]=p3[1];	a.zz[2]=p3[2];

	mreal d1[3],d2[3];
	d1[0] = p1[0]-p3[0];	d2[0] = p2[0]-p3[0];	d1[1] = p1[1]-p3[1];
	d2[1] = p2[1]-p3[1];	d1[2] = p1[2]-p3[2];	d2[2] = p2[2]-p3[2];
	a.c[0]=(c1[0]+c2[0]+c3[0])/3;	a.c[1]=(c1[1]+c2[1]+c3[1])/3;
	a.c[2]=(c1[2]+c2[2]+c3[2])/3;	a.c[3]=(c1[3]+c2[3]+c3[3])/3;
	add_light(a.c, d2[2]*d1[1]-d2[1]*d1[2], d2[0]*d1[2]-d2[2]*d1[0], d2[1]*d1[0]-d2[0]*d1[1]);
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::trig_plot_n(mreal *p3,mreal *p1,mreal *p2,
				mreal *c3,mreal *c1,mreal *c2,mreal *n3,mreal *n1,mreal *n2)
{
	bool pnt = fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01 &&
				fabs(p1[0]-p3[0])<0.01 && fabs(p1[1]-p3[1])<0.01;
	if(pnt)	return;
	mglPrim a(2);
	a.z = (p1[2]+p2[2]+p3[2])/3;
	a.x[0]=p1[0];	a.y[0]=p1[1];	a.zz[0]=p1[2];
	a.x[1]=p2[0];	a.y[1]=p2[1];	a.zz[1]=p2[2];
	a.x[2]=p3[0];	a.y[2]=p3[1];	a.zz[2]=p3[2];
	a.c[0]=(c1[0]+c2[0]+c3[0])/3;	a.c[1]=(c1[1]+c2[1]+c3[1])/3;
	a.c[2]=(c1[2]+c2[2]+c3[2])/3;	a.c[3]=(c1[3]+c2[3]+c3[3])/3;
	add_light(a.c, (n1[0]+n2[0]+n3[0])/3, (n1[1]+n2[1]+n3[1])/3, (n1[2]+n2[2]+n3[2])/3);
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::quad_plot(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
				mreal *c0,mreal *c1,mreal *c2,mreal *c3)
{
	bool pnt = fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01 &&
				fabs(p1[0]-p3[0])<0.01 && fabs(p1[1]-p3[1])<0.01 &&
				fabs(p1[0]-p0[0])<0.01 && fabs(p1[1]-p0[1])<0.01;
	if(pnt)	return;
	mglPrim a(3);
	a.z = (p1[2]+p2[2]+p3[2]+p0[2])/4;
	a.x[0]=p0[0];	a.y[0]=p0[1];	a.zz[0]=p0[2];
	a.x[1]=p1[0];	a.y[1]=p1[1];	a.zz[1]=p1[2];
	a.x[2]=p2[0];	a.y[2]=p2[1];	a.zz[2]=p2[2];
	a.x[3]=p3[0];	a.y[3]=p3[1];	a.zz[3]=p3[2];
	mreal d1[3];
	d1[0]=-p0[2]*p1[1]+p0[1]*p1[2]+p0[2]*p2[1]-p0[1]*p2[2]-p1[2]*p3[1]+
			p2[2]*p3[1]+p1[1]*p3[2]-p2[1]*p3[2];
	d1[1]= p0[2]*p1[0]-p0[0]*p1[2]-p0[2]*p2[0]+p0[0]*p2[2]+
			p1[2]*p3[0]-p2[2]*p3[0]-p1[0]*p3[2]+p2[0]*p3[2];
	d1[2]=-p0[1]*p1[0]+p0[0]*p1[1]+p0[1]*p2[0]-p0[0]*p2[1]-p1[1]*p3[0]+
			p2[1]*p3[0]+p1[0]*p3[1]-p2[0]*p3[1];
	a.c[0]=(c1[0]+c2[0]+c3[0]+c0[0])/4;	a.c[1]=(c1[1]+c2[1]+c3[1]+c0[1])/4;
	a.c[2]=(c1[2]+c2[2]+c3[2]+c0[2])/4;	a.c[3]=(c1[3]+c2[3]+c3[3]+c0[3])/4;
	add_light(a.c, d1[0], d1[1], d1[2]);
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::quad_plot_n(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
				mreal *c0,mreal *c1,mreal *c2,mreal *c3,mreal *n0,mreal *n1,mreal *n2,mreal *n3)
{
	bool pnt = fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01 &&
				fabs(p1[0]-p3[0])<0.01 && fabs(p1[1]-p3[1])<0.01 &&
				fabs(p1[0]-p0[0])<0.01 && fabs(p1[1]-p0[1])<0.01;
	if(pnt)	return;
	mglPrim a(3);
	a.z = (p1[2]+p2[2]+p3[2]+p0[2])/4;
	a.x[0]=p0[0];	a.y[0]=p0[1];	a.zz[0]=p0[2];
	a.x[1]=p1[0];	a.y[1]=p1[1];	a.zz[1]=p1[2];
	a.x[2]=p2[0];	a.y[2]=p2[1];	a.zz[2]=p2[2];
	a.x[3]=p3[0];	a.y[3]=p3[1];	a.zz[3]=p3[2];
	a.c[0]=(c1[0]+c2[0]+c3[0]+c0[0])/4;	a.c[1]=(c1[1]+c2[1]+c3[1]+c0[1])/4;
	a.c[2]=(c1[2]+c2[2]+c3[2]+c0[2])/4;	a.c[3]=(c1[3]+c2[3]+c3[3]+c0[3])/4;
	add_light(a.c, n1[0]+n2[0]+n3[0]+n0[0], n1[1]+n2[1]+n3[1]+n0[1], n1[2]+n2[2]+n3[2]+n0[2]);
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::quad_plot_a(mreal *p0,mreal *p1,mreal *p2,mreal *p3,
				mreal a0,mreal a1,mreal a2,mreal a3, mreal alpha)
{
/*	bool pnt = fabs(p1[0]-p2[0])<0.01 && fabs(p1[1]-p2[1])<0.01 &&
				fabs(p1[0]-p3[0])<0.01 && fabs(p1[1]-p3[1])<0.01 &&
				fabs(p1[0]-p0[0])<0.01 && fabs(p1[1]-p0[1])<0.01;
	if(pnt)	return;*/
	mglPrim a(3);
	a.z = (p1[2]+p2[2]+p3[2]+p0[2])/4;
	a.x[0]=p0[0];	a.y[0]=p0[1];	a.zz[0]=p0[2];
	a.x[1]=p1[0];	a.y[1]=p1[1];	a.zz[1]=p1[2];
	a.x[2]=p2[0];	a.y[2]=p2[1];	a.zz[2]=p2[2];
	a.x[3]=p3[0];	a.y[3]=p3[1];	a.zz[3]=p3[2];
	mreal v = (a0+a1+a2+a3)/4;
	mglColor c(GetC(v,false));
	a.c[0]=c.r;		a.c[1]=c.g;		a.c[2]=c.b;
	a.c[3]=alpha>0 ? alpha*(v+1)*(v+1) : -alpha*(v-1)*(v-1);
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::Glyph(mreal x, mreal y, mreal f, int s, long j, char col)
{
	mglPrim a(4);
	a.s = fscl/PlotFactor;	a.w = ftet;
	a.x[0] = (xPos - zoomx1*Width) /zoomx2;
	a.y[0] = (yPos - zoomy1*Height)/zoomy2;
	a.x[1] = x;		a.y[1] = y;
	a.zz[1] = f/fnt->GetFact(s&3);
	a.style = s;	a.m = j;
	a.z = a.zz[0] = zPos;
	mglColor cc = mglColor(col);
	if(!cc.Valid())	cc = mglColor(CDef[0],CDef[1],CDef[2]);
	a.c[0] = cc.r;	a.c[1] = cc.g;	a.c[2] = cc.b;	a.c[3] = CDef[3];
	add_prim(a);
}
//-----------------------------------------------------------------------------
void mglGraphPS::Clf(mglColor  Back)
{
	Fog(0);
	pNum=0;
	pPos = 0;	CurrPal = -1;	PDef = 0xffff;
	memset(G,0,3*Width*Height);
	memset(G4,0,4*Width*Height);
	if(Back.Valid())
	{
		BDef[0] = (unsigned char)(255*Back.r);
		BDef[1] = (unsigned char)(255*Back.g);
		BDef[2] = (unsigned char)(255*Back.b);
		if(TranspType==2)	Back = mglColor(0,0,0);
	}
}
//-----------------------------------------------------------------------------
void mglGraphPS::add_prim(mglPrim &a)
{
	if(!P)
	{
		pMax = 1000;
		P = (mglPrim *)malloc(pMax*sizeof(mglPrim));
	}
	else if(pNum+1>pMax)
	{
		pMax += 1000;
		P = (mglPrim *)realloc(P, pMax*sizeof(mglPrim));
	}
	// fog
	mreal zz = FogDist*(a.z/Depth-0.5-FogDz);
	if(zz<0)
	{
		mreal d = 1-exp(5*zz);
		if(255*d>=254)	return;	// not visible under fog
		mreal cb[3] = {BDef[0]/255., BDef[1]/255., BDef[2]/255.}, b = 1-d;
		a.c[0] = a.c[0]*b + cb[0]*d;	a.c[1] = a.c[1]*b + cb[1]*d;
		a.c[2] = a.c[2]*b + cb[2]*d;	a.c[3] = a.c[3]*b + d;
	}

	a.c[0] = int(a.c[0]*100)*0.01;	a.c[1] = int(a.c[1]*100)*0.01;
	a.c[2] = int(a.c[2]*100)*0.01;	a.c[3] = UseAlpha ? int(a.c[3]*500)*0.002 : 1;
//	i(a->c[3]<0)	a->c[3] = 0;		if(a->c[3]>1)	a->c[3] = 1;
	a.x[0] = int(a.x[0]*100)*0.01;	a.x[1] = int(a.x[1]*100)*0.01;
	a.x[2] = int(a.x[2]*100)*0.01;	a.x[3] = int(a.x[3]*100)*0.01;
	a.y[0] = int(a.y[0]*100)*0.01;	a.y[1] = int(a.y[1]*100)*0.01;
	a.y[2] = int(a.y[2]*100)*0.01;	a.y[3] = int(a.y[3]*100)*0.01;
	a.id = ObjId;	P[pNum]=a;
	pNum++;		Finished = false;
}
//-----------------------------------------------------------------------------
void mglGraphPS::add_light(mreal *b, mreal n0,mreal n1, mreal n2)
{
	mreal n[3]={n0,n1,n2};
	unsigned char r[4];
	col2int(b,n,r);
	b[0]=r[0]/255.;	b[1]=r[1]/255.;	b[2]=r[2]/255.;	b[3]=r[3]/255.;
}
//-----------------------------------------------------------------------------
char *mgl_get_dash(unsigned short d, mreal w)
{
	static char s[64],b[4];
	if(d==0xffff)	{	strcpy(s,"");	return s;	}
	int f=0, p=d&1, n=p?0:1, i, j;
	strcpy(s, p ? "" : "0");
	for(i=0;i<16;i++)
	{
		j = i;//15-i;
		if(((d>>j)&1) == p)	f++;
		else
		{
			sprintf(b," %g",f*w);	strcat(s,b);
			p = (d>>j)&1;	f = 1;	n++;
		}
	}
	sprintf(b," %g",f*w);	strcat(s,b);
	strcat(s,n%2 ? "" : " 0");
	return s;
}
//-----------------------------------------------------------------------------
bool mglPrim::IsSame(mreal wp,mreal *cp,int st)
{
	if(abs(type)!=1)	return false;
	if(w>=1 && wp!=w)	return false;
	if(w<1 && wp!=1)	return false;
	if(st!=style)		return false;
	return (cp[0]==c[0] && cp[1]==c[1] && cp[2]==c[2]);
}
//-----------------------------------------------------------------------------
void mglPrim::SetStyle(unsigned PDef, int pPos)
{
	style=0;
	switch(PDef)
	{
	case 0xff00:	style=1;	break;
	case 0xf0f0:	style=2;	break;
	case 0x8888:	style=3;	break;
	case 0xfe10:	style=4;	break;
	case 0xe4e4:	style=5;	break;
	}
	pPos = abs(pPos)%16;
	dash = (PDef>>(16-pPos))+(PDef<<pPos);
}
//-----------------------------------------------------------------------------
void mglGraphPS::WriteEPS(const char *fname,const char *descr)
{
	if(!P)	return;
	if(!Finished)	Finish();
	time_t now;
	time(&now);

	FILE *fp = fopen(fname,"wt");
	if(!fp)		{	SetWarn(mglWarnOpen,fname);	return;	}
	fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n%%%%BoundingBox: 0 0 %d %d\n",Width,Height);
	fprintf(fp,"%%%%Creator: MathGL library\n%%%%Title: %s\n",descr ? descr : fname);
	fprintf(fp,"%%%%CreationDate: %s\n",ctime(&now));
	fprintf(fp,"/lw {setlinewidth} def\n/rgb {setrgbcolor} def\n");
	fprintf(fp,"/np {newpath} def\n/cp {closepath} def\n");
	fprintf(fp,"/ll {lineto} def\n/mt {moveto} def\n");
	fprintf(fp,"/rl {rlineto} def\n/rm {rmoveto} def\n/dr {stroke} def\n");
	fprintf(fp,"/ss {%g} def\n",MarkSize*0.4*font_factor);// remove *font_factor); ???
	fprintf(fp,"/s2 {%g} def\n",MarkSize*0.8*font_factor);// remove *font_factor); ???
	fprintf(fp,"/sm {-%g} def\n",MarkSize*0.4*font_factor);//remove *font_factor); ???
	fprintf(fp,"/m_c {ss 0.3 mul 0 360 arc} def\n");
	fprintf(fp,"/d0 {[] 0 setdash} def\n/sd {0 setdash} def\n");

	bool m_p=false,m_x=false,m_d=false,m_v=false,m_t=false,
		m_s=false,m_a=false,m_o=false,m_O=false,m_T=false,
		m_V=false,m_S=false,m_D=false,m_Y=false,m_l=false,
		m_L=false,m_r=false,m_R=false;
	register long i;
	// add mark definition if present
	for(i=0;i<pNum;i++)
	{
		if(P[i].type>0)	continue;		if(P[i].m=='+')	m_p = true;
		if(P[i].m=='x')	m_x = true;		if(P[i].m=='s')	m_s = true;
		if(P[i].m=='d')	m_d = true;		if(P[i].m=='v')	m_v = true;
		if(P[i].m=='^')	m_t = true;		if(P[i].m=='*')	m_a = true;
		if(P[i].m=='o')	m_o = true;		if(P[i].m=='O')	m_O = true;
		if(P[i].m=='S')	m_S = true;		if(P[i].m=='D')	m_D = true;
		if(P[i].m=='V')	m_V = true;		if(P[i].m=='T')	m_T = true;
		if(P[i].m=='<')	m_l = true;		if(P[i].m=='L')	m_L = true;
		if(P[i].m=='>')	m_r = true;		if(P[i].m=='R')	m_R = true;
		if(P[i].m=='Y')	m_Y = true;
	}
	if(m_p)	fprintf(fp,"/m_p {sm 0 rm s2 0 rl sm sm rm 0 s2 rl d0} def\n");
	if(m_x)	fprintf(fp,"/m_x {sm sm rm s2 s2 rl 0 sm 2 mul rm sm 2 mul s2 rl d0} def\n");
	if(m_s)	fprintf(fp,"/m_s {sm sm rm 0 s2 rl s2 0 rl 0 sm 2 mul rl cp d0} def\n");
	if(m_d)	fprintf(fp,"/m_d {sm 0 rm ss ss rl ss sm rl sm sm rl cp d0} def\n");
	if(m_v)	fprintf(fp,"/m_v {sm ss 2 div rm s2 0 rl sm sm 1.5 mul rl d0 cp} def\n");
	if(m_t)	fprintf(fp,"/m_t {sm sm 2 div rm s2 0 rl sm ss 1.5 mul rl d0 cp} def\n");
	if(m_a)	fprintf(fp,"/m_a {sm 0 rm s2 0 rl sm 1.6 mul sm 0.8 mul rm ss 1.2 mul ss 1.6 mul rl 0 sm 1.6 mul rm sm 1.2 mul ss 1.6 mul rl d0} def\n");
	if(m_o)	fprintf(fp,"/m_o {ss 0 360 d0 arc} def\n");
	if(m_O)	fprintf(fp,"/m_O {ss 0 360 d0 arc fill} def\n");
	if(m_S)	fprintf(fp,"/m_S {sm sm rm 0 s2 rl s2 0 rl 0 sm 2 mul rl cp} def\n");
	if(m_D)	fprintf(fp,"/m_D {sm 0 rm ss ss rl ss sm rl sm sm rl cp} def\n");
	if(m_V)	fprintf(fp,"/m_V {sm ss 2 div rm s2 0 rl sm sm 1.5 mul rl cp} def\n");
	if(m_T)	fprintf(fp,"/m_T {sm sm 2 div rm s2 0 rl sm ss 1.5 mul rl cp} def\n");

	if(m_Y)	fprintf(fp,"/m_Y {0 sm rm 0 0 rl sm 1.6 mul ss 0.8 mul rm 0 0 rl ss 1.6 mul ss 0.8 mul rm 0 0 rl d0} def\n");
	if(m_r)	fprintf(fp,"/m_r {ss 2 div sm rm 0 s2 rl sm sm 1.5 mul rl d0 cp} def\n");
	if(m_l)	fprintf(fp,"/m_l {sm 2 div sm rm 0 s2 rl ss sm 1.5 mul rl d0 cp} def\n");
	if(m_R)	fprintf(fp,"/m_R {ss 2 div sm rm 0 s2 rl sm sm 1.5 mul rl cp} def\n");
	if(m_L)	fprintf(fp,"/m_L {sm 2 div sm rm 0 s2 rl ss sm 1.5 mul rl cp} def\n");
	fprintf(fp,"\n");

	// write definition for all glyphs
	put_desc(fp,"/%c%c_%04x { np\n", "\t%d %d mt ", "%d %d ll ", "cp\n", "} def\n");
	// write primitives
	mreal cp[3]={-1,-1,-1},wp=-1;
	int st=0;
	char str[256]="";
	for(i=0;i<pNum;i++)
	{
		if(P[i].type<0)	continue;
		memcpy(cp,P[i].c,3*sizeof(mreal));
		if(P[i].type>1)	sprintf(str,"%.2g %.2g %.2g rgb ", P[i].c[0],P[i].c[1],P[i].c[2]);

		if(P[i].type==0)	// mark
		{
			sprintf(str,"1 lw %.2g %.2g %.2g rgb ", P[i].c[0],P[i].c[1],P[i].c[2]);
			wp=1;
			if(P[i].s!=MarkSize)
			{
				fprintf(fp,"/ss {%g} def\n",P[i].s*0.4*font_factor);
				fprintf(fp,"/s2 {%g} def\n",P[i].s*0.8*font_factor);
				fprintf(fp,"/sm {-%g} def\n",P[i].s*0.4*font_factor);
			}
			switch(P[i].m)
			{
			case '+':	fprintf(fp,"np %g %g mt m_p %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case 'x':	fprintf(fp,"np %g %g mt m_x %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case 's':	fprintf(fp,"np %g %g mt m_s %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case 'd':	fprintf(fp,"np %g %g mt m_d %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case '*':	fprintf(fp,"np %g %g mt m_a %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case 'v':	fprintf(fp,"np %g %g mt m_v %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case '^':	fprintf(fp,"np %g %g mt m_t %sdr\n",P[i].x[0],P[i].y[0],str);	break;
			case 'S':	fprintf(fp,"np %g %g mt m_S %sfill\n",P[i].x[0],P[i].y[0],str);	break;
			case 'D':	fprintf(fp,"np %g %g mt m_D %sfill\n",P[i].x[0],P[i].y[0],str);	break;
			case 'V':	fprintf(fp,"np %g %g mt m_V %sfill\n",P[i].x[0],P[i].y[0],str);	break;
			case 'T':	fprintf(fp,"np %g %g mt m_T %sfill\n",P[i].x[0],P[i].y[0],str);	break;
			case 'o':	fprintf(fp,"%g %g m_o %sdr\n",P[i].x[0],P[i].y[0],str);break;
			case 'O':	fprintf(fp,"%g %g m_O %sdr\n",P[i].x[0],P[i].y[0],str);break;
			default:	fprintf(fp,"%g %g m_c %sfill\n",P[i].x[0],P[i].y[0],str);
			}
			if(P[i].s!=MarkSize)
			{
				fprintf(fp,"/ss {%g} def\n",MarkSize*0.4*font_factor);
				fprintf(fp,"/s2 {%g} def\n",MarkSize*0.8*font_factor);
				fprintf(fp,"/sm {-%g} def\n",MarkSize*0.4*font_factor);
			}
		}
		else if(P[i].type==2)	// quad
			fprintf(fp,"np %g %g mt %g %g ll %g %g ll cp %sfill\n",P[i].x[0],P[i].y[0],P[i].x[1],P[i].y[1],P[i].x[2],P[i].y[2],str);
		else if(P[i].type==3)	// trig
			fprintf(fp,"np %g %g mt %g %g ll %g %g ll %g %g ll cp %sfill\n",P[i].x[0],P[i].y[0],P[i].x[1],P[i].y[1],P[i].x[3],P[i].y[3],P[i].x[2],P[i].y[2],str);
		else if(P[i].type==1)	// line
		{
			sprintf(str,"%.2g lw %.2g %.2g %.2g rgb ", P[i].w>1 ? P[i].w:1., P[i].c[0],P[i].c[1],P[i].c[2]);
			wp = P[i].w>1  ? P[i].w:1;	st = P[i].style;
			put_line(fp,i,wp,cp,st, "np %g %g mt ", "%g %g ll ", false);
			const char *sd = mgl_get_dash(P[i].dash,P[i].w);
			if(sd && sd[0])	fprintf(fp,"%s [%s] sd dr\n",str,sd);
			else			fprintf(fp,"%s d0 dr\n",str);
		}
		else if(P[i].type==4)	// glyph
		{
			mreal ss = P[i].s/2, xx = P[i].x[1], yy = P[i].y[1];
			fprintf(fp,"gsave\t%g %g translate %g %g scale %g rotate %s\n", 
				P[i].x[0], P[i].y[0], ss, ss, -P[i].w, str);
			if(P[i].style&8)	// this is "line"
			{
				mreal dy = 0.004,f=fabs(P[i].zz[1]);
				fprintf(fp,"np %g %g mt %g %g ll %g %g ll %g %g ll cp ",
					xx,yy+dy, xx+f,yy+dy, xx+f,yy-dy, xx,yy-dy);
			}
			else
				fprintf(fp,"%.3g %.3g translate %g %g scale %c%c_%04x ", 
					xx, yy, P[i].zz[1], P[i].zz[1], P[i].style&1?'b':'n',
					P[i].style&2?'i':'n', P[i].m);
			if(P[i].style&4)	fprintf(fp,"dr");
			else	fprintf(fp,"eofill");
			fprintf(fp," grestore\n");
		}
	}
	for(i=0;i<pNum;i++)		if(P[i].type==-1)	P[i].type = 1;
	fprintf(fp,"\nshowpage\n%%%%EOF\n");
	fclose(fp);
}
//-----------------------------------------------------------------------------
void mglGraphPS::WriteSVG(const char *fname,const char *descr)
{
	if(!P)	return;
	if(!Finished)	Finish();
	time_t now;
	time(&now);

	FILE *fp = fopen(fname,"wt");
	if(!fp)		{	SetWarn(mglWarnOpen,fname);	return;	}
	fprintf(fp,"<?xml version=\"1.0\" standalone=\"no\"?>\n");
	fprintf(fp,"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 20000303 Stylable//EN\" \"http://www.w3.org/TR/2000/03/WD-SVG-20000303/DTD/svg-20000303-stylable.dtd\">\n");
	fprintf(fp,"<svg width=\"%d\" height=\"%d\">\n",Width,Height);

	fprintf(fp,"<!--Creator: MathGL library-->\n");
	fprintf(fp,"<!--Title: %s-->\n<!--CreationDate: %s-->\n\n",descr?descr:fname,ctime(&now));

	// write definition for all glyphs
	put_desc(fp,"<symbol id=\"%c%c_%04x\"><path d=\"", "\tM %d %d ",
		"L %d %d ", "Z\n", "\"/></symbol>\n");
	// currentColor -> inherit ???
	fprintf(fp,"<g fill=\"none\" stroke=\"none\" stroke-width=\"0.5\">\n");
	// write primitives
	mreal cp[3]={-1,-1,-1},wp=-1;
	register long i;
	int st=0;

	for(i=0;i<pNum;i++)
	{
		if(P[i].type==0)
		{
			mreal x=P[i].x[0],y=Height-P[i].y[0],s=0.4*font_factor*P[i].s;
			wp = 1;
			if(strchr("SDVT",P[i].m))
				fprintf(fp,"<g fill=\"#%02x%02x%02x\">\n",
					int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
			else
				fprintf(fp,"<g stroke=\"#%02x%02x%02x\">\n",
					int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
			switch(P[i].m)
			{
			case '+':
				fprintf(fp,"<path d=\"M %g %g L %g %g M %g %g L %g %g\"/>\n",
						x-s,y,x+s,y,x,y-s,x,y+s);	break;
			case 'x':
				fprintf(fp,"<path d=\"M %g %g L %g %g M %g %g L %g %g\"/>\n",
						x-s,y-s,x+s,y+s,x+s,y-s,x-s,y+s);	break;
			case 's':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y-s,x+s,y-s,x+s,y+s,x-s,y+s);	break;
			case 'd':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y,x,y-s,x+s,y,x,y+s);	break;
			case '^':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y+s/2,x+s,y+s/2,x,y-s);	break;
			case 'v':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y-s/2,x+s,y-s/2,x,y+s);	break;
			case 'S':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y-s,x+s,y-s,x+s,y+s,x-s,y+s);	break;
			case 'D':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y,x,y-s,x+s,y,x,y+s);	break;
			case 'T':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y+s/2,x+s,y+s/2,x,y-s);	break;
			case 'V':
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %gZ\"/>\n",
						x-s,y-s/2,x+s,y-s/2,x,y+s);	break;
			case 'o':
				fprintf(fp,"<circle cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
						x,y,s);	break;
			case 'O':
				fprintf(fp,"<circle style=\"fill:#%02x%02x%02x\" cx=\"%g\" cy=\"%g\" r=\"%g\"/>\n",
						int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]),x,y,s);	break;
			case '*':
				fprintf(fp,"<path d=\"M %g %g L %g %g M %g %g L %g %g M %g %g L %g %g\"/>\n",
						x-s,y,x+s,y,x-0.8*s,y-1.6*s,x+0.8*s,y+1.6*s,x+0.8*s,y-1.6*s,x-0.8*s,y+1.6*s);	break;
			default:
				fprintf(fp,"<circle style=\"fill:#%02x%02x%02x\" cx=\"%g\" cy=\"%g\" r=\"0.15\"/>\n",
						int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]),x,y);	break;
			}
			fprintf(fp,"</g>\n");
		}
		else if(P[i].type==2 && P[i].c[3]>0)
		{
			fprintf(fp,"<g fill=\"#%02x%02x%02x\" opacity=\"%g\">\n",
				int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]),P[i].c[3]);
			fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g Z\"/> </g>\n",
					P[i].x[0],Height-P[i].y[0],P[i].x[1],Height-P[i].y[1],
					P[i].x[2],Height-P[i].y[2]);
		}
		else if(P[i].type==3 && P[i].c[3]>0)
		{
			fprintf(fp,"<g fill=\"#%02x%02x%02x\" opacity=\"%g\">\n",
				int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]),P[i].c[3]);
			fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %g Z\"/> </g>\n",
					P[i].x[0],Height-P[i].y[0],P[i].x[1],Height-P[i].y[1],
					P[i].x[3],Height-P[i].y[3],P[i].x[2],Height-P[i].y[2]);
		}
		else if(P[i].type==1)
		{
//			const char *dash[]={"", "8 8","4 4","1 3","7 4 1 4","3 2 1 2"};
			fprintf(fp,"<g stroke=\"#%02x%02x%02x\"",
					int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
			if(P[i].style)	fprintf(fp," stroke-dasharray=\"%s\"", mgl_get_dash(P[i].dash,P[i].w));
			if(P[i].w>1)	fprintf(fp," stroke-width=\"%g\"", P[i].w);
			memcpy(cp,P[i].c,3*sizeof(mreal));
			wp = P[i].w>1  ? P[i].w:1;	st = P[i].style;
			put_line(fp,i,wp,cp,st, "><path d=\" M %g %g", " L %g %g", true);
			fprintf(fp,"\"/> </g>\n");
		}
		else if(P[i].type==4)
		{
			mreal ss = P[i].s/2, xx = P[i].x[1], yy = P[i].y[1];
			if(P[i].style&8)	// this is "line"
			{
				fprintf(fp,"<g transform=\"translate(%g,%g) scale(%.3g,%.3g) rotate(%g)\"",
					P[i].x[0], Height-P[i].y[0], ss, -ss, -P[i].w);
				if(P[i].style&4)
					fprintf(fp," stroke=\"#%02x%02x%02x\">", int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
				else
					fprintf(fp," fill=\"#%02x%02x%02x\">", int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
				mreal dy = 0.004,f=fabs(P[i].zz[1]);
				fprintf(fp,"<path d=\"M %g %g L %g %g L %g %g L %g %g\"/></g>\n",
					xx,yy+dy, xx+f,yy+dy, xx+f,yy-dy, xx,yy-dy);
			}
			else
			{
				ss *= P[i].zz[1];
				fprintf(fp,"<g transform=\"translate(%g,%g) scale(%.3g,%.3g) rotate(%g)\"",
					P[i].x[0], Height-P[i].y[0], ss, -ss, -P[i].w);
				if(P[i].style&4)
					fprintf(fp," stroke=\"#%02x%02x%02x\">", int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
				else
					fprintf(fp," fill=\"#%02x%02x%02x\">", int(255*P[i].c[0]),int(255*P[i].c[1]),int(255*P[i].c[2]));
				fprintf(fp,"<use x=\"%g\" y=\"%g\" xlink:href=\"#%c%c_%04x\"/></g>\n", 
					xx/P[i].zz[1], yy/P[i].zz[1], P[i].style&1?'b':'n',
					P[i].style&2?'i':'n', P[i].m);
			}
		}
	}

	for(i=0;i<pNum;i++)		if(P[i].type==-1)	P[i].type = 1;
	fprintf(fp,"</g></svg>");
	fclose(fp);
}
//-----------------------------------------------------------------------------
void mglGraphPS::Finish()
{
	register long i;
	if(P && pNum>0)
	{
		qsort(P,pNum,sizeof(mglPrim),mgl_compare_prim);
		for(i=0;i<pNum;i++)	P[i].Draw(this);
	}
	unsigned char w[4];
	for(i=0;i<Width*Height;i++)
	{
		w[0]=w[1]=w[2]=w[3]=255;
		combine(w,G4+4*i);	memcpy(G+3*i,w,3);
	}
	Finished = true;
}
//-----------------------------------------------------------------------------
#define imax(a,b)	(a)>(b) ? (a) : (b)
#define imin(a,b)	(a)<(b) ? (a) : (b)
void mglPrim::Draw(mglGraphPS *gr)
{
	mreal pp[12]={x[0],y[0],z,	x[1],y[1],z,	x[2],y[2],z,	x[3],y[3],z};
	gr->draw_prim(this, pp, c);
}
//-----------------------------------------------------------------------------
void mglGraphPS::draw_prim(mglPrim *pr, mreal *pp, mreal *c)
{
	ObjId = pr->id;	memcpy(CDef,c,4*sizeof(mreal));
	bool ul=UseLight;	UseLight=false;
	int m = pr->m, s = pr->style;
	switch(pr->type)
	{
	case 0:	mglGraphAB::mark_plot(pp,pr->m);	break;
	case 1:	mglGraphAB::line_plot(pp,pp+3,c,c);	break;
	case 2:	mglGraphAB::trig_plot(pp,pp+3,pp+6,c,c,c);	break;
	case 3:	mglGraphAB::quad_plot(pp,pp+3,pp+6,pp+9,c,c,c,c);	break;
	case 4:
		mreal pf=PlotFactor;	Push();
		SetPosScale(pp[0],pp[1],pp[2],pr->s*PlotFactor);	RotateN(pr->w,0,0,1);
		mglGraphAB::Glyph(pr->x[1],pr->y[1],pr->zz[1]*fnt->GetFact(s&3),s,m,0);
		Pop();	PlotFactor=pf;
		break;
	}
	UseLight=ul;
}
//-----------------------------------------------------------------------------
void mglGraphPS::pnt_plot(long x,long y, mreal, unsigned char c[4])
{
	long i0=x+Width*(Height-1-y);
	if(x<0 || x>=Width || y<0 || y>=Height)	return;
	combine(G4+4*i0,c);	OI[i0]=ObjId;
}
//-----------------------------------------------------------------------------
void mglGraphPS::put_line(FILE *fp, long i, mreal wp, mreal *cp,int st, const char *ifmt, const char *nfmt, bool neg)
{
	long k = i,j;	// first point
	mreal x0=P[i].x[0], y0=P[i].y[0];
	bool ok=true;
	while(ok)
	{
		for(ok=false,j=i+1;j<pNum && P[j].type<2;j++)
			if(P[j].IsSame(wp,cp,st) && P[j].type==1)
			{	// previous point
				if(P[j].x[1]==x0 && P[j].y[1]==y0)
				{
					k=j;	ok=true;	P[k].type = -2;
					x0 = P[k].x[0];	y0=P[k].y[0];
				}
				else if(P[j].x[0]==x0 && P[j].y[0]==y0)
				{
					k=j;	ok=true;	P[k].type = -2;
					x0 = P[k].x[1];	y0=P[k].y[1];
				}
			}
	}
	for(j=i;j<pNum ;j++)	if(P[j].type==-2)	P[j].type = 1;
	fprintf(fp,ifmt,x0,neg?Height-y0:y0);	ok=true;
	long m=1;
	while(ok)
	{
		for(ok=false,j=i;j<pNum && P[j].type<2;j++)
			if(P[j].IsSame(wp,cp,st) && P[j].type==1)
			{
				if(P[j].x[0]==x0 && P[j].y[0]==y0)
				{
					k=j;	P[k].type = -1;
					x0 = P[k].x[1];	y0=P[k].y[1];
					fprintf(fp,nfmt,x0,neg?Height-y0:y0);
					if(m>10)	{	m=0;	fprintf(fp,"\n");	}
					ok=true;	m++;
				}
				else if(P[j].x[1]==x0 && P[j].y[1]==y0)
				{
					k=j;	P[k].type = -1;
					x0 = P[k].x[0];	y0=P[k].y[0];
					fprintf(fp,nfmt,x0,neg?Height-y0:y0);
					if(m>10)	{	m=0;	fprintf(fp,"\n");	}
					ok=true;	m++;
				}
			}
	}
}
//-----------------------------------------------------------------------------
//put_desc(fp,"%c%c%c_%04x {", "np %d %d mt %d %d ll %d %d ll cp fill\n", 
//"np %d %d mt ", "%d %d ll ", "cp dr\n", "} def")
void mglGraphPS::put_desc(FILE *fp, const char *pre, const char *ln1, const char *ln2, const char *ln3, const char *suf)
{
	register long i,j,n;
	wchar_t *g;
	int *s;
	for(n=i=0;i<pNum;i++)	if(P[i].type==4)	n++;
	if(n==0)	return;		// no glyphs
	g = new wchar_t[n];	s = new int[n];
	for(n=i=0;i<pNum;i++)
	{
		if(P[i].type!=4 || (P[i].style&8))	continue;	// not a glyph
		bool is=false;
		for(j=0;j<n;j++)	if(g[j]==P[i].m && s[j]==(P[i].style&7))	is = true;
		if(is)	continue;		// glyph is described
		// have to describe
		g[n]=P[i].m;	s[n]=P[i].style&7;	n++;	// add to list of described
		// "%c%c%c_%04x {"
		fprintf(fp,pre, P[i].style&1?'b':'n', P[i].style&2?'i':'n', P[i].m);
		long ik,ii;
		int nl=fnt->GetNl(P[i].style&3,P[i].m);
		const short *ln=fnt->GetLn(P[i].style&3,P[i].m);
		bool np=true;
		if(ln && nl>0)	for(ik=0;ik<nl;ik++)
		{
			ii = 2*ik;
			if(ln[ii]==0x3fff && ln[ii+1]==0x3fff)	// line breakthrough
			{	fprintf(fp,"%s",ln3);	np=true;	continue;	}
			else if(np)	fprintf(fp,ln1,ln[ii],ln[ii+1]);
			else		fprintf(fp,ln2,ln[ii],ln[ii+1]);
			np=false;
		}
		fprintf(fp,"%s%s",ln3,suf);	// finish glyph description suf="} def"
	}
	delete []g;		delete []s;
}
//-----------------------------------------------------------------------------
/// Create mglGraph object in PostScript mode.
HMGL mgl_create_graph_ps(int width, int height)
{    return new mglGraphPS(width,height);	}
/// Create mglGraph object in PostScript mode.
uintptr_t mgl_create_graph_ps_(int *width, int *height)
{    return uintptr_t(new mglGraphPS(*width,*height));	}
//-----------------------------------------------------------------------------
