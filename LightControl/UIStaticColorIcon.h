#pragma once


// CStaticColorIcon

class CUIStaticColorIcon : public CStatic
{
	DECLARE_DYNAMIC(CUIStaticColorIcon)

public:
	CUIStaticColorIcon();
	CUIStaticColorIcon(COLORREF color);
	virtual ~CUIStaticColorIcon();

public:
	void SetColor(COLORREF color);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	
protected:
	CBrush m_brBack;
	COLORREF m_colorBack;
};


