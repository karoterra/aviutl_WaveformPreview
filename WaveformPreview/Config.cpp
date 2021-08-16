#include "pch.h"
#include "Config.h"

using namespace std;

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

COLORREF Config::IniLoadColor(FILTER *fp, LPSTR key, COLORREF def)
{
    try {
        char buf[1024];
        string defStr = ColorToString(def);
        if (fp->exfunc->ini_load_str(fp, key, buf, const_cast<LPSTR>(defStr.c_str())) == FALSE) {
            return def;
        }
        return StringToColor(buf, def);
    }
    catch (...) {
        return def;
    }
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

bool Config::LoadUserConfig(FILTER *fp)
{
    char buf[1024];

    scaleX = fp->exfunc->ini_load_int(fp, "scaleX", 0);
    scaleY = fp->exfunc->ini_load_int(fp, "scaleY", 0);
    dbMin = fp->exfunc->ini_load_int(fp, "dbMin", -60);
    if (dbMin >= 0) {
        dbMin = -60;
    }

    enableGrid = fp->exfunc->ini_load_int(fp, "enableGrid", 0);
    beat = fp->exfunc->ini_load_int(fp, "beat", 4);
    if (beat < 1) {
        beat = 4;
    }
    offset = fp->exfunc->ini_load_int(fp, "offset", 1);
    if (fp->exfunc->ini_load_str(fp, "tempo", buf, "100") == FALSE) {
        SetTempo("100");
    }
    SetTempo(buf);
    if (m_dTempo <= 0) {
        SetTempo("100");
    }

    autoFocus = fp->exfunc->ini_load_int(fp, "autoFocus", 1);
    pivot = fp->exfunc->ini_load_int(fp, "pivot", 0);
    cacheRange = fp->exfunc->ini_load_int(fp, "cacheRange", 1);

    backgroundColor = IniLoadColor(fp, "backgroundColor", RGB(0x00, 0x00, 0x00));
    waveformColor = IniLoadColor(fp, "waveformColor", RGB(0x00, 0xff, 0x00));
    scaleColor = IniLoadColor(fp, "scaleColor", RGB(0xff, 0xff, 0xff));
    cursorColor = IniLoadColor(fp, "cursorColor", RGB(0xff, 0x00, 0x00));
    endColor = IniLoadColor(fp, "endColor", RGB(0x88, 0x88, 0x88));
    cacheColor = IniLoadColor(fp, "cacheColor", RGB(0xff, 0x00, 0x00));
    selectColor = IniLoadColor(fp, "selectColor", RGB(0x00, 0x7a, 0xcc));
    nonSelectColor = IniLoadColor(fp, "nonSelectColor", RGB(0x30, 0x30, 0x30));
    gridMainColor = IniLoadColor(fp, "gridMainColor", RGB(0x80, 0x80, 0x80));
    gridSubColor = IniLoadColor(fp, "gridSubColor", RGB(0x50, 0x50, 0x50));

    return true;
}

bool Config::SaveUserConfig(FILTER *fp) const
{
    fp->exfunc->ini_save_int(fp, "scaleX", scaleX);
    fp->exfunc->ini_save_int(fp, "scaleY", scaleY);
    fp->exfunc->ini_save_int(fp, "dbMin", dbMin);

    fp->exfunc->ini_save_int(fp, "enableGrid", enableGrid);
    fp->exfunc->ini_save_int(fp, "beat", beat);
    fp->exfunc->ini_save_int(fp, "offset", offset);
    if (fp->exfunc->ini_save_str(fp, "tempo", const_cast<LPSTR>(m_tempo.ToString().c_str())) == FALSE) {
        return false;
    }

    fp->exfunc->ini_save_int(fp, "autoFocus", autoFocus);
    fp->exfunc->ini_save_int(fp, "pivot", pivot);
    fp->exfunc->ini_save_int(fp, "cacheRange", cacheRange);

    fp->exfunc->ini_save_str(fp, "backgroundColor", const_cast<LPSTR>(ColorToString(backgroundColor).c_str()));
    fp->exfunc->ini_save_str(fp, "waveformColor", const_cast<LPSTR>(ColorToString(waveformColor).c_str()));
    fp->exfunc->ini_save_str(fp, "scaleColor", const_cast<LPSTR>(ColorToString(scaleColor).c_str()));
    fp->exfunc->ini_save_str(fp, "cursorColor", const_cast<LPSTR>(ColorToString(cursorColor).c_str()));
    fp->exfunc->ini_save_str(fp, "endColor", const_cast<LPSTR>(ColorToString(endColor).c_str()));
    fp->exfunc->ini_save_str(fp, "cacheColor", const_cast<LPSTR>(ColorToString(cacheColor).c_str()));
    fp->exfunc->ini_save_str(fp, "selectColor", const_cast<LPSTR>(ColorToString(selectColor).c_str()));
    fp->exfunc->ini_save_str(fp, "nonSelectColor", const_cast<LPSTR>(ColorToString(nonSelectColor).c_str()));
    fp->exfunc->ini_save_str(fp, "gridMainColor", const_cast<LPSTR>(ColorToString(gridMainColor).c_str()));
    fp->exfunc->ini_save_str(fp, "gridSubColor", const_cast<LPSTR>(ColorToString(gridSubColor).c_str()));

    return true;
}
