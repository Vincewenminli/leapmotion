
// LeapMotionExpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LeapMotionExp.h"
#include "LeapMotionExpDlg.h"
#include "afxdialogex.h"

#include "Login.h"
#include "TipDlg.h"

#include "leapdata.h"

#include <iostream>
#include <fstream>
#include <Windows.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeapMotionExpDlg 对话框

CLeapMotionExpDlg::CLeapMotionExpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LEAPMOTIONEXP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLeapMotionExpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Picture, m_picture);
	DDX_Control(pDX, IDC_begin, m_begin);
}

BEGIN_MESSAGE_MAP(CLeapMotionExpDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()
// CLeapMotionExpDlg 消息处理程序
SampleListener listener;
Controller controller;

struct TrailInfo///
{
	int Type;
	int TargetPos;
	string Target;
	string Item[13];
};
TrailInfo Trail[145];

BOOL CLeapMotionExpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);		
	SetIcon(m_hIcon, FALSE);
	CLogin Info;
	int Response1 = Info.DoModal();
	while (1)
	{
		if (Response1 == 1)
		{
			CRect rectPicture;
			::GetWindowRect(m_picture, rectPicture);
			ScreenToClient(rectPicture);
			//计算屏幕分辨率
			float m_nWidth = GetSystemMetrics(SM_CXSCREEN);
			float m_nHeight = GetSystemMetrics(SM_CYSCREEN);
			//全屏
			ModifyStyle(WS_CAPTION, 0, 0);
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
			//将控件全屏
			CPoint topLeftPoint, topRightPoint;
			topLeftPoint.x = long(0);
			topLeftPoint.y = long(0);
			topRightPoint.x = long(m_nWidth);
			topRightPoint.y = long(m_nHeight);
			rectPicture.SetRect(topLeftPoint, topRightPoint);
			GetDlgItem(IDC_Picture)->MoveWindow(rectPicture, TRUE);
			//创建监听器
			controller.addListener(listener);
			controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
			Sleep(10);
			break;
		}
		else
		{
			PostMessage(WM_CLOSE);
			break;
		}
	}
	return TRUE;
}

void CLeapMotionExpDlg::OnPaint()
{
	//呈现指导语
	CRect beginRect;
	::GetWindowRect(m_begin, beginRect);
	ScreenToClient(beginRect);
	//计算屏幕分辨率
	float m_nWidth = GetSystemMetrics(SM_CXSCREEN);
	float m_nHeight = GetSystemMetrics(SM_CYSCREEN);
	//将控件全屏
	CPoint topLeftPoint, topRightPoint;
	topLeftPoint.x = long(m_nWidth * 1 / 4);
	topLeftPoint.y = long(m_nHeight * 4 / 6);
	topRightPoint.x = long(m_nWidth * 3 / 4);
	topRightPoint.y = long(m_nHeight * 6 / 7);
	beginRect.SetRect(topLeftPoint, topRightPoint);
	GetDlgItem(IDC_begin)->MoveWindow(beginRect, TRUE);
	begin_font.CreatePointFont(200, _T("华文楷体"), NULL);
	m_begin.SetFont(&begin_font, true);
	m_begin.SetWindowTextW(_T(" 请按空格键开始，在主试指导下熟悉操作环境并对该体感设备进行调试！ 当你觉得对该操作环境已经相当熟悉并且清楚实验操作时请按空格进入正式实验....."));
	//初始状态为小圆
	CDC *pDC;
	pDC = m_picture.GetDC();
	//设置某些参数
	SetTrialTypes();
	Number = Trail[1].Type;
	for (int i = 1; i <= Number; i++)
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(110, 110, 110));
		pDC->SelectObject(&brush);
		pDC->SelectObject(GetStockObject(NULL_PEN));
		pDC->Ellipse(CenterX - R0, CenterY - R0, CenterX + R0, CenterY + R0);
		brush.DeleteObject();
	}
	ShowCursor(FALSE);
	CDialogEx::OnPaint();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLeapMotionExpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLeapMotionExpDlg::OnClose()
{
	if (practise == 1)
	{
		KillTimer(1);
	}
	else
	{
		KillTimer(2);
	}
	controller.removeListener(listener);
	CDialogEx::OnClose();
}

void CLeapMotionExpDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)//循环练习
	{
		CRect rectPicture;
		CPoint point;
		CDC *pDC;
		CDC bDC;
		CBitmap bmp;
		m_picture.GetClientRect(&rectPicture);
		pDC = m_picture.GetDC();
		bDC.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC,rectPicture.Width(),rectPicture.Height());
		bDC.SelectObject(&bmp);
		//重设背景
		CBrush br(GetSysColor(COLOR_BTNFACE));
		//pDC->FillRect(rectPicture, &br);
		bDC.FillRect(rectPicture, &br);
		//当检测到手时，leapmotion输入
		if (isDetected)
		{
			point.x = px;
			point.y = py;
		}
		else
		{
			point.x = 0;
			point.y = 0;
			showtype = 0;
		}
		distance = sqrt(pow(point.x - CenterX, 2) + pow(point.y - CenterY, 2));
		float PieCenterx = CenterX, PieCentery = CenterY;
		float beginAngle;
		int Part = 0, Type = 0;
		//区分外扩和内缩
		//大圆和小圆
		if (distance > R2)
		{
			if (olddistance < R2 && showtype == 1)
			{
				in_out_confirm = 1;
				SYSTEMTIME time;
				GetSystemTime(&time);
				t2 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;
				//赋值上一次的确认选项编号
				confirmPart = oldPart;
			}
			else if (olddistance > R2)
			{
				in_out_confirm = 0;
			}
			showtype = 0;
		}
		else
		{
			if (distance<R0 && showtype == 0)
			{
				showtype = 1;
				SYSTEMTIME time;
				GetSystemTime(&time);
				t1 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;;
			}
			//防止突然从外层回到里层:中间暂停的时候按按钮会出现这样的特殊情况
			if (olddistance > R2)
			{
				in_out_confirm = 0;
			}

		}
		if (Isnewtrail)
		{
			showtype = 0;
		}
		//保存本次距离数据，为下次判断作准备
		olddistance = distance;
		if (effects)
		{
			if (waiting)
			{
				//Hlight(pDC, oldpiex, oldpiey, confirmPart, 3);
				Hlight(&bDC, oldpiex, oldpiey, confirmPart, 3);
				pDC->BitBlt(rectPicture.left, rectPicture.top, rectPicture.Width(), rectPicture.Height(), &bDC, 0, 0, SRCCOPY);
				KillTimer(1);
				Sleep(waitTime);
				SetTimer(1, timergap, NULL);
				waiting = 0;
				RS = 1;
			}
			else
			{
				RS = RS*rate;
				oldpiex = CenterX + (oldpiex - CenterX)*RS;
				oldpiey = CenterY + (oldpiey - CenterY)*RS;
				R2 = R2*rate;
				R1 = R2 / 2;
				Number = oldnumber;
				//Hlight(pDC, oldpiex, oldpiey, confirmPart, 3);
				Hlight(&bDC, oldpiex, oldpiey, confirmPart, 3);
			}
			if (RS<0.01)
			{
				effects = 0;
				R1 = RA1;
				R2 = RA2;
				Number = Trail[trialNumberCout + 1].Type;
			}
		}
		else
		{
			//绘图（绘图不影响数据）
			if (showtype == 0)
			{
				for (int i = 1; i <= Number; i++)
				{
					CBrush brush;
					brush.CreateSolidBrush(RGB(200, 200, 200));
					//pDC->SelectObject(&brush);
					//pDC->SelectObject(GetStockObject(NULL_PEN));
					//pDC->Ellipse(CenterX - R0, CenterY - R0, CenterX + R0, CenterY + R0);
					bDC.SelectObject(&brush);
					bDC.SelectObject(GetStockObject(NULL_PEN));
					bDC.Ellipse(CenterX - R0, CenterY - R0, CenterX + R0, CenterY + R0);
					brush.DeleteObject();
				}
				//提示被试选择正确的选项
				if (Isnewtrail)
				{
					CFont font;
					font.CreatePointFont(180, _T("微软雅黑"));
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(0, 0, 0));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(0, 0, 0));
					CString str;
					str = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->SetBkMode(TRANSPARENT);
					//pDC->TextOut(CenterX - 13, CenterY - 18, str);
					bDC.SetBkMode(TRANSPARENT);
					bDC.TextOut(CenterX - 13, CenterY - 18, str);
					pDC->BitBlt(rectPicture.left, rectPicture.top, rectPicture.Width(), rectPicture.Height(), &bDC, 0, 0, SRCCOPY);
					KillTimer(1);
					Sleep(blanktime);
					SetTimer(1, timergap, NULL);
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(30, 144, 255));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(30, 144, 255));
					CString str1;
					str1 = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->TextOut(CenterX - 13, CenterY - 18, str1);
					bDC.TextOut(CenterX - 13, CenterY - 18, str1);
					Isnewtrail = 0;
				}
				else
				{
					CFont font;
					font.CreatePointFont(180, _T("微软雅黑"));
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(30, 144, 255));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(30, 144, 255));
					CString str;
					str = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->SetBkMode(TRANSPARENT);
					//pDC->TextOut(CenterX - 13, CenterY - 18, str);
					bDC.SetBkMode(TRANSPARENT);
					bDC.TextOut(CenterX - 13, CenterY - 18, str);
				}

			}
			else
			{
				//判断激活类型
				if (distance < R1 && distance>R0)
				{
					Type = 1;//激活
				}
				else if (distance >= R1 && distance <= R2)
				{
					Type = 2;//高亮
					PieCenterx = point.x - (point.x - CenterX)*R1 / distance;
					PieCentery = point.y - (point.y - CenterY)*R1 / distance;
					oldpiex = PieCenterx;///
					oldpiey = PieCentery;
				}
				//判断选项
				int Angle;
				Angle = angle(point.x - CenterX, point.y - CenterY, R1, 0);
				if (point.y < CenterY)
				{
					Part = 1 + int(Angle / (360 / Number));
				}
				else
				{
					Part = Number - int(Angle / (360 / Number));
				}
				//DarwPie
				if (Type == 0 || Type == 1)
				{
					for (int i = 1; i <= Number; i++)
					{
						ID = i;
						beginAngle = (i - 1) * 360 / Number;
						if (i == Part && Type != 0)
						{
							//Hlight(pDC, PieCenterx, PieCentery, Part, Type);
							Hlight(&bDC, PieCenterx, PieCentery, Part, Type);
						}
						else
						{
							//DrawPie(pDC, CenterX, CenterY, R1, R2, beginAngle);
							DrawPie(&bDC, CenterX, CenterY, R1, R2, beginAngle);
						}
					}
				}
				else if (Type == 2)
				{
					for (int i = 1; i <= Number; i++)
					{
						ID = i;
						beginAngle = (i - 1) * 360 / Number;
						if (i == Part && Type != 0)
						{
							//Hlight(pDC, PieCenterx, PieCentery, Part, Type);
							Hlight(&bDC, PieCenterx, PieCentery, Part, Type);
						}
						else
						{
							//DrawPie(pDC, CenterX, CenterY, R1, R2, beginAngle);
							DrawPie(&bDC, CenterX, CenterY, R1, R2, beginAngle);
						}
					}
					//第一次选择
					if ((!pre) & Isfirst)
					{
						result.FirstResponse[trialNumberCout + 1] = Trail[trialNumberCout + 1].Item[Part];
						stringstream ss0;
						SYSTEMTIME time;
						GetSystemTime(&time);
						t3 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;;
						FirstRT = t3 - t1;
						ss0 << FirstRT;
						ss0 >> result.FirstRT[trialNumberCout + 1];
						Isfirst = 0;
					}
				}
				oldPart = Part;
				oldnumber = Number;///
			}
			if (isDetected)
			{
				//DrawDot(pDC, point.x, point.y);
				DrawDot(&bDC, point.x, point.y);
			}
			if (in_out_confirm == 1)
			{
				effects = 1;
				waiting = 1;
				Isfirst = 1;
				Isnewtrail = 1;///
				trialNumberCout = trialNumberCout + 1;
				if (trialNumberCout == 144)
				{
					trialNumberCout = 0;
					SetTrialTypes();
					Number = Trail[1].Type;
				}
			}
		}
		pDC->BitBlt(rectPicture.left,rectPicture.top,rectPicture.Width(),rectPicture.Height(),&bDC,0,0,SRCCOPY);
		bmp.DeleteObject();
		//bDC.DeleteDC();
		ReleaseDC(&bDC);
		ReleaseDC(pDC);
	}
	else if (nIDEvent == 2)//正式阶段（包括小段10次练习）
	{

		CRect rectPicture;
		CPoint point;
		CDC *pDC;
		CDC bDC;
		CBitmap bmp;
		m_picture.GetClientRect(&rectPicture);
		pDC = m_picture.GetDC();
		bDC.CreateCompatibleDC(pDC);
		bmp.CreateCompatibleBitmap(pDC, rectPicture.Width(), rectPicture.Height());
		bDC.SelectObject(&bmp);
		//重设背景
		CBrush br(GetSysColor(COLOR_BTNFACE));
		//pDC->FillRect(rectPicture, &br);
		bDC.FillRect(rectPicture, &br);
		//当检测到手时，leapmotion输入
		if (isDetected)
		{
			point.x = px;
			point.y = py;
		}
		else
		{
			point.x = 0;
			point.y = 0;
			showtype = 0;
		}
		distance = sqrt(pow(point.x - CenterX, 2) + pow(point.y - CenterY, 2));
		float PieCenterx = CenterX, PieCentery = CenterY;
		float beginAngle;
		int Part = 0, Type = 0;
		//区分外扩和内缩
		//大圆和小圆
		if (distance > R2)
		{
			if (olddistance < R2 && showtype == 1)
			{
				in_out_confirm = 1;
				SYSTEMTIME time;
				GetSystemTime(&time);
				t2 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;
				//赋值上一次的确认选项编号
				confirmPart = oldPart;
			}
			else if (olddistance > R2)
			{
				in_out_confirm = 0;
			}
			showtype = 0;
		}
		else
		{
			if (distance<R0 && showtype == 0)
			{
				showtype = 1;
				SYSTEMTIME time;
				GetSystemTime(&time);
				t1 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;;
			}
			//防止突然从外层回到里层:中间暂停的时候按按钮会出现这样的特殊情况
			if (olddistance > R2)
			{
				in_out_confirm = 0;
			}

		}
		if (Isnewtrail)
		{
			showtype = 0;
		}
		//保存本次距离数据，为下次判断作准备
		olddistance = distance;
		if (effects)
		{
			if (waiting)
			{
				//Hlight(pDC, oldpiex, oldpiey, confirmPart, 3);
				Hlight(&bDC, oldpiex, oldpiey, confirmPart, 3);
				pDC->BitBlt(rectPicture.left, rectPicture.top, rectPicture.Width(), rectPicture.Height(), &bDC, 0, 0, SRCCOPY);
				KillTimer(2);
				Sleep(waitTime);
				SetTimer(2, timergap, NULL);
				waiting = 0;
				RS = 1;
			}
			else
			{
				RS = RS*rate;
				oldpiex = CenterX + (oldpiex - CenterX)*RS;
				oldpiey = CenterY + (oldpiey - CenterY)*RS;
				R2 = R2*rate;
				R1 = R2 / 2;
				Number = oldnumber;
				//Hlight(pDC, oldpiex, oldpiey, confirmPart, 3);
				Hlight(&bDC, oldpiex, oldpiey, confirmPart, 3);
			}
			if (RS<0.01)
			{
				effects = 0;
				R1 = RA1;
				R2 = RA2;
				Number = Trail[trialNumberCout + 1].Type;
			}
		}
		else
		{
			//绘图（绘图不影响数据）
			if (showtype == 0)
			{
				for (int i = 1; i <= Number; i++)
				{
					CBrush brush;
					brush.CreateSolidBrush(RGB(200, 200, 200));
					//pDC->SelectObject(&brush);
					//pDC->SelectObject(GetStockObject(NULL_PEN));
					//pDC->Ellipse(CenterX - R0, CenterY - R0, CenterX + R0, CenterY + R0);
					bDC.SelectObject(&brush);
					bDC.SelectObject(GetStockObject(NULL_PEN));
					bDC.Ellipse(CenterX - R0, CenterY - R0, CenterX + R0, CenterY + R0);
					brush.DeleteObject();
				}
				//提示被试选择正确的选项
				if (Isnewtrail)
				{
					CFont font;
					font.CreatePointFont(180, _T("微软雅黑"));
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(0, 0, 0));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(0, 0, 0));
					CString str;
					str = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->SetBkMode(TRANSPARENT);
					//pDC->TextOut(CenterX - 13, CenterY - 18, str);
					bDC.SetBkMode(TRANSPARENT);
					bDC.TextOut(CenterX - 13, CenterY - 18, str);
					pDC->BitBlt(rectPicture.left, rectPicture.top, rectPicture.Width(), rectPicture.Height(), &bDC, 0, 0, SRCCOPY);
					KillTimer(2);
					Sleep(blanktime);
					SetTimer(2, timergap, NULL);
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(30, 144, 255));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(30, 144, 255));
					CString str1;
					str1 = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->TextOut(CenterX - 13, CenterY - 18, str1);
					bDC.TextOut(CenterX - 13, CenterY - 18, str1);
					Isnewtrail = 0;
				}
				else
				{
					CFont font;
					font.CreatePointFont(180, _T("微软雅黑"));
					//pDC->SelectObject(&font);
					//pDC->SetTextColor(RGB(30, 144, 255));
					bDC.SelectObject(&font);
					bDC.SetTextColor(RGB(30, 144, 255));
					CString str;
					str = Trail[trialNumberCout + 1].Target.c_str();
					//pDC->SetBkMode(TRANSPARENT);
					//pDC->TextOut(CenterX - 13, CenterY - 18, str);
					bDC.SetBkMode(TRANSPARENT);
					bDC.TextOut(CenterX - 13, CenterY - 18, str);
				}

			}
			else
			{
				//判断激活类型
				if (distance < R1 && distance>R0)
				{
					Type = 1;//激活
				}
				else if (distance >= R1 && distance <= R2)
				{
					Type = 2;//高亮
					PieCenterx = point.x - (point.x - CenterX)*R1 / distance;
					PieCentery = point.y - (point.y - CenterY)*R1 / distance;
					oldpiex = PieCenterx;///
					oldpiey = PieCentery;
				}
				//判断选项
				int Angle;
				Angle = angle(point.x - CenterX, point.y - CenterY, R1, 0);
				if (point.y < CenterY)
				{
					Part = 1 + int(Angle / (360 / Number));
				}
				else
				{
					Part = Number - int(Angle / (360 / Number));
				}
				//DarwPie
				if (Type == 0 || Type == 1)
				{
					for (int i = 1; i <= Number; i++)
					{
						ID = i;
						beginAngle = (i - 1) * 360 / Number;
						if (i == Part && Type != 0)
						{
							//Hlight(pDC, PieCenterx, PieCentery, Part, Type);
							Hlight(&bDC, PieCenterx, PieCentery, Part, Type);
						}
						else
						{
							//DrawPie(pDC, CenterX, CenterY, R1, R2, beginAngle);
							DrawPie(&bDC, CenterX, CenterY, R1, R2, beginAngle);
						}
					}
				}
				else if (Type == 2)
				{
					for (int i = 1; i <= Number; i++)
					{
						ID = i;
						beginAngle = (i - 1) * 360 / Number;
						if (i == Part && Type != 0)
						{
							//Hlight(pDC, PieCenterx, PieCentery, Part, Type);
							Hlight(&bDC, PieCenterx, PieCentery, Part, Type);
						}
						else
						{
							//DrawPie(pDC, CenterX, CenterY, R1, R2, beginAngle);
							DrawPie(&bDC, CenterX, CenterY, R1, R2, beginAngle);
						}
					}
					//第一次选择
					if ((!pre) & Isfirst)
					{
						result.FirstResponse[trialNumberCout + 1] = Trail[trialNumberCout + 1].Item[Part];
						stringstream ss0;
						SYSTEMTIME time;
						GetSystemTime(&time);
						t3 = time.wDay * 24 * 60 * 60 * 1000 + time.wHour * 60 * 60 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;;
						FirstRT = t3 - t1;
						ss0 << FirstRT;
						ss0 >> result.FirstRT[trialNumberCout + 1];
						Isfirst = 0;
					}
				}
				oldPart = Part;
				oldnumber = Number;///
			}
			if (isDetected)
			{
				DrawDot(&bDC, point.x, point.y);
			}
			//判断操作，试次计数；输出文件
			if (pre)
			{
				//预实验
				if (in_out_confirm)
				{
					trialNumberCout = trialNumberCout + 1;
					if (confirmPart == Trail[trialNumberCout].TargetPos)
					{
						RightRate = RightRate + 1;
					}
					effects = 1;
					waiting = 1;
					Isfirst = 1;
					Isnewtrail = 1;///
				}
				if (effects == 0)
				{
					if (trialNumberCout == Pre_TrialNumber)
					{
						trialNumberCout = 0;
						RightRate = 100 * RightRate / Pre_TrialNumber;
						KillTimer(2);
						SetTrialTypes();//重新生成系列，防止练习相同和练习与正式相同
						Number = Trail[1].Type;//如果本次不通过，即下一次练习的第一个trail的number
						CTipDlg Pre_To_Formal;
						int res = Pre_To_Formal.DoModal();
						while (1)
						{
							if (res == IDCANCEL)
							{
								break;
							}
						}
						if (RightRate > 80)
						{
							pre = 0;
							RightRate = 0;
						}
						else
						{
							pre = 1;
							RightRate = 0;
						}
						SetTimer(2, timergap, NULL);
					}
				}

			}
			else if (pre == 0)
			{
				//正式
				if (in_out_confirm)
				{
					trialNumberCout = trialNumberCout + 1;
					ReactionTime = t2 - t1;
					if (confirmPart == Trail[trialNumberCout].TargetPos)
					{
						result.Answer[trialNumberCout] = "Right";
					}
					else
					{
						result.Answer[trialNumberCout] = "False";
					}
					stringstream ss, ss1, ss2, ss3;
					result.RequestResponse[trialNumberCout] = Trail[trialNumberCout].Target;
					result.Response[trialNumberCout] = Trail[trialNumberCout].Item[confirmPart];
					if (result.Response[trialNumberCout] == result.FirstResponse[trialNumberCout])
					{
						result.IsConsistent[trialNumberCout] = "Consistent";
					}
					else
					{
						result.IsConsistent[trialNumberCout] = "InConsistent";
					}
					ss1 << Number;
					ss1 >> result.PieType[trialNumberCout];
					ss2 << ReactionTime;
					ss2 >> result.RT[trialNumberCout];
					ss << Trail[trialNumberCout].TargetPos;
					ss >> result.TargetPos[trialNumberCout];
					ss3 << DotRadius;
					ss3 >> result.Light[trialNumberCout];
					for (int l = 1; l <= Trail[trialNumberCout].Type; l++)
					{
						items = items + Trail[trialNumberCout].Item[l] + ".";
					}
					result.Items[trialNumberCout] = items;
					items = "";
					//Number = Trail[trialNumberCout + 1].Type;
					effects = 1;///
					waiting = 1;
					Isfirst = 1;
					Isnewtrail = 1;///
					rest = 1;//当确认之后rest赋值为1
				}
				if (effects == 0)
				{
					if (trialNumberCout == Formal_TrialNumber / 4 && rest == 1)
					{
						blocknumber = 1;
					}
					else if (trialNumberCout == Formal_TrialNumber / 2 && rest==1)
					{
						blocknumber = 2;
					}
					else if (trialNumberCout == Formal_TrialNumber / 4 * 3 && rest == 1)
					{
						blocknumber = 3;
					}
					else if (trialNumberCout == Formal_TrialNumber)
					{
						blocknumber = 4;
					}
					else
					{
						blocknumber = 0;
					}
					if (blocknumber && rest==1)
					{
						rest = 0;
						KillTimer(2);
						fstream file;
						if (blocknumber == 1)
						{
							filepath = filepath + result.Name[1] + "_" + result.Gender[1] + "_" + result.Age[1] + ".csv";
							file.open(filepath, ios::app | ios::app);
							file << result.Name[0] << "," << result.Gender[0] << "," << result.Age[0] << "," << "Blocknumber" << ","<< result.Light[0] << "," << result.PieType[0] << "," << result.RT[0] << "," << result.Items[0] << "," << result.TargetPos[0] << "," << result.RequestResponse[0] << "," << result.FirstResponse[0] << "," << result.FirstRT[0] << "," << result.Response[0] << "," << result.IsConsistent[0] << "," << result.Answer[0] << endl;
							for (int i = 1; i <= Formal_TrialNumber/4; i++)
							{
								file << result.Name[i] << "," << result.Gender[i] << "," << result.Age[i] << "," << blocknumber << "," << result.Light[i] << "," << result.PieType[i] << "," << result.RT[i] << "," << result.Items[i] << "," << result.TargetPos[i] << "," << result.RequestResponse[i] << "," << result.FirstResponse[i] << "," << result.FirstRT[i] << "," << result.Response[i] << "," << result.IsConsistent[i] << "," << result.Answer[i] << endl;
							}
							file.close();
							MessageBox(_T("当前实验进度 1/4，请休息两分钟，活动并放松手臂，避免疲劳！休息结束后按空格继续"), _T("提示"), MB_OK);
							SetTimer(2, timergap, NULL);
						}
						else if (blocknumber == 2)
						{
							file.open(filepath, ios::app | ios::app);
							for (int i = 1 + Formal_TrialNumber / 4; i <= Formal_TrialNumber/2; i++)
							{
								file << result.Name[i] << "," << result.Gender[i] << "," << result.Age[i] << "," << blocknumber << "," << result.Light[i] << "," << result.PieType[i] << "," << result.RT[i] << "," << result.Items[i] << "," << result.TargetPos[i] << "," << result.RequestResponse[i] << "," << result.FirstResponse[i] << "," << result.FirstRT[i] << "," << result.Response[i] << "," << result.IsConsistent[i] << "," << result.Answer[i] << endl;
							}
							file.close();
							MessageBox(_T("当前实验进度 2/4，请休息两分钟，活动并放松手臂，避免疲劳！休息结束后按空格继续"), _T("提示"), MB_OK);
							SetTimer(2, timergap, NULL);
						}
						else if (blocknumber == 3)
						{
							file.open(filepath, ios::app | ios::app);
							for (int i = 1 + Formal_TrialNumber / 2; i <= Formal_TrialNumber/4 * 3; i++)
							{
								file << result.Name[i] << "," << result.Gender[i] << "," << result.Age[i] << "," << blocknumber << "," << result.Light[i] << "," << result.PieType[i] << "," << result.RT[i] << "," << result.Items[i] << "," << result.TargetPos[i] << "," << result.RequestResponse[i] << "," << result.FirstResponse[i] << "," << result.FirstRT[i] << "," << result.Response[i] << "," << result.IsConsistent[i] << "," << result.Answer[i] << endl;
							}
							file.close();
							MessageBox(_T("当前实验进度 3/4，请休息两分钟，活动并放松手臂，避免疲劳！休息结束后按空格继续"), _T("提示"), MB_OK);
							SetTimer(2, timergap, NULL);
						}
						else
						{
							file.open(filepath, ios::app | ios::app);
							for (int i = 1 + Formal_TrialNumber / 4 * 3; i <= Formal_TrialNumber; i++)
							{
								file << result.Name[i] << "," << result.Gender[i] << "," << result.Age[i] << "," << blocknumber << "," << result.Light[i] << "," << result.PieType[i] << "," << result.RT[i] << "," << result.Items[i] << "," << result.TargetPos[i] << "," << result.RequestResponse[i] << "," << result.FirstResponse[i] << "," << result.FirstRT[i] << "," << result.Response[i] << "," << result.IsConsistent[i] << "," << result.Answer[i] << endl;
							}
							file.close();
							MessageBox(_T("你已完成本实验所有试次，再次感谢您的参与！谢谢！按空格键退出！"), _T("提示"), MB_OK);
							ShowCursor(TRUE);
							controller.removeListener(listener);
							PostMessage(WM_CLOSE);
						}
					}
				}
			}
		}
		pDC->BitBlt(rectPicture.left, rectPicture.top, rectPicture.Width(), rectPicture.Height(), &bDC, 0, 0, SRCCOPY);
		bmp.DeleteObject();
		//bDC.DeleteDC();
		ReleaseDC(&bDC);
		ReleaseDC(pDC);
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CLeapMotionExpDlg::DrawPie(CDC * pDC, int centerx, int centery, int r1, int r2, FLOAT BeginAngle)
{
	int LeftInX, LeftInY, LeftOutX, LeftOutY;
	int RightInX, RightInY, RightOutX, RightOutY;

	FLOAT SwipeAngle = 360 / Number;
	//注意是以屏幕左上角为顶点，因此是y减去相应数值
	LeftInX = centerx + r1*cos(BeginAngle*RADIAN);
	LeftInY = centery - r1*sin(BeginAngle*RADIAN);
	LeftOutX = centerx + r2*cos(BeginAngle*RADIAN);
	LeftOutY = centery - r2*sin(BeginAngle*RADIAN);
	//角度转化
	RightInX = centerx + r1*cos((BeginAngle + SwipeAngle)* RADIAN);
	RightInY = centery - r1*sin((BeginAngle + SwipeAngle)* RADIAN);
	RightOutX = centerx + r2*cos((BeginAngle + SwipeAngle)* RADIAN);
	RightOutY = centery - r2*sin((BeginAngle + SwipeAngle)* RADIAN);

	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	pDC->SelectObject(&newPen);
	pDC->MoveTo(LeftInX, LeftInY);
	pDC->AngleArc(centerx, centery, r1, BeginAngle, SwipeAngle);
	pDC->MoveTo(LeftInX, LeftInY);
	pDC->AngleArc(centerx, centery, r2, BeginAngle, SwipeAngle);
	pDC->LineTo(RightInX, RightInY);
	//输出选项标号
	CFont textFont;
	CString str;
	pDC->SetBkMode(TRANSPARENT);
	textFont.CreatePointFont(180, _T("微软雅黑"));
	pDC->SelectObject(&textFont);
	//pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SetTextColor(RGB(30, 144, 255));
	str = Trail[trialNumberCout + 1].Item[ID].c_str();
	pDC->TextOut(centerx + (r1 + r2) / 2 * cos((BeginAngle + SwipeAngle / 2)*RADIAN), centery - (r1 + r2) / 2 * sin((BeginAngle + SwipeAngle / 2)*RADIAN), str);
	textFont.DeleteObject();
	newPen.DeleteObject();
}

