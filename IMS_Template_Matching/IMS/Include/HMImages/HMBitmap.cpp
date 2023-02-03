// HMBitmap.cpp: implementation of the CHMBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HMBitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMBitmap::CHMBitmap()
{
	m_pBuffer = NULL;
	m_pBitmapInfo = NULL;
	m_pBufferConv = NULL;
	m_pBufferBright = NULL;
	Reset();
}

CHMBitmap::~CHMBitmap()
{
	Reset();
}

CHMBitmap::CHMBitmap(const CHMBitmap& rhs)
{
	if (this != &rhs)
	{
		m_BitmapFileHeader		= rhs.m_BitmapFileHeader;
		m_pBitmapInfo			= NULL;
		AllocBitmapInfo();
		int nStructSize = sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256);
		memcpy(m_pBitmapInfo, rhs.m_pBitmapInfo, nStructSize);

		m_nImageWidth			= rhs.m_nBufferWidth;
		m_nBufferWidth			= rhs.m_nBufferWidth;
		m_nImageHeight			= rhs.m_nImageHeight;

		m_pBuffer				= NULL;
		AllocBuffer();
		memcpy(m_pBuffer, rhs.m_pBuffer, m_nBufferWidth * m_nImageHeight);

		m_strFilePath			= rhs.m_strFilePath;
		m_bIsSaved				= rhs.m_bIsSaved;

		m_nWriteIdx				= rhs.m_nWriteIdx;
	}
}

CHMBitmap& CHMBitmap::operator=(const CHMBitmap& rhs)
{
	if (this != &rhs)
	{
		m_BitmapFileHeader		= rhs.m_BitmapFileHeader;
		m_pBitmapInfo			= NULL;
		AllocBitmapInfo();
		int nStructSize = sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256);
		memcpy(m_pBitmapInfo, rhs.m_pBitmapInfo, nStructSize);

		m_nImageWidth			= rhs.m_nBufferWidth;
		m_nBufferWidth			= rhs.m_nBufferWidth;
		m_nImageHeight			= rhs.m_nImageHeight;

		m_pBuffer				= NULL;
		AllocBuffer();
		memcpy(m_pBuffer, rhs.m_pBuffer, m_nBufferWidth * m_nImageHeight);

		m_strFilePath			= rhs.m_strFilePath;
		m_bIsSaved				= rhs.m_bIsSaved;

		m_nWriteIdx				= rhs.m_nWriteIdx;
	}
	return *this;
}

void CHMBitmap::Reset()
{
	ZeroMemory(&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	if (m_pBitmapInfo)
	{
		LPBYTE lpTemp = (LPBYTE)(m_pBitmapInfo);
		if (lpTemp)
		{
			delete [] lpTemp;
			lpTemp = NULL;
		}
		
		m_pBitmapInfo = NULL;
	}

	if (m_pBuffer)
		delete [] m_pBuffer;
	m_pBuffer = NULL;

	m_nImageWidth = 0;
	m_nBufferWidth = 0;
	m_nImageHeight = 0;

	m_strFilePath.Empty();
	m_bIsSaved = FALSE;

	m_nWriteIdx = 0;

	if(m_pBufferConv)
		delete [] m_pBufferConv;
	m_pBufferConv = NULL;

	if(m_pBufferBright)
		delete [] m_pBufferBright;
	m_pBufferBright = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CHMWndImage message handlers

//////////////////////////////////////////////////////////////////////////
// 버퍼 관련 함수들.
void CHMBitmap::AllocBuffer()
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
	if (m_nBufferWidth > 0 && m_nImageHeight > 0)
	{
		m_pBuffer = new BYTE[m_nBufferWidth * m_nImageHeight + 1];
		ZeroMemory(m_pBuffer, m_nImageWidth * m_nImageHeight + 1);
	}
}


void CHMBitmap::AllocBitmapInfo()
{
	if (m_pBitmapInfo)
	{
		LPBYTE lpTemp = (LPBYTE)(m_pBitmapInfo);
		if (lpTemp)
		{
			delete [] lpTemp;
			lpTemp = NULL;
		};
		m_pBitmapInfo = NULL;
	}

	int nStructSize = 0;
	nStructSize = sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256);

	LPBYTE lpTemp = new BYTE[nStructSize + 1];
	m_pBitmapInfo = (BITMAPINFO*)lpTemp;
	ZeroMemory(m_pBitmapInfo, nStructSize + 1);
}

//////////////////////////////////////////////////////////////////////////
// Image I/O
BOOL CHMBitmap::InitBitmap()
{
	Reset();

	return TRUE;
}

