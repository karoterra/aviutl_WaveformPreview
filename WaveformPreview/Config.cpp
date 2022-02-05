#include "pch.h"
#include "Config.h"
#include "aviutl.h"

using namespace std;
using namespace aviutl;

std::string Config::ColorToString(COLORREF color)
{
    ostringstream oss;
    oss << hex << setw(2) << setfill('0') << (int)GetRValue(color)
        << hex << setw(2) << setfill('0') << (int)GetGValue(color)
        << hex << setw(2) << setfill('0') << (int)GetBValue(color);
    return oss.str();
}

COLORREF Config::StringToColor(const std::string &str, COLORREF def)
{
    if (str.length() < 6) {
        return def;
    }
    int r = stoi(str.substr(0, 2), nullptr, 16);
    int g = stoi(str.substr(2, 2), nullptr, 16);
    int b = stoi(str.substr(4, 2), nullptr, 16);
    return RGB(r, g, b);
}

COLORREF Config::IniLoadColor(LPCSTR key, COLORREF def)
{
    try {
        char buf[1024];
        string defStr = ColorToString(def);
        if (IniLoadStr(key, buf, defStr.c_str()) == FALSE) {
            return def;
        }
        return StringToColor(buf, def);
    }
    catch (...) {
        return def;
    }
}

BOOL Config::IniSaveColor(LPCSTR key, COLORREF color)
{
    return IniSaveStr(key, ColorToString(color).c_str());
}

Config::Config()
    : scaleX(0)
    , scaleY(0)
    , dbMin(-60)
    , enableGrid(0)
    , beat(4)
    , offset(1)
    , autoFocus(1)
    , pivot(0)
    , cacheRange(1)
{
    SetTempo("100");
}

const DecimalEx *Config::GetTempo() const
{
    return &m_tempo;
}

double Config::GetTempoDouble() const
{
    return m_dTempo;
}

void Config::SetTempo(const DecimalEx &tempo)
{
    m_tempo = tempo;
    m_dTempo = m_tempo.ToDouble();
}

void Config::SetTempo(const std::string &tempo)
{
    m_tempo = DecimalEx(tempo);
    m_dTempo = m_tempo.ToDouble();
}

bool Config::LoadUserConfig()
{
    char buf[1024];

    scaleX = IniLoadInt("scaleX", 0);
    scaleY = IniLoadInt("scaleY", 0);
    dbMin = IniLoadInt("dbMin", -60);
    if (dbMin >= 0) {
        dbMin = -60;
    }

    enableGrid = IniLoadInt("enableGrid", 0);
    beat = IniLoadInt("beat", 4);
    if (beat < 1) {
        beat = 4;
    }
    offset = IniLoadInt("offset", 1);
    if (IniLoadStr("tempo", buf, "100") == FALSE) {
        SetTempo("100");
    }
    SetTempo(buf);
    if (m_dTempo <= 0) {
        SetTempo("100");
    }

    autoFocus = IniLoadInt("autoFocus", 1);
    pivot = IniLoadInt("pivot", 0);
    cacheRange = IniLoadInt("cacheRange", 1);

    backgroundColor = IniLoadColor("backgroundColor", RGB(0x00, 0x00, 0x00));
    waveformColor = IniLoadColor("waveformColor", RGB(0x00, 0xff, 0x00));
    scaleColor = IniLoadColor("scaleColor", RGB(0xff, 0xff, 0xff));
    cursorColor = IniLoadColor("cursorColor", RGB(0xff, 0x00, 0x00));
    previewColor = IniLoadColor("previewColor", RGB(0x00, 0x00, 0xff));
    endColor = IniLoadColor("endColor", RGB(0x88, 0x88, 0x88));
    cacheColor = IniLoadColor("cacheColor", RGB(0xff, 0x00, 0x00));
    selectColor = IniLoadColor("selectColor", RGB(0x00, 0x7a, 0xcc));
    nonSelectColor = IniLoadColor("nonSelectColor", RGB(0x30, 0x30, 0x30));
    gridMainColor = IniLoadColor("gridMainColor", RGB(0x80, 0x80, 0x80));
    gridSubColor = IniLoadColor("gridSubColor", RGB(0x50, 0x50, 0x50));

    return true;
}

bool Config::SaveUserConfig() const
{
    IniSaveInt("scaleX", scaleX);
    IniSaveInt("scaleY", scaleY);
    IniSaveInt("dbMin", dbMin);

    IniSaveInt("enableGrid", enableGrid);
    IniSaveInt("beat", beat);
    IniSaveInt("offset", offset);
    if (IniSaveStr("tempo", m_tempo.ToString().c_str()) == FALSE) {
        return false;
    }

    IniSaveInt("autoFocus", autoFocus);
    IniSaveInt("pivot", pivot);
    IniSaveInt("cacheRange", cacheRange);

    IniSaveColor("backgroundColor", backgroundColor);
    IniSaveColor("waveformColor", waveformColor);
    IniSaveColor("scaleColor", scaleColor);
    IniSaveColor("cursorColor", cursorColor);
    IniSaveColor("previewColor", previewColor);
    IniSaveColor("endColor", endColor);
    IniSaveColor("cacheColor", cacheColor);
    IniSaveColor("selectColor", selectColor);
    IniSaveColor("nonSelectColor", nonSelectColor);
    IniSaveColor("gridMainColor", gridMainColor);
    IniSaveColor("gridSubColor", gridSubColor);

    return true;
}
