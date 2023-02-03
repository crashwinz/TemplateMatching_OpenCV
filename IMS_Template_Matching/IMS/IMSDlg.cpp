
// IMSDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "IMS.h"
#include "IMSDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIMSDlg 대화 상자




CIMSDlg::CIMSDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIMSDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_dEditSetMatchRate = 0.5;
	m_dEditResultMatchRate = 0;
	m_RadioMatchingMethod = 5;
}

void CIMSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MATCHRATE_SET_VALUE, m_dEditSetMatchRate);
	DDX_Text(pDX, IDC_EDIT_MATCHRATE_RESULT_VALUE, m_dEditResultMatchRate);
	DDX_Control(pDX, IDC_EDIT_MATCHRATE_SET_VALUE, m_EditSetMatchRate);
	DDX_Radio(pDX, IDC_RADIO_METHOD_1, m_RadioMatchingMethod);
}

BEGIN_MESSAGE_MAP(CIMSDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_SOURCE_IMAGE, &CIMSDlg::OnBnClickedButtonLoadSourceImage)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_TEMPLATE_IMAGE, &CIMSDlg::OnBnClickedButtonLoadTemplateImage)
	ON_BN_CLICKED(IDC_BUTTON_TEMPLATE_MATCHING, &CIMSDlg::OnBnClickedButtonTemplateMatching)
	ON_BN_CLICKED(IDC_BUTTON_SOURCE_CLEAR, &CIMSDlg::OnBnClickedButtonSourceClear)
	ON_BN_CLICKED(IDC_BUTTON_TARGET_CLEAR, &CIMSDlg::OnBnClickedButtonTargetClear)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ORIGIN_SOURCE_IMAGE, &CIMSDlg::OnBnClickedButtonOpenOriginSourceImage)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ORIGIN_TEMP_IMAGE, &CIMSDlg::OnBnClickedButtonOpenOriginTempImage)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ORIGIN_RESULT_IMAGE, &CIMSDlg::OnBnClickedButtonOpenOriginResultImage)
	ON_BN_CLICKED(IDC_BUTTON_THREAD_START, &CIMSDlg::OnBnClickedButtonThreadStart)
	ON_BN_CLICKED(IDC_BUTTON_THREAD_STOP, &CIMSDlg::OnBnClickedButtonThreadStop)
	ON_BN_CLICKED(IDC_BUTTON_THREAD_PASUE, &CIMSDlg::OnBnClickedButtonThreadPasue)
	ON_MESSAGE (UPDATE_MSG, OnReceivedMsgFromThread)


END_MESSAGE_MAP()


// CIMSDlg 메시지 처리기

BOOL CIMSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	m_pBitmapSourceImgInfo = NULL;
	m_pBitmapTargetImgInfo = NULL;

	m_strSourceImagePath = "";
	m_strTempImagePath = "";
	m_strResultImagePath = "";

	m_ThreadWorkType = STOP;
	m_pThread = NULL;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CIMSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CIMSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CIMSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CIMSDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	// 엔터키 ESC키 눌렀을 떄 종료되는 상황 삭제
	if (WM_KEYDOWN == pMsg->message)
	{
		if(VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CIMSDlg::OnBnClickedButtonLoadSourceImage()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));
	
	BYTE* pTemp = NULL;
	int nTempWidth = 0, nTempHeight = 0;

	if(fileDlg.DoModal() == IDOK)
	{
		m_strSourceImagePath = fileDlg.GetPathName();
		//m_BitmapSourceImg.LoadBitmapFile(m_strSourceImagePath);
		//if(!(m_BitmapSourceImg.IsBitmap()))
		//{
		//	AfxMessageBox(_T("Is not Bitmap Image"));
		//	return;
		//}

		//pTemp = m_BitmapSourceImg.GetBuffer();
		//nTempWidth = m_BitmapSourceImg.GetWidth();
		//nTempHeight = m_BitmapSourceImg.GetHeight();

		//m_pBitmapSourceImgInfo = NULL;
		//m_pBitmapSourceImgInfo = m_BitmapSourceImg.GetBitmapInfo(); // 여기서 비트맵 정보를 얻게 된다.

		CClientDC dc(GetDlgItem(IDC_PICTURE_CONTROL_SOURCE_IMAGE));

		CRect rect;
		GetDlgItem(IDC_PICTURE_CONTROL_SOURCE_IMAGE)->GetClientRect(&rect);

		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);

		CImage image;
		image.Load(m_strSourceImagePath);

		image.StretchBlt(dc.m_hDC,0,0,rect.Width(),rect.Height(),SRCCOPY);
	}
}

