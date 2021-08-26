#pragma once

#include "EditStatus.h"
#include "CZoomIndicator.h"
#include "CWaveformCtrl.h"
#include "Config.h"
#include "CacheProcess.h"

class WaveformPreview
{
public:
    enum class CommandId : UINT
    {
        Cache = 0x8000,
        Config,
        Zoom,
    };

    WaveformPreview();
    ~WaveformPreview();

    BOOL Init(FILTER *fp);
    void Exit(FILTER *fp);

    void ClearStatus();
    void LoadStatus(FILTER *fp, void *editp);

    void CreateWaveform(FILTER *fp, void *editp);

    void ClearCache();
    void CreateCache(FILTER *fp, void *editp);

    void Display();
    void Update(FILTER *fp, void *editp, int pos, bool recreate);

    void ZoomIn();
    void ZoomOut();

    bool IsInit() const;

    const EditStatus *GetEditStatus() const;
    const Config *GetConfig() const;

    double GetPpf() const;

    int GetScrollPos() const;
    void SetScrollPos(int pos);

    BOOL MoveFrame(FILTER *fp, void *editp, int frame, bool selectMode);

    void Playback();

    void ShowConfigDialog();

    BOOL OnFileClose(FILTER *fp, void *editp);

    void OnSize(FILTER *fp, void *editp, int width, int height);
    BOOL OnCommand(FILTER *fp, void *editp, WPARAM wParam, LPARAM lParam);
    BOOL OnKeyDown(FILTER *fp, void *editp, UINT key);
    BOOL OnKeyUp(FILTER *fp, void *editp, UINT key);
    BOOL OnLButtonDown(FILTER *fp, void *editp, UINT flags, CPoint point);
    BOOL OnMouseWheel(FILTER *fp, void *editp, UINT flags, short delta, CPoint point);
    BOOL OnHScroll(FILTER *fp, void *editp, UINT sbCode, UINT pos);

protected:
    static constexpr double PxPerFrames[] = {
        0.002, 0.003, 0.004, 0.005, 0.0075,
        0.01, 0.015, 0.02, 0.03, 0.04, 0.05, 0.075,
        0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.75,
        1, 1.5, 2, 3, 4, 5, 7.5, 10
    };

    bool m_init = false;

    CWnd m_wnd;
    CZoomIndicator m_zoom;
    CButton m_cacheBtn;
    CButton m_configBtn;
    CWaveformCtrl m_waveform;

    CRect m_rect;
    CRect m_waveformRect;

    CWnd m_playbackWnd;

    CFont m_font;
    HCURSOR m_waitCursor;

    CacheProcess m_cacheProcess;
    EditStatus m_editStatus;

    Config m_config;

    bool m_isShiftDown = false;
    bool m_isCtrlDown = false;
    int m_prevFrame = 0;

    static BOOL CALLBACK EnumEnabledWindowProc(HWND hwnd, LPARAM lParam);
};
