#include "pch.h"
#include "WaveformPreview.h"
#include "CConfigDialog.h"
#include "misc.h"
#include <imm.h>

using namespace std;

WaveformPreview::WaveformPreview()
    : m_waveform(this)
    , m_waitCursor(NULL)
{
    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
    VERIFY(m_font.CreatePointFont(90, ncm.lfCaptionFont.lfFaceName));
}

WaveformPreview::~WaveformPreview()
{
    m_font.DeleteObject();
}

BOOL WaveformPreview::Init(FILTER *fp)
{
    if (m_wnd.Attach(fp->hwnd) == FALSE) {
        ShowError(NULL, _T("波形プレビューの初期化に失敗しました。\n(m_wnd.Attach)"));
        return FALSE;
    }
    m_wnd.SetWindowText(fp->name);

    m_wnd.GetClientRect(&m_rect);

    m_zoom.Create(CPoint(0, 0), &m_wnd, (UINT)CommandId::Zoom);

    CRect updateRect(m_zoom.Width, 0, m_zoom.Width + 95, 25);
    DWORD style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
    if (m_cacheBtn.Create("キャッシュ作成", style, updateRect, &m_wnd, (UINT)CommandId::Cache) == FALSE) {
        ShowError(NULL, _T("波形プレビューの初期化に失敗しました。\n(m_cacheBtn.Create)"));
        m_wnd.Detach();
        return FALSE;
    }
    m_cacheBtn.SetFont(&m_font);

    CRect configRect(updateRect.right, 0, updateRect.right + 50, 25);
    style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;
    if (m_configBtn.Create("設定", style, configRect, &m_wnd, (UINT)CommandId::Config) == FALSE) {
        ShowError(NULL, _T("波形プレビューの初期化に失敗しました。\n(m_configBtn.Create)"));
        m_wnd.Detach();
        return FALSE;
    }
    m_configBtn.SetFont(&m_font);

    m_waveformRect = CRect(0, m_zoom.Height, m_rect.right, m_rect.bottom);
    if (m_waveform.Create(m_waveformRect, &m_wnd, &m_font) == FALSE) {
        ShowError(NULL, _T("波形プレビューの初期化に失敗しました。\n(m_waveform.Create)"));
        m_wnd.Detach();
        return FALSE;
    }

    m_waitCursor = LoadCursor(NULL, IDC_WAIT);

    try {
        m_cacheProcess.Init(fp->dll_hinst);
    }
    catch (runtime_error &e) {
        m_cacheBtn.EnableWindow(FALSE);
        string s = "キャッシュマネージャーの初期化に失敗しました。\n\n" + string(e.what());
        ShowError(NULL, s.c_str());
    }

    m_editStatus.Clear();
    this->SetScrollPos(0);

    m_config.LoadUserConfig(fp);

    SYS_INFO sysInfo;
    fp->exfunc->get_sys_info(nullptr, &sysInfo);
    for (int i = 0; i < sysInfo.filter_n; i++) {
        FILTER *fp1 = (FILTER *)fp->exfunc->get_filterp(i);
        if (_tcscmp(fp1->name, _T("再生ウィンドウ")) == 0) {
            m_playbackWnd.Attach(fp1->hwnd);
            break;
        }
    }

    ImmAssociateContext(fp->hwnd, NULL);

    m_init = true;

    return TRUE;
}

void WaveformPreview::Exit(FILTER *fp)
{
    m_wnd.Detach();
    m_playbackWnd.Detach();
    m_config.SaveUserConfig(fp);
    try {
        m_cacheProcess.Exit();
    }
    catch (runtime_error &e) {
        string msg = "キャッシュマネージャーとの通信に失敗しました。\n\n" + string(e.what());
        ShowError(m_wnd.m_hWnd, msg.c_str());
    }
}

void WaveformPreview::ClearStatus()
{
    m_editStatus.Clear();
    ClearCache();
    m_prevFrame = 0;
}

