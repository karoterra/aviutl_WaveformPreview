#include "pch.h"
#include "WaveformPreview.h"
#include "misc.h"

using namespace std;

WaveformPreview wp;

static FILTER_DLL filter = {
    FILTER_FLAG_ALWAYS_ACTIVE | FILTER_FLAG_PRIORITY_LOWEST |
    FILTER_FLAG_WINDOW_SIZE | FILTER_FLAG_WINDOW_THICKFRAME | FILTER_FLAG_WINDOW_HSCROLL |
    FILTER_FLAG_EX_INFORMATION,
    400, 300,
    _T("波形プレビュー"),
    0, nullptr, nullptr, nullptr, nullptr,
    0, nullptr, nullptr,
    func_proc,
    func_init,
    func_exit,
    nullptr,
    func_WndProc,
    nullptr, nullptr,
    nullptr, 0,
    _T("波形プレビュー v0.2.0 by karoterra"),
    nullptr,
    nullptr,
};

EXTERN_C __declspec(dllexport) FILTER_DLL *__stdcall GetFilterTable()
{
    return &filter;
}

BOOL func_init(FILTER *fp)
{
    if (!AfxWinInit(fp->dll_hinst, nullptr, ::GetCommandLine(), 0)) {
        ShowError(NULL, _T("波形プレビューの初期化に失敗しました。\n(AfxWinInit)"));
        return FALSE;
    }

    return wp.Init(fp);
}

BOOL func_exit(FILTER *fp)
{
    wp.Exit(fp);
    return TRUE;
}

BOOL func_proc(FILTER *fp, FILTER_PROC_INFO *fpip)
{
    if (fp->exfunc->is_filter_window_disp(fp) == FALSE
        || fp->exfunc->is_saving(fpip->editp) == TRUE) {
        return FALSE;
    }
    wp.LoadStatus(fp, fpip->editp, fpip);
    return TRUE;
}

BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void *editp, FILTER *fp)
{
    if (!wp.IsInit()) {
        return FALSE;
    }

    BOOL res = FALSE;

    switch (message) {
    case WM_FILTER_FILE_CLOSE:
        res = wp.OnFileClose(fp, editp);
        break;
    case WM_FILTER_CHANGE_WINDOW:
        if (fp->exfunc->is_filter_active(fp) && fp->exfunc->is_filter_window_disp(fp)) {
            wp.LoadStatus(fp, editp);
        }
        break;
    case WM_SIZE:
        wp.OnSize(fp, editp, LOWORD(lparam), HIWORD(lparam));
        break;
    case WM_COMMAND:
        res = wp.OnCommand(fp, editp, wparam, lparam);
        break;
    case WM_HSCROLL:
        res = wp.OnHScroll(fp, editp, LOWORD(wparam), HIWORD(wparam));
        break;
    case WM_MOUSEWHEEL:
        res = wp.OnMouseWheel(fp, editp,
            GET_KEYSTATE_WPARAM(wparam), GET_WHEEL_DELTA_WPARAM(wparam),
            CPoint(LOWORD(lparam), HIWORD(lparam))
        );
        break;
    case WM_LBUTTONDOWN:
        res = wp.OnLButtonDown(fp, editp, wparam, CPoint(LOWORD(lparam), HIWORD(lparam)));
        break;
    case WM_MOUSEMOVE:
        if (wparam & MK_LBUTTON) {
            res = wp.OnLButtonDown(fp, editp, wparam, CPoint(LOWORD(lparam), HIWORD(lparam)));
        }
        break;
    case WM_KEYDOWN:
        res = wp.OnKeyDown(fp, editp, wparam, lparam);
        break;
    case WM_KEYUP:
        res = wp.OnKeyUp(fp, editp, wparam, lparam);
        break;
    }
    return res;
}