BOOL CHMBitmap::SetBitmap(LPBYTE lpImage, int nWidth, int nHeight, BOOL bConvert, BOOL bOffset)
{
	if (!lpImage)
		return FALSE;

	Reset();

	if (m_nImageWidth != nWidth || m_nImageHeight != nHeight)
	{
		m_nImageWidth = nWidth;
		m_nBufferWidth = WIDTHBYTES2(nWidth);
		m_nImageHeight = nHeight;
		AllocBuffer();
	}
	else
		ZeroMemory(m_pBuffer, m_nImageWidth * m_nImageHeight + 1);

	AllocBitmapInfo();
	if (!m_pBitmapInfo)
		return FALSE;

	m_BitmapFileHeader.bfType = DIB_HEADER_MARKER;  // "BM"
	m_BitmapFileHeader.bfReserved1 = 0;
	m_BitmapFileHeader.bfReserved2 = 0;

	m_pBitmapInfo->bmiHeader.biSize = 40;
	m_pBitmapInfo->bmiHeader.biWidth = m_nImageWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_nImageHeight;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = 0;
	m_pBitmapInfo->bmiHeader.biSizeImage = (m_nBufferWidth * m_nImageHeight);
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_BitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256) + m_nBufferWidth * m_nImageHeight;
	m_BitmapFileHeader.bfOffBits = m_BitmapFileHeader.bfSize - m_nBufferWidth * m_nImageHeight;
	
	// Make Palette Data
	BYTE temp[4];
	for (int i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		memcpy(&m_pBitmapInfo->bmiColors[i], temp, 4);
	}
	
	// Data 복사
	if (m_pBuffer)
	{
		for (int i = 0; i < m_nImageHeight; i++)
			memcpy(m_pBuffer + i * m_nBufferWidth, lpImage + i * m_nImageWidth, m_nImageWidth);
	}
	else
		InitBitmap();

	return TRUE;
}

