#include "AUDIT_PANEL.h"

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/datetime.h>
#include <fstream>
#include <sstream>

enum {
    ID_SEARCH_BTN = wxID_HIGHEST + 1000,
    ID_RECORD_EDIT_BASE = wxID_HIGHEST + 2000,
    ID_RECORD_DELETE_BASE = wxID_HIGHEST + 4000
};

wxBEGIN_EVENT_TABLE(AUDIT_PANEL, wxPanel)
    EVT_BUTTON(ID_SEARCH_BTN, AUDIT_PANEL::OnSearch)
wxEND_EVENT_TABLE()

wxString AUDIT_PANEL::GetCSVPath()
{
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxFileName fn(exePath);
    wxString dir = fn.GetPath();
    wxString dataDir = dir + wxFileName::GetPathSeparator() + "data";
    if (!wxDirExists(dataDir))
    {
        wxFileName::Mkdir(dataDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    wxString csvPath = dataDir + wxFileName::GetPathSeparator() + "data.csv";
    return csvPath;
}

wxArrayString AUDIT_PANEL::parseCSVLine(const wxString &line)
{
    wxArrayString out;
    std::string s = line.ToStdString();
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '"')
        {
            if (inQuotes && i + 1 < s.size() && s[i + 1] == '"')
            {
                cur.push_back('"');
                ++i;
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == ',' && !inQuotes)
        {
            out.Add(wxString(cur));
            cur.clear();
        }
        else
        {
            cur.push_back(c);
        }
    }
    out.Add(wxString(cur));
    return out;
}

wxString AUDIT_PANEL::csvEscape(const wxString &field)
{
    if (field.Find(',') == wxNOT_FOUND && field.Find('"') == wxNOT_FOUND && field.Find('\n') == wxNOT_FOUND)
        return field;
    wxString escaped = field;
    escaped.Replace("\"", "\"\"");
    return wxString("\"") + escaped + wxString("\"");
}

std::vector<AuditRecord> AUDIT_PANEL::loadAllRecords()
{
    std::vector<AuditRecord> out;
    wxString csvPath = GetCSVPath();
    if (!wxFileExists(csvPath))
        return out;

    wxTextFile file(csvPath);
    if (!file.Open())
        return out;

    size_t n = file.GetLineCount();
    for (size_t i = 0; i < n; ++i)
    {
        wxString line = file.GetLine(i);
        if (line.IsEmpty()) continue;
        wxArrayString cols = parseCSVLine(line);
        while (cols.GetCount() < 7) cols.Add(wxEmptyString);
        AuditRecord r;
        long id = 0;
        cols[0].ToLong(&id);
        r.id = id;
        r.updateTime = cols[1];
        r.name = cols[2];
        r.location = cols[3];
        r.qty = cols[4];
        r.status = cols[5];
        r.action = cols[6];
        out.push_back(r);
    }
    file.Close();
    return out;
}

bool AUDIT_PANEL::writeAllRecords(const std::vector<AuditRecord>& records)
{
    wxString csvPath = GetCSVPath();
    wxFileName fn(csvPath);
    wxString folder = fn.GetPath();
    if (!wxDirExists(folder))
        wxFileName::Mkdir(folder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    std::ofstream ofs(csvPath.ToStdString(), std::ios::trunc);
    if (!ofs.is_open()) return false;

    for (const auto &r : records)
    {
        wxString line;
        line += wxString::Format("%ld,", r.id);
        line += csvEscape(r.updateTime) + ",";
        line += csvEscape(r.name) + ",";
        line += csvEscape(r.location) + ",";
        line += csvEscape(r.qty) + ",";
        line += csvEscape(r.status) + ",";
        line += csvEscape(r.action);
        ofs << line.ToStdString() << "\n";
    }
    ofs.close();
    return true;
}

AUDIT_PANEL::AUDIT_PANEL(wxWindow *parent)
    : wxPanel(parent, wxID_ANY), scroller(nullptr), resultSizer(nullptr)
{
    SetBackgroundColour(*wxWHITE);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText *title = new wxStaticText(this, wxID_ANY, "Search Panel");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.MakeBold();
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    wxBoxSizer *searchgroup = new wxBoxSizer(wxHORIZONTAL);
    searchCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(300, -1));
    searchBtn = new wxButton(this, ID_SEARCH_BTN, "CARI");

    searchgroup->Add(searchCtrl, 0, wxALL, 5);
    searchgroup->Add(searchBtn, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    mainSizer->Add(searchgroup, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxALIGN_CENTER_HORIZONTAL, 5);

    wxStaticBoxSizer *resultBox = new wxStaticBoxSizer(wxVERTICAL, this, "Hasil Pencarian");

    scroller = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL);
    scroller->SetScrollRate(5, 5);
    scroller->SetMinSize(wxSize(700, 300));

    resultSizer = new wxBoxSizer(wxVERTICAL);
    scroller->SetSizer(resultSizer);

    resultBox->Add(scroller, 1, wxALL | wxEXPAND, 10);
    mainSizer->Add(resultBox, 1, wxALL | wxEXPAND, 10);

    SetSizerAndFit(mainSizer);
}

wxPanel* AUDIT_PANEL::MakeRecordCard(const AuditRecord& rec)
{
    // parent must match the owner of resultSizer (scroller)
    wxPanel *card = new wxPanel(scroller, wxID_ANY);
    card->SetBackgroundColour(wxColour(245, 245, 245));
    wxBoxSizer *cardSizer = new wxBoxSizer(wxVERTICAL);

    auto makeRow = [&](const wxString &label, const wxString &value) {
        wxBoxSizer *row = new wxBoxSizer(wxHORIZONTAL);
        wxStaticText *lbl = new wxStaticText(card, wxID_ANY, label);
        wxFont labelFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        lbl->SetFont(labelFont);
        wxStaticText *val = new wxStaticText(card, wxID_ANY, value);
        row->Add(lbl, 0, wxRIGHT, 10);
        row->Add(val, 1, wxEXPAND);
        return row;
    };

    cardSizer->Add(makeRow("ID:", wxString::Format("%ld", rec.id)), 0, wxALL, 4);
    cardSizer->Add(makeRow("Update:", rec.updateTime), 0, wxALL, 4);
    cardSizer->Add(makeRow("Nama Item:", rec.name), 0, wxALL, 4);
    cardSizer->Add(makeRow("Letak:", rec.location), 0, wxALL, 4);
    cardSizer->Add(makeRow("Jumlah:", rec.qty), 0, wxALL, 4);
    cardSizer->Add(makeRow("Status:", rec.status), 0, wxALL, 4);

    wxBoxSizer *btnRow = new wxBoxSizer(wxHORIZONTAL);
    int editId = ID_RECORD_EDIT_BASE + static_cast<int>(rec.id);
    int delId  = ID_RECORD_DELETE_BASE + static_cast<int>(rec.id);
    wxButton *editBtn = new wxButton(card, editId, "Edit");
    wxButton *delBtn  = new wxButton(card, delId,  "Hapus");
    btnRow->Add(editBtn, 0, wxRIGHT, 5);
    btnRow->Add(delBtn, 0, wxRIGHT, 5);
    cardSizer->Add(btnRow, 0, wxALL, 6);

    card->SetSizer(cardSizer);

    editBtn->Bind(wxEVT_BUTTON, &AUDIT_PANEL::OnEditRecord, this);
    delBtn->Bind(wxEVT_BUTTON, &AUDIT_PANEL::OnDeleteRecord, this);

    return card;
}

void AUDIT_PANEL::OnSearch(wxCommandEvent& WXUNUSED(evt))
{
    DoSearch();
}

void AUDIT_PANEL::DoSearch()
{
    if (resultSizer)
        resultSizer->Clear(true);

    wxString q = searchCtrl->GetValue().Trim(true).Trim(false);
    std::vector<AuditRecord> all = loadAllRecords();
    std::vector<AuditRecord> found;

    if (q.IsEmpty())
    {
        found = all;
    }
    else
    {
        long idQuery = 0;
        bool isNum = q.ToLong(&idQuery);
        for (const auto &r : all)
        {
            if (isNum)
            {
                if (r.id == idQuery) found.push_back(r);
            }
            else
            {
                if (r.name.Lower().Find(q.Lower()) != wxNOT_FOUND ||
                    r.location.Lower().Find(q.Lower()) != wxNOT_FOUND)
                {
                    found.push_back(r);
                }
            }
        }
    }

    if (found.empty())
    {
        wxStaticText *empty = new wxStaticText(scroller, wxID_ANY, "Tidak ada hasil.");
        resultSizer->Add(empty, 0, wxALL, 5);
    }
    else
    {
        for (const auto &r : found)
        {
            wxPanel *card = MakeRecordCard(r);
            resultSizer->Add(card, 0, wxALL | wxEXPAND, 6);
        }
    }

    resultSizer->Layout();
    if (scroller)
    {
        scroller->FitInside();
        scroller->SetScrollRate(5,5);
    }
    Layout();
}

void AUDIT_PANEL::OnEditRecord(wxCommandEvent& evt)
{
    int wid = evt.GetId();
    long recId = wid - ID_RECORD_EDIT_BASE;

    std::vector<AuditRecord> records = loadAllRecords();
    int idx = -1;
    for (size_t i = 0; i < records.size(); ++i)
        if (records[i].id == recId) { idx = static_cast<int>(i); break; }

    if (idx == -1)
    {
        wxMessageBox("Record tidak ditemukan.", "Error", wxICON_ERROR);
        return;
    }

    AuditRecord r = records[idx];

    wxTextEntryDialog dName(this, "Nama Item:", "Edit Record", r.name);
    if (dName.ShowModal() != wxID_OK) return;
    r.name = dName.GetValue();

    wxTextEntryDialog dLoc(this, "Letak (Gudang):", "Edit Record", r.location);
    if (dLoc.ShowModal() != wxID_OK) return;
    r.location = dLoc.GetValue();

    wxTextEntryDialog dQty(this, "Jumlah:", "Edit Record", r.qty);
    if (dQty.ShowModal() != wxID_OK) return;
    r.qty = dQty.GetValue();

    wxArrayString statusChoices;
    statusChoices.Add("Baik");
    statusChoices.Add("Baik Sebagian");
    statusChoices.Add("Rusak Total");
    statusChoices.Add("Rusak Sebagian");
    int sel = wxGetSingleChoiceIndex("Status:", "Edit Record - Status", statusChoices, this);
    if (sel != wxNOT_FOUND)
        r.status = statusChoices[sel];

    r.updateTime = wxDateTime::Now().FormatISOCombined(' ');

    records[idx] = r;
    if (!writeAllRecords(records))
    {
        wxMessageBox("Gagal menulis file CSV.", "Error", wxICON_ERROR);
        return;
    }

    DoSearch();
}

void AUDIT_PANEL::OnDeleteRecord(wxCommandEvent& evt)
{
    int wid = evt.GetId();
    long recId = wid - ID_RECORD_DELETE_BASE;

    std::vector<AuditRecord> records = loadAllRecords();
    int idx = -1;
    for (size_t i = 0; i < records.size(); ++i)
        if (records[i].id == recId) { idx = static_cast<int>(i); break; }

    if (idx == -1)
    {
        wxMessageBox("Record tidak ditemukan.", "Error", wxICON_ERROR);
        return;
    }

    int answer = wxMessageBox(wxString::Format("Hapus record ID %ld ?", recId), "Konfirmasi", wxYES_NO | wxICON_QUESTION);
    if (answer != wxYES) return;

    records.erase(records.begin() + idx);
    if (!writeAllRecords(records))
    {
        wxMessageBox("Gagal menulis file CSV.", "Error", wxICON_ERROR);
        return;
    }

    DoSearch();
}
