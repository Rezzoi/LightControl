
// LightControlDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "LightControl.h"
#include "LightControlDlg.h"
#include "afxdialogex.h"
#include "ExternLightControlVIT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_VALUE_MONITOR					1
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLightControlDlg 대화 상자



CLightControlDlg::CLightControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LIGHTCONTROL_DIALOG, pParent)
	, m_pLamp(nullptr)
	, m_dwBaudrate(9600)
	, m_bCommOpen(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLightControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PORT, m_comboPort);
	DDX_Control(pDX, IDC_COMBO_BAUD_RATE, m_comboBaudrate);
	DDX_Control(pDX, IDC_STATIC_OPEN, m_staticIconOpen);
	DDX_Control(pDX, IDC_STATIC_FANALARM, m_staticIconFan);
	DDX_Control(pDX, IDC_STATIC_TEMPALARM, m_staticIconTemp);
	DDX_Control(pDX, IDC_EDIT_CHANNE1, m_edit1);
	DDX_Control(pDX, IDC_EDIT_CHENNEL2, m_edit2);
	DDX_Control(pDX, IDC_EDIT_CHENNEL3, m_edit3);
	DDX_Control(pDX, IDC_EDIT_LAMP1, m_editLamp1);
	DDX_Control(pDX, IDC_EDIT_CHANNEL4, m_edit4);
	DDX_Control(pDX, IDC_EDIT_CHENNEL5, m_edit5);
	DDX_Control(pDX, IDC_EDIT_CHENNEL6, m_edit6);
	DDX_Control(pDX, IDC_EDIT_CHANNEL7, m_edit7);
	DDX_Control(pDX, IDC_EDIT_LAMP2, m_editLamp2);
	DDX_Control(pDX, IDC_LIST_COMMAND, m_listCommand);
}

BEGIN_MESSAGE_MAP(CLightControlDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CLightControlDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_SET1, &CLightControlDlg::OnBnClickedButtonSet1)
	ON_BN_CLICKED(IDC_BUTTON_SET2, &CLightControlDlg::OnBnClickedButtonSet2)
	ON_BN_CLICKED(IDC_BUTTON_SET3, &CLightControlDlg::OnBnClickedButtonSet3)
	ON_BN_CLICKED(IDC_BUTTON_SETLAMP1, &CLightControlDlg::OnBnClickedButtonSetlamp1)
	ON_BN_CLICKED(IDC_BUTTON_SET4, &CLightControlDlg::OnBnClickedButtonSet4)
	ON_BN_CLICKED(IDC_BUTTON_SET5, &CLightControlDlg::OnBnClickedButtonSet5)
	ON_BN_CLICKED(IDC_BUTTON_SET6, &CLightControlDlg::OnBnClickedButtonSet6)
	ON_BN_CLICKED(IDC_BUTTON_SET7, &CLightControlDlg::OnBnClickedButtonSet7)
	ON_BN_CLICKED(IDC_BUTTON_SETLAMP2, &CLightControlDlg::OnBnClickedButtonSetlamp2)
	ON_BN_CLICKED(IDC_BUTTON_ON1, &CLightControlDlg::OnBnClickedButtonOn1)
	ON_BN_CLICKED(IDC_BUTTON_OFF1, &CLightControlDlg::OnBnClickedButtonOff1)
	ON_BN_CLICKED(IDC_BUTTON_ON2, &CLightControlDlg::OnBnClickedButtonOn2)
	ON_BN_CLICKED(IDC_BUTTON_OFF2, &CLightControlDlg::OnBnClickedButtonOff2)
	ON_BN_CLICKED(IDC_BUTTON_ON3, &CLightControlDlg::OnBnClickedButtonOn3)
	ON_BN_CLICKED(IDC_BUTTON_OFF3, &CLightControlDlg::OnBnClickedButtonOff3)
	ON_BN_CLICKED(IDC_BUTTON_ONLAMP1, &CLightControlDlg::OnBnClickedButtonOnlamp1)
	ON_BN_CLICKED(IDC_BUTTON_OFFLAMP1, &CLightControlDlg::OnBnClickedButtonOfflamp1)
	ON_BN_CLICKED(IDC_BUTTON_ON4, &CLightControlDlg::OnBnClickedButtonOn4)
	ON_BN_CLICKED(IDC_BUTTON_OFF4, &CLightControlDlg::OnBnClickedButtonOff4)
	ON_BN_CLICKED(IDC_BUTTON_ON5, &CLightControlDlg::OnBnClickedButtonOn5)
	ON_BN_CLICKED(IDC_BUTTON_OFF5, &CLightControlDlg::OnBnClickedButtonOff5)
	ON_BN_CLICKED(IDC_BUTTON_ON6, &CLightControlDlg::OnBnClickedButtonOn6)
	ON_BN_CLICKED(IDC_BUTTON_OFF6, &CLightControlDlg::OnBnClickedButtonOff6)
	ON_BN_CLICKED(IDC_BUTTON_ON7, &CLightControlDlg::OnBnClickedButtonOn7)
	ON_BN_CLICKED(IDC_BUTTON_OFF7, &CLightControlDlg::OnBnClickedButtonOff7)
	ON_BN_CLICKED(IDC_BUTTON_ONLAMP2, &CLightControlDlg::OnBnClickedButtonOnlamp2)
	ON_BN_CLICKED(IDC_BUTTON_OFFLAMP2, &CLightControlDlg::OnBnClickedButtonOfflamp2)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST_VALUE, &CLightControlDlg::OnBnClickedButtonRequestValue)
	ON_BN_CLICKED(IDC_BUTTON_REQUEST_ONOFF, &CLightControlDlg::OnBnClickedButtonRequestOnoff)
	ON_MESSAGE(WM_RECIEVE, &CLightControlDlg::OnRecieve)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLightControlDlg 메시지 처리기

