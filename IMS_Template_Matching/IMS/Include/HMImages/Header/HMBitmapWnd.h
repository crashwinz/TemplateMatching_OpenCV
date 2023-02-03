#if !defined(AFX_HMBITMAPWND_H__202D976A_1A25_4331_BC56_8F3C19CB5F19__INCLUDED_)
#define AFX_HMBITMAPWND_H__202D976A_1A25_4331_BC56_8F3C19CB5F19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HMBitmapWnd.h : header file
//

#include "HMBitmap.h"
#include <list>

/////////////////////////////////////////////////////////////////////////////
// CHMBitmapWnd window

#define UM_LBUTTONDOWN		WM_USER + 1212
#define UM_LBUTTONUP		WM_USER + 1213
#define UM_MOUSEMOVE		WM_USER + 1214

class AFX_EXT_CLASS CHMBitmapWnd : public CWnd, public CHMBitmap
{
// Construction
public:
	CHMBitmapWnd();

// Attributes
public:

// Operations
public:
	BOOL				InitBitmap();

	int					GetHScroll()		{ return m_nHScroll; }
	int					GetVScroll()		{ return m_nVScroll; }
	// Display 좌표와 Bitmap 좌표 변환
	CRect				TransDisplayToBitmap(CRect rt);
	CRect				TransBitmapToDisplay(CRect rt);
	double				GetResolutionWidth()			{ return m_dDisplayWidthResolution; }
	double				GetResolutionHeight()			{ return m_dDisplayHeightResolution; }

	// Image Draw
	void				SetShowImage(CWnd* pParentWnd, CRect* pRect, BOOL bShowImage, BOOL bModifySize);
	void				SetShowImage(BOOL bShowImage);

	void				DrawTransRectangle(CRect& rt);
	void				EraseTransRectangle(CRect& rtErase);
	void				EraseTransRectangle();
	void				DrawTransEllipse(CRect& rt);
	void				EraseTransEllipse(CRect& rtErase);
	void				EraseTransEllipse();
	void				DrawLine(CRect& rt);
	void				EraseLine(CRect& rtErase);
	void				EraseLine();
	void				ShowCrossLine(BOOL bShowCrossLine, COLORREF color = RGB(0, 0, 0), int nGab = 0);
	void				ShowProcessingImage(BOOL bProcessing, int nPower = 1);
	void				ImageConvolution();

	void				MoveBitmapWindow(LPCRECT lpRect, BOOL bRepaint = TRUE);

	void				SetCrossLineType(int nType) {m_nCrossLineType = nType; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHMBitmapWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHMBitmapWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHMBitmapWnd)
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// 부모 윈도우 핸들
	CWnd*				m_pParentWnd;
	//////////////////////////////////////////////////////////////////////////
	// Display 여부
	BOOL				m_bShowImage;
	CRect				m_RectDisplay;
	double				m_dDisplayWidthResolution;
	double				m_dDisplayHeightResolution;

	//////////////////////////////////////////////////////////////////////////
	// Drawing Variable
	// Rectangle
	list<CRect>			m_DrawRectangle;
	// Circle
	list<CRect>			m_DrawRectEllipse;
	// Line
	list<CRect>			m_DrawLine;

	BOOL				m_bShowCrossLine;
	COLORREF			m_ColorCrossLIne;
	int					m_nCrossLineGab;
	BOOL				m_bShowProcess;
	BOOL				m_nConvPower;
private:
	
	// Scroll Pos
	int					m_nVScroll;
	int					m_nHScroll;

	// Max Scroll Pos
	int					m_nMaxVScroll;
	int					m_nMaxHScroll;

	int					m_nCrossLineType;

	// 2019.09.11 LHS Color Balance 기능 추가
protected:
	BOOL				m_bShowBrightImage;
	int					m_nBrightLevel;

public:
	void				ImageBrightness(int nLevel);
	void				ShowBrightImage(BOOL bShowBrightImage, int nLevel = 64);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HMBITMAPWND_H__202D976A_1A25_4331_BC56_8F3C19CB5F19__INCLUDED_)
