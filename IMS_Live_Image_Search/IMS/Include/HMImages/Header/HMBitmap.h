// HMBitmap.h: interface for the CHMBitmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMBITMAP_H__34D2DE80_7579_4242_A753_EB7B0BE15C3A__INCLUDED_)
#define AFX_HMBITMAP_H__34D2DE80_7579_4242_A753_EB7B0BE15C3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>

#include <algorithm>
#include <functional>      // For greater<int>( )
using namespace std;

#include "HMImageProcess.h"

#define SIZE_RGBQUAD(Size)  (Size * sizeof(RGBQUAD))
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

#ifndef DEF_WIDTHBYTES2
#define WIDTHBYTES2(bits)	((((bits) * 8) + 31) / 32 * 4)
#define DEF_WIDTHBYTES2
#endif

// Matching 결과를 저장하는 구조체.
typedef struct _CORRRESULT
{
	float	s_fCorr;
	CPoint	s_ptTemplate;

	_CORRRESULT()
	{
		Reset();
	}

	void Reset()
	{
		s_fCorr = 0.;
		s_ptTemplate.x = 0;
		s_ptTemplate.y = 0;
	}

} CorResult, *pCorResult;

class AFX_EXT_CLASS CHMBitmap : public CHMImageProcess
{
public:
	CHMBitmap();
	virtual ~CHMBitmap();
	CHMBitmap(const CHMBitmap& rhs);
	CHMBitmap& operator=(const CHMBitmap& rhs);

	// Image IO
	BOOL				InitBitmap();

	BOOL				LoadBitmapFile(CString strFilePath, BOOL bConvert = TRUE);
	BOOL				SaveBitmapFile(CString strFilePath);
	BOOL				SaveBitmapFileNoBlank(CString strFilePath);
	BOOL				SaveBitmapFile(CString strFilePath, LPBYTE lpImage, int nWidth, int nHeight);

	BOOL				SetBitmapBuffer(LPBYTE lpImage, int nSrcWidth, int nSrcBufWidth, int nSrcHeight, int nTgtWidth, int nTgtHeight, int nXStart, int nYStart);
	BOOL				SetBitmap(LPBYTE lpImage, int nWidth, int nHeight, BOOL bConvert = FALSE, BOOL bOffset = FALSE);
	BOOL				SetBlankBitmap(int nWidth, int nHeight, BYTE bDefaultColor);
	BOOL				SetBitmapPiece(LPBYTE lpImage, int nTotWidth, int nTotBufWidth, int nTotHeight, int nSize);
	BOOL				SetBitmapPiece(CHMBitmap* pBitmap, int nStartX, int nStartY);
	int					GetBitmapPieceSize()	{ return m_nWriteIdx; }
	BOOL				GetBitmapPiece(CHMBitmap* pBitmap, CRect rt);
	CString				GetFilePath()			{ return m_strFilePath; }

	// Bitmap에 접근.
	BOOL				IsBitmap()				{ return m_BitmapFileHeader.bfType == 0x4d42 ? TRUE : FALSE; }

	int					GetWidth()				{ return m_nImageWidth; }
	int					GetHeight()				{ return m_nImageHeight; }
	int					GetBufferWidth()		{ return m_nBufferWidth; }
	BITMAPINFO*			GetBitmapInfo()			{ return m_pBitmapInfo; }

	// Data에 접근.
	int					GetValue(int nXIdx, int nYIdx);
	LPBYTE				GetBuffer();
	void				SetValue(int nXIdx, int nYIdx, int nValue);
	void				ResetValue(int nValue);

	//////////////////////////////////////////////////////////////////////////
	// Image Processing
	void				GetMinMax(int &pnMax, int &pnMin, int &pnMid);
	int					GetAverage();
	int					GetAverage(BOOL bUp, int nRate);

	BOOL				ImageThreshold(int nThreshold);
	void				ImageConvertVert();

	// Pattern Matching
	CRect				TemplateMatchingMAD(CHMBitmap* pTemplate);
	CRect				TemplateMatchingNGC(CHMBitmap* pTemplate);
	CRect				TemplateMatchingNGC(CHMBitmap* pTemplate, int nRatio, double dMin);
	CRect				TemplateMatchingNGC(CHMBitmap* pTemplate, CRect RectArea, int nRatio, double dMin);
	// Ratio - 비교 픽셀 수. = 넓이 / nRatio, dMin : 상관계수의 최소값이 dMin 이하면 실패.

	// Generated message map functions
protected:
	void				AllocBitmapInfo();
	void				AllocBuffer();
	void				Reset();

	void				QuickSorting(pCorResult pfSource, UINT nArraySize);

	// 이미지 정보
	BITMAPFILEHEADER	m_BitmapFileHeader;
	BITMAPINFO*			m_pBitmapInfo;

	LPBYTE				m_pBuffer;
	LPBYTE				m_pBufferConv;
	int					m_nImageWidth;
	int					m_nBufferWidth;
	int					m_nImageHeight;
	
	// 파일 정보
	CString				m_strFilePath;
	BOOL				m_bIsSaved;

	// 부분 저장...
	int					m_nWriteIdx;

	LPBYTE				m_pBufferBright;
};

#endif // !defined(AFX_HMBITMAP_H__34D2DE80_7579_4242_A753_EB7B0BE15C3A__INCLUDED_)
