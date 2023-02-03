// HMBitmapWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "	\ add additional includes here"
#include "HMBitmapWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHMBitmapWndWnd

CHMBitmapWnd::CHMBitmapWnd()
{
	m_pParentWnd = NULL;
	
	m_RectDisplay.SetRect(0, 0, 0, 0);
	m_bShowImage = FALSE;

	m_nHScroll = 0;
	m_nVScroll = 0;

	m_nMaxHScroll = 0;
	m_nMaxVScroll = 0;

	m_DrawRectangle.clear();
	m_DrawRectEllipse.clear();
	m_DrawLine.clear();

	m_bShowCrossLine	= FALSE;
	m_nCrossLineGab		= 1;
	m_bShowProcess		= FALSE;
	m_nConvPower		= 1;

	m_bShowBrightImage = FALSE;
	m_nBrightLevel = 64;
}

CHMBitmapWnd::~CHMBitmapWnd()
{
	m_DrawRectangle.clear();
	m_DrawRectEllipse.clear();
	m_DrawLine.clear();
}

BEGIN_MESSAGE_MAP(CHMBitmapWnd, CWnd)
	//{{AFX_MSG_MAP(CHMBitmapWnd)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHMBitmapWnd message handlers

void CHMBitmapWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rt;
	if (!m_pBuffer || ! m_pBitmapInfo || !m_bShowImage)
	{
		MoveWindow(CRect(0, 0, 0, 0));
		return;
	}

	int nX, nY;
	int nImgWidth = m_nImageWidth;
	int nImgHeight = m_nImageHeight;
	//int nImgHeight = 360;
	
	if (m_nMaxHScroll == 0)
		nX = 0;
	else
	{
		nImgWidth = m_nImageWidth;
		nX = static_cast<int>(m_nHScroll / m_dDisplayWidthResolution);
	}
	if (m_nMaxVScroll == 0)
		nY = 0;
	else
	{
		nImgHeight = m_nImageHeight;
		nY = -1 * static_cast<int>(m_nVScroll / m_dDisplayHeightResolution);
	}
	
	// Draw Source Image
	dc.SetStretchBltMode(HALFTONE);
	if(m_bShowProcess)
	{
		ImageConvolution();

		if(m_pBufferConv)
		{
			StretchDIBits(dc.GetSafeHdc()
				, 0 , 0
				, m_RectDisplay.Width(), m_RectDisplay.Height()
				, nX, nY
				, nImgWidth, nImgHeight
				, m_pBufferConv
				, m_pBitmapInfo
				, DIB_RGB_COLORS, SRCCOPY);
		}
	}
	else
	{
		// 2019.09.11 LHS Color Balance 기능 추가
		if(m_bShowBrightImage)
		{
			ImageBrightness(m_nBrightLevel);

			if(m_pBufferBright)
			{
				StretchDIBits(dc.GetSafeHdc()
					, 0 , 0
					, m_RectDisplay.Width(), m_RectDisplay.Height()
					, nX, nY
					, nImgWidth, nImgHeight
					, m_pBufferBright
					, m_pBitmapInfo
					, DIB_RGB_COLORS, SRCCOPY);
			}
		}
		else
		{
			StretchDIBits(dc.GetSafeHdc()
				, 0 , 0
				, m_RectDisplay.Width(), m_RectDisplay.Height()
				, nX, nY
				, nImgWidth, nImgHeight
				, m_pBuffer
				, m_pBitmapInfo
				, DIB_RGB_COLORS, SRCCOPY);
		}
	}
	

	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	
	list<CRect>::iterator it;
	it = m_DrawRectangle.begin();
	while (it != m_DrawRectangle.end())
	{
		rt.SetRect(it->left, it->top, it->right, it->bottom);
		if (rt.Width() > 0 && rt.Height() > 0)
		{
			dc.Rectangle(&rt);
		}
		it++;
	}
	it = m_DrawRectEllipse.begin();
	while (it != m_DrawRectEllipse.end())
	{
		rt.SetRect(it->left, it->top, it->right, it->bottom);
		if (rt.Width() > 0 && rt.Height() > 0)
		{
			dc.Ellipse(&rt);
		}
		it++;
	}

	// Draw Line
	CPen Pen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen* pOldPen = (CPen*)dc.SelectObject(&Pen);
	it = m_DrawLine.begin();
	while (it != m_DrawLine.end())
	{
		rt.SetRect(it->left, it->top, it->right, it->bottom);
		dc.MoveTo(rt.left, rt.top);
		dc.LineTo(rt.right, rt.bottom);
		it++;
	}
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);

	// Draw Cross Line
	if (m_bShowCrossLine)
	{
		if(0 == m_nCrossLineType)
		{
			int i, j;
			for(i = 0; i < nImgHeight; i++)
			{
				for(j = 0; j < nImgWidth; j++)
				{
					if (i == nImgHeight / 2 && j % m_nCrossLineGab == 0)
						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);

					if (j == 140 && i % m_nCrossLineGab == 0)
						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
				}
			}
		}
		else
		{
			int i, j;
			for(i = 0; i < nImgHeight; i++)
			{
				for(j = 0; j < nImgWidth; j++)
				{
					if (j == nImgWidth / 2 && i % m_nCrossLineGab == 0)
						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);

					if(j % m_nCrossLineGab == 0)
					{
						if (i == (nImgHeight*2/6))
							dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);

						else if (i == (nImgHeight*3/6))
							dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);

						else if (i == (nImgHeight*4/6))
							dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
					}
					else
						;
				}
			}
