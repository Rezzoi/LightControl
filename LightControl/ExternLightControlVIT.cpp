#include "pch.h"
#include "ExternLightControlVIT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ASCII_CR			0x0D
#define ASCII_LF			0x0A
#define COMMAND_A			0x41	// D'A'TA
#define COMMAND_C			0x43	// Output Data Change & Output on
#define COMMAND_D			0x44	// Only Output Data Change
#define COMMAND_ERROR		0x45	// 'E'rror
#define COMMAND_G			0x47	// 'G'roup
#define COMMAND_O			0x4F	// 'O'n/'O'ff
#define COMMAND_M			0x4D	// 'M'ake
#define COMMAND_N			0x4E	// o'N'
#define COMMAND_F			0x46	// Of'F'
#define COMMAND_R			0x52	// Request
#define COMMAND_T			0x54	// DA'T'A
#define BUFFER_SIZE			7

char cInt2Hex[17] = { 0x30, 0x31, 0x32, 0x33, 0x34,
					 0x35, 0x36, 0x37, 0x38, 0x39,
					 0x41, 0x42, 0x43, 0x44, 0x45,
					 0x46, 0x47 };	// 0~9, A~G


CExternLightControlVIT::CExternLightControlVIT(void)
	:m_nResvLightValue(0)
	, m_nStartIdx(0)
	, m_nRecvIdx(0)
	, m_nTemperature(0)
	, m_bAlarmLampOff(0)
	, m_evtStatus(FALSE, FALSE)
	, m_evtExit(FALSE, FALSE)
{
	m_bCtrlTurnOn = FALSE;
}


CExternLightControlVIT::~CExternLightControlVIT(void)
{
	m_evtExit.SetEvent();
}

BOOL CExternLightControlVIT::OpenControl(CString strPort, DWORD dwBaudrate)
{
	if (CLightControlBase::OpenControl(strPort))
	{
		m_serial.SetInterface(static_cast<ISerial2Lamp*>(this));
		AfxBeginThread(ThreadLampStatus, this);
		m_evtStatus.SetEvent();
		return TRUE;
	}

	return FALSE;
}

BOOL CExternLightControlVIT::SetLightControlValue( int nChannel, int nValue )
{
	BYTE OutBuf[10];
	int nSize = 0;

	nSize = SetLightValue(OutBuf, nChannel, nValue);

	CString strcomm((LPCSTR)OutBuf, 5);
	if (m_pL2M)
		m_pL2M->SendCommand(strcomm);

	CSingleLock lock(&m_sectionCommandSend, TRUE);
	if(!m_serial.Send(OutBuf, nSize))
		return FALSE;	
	
	m_evtStatus.SetEvent();

	return TRUE;
}

int CExternLightControlVIT::GetLightControlValue( int nChannel )
{
	if (nChannel < 1 || nChannel > 16)
		return -1;

	CSingleLock lock(&m_sectionValue, TRUE);
	return m_arrLightValue[nChannel - 1];
}

BOOL CExternLightControlVIT::GetLightOnStatus( int nChannel )
{
	if (nChannel < 1 || nChannel > 16)
		return -1;

	CSingleLock lock(&m_sectionStatus, TRUE);
	return m_arrLampOn[nChannel - 1];
}

BOOL CExternLightControlVIT::IsUsingLightLamp( int nChannel )
{
	if (!m_serial.IsOpened())
		return FALSE;

	if (!m_arrLampOn[nChannel])
		return FALSE;

	if (m_arrLightValue[nChannel - 1] > 0)
		return TRUE;

	return FALSE;
}