void CIMSDlg::OnBnClickedButtonLoadTemplateImage()
{
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));

	BYTE* pTemp = NULL;
	int nTempWidth = 0, nTempHeight = 0;

	if(fileDlg.DoModal() == IDOK)
	{
		m_strTempImagePath = fileDlg.GetPathName();
		//m_BitmapTargetImg.LoadBitmapFile(m_strTargetImagePath);
		//if(!(m_BitmapTargetImg.IsBitmap()))
		//{
		//	AfxMessageBox(_T("Is not Bitmap Image"));
		//	return;
		//}

		//pTemp = m_BitmapTargetImg.GetBuffer();
		//nTempWidth = m_BitmapTargetImg.GetWidth();
		//nTempHeight = m_BitmapTargetImg.GetHeight();

		//m_pBitmapTargetImgInfo = NULL;
		//m_pBitmapTargetImgInfo = m_BitmapTargetImg.GetBitmapInfo(); // 여기서 비트맵 정보를 얻게 된다.

		CClientDC dc(GetDlgItem(IDC_PICTURE_CONTROL_TEMPLATE_IMAGE));

		CRect rect;
		GetDlgItem(IDC_PICTURE_CONTROL_TEMPLATE_IMAGE)->GetClientRect(&rect);

		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);

		CImage image;
		image.Load(m_strTempImagePath);

		image.StretchBlt(dc.m_hDC,0,0,rect.Width(),rect.Height(),SRCCOPY);
	}
}


