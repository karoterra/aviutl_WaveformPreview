#include "pch.h"
#include "pipeio.h"

using namespace std;

void ReadData(HANDLE handle, LPBYTE data, DWORD size)
{
    DWORD bytes = 0;
    while (size > 0) {
        if (!ReadFile(handle, data, size, &bytes, nullptr)) {
            DWORD error = GetLastError();
            throw runtime_error("Failed to ReadData (" + to_string(error) + ")");
        }
        size -= bytes;
        data += bytes;
    }
}

void WriteData(HANDLE handle, LPCBYTE data, DWORD size)
{
    DWORD bytes = 0;
    if (!WriteFile(handle, data, size, &bytes, nullptr)) {
        DWORD error = GetLastError();
        throw runtime_error("Failed to WriteData (" + to_string(error) + ")");
    }
}

int ReadInt(HANDLE handle)
{
    int val = 0;
    ReadData(handle, reinterpret_cast<LPBYTE>(&val), sizeof(int));
    return val;
}

void WriteInt(HANDLE handle, int x)
{
    WriteData(handle, reinterpret_cast<LPCBYTE>(&x), sizeof(int));
}

void ReadAudio(HANDLE handle, short *data, DWORD size)
{
    ReadData(handle, reinterpret_cast<LPBYTE>(data), size * sizeof(short));
}

void WriteAudio(HANDLE handle, const short *data, DWORD size)
{
    WriteData(handle, reinterpret_cast<LPCBYTE>(data), size * sizeof(short));
}

std::string ReadString(HANDLE handle)
{
    int size = ReadInt(handle);
    if (size <= 0) {
        return "";
    }

    vector<char> buf(size);
    ReadData(handle, reinterpret_cast<LPBYTE>(buf.data()), size);

    return string(buf.data());
}

void WriteString(HANDLE handle, const std::string &s)
{
    WriteInt(handle, s.length());
    if (s.length() > 0) {
        WriteData(handle, reinterpret_cast<LPCBYTE>(s.c_str()), s.length());
    }
}
