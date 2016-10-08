
// FaceRecognitionDlg.h : header file
//

#pragma once

// Declare libraries of OpenCV and C++
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "afxwin.h"

#define PATH_FILE "path.txt"
#define NAME_FILE "name_db.txt"


using namespace cv;
using namespace std;

// Global variable
static HANDLE g_hKillEvent;
static bool g_bIsRecognizing = false;
static string name;

// Handle thread of video
static CWinThread *hThread = NULL;
static CWinThread *hTraining = NULL;

// CFaceRecognitionDlg dialog
class CFaceRecognitionDlg : public CDialogEx
{
// Construction
public:
	CFaceRecognitionDlg(CWnd* pParent = NULL);	// standard constructor

// Declare struct of own class
	typedef struct THREADSTRUCT
	{
		CFaceRecognitionDlg *m_pThis;
	} THREADSTRUCT;


// Dialog Data
	enum { IDD = IDD_FACERECOGNITION_DIALOG };

// Create new thread when window is loaded
	static UINT StartThread(LPVOID param);
// Create training thread
	static UINT TrainingThread(LPVOID param);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Display image on window (to picture control)
	void DisplayImageToPictureCtrl(IplImage* pIplImg, CDC* pDC, CRect rect);

// Read data file database
	static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels);

// Get name by id
	static string getname_from_id(const string &filename, int id);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRecognize();
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnQuit();
	afx_msg void OnBnClickedBtnTraining();
	afx_msg void OnBnClickedOk();
	CButton btnRecognize;
};
