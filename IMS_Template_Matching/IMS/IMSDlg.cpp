
// IMSDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "IMS.h"
#include "IMSDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CIMSDlg ��ȭ ����




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


// CIMSDlg �޽��� ó����

BOOL CIMSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	m_pBitmapSourceImgInfo = NULL;
	m_pBitmapTargetImgInfo = NULL;

	m_strSourceImagePath = "";
	m_strTempImagePath = "";
	m_strResultImagePath = "";

	m_ThreadWorkType = STOP;
	m_pThread = NULL;

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CIMSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CIMSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CIMSDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	// ����Ű ESCŰ ������ �� ����Ǵ� ��Ȳ ����
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
		//m_pBitmapSourceImgInfo = m_BitmapSourceImg.GetBitmapInfo(); // ���⼭ ��Ʈ�� ������ ��� �ȴ�.

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
		//m_pBitmapTargetImgInfo = m_BitmapTargetImg.GetBitmapInfo(); // ���⼭ ��Ʈ�� ������ ��� �ȴ�.

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

	// Source, Temp �̹��� �ε�
	IplImage *src = cvLoadImage(m_strSourceImagePath, -1);
	IplImage *temp = cvLoadImage(m_strTempImagePath, -1);

	if(src->width < temp->width) // �ҽ��̹����� Width�� �� Ŀ���Ѵ�. �׷��� ������ ���α׷� �ٿ� �߻�.
	{
		AfxMessageBox("Temp Image's Width bigger than Source Image's Width");
		return;
	}

	if(src->height < temp->height) // �ҽ��̹����� Height�� �� Ŀ���Ѵ�. �׷��� ������ ���α׷� �ٿ� �߻�.
	{
		AfxMessageBox("Temp Image's Height bigger than Source Image's Height");
		return;
	}

	// �������� ���� �̹���
	IplImage *coeff = cvCreateImage( cvSize( src->width - temp->width+1, src->height - temp->height+1 ), IPL_DEPTH_32F, 1);

	if(src->nChannels != temp->nChannels) // �ҽ��̹����� 8��Ʈ�ε� ���ø���Ī�̹����� 24��Ʈ �̷� ��� ���α׷� �ٿ� �߻� ����ó�� [2021.12.14 kyj]
	{
		AfxMessageBox("Source and Template Image Type(bit) is not same!");
		return;
	}

	int nMatchingMethod = GetMatchingMethod();

	// �������� ���Ͽ� coeff�� �׷���.
	cvMatchTemplate(src, temp, coeff, nMatchingMethod);
	/*
	CV_TM_SQDIFF        =0
	CV_TM_SQDIFF_NORMED =1
	CV_TM_CCORR         =2
	CV_TM_CCORR_NORMED  =3
	CV_TM_CCOEFF        =4
	CV_TM_CCOEFF_NORMED =5
	*/

	// �������� �ִ밪�� ������ ��ġ�� ã�´�.
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

		// ����� ���Ϸ� �����Ͽ� Picture Control�� �Ѹ��� ���� �߰� [2021.12.14 kyj]
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
		// ã�� ��ü�� �簢�� Rect�� �׸���.
		cvRectangle(src, left_top, cvPoint(left_top.x + temp->width, left_top.y + temp->height), CV_RGB(255,0,0),2);

		// ����� ���Ϸ� �����Ͽ� Picture Control�� �Ѹ��� ���� �߰� [2021.12.14 kyj]
		m_strResultImagePath = "D:\\1234.bmp";
		cvSaveImage(m_strResultImagePath, src);

		// cvReleaseImage() = �Ҵ� �� �޸� ���� ���� ���ִ� �Լ�
		cvReleaseImage(&src);
		cvReleaseImage(&temp);
		cvReleaseImage(&coeff);

		cvDestroyAllWindows();   // ������ ����

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
	// �켱 ���߿� ������ ��ó ��Ʈ�� �̹��� ����¹��� �𸣰ڴ�.
	//m_pBitmapSourceImgInfo = NULL;

// 	HWND h_wnd = ::FindWindow(NULL, "����");
// 	
// 	if(NULL != h_wnd)
// 	{
// 		::SetWindowPos(h_wnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
// 	}

	// ��Ī �� �Ҹ� ���� �ϴ±��� �ּ�ó��(�ٸ� �ַ�ǿ� �̹� ���� �س���.) [2022.03.14 kyj]
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
// 	// �������� ���Ͽ� coeff�� �׷���.
// 	cvMatchTemplate(img1, temp, coeff, nMatchingMethod);
// 
// 	// �������� �ִ밪�� ������ ��ġ�� ã�´�.
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
// 		sndPlaySound("C:\\���.wav", SND_ASYNC);
// 	}
// 
// 	m_strResultImagePath = "D:\\12345.bmp";
// 	cvSaveImage(m_strResultImagePath, img1);
// 
// 	cvReleaseImage(&img1);
// 	cvReleaseImage(&temp);
// 	cvReleaseImage(&coeff);
// 
// 	cvDestroyAllWindows();   // ������ ����
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
	// ��������� �Ҹ����� �ϴ� ����


	
}

