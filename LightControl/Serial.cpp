#include "pch.h"
#include "Serial.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSerial::CSerial()
	: m_pS2L(nullptr)
{
	m_hComm = INVALID_HANDLE_VALUE;
	m_bOpened = FALSE;

	ZeroMemory(&m_ovRead, sizeof(OVERLAPPED));
	ZeroMemory(&m_ovWrite, sizeof(OVERLAPPED));
	m_token = "\r\n";
}

CSerial::~CSerial()
{
	Close();
}

void CSerial::InitOverlapped()
{
    m_ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

BOOL CSerial::Open(const CString& strPort, DWORD baudRate)
{
    if (m_bOpened)
        return TRUE;

    CString strComm;
    strComm.Format(_T("\\\\.\\%s"), strPort);

    m_hComm = CreateFile(
        strComm,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (m_hComm == INVALID_HANDLE_VALUE)
        return FALSE;

    InitOverlapped();

    SetupComm(m_hComm, 4096, 4096);

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);

    GetCommState(m_hComm, &dcb);

    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    SetCommState(m_hComm, &dcb);

    COMMTIMEOUTS timeouts = { 0 };
    SetCommTimeouts(m_hComm, &timeouts);

    PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);

    AfxBeginThread(RecvThread, this);

    m_bOpened = TRUE;
    return TRUE;
}

void CSerial::Close()
{
    if (!m_bOpened)
        return;

    CloseHandle(m_ovRead.hEvent);
    CloseHandle(m_ovWrite.hEvent);

    CloseHandle(m_hComm);

    m_hComm = INVALID_HANDLE_VALUE;
    m_bOpened = FALSE;
}

BOOL CSerial::Send(const BYTE* pData, DWORD size, DWORD timeout)
{
    if (!m_bOpened)
        return FALSE;

    DWORD dwWritten = 0;

    ResetEvent(m_ovWrite.hEvent);

    BOOL bRet = WriteFile(
        m_hComm,
        pData,
        size,
        &dwWritten,
        &m_ovWrite
    );

    if (!bRet)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            DWORD dwWait = WaitForSingleObject(m_ovWrite.hEvent, timeout);
            if (dwWait == WAIT_OBJECT_0)
            {
                GetOverlappedResult(m_hComm, &m_ovWrite, &dwWritten, FALSE);
                return size == dwWritten;
            }
            return FALSE;
        }
        return FALSE;
    }

    return TRUE;
}

int CSerial::Read(BYTE* pBuffer, DWORD bufferSize, DWORD timeout)
{
    if (!m_bOpened)
        return 0;

    DWORD dwError = 0;
    COMSTAT stat = { 0 };

    ClearCommError(m_hComm, &dwError, &stat);

    if (stat.cbInQue == 0)
        return 0;

    DWORD toRead = min(stat.cbInQue, bufferSize);
    DWORD dwRead = 0;

    ResetEvent(m_ovRead.hEvent);

    BOOL bRet = ReadFile(
        m_hComm,
        pBuffer,
        toRead,
        &dwRead,
        &m_ovRead
    );

    if (!bRet)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
            DWORD dwWait = WaitForSingleObject(m_ovRead.hEvent, timeout);
            if (dwWait == WAIT_OBJECT_0)
            {
                if (GetOverlappedResult(m_hComm, &m_ovRead, &dwRead, FALSE))
                    return (int)dwRead;
            }
            return 0;
        }
        return 0;
    }

    CString str((LPCSTR)pBuffer, dwRead);
    TRACE(_T("Read file: Buffer:%s, Size:%d\n"), str, dwRead);

    return (int)dwRead;
}

UINT CSerial::RecvThread(LPVOID pParam)
{
    CSerial* pSerial = (CSerial*)pParam;
 
    BYTE buf[1024];

    while (pSerial->IsOpened())
    {
        int n = pSerial->Read(buf, sizeof(buf));

        if (n > 0)
            pSerial->ProcessRecv(buf, n);

        Sleep(1);
    }

    return 0;
}

void CSerial::ProcessRecv(const BYTE* data, DWORD size)
{
    m_recvBuffer.append((const char*)data, size);

    size_t pos = m_recvBuffer.find(m_token);

    while (pos != -1)
    {
        string packet = m_recvBuffer.substr(0, pos);
        m_recvBuffer.erase(0, pos + m_token.length());

        if (m_pS2L)
            m_pS2L->ProcessPacket(packet);
    }
}

void CSerial::SetInterface(ISerial2Lamp* pS2L)
{
    m_pS2L = pS2L;
}