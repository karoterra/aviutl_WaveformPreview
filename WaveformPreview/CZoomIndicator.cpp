#include "pch.h"
#include "CZoomIndicator.h"

IMPLEMENT_DYNAMIC(CZoomIndicator, CStatic)

BEGIN_MESSAGE_MAP(CZoomIndicator, CStatic)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CZoomIndicator::Create(POINT topLeft, CWnd *parent, UINT id)
{
    DWORD style = WS_CHILD | WS_VISIBLE | SS_NOTIFY;
    CRect rect(topLeft, CPoint(Width, Height) + topLeft);
    if (CStatic::Create(nullptr, style, rect, parent, id) == FALSE) {
        return FALSE;
    }

    m_zoom = MaxZoom;

    return TRUE;
}

int CZoomIndicator::GetZoom() const
{
    return m_zoom;
}

bool CZoomIndicator::SetZoom(int zoom)
{
    zoom = std::clamp(zoom, MinZoom, MaxZoom);
    bool changed = zoom != m_zoom;
    if (changed) {
        WPARAM wp = MAKEWPARAM(GetDlgCtrlID(), (UINT)ZoomNotify::Changed);
        LPARAM lp = m_zoom;
        GetParent()->PostMessage(WM_COMMAND, wp, lp);
        m_zoom = zoom;
        Invalidate();
        UpdateWindow();
    }
    return changed;
}

void CZoomIndicator::OnPaint()
{
    CRect rc;
    if (GetUpdateRect(&rc, TRUE) == FALSE) {
        return;
    }

    PAINTSTRUCT ps;
    CDC *dc = BeginPaint(&ps);
    CPen pen1(PS_SOLID, 0, ActiveColor);
    CPen pen2(PS_SOLID, 0, InactiveColor);

    dc->SelectObject(&pen1);
    for (int i = 0; i < MaxZoom; i++) {
        if (i == m_zoom) {
            dc->SelectObject(&pen2);
        }
        dc->MoveTo(MarginH + i * 2, MarginV);
        dc->LineTo(MarginH + i * 2, Height - MarginV);
    }

    EndPaint(&ps);
    pen1.DeleteObject();
    pen2.DeleteObject();
}

void CZoomIndicator::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_dragging = true;
    SetZoom((point.x - MarginH) / 2);
}

void CZoomIndicator::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_dragging = false;
}

void CZoomIndicator::OnMouseMove(UINT nFlags, CPoint point)
{
    if ((nFlags & MK_LBUTTON) == 0) {
        m_dragging = false;
        return;
    }
    if (m_dragging) {
        SetZoom((point.x - MarginH) / 2);
    }
}