void CExternLightControlVIT::ProcessPacket(string recieve)
{
	TRACE(_T("Receive data\n"));
	CString strComm(recieve.c_str(), 5);

	int nSize = recieve.size();
	if (nSize <= 0)
	{
		return;
	}

	if (recieve[0] == COMMAND_R)
	{
		int nChannel = stoi(recieve.substr(1, 1));
		int nValue = stoi(recieve.substr(2, 3));

		if (nChannel >= 1 && nChannel <= 16)
		{
			CSingleLock lock(&m_sectionValue, TRUE);
			m_arrLightValue[nChannel - 1] = nValue;
		}

		if (nChannel == 7)
		{
			if (m_pParent)
				PostMessageW(m_pParent->GetSafeHwnd(), WM_UPDATEVIEW, TRUE, NULL);
		}
	}
	else if (recieve[0] == COMMAND_O)
	{
		BYTE nChannelHigh = recieve[3];
		BYTE nChannelLow = recieve[4];

		CSingleLock lock(&m_sectionStatus, TRUE);

		for (int i = 0; i < 8; ++i)
		{
			BOOL bOn = (nChannelLow >> i) & 1;
			m_arrLampOn[i] = bOn;
		}

		for (int i = 0; i < 8; ++i)
		{
			BOOL bOn = (nChannelHigh >> i) & 1;
			m_arrLampOn[i + 8] = bOn;
		}

		strComm = BufferToStatusString((LPBYTE)recieve.c_str(), nChannelHigh, nChannelLow);
	}

	CString* pMsg = new CString;
	*pMsg = strComm;
	if (m_pParent)
		PostMessageW(m_pParent->GetSafeHwnd(), WM_RECIEVE, NULL, (LPARAM)pMsg);
}

int CExternLightControlVIT::ConvertToHexData(CString strValue)
{
	int nLength = strValue.GetLength();
	TCHAR* tValue = (TCHAR*)(LPCTSTR)strValue;
	int nValue = 0;

	for (int nIdx = 0; nIdx < nLength; nIdx++)
	{
		TCHAR tTemp = 0;
		if (tValue[nIdx] >= L'0' && tValue[nIdx] <= L'9')
			tTemp = (tValue[nIdx] - 0x0030) & 0x000F;
		else if (tValue[nIdx] >= 'A' && tValue[nIdx] <= L'F')
			tTemp = (tValue[nIdx] - 0x0037) & 0x000F;
		else
			return 0;

		nValue |= (tTemp << ((nLength - 1) - nIdx) * 4);
	}

	return nValue;
}

BOOL CExternLightControlVIT::Initialize()
{
	return m_bCtrlTurnOn;
}

BOOL CExternLightControlVIT::SetFanAlarmOnOff(BOOL bOnOff)
{
	if(!m_serial.IsOpened())
		return FALSE;
		
	BYTE OutBuf[10];

	memset(OutBuf, 0, sizeof(OutBuf));
	sprintf_s((char*)OutBuf, sizeof(OutBuf), "UFAN%d%c%c", (bOnOff==TRUE)?1:0, ASCII_CR, ASCII_LF);

	if(!m_serial.Send(OutBuf, BUFFER_SIZE))
		return FALSE;
	
	return TRUE;
}

BOOL CExternLightControlVIT::SetTemperatureAlarmOnOff(BOOL bOnOff)
{
	if(!m_serial.IsOpened())
		return FALSE;

	BYTE OutBuf[10];

	memset(&OutBuf[0], 0x00, sizeof(OutBuf));
	sprintf_s((char*)OutBuf, sizeof(OutBuf), "UTEM%d%c%c", (bOnOff==TRUE) ? 1 : 0, ASCII_CR, ASCII_LF);

	if(!m_serial.Send(OutBuf, BUFFER_SIZE))
		return FALSE;

	return TRUE;
}

void CExternLightControlVIT::LightOnOff(int nChannel, BOOL bOn)
{
	BYTE OutBuf[10] = { 0 };

	if (bOn)
		SetSingleLightTurnOn(OutBuf, nChannel);
	else
		SetSingleLightTurnOff(OutBuf, nChannel);
	
	CSingleLock lock(&m_sectionCommandSend, TRUE);
	m_serial.Send(OutBuf, BUFFER_SIZE);

	m_evtStatus.SetEvent();
}

