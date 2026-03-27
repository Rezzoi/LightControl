#pragma once
#include "Serial.h"

#define WM_RECIEVE				(WM_APP + 100)

#define LAMP_MAX_COUNT			16

class ILamp2Main
{
public:
	virtual void SendCommand(CString strCommand) PURE;
};

class CLightControlBase
{
public:
	CLightControlBase();
	virtual ~CLightControlBase();

public:
	virtual BOOL		OpenControl(CString strPort, DWORD dwBaudrate = 9600);
	virtual CString		GetDeviceName() PURE;
	virtual BOOL		SetLightControlValue(int nChannel, int nValue) PURE;
	virtual int			GetLightControlValue(int nChannel) PURE;
	virtual BOOL		GetLightOnStatus(int nChannel) PURE;
	virtual BOOL		IsUsingLightLamp(int nChannel) PURE;
	virtual BOOL		SetFanAlarmOnOff(BOOL bOnOff) PURE;
	virtual BOOL		SetTemperatureAlarmOnOff(BOOL bOnOff) PURE;
	virtual void		RequestValue(int nChannel) PURE;
	virtual void		RequestOnOff() PURE;
	virtual void		LightOnOff(int nChannel, BOOL bOn) PURE;
	virtual void		LightOnOff(vector<int> vcChannel, BOOL bOn) PURE;

public:
	void SetInterface(ILamp2Main* pL2M);
	void SetParentWnd(CWnd* pParent);
	BOOL IsConnect();

protected:
	CSerial m_serial;
	ILamp2Main* m_pL2M;
	CWnd* m_pParent;
	int	m_arrLightValue[LAMP_MAX_COUNT];
	BOOL m_arrLampOn[LAMP_MAX_COUNT];
};

