/***************************************************************************
 * axis.cpp is part of Math Graphic Library
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
#include <time.h>
#include <wchar.h>
#include <ctype.h>
#include <stdlib.h>
#include "mgl/data.h"
#include "mgl/canvas.h"
//-----------------------------------------------------------------------------
#define islog(a, b) (((a)>0 && (b)>10*(a)) || ((b)<0 && (a)<10*(b)))
#define sign(a)	((a)<0 ? -1:1)

//-----------------------------------------------------------------------------
wchar_t *mgl_wcsdup(const wchar_t *s)
{
	wchar_t *r = (wchar_t *)malloc((wcslen(s)+1)*sizeof(wchar_t));
	memcpy(r,s,(wcslen(s)+1)*sizeof(wchar_t));
	return r;
}
//-----------------------------------------------------------------------------
void wcstrim_mgl(wchar_t *str)
{
	wchar_t *c = mgl_wcsdup(str);
	unsigned long n=wcslen(str);
	long k;
	for(k=0;k<long(wcslen(str));k++)	if(!isspace(str[k]))	break;
	wcscpy(c,&(str[k]));
	n = wcslen(c);
	for(k=n-1;k>=0;k--)	if(!isspace(c[k]))	break;
	c[k+1] = 0;
	wcscpy(str,c);	free(c);
}
//-----------------------------------------------------------------------------
mglAxis::mglAxis()
{	memset(this,0,sizeof(mglAxis));	str[0] = buf = new wchar_t[4096];	}
//-----------------------------------------------------------------------------
void mglAxis::AddLabel(float v, const wchar_t *b)
{
	wcscpy(str[num],b);		val[num] = v;	num++;
	if(num<64)	str[num] = str[num-1]+wcslen(b)+1;
}
//-----------------------------------------------------------------------------
//		Ticks setup
//-----------------------------------------------------------------------------
void mglCanvas::SetAxisStl(const char *stl, const char *tck, const char *sub)
{
	if(!stl || !(*stl))		strcpy(AxisStl,"k");
	else if(strlen(stl)<32)	strcpy(AxisStl,stl);
	if(!tck || !(*tck))		strcpy(TickStl,AxisStl);
	else if(strlen(tck)<32)	strcpy(TickStl,tck);
	if(!sub || !(*sub))		strcpy(SubTStl,AxisStl);
	else if(strlen(sub)<32)	strcpy(SubTStl,sub);
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTickLen(float tlen, float stt)
{	TickLen=fabs(tlen)>0?tlen:0.1;	st_t=stt>0?stt:1;	}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicks(char dir, float d, int ns, float org)
{
	if(dir=='x')
	{	ax.d=d;	ax.f=0;	ax.ns=ns;	ax.o=org;	ax.num=0;	ax.upd=false;	}
	else if(dir=='y')
	{	ay.d=d;	ay.f=0;	ay.ns=ns;	ay.o=org;	ay.num=0;	ay.upd=false;	}
	else if(dir=='z')
	{	az.d=d;	az.f=0;	az.ns=ns;	az.o=org;	az.num=0;	az.upd=false;	}
	else if(dir=='c' || dir=='a')
	{	ac.d=d;	ac.f=0;	ac.ns=ns;	ac.o=org;	ac.num=0;	ac.upd=false;	}
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, HCDT v, const char *lbl)
{
	long ll=strlen(lbl);
	wchar_t *wcs = new wchar_t[ll+1];
	mbstowcs(wcs,lbl,ll);	wcs[ll]=0;
	SetTicksVal(dir,v,wcs);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, HCDT v, const wchar_t *lbl)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	if(!v || !lbl || !lbl[0])	{	aa.f = aa.num = 0;	return;	}

	long len=wcslen(lbl), n=v->GetNx();
	if(n>64)	n=64;
	aa.num = n;	len = 0;	aa.f = 2;	aa.ns=0;
	aa.val[0] = v->v(0);	aa.str[0] = aa.buf;
	register int i,j;
	for(i=0,j=1;i<n && j<len;j++)
	{
		if(aa.buf[j]=='\n')
		{
			aa.buf[j]=0;	aa.str[i]=aa.buf+j+1;
			aa.val[i]=v->v(i);	i++;
		}
		if(j>1 && aa.buf[j]=='n' && aa.buf[j-1]=='\\')
		{
			aa.buf[j]=aa.buf[j-1]=0;	aa.str[i]=aa.buf+j+1;
			aa.val[i]=v->v(i);	i++;
		}
		if(i>63)	break;
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, const char *lbl)
{
	register long i,j,len=strlen(lbl);
	for(i=0,j=1;j<len;j++)
		if(lbl[j]=='\n' || (lbl[j]=='n' && lbl[j-1]=='\\'))	i++;
	if(i>63)	i=63;
	mglData val(i+1);	val.Fill(Min.x,Max.x);
	SetTicksVal(dir, &val, lbl);
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, const wchar_t *lbl)
{
	register long i,j,len=wcslen(lbl);
	for(i=0,j=1;j<len;j++)
		if(lbl[j]=='\n' || (lbl[j]=='n' && lbl[j-1]=='\\'))	i++;
	if(i>63)	i=63;
	mglData val(i+1);	val.Fill(Min.x,Max.x);
	SetTicksVal(dir, &val, lbl);
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, HCDT v, const wchar_t **lbl)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	if(!v || !lbl)	{	aa.f = aa.num=0;	return;	}
	long len=0, n=v->GetNx();
	if(n>50)	n=50;
	register int i;
	for(i=0;i<n;i++)	len += wcslen(lbl[i])+1;
	aa.num = n;	len = 0;	aa.f = 2;	aa.ns=0;
	for(i=0;i<n;i++)
	{
		wcscpy(aa.buf+len, lbl[i]);	aa.val[i] = v->v(i);
		aa.str[i] = aa.buf+len;		len += wcslen(lbl[i])+1;
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTicksVal(char dir, HCDT v, const char **lbl)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	if(!v || !lbl)	{	aa.f = aa.num = 0;	return;	}
	long len=0, n=v->GetNx();
	if(n>50)	n=50;
	register int i,ll;
	for(i=0;i<n;i++)	len += strlen(lbl[i])+1;
	aa.num = n;	len = 0;	aa.f = 2;	aa.ns=0;
	for(i=0;i<n;i++)
	{
		ll = strlen(lbl[i])+1;	mbstowcs(aa.buf+len,lbl[i],ll);
		aa.val[i] = v->v(i);	aa.str[i] = aa.buf+len;		len += ll;
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTickTempl(char dir, const wchar_t *t)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	if(aa.f==1)	aa.f = 0;	// remove time ticks
	if(wcslen(t)<255) wcscpy(aa.t,t);
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTickTempl(char dir, const char *t)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	if(aa.f==1)	aa.f = 0;	// remove time ticks
	if(strlen(t)<255) mbstowcs(aa.t,t,strlen(t)+1);
}
//-----------------------------------------------------------------------------
void mglCanvas::SetTickTime(char dir, float d, const char *t)
{
	mglAxis &aa=ax;
	if(dir=='c' || dir=='a')	aa = ac;
	else if(dir=='x')	aa = ax;
	else if(dir=='y')	aa = ay;
	else if(dir=='z')	aa = az;
	else return;
	aa.upd = false;

	aa.dv = d;	aa.f = 1;
	if(strlen(t)<255) mbstowcs(aa.t,t,strlen(t)+1);
}
//-----------------------------------------------------------------------------
void mglCanvas::AdjustTicks(const char *dir, bool force)
{
	UpdateAxis();	TuneTicks = true;
	if(strchr(dir,'x'))
	{	if(force)	{	ax.d=0;	ax.upd=false;	}
		AdjustTicks(ax,fx);	}
	if(strchr(dir,'y'))
	{	if(force)	{	ay.d=0;	ay.upd=false;	}
		AdjustTicks(ay,fy);	}
	if(strchr(dir,'z'))
	{	if(force)	{	az.d=0;	az.upd=false;	}
		AdjustTicks(az,fz);	}
	if(strchr(dir,'a') || strchr(dir,'c'))
	{	if(force)	{	ac.d=0;	ac.upd=false;	}
		AdjustTicks(ac,fa);	}
}
//-----------------------------------------------------------------------------
void mglCanvas::AdjustTicks(mglAxis &aa, bool ff)
{
	float d = fabs(aa.v2-aa.v1), n;
	if(aa.f>0 || aa.upd)	return;
	if(ff && islog(aa.v1,aa.v2))
	{	aa.dv = 0;	aa.ds=1;	}
	else if(aa.d>0)
	{	aa.ds = aa.d/(aa.ns+1);	}
	else if(aa.d>-1.5)	// like =0 or =-1
	{
		n = floor(log10(d));	d = floor(d*pow(10,-n));
		n = pow(10,n);			aa.o=0;
		if(d==1)	{	aa.dv = n/5;	aa.ds=n/10;	}
		else if(d<4){	aa.dv = n/2;	aa.ds=n/10;	}
		else if(d<7){	aa.dv = n;		aa.ds=n/5;	}
		else		{	aa.dv = 2*n;	aa.ds=n/2;	}
	}
	else	// TODO: Check it!!!
	{
		d /= -aa.d;		n = floor(log10(d));
		int k = int(d*pow(10,-n)+0.5);
		aa.dv = pow(10,n)*k;
		aa.o=0;	aa.ds = pow(10,n);
	}
}
//-----------------------------------------------------------------------------
int mgl_tick_ext(float a, float b, wchar_t s[32], float &v)
{
	int kind = 0;
	if(fabs(a-b)<0.01*fabs(a))
	{
		kind = 1;
		v = fabs(a-b);
		if(v>100.f)
		{
			int k=int(log10(v)-0.01);
			kind=3;		v=mgl_ipow(10,k);
			mglprintf(s, 32, L"(@{\\times{}10^{%d}})", k);
		}
		if(v<1e-2f)
		{
			int k=int(log10(v)-0.01)-1;
			kind=3;		v=mgl_ipow(10,k);
			mglprintf(s, 32, L"(@{\\times{}10^{%d}})", k);
		}
	}
	else
	{
		v = fabs(b)>fabs(a)?fabs(b):fabs(a);
		if(v>100.f)
		{
			kind = 2;
			int k=int(log10(v)-0.01);
			v=mgl_ipow(10,k);
			mglprintf(s, 32, L"\\times 10^{%d}", k);
		}
		if(v<1e-2f)
		{
			kind = 2;
			int k=int(log10(v)-0.01)-1;
			v=mgl_ipow(10,k);
			mglprintf(s, 32, L"\\times 10^{%d}", k);
		}
	}
	return kind;
}
//-----------------------------------------------------------------------------
void mgl_tick_text(float z, float z0, float d, float v, int kind, wchar_t str[64])
{
	float u = fabs(z)<d ? 0:z;
	if((kind&1) && z>z0)	u = fabs(z-z0)<d ? 0:(z-z0);
	if(kind==2 || (kind==3 && z>z0))	u /= v;
	if((kind&1) && z>z0)
	{
		int n1,n2;
		mglprintf(str, 64, fabs(u)<1 ? L"@{(+%.2g)}" : L"@{(+%.3g)}",u);
		n1=wcslen(str);	mglprintf(str, 64, L"@{(+%g)}",u);	n2=wcslen(str);
		if(n1<n2)	mglprintf(str, 64, fabs(u)<1 ? L"@{(+%.2g)}" : L"@{(+%.3g)}",u);
	}
	else
	{
		int n1,n2;
		mglprintf(str, 64, fabs(u)<1 ? L"%.2g" :  L"%.3g",u);
		n1=wcslen(str);	mglprintf(str, 64, L"%g",u);	n2=wcslen(str);
		if(n1<n2)	mglprintf(str, 64, fabs(u)<1 ? L"%.2g" :  L"%.3g",u);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::LabelTicks(mglAxis &aa)
{
	wchar_t buf[64];
	float v,v0,v1,w;
	if(aa.f==2 || aa.upd)	return;
	aa.num=0;	aa.str[0]=aa.buf;
	if(aa.f==1)	// time ticks
	{
		for(v=aa.v1; v<=aa.v2; v+=aa.dv)
		{
			time_t tt = v;	tm tp;
			localtime_r(&tt,&tp);	wcsftime(buf,64,aa.t,&tp);
			aa.AddLabel(v,buf);
		}
	}
	else if(aa.dv==0 && aa.v1>0)	// positive log-scale
	{
		v0 = exp(M_LN10*floor(0.1+log10(aa.v1)));
		for(v=v0;v<=aa.v2*MGL_FLT_EPS;v*=10)	if(v*MGL_FLT_EPS>=aa.v1)
		{
			mglprintf(buf,64,L"10^{%d}",int(floor(0.1+log10(v))));
			aa.AddLabel(v,buf);
		}
	}
	else if(aa.dv==0 && aa.v2<0)
	{
		v0 = -exp(M_LN10*floor(0.1+log10(-aa.v2)));
		for(v=v0;v>=aa.v1*MGL_FLT_EPS;v*=10)	if(v*MGL_FLT_EPS<=aa.v2)
		{
			mglprintf(buf,64,L"-10^{%d}",int(floor(0.1+log10(-v))));
			aa.AddLabel(v,buf);
		}
	}
	else if(aa.dv)	// ticks drawing
	{
		int kind=0;
		wchar_t s[32]=L"";
		if(aa.t[0]==0) kind = mgl_tick_ext(aa.v2, aa.v1, s, w);
		if(!TuneTicks)	kind = 0;

		v0 = isnan(aa.o) ? aa.v0 : aa.o;
		if(aa.v2>aa.v1)
		{	v1 = aa.v2;		v0 = v0 - aa.dv*floor((v0-aa.v1)/aa.dv+1e-3);	}
		else
		{	v1 = aa.v1;		v0 = v0 - aa.dv*floor((v0-aa.v2)/aa.dv+1e-3);	}

		for(v=v0;v<=v1;v+=aa.dv)
		{
			if(aa.t[0])	mglprintf(buf, 64, aa.t, fabs(v)<aa.dv/100 ? 0 : v);
			else	mgl_tick_text(v,v0,aa.dv/100,w,kind,buf);
			wcstrim_mgl(buf);	aa.AddLabel(v,buf);
		}
		if(kind&2)	aa.AddLabel(FactorPos*(aa.v2-aa.v1)+aa.v1,s);
	}
	aa.upd = true;
}
//-----------------------------------------------------------------------------
void mglCanvas::Axis(const char *dir, bool adjust)
{
	if(!dir || !dir[0])	return;
	bool text = !strchr(dir,'_');
	const char *ar = "AKDTVISO";
	char arr=0;
	for(unsigned i=0;i<strlen(ar);i++)
		if(strchr(dir,ar[i]))	{	arr=ar[i];	break;	}

	AdjustTicks(dir,adjust);
	// TODO: Ternary axis labeling ...
	if(strchr(dir,'x'))	DrawAxis(ax, text, arr);
	if(strchr(dir,'y'))	DrawAxis(ay, text, arr);
	if(strchr(dir,'z'))	DrawAxis(az, text, arr);
}
//-----------------------------------------------------------------------------
void mglCanvas::DrawAxis(mglAxis &aa, bool text, char arr)
{
	aa.org = mglPoint(GetOrgX(aa.ch), GetOrgY(aa.ch), GetOrgZ(aa.ch));
	if(aa.ch=='x')	aa.v0 = aa.org.x;
	if(aa.ch=='y')	aa.v0 = aa.org.y;
	if(aa.ch=='z')	aa.v0 = aa.org.z;
	mglPoint d = aa.dir, o = aa.org - d*(aa.org*d);	// "transverse" org
	LabelTicks(aa);
	mglPoint av=(Min+Max)/2, dv,da,db, p;
	dv = mglPoint(sign(av.x-o.x)*ax.dv, sign(av.y-o.y)*ay.dv, sign(av.z-o.z)*az.dv);
	da = aa.a*(dv*aa.a);	db = aa.b*(dv*aa.b);
	SetPenPal(AxisStl);

	register long i,k1,k2;
	p = o + d*aa.v1;	ScalePoint(p);	k1 = AddPntC(p,CDef);
	for(i=1;i<31;i++)	// axis itself
	{
		p = o + d*(aa.v1+(aa.v2-aa.v1)*i/30.);	ScalePoint(p);
		k2 = k1;	k1 = AddPntC(p,CDef);
		line_plot(k2,k1);
	}
	if(arr)
	{
		p = o + d*(aa.v1+(aa.v2-aa.v1)*1.05);	ScalePoint(p);
		k2 = k1;	k1 = AddPntC(p,CDef);
		arrow_plot(k1,k2,arr);
	}

	k2 = aa.num;
	if(k2>0)	for(i=0;i<k2;i++)
	{
		p = o+d*aa.val[i];	tick_draw(p,da,db,0);
		register float v;
		if(text)	for(v=aa.val[i]+aa.ds; v <= (i<k2-1 ? aa.val[i+1]:aa.v2); v+=aa.ds)
			tick_draw(o+d*v,da,db,1);
	}
	if(text)	DrawLabels(aa);
}
//-----------------------------------------------------------------------------
void mglCanvas::DrawLabels(mglAxis &aa)
{
	aa.org = mglPoint(GetOrgX(aa.ch), GetOrgY(aa.ch), GetOrgZ(aa.ch));
	mglPoint d = aa.dir, o = aa.org - d*(aa.org*d);	// "transverse" org
	mglPoint p, s=(Min+Max)/2, q=aa.org;
	ScalePoint(s,false);	ScalePoint(q,false);

	register long i,k1,n = aa.num;
	if(n>0)	for(i=0;i<n;i++)	// TODO: Add labels "rotation", "missing" and so on
	{
		p = o+d*aa.val[i];
		ScalePoint(p,false);	k1 = AddPntN(p,-1,aa.dir,0);
		text_plot(k1, aa.str[i], s.y>q.y ? "T":"t", -1);
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::tick_draw(mglPoint o, mglPoint d1, mglPoint d2, int f)
{
	if(TickLen==0)	return;
	float v = TickLen/sqrt(1+f);
	mglPoint p;
	long k1,k2,k3;

	if(*TickStl && !f)	SetPenPal(TickStl);
	if(*SubTStl && f)	SetPenPal(SubTStl);
	p = o+d1*v;	ScalePoint(p);	k1 = AddPntC(p,CDef);
	p = o;		ScalePoint(p);	k2 = AddPntC(p,CDef);
	p = o+d2*v;	ScalePoint(p);	k3 = AddPntC(p,CDef);
	line_plot(k1,k2);	line_plot(k2,k3);
}
//-----------------------------------------------------------------------------
void mglCanvas::Grid(const char *dir, const char *pen)
{
	if(!dir || !dir[0])	return;

	AdjustTicks(dir,false);
	SetPenPal(pen);
	// TODO: Ternary axis labeling ...
	if(strchr(dir,'x'))	DrawGrid(ax);
	if(strchr(dir,'y'))	DrawGrid(ay);
	if(strchr(dir,'z'))	DrawGrid(az);
}
//-----------------------------------------------------------------------------
void mglCanvas::DrawGrid(mglAxis &aa)
{
	aa.org = mglPoint(GetOrgX(aa.ch), GetOrgY(aa.ch), GetOrgZ(aa.ch));
	LabelTicks(aa);
	mglPoint d=aa.dir, da1,da2,db1,db2,oa,ob, p,q;
	da1 = aa.a*(aa.a*Min);	da2 = aa.a*(aa.a*Max);
	db1 = aa.b*(aa.b*Min);	db2 = aa.b*(aa.b*Max);
	oa  = aa.b*(aa.b*aa.org);	ob  = aa.a*(aa.a*aa.org);

	register long i,j,n=aa.num,k1,k2;
	float v;

	ReserveC(62*n);
	if(n>0)	for(i=0;i<n;i++)
	{
		q = oa+d*aa.val[i];	p = q+da1;	// lines along 'a'
		ScalePoint(p);		k1 = AddPntC(p,CDef);
		for(j=1;j<31;j++)
		{
			v = i/30.;
			p = q+da1*(1-v)+da2*v;	ScalePoint(p);
			k2 = k1;	k1 = AddPntC(p,CDef);
			line_plot(k2,k1);
		}
		q = ob+d*aa.val[i];	p = q+db1;	// lines along 'b'
		ScalePoint(p);		k1 = AddPntC(p,CDef);
		for(j=1;j<31;j++)
		{
			v = i/30.;
			p = q+db1*(1-v)+db2*v;	ScalePoint(p);
			k2 = k1;	k1 = AddPntC(p,CDef);
			line_plot(k2,k1);
		}
	}
}
//-----------------------------------------------------------------------------
void mglCanvas::Colorbar(const char *sch,int where)
{	// ‘0’ - right, ‘1’ - left, ‘2’ - above, ‘3’ - under
	if(sch && strchr(sch,'>'))	where = 0;
	if(sch && strchr(sch,'<'))	where = 1;
	if(sch && strchr(sch,'^'))	where = 2;
	if(sch && strchr(sch,'_'))	where = 3;
	if(sch && strchr(sch,'A'))	{	Push();	Identity();	}
	Colorbar(where, where==0?1:0, where==2?1:0, 1, 1, AddTexture(sch));
	if(sch && strchr(sch,'A'))	Pop();
}
//-----------------------------------------------------------------------------
void mglCanvas::Colorbar(int where, float x, float y, float w, float h, long s)
{
	float d = fabs(Max.c-Min.c);
	d = floor(d*pow(10,-floor(log10(d))));
	long n = 50*(d<4?int(2*d+0.5):int(d+0.5))+1;
	if(d==1.f)	n = 101;
	mglData v(n);
	if(ac.d || Min.c*Max.c<=0)	v.Fill(Min.c,Max.c);
	else if(Max.c>Min.c && Min.c>0)
	{	v.Fill(log(Min.c), log(Max.c));	v.Modify("exp(u)");		}
	else if(Min.c<Max.c && Max.c<0)
	{	v.Fill(log(-Min.c), log(-Max.c));	v.Modify("-exp(u)");	}
	mglColor *c=new mglColor[n];
	for(long i=0;i<n;i++)	c[i] = GetC(s,v.a[i]);
	colorbar(&v, c, where, x, y, w, h);
	delete []c;
}
//-----------------------------------------------------------------------------
void mglCanvas::Colorbar(HCDT v, const char *sch,int where)
{
	if(sch && strchr(sch,'>'))	where = 0;
	if(sch && strchr(sch,'<'))	where = 1;
	if(sch && strchr(sch,'^'))	where = 2;
	if(sch && strchr(sch,'_'))	where = 3;
	if(sch && strchr(sch,'A'))	{	Push();	Identity();	}
	Colorbar(v,sch,where, where==0?1:0, where==2?1:0, 1, 1);
	if(sch && strchr(sch,'A'))	Pop();
}
//-----------------------------------------------------------------------------
void mglCanvas::Colorbar(HCDT v, const char *sch, int where, float x, float y, float w, float h)
{
	mglColor *c=new mglColor[v->GetNx()];
	if(!sch || !(*sch))	sch = MGL_DEF_PAL;
	for(long i=0;i<v->GetNx();i++)	c[i] = mglColor(sch[i%strlen(sch)]);
	colorbar(v, c, where, x, y, w, h);
	delete []c;
}
//-----------------------------------------------------------------------------
void mglCanvas::Label(char dir, const char *str, float pos, float size, float shift)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Labelw(dir, wcs, pos, size, shift);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglCanvas::Labelw(char dir, const wchar_t *text, float pos, float size, float shift)
{
	float t, x0, y0, z0;
	x0 = GetOrgX(dir);	y0 = GetOrgY(dir);	z0 = GetOrgZ(dir);
	mglPoint p,s=(Min+Max)/2,q(x0,y0,z0);
	ScalePoint(s,false);	ScalePoint(q,false);

	char font[33]="C";
	if(pos<-0.2)	font[0]='L';	if(pos>0.2)	font[0]='R';
	strcat(font,FontDef);
	strcat(font,s.y>q.y ? "T":"t");

	if(dir=='x')	//	TODO: Tern axis & colorbar labels!!!
	{
		AdjustTicks(ax,fx);
		if(ax.dv)	t = (Min.x+Max.x+pos*(Max.x-Min.x))/2;
		else	t = Min.x*pow(Max.x/Min.x, (pos+1)/2);
		p = mglPoint(t,y0,z0);	q = mglPoint(1,0,0);
	}
	if(dir=='y')
	{
		AdjustTicks(ay,fy);
		if(ay.dv)	t = (Min.y+Max.y+pos*(Max.y-Min.y))/2;
		else	t = Min.y*pow(Max.y/Min.y, (pos+1)/2);
		p = mglPoint(x0,t,z0);	q = mglPoint(0,1,0);
	}
	if(dir=='z')
	{
		AdjustTicks(az,fz);
		if(az.dv)	t = (Min.z+Max.z+pos*(Max.z-Min.z))/2;
		else	t = Min.z*pow(Max.z/Min.z, (pos+1)/2);
		p = mglPoint(x0,y0,t);	q = mglPoint(0,0,1);
	}
	ScalePoint(p);
	text_plot(AddPntN(p,-1,q,0),text,font,size,1+shift);
}
//-----------------------------------------------------------------------------
void mglCanvas::Label(float x, float y, const char *str, const char *fnt, float size, bool rel)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Labelw(x,y,wcs, fnt, size, rel);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglCanvas::Labelw(float x, float y, const wchar_t *text, const char *fnt, float size, bool rel)
{
	Push();	Identity(rel);
	mglFormula *ox=fx, *oy=fy, *oz=fz;
	fx = fy = fz = NULL;
	char *f = new char[strlen(fnt)+1];
	strcpy(f,fnt);
	for(int i=0;f[i];i++)	if(f[i]=='a' || f[i]=='A')	f[i]=' ';
	mglPoint p((Min.x+Max.x)/2+PlotFactor*(Max.x-Min.x)*(x-0.5),
				(Min.y+Max.y)/2+PlotFactor*(Max.y-Min.y)*(y-0.5), Max.z);
	ScalePoint(p);	text_plot(AddPntN(p,-1,mglPoint(NAN),0),text,f,size);
	delete []f;	fx=ox;	fy=oy;	fz=oz;	Pop();
}
//-----------------------------------------------------------------------------
void mglCanvas::Title(const wchar_t *str,const char *font,float size)
{
	Push();	Identity();
	mglFormula *ox=fx, *oy=fy, *oz=fz;
	fx = fy = fz = NULL;
	mglPoint p((Min.x+Max.x)/2, Max.y+(Max.y-Min.y)*0.15, (Min.z+Max.z)/2);
	ScalePoint(p);	text_plot(AddPntN(p,-1,mglPoint(NAN),0), str, font, size);
	fx=ox;	fy=oy;	fz=oz;	Pop();
}
//-----------------------------------------------------------------------------
void mglCanvas::Title(const char *str,const char *font,float size)
{
	unsigned s = strlen(str)+1;
	wchar_t *wcs = new wchar_t[s];
	mbstowcs(wcs,str,s);
	Title(wcs, font, size);
	delete []wcs;
}
//-----------------------------------------------------------------------------
void mglCanvas::Box(const char *col, bool ticks)
{
	mglPoint o = Org;
	float tl=TickLen;
	if(!ticks)	TickLen=0;
	Org = Min;	Axis("xyz_");
	if(TernAxis==0)
	{
		Org.z=Max.z;	Axis("xy_");
		Org = Max;		Axis("xyz_");
		Org.z=Min.z;	Axis("xy_");
		Org.x=Min.x;	DrawAxis(az,0,0);
		Org.x=Max.x;	Org.y=Min.y;	DrawAxis(az,0,0);
	}
	else if(TernAxis==1)
	{
		Axis("t_");
		Org.z=Max.z;	Axis("xyt_");
		Org.x=Max.x;	Axis("z_");
		Org.x=Min.x;	Org.y=Max.y;	Axis("z_");
	}
	else if(TernAxis==2)
		Axis("tuv_");
	Org=o;	TickLen=tl;
}
//-----------------------------------------------------------------------------
float mglCanvas::text_plot(long p,const wchar_t *text,const char *fnt,float size,float sh)
{
	// TODO: Add code here
	return 0;
}
//-----------------------------------------------------------------------------
void mglCanvas::colorbar(HCDT v, const mglColor *s, int where, float x, float y, float w, float h)
{
	// TODO: Add code here
}
//-----------------------------------------------------------------------------