
// IMSDlg.h : 헤더 파일
//

#include <highgui.h>
#include <cv.h>
#include <opencv2/opencv.hpp>
#include "mmsystem.h"

#include <HMImages/Header/HMBitmap.h>
#include <../Include/CHImageProcess.h>
#include <../Include/CHImageData.h>

#pragma once

using namespace cv; // 필요한지 확인할것 [2021.12.13 kyj]

//#define align_4byte(bits) (((bits+31)/32*4)
#define align_4byte(in)	((in + 3)/4)*4
#define UPDATE_MSG  WM_USER + 1

enum ThreadWorking
{
	STOP = 0,
	RUNNING = 1,
	PAUSE = 2,
};

// CIMSDlg 대화 상자
class CIMSDlg : public CDialogEx
{
// 생성입니다.
public:
	CIMSDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IMS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLoadSourceImage();

	void CreateBitmapSourceImage(int w, int h, int bpp );

	Mat m_matSourceImage;

	BITMAPINFO *m_pBitmapSourceImgInfo;
	BITMAPINFO *m_pBitmapTargetImgInfo;

	CHMBitmap m_BitmapSourceImg;
	CHMBitmap m_BitmapTargetImg;

	CString m_strSourceImagePath;
	CString m_strTempImagePath;
	CString m_strResultImagePath;

	double m_dEditSetMatchRate;
	double m_dEditResultMatchRate;
	CEdit m_EditSetMatchRate;

	void Show(char *str, IplImage *img);
	int GetMatchingMethod();

	afx_msg void OnBnClickedButtonLoadTemplateImage();
	afx_msg void OnBnClickedButtonTemplateMatching();
	afx_msg void OnBnClickedButtonSourceClear();
	afx_msg void OnBnClickedButtonTargetClear();
	int m_RadioMatchingMethod;

	
	afx_msg void OnBnClickedButtonOpenOriginSourceImage();
	afx_msg void OnBnClickedButtonOpenOriginTempImage();
	afx_msg void OnBnClickedButtonOpenOriginResultImage();


	// 지울것 [2021.12.21 kyj]
	IplImage* ScreenCapture();

	bool m_bThreadStart;
	CWinThread *m_pThread;
	ThreadWorking m_ThreadWorkType;

	static UINT ThreadFunctionFirstTest(LPVOID _mothod);

	afx_msg void OnBnClickedButtonThreadStart();
	afx_msg void OnBnClickedButtonThreadStop();
	afx_msg void OnBnClickedButtonThreadPasue();
	afx_msg LRESULT OnReceivedMsgFromThread(WPARAM w, LPARAM l);

	////////////////////////////////////////////////// 여기까지 지울것 [2021.12.21 kyj]
};

struct SAlignFindParam
{
	SAlignFindParam() 
	{	
		Reset();
	}

	void Reset()
	{
		// match
		bMatchProcess			= TRUE;
		dMatchRate				= 0.8;

		// edge
		bEdgeProcess			= TRUE;
		nAlignWidth				= 100;
		nAlignHeight			= 100;
		nEdgeThreshold			= 50;
		nMergeRange				= 5;
		dEdgeRate				= 0.8;
		nKernelSize				= 5;

		// corner
		nCorner_DirType			= 0;	// lefttop, leftbottom, righttop, rightbottom 
		nCorner_SobelKernelSize = 3;
		nCorner_EdgeKernelSize	= 5;
		nCorner_EdgeThreshold	= 80;
		nCorner_IgnorePixel		= 20;

		// thickness
		bMeasureThickness		= FALSE;
		nEDThicknessLR			= 25;
		nEDThicknessTB			= 25;
		nEDThicknessRange		= 20;
	}

	// match
	BOOL	bMatchProcess;
	double	dMatchRate;

	// edge 
	BOOL	bEdgeProcess;
	int		nAlignWidth;
	int		nAlignHeight;
	int		nEdgeThreshold;
	double	dEdgeRate;
	int		nMergeRange;
	int		nKernelSize;

	// corner
	int		nCorner_DirType; 
	int		nCorner_SobelKernelSize;
	int		nCorner_EdgeKernelSize;
	int		nCorner_EdgeThreshold;
	int		nCorner_IgnorePixel;

	// thickness
	BOOL	bMeasureThickness;
	int		nEDThicknessLR;
	int		nEDThicknessTB;
	int		nEDThicknessRange;

};