void CLeapMotionExpDlg::Hlight(CDC * pDC, int centerx, int centery, int part, int LightType)
{
	//centerx为画圆的圆心坐标
	float xleft1, yleft1, xright1, yright1, Sx1, Sy1, Sx2, Sy2;
	float xleft2, yleft2, xright2, yright2, Mx1, My1, Mx2, My2;
	float BeginAngle, EndAngle;
	if (LightType == 1)
	{
		centerx = CenterX;
		centery = CenterY;
	}
	//小圆
	xleft1 = centerx - R1;
	yleft1 = centery - R1;
	xright1 = centerx + R1;
	yright1 = centery + R1;
	//大圆
	xleft2 = centerx - R2;
	yleft2 = centery - R2;
	xright2 = centerx + R2;
	yright2 = centery + R2;
	//坐标参数
	BeginAngle = (part - 1) * 360 / Number;
	EndAngle = part * 360 / Number;
	Sx1 = centerx + R1*cos(RADIAN*BeginAngle);
	Sy1 = centery - R1*sin(RADIAN*BeginAngle);
	Sx2 = centerx + R1*cos(RADIAN*EndAngle);
	Sy2 = centery - R1*sin(RADIAN*EndAngle);
	Mx1 = centerx + R2*cos(RADIAN*BeginAngle);
	My1 = centery - R2*sin(RADIAN*BeginAngle);
	Mx2 = centerx + R2*cos(RADIAN*EndAngle);
	My2 = centery - R2*sin(RADIAN*EndAngle);

	CBrush newBrush;
	CBrush newBrush2;
	CPen newPen;
	CPen newPen2;
	newBrush2.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	newPen2.CreatePen(PS_SOLID, 2, GetSysColor(COLOR_BTNFACE));//遮住多余的线
	if (LightType == 1)
	{
		newBrush.CreateSolidBrush(RGB(200, 200, 200));
	}
	else if (LightType == 2)
	{
		newBrush.CreateSolidBrush(RGB(100, 100, 100));
	}
	else if (LightType == 3)
	{
		newBrush.CreateSolidBrush(RGB(160, 255, 160));
	}
	else
	{
		newBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	}
	newPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	pDC->SelectObject(&newBrush);
	pDC->SelectObject(&newPen);
	pDC->Pie(xleft2, yleft2, xright2, yright2, Mx1, My1, Mx2, My2);
	pDC->SelectObject(&newBrush2);
	pDC->SelectObject(&newPen2);
	pDC->Pie(xleft1, yleft1, xright1, yright1, Sx1, Sy1, Sx2, Sy2);
	pDC->MoveTo(Sx1, Sy1);
	pDC->SelectObject(&newPen);
	pDC->AngleArc(centerx, centery, R1, BeginAngle, 360 / Number);
	//输出标号
	if ((LightType == 1) || (LightType == 2))
	{
		CFont textFont;
		CString str;
		pDC->SetBkMode(TRANSPARENT);
		textFont.CreatePointFont(180, _T("微软雅黑"));
		pDC->SelectObject(&textFont);
		//pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(30, 144, 255));
		str = Trail[trialNumberCout + 1].Item[part].c_str();
		pDC->TextOut(centerx + (R1 + R2) / 2 * cos((BeginAngle + EndAngle) / 2 * RADIAN), centery - (R1 + R2) / 2 * sin((BeginAngle + EndAngle) / 2 * RADIAN), str);
		textFont.DeleteObject();
	}
	newPen.DeleteObject();
	newBrush.DeleteObject();
	newBrush2.DeleteObject();
	newPen2.DeleteObject();
}

