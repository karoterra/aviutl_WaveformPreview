#include "pch.h"
#include "WaveformPreview.h"
#include "misc.h"
#include "aviutl.h"

using namespace std;
using namespace aviutl;

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
    _T("波形プレビュー v0.3.0 by karoterra"),
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
    if (IsFilterWindowDisp() == FALSE || IsSaving() != FALSE) {
        return FALSE;
    }
    wp.LoadStatus(fpip);
    return TRUE;
}

BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void *editp, FILTER *fp)
{
    if (!wp.IsInit()) {
        return FALSE;
    }

    BOOL res = FALSE;

    switch (message) {
    case WM_FILTER_INIT:
        StoreAviUtlPtr(editp, fp);
        wp.LoadConfig();
        break;
    case WM_FILTER_FILE_CLOSE:
        res = wp.OnFileClose();
        break;
    case WM_FILTER_CHANGE_WINDOW:
        if (IsFilterActive() != FALSE && IsFilterWindowDisp() != FALSE) {
            wp.LoadStatus();
        }
        break;
    case WM_FILTER_SAVE_END:
        wp.OnSaveEnd();
        break;
    case WM_SIZE:
        wp.OnSize(LOWORD(lparam), HIWORD(lparam));
        break;
    case WM_COMMAND:
        res = wp.OnCommand(wparam, lparam);
        break;
    case WM_HSCROLL:
        res = wp.OnHScroll(LOWORD(wparam), HIWORD(wparam));
        break;
    case WM_MOUSEWHEEL:
        res = wp.OnMouseWheel(
            GET_KEYSTATE_WPARAM(wparam), GET_WHEEL_DELTA_WPARAM(wparam),
            CPoint(LOWORD(lparam), HIWORD(lparam))
        );
        break;
    case WM_LBUTTONDOWN:
        res = wp.OnLButtonDown(wparam, CPoint(LOWORD(lparam), HIWORD(lparam)));
        break;
    case WM_MOUSEMOVE:
        if (wparam & MK_LBUTTON) {
            res = wp.OnLButtonDown(wparam, CPoint(LOWORD(lparam), HIWORD(lparam)));
        }
        break;
    case WM_KEYDOWN:
        res = wp.OnKeyDown(wparam, lparam);
        break;
    case WM_KEYUP:
        res = wp.OnKeyUp(wparam, lparam);
        break;
    }
    return res;
}
