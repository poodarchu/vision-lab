
// MultimediaDlg.h : 头文件
//

#pragma once
#include "ImageProcess.h"

//最大的线程数
#define MAX_THREAD 8 
//模板中心到边缘的最大距离
#define MAX_SPAN 15 

struct DrawPara
{
	CImage* pImgSrc;
	CDC* pDC;
	int oriX;
	int oriY;
	int width;
	int height;
};

// CMultimediaDlg 对话框
class CMultimediaDlg : public CDialogEx
{
// 构造
public:
	CMultimediaDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MULTIMEDIA_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void ThreadDraw(DrawPara *p);//绘制
	static UINT Update(void* p);
	void MedianFilter();//中值滤波
	void AddNoise();//添加椒盐噪声
	void ImageCopy(CImage* pImgSrc, CImage* pImgDrt);
	afx_msg LRESULT OnMedianFilterThreadMsgReceived(WPARAM wParam, LPARAM lParam);// 中值滤波线程消息接受函数
	afx_msg LRESULT OnNoiseThreadMsgReceived(WPARAM wParam, LPARAM lParam); //椒盐噪声线程消息接受函数

	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonProcess(); //图像处理入口函数
	afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL OnEraseBkgnd(CDC* pDC);
	CImage* getImage(){return m_pImgSrc;}
	

// 实现
protected:
	HICON m_hIcon;
	CImage *m_pImgSrc;//图像
	int m_nThreadNum;//线程数
	ThreadParam* m_pThreadParam;
	CTime startTime;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

};
