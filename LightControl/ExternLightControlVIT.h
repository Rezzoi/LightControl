#pragma once
#include "LightControlBase.h"


class CExternLightControlVIT : public CLightControlBase, public ISerial2Lamp
{
public:
	CExternLightControlVIT(void);
	virtual ~CExternLightControlVIT(void);
	static CString		GetControllerName()		{ return _T("VIT VLC-1kCM-8CH"); }

	virtual BOOL		OpenControl(CString strPort, DWORD dwBaudrate = 9600);
	virtual CString		GetDeviceName() {return CExternLightControlVIT::GetControllerName();}

	virtual BOOL		SetLightControlValue(int nChannel, int nValue);
	virtual int			GetLightControlValue(int nChannel);
	virtual BOOL		GetLightOnStatus(int nChannel);
	virtual BOOL		IsUsingLightLamp(int nChannel);
	virtual BOOL		SetFanAlarmOnOff(BOOL bOnOff);
	virtual BOOL		SetTemperatureAlarmOnOff(BOOL bOnOff);
	virtual void		LightOnOff(int nChannel, BOOL bOn);
	virtual void		LightOnOff(vector<int> vcChannel, BOOL bOn);
	virtual void		RequestValue(int nChannel);
	virtual void		RequestOnOff();
	virtual void		ProcessPacket(string recieve) override;

protected:
	virtual BOOL		CheckLampStatus();

protected:
	static UINT			ThreadLampStatus(LPVOID lpParam);
	int					ConvertToHexData(CString strValue);
	BOOL				Initialize();
	int					SetLightValueOn(LPBYTE lpBuffer, int nChannel, int nValue);
	int					SetLightValue(LPBYTE lpBuffer, int nChannel, int nValue);
	int					SetSingleLightTurn(LPBYTE lpBuffer, int nChannel, BOOL bOnOff);
	int					SetSingleLightTurnOn(LPBYTE lpBuffer, int nChannel);
	int					SetSingleLightTurnOff(LPBYTE lpBuffer, int nChannel);
	int					RequestChannelTurnAll(LPBYTE lpBuffer);
	int					RequestChannelData(LPBYTE lpBuffer, int nChannel);
	BOOL				Reset();
	BOOL				SetGroupOnOff(int nGroup, BOOL bOnOff);
	BOOL				SetGroupLightValue(int nGroup, int nValue);
	int					MakeInstructionValue(LPBYTE lpBuffer, int nCommand, int nFirst, int nSecond, int nThird, int nFourth);
	int					MakeInstruction(LPBYTE lpBuffer, int nCommand, int nFirst, int nSecond, int nThird, int nFourth);
	CString				ByteToBitString(BYTE data);
	CString				BufferToStatusString(LPBYTE lpBuffer, BYTE dataHigh, BYTE dataLow);

protected:
	INT					m_nResvLightValue;
	int					m_nStartIdx;
	int					m_nRecvIdx;
	int					m_nTemperature;
	BOOL				m_bAlarmLampOff;
	BOOL				m_bCtrlTurnOn;
	CCriticalSection	m_sectionValue;
	CCriticalSection	m_sectionStatus;
	CCriticalSection	m_sectionCommandSend;
	CEvent				m_evtStatus;
	CEvent				m_evtExit;
};