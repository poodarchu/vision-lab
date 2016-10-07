
// ExperimentImgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ExperimentImg.h"
#include "ExperimentImgDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CExperimentImgDlg 对话框




CExperimentImgDlg::CExperimentImgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExperimentImgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExperimentImgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_INFO, mEditInfo);
}

BEGIN_MESSAGE_MAP(CExperimentImgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CExperimentImgDlg::OnBnClickedButtonOpen)
END_MESSAGE_MAP()


// CExperimentImgDlg 消息处理程序

BOOL CExperimentImgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CExperimentImgDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CExperimentImgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CExperimentImgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CExperimentImgDlg::OnBnClickedButtonOpen()
{
	//选择文件对话框
	TCHAR  szFilter[] = _T("JPEG(*.jpg)|*.jpg|BMP(*.jpg)|*.bmp|TIFF (*.tif)|*.tif|All Files (*.*)|*.*||");
	CString filePath("");
	CFileDialog fileOpenDlg(TRUE, NULL, NULL,OFN_HIDEREADONLY,szFilter);
	if (fileOpenDlg.DoModal() == IDOK) 
	{
		VERIFY(filePath = fileOpenDlg.GetPathName());
	}
	//CImage类加载图像
	CString strFilePath(filePath);
	CImage imSrc;
	imSrc.Load(strFilePath);
	//文本控件显示文件信息
	mEditInfo.SetWindowTextA(strFilePath + "\n");
	

	CImage imThumb1;
	CDC* pDC = GetDC();
	imThumb1.Load(strFilePath);

	////直接绘制缩略图
	//if(imSrc.GetWidth()>imSrc.GetHeight())
	//	imThumb1.StretchBlt(pDC->m_hDC,10,10,150,imSrc.GetHeight()*150/imSrc.GetWidth());
	//else
	//	imThumb1.StretchBlt(pDC->m_hDC,10,10,imSrc.GetWidth()*150/imSrc.GetHeight(),150);
	//双缓存绘制
	CRect   rect;              // 存储客户区大小  
	CDC     MemDC;             // 用于缓冲绘图的内存DC  
	CBitmap MemBitmap;        // 内存DC中承载临时图像的位图 
	// 获取当前客户区大小  
	GetClientRect( &rect );   
      
	// 创建兼容当前DC的内存DC  
	MemDC.CreateCompatibleDC( pDC );  
  
	// 创建兼容位图  
	//MemBitmap.CreateCompatibleBitmap( pDC, rect.Width(), rect.Height() );  
	MemBitmap.CreateCompatibleBitmap( pDC, 150, 150 );  
	// 将兼容位图选入到内存缓冲DC，相当于画布，必须选进内存DC  
	MemDC.SelectObject( &MemBitmap );   
  
	 // 使用原背景色填充内存缓冲DC  
	MemDC.FillSolidRect( rect, pDC->GetBkColor() );   

	// 将绘图代码中的“pDC->”全部改为“MemDC.”即可。
	if(imSrc.GetWidth()>imSrc.GetHeight())
		imThumb1.StretchBlt(MemDC.m_hDC,0,(150-imSrc.GetHeight()*150/imSrc.GetWidth())/2,150,imSrc.GetHeight()*150/imSrc.GetWidth());
	else
		imThumb1.StretchBlt(MemDC.m_hDC,(150-imSrc.GetWidth()*150/imSrc.GetHeight())/2,0,imSrc.GetWidth()*150/imSrc.GetHeight(),150);
	//将内存缓冲DC中的内容复制到当前DC
	//pDC->BitBlt( 0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY ); 
	pDC->BitBlt(10,10,150,150,&MemDC,0,0,SRCCOPY ); 

	DrawPara param;
	param.filepath = strFilePath;
	param.oriX = 10;
	param.oriY = 170;
	param.width = 150;
	param.height = 150;
	param.pDC = pDC;

	

}

//int ThreadDraw(&CExperimentImgDlg::ThreadDraw,DrawPara *param)
//{
//
//
//	 CImage imSrc;
//	 imSrc.Load(param->filepath);
//	 	//计算目标图像（绘制区）大小
//	int dstWidth, dstHeigh;
//	if(imSrc.GetWidth() > imSrc.GetHeight())
//	{
//		dstWidth = param->width;
//		dstHeigh = imSrc.GetHeight()*param->width/imSrc.GetWidth();
//	}else
//	{
//		dstHeigh = param->height;
//		dstWidth = imSrc.GetWidth()*param->height/imSrc.GetHeight();
//	}
//	 	CDC     MemDC;             // 用于缓冲绘图的内存DC  
//	CBitmap MemBitmap;        // 内存DC中承载临时图像的位图 
//	MemDC.CreateCompatibleDC(param->pDC); 
//	MemBitmap.CreateCompatibleBitmap( param->pDC, param->width, param->height ); 
//	MemDC.SelectObject( &MemBitmap ); 
//
//	if(imSrc.GetWidth()>imSrc.GetHeight())
//		imSrc.StretchBlt(MemDC.m_hDC,0,(param->height-imSrc.GetHeight()*param->width/imSrc.GetWidth())/2,param->width,imSrc.GetHeight()*param->width/imSrc.GetWidth());
//	else
//		imSrc.StretchBlt(MemDC.m_hDC,(param->width-imSrc.GetWidth()*param->height/imSrc.GetHeight())/2,0,imSrc.GetWidth()*param->height/imSrc.GetHeight(),param->height);
//	
//	param->pDC->BitBlt(param->oriX,param->oriY,param->width,param->height,&MemDC,0,0,SRCCOPY ); 
//	return 1;
//
//}
