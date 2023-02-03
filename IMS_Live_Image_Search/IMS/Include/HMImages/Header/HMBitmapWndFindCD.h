// HMBitmapWndFindCD.h: interface for the CHMBitmapWndFindCD class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMBITMAPWNDFINDCD_H__FCE2589A_A4E4_4C33_B704_4F8F1E91EBB7__INCLUDED_)
#define AFX_HMBITMAPWNDFINDCD_H__FCE2589A_A4E4_4C33_B704_4F8F1E91EBB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HMBitmapWnd.h"

typedef struct _LINEEQUATION
{
	float s_fA;
	float s_fB;
	float s_fC;

	float s_fStartX;
	float s_fStartY;
	float s_fEndX;
	float s_fEndY;
	float s_fAngle;

	_LINEEQUATION()
	{
		Reset();
	}

	void Reset()
	{
		s_fA = 0.0;
		s_fB = 0.0;
		s_fC = 0.0;

		s_fStartX = 0.0;
		s_fStartY = 0.0;
		s_fEndX = 0.0;
		s_fEndY = 0.0;
		s_fAngle = 0.0;
	}

	void SetTwoPoint(float fStartX, float fStartY, float fEndX, float fEndY)
	{
		s_fStartX = fStartX;
		s_fStartY = fStartY;
		s_fEndX = fEndX;
		s_fEndY = fEndY;

		CalcAngle();
	}

	void CalcAngle()
	{
		s_fA = (s_fEndY - s_fStartY) / (s_fEndX - s_fStartX);
		s_fB = -1.0;
		s_fC = s_fStartY - s_fA * s_fStartX;

		s_fAngle = atan(s_fA);
	}
} LineEquation, *pLineEquation;

enum LineDirection	{LineUnknown, LineVert, LineHoriz};

#define CD_RESULT_LINEUNKNOWN		1
#define CD_RESULT_NOTFINDLINE		2
#define CD_RESULT_SUCCESS			0

class AFX_EXT_CLASS CHMBitmapWndFindCD : public CHMBitmapWnd  
{
public:
	CHMBitmapWndFindCD();
	virtual ~CHMBitmapWndFindCD();

	void				SetThresholdOffset(double dOffset)		{ m_dThresholdOffset = dOffset; }
	int					FindCD(CHMBitmap* pBitmap, double dResolution, double* pfCDum, double* pfCDPixel = NULL, double* pfAngle = NULL);
protected:

	BOOL				ImageFindLine(CHMBitmap* pDest, LineDirection& LineDir, LineEquation& LineFirst, LineEquation& LineSecond);
	LineDirection		ImageFindLineDirection();

	double				m_dThresholdOffset;
};

#endif // !defined(AFX_HMBITMAPWNDFINDCD_H__FCE2589A_A4E4_4C33_B704_4F8F1E91EBB7__INCLUDED_)
