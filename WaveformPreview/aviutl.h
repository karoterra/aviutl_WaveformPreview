#pragma once

namespace aviutl {

    void StoreAviUtlPtr(void* editp, FILTER* fp);

    BOOL IsFilterActive();
    BOOL IsFilterWindowDisp();

    BOOL IsEditing();
    BOOL IsSaving();

    BOOL GetFileInfo(FILE_INFO* fip);

    int GetAudioFiltered(int n, void* buf);

    int GetFrame();
    int SetFrame(int n);

    BOOL GetSelectFrame(int* s, int* e);
    BOOL SetSelectFrame(int s, int e);

    int IniLoadInt(LPCSTR key, int n);
    int IniSaveInt(LPCSTR key, int n);

    BOOL IniLoadStr(LPCSTR key, LPSTR str, LPCSTR def);
    BOOL IniSaveStr(LPCSTR key, LPCSTR str);

};
