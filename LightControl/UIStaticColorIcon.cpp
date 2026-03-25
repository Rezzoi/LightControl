// StaticColorIcon.cpp: 구현 파일
//

#include "pch.h"
#include "UIStaticColorIcon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStaticColorIcon

IMPLEMENT_DYNAMIC(CUIStaticColorIcon, CStatic)

CUIStaticColorIcon::CUIStaticColorIcon()
{
	m_colorBack = RGB(15, 15, 15);
	m_brBack.CreateSolidBrush(m_colorBack);
}

CUIStaticColorIcon::CUIStaticColorIcon(COLORREF color)
{
	m_colorBack = color;
}

CUIStaticColorIcon::~CUIStaticColorIcon()
{
}

void CUIStaticColorIcon::SetColor(COLORREF color)
{
	m_colorBack = color;
	Invalidate();
}

BEGIN_MESSAGE_MAP(CUIStaticColorIcon, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CUIStaticColorIcon::OnPaint()
{
	CPaintDC dc(this); 

	if (m_brBack.GetSafeHandle())
	{
		m_brBack.DeleteObject();
	}
	CRect rect;
	GetClientRect(&rect);
	m_brBack.CreateSolidBrush(m_colorBack);
	dc.FillRect(rect, &m_brBack);
}
