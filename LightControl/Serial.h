#pragma once
#include <string>
#include <queue>
#include <mutex>
using namespace std;

class ISerial2Lamp
{
public:
    virtual void ProcessPacket(string recieve) PURE;
};

class CSerial
{
public:
    CSerial();
    ~CSerial();

public:
    BOOL Open(const CString& strPort, DWORD baudRate = CBR_9600);
    void Close();

    BOOL Send(const BYTE* pData, DWORD size, DWORD timeout = 3000);
    int  Read(BYTE* pBuffer, DWORD bufferSize, DWORD timeout = 3000);

    BOOL IsOpened() const { return m_bOpened; }
    void SetInterface(ISerial2Lamp* pS2L);

protected:
    static UINT RecvThread(LPVOID pParam);
    void InitOverlapped();
    void ProcessRecv(const BYTE* data, DWORD size);

private:
    HANDLE m_hComm;
    BOOL   m_bOpened;
    OVERLAPPED m_ovRead;
    OVERLAPPED m_ovWrite;
    ISerial2Lamp* m_pS2L;
    string m_token;
    string m_recvBuffer;
};