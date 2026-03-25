#include "pch.h"
#include "LightControlBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLightControlBase::CLightControlBase()
	:m_pL2M(nullptr)
	, m_pParent(nullptr)
{
	memset(m_arrLightValue, 0, sizeof(m_arrLightValue));
	memset(m_arrLampOn, 0, sizeof(m_arrLampOn));
}

CLightControlBase::~CLightControlBase()
{
}

BOOL CLightControlBase::OpenControl(CString strPort, DWORD dwBaudrate)
{
	if (m_serial.IsRunning())
		m_serial.Close();

	return m_serial.Open(strPort);
}

void CLightControlBase::SetInterface(ILamp2Main* pL2M)
{
	m_pL2M = pL2M;
}

void CLightControlBase::SetParentWnd(CWnd* pParent)
{
	m_pParent = pParent;
}

BOOL CLightControlBase::IsConnect()
{
	return m_serial.IsRunning();
}
