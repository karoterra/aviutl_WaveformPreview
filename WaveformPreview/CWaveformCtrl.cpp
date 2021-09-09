#include "pch.h"
#include <omp.h>
#include "CWaveformCtrl.h"
#include "WaveformPreview.h"

using namespace std;

const double eps = 1e-9;

int decibel(short x, double dbMin, int hh)
{
    double y;
    if (x >= 0) {
        y = 20.0 * log10((double)x / SHRT_MAX + eps);
        y = -hh / (-dbMin) * (y - dbMin);
        return clamp((int)y, -hh, 0);
    }
    else {
        y = 20.0 * log10((double)x / SHRT_MIN + eps);
        y = hh / (-dbMin) * (y - dbMin);
        return clamp((int)y, 0, hh);
    }
}

IMPLEMENT_DYNAMIC(CWaveformCtrl, CStatic)

BEGIN_MESSAGE_MAP(CWaveformCtrl, CStatic)
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()

CWaveformCtrl::CWaveformCtrl(const WaveformPreview *root)
    : m_root(root)
    , m_font(nullptr)
{
}

BOOL CWaveformCtrl::Create(const CRect &rect, CWnd *parent, CFont *font)
{
    DWORD style = WS_CHILD | WS_VISIBLE;
    if (CStatic::Create(nullptr, style, rect, parent) == FALSE) {
        return FALSE;
    }
    m_font = font;
    return TRUE;
}

int CWaveformCtrl::PointToFrame(CPoint point) const
{
    if (m_rect.PtInRect(point) == FALSE) {
        return -1;
    }
    double ppf = m_root->GetPpf();
    int frame = m_root->GetScrollPos() + (int)(point.x / ppf);
    return frame;
}

void CWaveformCtrl::DrawWaveform(CDC *dc, const CRect &rect)
{
    auto status = m_root->GetEditStatus();
    auto config = m_root->GetConfig();
    int height = rect.Height() / status->audioCh;
    int hh = height / 2;
    vector<int> centers;
    for (int i = 0; i < status->audioCh; i++) {
        centers.push_back(i * height + rect.top + height / 2);
    }
    CPen pen(PS_SOLID, 0, config->waveformColor);
    dc->SelectObject(&pen);

    for (int x = 0; x < rect.Width(); x++) {
        for (int ch = 0; ch < status->audioCh; ch++) {
            int idx = (x * status->audioCh + ch) * 2;
            if (config->scaleY == (int)ScaleY::Linear) {
                int y0 = status->waveform[idx] * hh / SHRT_MIN + centers[ch];
                int y1 = status->waveform[idx + 1] * hh / SHRT_MIN + centers[ch] + 1;
                dc->MoveTo(x, y0);
                dc->LineTo(x, y1);
            }
            else {
                int y0 = decibel(status->waveform[idx], config->dbMin, hh) + centers[ch];
                int y1 = decibel(status->waveform[idx + 1], config->dbMin, hh) + centers[ch] + 1;
                dc->MoveTo(x, y0);
                dc->LineTo(x, y1);
            }
        }
    }
}

void CWaveformCtrl::DrawScale(CDC *dc)
{
    auto status = m_root->GetEditStatus();
    auto config = m_root->GetConfig();
    double ppf = m_root->GetPpf();
    int pos = m_root->GetScrollPos();
    CPen scalePen(PS_SOLID, 0, config->scaleColor);

    if (status->IsCached()) {
        dc->FillSolidRect(0, 18, m_rect.Width(), 12, config->nonSelectColor);
        dc->FillSolidRect(
            (int)((status->cacheStart - pos) * ppf),
            18,
            (int)((status->cacheEnd - status->cacheStart + 1) * ppf),
            12,
            config->cacheColor);
    }

    if (status->totalFrame > 0 && !status->IsSelectAll()) {
        int y = status->IsCached() ? 24 : 18;
        int cy = status->IsCached() ? 6 : 12;
        dc->FillSolidRect(0, y, m_rect.Width(), cy, config->nonSelectColor);
        dc->FillSolidRect(
            (int)((status->selectStart - pos) * ppf),
            y,
            (int)((status->selectEnd - status->selectStart + 1) * ppf),
            cy,
            config->selectColor);
    }

    dc->SelectObject(&scalePen);
    dc->SetTextColor(config->scaleColor);
    dc->SetBkColor(config->backgroundColor);
    dc->SelectObject(m_font);
    dc->MoveTo(0, 29);
    dc->LineTo(m_rect.right, 29);
    int frameStep = (int)floor(1 / ppf * 10);
    int scaleX = config->scaleX;
    int frame = pos;
    if (frame % frameStep != 0) {
        frame += frameStep - frame % frameStep;
    }
    while (true) {
        int x = (int)((frame - pos) * ppf);
        if (x > m_rect.right) {
            break;
        }
        dc->MoveTo(x, 29);
        if (frame % (frameStep * 10) == 0) {
            dc->LineTo(x, 17);
            if (scaleX == (int)ScaleX::Time) {
                dc->TextOut(x, 0, status->FrameToTime(frame).c_str());
            }
            else {
                dc->TextOut(x, 0, to_string(frame + 1).c_str());
            }
        }
        else {
            dc->LineTo(x, 23);
        }
        frame += frameStep;
    }
}

