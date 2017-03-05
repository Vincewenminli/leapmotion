// TipDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LeapMotionExp.h"
#include "TipDlg.h"
#include "afxdialogex.h"


// CTipDlg �Ի���

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


// CTipDlg ��Ϣ�������
float RightRate=0;
BOOL CTipDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	CString str1, str2, str3,str;
	if (RightRate>80)
	{
		str1 = "����˳��ͨ����ϰ���֣���ϰ��ȷ��Ϊ";
		str3 = "%,���ڱ�֤��ȷ�ʵ�ǰ���¾����ܿ�ط�Ӧ�����ո��������ʽʵ��";
	}
	else
	{
		str1 = "����δͨ����ϰ���֣���ϰ��ȷ��Ϊ";
		str3 = "%,���ڱ�֤��ȷ�ʵ�ǰ���¾����ܿ�ط�Ӧ�����ո������";
	}
	str2.Format(_T("%.2f"), RightRate);
	str = str1 + str2 + str3;
	instructions_font.CreatePointFont(110, _T("���Ŀ���"), NULL);
	m_instructions.SetFont(&instructions_font, true);
	m_instructions.SetWindowTextW(str);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
