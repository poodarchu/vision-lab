
// MultimediaDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Multimedia.h"
#include "MultimediaDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMultimediaDlg 对话框

CMultimediaDlg::CMultimediaDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMultimediaDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pImgSrc = NULL;
	m_pImgSrc = NULL;
	m_nThreadNum = 1;
	m_pThreadParam = new ThreadParam[MAX_THREAD];
	srand(time(0));
}

void CMultimediaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMultimediaDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CMultimediaDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CMultimediaDlg::OnBnClickedButtonProcess)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER, &CMultimediaDlg::OnNMCustomdrawSlider)

	ON_MESSAGE(WM_NOISE, &CMultimediaDlg::OnNoiseThreadMsgReceived)
	ON_MESSAGE(WM_MEDIAN_FILTER, &CMultimediaDlg::OnMedianFilterThreadMsgReceived)
END_MESSAGE_MAP()


// CMultimediaDlg 消息处理程序

BOOL CMultimediaDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	
	cmb_thread->AddString("WIN多线程");	
	cmb_thread->AddString("OpenMP");
	cmb_thread->AddString("CUDA");
	cmb_thread->SetCurSel(0);
	
	CComboBox* cmb_function = ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION));
	cmb_function->AddString("椒盐噪声");
	cmb_function->AddString("中值滤波");
	cmb_function->SetCurSel(0);

	CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER);
	slider->SetRange(1, MAX_THREAD, TRUE);
	slider->SetPos(MAX_THREAD);
	
	AfxBeginThread((AFX_THREADPROC)&CMultimediaDlg::Update, this);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMultimediaDlg::OnPaint()
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
		if (m_pImgSrc !=NULL)
		{
			DrawPara p;
			p.height = m_pImgSrc->GetHeight();
			p.width =  m_pImgSrc->GetWidth();
			p.oriX = 30;
			p.oriY = 50;
			p.pDC = GetDC();
			p.pImgSrc = m_pImgSrc;
			ThreadDraw(&p);
		}
		
	}
}

UINT CMultimediaDlg::Update(void* p)
{
	while(1)
	{
		Sleep(200);
		CMultimediaDlg* dlg = (CMultimediaDlg*)p;
		if (dlg->m_pImgSrc !=NULL)
		{
			DrawPara p;
			p.height = dlg->m_pImgSrc->GetHeight();
			p.width =  dlg->m_pImgSrc->GetWidth();
			p.oriX = 30;
			p.oriY = 50;
			p.pDC = dlg->GetDC();
			p.pImgSrc = dlg->getImage();
			dlg->ThreadDraw(&p);
		}
	}
	return 0;
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMultimediaDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMultimediaDlg::OnBnClickedButtonOpenfile()
{
	//选择文件对话框
	TCHAR  szFilter[] = _T("JPEG(*.jpg)|*.jpg|BMP(*.jpg)|*.bmp|TIFF (*.tif)|*.tif|All Files (*.*)|*.*||");
	CFileDialog fileOpenDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (fileOpenDlg.DoModal() == IDOK)
	{
		CString imFilePath;
		VERIFY(imFilePath = fileOpenDlg.GetPathName());
		if (m_pImgSrc != NULL)
		{
			m_pImgSrc->Destroy();
			delete m_pImgSrc;
		}
		m_pImgSrc = new CImage();
		m_pImgSrc->Load(imFilePath);

		this->Invalidate();
	}
}

void CMultimediaDlg::ThreadDraw(DrawPara *p)
{
	CRect rect;
	GetClientRect(&rect);
	CDC     memDC;             // 用于缓冲绘图的内存画笔  
	CBitmap memBitmap;         // 用于缓冲绘图的内存画布
	memDC.CreateCompatibleDC(p->pDC);  // 创建与原画笔兼容的画笔
	memBitmap.CreateCompatibleBitmap(p->pDC, p->width, p->height);  // 创建与原位图兼容的内存画布
	memDC.SelectObject(&memBitmap);      // 创建画笔与画布的关联
	memDC.FillSolidRect(rect,p->pDC->GetBkColor());

	// 将pImgSrc的内容缩放画到内存画布中
	p->pImgSrc->StretchBlt(memDC.m_hDC, 0, 0, p->width, p->height);
	
	// 将已画好的画布复制到真正的缓冲区中
	p->pDC->BitBlt(p->oriX, p->oriY, p->width, p->height, &memDC, 0, 0, SRCCOPY ); 
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CMultimediaDlg::ImageCopy(CImage* pImgSrc, CImage* pImgDrt)
{
    int MaxColors = pImgSrc->GetMaxColorTableEntries();
    RGBQUAD* ColorTab;
    ColorTab = new RGBQUAD[MaxColors];

    CDC *pDCsrc,*pDCdrc;
    if (!pImgDrt->IsNull())
    {
        pImgDrt->Destroy();
    }
    pImgDrt->Create(pImgSrc->GetWidth(),pImgSrc->GetHeight(),pImgSrc->GetBPP(),0);

    if (pImgSrc->IsIndexed())
    {
        pImgSrc->GetColorTable(0,MaxColors,ColorTab);
        pImgDrt->SetColorTable(0,MaxColors,ColorTab);
    }

    pDCsrc=CDC::FromHandle(pImgSrc->GetDC());
    pDCdrc=CDC::FromHandle(pImgDrt->GetDC());
    pDCdrc->BitBlt(0,0,pImgSrc->GetWidth(),pImgSrc->GetHeight(),pDCsrc,0,0,SRCCOPY);
    pImgSrc->ReleaseDC();
    pImgDrt->ReleaseDC();
    delete ColorTab;

}

BOOL CMultimediaDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;

}

void CMultimediaDlg::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER);
	CString text("");
	m_nThreadNum = slider->GetPos();
	text.Format(_T("%d"), m_nThreadNum);
	GetDlgItem(IDC_STATIC_THREADNUM)->SetWindowText(text);
	*pResult = 0;
}

