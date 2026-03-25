#include "pch.h"
#include "Serial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSerial::CSerial()
	:m_pThread(nullptr)
	, m_running(false)
	, m_pS2L(nullptr)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_token = "\r\n";
}

CSerial::~CSerial()
{
	Close();
}

BOOL CSerial::Open(CString strPort, int nBaudrate)
{
	m_hComm = CreateFile(strPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (m_hComm == INVALID_HANDLE_VALUE)
		return FALSE;

	TRACE(_T("Serial port open success\n"));

	DCB dcb;
	GetCommState(m_hComm, &dcb);

	dcb.BaudRate = nBaudrate;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	SetCommState(m_hComm, &dcb);
	SetCommMask(m_hComm, EV_RXCHAR);

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.ReadTotalTimeoutMultiplier = 100;
	timeouts.WriteTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(m_hComm, &timeouts);

	m_pThread = AfxBeginThread(RecvThread, this);

	m_running = true;

	return TRUE;
}

void CSerial::Close()
{
	if (m_hComm != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hComm);
		m_hComm = INVALID_HANDLE_VALUE;
		m_running = false;
	}
}

BOOL CSerial::Send(const void* pData, int nSize)
{
	DWORD written = 0;

	COMSTAT stat;
	DWORD err;
	ClearCommError(m_hComm, &err, &stat);
	TRACE(_T("[Before] OutQueue: %d\n"), stat.cbOutQue);

	BOOL bResult = WriteFile(m_hComm, pData, nSize, &written, NULL);
	if (!bResult)
	{
		CString str((LPCSTR)pData, nSize);
		TRACE(_T("Write file failed. Data:%s\n"), str);
	}

	return written == nSize;
}

void CSerial::SetToken(const std::string& token)
{
	m_token = token;
}

void CSerial::SetInterface(ISerial2Lamp* pS2L)
{
	m_pS2L = pS2L;
}

UINT CSerial::RecvThread(LPVOID pParam)
{
	CSerial* p = (CSerial*)pParam;

	BYTE buffer[1024] = { 0 };
	DWORD read = 0;
	DWORD event = 0;

	while (p->m_running)
	{
		WaitCommEvent(p->m_hComm, &event, NULL);

		if (event & EV_RXCHAR)
		{
			if (ReadFile(p->m_hComm, buffer, sizeof(buffer), &read, NULL))
			{
				CString str((LPCSTR)buffer, read);
				TRACE(_T("Read file: Buffer:%s, Size:%d\n"), str, read);
				if (read > 0)
					p->ProcessRecv(buffer, read);
			}
		}
	}

	return 0;
}

void CSerial::ProcessRecv(const BYTE* data, DWORD size)
{
	m_recvBuffer.append((const char*)data, size);

	size_t pos;

	while ((pos = m_recvBuffer.find(m_token)) != string::npos)
	{
		string packet = m_recvBuffer.substr(0, pos);
		m_recvBuffer.erase(0, pos + m_token.length());

		if (m_pS2L)
			m_pS2L->ProcessPacket(packet);
	}
}