void CExternLightControlVIT::LightOnOff(vector<int> vcChannel, BOOL bOn)
{
	BYTE dataHight = 0;
	BYTE dataLow = 0;

	for (int i = 0; i < vcChannel.size(); i++)
	{
		int nChannel = vcChannel[i];

		if (nChannel >= 1 && nChannel <= 8)
		{
			dataLow |= (1 << (nChannel - 1));
		}
		else if (nChannel >= 9 && nChannel <= 16)
		{
			dataHight |= (1 << (nChannel % 9));
		}
		else
			return;
	}

	BYTE OutBuf[10] = { 0 };

	if (bOn)
		MakeInstruction(OutBuf, COMMAND_O, COMMAND_N, COMMAND_N, dataHight, dataLow);
	else
		MakeInstruction(OutBuf, COMMAND_O, COMMAND_F, COMMAND_F, dataHight, dataLow);

	CSingleLock lock(&m_sectionCommandSend, TRUE);
	m_serial.Send(OutBuf, BUFFER_SIZE);

	m_evtStatus.SetEvent();

	if (m_pL2M)
	{
		CString strMsg = BufferToStatusString(OutBuf, dataHight, dataLow);
		m_pL2M->SendCommand(strMsg);
	}
}

void CExternLightControlVIT::RequestValue(int nChannel)
{
	BYTE OutBuf[10];
	RequestChannelData(OutBuf, nChannel);

	CSingleLock lock(&m_sectionCommandSend, TRUE);
	m_serial.Send(OutBuf, BUFFER_SIZE);

	if (m_pL2M)
	{
		CString strMsg((LPCSTR)OutBuf, 5);
		m_pL2M->SendCommand(strMsg);
	}
}

void CExternLightControlVIT::RequestOnOff()
{
	BYTE OutBuf[10];
	RequestChannelTurnAll(OutBuf);

	CSingleLock lock(&m_sectionCommandSend, TRUE);
	m_serial.Send(OutBuf, BUFFER_SIZE);

	if (m_pL2M)
	{
		CString strMsg((LPCSTR)OutBuf, 5);
		m_pL2M->SendCommand(strMsg);
	}
}

BOOL CExternLightControlVIT::Reset()
{
	if(!m_serial.IsOpened())
		return FALSE;

	BYTE OutBuf[10];
	int nSize = 0;

	memset(&OutBuf[0], 0x00, sizeof(OutBuf));
	sprintf_s((char*)OutBuf, sizeof(OutBuf), "reset%c%c", ASCII_CR, ASCII_LF);

	nSize = strlen((char*)OutBuf);
	if(!m_serial.Send(OutBuf, nSize))
		return FALSE;

	return TRUE;
}

int CExternLightControlVIT::SetLightValueOn(LPBYTE lpBuffer, int nChannel, int nValue)
{
	if( nValue < 0 || nValue > 255 )
		return -1;

	int nHundreds	= nValue / 100;			// 백의 자리
	int nTens		= (nValue / 10) % 10;	// 십의 자리
	int nUnits		= nValue % 10;			// 일의 자리

	return MakeInstructionValue(lpBuffer, COMMAND_C, nChannel, nHundreds, nTens, nUnits);
}

int CExternLightControlVIT::SetLightValue(LPBYTE lpBuffer, int nChannel, int nValue)
{
	if(nValue < 0 || nValue > 255)
		return -1;

	int nHundreds	= nValue / 100;			// 백의 자리
	int nTens		= (nValue / 10) % 10;	// 십의 자리
	int nUnits		= nValue % 10;			// 일의 자리

	return MakeInstructionValue(lpBuffer, COMMAND_D, nChannel, nHundreds, nTens, nUnits);
}

int CExternLightControlVIT::SetSingleLightTurn(LPBYTE lpBuffer, int nChannel, BOOL bOnOff)
{
	int nSize = 0;
	BYTE nCommandChannel = 0;

	if( nChannel > 8 )
	{
		nChannel = nChannel%9;

		if( bOnOff )
			nCommandChannel = 1 << (nChannel);
		else
			nCommandChannel = 0 << (nChannel);

		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_N, COMMAND_F, nCommandChannel, 0);
	}
	else
	{
		if(bOnOff)
			nCommandChannel = 1 << (nChannel-1);
		else
			nCommandChannel = 0 << (nChannel-1);

		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_N, COMMAND_F, 0, nCommandChannel);
	}

	return nSize;
}

