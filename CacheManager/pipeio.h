#pragma once

void ReadData(HANDLE handle, LPBYTE data, DWORD size);
void WriteData(HANDLE handle, LPCBYTE data, DWORD size);

int ReadInt(HANDLE handle);
void WriteInt(HANDLE handle, int x);

void ReadAudio(HANDLE handle, short *data, DWORD size);
void WriteAudio(HANDLE handle, const short *data, DWORD size);

std::string ReadString(HANDLE handle);
void WriteString(HANDLE handle, const std::string &s);
