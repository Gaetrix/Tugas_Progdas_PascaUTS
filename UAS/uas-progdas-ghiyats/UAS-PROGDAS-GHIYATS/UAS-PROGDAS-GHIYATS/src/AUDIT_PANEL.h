#ifndef AUDIT_PANEL_H
#define AUDIT_PANEL_H

#include <wx/wx.h>
#include <vector>

struct AuditRecord
{
    long id;
    wxString updateTime;
    wxString name;
    wxString location;
    wxString qty;
    wxString status;
    wxString action;
};

class AUDIT_PANEL : public wxPanel
{
public:
    AUDIT_PANEL(wxWindow* parent);

private:
    wxTextCtrl* searchCtrl;
    wxButton* searchBtn;
    wxBoxSizer* resultSizer;
    wxScrolledWindow* scroller;

    static wxString GetCSVPath();
    static wxArrayString parseCSVLine(const wxString &line);
    static wxString csvEscape(const wxString &field);

    std::vector<AuditRecord> loadAllRecords();
    bool writeAllRecords(const std::vector<AuditRecord>& records);

    void OnSearch(wxCommandEvent& evt);
    void DoSearch();
    void OnEditRecord(wxCommandEvent& evt);
    void OnDeleteRecord(wxCommandEvent& evt);

    wxPanel* MakeRecordCard(const AuditRecord& rec);

    wxDECLARE_EVENT_TABLE();
};

#endif // AUDIT_PANEL_H
