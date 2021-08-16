#pragma once

enum class ZoomNotify
{
    Changed = 0x8000,
};

class CZoomIndicator : public CStatic
{
    DECLARE_DYNAMIC(CZoomIndicator)

public:
    static const int MinZoom = 0;
    static const int MaxZoom = 26;

    static const int MarginH = 8;
    static const int MarginV = 4;
    static const int Width = MaxZoom * 2 - 1 + MarginH * 2;
    static const int Height = 25;

    BOOL Create(POINT topLeft, CWnd *parent, UINT id);

    int GetZoom() const;
    bool SetZoom(int zoom);

protected:
    static const COLORREF ActiveColor = RGB(0x60, 0xA0, 0xFF);
    static const COLORREF InactiveColor = RGB(0x20, 0x40, 0x80);

    int m_zoom = MaxZoom;
    bool m_dragging = false;

    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

