
// FaceRecognitionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FaceRecognition.h"
#include "FaceRecognitionDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// Training dialog
class CTrainingDlg : public CDialogEx
{
public:
	CTrainingDlg();

	// Dialog Data
	enum { IDD = IDD_TRAININGBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CEdit szName;
};

CTrainingDlg::CTrainingDlg() : CDialogEx(CTrainingDlg::IDD)
{
}

void CTrainingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, szName);
}

BEGIN_MESSAGE_MAP(CTrainingDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CTrainingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CTrainingDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CFaceRecognitionDlg dialog



CFaceRecognitionDlg::CFaceRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFaceRecognitionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFaceRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, btnRecognize);
}

BEGIN_MESSAGE_MAP(CFaceRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CFaceRecognitionDlg::OnBnClickedBtnRecognize)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON3, &CFaceRecognitionDlg::OnBnClickedBtnQuit)
	ON_BN_CLICKED(IDC_BUTTON2, &CFaceRecognitionDlg::OnBnClickedBtnTraining)
END_MESSAGE_MAP()


// CFaceRecognitionDlg message handlers

BOOL CFaceRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	// Initalize new thread of video when the program is started
	THREADSTRUCT *param = new THREADSTRUCT;
	param->m_pThis = this;
	hThread = AfxBeginThread(StartThread, param);
	g_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFaceRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFaceRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFaceRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFaceRecognitionDlg::DisplayImageToPictureCtrl(IplImage* pIplImg, CDC* pDC, CRect rect)
{
	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biXPelsPerMeter = 100;
	bitmapInfo.bmiHeader.biYPelsPerMeter = 100;
	bitmapInfo.bmiHeader.biClrUsed = 0;
	bitmapInfo.bmiHeader.biClrImportant = 0;
	bitmapInfo.bmiHeader.biSizeImage = 0;
	bitmapInfo.bmiHeader.biWidth = pIplImg->width;
	bitmapInfo.bmiHeader.biHeight = -pIplImg->height;

	IplImage* tempImage = NULL;

	tempImage = (IplImage*)cvClone(pIplImg);
	bitmapInfo.bmiHeader.biBitCount = tempImage->depth * tempImage->nChannels;

	if (tempImage != NULL)
	{
		pDC->SetStretchBltMode(COLORONCOLOR);
		StretchDIBits(pDC->GetSafeHdc(), rect.left, rect.top, rect.right, rect.bottom,
			0, 0, tempImage->width, tempImage->height, tempImage->imageData, &bitmapInfo,
			DIB_RGB_COLORS, SRCCOPY);

		// Free memmory of image
		cvReleaseImage(&tempImage);
	}
}


UINT CFaceRecognitionDlg::StartThread(LPVOID param)
{
	THREADSTRUCT  *ts = (THREADSTRUCT*)param;
	vector<Rect> faces;
	Mat frame;
	Mat gray;
	Mat original;
	CDC* vDC;
	CRect rect;

	// Variable for recognize the face
	vector<Mat> images;
	vector<int> labels;
	double confident = 0.0;
	stringstream conf;
	int prediction;
	int img_width = 0;
	int img_height = 0;
	Ptr<FaceRecognizer> model;

	if (g_bIsRecognizing)
	{
		try
		{
			read_csv(PATH_FILE, images, labels);
		}
		catch (cv::Exception& e)
		{
			cerr << "Error opening file haar\"" << "\". Reason: " << e.msg << endl;
			exit(1);
		}

		img_width = images[0].cols;
		img_height = images[0].rows;

		model = createEigenFaceRecognizer();
		model->train(images, labels);
	}

	CascadeClassifier haar_cascade;
	haar_cascade.load("haarcascade_frontalface_default.xml");

	// TODO: Add your control notification handler code here
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		AfxMessageBox(_T("Failed to load video file"));
	}

	while (true)
	{
		Rect face_i;
		Mat face;
		cap >> frame;
		if (!frame.empty())
		{
			original = frame.clone();
		}

		// Only run this statement when user click recognize button
		if (g_bIsRecognizing)
		{
			cvtColor(original, gray, CV_BGR2GRAY);
			equalizeHist(gray, gray);

			haar_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(110, 110));
			for (int i = 0; i < faces.size(); i++)
			{
				// Process face
				face_i = faces[i];

				// Crop the face from the image
				Mat face = gray(face_i);
				Mat face_resized;
				resize(face, face_resized, Size(img_width, img_height), 1.0, 1.0, INTER_CUBIC);

				model->predict(face_resized, prediction, confident);

				if (confident / 100 < 100){  // confident >= 30%
					rectangle(original, face_i, CV_RGB(0, 255, 0), 1);

					// Create the text we will annotate the box with:
					string box_text = getname_from_id(NAME_FILE, prediction);
					box_text += " (";
					box_text += to_string((int)(confident / 100));
					box_text += " %)";

					// Get position of the face 
					int pos_x = max(face_i.tl().x - 10, 0);
					int pos_y = max(face_i.tl().y - 10, 0);

					// Add the text into the image:
					putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
				}
				else
				{
					rectangle(original, face_i, CV_RGB(0, 255, 0), 1);

					string box_text = "unknown";
					int pos_x = max(face_i.tl().x - 10, 0);
					int pos_y = max(face_i.tl().y - 10, 0);
					putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0), 2.0);
				}

				// Draw a rectangle around the face when recognized it
				rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
			}
		}

		vDC = ts->m_pThis->GetDlgItem(IDC_VIDEO)->GetDC();

		ts->m_pThis->GetDlgItem(IDC_VIDEO)->GetClientRect(&rect);

		IplImage* image = cvCloneImage(&(IplImage)original);
		ts->m_pThis->DisplayImageToPictureCtrl(image, vDC, rect); 
		ts->m_pThis->ReleaseDC(vDC);
		cvReleaseImage(&image);

		cvWaitKey(30);
			
		DWORD dwRet = WaitForSingleObject(g_hKillEvent, 0);
		if (dwRet == WAIT_OBJECT_0) 
			break;
	}

	return 1;
}