//图像处理
void CMultimediaDlg::OnBnClickedButtonProcess()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox* cmb_function = ((CComboBox*)GetDlgItem(IDC_COMBO_FUNCTION));
	int func = cmb_function->GetCurSel();
	switch (func)
	{
	case 0:  //椒盐噪声
		AddNoise();
		break;
	case 1: //自适应中值滤波
		MedianFilter();
		break;
	default: 
		break;
	}
}

void CMultimediaDlg::AddNoise()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = CTime::GetTickCount();
	switch (thread)
	{
	case 0://win多线程
		{
			int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
			for (int i = 0; i < m_nThreadNum; ++i)
			{
				m_pThreadParam[i].startIndex = i * subLength;
				m_pThreadParam[i].endIndex = i != m_nThreadNum -1 ? 
					 (i + 1) * subLength - 1 :m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
				m_pThreadParam[i].src = m_pImgSrc;
				AfxBeginThread((AFX_THREADPROC)&ImageProcess::addNoise, &m_pThreadParam[i]);
			}
		}

		break;

	case 1://openmp
		{
			int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

			#pragma omp parallel for num_threads(m_nThreadNum)
			for (int i = 0; i < m_nThreadNum; ++i)
			{
				m_pThreadParam[i].startIndex = i * subLength;
				m_pThreadParam[i].endIndex = i != m_nThreadNum -1 ? 
					 (i + 1) * subLength - 1 :m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
				m_pThreadParam[i].src = m_pImgSrc;
				ImageProcess::addNoise(&m_pThreadParam[i]);
			}


			CTime endTime = CTime::GetTickCount();
			CString timeStr;
			timeStr.Format("耗时:%dms", endTime - startTime);
			AfxMessageBox(timeStr);
		}

		break;

	case 2://cuda
		break;
	}
}

void CMultimediaDlg::MedianFilter()
{
	CComboBox* cmb_thread = ((CComboBox*)GetDlgItem(IDC_COMBO_THREAD));
	int thread = cmb_thread->GetCurSel();
	startTime = CTime::GetTickCount();
	m_nThreadNum;
	switch (thread)
	{
	case 0://win多线程
		{
			int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;
			int h = m_pImgSrc->GetHeight() / m_nThreadNum;
			int w = m_pImgSrc->GetWidth()/ m_nThreadNum;
			for (int i = 0; i < m_nThreadNum; ++i)
			{
				m_pThreadParam[i].startIndex = i * subLength;
				m_pThreadParam[i].endIndex = i != m_nThreadNum -1 ? 
					 (i + 1) * subLength - 1 :m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
				m_pThreadParam[i].maxSpan = MAX_SPAN;
				m_pThreadParam[i].src = m_pImgSrc;
				AfxBeginThread((AFX_THREADPROC)&ImageProcess::medianFilter, &m_pThreadParam[i]);
			}

		}

		break;

	case 1://openmp
		{
			int subLength = m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() / m_nThreadNum;

			#pragma omp parallel for num_threads(m_nThreadNum)
			for (int i = 0; i < m_nThreadNum; ++i)
			{
				m_pThreadParam[i].startIndex = i * subLength;
				m_pThreadParam[i].endIndex = i != m_nThreadNum -1 ? 
					 (i + 1) * subLength - 1 :m_pImgSrc->GetWidth() * m_pImgSrc->GetHeight() - 1;
				m_pThreadParam[i].maxSpan = MAX_SPAN;
				m_pThreadParam[i].src = m_pImgSrc;
				ImageProcess::medianFilter(&m_pThreadParam[i]);
			}

			CTime endTime = CTime::GetTickCount();
			CString timeStr;
			timeStr.Format("耗时:%dms", endTime - startTime);
			AfxMessageBox(timeStr);
		}

		break;

	case 2://cuda
		break;
	}
}


LRESULT CMultimediaDlg::OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int tempCount = 0;
	if ((int)wParam == 1)
	{
		// 当所有线程都返回了值1代表全部结束~显示时间
		if (m_nThreadNum == ++tempCount)
		{
			CTime endTime = CTime::GetTickCount();
			CString timeStr;
			timeStr.Format("耗时:%dms", endTime - startTime);
			tempCount = 0;

			// 显示消息窗口
			AfxMessageBox(timeStr);
		}
	}

	return 0;
}

LRESULT CMultimediaDlg::OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam)
{
	static int tempCount = 0;
	if ((int)wParam == 1)
		tempCount++;
	if (m_nThreadNum == tempCount)
	{
		CTime endTime = CTime::GetTickCount();
		CString timeStr;
		timeStr.Format("耗时:%dms", endTime - startTime);
		tempCount = 0;

		AfxMessageBox(timeStr);
	}
	return 0;
}