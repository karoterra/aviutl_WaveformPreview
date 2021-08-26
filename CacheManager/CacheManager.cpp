#include "pch.h"
#include "CacheManager.h"
#include "command.h"
#include "pipeio.h"

using namespace std;

CacheManager::CacheManager()
    : m_channel(1)
{
    m_hWrite = GetStdHandle(STD_OUTPUT_HANDLE);
    if (m_hWrite == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        throw runtime_error("Failed to GetStdHandle (" + to_string(error) + ")");
    }
    m_hRead = GetStdHandle(STD_INPUT_HANDLE);
    if (m_hRead == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        throw runtime_error("Failed to GetStdHandle (" + to_string(error) + ")");
    }
}

void CacheManager::Clear()
{
    m_frames.clear();
    m_frames.shrink_to_fit();
    WriteString(m_hWrite, "");
}

void CacheManager::SetStatus()
{
    try {
        m_channel = ReadInt(m_hRead);
        int frame = ReadInt(m_hRead);
        m_frames.reserve(frame);
        WriteString(m_hWrite, "");
    }
    catch (bad_alloc &e) {
        WriteString(m_hWrite, "BAD ALLOC");
    }
}

void CacheManager::StoreFrame()
{
    try {
        int size = ReadInt(m_hRead);
        m_frames.emplace_back(size);

        ReadAudio(m_hRead, m_frames.back().data(), size);

        WriteString(m_hWrite, "");
    }
    catch (bad_alloc &e) {
        WriteString(m_hWrite, "BAD ALLOC");
    }
}

void CacheManager::GetSamples()
{
    double range[2];
    ReadData(m_hRead, reinterpret_cast<LPBYTE>(range), sizeof(double) * 2);
    size_t startFrame, startSample;
    size_t endFrame, endSample;
    GetIndex(range[0], startFrame, startSample);
    GetIndex(range[1], endFrame, endSample);

    size_t size = 0;
    if (startFrame == endFrame) {
        size = endSample - startSample;
    }
    else {
        size = m_frames[startFrame].size() - startSample;
        for (size_t frame = startFrame + 1; frame < endFrame; frame++) {
            size += m_frames[frame].size();
        }
        size += endSample;
    }
    if (size == 0) {
        WriteInt(m_hWrite, 0);
        return;
    }

    Frame buf(size);
    if (startFrame == endFrame) {
        memcpy(buf.data(), &m_frames[startFrame][startSample], size * sizeof(short));
    }
    else {
        size_t count = 0;
        memcpy(buf.data(), &m_frames[startFrame][startSample],
            (m_frames[startFrame].size() - startSample) * sizeof(short));
        count += m_frames[startFrame].size() - startSample;
        for (size_t frame = startFrame + 1; frame < endFrame; frame++) {
            memcpy(&buf[count], m_frames[frame].data(), m_frames[frame].size() * sizeof(short));
            count += m_frames[frame].size();
        }
        if (endSample > 0) {
            memcpy(&buf[count], m_frames[endFrame].data(), endSample * sizeof(short));
        }
    }

    WriteInt(m_hWrite, size);
    WriteAudio(m_hWrite, buf.data(), size);
}

void CacheManager::Main()
{
    while (true) {
        CacheCommand cmd = static_cast<CacheCommand>(ReadInt(m_hRead));
        switch (cmd) {
        case CacheCommand::Clear:
            Clear();
            break;
        case CacheCommand::SetStatus:
            SetStatus();
            break;
        case CacheCommand::StoreFrame:
            StoreFrame();
            break;
        case CacheCommand::GetSamples:
            GetSamples();
            break;
        case CacheCommand::Exit:
            WriteString(m_hWrite, "");
            return;
        default:
            WriteString(m_hWrite, "invalid command");
            break;
        }
    }
}

void CacheManager::GetIndex(double frame, size_t &idxFrame, size_t &idxSample)
{
    if (frame < 0) {
        idxFrame = 0;
        idxSample = 0;
        return;
    }
    else if (frame >= m_frames.size()) {
        idxFrame = m_frames.size() - 1;
        idxSample = m_frames.back().size();
        return;
    }
    idxFrame = (size_t)frame;
    double rate = frame - idxFrame;
    idxSample = (size_t)(m_frames[idxFrame].size() * rate);
    idxSample -= idxSample % m_channel;
}