BOOL CHMBitmap::LoadBitmapFile(CString strFilePath, BOOL bConvert)
{
	BOOL bRet = TRUE;

	// Bitmap Header
	Reset();

	m_strFilePath = strFilePath;
	CFile File;
	CFileException ex;
	if (!File.Open(strFilePath, CFile::modeRead/* | CFile::shareDenyNone*/, &ex))
	{
//		TCHAR szError[1024] = {0, };
//		ex.GetErrorMessage(szError, 1024);
//		AfxMessageBox(szError);
		return FALSE;
	}

	// Read File Header
	File.Read((void*)(&m_BitmapFileHeader), sizeof(BITMAPFILEHEADER));
	// Check Is Bitmap Image
	if (!IsBitmap())
	{
		ZeroMemory(&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
		File.Close();
		return FALSE;
	}

	// Alloc BITMAPINFO and Read
	BITMAPINFOHEADER BitmapInfoHeader;
	File.Read((void*)(&BitmapInfoHeader), sizeof(BITMAPINFOHEADER));

	// Buffer Width는 4의 배수.
	m_nBufferWidth = WIDTHBYTES2(BitmapInfoHeader.biWidth);
	if (BitmapInfoHeader.biSizeImage != m_nBufferWidth * BitmapInfoHeader.biHeight)
		BitmapInfoHeader.biSizeImage = m_nBufferWidth * BitmapInfoHeader.biHeight;

	AllocBitmapInfo();
	if (!m_pBitmapInfo)
	{
		Reset();
		File.Close();
		return FALSE;
	}

	memcpy(&m_pBitmapInfo->bmiHeader, &BitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	File.Read((void*)(&m_pBitmapInfo->bmiColors), SIZE_RGBQUAD(256));
	
	// Alloc Memory
	m_nImageWidth = m_pBitmapInfo->bmiHeader.biWidth;
	m_nImageHeight = m_pBitmapInfo->bmiHeader.biHeight;

	// Read Data
	AllocBuffer();
	DWORD dwTotRead = 0, dwCurRead = 0;

	// 이미지를 버퍼에 읽어들인다.
	while (dwTotRead < m_pBitmapInfo->bmiHeader.biSizeImage)
	{
		if (m_pBitmapInfo->bmiHeader.biSizeImage - dwTotRead > 4096)
			dwCurRead = File.Read(m_pBuffer + dwTotRead, 4096);
		else
			dwCurRead = File.Read(m_pBuffer + dwTotRead, m_pBitmapInfo->bmiHeader.biSizeImage - dwTotRead);
		if (dwCurRead == 0)
			break;
		dwTotRead += dwCurRead;
	}

	File.Close();

	if (IsBitmap() && bConvert == TRUE)
		ImageConvertVert();

	return bRet;
}

BOOL CHMBitmap::SetBitmapBuffer(LPBYTE lpImage, int nSrcWidth, int nSrcBufWidth, int nSrcHeight
							  , int nTgtWidth, int nTgtHeight, int nXStart, int nYStart)
{
	if (!lpImage || nXStart + nTgtWidth > nSrcWidth || nYStart + nTgtHeight > nSrcHeight)
		return FALSE;

	Reset();

	if (m_nImageWidth != nTgtWidth || m_nImageHeight != nTgtHeight)
	{
		m_nImageWidth = nTgtWidth;
		m_nBufferWidth = WIDTHBYTES2(nTgtWidth);
		m_nImageHeight = nTgtHeight;
		AllocBuffer();
	}
	else
		ZeroMemory(m_pBuffer, m_nImageWidth * m_nImageHeight + 1);

	AllocBitmapInfo();
	if (!m_pBitmapInfo)
		return FALSE;

	m_BitmapFileHeader.bfType = DIB_HEADER_MARKER;  // "BM"
	m_BitmapFileHeader.bfReserved1 = 0;
	m_BitmapFileHeader.bfReserved2 = 0;

	m_pBitmapInfo->bmiHeader.biSize = 40;
	m_pBitmapInfo->bmiHeader.biWidth = m_nImageWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_nImageHeight;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = 0;
	m_pBitmapInfo->bmiHeader.biSizeImage = (m_nBufferWidth * m_nImageHeight);
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_BitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256) + m_nBufferWidth * m_nImageHeight;
	m_BitmapFileHeader.bfOffBits = m_BitmapFileHeader.bfSize - m_nBufferWidth * m_nImageHeight;
	
	// Make Palette Data
	BYTE temp[4];
	for (int i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		memcpy(&m_pBitmapInfo->bmiColors[i], temp, 4);
	}
	
	// Data 복사
	if (m_pBuffer)
	{
		LPBYTE lpStart = lpImage + (nYStart * nSrcBufWidth + nXStart);
		for (int i = 0; i < m_nImageHeight; i++)
			memcpy(m_pBuffer + i * m_nBufferWidth, lpStart + i * nSrcBufWidth, nTgtWidth);
	}
	else
		InitBitmap();

	return TRUE;
}

BOOL CHMBitmap::SetBlankBitmap(int nWidth, int nHeight, BYTE bDefaultColor)
{
	Reset();

	if (m_nImageWidth != nWidth || m_nImageHeight != nHeight)
	{
		m_nImageWidth = nWidth;
		m_nBufferWidth = WIDTHBYTES2(nWidth);
		m_nImageHeight = nHeight;
		AllocBuffer();
	}
	else
		ZeroMemory(m_pBuffer, m_nImageWidth * m_nImageHeight + 1);

	AllocBitmapInfo();
	if (!m_pBitmapInfo)
		return FALSE;

	m_BitmapFileHeader.bfType = DIB_HEADER_MARKER;  // "BM"
	m_BitmapFileHeader.bfReserved1 = 0;
	m_BitmapFileHeader.bfReserved2 = 0;

	m_pBitmapInfo->bmiHeader.biSize = 40;
	m_pBitmapInfo->bmiHeader.biWidth = m_nImageWidth;
	m_pBitmapInfo->bmiHeader.biHeight = m_nImageHeight;
	m_pBitmapInfo->bmiHeader.biPlanes = 1;
	m_pBitmapInfo->bmiHeader.biBitCount = 8;
	m_pBitmapInfo->bmiHeader.biCompression = 0;
	m_pBitmapInfo->bmiHeader.biSizeImage = (m_nBufferWidth * m_nImageHeight);
	m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2923;
	m_pBitmapInfo->bmiHeader.biClrUsed = 0;
	m_pBitmapInfo->bmiHeader.biClrImportant = 0;

	m_BitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256) + m_nBufferWidth * m_nImageHeight;
	m_BitmapFileHeader.bfOffBits = m_BitmapFileHeader.bfSize - m_nBufferWidth * m_nImageHeight;
	
	// Make Palette Data
	BYTE temp[4];
	int i;
	for (i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		memcpy(&m_pBitmapInfo->bmiColors[i], temp, 4);
	}

	memset(m_pBuffer, bDefaultColor, m_nBufferWidth * m_nImageHeight + 1);

	return TRUE;
}

