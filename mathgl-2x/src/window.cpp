/***************************************************************************
 * window.cpp is part of Math Graphic Library
 * Copyright (C) 2007-2012 Alexey Balakin <mathgl.abalakin@gmail.ru>       *
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
#include "mgl2/canvas_wnd.h"
//-----------------------------------------------------------------------------
mglCanvasWnd::mglCanvasWnd() : mglCanvas()
{
	Setup();	LoadFunc=0;	FuncPar=0;	DrawFunc=0;	ClickFunc=0;
	GG = 0;		NumFig = 0;	CurFig = -1;
}
//-----------------------------------------------------------------------------
mglCanvasWnd::~mglCanvasWnd()	{	if(GG) free(GG);	}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetCurFig(int c)
{
	CurFig=c;
	if(get(MGL_VECT_FRAME) && c>=0 && c<(long)DrwDat.size())	GetFrame(c);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::ClearFrames()
{
	if(GG)	free(GG);	GG = 0;
	CurFrameId = NumFig = CurFig = 0;
	DrwDat.clear();
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetSize(int w,int h)
{
	ClearFrames();
	mglCanvas::SetSize(w,h);
//	if(Wnd)	Wnd->size(w,h);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::EndFrame()
{
	CurFig = CurFrameId-1;
	if(!GG)
	{
		GG = (unsigned char *)malloc(3*Width*Height);
		NumFig = 1;		CurFig = 0;
	}
	else if(CurFig>NumFig-1)
	{
		GG = (unsigned char *)realloc(GG,3*(NumFig+1)*Width*Height);
		NumFig++;
	}
	mglCanvas::EndFrame();
	memcpy(GG + CurFig*Width*Height*3,G,3*Width*Height);
	CurFig++;
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::DelFrame(long i)
{
	if(i<0 || i>=CurFrameId)	return;
	if(CurFig>=i)	CurFig--;
	long n = Width*Height*3;
	if(CurFrameId-i>1)	memmove(GG+i*n, GG+i*n+n, n*(CurFrameId-i-1));
	mglCanvas::DelFrame(i);
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::SetDrawFunc(int (*draw)(mglBase *gr, void *p), void *par, void (*reload)(void *p))
{
	ClearFrames();
	int n = draw ? draw(this,par) : 0;
	if(n<NumFig && n>=0)	NumFig = n;
	DrawFunc = draw;		FuncPar = par;
	LoadFunc = reload;
}
//-----------------------------------------------------------------------------
const unsigned char *mglCanvasWnd::GetBits()
{
	const unsigned char *g = mglCanvas::GetBits();
	if(GG && NumFig>0 && CurFig<NumFig && CurFig>=0 && !get(MGL_VECT_FRAME))
		g = GG + CurFig*Width*Height*3;
	return g;
}
//-----------------------------------------------------------------------------
void mglCanvasWnd::ReLoad()
{
	if(LoadFunc)
	{
		LoadFunc(FuncPar);
		// update number of slides
		ClearFrames();
		int n = DrawFunc ? DrawFunc(this,FuncPar) : 0;
		if(n<NumFig && n>=0)	NumFig = n;
		Update();
	}
}
//-----------------------------------------------------------------------------
void mgl_wnd_toggle_alpha(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleAlpha();	}
void mgl_wnd_toggle_light(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleLight();	}
void mgl_wnd_toggle_zoom(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleZoom();	}
void mgl_wnd_toggle_rotate(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleRotate();	}
void mgl_wnd_toggle_no(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ToggleNo();	}
void mgl_wnd_update(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Update();	}
void mgl_wnd_reload(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ReLoad();	}
void mgl_wnd_adjust(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Adjust();	}
void mgl_wnd_next_frame(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->NextFrame();	}
void mgl_wnd_prev_frame(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->PrevFrame();	}
void mgl_wnd_animation(HMGL gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Animation();	}
void mgl_setup_window(HMGL gr, int clf_upd, int showpos)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->Setup(clf_upd, showpos);	}
void mgl_set_click_func(HMGL gr, void (*func)(void *p))
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);	if(g)	g->ClickFunc = func;	}
void mgl_get_last_mouse_pos(HMGL gr, mreal *x, mreal *y, mreal *z)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>(gr);
	mglPoint p;	if(g)	p=g->GetMousePos();
	*x=p.x;	*y=p.y;	*z=p.z;	}
//-----------------------------------------------------------------------------
void mgl_wnd_toggle_alpha_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleAlpha();	}
void mgl_wnd_toggle_light_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleLight();	}
void mgl_wnd_toggle_zoom_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleZoom();	}
void mgl_wnd_toggle_rotate_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleRotate();	}
void mgl_wnd_toggle_no_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ToggleNo();	}
void mgl_wnd_update_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Update();	}
void mgl_wnd_reload_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->ReLoad();	}
void mgl_wnd_adjust_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Adjust();	}
void mgl_wnd_next_frame_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->NextFrame();	}
void mgl_wnd_prev_frame_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->PrevFrame();	}
void mgl_wnd_animation_(uintptr_t *gr)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Animation();	}
void mgl_setup_window_(uintptr_t *gr, int *clf_upd, int *showpos)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	if(g)	g->Setup(*clf_upd, *showpos);	}
void mgl_get_last_mouse_pos_(uintptr_t *gr, mreal *x, mreal *y, mreal *z)
{	mglCanvasWnd *g = dynamic_cast<mglCanvasWnd *>((HMGL)(*gr));
	mglPoint p;	if(g)	p=g->GetMousePos();
	*x=p.x;	*y=p.y;	*z=p.z;	}
//-----------------------------------------------------------------------------
//
//	mglDraw class handling
//
//-----------------------------------------------------------------------------
/*int mgl_draw_class(HMGL gr, void *p)	// so stupid way to save mglDraw class inheritance :(
{
	mglGraph g(gr);	mglWindow *w = (mglWindow *)p;
	return (w && w->dr) ? w->dr->Draw(&g) : 0;
}
void mgl_reload_class(void *p)	// so stupid way to save mglDraw class inheritance :(
{	mglWindow *w = (mglWindow *)p;	if(w && w->dr)	w->dr->Reload();}
void mgl_click_class(void *p)	// so stupid way to save mglDraw class inheritance :(
{	mglWindow *w = (mglWindow *)p;	if(w && w->dr)	w->dr->Click();	}*/
int mgl_draw_class(HMGL gr, void *p)
{	mglGraph g(gr);	mglDraw *dr = (mglDraw *)p;	return dr->Draw(&g);	}
void mgl_reload_class(void *p)
{	mglDraw *dr = (mglDraw *)p;	dr->Reload();	}
void mgl_click_class(void *p)
{	mglDraw *dr = (mglDraw *)p;	dr->Click();		}
//-----------------------------------------------------------------------------
typedef int (*draw_func)(mglGraph *gr);
int mgl_draw_graph(HMGL gr, void *p)
{
	mglGraph g(gr);
	draw_func func = (draw_func)(p);
	return func ? func(&g) : 0;
}
//-----------------------------------------------------------------------------
void *mgl_draw_calc(void *p)
{
#if MGL_HAVE_PTHREAD
	((mglDraw *)p)->Calc();
#endif
	return 0;
}
//-----------------------------------------------------------------------------
void mgl_draw_thr(void *p)
{
#if MGL_HAVE_PTHREAD
	mglDraw *d = (mglDraw *)p;
	if(!d || d->running)	return;
	pthread_create(&(d->thr),0,mgl_draw_calc,d);
	pthread_detach(d->thr);
#endif
}
//-----------------------------------------------------------------------------