void CFaceRecognitionDlg::OnBnClickedBtnRecognize()
{
	// TODO: Add your control notification handler code here
	
	// Destroy the current thread
	SetEvent(g_hKillEvent);
	DWORD dwRet = WaitForSingleObject(hThread->m_hThread, 1000);
	
	// Create new thread
	THREADSTRUCT *param = new THREADSTRUCT;
	param->m_pThis = this;
	hThread = AfxBeginThread(StartThread, param);
	g_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	g_bIsRecognizing = true;
}

// Catch event when user click x button on menu
void CFaceRecognitionDlg::OnClose()
{
	SetEvent(g_hKillEvent);
	// waite the thread to die
	DWORD dwRet = WaitForSingleObject(hThread->m_hThread, 1000);
	if (dwRet == WAIT_TIMEOUT)
	{
	}
	EndDialog(IDCANCEL);
}


void CFaceRecognitionDlg::OnBnClickedBtnQuit()
{
	// TODO: Add your control notification handler code here
	SetEvent(g_hKillEvent);
	// waite the thread to die
	DWORD dwRet = WaitForSingleObject(hThread->m_hThread, 1000);
	if (dwRet == WAIT_TIMEOUT)
	{
	}
	EndDialog(IDCANCEL);
}

void CFaceRecognitionDlg::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels)
{
	char separator = ';';

	ifstream file(filename.c_str(), ifstream::in);

	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}


string CFaceRecognitionDlg::getname_from_id(const string &filename, int id)
{
	string name, line, name_id;
	stringstream _id;
	_id << id;

	ifstream file(filename.c_str(), ifstream::in);
	while (getline(file, line)){
		stringstream liness(line);
		getline(liness, name_id, ';');
		if (name_id.compare(_id.str()) == 0)
		{
			getline(liness, name);
			break;
		}
	}
	return name;
}

int WriteDataToNameDB(const string &filename, string name)
{
	int  maxID = 0; // the max id of name in database
	string line;
	
	ifstream file(filename.c_str(), ifstream::in);
	while (getline(file, line)){
		maxID ++;
	}
	file.close();

	// add data to botton of the file
	ofstream file2(filename.c_str(), ifstream::app);
	file2 << to_string(maxID) << ";" << name << endl;
	file2.close();

	return maxID; // return id max
}

void WriteDataToPathFile(const string &filename, string name, int maxID)
{
	// add data to botton of the file
	int i;
	ofstream file2(filename.c_str(), ifstream::app);

	for (i = 1; i <= 20; i++)
	{
		file2 << "data/" << name << "/" << to_string(i) << ".png;" << to_string(maxID) << endl;
	}
	file2.close();
}


void CFaceRecognitionDlg::OnBnClickedBtnTraining()
{
	// TODO: Add your control notification handler code here
	CTrainingDlg dlgTraining;
	dlgTraining.DoModal();
}


void CTrainingDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CTrainingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	CString szYourName;
	szName.GetWindowTextW(szYourName);
	if (szYourName.IsEmpty())
	{
		MessageBox(_T("Name must not empty!"), _T("Alert"), MB_OK);
	}
	else
	{
		// Add data to name_db.txt and path.txt
		wstring wstr = (wstring)szYourName;
		name.assign(wstr.begin(), wstr.end());
		int maxID = WriteDataToNameDB(NAME_FILE, name);

		WriteDataToPathFile(PATH_FILE, name, maxID);

		// Change to data directory
		if (SetCurrentDirectory(_T("data")))
		{
			CreateDirectory(szYourName, NULL);
			MessageBox(_T("Begin recognized process!"), _T("Alert"), MB_OK);
			CDialogEx::OnCancel(); // Close training dialog
		}

		// Begin training
		// Destroy the current thread
		SetEvent(g_hKillEvent);
		DWORD dwRet = WaitForSingleObject(hThread->m_hThread, 1000);

		//cvWaitKey(5000);

		// Create new thread
		CFaceRecognitionDlg::THREADSTRUCT *param = new CFaceRecognitionDlg::THREADSTRUCT;
		param->m_pThis = (CFaceRecognitionDlg *)GetParent();
		hThread = AfxBeginThread(CFaceRecognitionDlg::TrainingThread, param);
		g_hKillEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		// Disable "recognize" button
		/*CWnd * btnRec = GetParent()->GetDlgItem(IDC_BUTTON1);
		btnRec->EnableWindow(FALSE);*/
	}
}

// Training thread
UINT CFaceRecognitionDlg::TrainingThread(LPVOID param)
{

	THREADSTRUCT  *ts = (THREADSTRUCT*)param;
	vector<Rect> faces;
	Mat frame;
	Mat gray;
	Mat crop;
	Mat res;
	Mat original;
	CDC* vDC;
	CRect rect;
	Rect roi;
	int fileNum = 1;
	string filename;
	bool finish = false;
	string text = "Completed: 0% / 100%";

	cvWaitKey(5000);

	// Change to main directory (contain main program)
	SetCurrentDirectory(_T("../"));

	CascadeClassifier haar_cascade;
	haar_cascade.load("haarcascade_frontalface_default.xml");

	// TODO: Add your control notification handler code here
	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		AfxMessageBox(_T("Failed to load video file"));
	}

	while (!finish)
	{
		Mat face;
		cap >> frame;
		if (!frame.empty())
		{
			original = frame.clone();
		}
	
		cvtColor(original, gray, CV_BGR2GRAY);
		equalizeHist(gray, gray);

		haar_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(110, 110));
		for (int i = 0; i < faces.size(); i++)
		{
			roi.x = faces[i].x;
			roi.y = faces[i].y;
			roi.width = faces[i].width;
			roi.height = faces[i].height;
			
			crop = original(roi);

			resize(crop, res, Size(190, 190), 0, 0, INTER_LINEAR); 
			cvtColor(res, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale
			
			filename = "data/" + name + "/" + to_string(fileNum) + ".png";
			imwrite(filename, gray);

			fileNum ++;

			text = ""; // reset value of string
			
			text += "Completed: ";
			text += to_string(fileNum * 5);
			text += "% / 100%";

			if (fileNum > 20)
			{
				finish = true;
				AfxMessageBox(_T("Training completed!"));
			}
		}

		// Display text to know percent of complete training
		if (!finish)
			putText(original, text, Point(30, 30), FONT_HERSHEY_PLAIN, 1, CV_RGB(0, 0, 255), 2);
	
		vDC = ts->m_pThis->GetDlgItem(IDC_VIDEO)->GetDC();
		ts->m_pThis->GetDlgItem(IDC_VIDEO)->GetClientRect(&rect);

		IplImage* image = cvCloneImage(&(IplImage)original);
		ts->m_pThis->DisplayImageToPictureCtrl(image, vDC, rect);
		ts->m_pThis->ReleaseDC(vDC);
		cvReleaseImage(&image);

		cvWaitKey(30);

		DWORD dwRet = WaitForSingleObject(g_hKillEvent, 0);
		if (dwRet == WAIT_OBJECT_0)
			break;
	}

	// Enable "recognize" button
	/*CWnd * btnRec = ts->m_pThis->GetDlgItem(IDC_BUTTON1);
	btnRec->EnableWindow(TRUE);*/

	return 1;
}