void CIMSDlg::OnBnClickedButtonTemplateMatching()
{
	if(m_strSourceImagePath == "")
	{
		AfxMessageBox("Source Image Empty!!!");
		return;
	}

	if(m_strTempImagePath == "")
	{
		AfxMessageBox("Target Image Empty!!!");
		return;
	}

	double dmin, dmax;
	CvPoint left_top;
	CString strTemp;

	// Source, Temp 이미지 로드
	IplImage *src = cvLoadImage(m_strSourceImagePath, -1);
	IplImage *temp = cvLoadImage(m_strTempImagePath, -1);

	if(src->width < temp->width) // 소스이미지의 Width가 더 커야한다. 그렇지 않으면 프로그램 다운 발생.
	{
		AfxMessageBox("Temp Image's Width bigger than Source Image's Width");
		return;
	}

	if(src->height < temp->height) // 소스이미지의 Height가 더 커야한다. 그렇지 않으면 프로그램 다운 발생.
	{
		AfxMessageBox("Temp Image's Height bigger than Source Image's Height");
		return;
	}

	// 상관계수를 구할 이미지
	IplImage *coeff = cvCreateImage( cvSize( src->width - temp->width+1, src->height - temp->height+1 ), IPL_DEPTH_32F, 1);

	if(src->nChannels != temp->nChannels) // 소스이미지가 8비트인데 탬플릿매칭이미지가 24비트 이런 경우 프로그램 다운 발생 예외처리 [2021.12.14 kyj]
	{
		AfxMessageBox("Source and Template Image Type(bit) is not same!");
		return;
	}

	int nMatchingMethod = GetMatchingMethod();

	// 상관계수를 구하여 coeff에 그려줌.
	cvMatchTemplate(src, temp, coeff, nMatchingMethod);
	/*
	CV_TM_SQDIFF        =0
	CV_TM_SQDIFF_NORMED =1
	CV_TM_CCORR         =2
	CV_TM_CCORR_NORMED  =3
	CV_TM_CCOEFF        =4
	CV_TM_CCOEFF_NORMED =5
	*/

	// 상관계수가 최대값을 가지는 위치를 찾는다.
	cvMinMaxLoc(coeff, &dmin, &dmax, NULL, &left_top);

	m_EditSetMatchRate.GetWindowTextA(strTemp);
	m_dEditResultMatchRate = atof(strTemp);

	strTemp.Format("%.3f", dmax);
	SetDlgItemText(IDC_EDIT_MATCHRATE_RESULT_VALUE, strTemp);

	/*if(nMatchingMethod != CV_TM_SQDIFF && nMatchingMethod != CV_TM_CCORR && nMatchingMethod != CV_TM_CCOEFF)
	{
		strTemp.Format("%.3f", dmax);
		SetDlgItemText(IDC_EDIT_MATCHRATE_RESULT_VALUE, strTemp);
	}
	else
	{
		int nResult = -1;
		strTemp.Format("%d", nResult);
		SetDlgItemText(IDC_EDIT_MATCHRATE_RESULT_VALUE, strTemp);
	}*/
	if(dmax < m_dEditResultMatchRate)
	{
		AfxMessageBox("Matching Fail!!");

		// 결과를 파일로 저장하여 Picture Control에 뿌리기 위해 추가 [2021.12.14 kyj]
		m_strResultImagePath = "D:\\1234.bmp";
		cvSaveImage(m_strResultImagePath, src);

		CClientDC dc(GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE));

		CRect rect;
		GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE)->GetClientRect(&rect);

		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);

		CImage image;
		image.Load(m_strResultImagePath);

		image.StretchBlt(dc.m_hDC,0,0,rect.Width(),rect.Height(),SRCCOPY);

		cvReleaseImage(&src);
		cvReleaseImage(&temp);
		cvReleaseImage(&coeff);

		return;
	}
	else
	{
		// 찾은 물체에 사각형 Rect를 그린다.
		cvRectangle(src, left_top, cvPoint(left_top.x + temp->width, left_top.y + temp->height), CV_RGB(255,0,0),2);

		// 결과를 파일로 저장하여 Picture Control에 뿌리기 위해 추가 [2021.12.14 kyj]
		m_strResultImagePath = "D:\\1234.bmp";
		cvSaveImage(m_strResultImagePath, src);

		// cvReleaseImage() = 할당 된 메모리 공간 해제 해주는 함수
		cvReleaseImage(&src);
		cvReleaseImage(&temp);
		cvReleaseImage(&coeff);

		cvDestroyAllWindows();   // 윈도우 제거

		CClientDC dc(GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE));

		CRect rect;
		GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE)->GetClientRect(&rect);

		SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);

		CImage image;
		image.Load(m_strResultImagePath);

		image.StretchBlt(dc.m_hDC,0,0,rect.Width(),rect.Height(),SRCCOPY);
	}
}


