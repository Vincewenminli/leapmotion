// Login.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LeapMotionExp.h"
#include "Login.h"
#include "afxdialogex.h"


// CLogin �Ի���

IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Instruction, m_instruction);
	DDX_Control(pDX, IDC_EDIT1, m_edit1);
	DDX_Control(pDX, IDC_EDIT3, m_edit3);
}


BEGIN_MESSAGE_MAP(CLogin, CDialogEx)
	ON_BN_CLICKED(IDC_Login, &CLogin::OnBnClickedLogin)
END_MESSAGE_MAP()

struct Result
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
// CLogin ��Ϣ�������
void CLogin::OnBnClickedLogin()
{
	CString name;
	CString age;
	string Names;;
	string Genders;
	string Ages;
	GetDlgItemText(IDC_EDIT1, name);
	GetDlgItemText(IDC_EDIT3, age);
	UINT Selected = GetCheckedRadioButton(IDC_RADIO3, IDC_RADIO4);

	if (name.IsEmpty() || age.IsEmpty())
	{
		MessageBox(_T("��ĸ�����Ϣ������"), _T("��ʾ"));
		return;
	}
	//��������ʼ��
	result.Name[0] = { "Name" };
	result.Gender[0] = { "Gender" };
	result.Age[0] = { "Age" };
	result.PieType[0] = { "PieType" };
	result.RT[0] = { "ReactionTime" };
	result.Answer[0] = { "Answer" };
	result.IsConsistent[0] = { "IsConsistent" };
	result.Response[0] = { "Response" };
	result.FirstResponse[0] = { "FirstResponse" };
	result.FirstRT[0] = {"FirstRT"};
	result.RequestResponse[0] = { "RequestResponse" };
	result.Items[0] = { "ItemList" };
	result.TargetPos[0] = { "TargetPos" };
	result.Light[0] = {"CursorSize"};
	Names = (CStringA)name;
	Ages = (CStringA)age;
	for (int i = 1; i <= 144; i++)
	{
		result.Name[i] = Names;
		if (Selected == IDC_RADIO4)
		{
			result.Gender[i] = { "Female" };
		}
		else
		{
			result.Gender[i] = { "Male" };
		}
		result.Age[i] = Ages;
	}
	EndDialog(1);
}


BOOL CLogin::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	instruction_font.CreatePointFont(150, _T("���Ŀ���"), NULL);
	m_instruction.SetFont(&instruction_font, true);
	m_instruction.SetWindowTextW(_T("    ���ȸ�л���������ǵ���н�������ѧʵ�飬��л������������ѧ�о�������֧�֣�\n    ���ǵ�ʵ����һ������LeapMotion��н����豸��ʵ���о�������������ز���������������ɣ�\n     �ڴ�֮ǰ����������һЩ��Ҫ�ĸ�����Ϣ����Щ��Ϣ����ֻ���ڿ�ѧ�о����������ã�����ȫ���Է��ģ�"));
	//Ĭ������ѡ����
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

string CString2string(CString cstr)
{
	DWORD  num = WideCharToMultiByte(CP_ACP, 0, cstr, -1, NULL, 0, NULL, 0);
	char *cword;
	cword = (char*)calloc(num, sizeof(char));
	if (cword == NULL)
	{
		free(cword);
	}
	memset(cword, 0, num * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, cstr, -1, cword, num, NULL, 0);
	string ans = cword;
	free(cword);
	return ans;
}

BOOL CLogin::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		OnBnClickedLogin();
	}
	else
	{
		return CDialogEx::PreTranslateMessage(pMsg);
	}
}
