#pragma once

#include "DecimalEx.h"
#include "Config.h"

class CConfigDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CConfigDialog)

public:
    int m_scaleX;
    int m_scaleY;
    int m_dbMin;
    
    int m_enableGrid;
    DecimalEx m_tempo;
    int m_beat;
    int m_offset;

    int m_autoFocus;
    int m_pivot;
    int m_cacheRange;

    COLORREF m_backgroundColor;
    COLORREF m_waveformColor;
    COLORREF m_scaleColor;
    COLORREF m_cursorColor;
    COLORREF m_endColor;
    COLORREF m_cacheColor;
    COLORREF m_selectColor;
    COLORREF m_nonSelectColor;
    COLORREF m_gridMainColor;
    COLORREF m_gridSubColor;

    CConfigDialog(CWnd *pParent = nullptr);

    void LoadConfig(const Config &config);
    void SaveConfig(Config &config) const;

    COLORREF GetColor(int nID) const;
    void SetColor(int nID, COLORREF color);

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};