BOOL CHMBitmap::SetBitmapPiece(LPBYTE lpImage, int nTotWidth, int nTotBufWidth, int nTotHeight, int nSize)
{
	if (m_nImageWidth != nTotWidth || m_nImageHeight != nTotHeight || m_nBufferWidth != nTotBufWidth || !m_pBuffer)
	{
		Reset();

		m_nImageWidth = nTotWidth;
		m_nBufferWidth = WIDTHBYTES2(nTotWidth);
		m_nImageHeight = nTotHeight;

		AllocBitmapInfo();
		if (!m_pBitmapInfo)
			return FALSE;

		m_BitmapFileHeader.bfReserved1 = 0;
		m_BitmapFileHeader.bfReserved2 = 0;

		m_pBitmapInfo->bmiHeader.biSize = 40;
		m_pBitmapInfo->bmiHeader.biWidth = m_nImageWidth;
		m_pBitmapInfo->bmiHeader.biHeight = m_nImageHeight;
		m_pBitmapInfo->bmiHeader.biPlanes = 1;
		m_pBitmapInfo->bmiHeader.biBitCount = 8;
		m_pBitmapInfo->bmiHeader.biCompression = 0;
		m_pBitmapInfo->bmiHeader.biSizeImage = (m_nBufferWidth * m_nImageHeight);
		m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 2923;
		m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 2923;
		m_pBitmapInfo->bmiHeader.biClrUsed = 0;
		m_pBitmapInfo->bmiHeader.biClrImportant = 0;

		m_BitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256) + m_nBufferWidth * m_nImageHeight;
		m_BitmapFileHeader.bfOffBits = m_BitmapFileHeader.bfSize - m_nBufferWidth * m_nImageHeight;
		
		// Make Palette Data
		BYTE temp[4];
		for (int i = 0; i < 256; i++)
		{
			ZeroMemory(temp, 4);
			temp[0] = i;
			temp[1] = i;
			temp[2] = i;
			memcpy(&m_pBitmapInfo->bmiColors[i], temp, 4);
		}
		
		// Data 복사
		AllocBuffer();
	}

	if (m_pBuffer)
	{
		if (!IsBitmap())
		{
			memcpy(m_pBuffer + m_nWriteIdx, lpImage, nSize);
			m_nWriteIdx += nSize;
			if (m_nWriteIdx >= m_nBufferWidth * m_nImageHeight)
				m_BitmapFileHeader.bfType = DIB_HEADER_MARKER;  // "BM"
		}
	}
	else
		InitBitmap();

	return TRUE;
}

BOOL CHMBitmap::SetBitmapPiece(CHMBitmap* pBitmap, int nStartX, int nStartY)
{
	if (!pBitmap || !pBitmap->IsBitmap() || !IsBitmap())
		return FALSE;

	LPBYTE lpBuffer = pBitmap->GetBuffer();
	int nXStart, nXEnd, nWidth, nXGab = 0;
	for (int y = 0; y < pBitmap->GetHeight(); y++)
	{
		if (nStartY + y < 0 || nStartY + y >= m_nImageHeight)
			continue;

		nXStart = nStartX;
		nXEnd = nStartX + pBitmap->GetWidth();
		if (nStartX < 0)
		{
			nXStart = 0;
			nXGab = nStartX * -1;
		}
		if (nXEnd > m_nImageWidth)
			nXEnd = m_nImageWidth;
		nWidth = nXEnd - nXStart;
		memcpy(m_pBuffer + ((nStartY + y) * m_nBufferWidth + nXStart), lpBuffer + y * pBitmap->GetBufferWidth() + nXGab, nWidth);
	}
	
	return TRUE;
}

BOOL CHMBitmap::GetBitmapPiece(CHMBitmap* pBitmap, CRect rt)
{
	if (!pBitmap || rt.Width() <= 0 || rt.Height() <= 0)
		return FALSE;

	// 이미지 크기 조절.
	if (rt.left < 0 || rt.right > m_nImageWidth)
		return FALSE;
	if (rt.top < 0 || rt.bottom > m_nImageHeight)
		return FALSE;

	// 임시 버퍼 생성.
	int nDestImageWidth = rt.Width();
	int nDestImageHeight = rt.Height();
	LPBYTE lpDest = new BYTE[nDestImageWidth * nDestImageHeight + 1];
	ZeroMemory(lpDest, sizeof(BYTE) * nDestImageWidth * nDestImageHeight + 1);

	// 이미지 일부 복사.
	for (int i = rt.top; i < rt.bottom; i++)
		memcpy(lpDest + ((i - rt.top) * nDestImageWidth), m_pBuffer + (i * m_nBufferWidth) + rt.left, sizeof(BYTE) * nDestImageWidth);

	// 개체에 넣기.
	pBitmap->SetBitmap(lpDest, nDestImageWidth, nDestImageHeight);

	if (lpDest)
	{
		delete [] lpDest;
		lpDest = NULL;
	}


	return TRUE;
}