void CIMSDlg::OnBnClickedButtonSourceClear()
{
	// 우선 나중에 만들자 픽처 컨트롤 이미지 지우는법을 모르겠다.
	//m_pBitmapSourceImgInfo = NULL;

// 	HWND h_wnd = ::FindWindow(NULL, "계산기");
// 	
// 	if(NULL != h_wnd)
// 	{
// 		::SetWindowPos(h_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
// 	}

	// 매칭 시 소리 나게 하는구문 주석처리(다른 솔루션에 이미 구현 해놨음.) [2022.03.14 kyj]
// 	double dmin, dmax;
// 	CvPoint left_top;
// 	CString strTemp;
// 
// 	IplImage *img1 = NULL;
// 	//Mat src =  cvarrToMat(ScreenCapture());
// 
// 	//img1 = &IplImage(src);
// 	img1 = ScreenCapture();
// 
// 	//Show("TEST12345", img1);
// 
// 	IplImage *temp = cvLoadImage(m_strTempImagePath, -1);
// 	IplImage *coeff = cvCreateImage( cvSize( img1->width - temp->width+1, img1->height - temp->height+1 ), IPL_DEPTH_32F, 1);
// 
// 	int nMatchingMethod = GetMatchingMethod();
// 
// 	// 상관계수를 구하여 coeff에 그려줌.
// 	cvMatchTemplate(img1, temp, coeff, nMatchingMethod);
// 
// 	// 상관계수가 최대값을 가지는 위치를 찾는다.
// 	cvMinMaxLoc(coeff, &dmin, &dmax, NULL, &left_top);
// 	cvRectangle(img1, left_top, cvPoint(left_top.x + temp->width, left_top.y + temp->height), CV_RGB(255,0,0),2);
// 
// 	m_EditSetMatchRate.GetWindowTextA(strTemp);
// 	m_dEditResultMatchRate = atof(strTemp);
// 
// 	strTemp.Format("%.3f", dmax);
// 	SetDlgItemText(IDC_EDIT_MATCHRATE_RESULT_VALUE, strTemp);
// 
// 	if(1)
// 	{
// 		sndPlaySound("C:\\띠옹.wav", SND_ASYNC);
// 	}
// 
// 	m_strResultImagePath = "D:\\12345.bmp";
// 	cvSaveImage(m_strResultImagePath, img1);
// 
// 	cvReleaseImage(&img1);
// 	cvReleaseImage(&temp);
// 	cvReleaseImage(&coeff);
// 
// 	cvDestroyAllWindows();   // 윈도우 제거
// 
// 	CClientDC dc(GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE));
// 
// 	CRect rect;
// 	GetDlgItem(IDC_PICTURE_CONTROL_RESULT_IMAGE)->GetClientRect(&rect);
// 
// 	SetStretchBltMode(dc.GetSafeHdc(), COLORONCOLOR);
// 
// 	CImage image;
// 	image.Load(m_strResultImagePath);
// 
// 	image.StretchBlt(dc.m_hDC,0,0,rect.Width(),rect.Height(),SRCCOPY);
	// 여기까지가 소리나게 하는 구문


	
}

void CIMSDlg::OnBnClickedButtonTargetClear()
{
	// 우선 나중에 만들자 픽처 컨트롤 이미지 지우는법을 모르겠다.
	//m_pBitmapTargetImgInfo = NULL;
}

UINT CIMSDlg::ThreadFunctionFirstTest(LPVOID _mothod)
{
	CIMSDlg *pDlg = (CIMSDlg*)AfxGetApp()->m_pMainWnd;

	int num = 0;

	while(pDlg->m_ThreadWorkType == RUNNING)
	{
		pDlg->OnBnClickedButtonSourceClear();
		Sleep(10);
	}

	return 0;
}

