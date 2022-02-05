#pragma once
class CacheProcess
{
public:

    CacheProcess();
    virtual ~CacheProcess();

    void Init(HMODULE hModule);
    void Exit();
    void Terminate();

    void Clear();
    void CreateCache(int start, int end);
    void GetSamples(const double range[2], std::vector<short> &data);

protected:
    HANDLE m_hWrite;
    HANDLE m_hRead;
    HANDLE m_hProcess;
};
