// MFC_with_OpenCVDlg.cpp : 구현 파일
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console") // 콘솔창 출력

#include "stdafx.h"
#include "MFC_with_OpenCV.h"
#include "MFC_with_OpenCVDlg.h"
#include "afxdialogex.h"

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

char drone_id[10]; // 10자리 드론 식별번호.
				   // 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
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

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

														// 구현입니다.
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

// CMFC_with_OpenCVDlg 대화 상자

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

// CMFC_with_OpenCVDlg 메시지 처리기

BOOL CMFC_with_OpenCVDlg::OnInitDialog()
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);         // 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);      // 작은 아이콘을 설정합니다.

								  // TODO: 여기에 추가 초기화 작업을 추가합니다.

	Name_UI_Text = "1. 드론 식별 번호 생성 \n2. 이름 입력 후 input 버튼 클릭\n"; // 초기 static text 지정.
	CString Temp_t = "3. 드론 식별 번호 -> 사진에 관심영역 지정후 Enter 키\n4. 이미지 확인 시 식별 번호 생성 후 할 것\n";
	CString Temp_t2 = "5. 드론 깜빡이는 시간을 설정하고 싶으면 값을 입력 후 식별 번호를 생성할 것";
	Name_UI_Text += Temp_t;
	Name_UI_Text += Temp_t2;
	m_NAME_TEXT.SetWindowText(Name_UI_Text);

	CString Period = "프레임 추출 간격 (default = 1 sec) : ";
	m_drone_text.SetWindowTextA(Period);

	Text = "\t\t데이터 베이스\n"; // 초기 static text 지정.
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

	SetTimer(1000, 30, NULL); // 100이 반복주기 0.1 초

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFC_with_OpenCVDlg::OnPaint()
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
HCURSOR CMFC_with_OpenCVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFC_with_OpenCVDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	split(hsv, channels); //HSV 채널 분리 channel[0] - 색상, channel[1] - 채도 , channel[2] - 명도
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

	threshold(channels[1], mask1, maxSat, 255, cv::THRESH_BINARY_INV); // 이진화
	threshold(channels[1], mask2, minSat, 255, cv::THRESH_BINARY);

	cv::Mat satMask;
	satMask = mask1& mask2;
	// 색깔 영역만 원본영상에서 copy
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
			if (image.at<unsigned char>(y, x) == 255) // 범위를 통과한 mask
			{
				if (i_count == 0) bound_count++;
				count++;
			}
		}
	} // for 문 괄호

	similarity = (float)count / float(pixel);

	if (i_count == 0) { 
		ratio = (float)bound_count / (float)pixel;
		return '1'; 
	}
	else if (i_count != 0 && (similarity > 0.5*ratio))
	{
		if (similarity > 4*ratio) // 지금 이미지의 빨간색 분포가 기준이 되는 이미지의 분포의 4배이상이 되면
		{
			bound_count = count;  
			ratio = (float)bound_count / (float)pixel;
			for (int k = 0; k < i_count; k++) {
				drone_id[k] = '0';
			}
		}
		return '1'; // 유사도 1/2 으로 설정.
	}
		
	else { 
		
		return '0';
	}	
}

