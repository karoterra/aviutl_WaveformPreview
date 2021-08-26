#pragma once

using Frame = std::vector<short>;

class CacheManager
{
public:
    CacheManager();

    void Clear();
    void SetStatus();
    void StoreFrame();
    void GetSamples();

    void Main();

protected:
    HANDLE m_hWrite;
    HANDLE m_hRead;

    int m_channel;
    std::vector<Frame> m_frames;

    void GetIndex(double frame, size_t &idxFrame, size_t &idxSample);
};