// 
// #define DEFECT_RECT_SIZE_HALF 15
// 			int i, j;
// 			for(i = 0; i < nImgHeight; i++)
// 			{
// 				for(j = 0; j < nImgWidth; j++)
// 				{
// 					//Center
// 					if (j == nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && i >= nImgHeight / 2 - DEFECT_RECT_SIZE_HALF && i < nImgHeight / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (j == nImgWidth / 2 + DEFECT_RECT_SIZE_HALF && i >= nImgHeight / 2 - DEFECT_RECT_SIZE_HALF && i < nImgHeight / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 
// 					if (i == nImgHeight / 2 - DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (i == nImgHeight / 2 + DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					
// 					//Ref High
// 					if (j == nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && i >= nImgHeight *1/6 - DEFECT_RECT_SIZE_HALF && i < nImgHeight *1/6 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (j == nImgWidth / 2 + DEFECT_RECT_SIZE_HALF && i >= nImgHeight *1/6 - DEFECT_RECT_SIZE_HALF && i < nImgHeight *1/6 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 
// 					if (i == nImgHeight *1/6 - DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (i == nImgHeight *1/6 + DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					
// 					//Ref Low
// 					if (j == nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && i >= nImgHeight *5/6 - DEFECT_RECT_SIZE_HALF && i < nImgHeight *5/6 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (j == nImgWidth / 2 + DEFECT_RECT_SIZE_HALF && i >= nImgHeight *5/6 - DEFECT_RECT_SIZE_HALF && i < nImgHeight *5/6 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 
// 					if (i == nImgHeight *5/6 - DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					if (i == nImgHeight *5/6 + DEFECT_RECT_SIZE_HALF && j >= nImgWidth / 2 - DEFECT_RECT_SIZE_HALF && j < nImgWidth / 2 + DEFECT_RECT_SIZE_HALF)
// 						dc.FillSolidRect(j, i, 1, 1, m_ColorCrossLIne);
// 					
// 				}
// 			}
		}
	}

	// Do not call CWnd::OnPaint() for painting messages
}
void CHMBitmapWnd::ImageConvolution()
{
	if(m_pBuffer == NULL || m_nImageWidth < 0 || m_nImageHeight < 0)
		return;	
	
	if(m_pBufferConv)
		delete []m_pBufferConv;
	m_pBufferConv = NULL;

	m_pBufferConv = new BYTE[m_nImageWidth*m_nImageHeight];
	memcpy(m_pBufferConv, m_pBuffer, m_nImageWidth*m_nImageHeight);
	//처음 1/3까지
	int i, j;
	int nHeight = m_nImageHeight / 3;
	int nValue1, nValue2, nValue3;

	for(j = nHeight;j < nHeight * 2;j++)
	{
		for(i = 0;i<m_nImageWidth;i++)
		{
			nValue1 = m_pBufferConv[j*m_nImageWidth+i] - m_pBufferConv[(j-nHeight)*m_nImageWidth+i];
			nValue2 = m_pBufferConv[j*m_nImageWidth+i] - m_pBufferConv[(j+nHeight)*m_nImageWidth+i];
			nValue3 = 128 + (nValue1 + nValue2) / 2 * m_nConvPower;
			if(nValue3 < 0)
				nValue3 = 0;
			if(nValue3 > 255)
				nValue3 = 255;

			m_pBufferConv[j*m_nImageWidth+i] = nValue3;
		}
	}
}
void CHMBitmapWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	// Get the minimum and maximum scroll-bar positions.
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_LEFT:      // Scroll to far left.
		break;
	case SB_RIGHT:      // Scroll to far right.
		break;
	case SB_ENDSCROLL:   // End scroll.
		break;
	case SB_LINELEFT:      // Scroll left.
		if (m_nHScroll > 0)
			m_nHScroll--;
		break;
	case SB_LINERIGHT:   // Scroll right.
		if (m_nHScroll < m_nMaxHScroll)
			m_nHScroll++;
		break;
	case SB_PAGELEFT:    // Scroll one page left.
		if (5 < m_nMaxHScroll / 256)
			m_nHScroll -= 5;
		else
			m_nHScroll -= m_nMaxHScroll / 256;

		if (m_nHScroll < 0)
			m_nHScroll = 0;
		break;
	case SB_PAGERIGHT:      // Scroll one page right.
		if (5 < m_nMaxHScroll / 256)
			m_nHScroll += 5;
		else
			m_nHScroll += m_nMaxHScroll / 256;

		if (m_nHScroll > m_nMaxHScroll)
			m_nHScroll = m_nMaxHScroll;
		break;
	case SB_THUMBPOSITION: // Scroll to absolute position. nPos is the position
		break;
	case SB_THUMBTRACK:   // Drag scroll box to specified position. nPos is the
		m_nHScroll = nPos;
		break;
	}
	
	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_HORZ, m_nHScroll);

	if (m_hWnd)
		Invalidate();

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHMBitmapWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	// Get the current position of scroll box.
	// Determine the new position of scroll box.
	switch (nSBCode)
	{
	case SB_BOTTOM:			// Scroll to bottom. 
		break;
	case SB_ENDSCROLL:		// End scroll.
		break;
	case SB_LINEDOWN:		// Scroll one line down.
		if (m_nVScroll < m_nMaxVScroll)
			m_nVScroll++;
		break;
	case SB_LINEUP:			// Scroll one line up. 
		if (m_nVScroll > 0)
			m_nVScroll--;
		break;
	case SB_PAGEDOWN:		// Scroll one page down. 
		if (5 < m_nMaxVScroll / 256)
			m_nVScroll += 5;
		else
			m_nVScroll += (m_nMaxVScroll / 256);

		if (m_nVScroll > m_nMaxVScroll)
			m_nVScroll = m_nMaxVScroll;
		break;
	case SB_PAGEUP:			// Scroll one page up.
		if (5 < m_nMaxVScroll / 256)
			m_nVScroll -= 5;
		else
			m_nVScroll -= (m_nMaxVScroll / 256);

		if (m_nVScroll < 0)
			m_nVScroll = 0;
		break;
	case SB_THUMBPOSITION:	// Scroll to the absolute position. The current position is provided in nPos. 
		break;
	case SB_THUMBTRACK:		// Drag scroll box to specified position. The current position is provided in nPos. 
		m_nVScroll = nPos;
		break;
	case SB_TOP:			// Scroll to top. 
		break;
	}

	// Set the new position of the thumb (scroll box).
	SetScrollPos(SB_VERT, m_nVScroll);

	if (m_hWnd)
		Invalidate();

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CHMBitmapWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_pParentWnd)
		m_pParentWnd->SendMessage(WM_LBUTTONDOWN, static_cast<WPARAM>(nFlags), MAKELPARAM(point.x, point.y));

	CWnd::OnLButtonDown(nFlags, point);
}

void CHMBitmapWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_pParentWnd)
		m_pParentWnd->SendMessage(WM_MOUSEMOVE, static_cast<WPARAM>(nFlags), MAKELPARAM(point.x, point.y));

	CWnd::OnMouseMove(nFlags, point);
}

void CHMBitmapWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_pParentWnd)
		m_pParentWnd->SendMessage(WM_LBUTTONUP, static_cast<WPARAM>(nFlags), MAKELPARAM(point.x, point.y));

	CWnd::OnLButtonUp(nFlags, point);
}

BOOL CHMBitmapWnd::InitBitmap()
{
	SetShowImage(FALSE);

	CHMBitmap::InitBitmap();

	return TRUE;
}

void CHMBitmapWnd::SetShowImage(CWnd* pParentWnd, CRect* pRect, BOOL bShowImage, BOOL bModifySize)
{
	if (!m_pBuffer || !pParentWnd)
		return;

	m_bShowImage = bShowImage;

	// Display 크기 제어.
	if (bModifySize)
	{
		// Display 크기 지정, 해상도 지정.
		m_RectDisplay = *pRect;
		m_dDisplayWidthResolution = static_cast<double>(m_RectDisplay.Width()) / static_cast<double>(m_nImageWidth);
		m_dDisplayHeightResolution = static_cast<double>(m_RectDisplay.Height()) / static_cast<double>(m_nImageHeight);
	}
	else
	{
		m_RectDisplay.SetRect(pRect->left, pRect->top, pRect->left + m_nImageWidth, pRect->top + m_nImageHeight);
		m_dDisplayWidthResolution = 1.0;
		m_dDisplayHeightResolution = 1.0;
	}

	// Scroll 제어.
	CRect rect;
	pParentWnd->GetClientRect(&rect);
	// 이미지가 화면보다 폭이 작으면 스크롤 없애기.
	DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (rect.Width() + 16 < m_RectDisplay.Width())
	{
		dwStyle |= WS_HSCROLL;
		pRect->right = rect.right;
	}
	if (rect.Height() + 16 < m_RectDisplay.Height())
	{
		dwStyle |= WS_VSCROLL;
		pRect->bottom = rect.bottom;
	}

	// Window 생성.
	if (!m_hWnd)
	{
		m_pParentWnd = pParentWnd;
		if (!Create(NULL, NULL, dwStyle, *pRect, m_pParentWnd, 1001))
			return;
	}
	else
	{
		if ((int)m_pParentWnd != (int)pParentWnd)
		{
			m_pParentWnd = pParentWnd;
			SetParent(m_pParentWnd);
		}
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	MoveBitmapWindow(pRect);

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::MoveBitmapWindow(LPCRECT lpRect, BOOL bRepaint)
{
	DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

	// Is HScroll
	if (dwStyle & WS_HSCROLL)
	{
		// Set Scroll Range And Set Max Variable
		int nMaxHScroll = m_RectDisplay.Width() - (lpRect->right - lpRect->left) + 17;
		if (m_nMaxHScroll != nMaxHScroll)
		{
			m_nMaxHScroll = nMaxHScroll;
			SetScrollRange(SB_HORZ, 0, m_nMaxHScroll);
			
			m_nHScroll = 0;
			
			SetScrollPos(SB_HORZ, m_nHScroll);
		}
	}
	
	// Is VScroll
	if (dwStyle & WS_VSCROLL)
	{
		int nMaxVScroll = m_RectDisplay.Height() - (lpRect->bottom - lpRect->top) + 17;
		if (m_nMaxVScroll != nMaxVScroll)
		{
			m_nMaxVScroll = nMaxVScroll;
			SetScrollRange(SB_VERT, 0, m_nMaxVScroll);
			
			m_nVScroll = 0;

			// Init Scroll Range
			SetScrollPos(SB_VERT, m_nVScroll);
		}
	}
	
	MoveWindow(lpRect, bRepaint);
}

void CHMBitmapWnd::SetShowImage(BOOL bShowImage)
{
	m_bShowImage = bShowImage;
	if (m_hWnd)
		Invalidate();
}

CRect CHMBitmapWnd::TransDisplayToBitmap(CRect rt)
{
	CRect rtReturn(0, 0, 0, 0);

	if (m_bShowImage)
	{
		rtReturn.left = static_cast<int>(static_cast<double>(rt.left) / m_dDisplayWidthResolution);
		rtReturn.right = static_cast<int>(static_cast<double>(rt.right) / m_dDisplayWidthResolution);
		rtReturn.top = static_cast<int>(static_cast<double>(rt.top) / m_dDisplayHeightResolution);
		rtReturn.bottom = static_cast<int>(static_cast<double>(rt.bottom) / m_dDisplayHeightResolution);
	}

	return rtReturn;
}

CRect CHMBitmapWnd::TransBitmapToDisplay(CRect rt)
{
	CRect rtReturn(0, 0, 0, 0);

	if (m_bShowImage)
	{
		rtReturn.left = static_cast<int>(static_cast<double>(rt.left) * m_dDisplayWidthResolution);
		rtReturn.right = static_cast<int>(static_cast<double>(rt.right) * m_dDisplayWidthResolution);
		rtReturn.top = static_cast<int>(static_cast<double>(rt.top) * m_dDisplayHeightResolution);
		rtReturn.bottom = static_cast<int>(static_cast<double>(rt.bottom) * m_dDisplayHeightResolution);
	}

	return rtReturn;
}

//////////////////////////////////////////////////////////////////////////
// Draw Image
void CHMBitmapWnd::DrawTransRectangle(CRect& rt)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawRectangle.begin();
	while (it != m_DrawRectangle.end())
	{
		if (it->left == rt.left && it->top == rt.top && it->right == rt.right && it->bottom == rt.bottom)
			return;
		it++;
	}
	m_DrawRectangle.push_back(rt);

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseTransRectangle(CRect& rtErase)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawRectangle.begin();
	while (it != m_DrawRectangle.end())
	{
		if (it->left == rtErase.left && it->top == rtErase.top && it->right == rtErase.right && it->bottom == rtErase.bottom)
		{
			m_DrawRectangle.erase(it);
			break;
		}
		it++;
	}

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseTransRectangle()
{
	m_DrawRectangle.clear();

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::DrawTransEllipse(CRect& rt)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawRectEllipse.begin();
	while (it != m_DrawRectEllipse.end())
	{
		if (it->left == rt.left && it->top == rt.top && it->right == rt.right && it->bottom == rt.bottom)
			return;
		it++;
	}

	m_DrawRectEllipse.push_back(rt);

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseTransEllipse(CRect& rtErase)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawRectEllipse.begin();
	while (it != m_DrawRectEllipse.end())
	{
		if (it->left == rtErase.left && it->top == rtErase.top && it->right == rtErase.right && it->bottom == rtErase.bottom)
		{
			m_DrawRectEllipse.erase(it);
			break;
		}
		it++;
	}

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseTransEllipse()
{
	m_DrawRectEllipse.clear();

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::DrawLine(CRect& rt)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawLine.begin();
	while (it != m_DrawLine.end())
	{
		if (it->left == rt.left && it->top == rt.top && it->right == rt.right && it->bottom == rt.bottom)
			return;
		it++;
	}

	m_DrawLine.push_back(rt);

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseLine(CRect& rtErase)
{
	if (!m_hWnd)
		return;

	list<CRect>::iterator it;
	it = m_DrawLine.begin();
	while (it != m_DrawLine.end())
	{
		if (it->left == rtErase.left && it->top == rtErase.top && it->right == rtErase.right && it->bottom == rtErase.bottom)
		{
			m_DrawLine.erase(it);
			break;
		}
		it++;
	}

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::EraseLine()
{
	m_DrawLine.clear();

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::ShowCrossLine(BOOL bShowCrossLine, COLORREF color, int nGab)
{
	m_bShowCrossLine = bShowCrossLine;
	m_ColorCrossLIne = color;
	m_nCrossLineGab = nGab;

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::ShowProcessingImage(BOOL bProcessing, int nPower)
{
	m_bShowProcess = bProcessing;
	m_nConvPower = nPower;

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::ShowBrightImage(BOOL bShowBrightImage, int nLevel)
{
	m_bShowBrightImage = bShowBrightImage;
	m_nBrightLevel = nLevel;

	if (m_hWnd)
		Invalidate();
}

void CHMBitmapWnd::ImageBrightness(int nLevel)
{
	if(m_pBuffer == NULL || m_nImageWidth < 0 || m_nImageHeight < 0)
		return;	

	if(m_pBufferBright)
		delete []m_pBufferBright;
	m_pBufferBright = NULL;

	m_pBufferBright = new BYTE[m_nImageWidth*m_nImageHeight];
	memcpy(m_pBufferBright, m_pBuffer, m_nImageWidth*m_nImageHeight);
	
	double dScale = 255.0 / m_nBrightLevel;
	double dGray;

	for(int j = 0;j < m_nImageHeight;j++)
	{
		for(int i = 0;i<m_nImageWidth;i++)
		{
			dGray = (double)m_pBufferBright[j*m_nImageWidth+i] * dScale;
			if(dGray > 255.0)
				dGray = 255.0;
			else if(dGray < 0.0)
				dGray = 0.0;

			m_pBufferBright[j*m_nImageWidth+i] = (BYTE)dGray;
		}
	}
}