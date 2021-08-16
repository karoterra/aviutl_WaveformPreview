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

COLORREF Config::StringToColor(const std::string &str)
{
    if (str.length() < 6) {
        return RGB(0, 0, 0);
    }
    int r = stoi(str.substr(0, 2), nullptr, 16);
    int g = stoi(str.substr(2, 2), nullptr, 16);
    int b = stoi(str.substr(4, 2), nullptr, 16);
    return RGB(r, g, b);
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

    enableGrid = fp->exfunc->ini_load_int(fp, "enableGrid", 0);
    beat = fp->exfunc->ini_load_int(fp, "beat", 4);
    offset = fp->exfunc->ini_load_int(fp, "offset", 1);
    if (fp->exfunc->ini_load_str(fp, "tempo", buf, "100") == FALSE) {
        return false;
    }
    SetTempo(buf);

    autoFocus = fp->exfunc->ini_load_int(fp, "autoFocus", 1);
    pivot = fp->exfunc->ini_load_int(fp, "pivot", 0);
    cacheRange = fp->exfunc->ini_load_int(fp, "cacheRange", 1);

    if (fp->exfunc->ini_load_str(fp, "backgroundColor", buf, "000000") == FALSE) {
        return false;
    }
    backgroundColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "waveformColor", buf, "00ff00") == FALSE) {
        return false;
    }
    waveformColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "scaleColor", buf, "ffffff") == FALSE) {
        return false;
    }
    scaleColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "cursorColor", buf, "ff0000") == FALSE) {
        return false;
    }
    cursorColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "endColor", buf, "888888") == FALSE) {
        return false;
    }
    endColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "cacheColor", buf, "ff0000") == FALSE) {
        return false;
    }
    cacheColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "selectColor", buf, "007acc") == FALSE) {
        return false;
    }
    selectColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "nonSelectColor", buf, "303030") == FALSE) {
        return false;
    }
    nonSelectColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "gridMainColor", buf, "808080") == FALSE) {
        return false;
    }
    gridMainColor = StringToColor(buf);
    if (fp->exfunc->ini_load_str(fp, "gridSubColor", buf, "303030") == FALSE) {
        return false;
    }
    gridSubColor = StringToColor(buf);

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
    if (fp->exfunc->ini_save_str(fp, "tempo", const_cast<char*>(m_tempo.ToString().c_str())) == FALSE) {
        return false;
    }

    fp->exfunc->ini_save_int(fp, "autoFocus", autoFocus);
    fp->exfunc->ini_save_int(fp, "pivot", pivot);
    fp->exfunc->ini_save_int(fp, "cacheRange", cacheRange);

    fp->exfunc->ini_save_str(fp, "backgroundColor", const_cast<char *>(ColorToString(backgroundColor).c_str()));
    fp->exfunc->ini_save_str(fp, "waveformColor", const_cast<char *>(ColorToString(waveformColor).c_str()));
    fp->exfunc->ini_save_str(fp, "scaleColor", const_cast<char *>(ColorToString(scaleColor).c_str()));
    fp->exfunc->ini_save_str(fp, "cursorColor", const_cast<char *>(ColorToString(cursorColor).c_str()));
    fp->exfunc->ini_save_str(fp, "endColor", const_cast<char *>(ColorToString(endColor).c_str()));
    fp->exfunc->ini_save_str(fp, "cacheColor", const_cast<char *>(ColorToString(cacheColor).c_str()));
    fp->exfunc->ini_save_str(fp, "selectColor", const_cast<char *>(ColorToString(selectColor).c_str()));
    fp->exfunc->ini_save_str(fp, "nonSelectColor", const_cast<char *>(ColorToString(nonSelectColor).c_str()));
    fp->exfunc->ini_save_str(fp, "gridMainColor", const_cast<char *>(ColorToString(gridMainColor).c_str()));
    fp->exfunc->ini_save_str(fp, "gridSubColor", const_cast<char *>(ColorToString(gridSubColor).c_str()));

    return true;
}