int CExternLightControlVIT::SetSingleLightTurnOn(LPBYTE lpBuffer, int nChannel)
{
	int nSize = 0;
	BYTE nCommandChannel = 0;

	CString strMsg;

	if(nChannel > 8)
	{
		nChannel = nChannel%9;
		nCommandChannel = 1 << (nChannel);
		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_N, COMMAND_N, nCommandChannel, 0);

		strMsg = BufferToStatusString(lpBuffer, nCommandChannel, 0);
	}
	else
	{
		nCommandChannel = 1 << (nChannel-1);
		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_N, COMMAND_N, 0, nCommandChannel);

		strMsg = BufferToStatusString(lpBuffer, 0, nCommandChannel);
	}

	if (m_pL2M)
		m_pL2M->SendCommand(strMsg);

	return nSize;
}

int CExternLightControlVIT::SetSingleLightTurnOff(LPBYTE lpBuffer, int nChannel)
{
	int nSize = 0;
	BYTE nCommandChannel = 0;

	CString strMsg;

	if(nChannel > 8)
	{
		nChannel = nChannel%9;
		nCommandChannel = 1 << (nChannel);
		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_F, COMMAND_F, nCommandChannel, 0);

		strMsg = BufferToStatusString(lpBuffer, nCommandChannel, 0);
	}
	else
	{
		nCommandChannel = 1 << (nChannel-1);
		nSize = MakeInstruction(lpBuffer, COMMAND_O, COMMAND_F, COMMAND_F, 0, nCommandChannel);

		strMsg = BufferToStatusString(lpBuffer, 0, nCommandChannel);
	}

	if (m_pL2M)
		m_pL2M->SendCommand(strMsg);

	return nSize;
}


int CExternLightControlVIT::RequestChannelTurnAll(LPBYTE lpBuffer)
{
	int nSize = 0;
	nSize = MakeInstruction(lpBuffer, COMMAND_R, COMMAND_O, COMMAND_O, COMMAND_N, COMMAND_F);	// 전체널
	
	return nSize;
}

int CExternLightControlVIT::RequestChannelData(LPBYTE lpBuffer, int nChannel)
{
	int nSize = 0;
	//nSize = MakeInstruction(lpBuffer, COMMAND_R, nChannel, COMMAND_D, COMMAND_A, COMMAND_T);	// 전체널

	lpBuffer[nSize++] = COMMAND_R;

	lpBuffer[nSize++] = cInt2Hex[nChannel];
	lpBuffer[nSize++] = COMMAND_D;
	lpBuffer[nSize++] = COMMAND_A;
	lpBuffer[nSize++] = COMMAND_T;

	lpBuffer[nSize++] = ASCII_CR;
	lpBuffer[nSize++] = ASCII_LF;

	return nSize;
}

BOOL CExternLightControlVIT::SetGroupOnOff(int nGroup, BOOL bOnOff)
{
	if( nGroup < 1 || nGroup > 4 )
		return FALSE;

	if( !m_serial.IsOpened() )
		return FALSE;

	BYTE OutBuf[10];
	int nSize = 0;

	memset(&OutBuf[0], 0x00, sizeof(OutBuf));
	//sprintf_s((char*)OutBuf, sizeof(OutBuf), "UFAN%d%c%c", (bOnOff==TRUE) ? 1 : 0, ASCII_CR, ASCII_LF);

	OutBuf[nSize++] = COMMAND_G;
	OutBuf[nSize++] = COMMAND_O;
	OutBuf[nSize++] = COMMAND_F;
	OutBuf[nSize++] = cInt2Hex[nGroup];
	OutBuf[nSize++] = (bOnOff==TRUE) ? cInt2Hex[1] : cInt2Hex[0];

	OutBuf[nSize++] = ASCII_CR;
	OutBuf[nSize++] = ASCII_LF;

	nSize = strlen((char*)OutBuf);

	return TRUE;
}

BOOL CExternLightControlVIT::SetGroupLightValue(int nGroup, int nValue)
{
	if(nGroup < 1 || nGroup > 4)
		return FALSE;

	if( nValue < 0 || nValue > 255 )
		return FALSE;

	int nHundreds	= nValue / 100;			// 백의 자리
	int nTens		= (nValue / 10) % 10;	// 십의 자리
	int nUnits		= nValue % 10;			// 일의 자리

	if(!m_serial.IsOpened())
		return FALSE;

	BYTE OutBuf[10];
	int nSize = 0;

	memset(&OutBuf[0], 0x00, sizeof(OutBuf));
	//sprintf_s((char*)OutBuf, sizeof(OutBuf), "UFAN%d%c%c", (bOnOff==TRUE) ? 1 : 0, ASCII_CR, ASCII_LF);

	OutBuf[nSize++] = COMMAND_G;
	OutBuf[nSize++] = cInt2Hex[nGroup];
	OutBuf[nSize++] = cInt2Hex[nHundreds];
	OutBuf[nSize++] = cInt2Hex[nTens];
	OutBuf[nSize++] = cInt2Hex[nUnits];

	OutBuf[nSize++] = ASCII_CR;
	OutBuf[nSize++] = ASCII_LF;

	nSize = strlen((char*)OutBuf);

	return TRUE;
}