IplImage* CIMSDlg::ScreenCapture()
{
 	//HWND h_wnd = ::FindWindow(NULL, "계산기");
	HWND h_wnd = ::GetDesktopWindow();
 
 	if(NULL == h_wnd)
 	{
 		return NULL;
 	}

	HDC h_screen_dc;

 
 	// IplImage 구조체 선언
 	IplImage* img_src = NULL;
 
 	// IplImage를 사용하기 위한 변수 선언
 	bool bMustRelease = false;
 	const int nDepth = IPL_DEPTH_8U; 
 	const int nChannels = 3; 
 
 	// 스크린 전체를 캡쳐하기 위해서 CWindowDC 형식으로 DC를 얻는다.
 	// GetDC의 파라메터에 특정 윈도우 핸들을 넣지 않고 NULL을 넣으면
 	// CWindowDC 형식으로 DC를 얻게 된다.
 	h_screen_dc = ::GetDC(NULL);
 
 	// 현재 스크린의 해상도를 얻는다.
 	int width = ::GetDeviceCaps(h_screen_dc, HORZRES);
 	int height = ::GetDeviceCaps(h_screen_dc, VERTRES);
 
 	// DIB의 형식을 정의한다.
 	BITMAPINFO dib_define;
 	BITMAPINFOHEADER* bmih = &(dib_define.bmiHeader);
 	bmih->biSize = sizeof(BITMAPINFOHEADER);
 	bmih->biWidth = 1920;
 	bmih->biHeight = 1080;
 	bmih->biPlanes = 1;
 	bmih->biBitCount = 24;
 	bmih->biCompression = BI_RGB;
 	bmih->biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
 	bmih->biXPelsPerMeter = 0;
 	bmih->biYPelsPerMeter = 0;
 	bmih->biClrImportant = 0;
 	bmih->biClrUsed = 0;
 
 	// DIB의 내부 이미지 비트 패턴을 참조할 포인터 변수
 	BYTE *p_image_data = NULL;
 
 	// dib_define에 정의된 내용으로 DIB를 생성한다.
 	HBITMAP h_bitmap = ::CreateDIBSection(h_screen_dc, &dib_define, DIB_RGB_COLORS, (void **)&p_image_data, 0, 0);
 
 	// 이미지를 추출하기 위해서 가상 DC를 생성한다. 메인 DC에서는 직접적으로 비트맵에 접근하여
 	// 이미지 패턴을 얻을 수 없기 때문이다.
 	HDC h_memory_dc = ::CreateCompatibleDC(h_screen_dc);
 
 	// 가상 DC에 이미지를 추출할 비트맵을 연결한다.
 	HBITMAP h_old_bitmap = (HBITMAP)::SelectObject(h_memory_dc, h_bitmap);
 
 	// 현재 스크린 화면을 캡쳐한다.
 	//::BitBlt(h_memory_dc, 0, 0, width/3, height-600, h_screen_dc, 640, 300, SRCCOPY);
	::BitBlt(h_memory_dc, 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);

 	// 본래의 비트맵으로 복구한다.
 	::SelectObject(h_memory_dc, h_old_bitmap);
 
 	// 가상 DC를 제거한다.
 	DeleteDC(h_memory_dc);
 
 	// 사용했던 DC 를 해제한다.
 	if(NULL != h_screen_dc) ::ReleaseDC(NULL, h_screen_dc);
 	h_screen_dc = NULL;
 
 	// 스크린 전체를 캡쳐하기 위해서 CWindowDC 형식으로 DC를 얻는다.
 	// GetDC의 파라메터에 특정 윈도우 핸들을 넣지 않고 NULL을 넣으면
 	// CWindowDC 형식으로 DC를 얻게 된다
 	if(h_screen_dc == NULL) 
 	{ 
 		h_screen_dc = ::GetDC(NULL); 
 		bMustRelease = true; 
 	} 
 
	// IplImage 구조체에 스크린 캡쳐영상 저장
	if(GetDIBits(h_screen_dc, h_bitmap, 0, 0, NULL, &dib_define, DIB_RGB_COLORS)) 
 	{ 
 		int nHeight = (bmih->biHeight > 0) ? bmih->biHeight : -bmih->biHeight; 
 		img_src = cvCreateImage(cvSize(bmih->biWidth, nHeight), nDepth, nChannels); 
 		//img_src->origin = (bmih->biHeight > 0); 
		img_src->origin = 0; // 화면이 반대로나와서 1->0으로 변경 [2021.12.21 kyj]
 		bmih->biBitCount = (WORD)(img_src->nChannels * 8); 
 		bmih->biCompression = BI_RGB; 
 		GetDIBits(h_screen_dc, h_bitmap, 0, nHeight, img_src->imageData, &dib_define, DIB_RGB_COLORS); 
 		cvFlip(img_src);
 	}
 
 	// 사용했던 비트맵과 DC 를 해제한다.
 	if(bMustRelease) ::ReleaseDC(NULL, h_screen_dc); 
 	DeleteObject(h_bitmap);
 
 	// IplImage 영상을 되돌림
 	return img_src;

	//return NULL;
}

