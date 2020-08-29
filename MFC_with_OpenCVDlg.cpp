// MFC_with_OpenCVDlg.cpp : ���� ����
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") // �ܼ�â ���

#include "stdafx.h"
#include "MFC_with_OpenCV.h"
#include "MFC_with_OpenCVDlg.h"
#include "afxdialogex.h"

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

char drone_id[10]; // 10�ڸ� ��� �ĺ���ȣ.
				   // ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.
void detectHScolor(const cv::Mat& image, double minHue, double maxHue,
	double minSat, double maxSat, cv::Mat& mask);
void finish_with_error(MYSQL *conn);
void user_check(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row);
int get_seq(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row);
char encoding(Mat image, int i_count);
int bound_count;
float ratio;
bool image_show = false;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

														// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMFC_with_OpenCVDlg ��ȭ ����

CMFC_with_OpenCVDlg::CMFC_with_OpenCVDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_WITH_OPENCV_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_with_OpenCVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE, m_picture);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_TEXT, m_Text);
	DDX_Control(pDX, IDC_NAME_TEXT, m_NAME_TEXT);
	DDX_Control(pDX, IDC_BLINK_PERIOD, m_period);
	DDX_Control(pDX, IDC_DRONE_PRINT, m_drone_text);
	DDX_Control(pDX, IDC_IDF_NUM, m_idf_num);
}

BEGIN_MESSAGE_MAP(CMFC_with_OpenCVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN1, &CMFC_with_OpenCVDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN2, &CMFC_with_OpenCVDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_NAME, &CMFC_with_OpenCVDlg::OnEnChangeName)
	ON_BN_CLICKED(IDC_INPUT, &CMFC_with_OpenCVDlg::OnBnClickedInput)
	ON_BN_CLICKED(IDC_IMAGE_SHOW, &CMFC_with_OpenCVDlg::OnBnClickedImageShow)
	ON_EN_CHANGE(IDC_BLINK_PERIOD, &CMFC_with_OpenCVDlg::OnEnChangeBlinkPeriod)
	ON_STN_CLICKED(IDC_DRONE_PRINT, &CMFC_with_OpenCVDlg::OnStnClickedDronePrint)
	ON_BN_CLICKED(IDC_SEARCH, &CMFC_with_OpenCVDlg::OnBnClickedSearch)
END_MESSAGE_MAP()

// CMFC_with_OpenCVDlg �޽��� ó����

BOOL CMFC_with_OpenCVDlg::OnInitDialog()
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);         // ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);      // ���� �������� �����մϴ�.

								  // TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	Name_UI_Text = "1. ��� �ĺ� ��ȣ ���� \n2. �̸� �Է� �� input ��ư Ŭ��\n"; // �ʱ� static text ����.
	CString Temp_t = "3. ��� �ĺ� ��ȣ -> ������ ���ɿ��� ������ Enter Ű\n4. �̹��� Ȯ�� �� �ĺ� ��ȣ ���� �� �� ��\n";
	CString Temp_t2 = "5. ��� �����̴� �ð��� �����ϰ� ������ ���� �Է� �� �ĺ� ��ȣ�� ������ ��";
	Name_UI_Text += Temp_t;
	Name_UI_Text += Temp_t2;
	m_NAME_TEXT.SetWindowText(Name_UI_Text);

	CString Period = "������ ���� ���� (default = 1 sec) : ";
	m_drone_text.SetWindowTextA(Period);

	Text = "\t\t������ ���̽�\n"; // �ʱ� static text ����.
	m_Text.SetWindowText(Text);

	conn = mysql_init(NULL);
	int seq = 0;
	char idf_num[10];
	char name[30] = "admin";

	if (mysql_real_connect(conn, host, user, pw, db, 3306, NULL, 0) == NULL)
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}
	if (mysql_query(conn, "create table if not exists Drone(seq int, idf_num char(10) ,name char(30))"))
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}

	SetTimer(1000, 30, NULL); // 100�� �ݺ��ֱ� 0.1 ��

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CMFC_with_OpenCVDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFC_with_OpenCVDlg::OnPaint()
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
HCURSOR CMFC_with_OpenCVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFC_with_OpenCVDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void CMFC_with_OpenCVDlg::OnTimer(UINT_PTR nIDEvent)
{
	//TODO
	CDialogEx::OnTimer(nIDEvent);
}

