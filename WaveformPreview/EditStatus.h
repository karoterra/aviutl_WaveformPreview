#pragma once

using AudioBuf = std::vector<short>;

struct EditStatus
{
    int currentFrame = 0;
    int totalFrame = 0;
    int selectStart = 0;
    int selectEnd = 0;
    double videoRate = 30.0;
    int audioRate = 44100;
    int audioCh = 0;
    int cacheStart = -1;
    int cacheEnd = -1;
    AudioBuf waveform;
    AudioBuf audioCache;
    std::vector<size_t> audioIndex;

    void Clear();
    void Load(FILTER *fp, void *editp);
    void CreateWaveform(FILTER *fp, void *editp, int pos, int width, double ppf);
    void CreateWaveformMT(FILTER *fp, void *editp, int pos, int width, double ppf);
    void ClearCache();
    void CreateCache(FILTER *fp, void *editp, int start, int end);
    void CreateWaveformFromCache(FILTER *fp, void *editp, int pos, int width, double ppf);

    bool IsCached() const;

    std::string FrameToTime(int frame) const;

    size_t GetIndex(double frame) const;

    bool IsSelectAll() const;
};
