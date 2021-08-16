#pragma once

class WaveformPreview;

class CWaveformCtrl : public CStatic
{
    DECLARE_DYNAMIC(CWaveformCtrl)

public:
    CWaveformCtrl(const WaveformPreview *root);

    BOOL Create(const CRect &rect, CWnd *parent, CFont *font);

    int PointToFrame(CPoint point) const;

protected:
    static const COLORREF BackgroundColor = RGB(0, 0, 0);
    static const COLORREF WaveformColor = RGB(0, 0xff, 0);
    static const COLORREF ScaleColor = RGB(0xff, 0xff, 0xff);
    static const COLORREF CursorColor = RGB(0xff, 0, 0);
    static const COLORREF EndColor = RGB(0x88, 0x88, 0x88);
    static const COLORREF SelectColor = RGB(0, 0x7A, 0xCC);
    static const COLORREF NonSelectColor = RGB(0x30, 0x30, 0x30);

    const WaveformPreview *m_root;
    CFont *m_font;
    CRect m_rect;

    void DrawWaveform(CDC *dc, const CRect &rect);
    void DrawScale(CDC *dc);
    void DrawGrid(CDC *dc);

    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

