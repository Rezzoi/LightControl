
// LightControlDlg.h: 헤더 파일
//

#pragma once
#include "LightControlBase.h"
#include "UIStaticColorIcon.h"

// CLightControlDlg 대화 상자
class CLightControlDlg : public CDialogEx , public ILamp2Main
{
// 생성입니다.
public:
	CLightControlDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTCONTROL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	virtual void SendCommand(CString strCommand) override;

protected:
	void ReadSerialPort(CStringArray& arrStr);
	void AddMessage(CString strMsg);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonSet1();
	afx_msg void OnBnClickedButtonSet2();
	afx_msg void OnBnClickedButtonSet3();
	afx_msg void OnBnClickedButtonSetlamp1();
	afx_msg void OnBnClickedButtonSet4();
	afx_msg void OnBnClickedButtonSet5();
	afx_msg void OnBnClickedButtonSet6();
	afx_msg void OnBnClickedButtonSet7();
	afx_msg void OnBnClickedButtonSetlamp2();
	afx_msg void OnBnClickedButtonOn1();
	afx_msg void OnBnClickedButtonOff1();
	afx_msg void OnBnClickedButtonOn2();
	afx_msg void OnBnClickedButtonOff2();
	afx_msg void OnBnClickedButtonOn3();
	afx_msg void OnBnClickedButtonOff3();
	afx_msg void OnBnClickedButtonOnlamp1();
	afx_msg void OnBnClickedButtonOfflamp1();
	afx_msg void OnBnClickedButtonOn4();
	afx_msg void OnBnClickedButtonOff4();
	afx_msg void OnBnClickedButtonOn5();
	afx_msg void OnBnClickedButtonOff5();
	afx_msg void OnBnClickedButtonOn6();
	afx_msg void OnBnClickedButtonOff6();
	afx_msg void OnBnClickedButtonOn7();
	afx_msg void OnBnClickedButtonOff7();
	afx_msg void OnBnClickedButtonOnlamp2();
	afx_msg void OnBnClickedButtonOfflamp2();
	afx_msg void OnBnClickedButtonRequestValue();
	afx_msg void OnBnClickedButtonRequestOnoff();
	afx_msg LRESULT OnRecieve(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	CLightControlBase* m_pLamp;
	CString m_strPort;
	DWORD m_dwBaudrate;
	BOOL m_bCommOpen;

	CComboBox m_comboPort;
	CComboBox m_comboBaudrate;
	CListBox m_listCommand;
	CEdit m_editLamp1;
	CEdit m_editLamp2;
	CUIStaticColorIcon m_staticIconOpen;
	CUIStaticColorIcon m_staticOnOff[7];
	CEdit m_edit1;
	CEdit m_edit2;
	CEdit m_edit3;
	CEdit m_edit4;
	CEdit m_edit5;
	CEdit m_edit6;
	CEdit m_edit7;
};
