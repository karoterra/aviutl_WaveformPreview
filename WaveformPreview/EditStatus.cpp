#include "pch.h"
#include "EditStatus.h"
#include <omp.h>
#include "aviutl.h"

using namespace std;
using namespace aviutl;

void EditStatus::Clear()
{
    currentFrame = 0;
    totalFrame = 0;
    previewFrame = 0;
    selectStart = 0;
    selectEnd = 0;
    videoRate = 30.0;
    audioRate = 44100;
    audioCh = 0;
    waveform.clear();
}

void EditStatus::Load(FILTER_PROC_INFO *fpip)
{
    FILE_INFO fi;
    GetFileInfo(&fi);

    currentFrame = GetFrame();
    totalFrame = fi.frame_n;
    previewFrame = (fpip != nullptr) ? fpip->frame : currentFrame;
    GetSelectFrame(&selectStart, &selectEnd);

    if (fi.flag & FILE_INFO_FLAG_VIDEO) {
        videoRate = (double)fi.video_rate / fi.video_scale;
    }
    else {
        videoRate = 30.0;
    }

    if (fi.flag & FILE_INFO_FLAG_AUDIO) {
        audioRate = fi.audio_rate;
        audioCh = fi.audio_ch;
    }
    else {
        audioRate = 44100;
        audioCh = 0;
    }
}

void EditStatus::CreateWaveform(int pos, int width, double ppf)
{
    if (audioCh == 0) {
        return;
    }

    waveform.resize(width * 2 * audioCh);
    for (auto &v : waveform) {
        v = 0;
    }
    double lastFrame = pos + width / ppf;
    if (lastFrame >= totalFrame) {
        width = (int)ceil((totalFrame - pos) * ppf);
        lastFrame = totalFrame;
    }

    AudioBuf buf;
    for (int frame = pos; frame < lastFrame; frame++) {
        int size = GetAudioFiltered(frame, nullptr);
        int bufSize = size * audioCh;
        buf.resize(bufSize);
        GetAudioFiltered(frame, buf.data());
        for (int i = 0; i < bufSize; i++) {
            int ch = i % audioCh;
            int x = (int)((frame - pos + (double)i / bufSize) * ppf);
            if (x >= width) {
                break;
            }
            int idx = (x * audioCh + ch) * 2;
            waveform[idx] = max(buf[i], waveform[idx]);
            waveform[idx + 1] = min(buf[i], waveform[idx + 1]);
        }
    }
}

void EditStatus::CreateWaveformFromCache(CacheProcess &cp, int pos, int width, double ppf)
{
    if (audioCh == 0) {
        return;
    }

    waveform.resize(width * 2 * audioCh);
    for (auto &v : waveform) {
        v = 0;
    }
    double lastFrame = pos + width / ppf;
    if (lastFrame >= totalFrame) {
        width = (int)ceil((totalFrame - pos) * ppf);
    }

#   pragma omp parallel for
    for (int x = 0; x < width; x++) {
        double range[2] = { pos - cacheStart + x / ppf, pos - cacheStart + (x + 1) / ppf };
        vector<short> data;
#       pragma omp critical
        {
            cp.GetSamples(range, data);
        }
        for (size_t i = 0; i < data.size(); i++) {
            int ch = i % audioCh;
            size_t idx = (x * audioCh + ch) * 2;
            waveform[idx] = max(data[i], waveform[idx]);
            waveform[idx + 1] = min(data[i], waveform[idx + 1]);
        }
    }
}

bool EditStatus::IsPreview() const
{
    return currentFrame != previewFrame;
}

bool EditStatus::IsCached() const
{
    return cacheStart != -1;
}

std::string EditStatus::FrameToTime(int frame) const
{
    char buf[32];
    int t = (int)(frame / videoRate * 100);
    int ms = t % 100;
    t /= 100;
    int h = t / 3600;
    int m = t % 3600 / 60;
    int s = t % 60;
    snprintf(buf, 32, "%02d:%02d:%02d.%02d", h, m, s, ms);
    return buf;
}

bool EditStatus::IsSelectAll() const
{
    return totalFrame > 0 && selectStart == 0 && selectEnd == totalFrame - 1;
}
