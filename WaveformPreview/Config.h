#pragma once

#include "DecimalEx.h"

enum class ScaleX
{
    Time = 0,
    Frame,
};

enum class ScaleY
{
    Linear = 0,
    Decibel,
};

enum class Pivot
{
    CurrentFrame = 0,
    MouseCursor,
};

enum class CacheRange
{
    All = 0,
    Display,
};

struct Config
{
    int scaleX;
    int scaleY;
    int dbMin;

    int enableGrid;
    int beat;
    int offset;

    int autoFocus;
    int pivot;
    int cacheRange;

    COLORREF backgroundColor;
    COLORREF waveformColor;
    COLORREF scaleColor;
    COLORREF cursorColor;
    COLORREF previewColor;
    COLORREF endColor;
    COLORREF cacheColor;
    COLORREF selectColor;
    COLORREF nonSelectColor;
    COLORREF gridMainColor;
    COLORREF gridSubColor;

    static std::string ColorToString(COLORREF color);
    static COLORREF StringToColor(const std::string &str, COLORREF def = 0);
    static COLORREF IniLoadColor(FILTER *fp, LPSTR key, COLORREF def);

    Config();

    const DecimalEx *GetTempo() const;
    double GetTempoDouble() const;
    void SetTempo(const DecimalEx &tempo);
    void SetTempo(const std::string &tempo);

    bool LoadUserConfig(FILTER *fp);
    bool SaveUserConfig(FILTER *fp) const;

protected:
    DecimalEx m_tempo;
    double m_dTempo;
};
