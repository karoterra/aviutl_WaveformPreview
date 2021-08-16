#pragma once
struct DecimalEx : public DECIMAL
{
    DecimalEx();
    DecimalEx(int i);
    DecimalEx(const std::string &s);

    double ToDouble() const;
    std::string ToString() const;
};
