/* mgl_eval.h is part of Math Graphic Library
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
//---------------------------------------------------------------------------
#ifndef _MGL_EVAL_H_
#define _MGL_EVAL_H_
//---------------------------------------------------------------------------
// ���� ������
#define MGL_ERR_LOG		1
#define MGL_ERR_ARC		2
#define MGL_ERR_SQRT	3
//---------------------------------------------------------------------------
/// Class for evaluating formula specified by the string
class mglFormula					// ������ ��� ����� � ���������� ������
{
public:
	/// Evaluates the formula for 'x','r'=\a x, 'y','n'=\a y, 'z','t'=\a z, 'u'=\a u
	float Calc(float x,float y=0,float z=0,float u=0);
	/// Evaluates the formula for 'x, y, z, u, v, w'
	float Calc(float x,float y,float z,float u,float v,float w);
	/// Evaluates the formula for variables \a var
	float Calc(float var['z'-'a'+1]);
	/// Return error code
	int GetError();
	/// Parse the formula \a str and create formula-tree
	mglFormula(const char *str);
	/// Clean up formula-tree
	~mglFormula();
protected:
	static float a1['z'-'a'+1];
	float CalcIn();
	mglFormula *Left,*Right;	// ������ � ������ ��������� �������
	int Kod;					// ��� ��������
	float Res;					// ����� ���� ��� ���������� � �� ����������
	static int Error;
};
//---------------------------------------------------------------------------
char *mgl_strdup(const char *s);
//-----------------------------------------------------------------------------
#endif