BOOL CHMBitmap::SaveBitmapFile(CString strFilePath)
{
	if (!m_pBitmapInfo || !m_pBuffer)
		return FALSE;

	// Display Buffer
	CFile File;
	int nIdx = strFilePath.ReverseFind('\\');
	if (nIdx == -1)
		nIdx = strFilePath.ReverseFind('/');
	CString strPath = strFilePath.Left(nIdx);
	if (!strPath.IsEmpty())
		CreateDirectory(strPath, NULL);

	if (!File.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return FALSE;

	// 비트맵 파일 헤더를 파일에 쓰기
	File.Write((LPSTR)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	File.Write((LPSTR)&m_pBitmapInfo->bmiHeader , sizeof(BITMAPINFOHEADER) );

	// Make Palette Data
	BYTE temp[4];
	int i;
	for (i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		File.Write(temp, 4);
	}

	// 나머지 데이터를 파일에 쓰기
	for (i = 0; i < m_nImageHeight; i++)
		File.Write(m_pBuffer + (m_nImageHeight - i - 1) * m_nBufferWidth, m_nBufferWidth);

	File.Close();

	return TRUE;
}

BOOL CHMBitmap::SaveBitmapFileNoBlank(CString strFilePath)
{
	if (!m_pBitmapInfo || !m_pBuffer)
		return FALSE;

	// Display Buffer
	CFile File;
	int nIdx = strFilePath.ReverseFind('\\');
	if (nIdx == -1)
		nIdx = strFilePath.ReverseFind('/');
	CString strPath = strFilePath.Left(nIdx);
	if (!strPath.IsEmpty())
		CreateDirectory(strPath, NULL);

	if (!File.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return FALSE;

	// 비트맵 파일 헤더를 파일에 쓰기
	File.Write((LPSTR)&m_BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	File.Write((LPSTR)&m_pBitmapInfo->bmiHeader , sizeof(BITMAPINFOHEADER) );

	// Make Palette Data
	BYTE temp[4];
	int i;
	for (i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		File.Write(temp, 4);
	}

	// 나머지 데이터를 파일에 쓰기
	for (i = 0; i < m_nImageHeight; i++)
		File.Write(m_pBuffer + (m_nImageHeight - i - 1) * m_nImageWidth, m_nBufferWidth);

	File.Close();

	return TRUE;
}

BOOL CHMBitmap::SaveBitmapFile(CString strFilePath, LPBYTE lpImage, int nWidth, int nHeight)
{
	if (!lpImage)
		return FALSE;
	
	BITMAPFILEHEADER	BitmapFileHeader;
	BITMAPINFOHEADER	BitmapInfoHeader;
	RGBQUAD*			pRGBQuad = NULL;
	int					nBufferWidth = WIDTHBYTES2(nWidth);

	BitmapFileHeader.bfType = DIB_HEADER_MARKER;  // "BM"
	BitmapFileHeader.bfReserved1 = 0;
	BitmapFileHeader.bfReserved2 = 0;

	BitmapInfoHeader.biSize = 40;
	BitmapInfoHeader.biWidth = nWidth;
	BitmapInfoHeader.biHeight = nHeight;
	BitmapInfoHeader.biPlanes = 1;
	BitmapInfoHeader.biBitCount = 8;
	BitmapInfoHeader.biCompression = 0;
	BitmapInfoHeader.biSizeImage = (nBufferWidth * nHeight);
	BitmapInfoHeader.biXPelsPerMeter = 2923;
	BitmapInfoHeader.biYPelsPerMeter = 2923;
	BitmapInfoHeader.biClrUsed = 0;
	BitmapInfoHeader.biClrImportant = 0;

	BitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + SIZE_RGBQUAD(256) + nBufferWidth * nHeight;
	BitmapFileHeader.bfOffBits = BitmapFileHeader.bfSize - nBufferWidth * nHeight;

	// Display Buffer
	CFile File;
	CString strPath = strPath.Left(strFilePath.ReverseFind('\\'));
	CreateDirectory(strPath, NULL);
	
	if (!File.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
		return FALSE;
	
	// 비트맵 파일 헤더를 파일에 쓰기
	File.Write((LPSTR)&BitmapFileHeader, sizeof(BITMAPFILEHEADER));
	File.Write((LPSTR)&BitmapInfoHeader , sizeof(BITMAPINFOHEADER) );
	
	// Make Palette Data
	BYTE temp[4];
	for (int i = 0; i < 256; i++)
	{
		ZeroMemory(temp, 4);
		temp[0] = i;
		temp[1] = i;
		temp[2] = i;
		File.Write(temp, 4);
	}

	// 나머지 데이터를 파일에 쓰기
	DWORD dwTotWrite = 0, dwCurWrite = 0;
	int nWrite = 0;
	while (dwTotWrite < m_pBitmapInfo->bmiHeader.biSizeImage)
	{
		if (m_pBitmapInfo->bmiHeader.biSizeImage - dwTotWrite > 4096)
			nWrite = 4096;
		else
			nWrite = m_pBitmapInfo->bmiHeader.biSizeImage - dwTotWrite;
		
		File.Write(m_pBuffer + dwTotWrite, nWrite);

		dwTotWrite += nWrite;
	}

	File.Close();

	if (pRGBQuad)
	{
		LPBYTE lpTemp = (LPBYTE)(pRGBQuad);
		if (lpTemp)
		{
			delete [] lpTemp;
			lpTemp = NULL;
		}
		pRGBQuad = NULL;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 점에 접근
void CHMBitmap::SetValue(int nXIdx, int nYIdx, int nValue)
{
	if (nValue < 0)
		nValue = -nValue;
	
	if (m_pBuffer && nXIdx >= 0 && m_nImageWidth > nXIdx && nYIdx >= 0 && m_nImageHeight > nYIdx)
		m_pBuffer[nYIdx * m_nBufferWidth + nXIdx] = (BYTE)(nValue > 255 ? 255 : nValue);
}

void CHMBitmap::ResetValue(int nValue)
{
	if (m_pBuffer)
		memset(m_pBuffer, nValue, m_nBufferWidth * m_nImageHeight);
}

// Bitmap Data에 접근
int CHMBitmap::GetValue(int nXIdx, int nYIdx)
{
	if (m_pBuffer && nXIdx >= 0 && m_nImageWidth > nXIdx && nYIdx >= 0 && m_nImageHeight > nYIdx)
		return static_cast<int>(m_pBuffer[nYIdx * m_nBufferWidth + nXIdx]);
	else
		return -1;
}

LPBYTE CHMBitmap::GetBuffer()
{
	if (m_pBuffer)
		return m_pBuffer;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// Image Processing
void CHMBitmap::GetMinMax(int &pnMax, int &pnMin, int &pnMid)
{
	int nValue;
	int nMin = 256, nMax = -1, nMid = -1;
	for (int i = 0; i < m_nImageHeight; i++)
	{
		for (int j = 0; j < m_nImageWidth; j++)
		{
			nValue = m_pBuffer[i * m_nBufferWidth + j];
			if (nValue > nMax)
				nMax = nValue;
			if (nValue < nMin)
				nMin = nValue;
		}
	}
	pnMin = nMin;
	pnMax = nMax;
	pnMid = (nMax + nMin) / 2;
}

BOOL CHMBitmap::ImageThreshold(int nThreshold)
{
	CHMImageProcess::ImageThreshold(m_pBuffer, m_nImageWidth, m_nBufferWidth, m_nImageHeight, nThreshold);

	return TRUE;
}

int CHMBitmap::GetAverage()
{
	if (!IsBitmap())
		return -1;

	int nSum = 0, i, j;
	for (i = 0; i < m_nImageHeight; i++)
	{
		for (j = 0; j < m_nImageWidth; j++)
		{
			nSum += m_pBuffer[i * m_nBufferWidth + j];
		}
	}

	return nSum / (m_nImageHeight * m_nImageWidth);
}

int CHMBitmap::GetAverage(BOOL bUp, int nRate)
{
	if (!IsBitmap())
		return -1;
	
	int i, j;
	int nArrColor[256];
	for (i = 0; i < 256; i++)
		nArrColor[i] = 0;

	// Color별로 개수를 구한다.
	for (i = 0; i < m_nImageHeight; i++)
	{
		for (j = 0; j < m_nImageWidth; j++)
			nArrColor[m_pBuffer[i * m_nBufferWidth + j]]++;
	}

	// Rate만큼을 구하고 그 Sum을 구한다.
	int nPixel = static_cast<int>((m_nImageWidth * m_nImageHeight) * (static_cast<double>(nRate) / 100.0));
	int nCountSum = 0, nSum = 0;
	if (bUp)
	{
		for (i = 255; i >= 0; i--)
		{
			if (0 != nArrColor[i])
			{
				if (nCountSum + nArrColor[i] <= nPixel)
				{
					nCountSum += nArrColor[i];
					nSum += (nArrColor[i] * i);
				}
				else
				{
					int nCount = nPixel - nCountSum;
					nCountSum += nCount;
					nSum += (nCount * i);
				}
			}
		}
	}
	else
	{
		for (i = 0; i < 256; i++)
		{
			if (0 != nArrColor[i])
			{
				if (nCountSum + nArrColor[i] <= nPixel)
				{
					nCountSum += nArrColor[i];
					nSum += (nArrColor[i] * i);
				}
				else
				{
					int nCount = nPixel - nCountSum;
					nCountSum += nCount;
					nSum += (nCount * i);
				}
			}
		}
	}
	
	return nSum / nCountSum;
}

void CHMBitmap::ImageConvertVert()
{
	LPBYTE lpBuffer = new BYTE[m_nBufferWidth * m_nImageHeight];

	memcpy(lpBuffer, m_pBuffer, m_nBufferWidth * m_nImageHeight);

	for (int i = 0; i < m_nImageHeight; i++)
		memcpy(m_pBuffer + i * m_nBufferWidth, lpBuffer + (m_nImageHeight - i - 1) * m_nBufferWidth, m_nBufferWidth);

	if (lpBuffer)
	{
		delete [] lpBuffer;
		lpBuffer = NULL;
	}

}

CRect CHMBitmap::TemplateMatchingMAD(CHMBitmap* pTemplate)
{
	CRect RectResult(-1, -1, -1, -1);
	if (!pTemplate || !pTemplate->IsBitmap())
		return RectResult;

	register int i, j, m, n;

	int nTempWidth = pTemplate->GetWidth();
	int nTempHeight = pTemplate->GetHeight();
	int nTempBufferWidth = pTemplate->GetBufferWidth();
	LPBYTE lpTemp = pTemplate->GetBuffer();

	float SumD, fValue, fMinCorr = 255.0f * nTempHeight * nTempWidth;

	for (m = 0; m < m_nImageHeight - nTempHeight; m++)
	{
		for (n = 0; n < m_nImageWidth - nTempWidth; n++)
		{
			SumD = 0.0f;
			for (i = 0; i < nTempHeight; i++)
			{
				for (j = 0; j < nTempWidth; j++)
				{
					fValue = (float)(m_pBuffer[(m + i) * m_nBufferWidth + (n + j)] - lpTemp[i * nTempBufferWidth + j]);
					if (fValue < 0)
						fValue *= -1;
					SumD += fValue;
				}
			}
			if (SumD < fMinCorr)
			{
				fMinCorr = SumD;
				RectResult.left = n;
				RectResult.top = m;
				RectResult.right = n + nTempWidth;
				RectResult.bottom = m + nTempHeight;
			}
		}
	}
	fMinCorr /= (float)(nTempWidth * nTempHeight);

	return RectResult;
}

CRect CHMBitmap::TemplateMatchingNGC(CHMBitmap* pTemplate, int nRatio, double dMin)
{
	CRect RectResult(-1, -1, -1, -1);
	if (!pTemplate || !pTemplate->IsBitmap() || pTemplate->GetWidth() >= m_nImageWidth || pTemplate->GetHeight() >= m_nImageHeight)
		return RectResult;
	if (nRatio <= 0)
		return RectResult;
	if (dMin < 0 || dMin > 1)
		return RectResult;

	int nTempAverage = pTemplate->GetAverage();
	int nTempWidth = pTemplate->GetWidth();
	int nTempHeight = pTemplate->GetHeight();
	int nTempBufferWidth = pTemplate->GetBufferWidth();
	LPBYTE lpTemp = pTemplate->GetBuffer();
	if (nTempWidth < 10 || nTempHeight < 10)
		return RectResult;

	float ST = 0.0f, temp;
	float SumT = 0.0f;
	register int m, n, i, j;

	//////////////////////////////////////////////////////////////////////////
	// 임의의 점의 개수를 계산.
	int nPixelCountWidth = nTempWidth / nRatio;
	int nPixelCountHeight = nTempHeight / nRatio;
	if (nPixelCountWidth < 1 || nPixelCountHeight < 1)
		return RectResult;

	int NoT = nPixelCountWidth * nPixelCountHeight;

	// 균일하게 NoT만큼 비교.
	int nGabWidth = nTempWidth / (nPixelCountWidth + 1);
	int nGabHeight = nTempHeight / (nPixelCountHeight + 1);
//	TRACE("width : %d, height : %d, width gab : %d, height gab : %d, NoT - %d\n", nTempWidth, nTempHeight, nGabWidth, nGabHeight, NoT);
	for (i = 1; i <= nPixelCountHeight; i++)
	{
		for(j = 1; j <= nPixelCountWidth; j++)
		{
			m = i * nGabHeight;
			n = j * nGabWidth;

			temp = (float)lpTemp[m * nTempBufferWidth + n];
			SumT += temp;
			ST += temp * temp;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 결과를 저장할 변수
	pCorResult	pfResult;
	pfResult = new CorResult[(m_nImageHeight - nTempHeight) * (m_nImageWidth - nTempWidth)];

	// 변수 선언 및 설정
	float MaxCorr = 0.0f, CurCorr, a1, a2, DeNomi, Nomi;
	float SumI, SI, ScT, tpi;

	DWORD dwTick = GetTickCount();
	// 탐색루프의 시작
	int nH, nV;
	for (m = 0; m < m_nImageHeight - nTempHeight; m++)
	{
		for (n = 0; n < m_nImageWidth - nTempWidth; n++)
		{
			SumI = SI = ScT = 0.0f;
			for (i = 1; i <= nPixelCountHeight; i++)
			{
				for(j = 1; j <= nPixelCountWidth; j++)
				{
					nH = i * nGabHeight;
					nV = j * nGabWidth;

					temp = (float)m_pBuffer[(m + nH) * m_nBufferWidth + (n + nV)];
					tpi = (float)lpTemp[nH * nTempBufferWidth + nV];
					SumI += temp;
					SI += temp * temp;
					ScT += tpi * temp;
				}
			}

			a1 = NoT * SI - SumI * SumI;
			a2 = NoT * ST - SumT * SumT;
			DeNomi = (float)(a1 * a2);
			Nomi = (NoT * ScT - SumI * SumT);

			if (DeNomi < 0.0001)
				CurCorr = 0;
			else
				CurCorr = Nomi * Nomi / DeNomi;

			pfResult[m * (m_nImageWidth - nTempWidth) + n].s_fCorr = CurCorr;
			pfResult[m * (m_nImageWidth - nTempWidth) + n].s_ptTemplate.x = n;
			pfResult[m * (m_nImageWidth - nTempWidth) + n].s_ptTemplate.y = m;
		}
	}

	QuickSorting(pfResult, (m_nImageHeight - nTempHeight) * (m_nImageWidth - nTempWidth));

	//////////////////////////////////////////////////////////////////////////
	// 상위 30개에 대해 상세 풀 매칭.
	// 템플레이트에 대해 미리 계산할 식은 계산해 놓는다.
	for (i = 0; i < nTempHeight; i++)
	{
		for (j = 0; j < nTempWidth; j++)
		{
			temp = (float)lpTemp[i * nTempBufferWidth + j];
			SumT += temp;
			ST += temp * temp;
			NoT++;
		}
	}

	int nMatchSum, nMatchAvg;
	for (int k = 0; k < 30; k++)
	{
		m = pfResult[k].s_ptTemplate.y;
		n = pfResult[k].s_ptTemplate.x;
		
		// 탐색루프의 시작
		SumI = SI = ScT = 0.0f;
		nMatchSum = 0;
		// 이미지의 전체를 비교.
		for (i = 0; i < nTempHeight; i++)
		{
			for (j = 0; j < nTempWidth; j++)
			{
				temp = (float)m_pBuffer[(m + i) * m_nBufferWidth + (n + j)];
				nMatchSum += static_cast<int>(temp);

				tpi = (float)lpTemp[i * nTempBufferWidth + j];
				SumI += temp;
				SI += temp * temp;
				ScT += tpi * temp;
			}
		}
		nMatchAvg = nMatchSum / (nTempWidth * nTempHeight);

		a1 = NoT * SI - SumI * SumI;
		a2 = NoT * ST - SumT * SumT;
		DeNomi = (float)(a1 * a2);
		Nomi = (NoT * ScT - SumI * SumT);
		
		if (DeNomi < 0.0001)
			CurCorr = 0;
		else
			CurCorr = Nomi * Nomi / DeNomi;

		// 밝기 평균으로 CurCorr를 보상한다.
		CurCorr = CurCorr + static_cast<float>((fabs((nTempAverage - nMatchAvg) / 255.0)) / 2.0);

		if (CurCorr > MaxCorr)
		{
			MaxCorr = CurCorr;
			RectResult.left = n;
			RectResult.top = m;
			RectResult.right = n + nTempWidth;
			RectResult.bottom = m + nTempHeight;
		}
	}

	if (pfResult)
	{
		delete [] pfResult;
		pfResult = NULL;
	}


//	int nTop, nBottom;
//	nBottom = m_nImageHeight - RectResult.top;
//	nTop = m_nImageHeight - RectResult.bottom;
//	RectResult.top = nTop;
//	RectResult.bottom = nBottom;

	if (MaxCorr < dMin)
		RectResult.SetRect(-1, -1, -1, -1);

	return RectResult;
}

CRect CHMBitmap::TemplateMatchingNGC(CHMBitmap* pTemplate, CRect RectArea, int nRatio, double dMin)
{
	CHMBitmap BmpPeace;
	this->GetBitmapPiece(&BmpPeace, RectArea);
	BmpPeace.SaveBitmapFile("NGCPiece.bmp");

	CRect RectResult;
	RectResult = BmpPeace.TemplateMatchingNGC(pTemplate, nRatio, dMin);
	pTemplate->SaveBitmapFile("NGCTemplate.bmp");

	if (RectResult.left != -1 && RectResult.top != -1 && RectResult.right != -1 && RectResult.bottom)
	{
		RectResult.left += RectArea.left;
		RectResult.top += RectArea.top;
		RectResult.right += RectArea.left;
		RectResult.bottom += RectArea.top;
	}

	return RectResult;
}

CRect CHMBitmap::TemplateMatchingNGC(CHMBitmap* pTemplate)
{
	return TemplateMatchingNGC(pTemplate, 10, 0.3);
}

//이분 분할을 이용한 퀵 정렬
void CHMBitmap::QuickSorting(pCorResult pfSource, UINT nArraySize)
{
	int top;
	int stack[10000];

	UINT v;
	CorResult transTemp, compTemp;
	int i, j;
	UINT nLeft = 0, nRight = nArraySize - 1;
	top = -1;
	stack[++top] = nRight;
	stack[++top] = nLeft;

	while (top >= 0)
	{
		nLeft = stack[top--];
		nRight = stack[top--];
		if (nRight - nLeft + 1 > 1)    // terminal condition
		{
			v = (nRight + nLeft) / 2;
			transTemp = pfSource[v];     // exchange partition with rightmost
			pfSource[v] = pfSource[nRight];
			pfSource[nRight] = transTemp;
			compTemp = transTemp;
			i = nLeft - 1;
			j = nRight;

			while (1)      /* partition */
			{
				while (pfSource[++i].s_fCorr > compTemp.s_fCorr && static_cast<UINT>(i) < nArraySize - 1);
				while (pfSource[--j].s_fCorr < compTemp.s_fCorr && j > 0);
				if (i >= j)
					break;

				transTemp = pfSource[i];
				pfSource[i] = pfSource[j];
				pfSource[j] = transTemp;
			}

			transTemp = pfSource[i];
			pfSource[i] = pfSource[nRight];
			pfSource[nRight] = transTemp;
			stack[++top] = nRight;
			stack[++top] = i+1;
			stack[++top] = i-1;
			stack[++top] = nLeft;
		}
	}
}

