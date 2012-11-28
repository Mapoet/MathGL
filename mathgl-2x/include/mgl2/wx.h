/***************************************************************************
 * wx.h.cpp is part of Math Graphic Library
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
#ifndef MGL_WX_H
#define MGL_WX_H
//-----------------------------------------------------------------------------
#include <mgl2/base.h>
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
/// Creates WX window for plotting
HMGL mgl_create_graph_wx(int (*draw)(HMGL gr, void *p), const char *title, void *par, void (*load)(void *p));
uintptr_t mgl_create_graph_wx_(const char *title, int);
/// Run main WX loop for event handling.
int mgl_wx_run();
int mgl_wx_run_();
#ifdef __cplusplus
}
//-----------------------------------------------------------------------------
#include <mgl2/wnd.h>
//-----------------------------------------------------------------------------
/// Wrapper class for windows displaying graphics
class mglWX : public mglWnd
{
public:
	mglWX(const char *title="MathGL") : mglWnd()
	{	gr = mgl_create_graph_wx(0,title,0,0);	}
	mglWX(int (*draw)(HMGL gr, void *p), const char *title="MathGL", void *par=NULL, void (*load)(void *p)=0) : mglWnd()
	{	gr = mgl_create_graph_wx(draw,title,par,load);	}
	mglWX(int (*draw)(mglGraph *gr), const char *title="MathGL") : mglWnd()
	{	gr = mgl_create_graph_wx(mgl_draw_graph,title,(void*)draw,0);	}
	mglWX(mglDraw *draw, const char *title="MathGL") : mglWnd()
	{	gr = mgl_create_graph_wx(mgl_draw_class,title,draw,mgl_reload_class);
		mgl_set_click_func(gr, mgl_click_class);	}
	int Run()	{	return mgl_wx_run();	}	///< Run main loop for event handling

};
//-----------------------------------------------------------------------------
#include <wx/window.h>
#include <wx/image.h>
#include <wx/timer.h>
#include <wx/bitmap.h>
class mglCanvas;
//-----------------------------------------------------------------------------
/// Class is Wx widget which display MathGL graphics
class wxMathGL : public wxWindow
{
public:
	wxString appName;	///< Application name for message boxes
	bool AutoResize; 	///< Allow auto resizing (default is false)

	wxMathGL(wxWindow *parent, wxWindowID id=-1, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=0, const wxString& name=wxPanelNameStr);
	virtual ~wxMathGL();
	double GetRatio();
	void SetPopup(wxMenu *p)	{	popup = p;	};	///< Set popup menu pointer
	void SetSize(int w, int h);		///< Set window/picture sizes
	void SetGraph(HMGL gr);	///< Set grapher object
	inline void SetGraph(mglGraph *GR)
	{	SetGraph(GR->Self());	}
	inline HMGL GetGraph()	{	return (HMGL)gr;	}

	/// Set drawing functions and its parameter
	inline void SetDraw(int (*func)(mglBase *gr, void *par), void *par=0)
	{	draw_func = func;	draw_par = par;	}
	inline void SetDraw(mglDraw *dr)
	{	draw_cl = dr;	}
	inline void SetDraw(int (*draw)(mglGraph *gr))
	{	SetDraw(mgl_draw_graph,(void*)draw);	}
	inline void ZoomRegion(mreal xx1,mreal xx2,mreal yy1, mreal yy2)
	{	x1=xx1;	y1=yy1;	x2=xx2;	y2=yy2;	}

	int GetPer() 	{return per;};		///< Get perspective value
	int GetPhi() 	{return phi;};		///< Get Phi-angle value
	int GetTet() 	{return tet;};		///< Get Theta-angle value
	bool GetAlpha()	{return alpha;};	///< Get transparency state
	bool GetLight()	{return light;};	///< Get lightning state
	bool GetZoom()	{return zoom;};		///< Get mouse zooming state
	bool GetRotate()	{return rotate;};	///< Get mouse rotation state

	void Repaint();
	void Update();			///< Update picture
	void Copy(); 			///< copy graphics to clipboard
	void Print();			///< Print plot
//	void Stop();			///< Stop execution
	void SetPer(int p);		///< Set perspective value
	void SetPhi(int p);		///< Set Phi-angle value
	void SetTet(int t);		///< Set Theta-angle value
	void SetAlpha(bool a);	///< Switch on/off transparency
	void SetLight(bool l);	///< Switch on/off lightning
	void SetZoom(bool z);	///< Switch on/off mouse zooming
	void SetRotate(bool r);	///< Switch on/off mouse rotation
	void ZoomIn();			///< Zoom in graphics
	void ZoomOut();			///< Zoom out graphics
	void Restore();			///< Restore zoom and rotation to default values
//	void Reload();			///< Reload data and execute script
	void ShiftLeft();		///< Shift graphics to left direction
	void ShiftRight();		///< Shift graphics to right direction
	void ShiftUp();			///< Shift graphics to up direction
	void ShiftDown();		///< Shift graphics to down direction
	void ExportPNG(wxString fname=L"");	///< export to PNG file
	void ExportPNGs(wxString fname=L"");	///< export to PNG file (no transparency)
	void ExportJPG(wxString fname=L"");	///< export to JPEG file
	void ExportBPS(wxString fname=L"");	///< export to bitmap EPS file
	void ExportEPS(wxString fname=L"");	///< export to vector EPS file
	void ExportSVG(wxString fname=L"");	///< export to SVG file

	void Adjust();		///< Adjust plot size to fill entire window
	void NextSlide();	///< Show next slide
	void PrevSlide();	///< Show previous slide
	void Animation(bool st=true);	///< Start animation

	void About();		///< Show about information
	void AboutQt();		///< Show information about Qt version

protected:
	void OnPaint(wxPaintEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnNextSlide(wxTimerEvent& evt);	///< Show next slide
	void OnMouseLeftDown(wxMouseEvent &ev);
	void OnMouseDown(wxMouseEvent &ev);
	void OnMouseLeftUp(wxMouseEvent &ev);
	void OnMouseRightUp(wxMouseEvent &ev);
	void OnMouseMove(wxMouseEvent &ev);
//	void MousePressEvent(QMouseEvent *);
//	void MouseReleaseEvent(QMouseEvent *);
//	void MouseMoveEvent(QMouseEvent *);

	mglCanvas *gr;		///< pointer to grapher
	void *draw_par;		///< Parameters for drawing function mglCanvasWnd::DrawFunc.
	/// Drawing function for window procedure. It should return the number of frames.
	int (*draw_func)(mglBase *gr, void *par);
	mglDraw *draw_cl;

	wxString MousePos;	///< Last mouse position
	wxBitmap pic;		///< Pixmap for drawing (changed by update)
	double tet, phi;	///< Rotation angles
	double per;			///< Value of perspective ( must be in [0,1) )
	bool alpha;			///< Transparency state
	bool light;			///< Lightning state
	bool zoom;			///< Mouse zoom state
	bool rotate;		///< Mouse rotation state
	mreal x1,x2,y1,y2;	///< Zoom in region
	bool showMessage;	///< Flag for showing messages (enabled by each execute())
	wxMenu *popup;		///< Pointer to pop-up menu
	wxTimer *timer;		///< Timer for animation
	DECLARE_EVENT_TABLE()
private:
	int x0, y0, xe, ye;		///< Temporary variables for mouse
	unsigned char *grBuf;
};
//-----------------------------------------------------------------------------
#endif
#endif