void detectHScolor(const cv::Mat& image, double minHue, double maxHue,
	double minSat, double maxSat, cv::Mat& mask)
{
	Mat hsv;
	cvtColor(image, hsv, CV_BGR2HSV);

	vector<cv::Mat> channels;
	split(hsv, channels); //HSV ä�� �и� channel[0] - ����, channel[1] - ä�� , channel[2] - ��
						  // Hue mask [0~255]
	Mat mask1;
	threshold(channels[0], mask1, maxHue, 255, cv::THRESH_BINARY_INV);
	Mat mask2;
	threshold(channels[0], mask2, minHue, 255, cv::THRESH_BINARY);
	cv::Mat hueMask;
	if (minHue < maxHue)
		hueMask = mask1 & mask2;
	else
		hueMask = mask1 | mask2;

	threshold(channels[1], mask1, maxSat, 255, cv::THRESH_BINARY_INV); // ����ȭ
	threshold(channels[1], mask2, minSat, 255, cv::THRESH_BINARY);

	cv::Mat satMask;
	satMask = mask1& mask2;
	// ���� ������ �������󿡼� copy
	mask = hueMask & satMask;
}

char encoding(Mat image, int i_count)
{
	int count = 0;
	int pixel = 0;
	float similarity = 0.0;
	for (int x = 0; x < image.cols; x++)
	{
		for (int y = 0; y < image.rows; y++)
		{
			pixel++;
			if (image.at<unsigned char>(y, x) == 255) // ������ ����� mask
			{
				if (i_count == 0) bound_count++;
				count++;
			}
		}
	} // for �� ��ȣ

	similarity = (float)count / float(pixel);

	if (i_count == 0) { 
		ratio = (float)bound_count / (float)pixel;
		return '1'; 
	}
	else if (i_count != 0 && (similarity > 0.5*ratio))
	{
		if (similarity > 4*ratio) // ���� �̹����� ������ ������ ������ �Ǵ� �̹����� ������ 4���̻��� �Ǹ�
		{
			bound_count = count;  
			ratio = (float)bound_count / (float)pixel;
			for (int k = 0; k < i_count; k++) {
				drone_id[k] = '0';
			}
		}
		return '1'; // ���絵 1/2 ���� ����.
	}
		
	else { 
		
		return '0';
	}	
}

