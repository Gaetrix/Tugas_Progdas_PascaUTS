#ifndef DATA_PANEL_H
#define DATA_PANEL_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/grid.h>
#include <wx/choice.h>

class DATA_PANEL : public wxPanel
{
public:
    DATA_PANEL(wxWindow *parent);

private:
    wxGrid *grid;
    wxTextCtrl *nameCtrl;
    wxChoice *posCtrl;
    wxTextCtrl *amtCtrl;
    wxButton *addBtn;
    wxChoice *statusCtrl;

    wxButton *exportBtn;

    void OnSize(wxSizeEvent &event);
    void AdjustColumnWidths();
    void loadDataFromCSV();
    void onAddItemInventory(wxCommandEvent &event);
    void OnExportCSV(wxCommandEvent &event);

    wxDECLARE_EVENT_TABLE();
};

#endif // DATA_PANEL_H