void WaveformPreview::LoadStatus(FILTER *fp, void *editp, FILTER_PROC_INFO *fpip)
{
    m_editStatus.Load(fp, editp, fpip);

    int pos = GetScrollPos();
    int last = pos + (int)(m_waveformRect.Width() / GetPpf());
    if (m_config.autoFocus) {
        if (m_editStatus.IsPreview() && (m_editStatus.previewFrame < pos || last < m_editStatus.previewFrame)) {
            pos = m_editStatus.previewFrame;
        }
        else if (!m_editStatus.IsPreview() && (m_editStatus.currentFrame < pos || last < m_editStatus.currentFrame)) {
            pos = (int)(m_editStatus.currentFrame - m_waveformRect.Width() / 2 / GetPpf());
        }
    }

    Update(fp, editp, pos, false);
}

void WaveformPreview::CreateWaveform(FILTER *fp, void *editp)
{
    HCURSOR cursor = SetCursor(m_waitCursor);

    if (m_editStatus.IsCached()) {
        try {
            m_editStatus.CreateWaveformFromCache(m_cacheProcess,
                GetScrollPos(), m_waveformRect.Width(), GetPpf());
        }
        catch (runtime_error &e) {
            m_cacheProcess.Terminate();
            m_cacheBtn.EnableWindow(FALSE);
            string msg = "キャッシュマネージャーとの通信に失敗しました。\n\n" + string(e.what());
            ShowError(m_wnd.m_hWnd, msg.c_str());
        }
    }
    else {
        m_editStatus.CreateWaveform(fp, editp,
            GetScrollPos(), m_waveformRect.Width(), GetPpf());
    }

    SetCursor(cursor);
}

void WaveformPreview::ClearCache()
{
    try {
        m_editStatus.cacheStart = -1;
        m_editStatus.cacheEnd = -1;
        m_cacheBtn.SetWindowTextA(_T("キャッシュ作成"));
        m_cacheProcess.Clear();
    }
    catch (runtime_error &e) {
        m_cacheProcess.Terminate();
        m_cacheBtn.EnableWindow(FALSE);
        string msg = "キャッシュマネージャーとの通信に失敗しました。\n\n" + string(e.what());
        ShowError(m_wnd.m_hWnd, msg.c_str());
    }
}

void WaveformPreview::CreateCache(FILTER *fp, void *editp)
{
    HCURSOR cursor = SetCursor(m_waitCursor);

    int start = m_editStatus.selectStart;
    int end = m_editStatus.selectEnd;
    if (m_editStatus.IsSelectAll() && m_config.cacheRange == (int)CacheRange::Display) {
        start = GetScrollPos();
        end = start + (int)(m_waveformRect.Width() / GetPpf());
        end = min(end, m_editStatus.totalFrame - 1);
    }
    try {
        m_cacheProcess.CreateCache(fp, editp, start, end);
        m_editStatus.cacheStart = start;
        m_editStatus.cacheEnd = end;
        m_cacheBtn.SetWindowTextA(_T("キャッシュ削除"));
    }
    catch (bad_alloc &e) {
        ShowWarning(NULL, _T("キャッシュの作成に失敗しました。\n選択範囲を狭くして再度試してください。"));
        ClearCache();
    }
    catch (runtime_error &e) {
        m_cacheProcess.Terminate();
        m_cacheBtn.EnableWindow(FALSE);
        string msg = "キャッシュマネージャーとの通信に失敗しました。\n\n" + string(e.what());
        ShowError(m_wnd.m_hWnd, msg.c_str());
    }

    SetCursor(cursor);
}

void WaveformPreview::Display()
{
    m_waveform.Invalidate();
    m_waveform.UpdateWindow();
    return;
}

void WaveformPreview::Update(FILTER *fp, void *editp, int pos, bool recreate)
{
    int prev = GetScrollPos();
    SetScrollPos(pos);

    if (recreate
        || prev != pos
        || (!m_editStatus.IsCached() && !m_editStatus.IsPreview()))
    {
        CreateWaveform(fp, editp);
    }

    Display();
}

void WaveformPreview::ZoomIn()
{
    m_zoom.SetZoom(m_zoom.GetZoom() + 1);
}

void WaveformPreview::ZoomOut()
{
    m_zoom.SetZoom(m_zoom.GetZoom() - 1);
}