void CLeapMotionExpDlg::DrawDot(CDC * pDC, int PointX, int PointY)
{
	float xleft, yleft, xright, yright;
	xleft = PointX - DotRadius;
	yleft = PointY - DotRadius;
	xright = PointX + DotRadius;
	yright = PointY + DotRadius;
	CBrush newBrush;
	CPen newPen;
	newBrush.CreateSolidBrush(RGB(160, 255, 160));
	newPen.CreatePen(PS_NULL, 0, RGB(255, 255, 255));
	pDC->SelectObject(&newPen);
	pDC->SelectObject(&newBrush);
	pDC->Ellipse(xleft, yleft, xright, yright);
	newBrush.DeleteObject();
	newPen.DeleteObject();
}

float CLeapMotionExpDlg::angle(int x1, int y1, int x2, int y2)
{
	int ab = x1*x2 + y1*y2;
	float abs = sqrt(x1*x1 + y1*y1)*sqrt(x2*x2 + y2*y2);
	if (abs == 0)
	{
		return 0;
	}
	else
	{
		return acos(ab / abs) * 180 / 3.1415926;
	}
}

void CLeapMotionExpDlg::SetTrialTypes()
{
	srand(unsigned(time(NULL)));
	//预置整个实验条件设置,i为实验试次编号
	for (int i = 1; i <= 144; i++)
	{
		//设置选项个数
		if (i >= 1 && i <= 24)
		{
			Trail[i].Type = TypeList[0];
		}
		else if (i >= 25 && i <= 48)
		{
			Trail[i].Type = TypeList[1];
		}
		else if (i >= 49 && i <= 72)
		{
			Trail[i].Type = TypeList[2];
		}
		else if (i >= 73 && i <= 96)
		{
			Trail[i].Type = TypeList[3];
		}
		else if (i >= 97 && i <= 120)
		{
			Trail[i].Type = TypeList[4];
		}
		else if (i >= 121 && i <= 144)
		{
			Trail[i].Type = TypeList[5];
		}
		//靶目标位置
		if (i % 24 == 0)
		{
			if (Trail[i].Type == 2)
			{
				int pos[25] = { 0,1,2,1,2, 1,2, 1,2, 1,2, 1,2,1,2,1,2, 1,2, 1,2, 1,2, 1,2 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}
			}
			else if (Trail[i].Type == 3)
			{
				int pos[25] = { 0,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3,1,2,3 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}

			}
			else if (Trail[i].Type == 4)
			{
				int pos[25] = { 0,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4,1,2,3,4 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}

			}
			else if (Trail[i].Type == 6)
			{
				int pos[25] = { 0,1,2,3,4,5,6,1,2,3,4,5,6 ,1,2,3,4,5,6 ,1,2,3,4,5,6 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}

			}
			else if (Trail[i].Type == 8)
			{
				int pos[25] = { 0,1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8, 1,2,3,4,5,6,7,8 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}

			}
			else if (Trail[i].Type == 12)
			{
				int pos[25] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,1,2,3,4,5,6,7,8,9,10,11,12 };
				for (int m = 1; m <= 24; m++)
				{
					int index = 1 + rand() % 23;
					int tmp;
					tmp = pos[m];
					pos[m] = pos[index];
					pos[index] = tmp;
				}
				for (int j = i - 23; j <= i; j++)
				{
					Trail[j].TargetPos = pos[24 - i + j];
				}

			}
		}
		//设置靶目标内容
		int target;
		target = i % 12;
		if (target == 1)
		{
			Trail[i].Target = ItemList[0];
		}
		else if (target == 2)
		{
			Trail[i].Target = ItemList[1];
		}
		else if (target == 3)
		{
			Trail[i].Target = ItemList[2];
		}
		else if (target == 4)
		{
			Trail[i].Target = ItemList[3];
		}
		else if (target == 5)
		{
			Trail[i].Target = ItemList[4];
		}
		else if (target == 6)
		{
			Trail[i].Target = ItemList[5];
		}
		else if (target == 7)
		{
			Trail[i].Target = ItemList[6];
		}
		else if (target == 8)
		{
			Trail[i].Target = ItemList[7];
		}
		else if (target == 9)
		{
			Trail[i].Target = ItemList[8];
		}
		else if (target == 10)
		{
			Trail[i].Target = ItemList[9];
		}
		else if (target == 11)
		{
			Trail[i].Target = ItemList[10];
		}
		else if (target == 0)
		{
			Trail[i].Target = ItemList[11];
		}
	}
	//菜单其他内容
	for (int i = 1; i <= 144; i++)
	{
		if (Trail[i].Target == ItemList[0])
		{
			string List[12] = { "", "是","这","说","和","到","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[1])
		{
			string List[12] = { "","的","这","说","和","到","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[2])
		{
			string List[12] = { "","的","是","说","和","到","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[3])
		{
			string List[12] = { "","的","是","这","和","到","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[4])
		{
			string List[12] = { "","的","是","这","说","到","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[5])
		{
			string List[12] = { "","的","是","这","说","和","里","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[6])
		{
			string List[12] = { "","的","是","这","说","和","到","来","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[7])
		{
			string List[12] = { "","的","是","这","说","和","到","里","把","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[8])
		{
			string List[12] = { "","的","是","这","说","和","到","里","来","看","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[9])
		{
			string List[12] = { "","的","是","这","说","和","到","里","来","把","要","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[10])
		{
			string List[12] = { "","的","是","这","说","和","到","里","来","把","看","很" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
		else if (Trail[i].Target == ItemList[11])
		{
			string List[12] = { "","的","是","这","说","和","到","里","来","把","看","要" };
			for (int m = 1; m <= 11; m++)
			{
				int index = 1 + rand() % 10;
				string tmp;
				tmp = List[m];
				List[m] = List[index];
				List[index] = tmp;
			}
			int t = 0;
			for (int j = 1; j <= Trail[i].Type; j++)
			{
				if (t == 0)
				{
					if (j == Trail[i].TargetPos)
					{
						Trail[i].Item[j] = Trail[i].Target;
						j = j - 1;
						t = 1;
					}
					else
					{
						Trail[i].Item[j] = List[j];
					}

				}
				else
				{
					if (j == 12)
					{
						break;
					}
					Trail[i].Item[j + 1] = List[j];
				}

			}
		}
	}
	//打乱整体顺序
	for (int n = 1; n <= 144; n++)
	{
		TrailInfo tmp;
		int index;
		index = 1 + rand() % 143;
		tmp = Trail[n];
		Trail[n] = Trail[index];
		Trail[index] = tmp;
	}
}

BOOL CLeapMotionExpDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE)
	{
		if (begin == 1)
		{
			//开始进入循环练习阶段
			practise = 1;
			SetTimer(1, timergap, NULL);
			begin = 0;
		}
		else
		{
			if (practise == 1)
			{
				KillTimer(1);
				int ans;
				ans = MessageBox(_T("你确定你已经熟悉该体感操作方式了吗？按 是 进入正式实验，按 否 继续练习..."), _T("提示"), MB_YESNO | MB_ICONQUESTION);
				if (ans == IDYES)
				{
					practise = 0;
					MessageBox(_T("接下来的实验分为练习阶段和正式实验两个部分，请认真对待！"), _T("说明"), MB_OK);
					trialNumberCout = 0;
					SetTrialTypes();
					Number = Trail[1].Type;
					MessageBox(_T("现在是总共10个试次的练习阶段，只有达到一定正确率才能进入正式实验"), _T("提示"), MB_OK);
					SetTimer(2, timergap, NULL);
				}
				else
				{
					SetTimer(1, timergap, NULL);
				}

			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}
