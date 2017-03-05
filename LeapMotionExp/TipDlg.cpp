// TipDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LeapMotionExp.h"
#include "TipDlg.h"
#include "afxdialogex.h"


// CTipDlg 对话框

IMPLEMENT_DYNAMIC(CTipDlg, CDialogEx)

CTipDlg::CTipDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

CTipDlg::~CTipDlg()
{
}

void CTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_tip, m_instructions);
}


BEGIN_MESSAGE_MAP(CTipDlg, CDialogEx)
END_MESSAGE_MAP()


// CTipDlg 消息处理程序
float RightRate=0;
BOOL CTipDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString str1, str2, str3,str;
	if (RightRate>80)
	{
		str1 = "你已顺利通过练习部分，练习正确率为";
		str3 = "%,请在保证正确率的前提下尽可能快地反应。按空格键进入正式实验";
	}
	else
	{
		str1 = "你尚未通过练习部分，练习正确率为";
		str3 = "%,请在保证正确率的前提下尽可能快地反应。按空格键继续";
	}
	str2.Format(_T("%.2f"), RightRate);
	str = str1 + str2 + str3;
	instructions_font.CreatePointFont(110, _T("华文楷体"), NULL);
	m_instructions.SetFont(&instructions_font, true);
	m_instructions.SetWindowTextW(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
