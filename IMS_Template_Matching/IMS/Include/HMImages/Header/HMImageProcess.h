// HMImageProcess.h: interface for the CHMImageProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HMIMAGEPROCESS_H__85F001BC_53F6_43B6_BB51_1E1F82F46FD8__INCLUDED_)
#define AFX_HMIMAGEPROCESS_H__85F001BC_53F6_43B6_BB51_1E1F82F46FD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CHMImageProcess
{
public:
	CHMImageProcess();
	virtual ~CHMImageProcess();

	BOOL HSobelBoundary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight);
	BOOL VSobelBoundary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight);
	BOOL PrewittMasking(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight);
	BOOL LaplacianMasking(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight);

	BOOL ImageThreshold(LPBYTE lpInImg, int nWidth, int nBufWidth, int nHeight, int nThreshold);
	BOOL ImageThreshold(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight, int nThreshold);
	BOOL ImageBinary(LPBYTE lpInImg, int nWidth, int nBufWidth, int nHeight);
	BOOL ImageBinary(LPBYTE lpInImg, LPBYTE lpOutImg, int nWidth, int nBufWidth, int nHeight);
};

#endif // !defined(AFX_HMIMAGEPROCESS_H__85F001BC_53F6_43B6_BB51_1E1F82F46FD8__INCLUDED_)
