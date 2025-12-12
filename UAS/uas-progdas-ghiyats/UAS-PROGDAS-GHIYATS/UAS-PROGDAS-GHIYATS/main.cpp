#include <wx/wx.h>
#include <wx/notebook.h>

#include "AUDIT_PANEL.h"
#include "DATA_PANEL.h"

class MyFrame : public wxFrame {
public:
    MyFrame();
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, "Ghiyats Inventory", wxDefaultPosition, wxSize(980, 600)) {

    SetBackgroundColour(*wxWHITE);

    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

    // Menambahkan panel ke tab
    notebook->AddPage(new DATA_PANEL(notebook), "DATA PANEL");
    notebook->AddPage(new AUDIT_PANEL(notebook), "AUDIT PANEL");
}