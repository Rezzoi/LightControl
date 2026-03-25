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

    BOOL Open(CString strPort, int nBaudrate = 9600);
    void Close();
    BOOL Send(const void* pData, int nSize);
    void SetToken(const std::string& token);
    BOOL IsRunning() { return m_running; }
    void SetInterface(ISerial2Lamp* pS2L);

protected:
    static UINT RecvThread(LPVOID pParam);
    void ProcessRecv(const BYTE* data, DWORD size);

private:
    ISerial2Lamp* m_pS2L;
    HANDLE m_hComm;
    CWinThread* m_pThread;
    string m_recvBuffer;
    string m_token;
    mutex m_mutex;
    bool m_running;
};

