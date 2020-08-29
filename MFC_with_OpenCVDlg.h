// MFC_with_OpenCVDlg.h : ��� ����
//
#pragma once
#include "afxwin.h"
#include <stdio.h>
#include <stdlib.h>

// database
#include <WinSock2.h>
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>

// opencv
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <cv.h>
#include <core.hpp>
#include <highgui.hpp>
#include <imgproc.hpp>
#include <video.hpp>
#include <time.h>

using namespace cv;
using namespace std;

// CMFC_with_OpenCVDlg ��ȭ ����
class CMFC_with_OpenCVDlg : public CDialogEx
{
	// �����Դϴ�.
public:
	CMFC_with_OpenCVDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.

												 // ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_WITH_OPENCV_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   // DDX/DDV �����Դϴ�.


													   // �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_picture;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	VideoCapture *video;
	Mat mat_frame;
	CImage cimage_mfc;

	CString Text, Name_UI_Text;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	MYSQL *conn;    //   the connection
	MYSQL_RES *res; //  the results
	MYSQL_ROW row;  //     the results row(line by line)

					// database // 
					//--------------------------------------
	const char *host = "localhost";
	const char *user = "root";
	const char* pw; // insert password
	const char *db = "drone";
	//--------------------------------------

	afx_msg void OnEnChangeName();
	CEdit m_Name;
	afx_msg void OnBnClickedInput();
	CString Name;
	CStatic m_Text;
	CStatic m_NAME_TEXT;
	afx_msg void OnBnClickedImageShow();
	Mat Seq_Image1, Seq_Image2, Seq_Image3, Seq_Image4, Seq_Image5, Seq_Image6, Seq_Image7,
		Seq_Image8, Seq_Image9, Seq_Image10;

	CEdit m_period;
	CStatic m_drone_text;
	afx_msg void OnEnChangeBlinkPeriod();
	afx_msg void OnStnClickedDronePrint();
	CEdit m_idf_num;
	afx_msg void OnBnClickedSearch();
};