bool WaveformPreview::IsInit() const
{
    return m_init;
}

const EditStatus *WaveformPreview::GetEditStatus() const
{
    return &m_editStatus;
}

const Config *WaveformPreview::GetConfig() const
{
    return &m_config;
}

double WaveformPreview::GetPpf() const
{
    return PxPerFrames[m_zoom.GetZoom()];
}

int WaveformPreview::GetScrollPos() const
{
    return m_wnd.GetScrollPos(SB_HORZ);
}

void WaveformPreview::SetScrollPos(int pos)
{
    SCROLLINFO si{
        sizeof(SCROLLINFO),
        SIF_DISABLENOSCROLL | SIF_POS | SIF_RANGE | SIF_PAGE,
        0, (int)(m_editStatus.totalFrame + 100 / GetPpf()),
        (UINT)(m_rect.Width() / GetPpf()),
        pos
    };
    m_wnd.SetScrollInfo(SB_HORZ, &si);
}

BOOL WaveformPreview::MoveFrame(FILTER *fp, void *editp, int frame, bool selectMode)
{
    if (!fp->exfunc->is_editing(editp)) {
        return FALSE;
    }

    int newFrame = fp->exfunc->set_frame(editp, frame);
    BOOL res = (m_editStatus.currentFrame != newFrame) ? TRUE : FALSE;
    if (!selectMode) {
        m_prevFrame = newFrame;
    }
    else if (m_prevFrame > newFrame) {
        fp->exfunc->set_select_frame(editp, newFrame, m_prevFrame);
    }
    else {
        fp->exfunc->set_select_frame(editp, m_prevFrame, newFrame);
    }
    return res;
}

void WaveformPreview::Playback()
{
    if (m_playbackWnd.m_hWnd != nullptr) {
        m_playbackWnd.PostMessage(WM_COMMAND, 0x385, (LPARAM)&m_editStatus.currentFrame);
    }
}

void WaveformPreview::ShowConfigDialog()
{
    vector<HWND> hwnds;
    auto thread = GetCurrentThreadId();
    EnumThreadWindows(thread, &EnumEnabledWindowProc, (LPARAM)&hwnds);

    CConfigDialog dlg(&m_wnd);
    dlg.LoadConfig(m_config);
    if (dlg.DoModal() == IDOK) {
        dlg.SaveConfig(m_config);
        Display();
    }

    for (auto& hwnd : hwnds) {
        EnableWindow(hwnd, TRUE);
    }

    m_wnd.SetActiveWindow();
}

BOOL WaveformPreview::OnFileClose(FILTER *fp, void *editp)
{
    ClearStatus();
    Update(fp, editp, 0, true);
    return FALSE;
}

BOOL WaveformPreview::OnSaveEnd(FILTER* fp, void* edit)
{
    m_zoom.Invalidate();
    Display();
    return FALSE;
}

void WaveformPreview::OnSize(FILTER *fp, void *editp, int width, int height)
{
    m_rect.right = width;
    m_rect.bottom = height;
    m_waveformRect.right = width;
    m_waveformRect.bottom = height;
    m_waveform.MoveWindow(
        m_waveformRect.left, m_waveformRect.top,
        m_waveformRect.Width(), m_waveformRect.Height()
    );
    Update(fp, editp, GetScrollPos(), true);
}

BOOL WaveformPreview::OnCommand(FILTER *fp, void *editp, WPARAM wParam, LPARAM lParam)
{
    switch (LOWORD(wParam)) {
    case (UINT)CommandId::Cache:
        m_wnd.SetFocus();
        if (!fp->exfunc->is_editing(editp)) break;
        if (m_editStatus.IsCached()) {
            ClearCache();
        }
        else {
            CreateCache(fp, editp);
        }
        Update(fp, editp, GetScrollPos(), true);
        break;
    case (UINT)CommandId::Config:
        ShowConfigDialog();
        break;
    case (UINT)CommandId::Zoom:
        if (HIWORD(wParam) == (UINT)ZoomNotify::Changed) {
            double prevPpf = PxPerFrames[lParam];
            int current = m_editStatus.currentFrame;
            if (m_config.pivot == (int)Pivot::MouseCursor) {
                CPoint point;
                GetCursorPos(&point);
                m_wnd.ScreenToClient(&point);
                if (m_waveformRect.PtInRect(point) == FALSE) {
                    point.x = m_waveformRect.Width() / 2;
                }
                current = (int)(GetScrollPos() + point.x / prevPpf);
            }
            double cursorX = (current - GetScrollPos()) * prevPpf;
            int pos = (int)(current - cursorX / GetPpf());
            Update(fp, editp, pos, true);
        }
        break;
    }
    return FALSE;
}