void CIMSDlg::Show(char *str, IplImage *img)
{
	cvNamedWindow( str, 1);
	cvShowImage( str, img );
}

int CIMSDlg::GetMatchingMethod()
{
	if(m_ThreadWorkType != RUNNING)
		UpdateData(true);
	else
	{
		PostMessage(UPDATE_MSG, 0, 0);
	}

	int nMatchingMethod = m_RadioMatchingMethod;

	return nMatchingMethod;
}

void CIMSDlg::OnBnClickedButtonOpenOriginSourceImage()
{
	if(m_strSourceImagePath == "")
	{
		AfxMessageBox("Source Image is Empty!");
		return;
	}

	IplImage *src = cvLoadImage(m_strSourceImagePath, -1);

	Show("Source Image", src);
	cvWaitKey(0);
	cvReleaseImage(&src);

	cvDestroyAllWindows();

}


void CIMSDlg::OnBnClickedButtonOpenOriginTempImage()
{
	if(m_strTempImagePath == "")
	{
		AfxMessageBox("Template Image is Empty!");
		return;
	}

	IplImage *temp = cvLoadImage(m_strTempImagePath, -1);

	Show("Template Image", temp);
	cvWaitKey(0);
	cvReleaseImage(&temp);

	cvDestroyAllWindows();
}


void CIMSDlg::OnBnClickedButtonOpenOriginResultImage()
{
	if(m_strTempImagePath == "")
	{
		AfxMessageBox("Result Image is Empty!");
		return;
	}

	IplImage *result = cvLoadImage(m_strResultImagePath, -1);

	Show("Result Image", result);
	cvWaitKey(0);
	cvReleaseImage(&result);

	cvDestroyAllWindows();
}


void CIMSDlg::OnBnClickedButtonThreadStart()
{
	if(m_pThread == NULL)
	{
		m_pThread = AfxBeginThread(ThreadFunctionFirstTest, this);

		if(m_pThread == NULL)
		{
			AfxMessageBox("Error!!!");
		}

		// m_bAutoDelete를 선언해준것은 스레드 객체를 자동삭제 방지를 하겠다는 의미
		// 즉, 사용자가 직접 Delete 하게ㅆ다는 것.
		m_pThread->m_bAutoDelete = FALSE;

		m_ThreadWorkType = RUNNING;
	}
	else
	{
		if(m_ThreadWorkType == PAUSE)
		{
			// 스레드 재시작하겠다는 함수
			m_pThread->ResumeThread();
			m_ThreadWorkType = RUNNING;
		}
	}
}


void CIMSDlg::OnBnClickedButtonThreadStop()
{
	if(m_pThread == NULL)
	{
		AfxMessageBox("Thread Not Start!!");
	}
	else
	{
		m_pThread->SuspendThread(); // 스레드 일시정지 함수
		m_ThreadWorkType = PAUSE;
	}
}


void CIMSDlg::OnBnClickedButtonThreadPasue()
{
	if(m_pThread == NULL)
	{
		AfxMessageBox("Thread Not Start!!");
	}
	else
	{
		m_pThread->SuspendThread();

		DWORD dwResult;
		::GetExitCodeThread(m_pThread->m_hThread, &dwResult);

		delete m_pThread;
		m_pThread = NULL;

		m_ThreadWorkType = STOP;

		AfxMessageBox("Thread Stop OK!!!");
	}
}

LRESULT CIMSDlg::OnReceivedMsgFromThread( WPARAM w, LPARAM l )
{

	UpdateData(true);

	return 0;

}