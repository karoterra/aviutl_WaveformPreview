#include "pch.h"
#include "EditStatus.h"
#include <omp.h>

using namespace std;

void EditStatus::Clear()
{
    currentFrame = 0;
    totalFrame = 0;
    selectStart = 0;
    selectEnd = 0;
    videoRate = 30.0;
    audioRate = 44100;
    audioCh = 0;
    waveform.clear();
    ClearCache();
}

void EditStatus::Load(FILTER *fp, void *editp)
{
    FILE_INFO fi;
    fp->exfunc->get_file_info(editp, &fi);

    currentFrame = fp->exfunc->get_frame(editp);
    totalFrame = fi.frame_n;
    fp->exfunc->get_select_frame(editp, &selectStart, &selectEnd);

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

void EditStatus::CreateWaveform(FILTER *fp, void *editp, int pos, int width, double ppf)
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

    auto get_audio = fp->exfunc->get_audio_filtered;
    AudioBuf buf;
    for (int frame = pos; frame < lastFrame; frame++) {
        int size = get_audio(editp, frame, nullptr);
        int bufSize = size * audioCh;
        buf.resize(bufSize);
        get_audio(editp, frame, buf.data());
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

void EditStatus::CreateWaveformMT(FILTER *fp, void *editp, int pos, int width, double ppf)
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

    auto get_audio = fp->exfunc->get_audio_filtered;
    AudioBuf buf1, buf2;
    AudioBuf *p1 = &buf1, *p2 = &buf2, *tmp = nullptr;
    int ready = 0;
#   pragma omp parallel num_threads(2)
    {
        switch (omp_get_thread_num()) {
        case 0: {
            for (int frame = pos; frame < lastFrame; frame++) {
                while (ready) {
#                   pragma omp flush(ready)
                }
#               pragma omp flush(p1)

                p1->resize(get_audio(editp, frame, nullptr) * audioCh);
                get_audio(editp, frame, p1->data());

                ready = 1;
#               pragma omp flush(ready)
            }
            break;
        }
        case 1: {
            for (int frame = pos; frame < lastFrame; frame++) {
                while (!ready) {
#                   pragma omp flush(ready)
                }
                tmp = p1;
                p1 = p2;
                p2 = tmp;
#               pragma omp flush(p1)

                ready = 0;
#               pragma omp flush(ready)

                const int size = p2->size();
                int last = (int)((width / ppf - (frame - pos)) * size);
                if (size < last) last = size;
                for (int i = 0; i < last; i++) {
                    int ch = i % audioCh;
                    int x = (int)((frame - pos + (double)i / size) * ppf);
                    int idx = (x * audioCh + ch) * 2;
                    short v = (*p2)[i];
                    waveform[idx] = max(v, waveform[idx]);
                    waveform[idx + 1] = min(v, waveform[idx + 1]);
                }
            }
            break;
        }
        }
    }
}

void EditStatus::ClearCache()
{
    cacheStart = -1;
    cacheEnd = -1;
    audioCache.clear();
    audioIndex.clear();
}

void EditStatus::CreateCache(FILTER *fp, void *editp, int start, int end)
{
    if (audioCh == 0) {
        return;
    }

    start = max(start, 0);
    end = min(end, totalFrame - 1);

    size_t count = 0;
    audioIndex.clear();
    audioIndex.push_back(count);
    for (int i = 0; i < totalFrame; i++) {
        count += fp->exfunc->get_audio_filtered(editp, i, nullptr) * audioCh;
        audioIndex.push_back(count);
        if (audioIndex.back() == 0) {
            throw runtime_error("ëIëîÕàÕÇ™çLÇ∑Ç¨Ç‹Ç∑ÅB");
        }
    }

    audioCache.resize(audioIndex[end + 1] - audioIndex[start]);
    count = 0;
    for (int i = start; i <= end; i++) {
        count += fp->exfunc->get_audio_filtered(editp, i, &audioCache[count]) * audioCh;
    }

    cacheStart = start;
    cacheEnd = end;
}

void EditStatus::CreateWaveformFromCache(FILTER *fp, void *editp, int pos, int width, double ppf)
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

    size_t cacheStartIdx = audioIndex[cacheStart];
    size_t cacheSize = audioCache.size();

#pragma omp parallel for
    for (int x = 0; x < width; x++) {
        size_t first = GetIndex(pos + x / ppf) - cacheStartIdx;
        size_t last = GetIndex(pos + (x + 1) / ppf) - cacheStartIdx;

        for (size_t i = first; i < last; i++) {
            if (i < 0) {
                continue;
            }
            if (i >= cacheSize) {
                break;
            }
            int ch = i % audioCh;
            size_t idx = (x * audioCh + ch) * 2;
            short v = audioCache[i];
            waveform[idx] = max(v, waveform[idx]);
            waveform[idx + 1] = min(v, waveform[idx + 1]);
        }
    }
}

bool EditStatus::IsCached() const
{
    return audioCache.size() > 0;
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

size_t EditStatus::GetIndex(double frame) const
{
    if (frame >= totalFrame) {
        return audioIndex.back();
    }

    size_t iFrame = (size_t)frame;
    size_t index = audioIndex[iFrame];
    size_t next = audioIndex[iFrame + 1];
    index += (size_t)((next - index) / audioCh * (frame - iFrame)) * audioCh;
    return index;
}

bool EditStatus::IsSelectAll() const
{
    return totalFrame > 0 && selectStart == 0 && selectEnd == totalFrame - 1;
}