void CMFC_with_OpenCVDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH] = { 0, }; // Init
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = NULL; // ���� ���� ��� ������ ������.
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (::GetOpenFileName(&ofn) == false) exit(1);

	video = new VideoCapture(ofn.lpstrFile);
	if (!video->isOpened())
	{
		MessageBox(_T("���� ���Ⱑ �ȵ�. \n"));
	}

	CString IDF_Message = "�ĺ� ��ȣ  : \n";
	string trackerTypes[6] = { "BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN" };
	string trackerType = trackerTypes[3];
	Ptr<Tracker> tracker;

	if (trackerType == "BOOSTING")
		tracker = TrackerBoosting::create();
	if (trackerType == "MIL")
		tracker = TrackerMIL::create();
	if (trackerType == "KCF")
		tracker = TrackerKCF::create();
	if (trackerType == "TLD")
		tracker = TrackerTLD::create();
	if (trackerType == "MEDIANFLOW")
		tracker = TrackerMedianFlow::create();
	if (trackerType == "GOTURN")
		tracker = TrackerGOTURN::create();

	double check_t = 0;
	char image_name[200]; // �̸� 200�� ������.
	Mat image;
	int i_count = 0;
	CString Drone_Period;
	m_drone_text.GetWindowText(Drone_Period);
	int blink_period = 1;

	if (Drone_Period.IsEmpty() == TRUE)
		blink_period = 1;
	else
		blink_period = _ttoi(Drone_Period);

	clock_t begin, end;
	Mat frame;
	bool ok = video->read(frame);
	// Define initial boundibg box
	Rect2d roi(287, 23, 86, 320);

	roi = selectROI(frame, false); // ROI ����

								   // Display bounding box.
	rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
	//   imshow("Tracking", frame);
	//   waitKey(1);

	tracker->init(frame, roi);

	while (video->read(frame) && i_count <10)
	{
		begin = clock(); // �ð� ���� ����.

						 // Start timer
		double timer = (double)getTickCount();

		// Update the tracking result
		bool ok = tracker->update(frame, roi);

		// Calculate Frames per second (FPS)
		float fps = getTickFrequency() / ((double)getTickCount() - timer);
		// getTickFrequency() : �и��� ������ �ڵ� �κ��� ����ð� �˾Ƴ�
		// getTickCount() : ��ǻ�͸� ������ �ĺ��� ������ �ð��� �߻��ϴ� Ŭ�� ����Ʋ ��
		if (ok)
		{
			// Tracking success : Draw the tracked object
			rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
			/* void rectangle(Mat& img, Rect rec, const Scalar& color, int thickness=1, int lineType=8, ins shift=0);
			�� �Լ��� ���� img�� �簢�� rec�� ���� ���ǵǴ� ���簢���� color �������� thickness �β��� �簢���� �׸��� ���̴�. */
		}
		else
		{
			// Tracking failure detected.
			putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
		}

		// Display tracker type on frame
		putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 50, 255), 2);

		// Display FPS on frame
		putText(frame, "FPS : " + SSTR(int(fps)), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 50, 255), 2);

		// Display frame.
		imshow("Create IDF Number ...", frame);
		waitKey(1);

		end = clock();
		if (check_t < 1) // blink period(��) ��ŭ ���徿 ���� �ϵ���.
		{
			check_t += (double)(end - begin) / CLOCKS_PER_SEC;
		}

		else if (check_t > 1) // blink period �� default �� 1��
		{
			*video >> image;
			check_t = 0;
			sprintf(image_name, "ROI_image%d.jpg", i_count);
			imwrite(image_name, image); // img�� ���Ϸ� �����Ѵ�.

			Mat sub_frame = imread(image_name);
			Mat sub_frame_image = sub_frame(roi);
			sprintf(image_name, "ROI_image%d.jpg", i_count); // ROI �� Ȯ���ؼ� ������.
			imwrite(image_name, sub_frame_image);

			//         Mat R_image = imread(image_name);
			Mat R_image = sub_frame_image;
			Mat mask, mask02, temp_mask;
			detectHScolor(R_image, 150, 200, 15, 255, mask);
			detectHScolor(R_image, 0, 10, 15, 255, mask02);
			temp_mask = mask | mask02;
			Mat detected(R_image.size(), CV_8UC3, cv::Scalar(0, 0, 0));

			R_image.copyTo(detected, temp_mask); // ����ũ�� ���� ���͸� �������� �ɷ���

			if (i_count == 0) Seq_Image1 = detected;
			else if (i_count == 1) Seq_Image2 = detected;
			else if (i_count == 2) Seq_Image3 = detected;
			else if (i_count == 3) Seq_Image4 = detected;
			else if (i_count == 4) Seq_Image5 = detected;
			else if (i_count == 5) Seq_Image6 = detected;
			else if (i_count == 6) Seq_Image7 = detected;
			else if (i_count == 7) Seq_Image8 = detected;
			else if (i_count == 8) Seq_Image9 = detected;
			else if (i_count == 9) { Seq_Image10 = detected; image_show = true; }

			sprintf(image_name, "G_C_ROI%d.jpg", i_count);
			imwrite(image_name, detected);

			drone_id[i_count] = encoding(temp_mask, i_count);

			if (i_count == 9) // ����� ������ 10���� ������
			{
				IDF_Message += drone_id;
				MessageBox(_T(IDF_Message), _T("Drone Identify Number"), MB_ICONINFORMATION);
				waitKey(1);
				//   break;
			}
			i_count++;
		}
	}
}

