#include "pch.h"
#include "CacheProcess.h"
#include "../CacheManager/command.h"
#include "../CacheManager/pipeio.h"

using namespace std;

BOOL SafeCloseHandle(HANDLE &handle) {
    BOOL res = TRUE;
    if (handle != INVALID_HANDLE_VALUE) {
        res = CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
    return res;
}

CacheProcess::CacheProcess()
    : m_hWrite(INVALID_HANDLE_VALUE)
    , m_hRead(INVALID_HANDLE_VALUE)
    , m_hProcess(INVALID_HANDLE_VALUE)
{
}

CacheProcess::~CacheProcess()
{
    Terminate();
}

void CacheProcess::Init(HMODULE hModule)
{
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = nullptr;

    HANDLE hOutR, hOutW;
    HANDLE hInR, hInW;
    if (!CreatePipe(&hOutR, &hOutW, &sa, 0)) {
        throw runtime_error("Failed to CreatePipe (" + to_string(GetLastError()) + ")");
    }
    if (!SetHandleInformation(hOutR, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hOutR);
        CloseHandle(hOutW);
        throw runtime_error("Failed to SetHandleInformation (" + to_string(GetLastError()) + ")");
    }
    if (!CreatePipe(&hInR, &hInW, &sa, 0)) {
        CloseHandle(hOutR);
        CloseHandle(hOutW);
        throw runtime_error("Failed to CreatePipe (" + to_string(GetLastError()) + ")");
    }
    if (!SetHandleInformation(hInW, HANDLE_FLAG_INHERIT, 0)) {
        CloseHandle(hOutR);
        CloseHandle(hOutW);
        CloseHandle(hInR);
        CloseHandle(hInW);
        throw runtime_error("Failed to SetHandleInformation (" + to_string(GetLastError()) + ")");
    }

    TCHAR command[MAX_PATH];
    GetModuleFileName(hModule, command, MAX_PATH);
    size_t length = _tcslen(command);
    command[length - 1] = 'e';
    command[length - 2] = 'x';
    command[length - 3] = 'e';

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hInR;
    si.hStdOutput = hOutW;
    si.hStdError = hOutW;

    if (!CreateProcess(nullptr, command, nullptr, nullptr,
        TRUE, NORMAL_PRIORITY_CLASS | DETACHED_PROCESS,
        nullptr, nullptr,
        &si, &pi))
    {
        CloseHandle(hOutR);
        CloseHandle(hOutW);
        CloseHandle(hInR);
        CloseHandle(hInW);
        throw runtime_error("Failed to CreateProcess (" + to_string(GetLastError()) + ")");
    }

    m_hWrite = hInW;
    m_hRead = hOutR;
    CloseHandle(hOutW);
    CloseHandle(hInR);
    m_hProcess = pi.hProcess;
    CloseHandle(pi.hThread);
}

void CacheProcess::Exit()
{
    if (m_hWrite != INVALID_HANDLE_VALUE) {
        WriteInt(m_hWrite, static_cast<int>(CacheCommand::Exit));
        string s = ReadString(m_hRead);
        SafeCloseHandle(m_hProcess);
    }
    SafeCloseHandle(m_hRead);
    SafeCloseHandle(m_hWrite);
}

void CacheProcess::Terminate()
{
    if (m_hProcess != INVALID_HANDLE_VALUE) {
        TerminateProcess(m_hProcess, 1);
        SafeCloseHandle(m_hProcess);
    }
    SafeCloseHandle(m_hRead);
    SafeCloseHandle(m_hWrite);
}

void CacheProcess::Clear()
{
    if (m_hProcess == INVALID_HANDLE_VALUE) return;

    WriteInt(m_hWrite, static_cast<int>(CacheCommand::Clear));
    string s = ReadString(m_hRead);
}

void CacheProcess::CreateCache(FILTER *fp, void *editp, int start, int end)
{
    if (m_hProcess == INVALID_HANDLE_VALUE) return;

    FILE_INFO fi;
    fp->exfunc->get_file_info(editp, &fi);

    WriteInt(m_hWrite, static_cast<int>(CacheCommand::SetStatus));
    WriteInt(m_hWrite, fi.audio_ch);
    WriteInt(m_hWrite, end - start + 1);
    string s = ReadString(m_hRead);
    if (s == "BAD ALLOC") {
        throw bad_alloc();
    }

    vector<short> buf;
    for (int frame = start; frame <= end; frame++) {
        int size = fp->exfunc->get_audio_filtered(editp, frame, nullptr) * fi.audio_ch;
        buf.resize(size);
        fp->exfunc->get_audio_filtered(editp, frame, buf.data());
        
        WriteInt(m_hWrite, static_cast<int>(CacheCommand::StoreFrame));
        WriteInt(m_hWrite, size);
        WriteAudio(m_hWrite, buf.data(), size);
        s = ReadString(m_hRead);
        if (s == "BAD ALLOC") {
            throw bad_alloc();
        }
    }
}

void CacheProcess::GetSamples(const double range[2], std::vector<short> &data)
{
    if (m_hProcess == INVALID_HANDLE_VALUE) return;

    WriteInt(m_hWrite, static_cast<int>(CacheCommand::GetSamples));
    WriteData(m_hWrite, reinterpret_cast<LPCBYTE>(range), sizeof(double) * 2);
    
    int size = ReadInt(m_hRead);
    data.resize(size);
    if (size == 0) {
        return;
    }
    ReadAudio(m_hRead, data.data(), size);
}