void CWaveformCtrl::DrawGrid(CDC *dc)
{
    auto status = m_root->GetEditStatus();
    auto config = m_root->GetConfig();
    double ppf = m_root->GetPpf();
    int pos = m_root->GetScrollPos();
    int offset = config->offset - 1;
    double bpp = config->GetTempoDouble() / 60 / status->videoRate / ppf;
    double ppb = 1 / bpp;
    if (ppb * config->beat < 5) {
        return;
    }
    bool drawLine2 = ppb >= 5;
    CPen pen1(PS_DASH, 0, config->gridMainColor);
    CPen pen2(PS_DOT, 0, config->gridSubColor);
    dc->SelectObject(&pen1);
    dc->SetBkMode(TRANSPARENT);

    double x = (offset - pos) * ppf;
    int beat = ((int)ceil(-x * bpp) % config->beat + config->beat) % config->beat;
    x = fmod(fmod(x, ppb) + ppb, ppb);
    while (x < m_rect.right) {
        if (beat == 0) {
            dc->SelectObject(&pen1);
            dc->MoveTo((int)x, 30);
            dc->LineTo((int)x, m_rect.bottom);
        }
        else if (drawLine2) {
            dc->SelectObject(&pen2);
            dc->MoveTo((int)x, 30);
            dc->LineTo((int)x, m_rect.bottom);
        }
        x += ppb;
        beat = (beat + 1) % config->beat;
    }
}

void CWaveformCtrl::OnPaint()
{
    CRect rc;
    if (GetUpdateRect(&rc, FALSE) == FALSE) {
        return;
    }

    PAINTSTRUCT ps;
    CDC *dc = BeginPaint(&ps);
    CDC memDC;
    CBitmap memBmp;
    memDC.CreateCompatibleDC(dc);
    memBmp.CreateCompatibleBitmap(dc, m_rect.Width(), m_rect.Height());
    memDC.SelectObject(&memBmp);

    auto status = m_root->GetEditStatus();
    auto config = m_root->GetConfig();
    double ppf = m_root->GetPpf();
    int pos = m_root->GetScrollPos();
    CPen cursorPen(PS_SOLID, 0, config->cursorColor ^ config->backgroundColor);
    CPen endPen(PS_SOLID, 0, config->endColor);
    CPen previewPen(PS_SOLID, 0, config->previewColor ^ config->backgroundColor);

    memDC.FillSolidRect(&m_rect, config->backgroundColor);

    int x = (int)((status->totalFrame - pos) * ppf);
    if (0 <= x && x <= m_rect.right) {
        memDC.SelectObject(&endPen);
        memDC.MoveTo(x, 0);
        memDC.LineTo(x, m_rect.bottom);
    }

    if (status->audioCh != 0) {
        DrawWaveform(&memDC, CRect(0, 30, m_rect.right, m_rect.bottom));
    }

    DrawScale(&memDC);
    if (config->enableGrid != 0) {
        DrawGrid(&memDC);
    }

    x = (int)((status->currentFrame - pos) * ppf);
    if (status->totalFrame > 0 && 0 <= x && x <= m_rect.right) {
        memDC.SelectObject(&cursorPen);
        memDC.SetROP2(R2_XORPEN);
        memDC.MoveTo(x, 0);
        memDC.LineTo(x, m_rect.bottom);
    }

    x = (int)((status->previewFrame - pos) * ppf);
    if (status->IsPreview()) {
        memDC.SelectObject(&previewPen);
        memDC.SetROP2(R2_XORPEN);
        memDC.MoveTo(x, 0);
        memDC.LineTo(x, m_rect.bottom);
    }

    dc->BitBlt(0, 0, m_rect.Width(), m_rect.Height(), &memDC, 0, 0, SRCCOPY);
    EndPaint(&ps);
}

void CWaveformCtrl::OnSize(UINT nType, int cx, int cy)
{
    GetClientRect(&m_rect);
}
