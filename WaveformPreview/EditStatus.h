#pragma once

#include "CacheProcess.h"

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

    void Clear();
    void Load(FILTER *fp, void *editp);
    void CreateWaveform(FILTER *fp, void *editp, int pos, int width, double ppf);
    void CreateWaveformMT(FILTER *fp, void *editp, int pos, int width, double ppf);
    void CreateWaveformFromCache(CacheProcess &cp, int pos, int width, double ppf);

    bool IsCached() const;

    std::string FrameToTime(int frame) const;

    bool IsSelectAll() const;
};
