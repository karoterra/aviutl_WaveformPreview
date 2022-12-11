#include "pch.h"
#include "aviutl.h"

static FILTER* g_fp = nullptr;
static const EXFUNC* g_exfunc = nullptr;
static void* g_editp = nullptr;

void aviutl::StoreAviUtlPtr(void* editp, FILTER* fp) {
    g_fp = fp;
    g_exfunc = fp->exfunc;
    g_editp = editp;
}

BOOL aviutl::IsFilterActive() {
    return g_exfunc->is_filter_active(g_fp);
}

BOOL aviutl::IsFilterWindowDisp() {
    return g_exfunc->is_filter_window_disp(g_fp);
}

BOOL aviutl::IsEditing() {
    return g_exfunc->is_editing(g_editp);
}

BOOL aviutl::IsSaving() {
    return g_exfunc->is_saving(g_editp);
}

BOOL aviutl::GetFileInfo(FILE_INFO* fip) {
    return g_exfunc->get_file_info(g_editp, fip);
}

int aviutl::GetAudioFiltered(int n, void* buf) {
    return g_exfunc->get_audio_filtered(g_editp, n, buf);
}

int aviutl::GetFrame() {
    return g_exfunc->get_frame(g_editp);
}

int aviutl::SetFrame(int n) {
    return g_exfunc->set_frame(g_editp, n);
}

BOOL aviutl::GetSelectFrame(int* s, int* e) {
    return g_exfunc->get_select_frame(g_editp, s, e);
}

BOOL aviutl::SetSelectFrame(int s, int e) {
    return g_exfunc->set_select_frame(g_editp, s, e);
}

int aviutl::IniLoadInt(LPCSTR key, int n) {
    return g_exfunc->ini_load_int(g_fp, const_cast<LPSTR>(key), n);
}

int aviutl::IniSaveInt(LPCSTR key, int n) {
    return g_exfunc->ini_save_int(g_fp, const_cast<LPSTR>(key), n);
}

BOOL aviutl::IniLoadStr(LPCSTR key, LPSTR str, LPCSTR def) {
    return g_exfunc->ini_load_str(g_fp, const_cast<LPSTR>(key), str, const_cast<LPSTR>(def));
}

BOOL aviutl::IniSaveStr(LPCSTR key, LPCSTR str) {
    return g_exfunc->ini_save_str(g_fp, const_cast<LPSTR>(key), const_cast<LPSTR>(str));
}