void CMFC_with_OpenCVDlg::OnBnClickedButton2() // ����� ��ȸ
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// use database drone & drone table�� ��� ������
	Text = "\t\t������ ���̽�\n";
	Text += '\n';

	if (mysql_query(conn, "SELECT * FROM drone"))
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}

	// mysql_store_result�Լ��� result set�� ��������
	// result set�� MYSQL_RES ����ü�� �����Ѵ�
	res = mysql_store_result(conn);

	if (res == NULL)
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}
	// mysql_num_fields �Լ��� ���̺��� Column ���� �˾Ƴ���

	int num_fields = mysql_num_fields(res);

	// row�� ���������� �ݺ��ؼ� row�� �������鼭
	// ������ ����ŭ row�� ������ �ֿܼ� ����Ѵ�

	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i<num_fields; i++)
		{
			if (i == 0 || i == 1)
				Text += " | ";

			if (*row[i] < 48 || *row[i] > 57)
				Text += " | ";

			printf("%s ", row[i] ? row[i] : "NULL");
			Text += row[i];
		}
		Text += " | ";
		Text += '\n';
	}

	m_Text.SetWindowText(Text);
	// result set�� ����(free)���ش�
	mysql_free_result(res);

}

void finish_with_error(MYSQL *conn)
{
	fprintf(stderr, "%s\n", mysql_error(conn));
	mysql_close(conn);
	exit(1);
}

void user_check(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row)
{
	// use database drone & drone table�� ��� ������

	if (mysql_query(conn, "SELECT * FROM drone"))
	{
		finish_with_error(conn);
	}

	// mysql_store_result�Լ��� result set�� ��������
	// result set�� MYSQL_RES ����ü�� �����Ѵ�
	res = mysql_store_result(conn);

	if (res == NULL)
	{
		finish_with_error(conn);
	}
	// mysql_num_fields �Լ��� ���̺��� Column ���� �˾Ƴ���

	int num_fields = mysql_num_fields(res);

	// row�� ���������� �ݺ��ؼ� row�� �������鼭
	// ������ ����ŭ row�� ������ �ֿܼ� ����Ѵ�

	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i<num_fields; i++)
		{
			printf("%s ", row[i] ? row[i] : "NULL");
		}
		printf("\n");
	}

	// result set�� ����(free)���ش�
	mysql_free_result(res);

}

int get_seq(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row) {

	int count = 0;

	if (mysql_query(conn, "SELECT * FROM drone"))
	{
		finish_with_error(conn);
	}

	res = mysql_store_result(conn);

	if (res == NULL)
	{
		finish_with_error(conn);
	}
	// mysql_num_fields �Լ��� ���̺��� Column ���� �˾Ƴ���

	int num_fields = mysql_num_fields(res);

	// row�� ���������� �ݺ��ؼ� row�� �������鼭
	// ������ ����ŭ row�� ������ �ֿܼ� ����Ѵ�

	while (row = mysql_fetch_row(res))
		count++;

	// result set�� ����(free)���ش�
	mysql_free_result(res);

	return count;
}

void create_query(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row) {}