void CMFC_with_OpenCVDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH] = { 0, }; // Init
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = NULL; // 필터 없이 모든 파일을 보여줌.
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (::GetOpenFileName(&ofn) == false) exit(1);

	video = new VideoCapture(ofn.lpstrFile);
	if (!video->isOpened())
	{
		MessageBox(_T("파일 열기가 안됨. \n"));
	}

	CString IDF_Message = "식별 번호  : \n";
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
	char image_name[200]; // 이름 200자 안으로.
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

	roi = selectROI(frame, false); // ROI 설정

								   // Display bounding box.
	rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
	//   imshow("Tracking", frame);
	//   waitKey(1);

	tracker->init(frame, roi);

	while (video->read(frame) && i_count <10)
	{
		begin = clock(); // 시간 측정 시작.

						 // Start timer
		double timer = (double)getTickCount();

		// Update the tracking result
		bool ok = tracker->update(frame, roi);

		// Calculate Frames per second (FPS)
		float fps = getTickFrequency() / ((double)getTickCount() - timer);
		// getTickFrequency() : 밀리초 단위로 코드 부분의 실행시간 알아냄
		// getTickCount() : 컴퓨터를 시작한 후부터 마지막 시간에 발생하는 클록 사이틀 수
		if (ok)
		{
			// Tracking success : Draw the tracked object
			rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
			/* void rectangle(Mat& img, Rect rec, const Scalar& color, int thickness=1, int lineType=8, ins shift=0);
			이 함수는 영상 img에 사각형 rec에 의해 정의되는 직사각형을 color 색상으로 thickness 두께로 사각형을 그리는 것이다. */
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
		if (check_t < 1) // blink period(초) 만큼 한장씩 저장 하도록.
		{
			check_t += (double)(end - begin) / CLOCKS_PER_SEC;
		}

		else if (check_t > 1) // blink period 의 default 는 1초
		{
			*video >> image;
			check_t = 0;
			sprintf(image_name, "ROI_image%d.jpg", i_count);
			imwrite(image_name, image); // img를 파일로 저장한다.

			Mat sub_frame = imread(image_name);
			Mat sub_frame_image = sub_frame(roi);
			sprintf(image_name, "ROI_image%d.jpg", i_count); // ROI 만 확대해서 덮어씌우기.
			imwrite(image_name, sub_frame_image);

			//         Mat R_image = imread(image_name);
			Mat R_image = sub_frame_image;
			Mat mask, mask02, temp_mask;
			detectHScolor(R_image, 150, 200, 15, 255, mask);
			detectHScolor(R_image, 0, 10, 15, 255, mask02);
			temp_mask = mask | mask02;
			Mat detected(R_image.size(), CV_8UC3, cv::Scalar(0, 0, 0));

			R_image.copyTo(detected, temp_mask); // 마스크에 원본 벡터를 투과시켜 걸러냄

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

			if (i_count == 9) // 저장된 사진이 10장이 넘을때
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

void CMFC_with_OpenCVDlg::OnBnClickedButton2() // 사용자 조회
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// use database drone & drone table을 모두 가져옴
	Text = "\t\t데이터 베이스\n";
	Text += '\n';

	if (mysql_query(conn, "SELECT * FROM drone"))
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}

	// mysql_store_result함수로 result set을 가져오고
	// result set을 MYSQL_RES 구조체에 저장한다
	res = mysql_store_result(conn);

	if (res == NULL)
	{
		finish_with_error(conn);
		MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
	}
	// mysql_num_fields 함수로 테이블의 Column 수를 알아낸다

	int num_fields = mysql_num_fields(res);

	// row가 없을때까지 반복해서 row를 가져오면서
	// 영역의 수만큼 row의 내용을 콘솔에 출력한다

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
	// result set을 해제(free)해준다
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
	// use database drone & drone table을 모두 가져옴

	if (mysql_query(conn, "SELECT * FROM drone"))
	{
		finish_with_error(conn);
	}

	// mysql_store_result함수로 result set을 가져오고
	// result set을 MYSQL_RES 구조체에 저장한다
	res = mysql_store_result(conn);

	if (res == NULL)
	{
		finish_with_error(conn);
	}
	// mysql_num_fields 함수로 테이블의 Column 수를 알아낸다

	int num_fields = mysql_num_fields(res);

	// row가 없을때까지 반복해서 row를 가져오면서
	// 영역의 수만큼 row의 내용을 콘솔에 출력한다

	while (row = mysql_fetch_row(res))
	{
		for (int i = 0; i<num_fields; i++)
		{
			printf("%s ", row[i] ? row[i] : "NULL");
		}
		printf("\n");
	}

	// result set을 해제(free)해준다
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
	// mysql_num_fields 함수로 테이블의 Column 수를 알아낸다

	int num_fields = mysql_num_fields(res);

	// row가 없을때까지 반복해서 row를 가져오면서
	// 영역의 수만큼 row의 내용을 콘솔에 출력한다

	while (row = mysql_fetch_row(res))
		count++;

	// result set을 해제(free)해준다
	mysql_free_result(res);

	return count;
}

void create_query(MYSQL *conn, MYSQL_RES *res, MYSQL_ROW row) {}

void CMFC_with_OpenCVDlg::OnEnChangeName() // Edit
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMFC_with_OpenCVDlg::OnBnClickedInput() // Input button
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	char c_query[100] = { 0 };
	CString name;
	CString Drone_ID_Length = drone_id;

	m_Name.GetWindowText(name);
	if (name.GetLength() > 30)
	{
		MessageBox(_T("이름 길이를 초과 하셨습니다. "), _T("Message Length Over"), MB_ICONERROR);
	}
	else if (Drone_ID_Length.GetLength() < 10)
		MessageBox(_T("드론 식별 코드가 생성되지 않았습니다. "));
	else if (name.IsEmpty() == TRUE)
		MessageBox(_T("이름을 입력해주세요 "));

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
void CMFC_with_OpenCVDlg::OnBnClickedImageShow() // 이미지 출력하는 버튼.
{
	if (image_show == false)
		MessageBox(_T("드론 식별 코드가 생성 뒤에 해주세요. "));

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

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMFC_with_OpenCVDlg::OnEnChangeBlinkPeriod()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMFC_with_OpenCVDlg::OnStnClickedDronePrint()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CMFC_with_OpenCVDlg::OnBnClickedSearch()
{
	CString IDF_NUM;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_idf_num.GetWindowText(IDF_NUM);
	CString IsSame;
	CString temp = NULL;
	int index = 0;
	bool Same = false;

	if (IDF_NUM.IsEmpty() == TRUE) {
		MessageBox(_T("Identification Number is empty"), _T("Error"), MB_ICONERROR);
	}
	else {

		// use database drone & drone table을 모두 가져옴
		Text = "\t\t데이터 베이스\n";
		Text += '\n';

		if (mysql_query(conn, "SELECT * FROM drone"))
		{
			finish_with_error(conn);
			MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
		}

		// mysql_store_result함수로 result set을 가져오고
		// result set을 MYSQL_RES 구조체에 저장한다
		res = mysql_store_result(conn);

		if (res == NULL)
		{
			finish_with_error(conn);
			MessageBox(_T("Database Query Error"), _T("Error"), MB_ICONERROR);
		}
		// mysql_num_fields 함수로 테이블의 Column 수를 알아낸다

		int num_fields = mysql_num_fields(res);

		// row가 없을때까지 반복해서 row를 가져오면서
		// 영역의 수만큼 row의 내용을 콘솔에 출력한다

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
		
		// result set을 해제(free)해준다
		mysql_free_result(res);

	}
}