int CExternLightControlVIT::MakeInstruction(LPBYTE lpBuffer, int nCommand, int nFirst, int nSecond, int nThird, int nFourth)
{
	int nIdx = 0;
		
	lpBuffer[nIdx++] = (unsigned char)nCommand;
	
	lpBuffer[nIdx++] = (unsigned char)nFirst;
	lpBuffer[nIdx++] = (unsigned char)nSecond;
	lpBuffer[nIdx++] = (unsigned char)nThird;
	lpBuffer[nIdx++] = (unsigned char)nFourth;
	
	lpBuffer[nIdx++] = ASCII_CR;
	lpBuffer[nIdx++] = ASCII_LF;

	return nIdx;
}

int CExternLightControlVIT::MakeInstructionValue(LPBYTE lpBuffer, int nCommand, int nFirst, int nSecond, int nThird, int nFourth)
{
	int nIdx = 0;

	lpBuffer[nIdx++] = (unsigned char)nCommand;

	lpBuffer[nIdx++] = cInt2Hex[nFirst];//(unsigned char)nFirst;
	lpBuffer[nIdx++] = cInt2Hex[nSecond];//(unsigned char)nSecond;
	lpBuffer[nIdx++] = cInt2Hex[nThird];//(unsigned char)nThird;
	lpBuffer[nIdx++] = cInt2Hex[nFourth];//(unsigned char)nFourth;

	lpBuffer[nIdx++] = ASCII_CR;
	lpBuffer[nIdx++] = ASCII_LF;

	return nIdx;
}

CString CExternLightControlVIT::ByteToBitString(BYTE data)
{
	CString str;
	for (int i = 7; i >= 0; --i)
	{
		str += (data & (1 << i)) ? _T('1') : _T('0');
	}
	return str;
}

CString CExternLightControlVIT::BufferToStatusString(LPBYTE lpBuffer, BYTE dataHigh, BYTE dataLow)
{
	CString strMsg((LPCSTR)lpBuffer, 3);
	strMsg += _T(" ");
	strMsg += ByteToBitString(dataHigh);
	strMsg += _T(" ");
	strMsg += ByteToBitString(dataLow);

	return strMsg;
}

UINT CExternLightControlVIT::ThreadLampStatus(LPVOID lpParam)
{
	CExternLightControlVIT* pCtrl = static_cast<CExternLightControlVIT*>(lpParam);
	if (!pCtrl)
		return 0;
	
	HANDLE handle[2] = { pCtrl->m_evtExit.m_hObject , pCtrl->m_evtStatus.m_hObject };

	while (TRUE)
	{
		DWORD dwResult = WaitForMultipleObjects(2, handle, FALSE, INFINITE);

		if (dwResult == WAIT_OBJECT_0)
			break;
		else if (dwResult == WAIT_OBJECT_0 + 1)
		{
			pCtrl->m_evtStatus.ResetEvent();
			pCtrl->CheckLampStatus();
		}
	}

	AfxEndThread(0);

	return 0;
}

BOOL CExternLightControlVIT::CheckLampStatus()
{
	BYTE OutBuf[10] = { 0 };
	
	TRACE(_T("Send request command.\n"));
	CSingleLock lock(&m_sectionCommandSend, TRUE);

	for (int i = 1; i <= 7; i++)
	{
		RequestChannelData(OutBuf, i);
		if (!m_serial.Send(OutBuf, BUFFER_SIZE))
			return FALSE;
	}

	RequestChannelTurnAll(OutBuf);
	if (!m_serial.Send(OutBuf, BUFFER_SIZE))
		return FALSE;

	return TRUE;
}