void CIMSDlg::OnBnClickedButtonTargetClear()
{
	// �켱 ���߿� ������ ��ó ��Ʈ�� �̹��� ����¹��� �𸣰ڴ�.
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
 	//HWND h_wnd = ::FindWindow(NULL, "����");
	HWND h_wnd = ::GetDesktopWindow();
 
 	if(NULL == h_wnd)
 	{
 		return NULL;
 	}

	HDC h_screen_dc;

 
 	// IplImage ����ü ����
 	IplImage* img_src = NULL;
 
 	// IplImage�� ����ϱ� ���� ���� ����
 	bool bMustRelease = false;
 	const int nDepth = IPL_DEPTH_8U; 
 	const int nChannels = 3; 
 
 	// ��ũ�� ��ü�� ĸ���ϱ� ���ؼ� CWindowDC �������� DC�� ��´�.
 	// GetDC�� �Ķ���Ϳ� Ư�� ������ �ڵ��� ���� �ʰ� NULL�� ������
 	// CWindowDC �������� DC�� ��� �ȴ�.
 	h_screen_dc = ::GetDC(NULL);
 
 	// ���� ��ũ���� �ػ󵵸� ��´�.
 	int width = ::GetDeviceCaps(h_screen_dc, HORZRES);
 	int height = ::GetDeviceCaps(h_screen_dc, VERTRES);
 
 	// DIB�� ������ �����Ѵ�.
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
 
 	// DIB�� ���� �̹��� ��Ʈ ������ ������ ������ ����
 	BYTE *p_image_data = NULL;
 
 	// dib_define�� ���ǵ� �������� DIB�� �����Ѵ�.
 	HBITMAP h_bitmap = ::CreateDIBSection(h_screen_dc, &dib_define, DIB_RGB_COLORS, (void **)&p_image_data, 0, 0);
 
 	// �̹����� �����ϱ� ���ؼ� ���� DC�� �����Ѵ�. ���� DC������ ���������� ��Ʈ�ʿ� �����Ͽ�
 	// �̹��� ������ ���� �� ���� �����̴�.
 	HDC h_memory_dc = ::CreateCompatibleDC(h_screen_dc);
 
 	// ���� DC�� �̹����� ������ ��Ʈ���� �����Ѵ�.
 	HBITMAP h_old_bitmap = (HBITMAP)::SelectObject(h_memory_dc, h_bitmap);
 
 	// ���� ��ũ�� ȭ���� ĸ���Ѵ�.
 	//::BitBlt(h_memory_dc, 0, 0, width/3, height-600, h_screen_dc, 640, 300, SRCCOPY);
	::BitBlt(h_memory_dc, 0, 0, width, height, h_screen_dc, 0, 0, SRCCOPY);

 	// ������ ��Ʈ������ �����Ѵ�.
 	::SelectObject(h_memory_dc, h_old_bitmap);
 
 	// ���� DC�� �����Ѵ�.
 	DeleteDC(h_memory_dc);
 
 	// ����ߴ� DC �� �����Ѵ�.
 	if(NULL != h_screen_dc) ::ReleaseDC(NULL, h_screen_dc);
 	h_screen_dc = NULL;
 
 	// ��ũ�� ��ü�� ĸ���ϱ� ���ؼ� CWindowDC �������� DC�� ��´�.
 	// GetDC�� �Ķ���Ϳ� Ư�� ������ �ڵ��� ���� �ʰ� NULL�� ������
 	// CWindowDC �������� DC�� ��� �ȴ�
 	if(h_screen_dc == NULL) 
 	{ 
 		h_screen_dc = ::GetDC(NULL); 
 		bMustRelease = true; 
 	} 
 
	// IplImage ����ü�� ��ũ�� ĸ�Ŀ��� ����
	if(GetDIBits(h_screen_dc, h_bitmap, 0, 0, NULL, &dib_define, DIB_RGB_COLORS)) 
 	{ 
 		int nHeight = (bmih->biHeight > 0) ? bmih->biHeight : -bmih->biHeight; 
 		img_src = cvCreateImage(cvSize(bmih->biWidth, nHeight), nDepth, nChannels); 
 		//img_src->origin = (bmih->biHeight > 0); 
		img_src->origin = 0; // ȭ���� �ݴ�γ��ͼ� 1->0���� ���� [2021.12.21 kyj]
 		bmih->biBitCount = (WORD)(img_src->nChannels * 8); 
 		bmih->biCompression = BI_RGB; 
 		GetDIBits(h_screen_dc, h_bitmap, 0, nHeight, img_src->imageData, &dib_define, DIB_RGB_COLORS); 
 		cvFlip(img_src);
 	}
 
 	// ����ߴ� ��Ʈ�ʰ� DC �� �����Ѵ�.
 	if(bMustRelease) ::ReleaseDC(NULL, h_screen_dc); 
 	DeleteObject(h_bitmap);
 
 	// IplImage ������ �ǵ���
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

		// m_bAutoDelete�� �������ذ��� ������ ��ü�� �ڵ����� ������ �ϰڴٴ� �ǹ�
		// ��, ����ڰ� ���� Delete �ϰԤ��ٴ� ��.
		m_pThread->m_bAutoDelete = FALSE;

		m_ThreadWorkType = RUNNING;
	}
	else
	{
		if(m_ThreadWorkType == PAUSE)
		{
			// ������ ������ϰڴٴ� �Լ�
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
		m_pThread->SuspendThread(); // ������ �Ͻ����� �Լ�
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