void CMFC_with_OpenCVDlg::OnEnChangeName() // Edit
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialogEx::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CMFC_with_OpenCVDlg::OnBnClickedInput() // Input button
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	char c_query[100] = { 0 };
	CString name;
	CString Drone_ID_Length = drone_id;

	m_Name.GetWindowText(name);
	if (name.GetLength() > 30)
	{
		MessageBox(_T("�̸� ���̸� �ʰ� �ϼ̽��ϴ�. "), _T("Message Length Over"), MB_ICONERROR);
	}
	else if (Drone_ID_Length.GetLength() < 10)
		MessageBox(_T("��� �ĺ� �ڵ尡 �������� �ʾҽ��ϴ�. "));
	else if (name.IsEmpty() == TRUE)
		MessageBox(_T("�̸��� �Է����ּ��� "));

	else {
		sprintf(c_query, "insert into Drone values(%d, '%s', '%s')", get_seq(conn, res, row) + 1, drone_id, name);

		if (mysql_query(conn, c_query))
		{
			finish_with_error(conn);
			MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
		}
	}
}
// ID : IDC_IMAGE_SHOW 
void CMFC_with_OpenCVDlg::OnBnClickedImageShow() // �̹��� ����ϴ� ��ư.
{
	if (image_show == false)
		MessageBox(_T("��� �ĺ� �ڵ尡 ���� �ڿ� ���ּ���. "));

	else {
		imshow("Seq_Image 01", Seq_Image1);
		moveWindow("Seq_Image 01", 1000, 0);
		waitKey(10);
		imshow("Seq_Image 02", Seq_Image2);
		moveWindow("Seq_Image 02", 1000, 100);
		waitKey(10);
		imshow("Seq_Image 03", Seq_Image3);
		moveWindow("Seq_Image 03", 1000, 200);
		waitKey(10);
		imshow("Seq_Image 04", Seq_Image4);
		moveWindow("Seq_Image 04", 1000, 300);
		waitKey(10);
		imshow("Seq_Image 05", Seq_Image5);
		moveWindow("Seq_Image 05", 1000, 400);
		waitKey(10);
		imshow("Seq_Image 06", Seq_Image6);
		moveWindow("Seq_Image 06", 1130, 0);
		waitKey(10);
		imshow("Seq_Image 07", Seq_Image7);
		moveWindow("Seq_Image 07", 1130, 100);
		waitKey(10);
		imshow("Seq_Image 08", Seq_Image8);
		moveWindow("Seq_Image 08", 1130, 200);
		waitKey(10);
		imshow("Seq_Image 09", Seq_Image9);
		moveWindow("Seq_Image 09", 1130, 300);
		waitKey(10);
		imshow("Seq_Image 10", Seq_Image10);
		moveWindow("Seq_Image 10", 1130, 400);

		waitKey(10000);
		destroyAllWindows();
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CMFC_with_OpenCVDlg::OnEnChangeBlinkPeriod()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialogEx::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}


void CMFC_with_OpenCVDlg::OnStnClickedDronePrint()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CMFC_with_OpenCVDlg::OnBnClickedSearch()
{
	CString IDF_NUM;
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_idf_num.GetWindowText(IDF_NUM);
	CString IsSame;
	CString temp = NULL;
	int index = 0;
	bool Same = false;

	if (IDF_NUM.IsEmpty() == TRUE) {
		MessageBox(_T("Identification Number is empty"), _T("Error"), MB_ICONERROR);
	}
	else {

		// use database drone & drone table�� ��� ������
		Text = "\t\t������ ���̽�\n";
		Text += '\n';

		if (mysql_query(conn, "SELECT * FROM drone"))
		{
			finish_with_error(conn);
			MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
		}

		// mysql_store_result�Լ��� result set�� ��������
		// result set�� MYSQL_RES ����ü�� �����Ѵ�
		res = mysql_store_result(conn);

		if (res == NULL)
		{
			finish_with_error(conn);
			MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
		}
		// mysql_num_fields �Լ��� ���̺��� Column ���� �˾Ƴ���

		int num_fields = mysql_num_fields(res);

		// row�� ���������� �ݺ��ؼ� row�� �������鼭
		// ������ ����ŭ row�� ������ �ֿܼ� ����Ѵ�

		while (row = mysql_fetch_row(res))
		{	
			for (int i = 0; i<num_fields; i++)
			{
				if (i == 0 || i == 1)
					Text += " | ";

				if (*row[i] < 48 || *row[i] > 57)
					Text += " | ";

				printf("%s ", row[i] ? row[i] : "NULL");

				IsSame += row[i];
				Text += row[i];
			}

			for (int i = 0; i < IsSame.GetLength(); i++) {

				if (IsSame[i] == IDF_NUM[index]) {
					if (IsSame[i + 1] == IDF_NUM[index + 1]) {
						while (1)
						{
							temp += IsSame[i++];
							if (temp.GetLength() == 10)
								break;
						}
						if (temp.Compare(IDF_NUM) == 0)
							Same = true;
					}
				}			
			}

			Text += " | ";
			Text += '\n';
		}

		if (Same == true) {
			m_Text.SetWindowText(IsSame);
		}
		else {
			m_Text.SetWindowText(Text);
			MessageBox(_T("Search Error : No_User, Print All User"), _T("Error"), MB_ICONERROR);
		}
		
		// result set�� ����(free)���ش�
		mysql_free_result(res);

	}
}