BOOL CLightControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	//combo port
	CStringArray strArr;
	ReadSerialPort(strArr);
	int nCount = strArr.GetCount();

	if (nCount > 0)
	{
		for (int i = 0; i < nCount; i++)
		{
			CString strPort = strArr.GetAt(i);
			m_comboPort.AddString(strPort);
		}
		m_strPort = strArr.GetAt(0);
		m_comboPort.SetCurSel(0);
	}

	//baud rate
	strArr.RemoveAll();
	strArr.Add(_T("9600"));
	strArr.Add(_T("19200"));
	strArr.Add(_T("38400"));
	strArr.Add(_T("57600"));
	strArr.Add(_T("115200"));
	strArr.Add(_T("230400"));
	nCount = strArr.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CString strBaudrate = strArr.GetAt(i);
		m_comboBaudrate.AddString(strBaudrate);
	}
	m_dwBaudrate = (DWORD)_ttoi(strArr.GetAt(0));
	m_comboBaudrate.SetCurSel(0);

	m_staticIconOpen.SetColor(RGB(250, 0, 0));
	m_staticIconFan.SetColor(RGB(0, 250, 0));
	m_staticIconTemp.SetColor(RGB(0, 250, 0));

	m_pLamp = new CExternLightControlVIT();
	if (m_pLamp)
	{
		m_pLamp->SetInterface(static_cast<ILamp2Main*>(this));
		m_pLamp->SetParentWnd(this);
	}

	SetTimer(TIMER_VALUE_MONITOR, 1000, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLightControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLightControlDlg::OnPaint()
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
HCURSOR CLightControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLightControlDlg::ReadSerialPort(CStringArray& arrStr)
{
	DWORD dwcSubKeys, dwcValues, dwcMaxSubKeyName, dwcMaxValueName, dwcMaxValueData;
	TCHAR szBuffer[255];
	DWORD dwcSubKeyIndex = 0;
	DWORD dwValueIndex = 0;
	HKEY hKey = 0;
	LONG lStatus = ERROR_SUCCESS;
	TCHAR szValueName[255];
	TCHAR szValueData[255];
	CString szSubKey = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM");
	lStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_READ, &hKey);

	if (lStatus == ERROR_SUCCESS)
	{
		RegQueryInfoKey(hKey, szBuffer, NULL, 0, &dwcSubKeys, &dwcMaxSubKeyName, NULL, &dwcValues, &dwcMaxValueName, &dwcMaxValueData, NULL, NULL);

		do {
			DWORD dwType;
			DWORD dwcValueName = sizeof(szValueName);
			DWORD dwcValueData = sizeof(szValueData);

			lStatus = RegEnumValue(hKey, dwValueIndex, szValueName, &dwcValueName, NULL, &dwType, (BYTE*)szValueData, &dwcValueData);

			if (lStatus == ERROR_SUCCESS)
				arrStr.Add(szValueData);

			dwValueIndex++;
		} while (lStatus == ERROR_SUCCESS);
	}
}

void CLightControlDlg::AddMessage(CString strMsg)
{
	if (m_listCommand.GetCount() >= 50)
		m_listCommand.DeleteString(0);

	int nIndex = m_listCommand.AddString(strMsg);
	m_listCommand.SetCurSel(nIndex);
}

void CLightControlDlg::OnBnClickedButtonOpen()
{
	if (!m_pLamp)
		return;

	CString strMsg;
	strMsg.Format(_T("Try openning %s, Baudrate: 9600..."), m_strPort);
	AddMessage(strMsg);

	if (m_pLamp->OpenControl(m_strPort))
	{
		AddMessage(_T("Successfully open."));
		//m_pLamp->SetFanAlarmOnOff(TRUE);
		//m_pLamp->SetTemperatureAlarmOnOff(TRUE);
	}
}

