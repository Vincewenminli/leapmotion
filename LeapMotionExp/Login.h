#pragma once
#include "afxwin.h"
#include "stringdata.h"

// CLogin �Ի���

class CLogin : public CDialogEx
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogin();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();
	CStatic m_instruction;
private:
	CFont instruction_font;
public:
	virtual BOOL OnInitDialog();
	CEdit m_edit1;
	CEdit m_edit2;
	CEdit m_edit3;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
