#include "pch.h"
#include "DecimalEx.h"

using namespace std;

DecimalEx::DecimalEx()
{
    signscale = 0;
    Hi32 = 0;
    Lo64 = 0;
}

DecimalEx::DecimalEx(int i)
{
    if (i < 0) {
        sign = DECIMAL_NEG;
        scale = 0;
        i = -i;
    }
    else {
        signscale = 0;
    }
    Hi32 = 0;
    Lo64 = i;
}

DecimalEx::DecimalEx(const std::string &s)
{
    signscale = 0;
    Hi32 = 0;
    Lo64 = 0;

    size_t idx = 0;
    if (s[idx] == '-') {
        sign = DECIMAL_NEG;
        idx++;
    }

    int point = -1;
    for (; idx < s.length(); idx++) {
        if ('0' <= s[idx] && s[idx] <= '9') {
            Lo64 = Lo64 * 10 + s[idx] - '0';
        }
        else if (point == -1 && s[idx] == '.') {
            point = idx;
        }
        else {
            break;
        }
    }
    if (point >= 0) {
        scale = static_cast<BYTE>(idx - point - 1);
    }
}

double DecimalEx::ToDouble() const
{
    double x = static_cast<double>(Lo64);
    if (sign != 0) {
        x = -x;
    }
    for (int i = 0; i < scale; i++) {
        x /= 10;
    }
    return x;
}

std::string DecimalEx::ToString() const
{
    string s = to_string(Lo64);
    if (scale >= s.length()) {
        s.insert(0, scale - s.length() + 1, '0');
    }
    if (sign != 0) {
        s.insert(0, "-");
    }
    if (scale > 0) {
        s.insert(s.length() - scale, ".");
    }
    return s;
}
