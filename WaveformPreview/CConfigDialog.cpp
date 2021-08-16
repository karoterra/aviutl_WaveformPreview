#include "pch.h"
#include "resource.h"
#include "CConfigDialog.h"

IMPLEMENT_DYNAMIC(CConfigDialog, CDialogEx)

CConfigDialog::CConfigDialog(CWnd *pParent)
    : CDialogEx(IDD_CONFIG, pParent)
    , m_scaleX(0)
    , m_scaleY(0)
    , m_dbMin(-60)
    , m_enableGrid(0)
    , m_tempo(100)
    , m_beat(4)
    , m_offset(1)
    , m_autoFocus(1)
    , m_pivot(0)
    , m_cacheRange(0)
{
}

BEGIN_MESSAGE_MAP(CConfigDialog, CDialogEx)
END_MESSAGE_MAP()

void CConfigDialog::LoadConfig(const Config &config)
{
    m_scaleX = config.scaleX;
    m_scaleY = config.scaleY;
    m_dbMin = config.dbMin;

    m_enableGrid = config.enableGrid;
    m_tempo = *config.GetTempo();
    m_beat = config.beat;
    m_offset = config.offset;

    m_autoFocus = config.autoFocus;
    m_pivot = config.pivot;
    m_cacheRange = config.cacheRange;

    m_backgroundColor = config.backgroundColor;
    m_waveformColor = config.waveformColor;
    m_scaleColor = config.scaleColor;
    m_cursorColor = config.cursorColor;
    m_endColor = config.endColor;
    m_cacheColor = config.cacheColor;
    m_selectColor = config.selectColor;
    m_nonSelectColor = config.nonSelectColor;
    m_gridMainColor = config.gridMainColor;
    m_gridSubColor = config.gridSubColor;
}

void CConfigDialog::SaveConfig(Config &config) const
{
    config.scaleX = m_scaleX;
    config.scaleY = m_scaleY;
    config.dbMin = m_dbMin;

    config.enableGrid = m_enableGrid;
    config.SetTempo(m_tempo);
    config.beat = m_beat;
    config.offset = m_offset;

    config.autoFocus = m_autoFocus;
    config.pivot = m_pivot;
    config.cacheRange = m_cacheRange;

    config.backgroundColor = m_backgroundColor;
    config.waveformColor = m_waveformColor;
    config.scaleColor = m_scaleColor;
    config.cursorColor = m_cursorColor;
    config.endColor = m_endColor;
    config.cacheColor = m_cacheColor;
    config.selectColor = m_selectColor;
    config.nonSelectColor = m_nonSelectColor;
    config.gridMainColor = m_gridMainColor;
    config.gridSubColor = m_gridSubColor;
}

COLORREF CConfigDialog::GetColor(int nID) const
{
    return ((CMFCColorButton *)GetDlgItem(nID))->GetColor();
}

void CConfigDialog::SetColor(int nID, COLORREF color)
{
    ((CMFCColorButton *)GetDlgItem(nID))->SetColor(color);
}

void CConfigDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_SCALE_X_TIME, m_scaleX);
    DDX_Radio(pDX, IDC_SCALE_Y_LINEAR, m_scaleY);
    DDX_Text(pDX, IDC_DB_MIN, m_dbMin);
    DDV_MinMaxInt(pDX, m_dbMin, INT_MIN, -1);

    DDX_Check(pDX, IDC_ENABLE_GRID, m_enableGrid);
    DDX_Text(pDX, IDC_TEMPO, m_tempo);
    if (m_tempo.ToDouble() < 0) {
        AfxMessageBox("0‚æ‚è‘å‚«‚¢’l‚ð“ü—Í‚µ‚Ä‚­‚¾‚³‚¢");
        pDX->Fail();
    }
    if (m_tempo.ToString().length() > 1023) {
        AfxMessageBox("1023•¶ŽšˆÈ‰º‚Å“ü—Í‚µ‚Ä‚­‚¾‚³‚¢");
        pDX->Fail();
    }
    DDX_Text(pDX, IDC_BEAT, m_beat);
    DDV_MinMaxInt(pDX, m_beat, 1, INT_MAX);
    DDX_Text(pDX, IDC_OFFSET, m_offset);

    DDX_Check(pDX, IDC_AUTO_FOCUS, m_autoFocus);
    DDX_Radio(pDX, IDC_PIVOT_FRAME, m_pivot);
    DDX_Radio(pDX, IDC_CACHE_RANGE_ALL, m_cacheRange);

    if (pDX->m_bSaveAndValidate) {
        m_backgroundColor = GetColor(IDC_COLOR_BACKGROUND);
        m_waveformColor = GetColor(IDC_COLOR_WAVEFORM);
        m_scaleColor = GetColor(IDC_COLOR_SCALE);
        m_cursorColor = GetColor(IDC_COLOR_CURSOR);
        m_endColor = GetColor(IDC_COLOR_END);
        m_cacheColor = GetColor(IDC_COLOR_CACHE);
        m_selectColor = GetColor(IDC_COLOR_SELECT);
        m_nonSelectColor = GetColor(IDC_COLOR_NONSELECT);
        m_gridMainColor = GetColor(IDC_COLOR_GRID1);
        m_gridSubColor = GetColor(IDC_COLOR_GRID2);
    }
    else {
        SetColor(IDC_COLOR_BACKGROUND, m_backgroundColor);
        SetColor(IDC_COLOR_WAVEFORM, m_waveformColor);
        SetColor(IDC_COLOR_SCALE, m_scaleColor);
        SetColor(IDC_COLOR_CURSOR, m_cursorColor);
        SetColor(IDC_COLOR_END, m_endColor);
        SetColor(IDC_COLOR_CACHE, m_cacheColor);
        SetColor(IDC_COLOR_SELECT, m_selectColor);
        SetColor(IDC_COLOR_NONSELECT, m_nonSelectColor);
        SetColor(IDC_COLOR_GRID1, m_gridMainColor);
        SetColor(IDC_COLOR_GRID2, m_gridSubColor);
    }
}