void CLightControlDlg::OnOK()
{
	
}

void CLightControlDlg::OnCancel()
{
	if (m_pLamp)
		delete m_pLamp;

	m_pLamp = nullptr;

	CDialogEx::OnCancel();
}

void CLightControlDlg::SendCommand(CString strCommand)
{
	CString strMsg;
	strMsg.Format(_T("PC ---> Controler : %s"), strCommand);
	AddMessage(strMsg);
}

void CLightControlDlg::OnBnClickedButtonSet1()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit1.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(1, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet2()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit2.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(2, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet3()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit3.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(3, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSetlamp1()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_editLamp1.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(1, _ttoi(strValue));
	m_pLamp->SetLightControlValue(2, _ttoi(strValue));
	m_pLamp->SetLightControlValue(3, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet4()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit4.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(4, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet5()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit5.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(5, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet6()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit6.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(6, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSet7()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_edit7.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(7, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonSetlamp2()
{
	if (!m_pLamp)
		return;

	CString strValue;
	m_editLamp2.GetWindowTextW(strValue);
	m_pLamp->SetLightControlValue(4, _ttoi(strValue));
	m_pLamp->SetLightControlValue(5, _ttoi(strValue));
	m_pLamp->SetLightControlValue(6, _ttoi(strValue));
	m_pLamp->SetLightControlValue(7, _ttoi(strValue));
}

void CLightControlDlg::OnBnClickedButtonOn1()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(1, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff1()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(1, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn2()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(2, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff2()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(2, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn3()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(3, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff3()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(3, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOnlamp1()
{
	if (!m_pLamp)
		return;

	vector<int> vcChannel;
	vcChannel.push_back(1);
	vcChannel.push_back(2);
	vcChannel.push_back(3);

	m_pLamp->LightOnOff(vcChannel, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOfflamp1()
{
	if (!m_pLamp)
		return;

	vector<int> vcChannel;
	vcChannel.push_back(1);
	vcChannel.push_back(2);
	vcChannel.push_back(3);

	m_pLamp->LightOnOff(vcChannel, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn4()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(4, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff4()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(4, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn5()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(5, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff5()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(5, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn6()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(6, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff6()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(6, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOn7()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(7, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOff7()
{
	if (!m_pLamp)
		return;

	m_pLamp->LightOnOff(7, FALSE);
}

void CLightControlDlg::OnBnClickedButtonOnlamp2()
{
	if (!m_pLamp)
		return;

	vector<int> vcChannel;
	vcChannel.push_back(4);
	vcChannel.push_back(5);
	vcChannel.push_back(6);
	vcChannel.push_back(7);

	m_pLamp->LightOnOff(vcChannel, TRUE);
}

void CLightControlDlg::OnBnClickedButtonOfflamp2()
{
	if (!m_pLamp)
		return;

	vector<int> vcChannel;
	vcChannel.push_back(4);
	vcChannel.push_back(5);
	vcChannel.push_back(6);
	vcChannel.push_back(7);

	m_pLamp->LightOnOff(vcChannel, FALSE);
}

void CLightControlDlg::OnBnClickedButtonRequestValue()
{
	if (!m_pLamp)
		return;

	for (int i = 1; i <= 8; i++)
	{
		m_pLamp->RequestValue(i);
	}
}

void CLightControlDlg::OnBnClickedButtonRequestOnoff()
{
	if (!m_pLamp)
		return;

	m_pLamp->RequestOnOff();
}

LRESULT CLightControlDlg::OnRecieve(WPARAM wParam, LPARAM lParam)
{
	CString* pMsg = (CString*)lParam;
	CString strMsg;
	if (pMsg)
	{
		strMsg = *pMsg;
		delete pMsg;
		pMsg = nullptr;
	}

	TRACE(_T("Main recieve message %s.\n"), strMsg);

	strMsg.Format(_T("Controler ---> PC : %s"), strMsg);
	AddMessage(strMsg);

	return 0;
}

void CLightControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case TIMER_VALUE_MONITOR:
	{
		if (!m_pLamp)
			return;

		BOOL bOpen = m_pLamp->IsConnect();
		if (bOpen != m_bCommOpen)
		{
			m_bCommOpen = bOpen;
			if (m_bCommOpen)
				m_staticIconOpen.SetColor(RGB(0, 250, 0));
			else 
				m_staticIconOpen.SetColor(RGB(250, 0, 0));
		}

		UINT nID = IDC_STATIC_VALUE1;
		for (int i = 1; i <= 7; i++)
		{
			CString strValue;
			strValue.Format(_T("%d"), m_pLamp->GetLightControlValue(i));
			if (GetDlgItem(nID))
				GetDlgItem(nID)->SetWindowTextW(strValue);
			nID++;
		}
	}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
