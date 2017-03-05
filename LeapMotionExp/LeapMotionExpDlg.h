
// LeapMotionExpDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "stringdata.h"

// CLeapMotionExpDlg 对话框
class CLeapMotionExpDlg : public CDialogEx
{
	// 构造
public:
	CLeapMotionExpDlg(CWnd* pParent = NULL);	// 标准构造函数

												// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LEAPMOTIONEXP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


														// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	void DrawPie(CDC *pDC, int centerx, int centery, int r1, int r2, FLOAT BeginAngle);
	void Hlight(CDC *pDC, int centerx, int centery, int part, int LightType);
	void DrawDot(CDC *pDC, int PointX, int PointY);
	float angle(int x1, int y1, int x2, int y2);
	void SetTrialTypes();

public:
#define RADIAN 3.1415926/180.0
#define RA1 100
#define RA2 200
	int R0 = 30, R1 = RA1, R2 = RA2;
	int Number;
	int DotRadius = 20;
	int DotRadiusList[5] = { 10,15,20,25,30 };//================================================
	double distance, olddistance;
	int showtype = 0, in_out_confirm = 0, inner = 0;
	int trialNumberCout;
	string ItemList[12] = { "的","是","这","说","和","到","里","来","把","看","要","很" };
	int TypeList[6] = { 2,3,4,6,8,12 };
	int confirmPart, oldPart, ID;
	int effects, waiting;
	float oldpiex, oldpiey, oldnumber;
	float rate = 0.6, RS;
	int Isfirst = 1;

public:
	int begin = 1, rest = 1, practise = 0;
	int blocknumber = 1;
	int Number_of_blocks = 2;//每个区组都是单独随机算法生成
	int ExpType = 1;//1为预实验
	int Pre_TrialNumber = 10, Formal_TrialNumber = 144;
	int pre = 1;
	int t1, t2, t3, ReactionTime, FirstRT;
	string filepath = "E:\\心理学\\心理课程文件\\短学期\\leapmotion项目\\Result\\LeapExp_";
	string items;
	int timergap = 30, waitTime = 300;//等待暂停
	int blanktime = 1000, Isnewtrail = 1;

public:
	CStatic m_picture;
	CStatic m_begin;
private:
	CFont begin_font;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
extern float sensitivity;
extern float px, py;
extern int isDetected;
extern int CenterX, CenterY;
extern float RightRate;

extern struct Result
{
	string Name[145];
	string Gender[145];
	string Age[145];
	string PieType[145];
	string RT[145];
	string Answer[145];
	string IsConsistent[145];
	string Response[145];
	string FirstResponse[145];
	string FirstRT[145];
	string TargetPos[145];
	string RequestResponse[145];
	string Items[145];
	string Light[145];
}result;