BOOL WaveformPreview::OnKeyDown(FILTER *fp, void *editp, UINT key, LPARAM lParam)
{
    BOOL res = FALSE;
    switch (key) {
    case VK_SHIFT:
        m_isShiftDown = true;
        break;
    case VK_CONTROL:
        m_isCtrlDown = true;
        break;
    }
    m_wnd.GetWindow(GW_OWNER)->PostMessage(WM_KEYDOWN, key, lParam);
    return res;
}

BOOL WaveformPreview::OnKeyUp(FILTER *fp, void *editp, UINT key, LPARAM lParam)
{
    switch (key) {
    case VK_SHIFT:
        m_isShiftDown = false;
        break;
    case VK_CONTROL:
        m_isCtrlDown = false;
        break;
    }
    m_wnd.GetWindow(GW_OWNER)->PostMessage(WM_KEYUP, key, lParam);
    return FALSE;
}

BOOL WaveformPreview::OnLButtonDown(FILTER *fp, void *editp, UINT flags, CPoint point)
{
    if (!fp->exfunc->is_editing(editp) || fp->exfunc->is_saving(editp)) {
        return FALSE;
    }

    point.y -= m_zoom.Height;
    int newFrame = m_waveform.PointToFrame(point);
    if (newFrame < 0) {
        return FALSE;
    }
    return MoveFrame(fp, editp, newFrame, (flags & MK_SHIFT) != 0);
}

BOOL WaveformPreview::OnMouseWheel(FILTER *fp, void *editp, UINT flags, short delta, CPoint point)
{
    if (fp->exfunc->is_saving(editp)) {
        return FALSE;
    }

    if (flags & MK_CONTROL) {
        if (delta > 0) {
            ZoomIn();
        }
        else {
            ZoomOut();
        }
    }
    else {
        int pos = GetScrollPos();
        pos -= (int)(100.0 * delta / WHEEL_DELTA / GetPpf());
        Update(fp, editp, pos, false);
    }
    return FALSE;
}

BOOL WaveformPreview::OnHScroll(FILTER *fp, void *editp, UINT sbCode, UINT pos)
{
    if (fp->exfunc->is_saving(editp)) {
        return FALSE;
    }

    int _pos = GetScrollPos();
    switch (sbCode) {
    case SB_LEFT:
        _pos = 0;
        break;
    case SB_RIGHT:
        _pos = m_editStatus.totalFrame - 1;
        break;
    case SB_LINELEFT:
        _pos = (int)(GetScrollPos() - 10 / GetPpf());
        break;
    case SB_LINERIGHT:
        _pos = (int)(GetScrollPos() + 10 / GetPpf());
        break;
    case SB_PAGELEFT:
        break;
    case SB_PAGERIGHT:
        break;
    case SB_THUMBTRACK: {
        SCROLLINFO si{ sizeof(SCROLLINFO), SIF_TRACKPOS };
        m_wnd.GetScrollInfo(SB_HORZ, &si);
        _pos = si.nTrackPos;
        break;
    }
    }
    Update(fp, editp, _pos, false);
    return FALSE;
}

BOOL WaveformPreview::EnumEnabledWindowProc(HWND hwnd, LPARAM lParam)
{
    auto hwnds = reinterpret_cast<vector<HWND>*>(lParam);
    if (IsWindowVisible(hwnd) && IsWindowEnabled(hwnd)) {
        hwnds->push_back(hwnd);
        EnableWindow(hwnd, FALSE);
    }
    return TRUE;
}
