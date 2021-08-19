#include "pch.h"
#include "misc.h"

int ShowError(HWND hwnd, LPCTSTR text)
{
    return MessageBox(hwnd, text, _T("�g�`�v���r���["), MB_ICONERROR | MB_OK);
}

int ShowWarning(HWND hwnd, LPCTSTR text)
{
    return MessageBox(hwnd, text, _T("�g�`�v���r���["), MB_ICONWARNING | MB_OK);
}
