#include "DATA_PANEL.h"

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/datetime.h>
#include <wx/textfile.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <fstream>
#include <sstream>

enum {
    ID_EXPORT_CSV = wxID_HIGHEST + 5000
};

static wxString GetCSVPath()
{
    wxString exeDir = wxStandardPaths::Get().GetExecutablePath();
    wxFileName fn(exeDir);
    wxString dir = fn.GetPath();
    wxFileName dataDir(dir + wxFileName::GetPathSeparator() + "data", "");
    if (!dataDir.DirExists())
    {
        wxFileName::Mkdir(dataDir.GetFullPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
    wxString csvPath = dataDir.GetFullPath() + wxFileName::GetPathSeparator() + "data.csv";
    return csvPath;
}

static wxArrayString parseCSVLine(const wxString &line)
{
    wxArrayString out;
    std::string s = line.ToStdString();
    std::stringstream ss(s);
    std::string cell;
    bool inQuotes = false;
    std::string cur;
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

static wxString csvEscape(const wxString &field)
{
    if (field.Find(',') == wxNOT_FOUND && field.Find('"') == wxNOT_FOUND && field.Find('\n') == wxNOT_FOUND)
        return field;
    wxString escaped = field;
    escaped.Replace("\"", "\"\"");
    return wxString("\"") + escaped + wxString("\"");
}

wxBEGIN_EVENT_TABLE(DATA_PANEL, wxPanel)
    EVT_SIZE(DATA_PANEL::OnSize)
    EVT_BUTTON(ID_EXPORT_CSV, DATA_PANEL::OnExportCSV)
wxEND_EVENT_TABLE()

DATA_PANEL::DATA_PANEL(wxWindow *parent)
    : wxPanel(parent, wxID_ANY)
{

    SetBackgroundColour(*wxWHITE);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText *title = new wxStaticText(this, wxID_ANY, "Auditing Panel");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(14);
    titleFont.MakeBold();
    title->SetFont(titleFont);
    mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(20, 6);
    grid->HideRowLabels();
    grid->DisableDragRowSize();
    grid->SetColLabelValue(0, "ID");
    grid->SetColLabelValue(1, "Update");
    grid->SetColLabelValue(2, "Nama Item");
    grid->SetColLabelValue(3, "Letak");
    grid->SetColLabelValue(4, "Jumlah");
    grid->SetColLabelValue(5, "Status");

    grid->SetColSize(0, 60);
    grid->SetColSize(1, 100);
    grid->SetColSize(2, 200);
    grid->SetColSize(3, 150);
    grid->SetColSize(4, 80);
    grid->SetColSize(5, 100);

    grid->EnableEditing(false);
    mainSizer->Add(grid, 1, wxALL | wxEXPAND, 10);

    wxBoxSizer *fieldgroup = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *namestatus = new wxBoxSizer(wxVERTICAL);
    wxStaticText *nameLabel = new wxStaticText(this, wxID_ANY, "NAMA BARANG:");
    nameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(320, -1));
    wxStaticText *statusLabel = new wxStaticText(this, wxID_ANY, "STATUS:");
    wxArrayString statusChoices;
    statusChoices.Add("Baik");
    statusChoices.Add("Baik Sebagian");
    statusChoices.Add("Rusak Total");
    statusChoices.Add("Rusak Sebagian");
    statusCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(320, -1), statusChoices);
    statusCtrl->SetSelection(0);

    namestatus->Add(nameLabel, 0, wxLEFT | wxTOP, 4);
    namestatus->Add(nameCtrl, 0, wxTOP | wxEXPAND, 6);
    namestatus->AddSpacer(6);
    namestatus->Add(statusLabel, 0, wxLEFT, 4);
    namestatus->Add(statusCtrl, 0, wxTOP | wxEXPAND, 6);

    wxBoxSizer *amtpos = new wxBoxSizer(wxVERTICAL);
    wxStaticText *amtLabel = new wxStaticText(this, wxID_ANY, "JUMLAH:");
    amtCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(320, -1));
    wxStaticText *posLabel = new wxStaticText(this, wxID_ANY, "LETAK (GUDANG):");
    wxArrayString posChoices;
    posChoices.Add("Gudang 1");
    posChoices.Add("Gudang 2");
    posChoices.Add("Gudang 3");
    posCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(320, -1), posChoices);
    posCtrl->SetSelection(0);

    amtpos->Add(amtLabel, 0, wxLEFT | wxTOP, 4);
    amtpos->Add(amtCtrl, 0, wxTOP | wxEXPAND, 6);
    amtpos->AddSpacer(6);
    amtpos->Add(posLabel, 0, wxLEFT, 4);
    amtpos->Add(posCtrl, 0, wxTOP | wxEXPAND, 6);

    fieldgroup->Add(namestatus, 1, wxALL | wxEXPAND, 8);
    fieldgroup->Add(amtpos, 1, wxALL | wxEXPAND, 8);

    mainSizer->Add(fieldgroup, 0, wxLEFT | wxRIGHT | wxTOP | wxEXPAND, 10);

    addBtn = new wxButton(this, wxID_ANY, "TAMBAH");
    addBtn->Bind(wxEVT_BUTTON, &DATA_PANEL::onAddItemInventory, this);

    exportBtn = new wxButton(this, ID_EXPORT_CSV, "EXPORT CSV");

    wxBoxSizer *btnRow = new wxBoxSizer(wxHORIZONTAL);
    btnRow->Add(addBtn, 0, wxRIGHT | wxEXPAND, 6);
    btnRow->Add(exportBtn, 0, wxLEFT | wxEXPAND, 6);
    mainSizer->Add(btnRow, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    this->SetSizerAndFit(mainSizer);
    loadDataFromCSV();
}

void DATA_PANEL::AdjustColumnWidths()
{
    if (!grid)
        return;

    int gridWidth = grid->GetClientSize().GetWidth();
    if (gridWidth <= 0)
        return;

    int scrollbarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);
    int availableWidth = gridWidth - scrollbarWidth;

    if (availableWidth <= 0)
        return;

    double proportions[6] = {0.08, 0.13, 0.28, 0.18, 0.15, 0.18};

    for (int col = 0; col < 6; col++)
    {
        int colWidth = static_cast<int>(availableWidth * proportions[col]);
        if (colWidth < 10)
            colWidth = 10;

        grid->SetColSize(col, colWidth);
    }
}

void DATA_PANEL::loadDataFromCSV()
{
    wxString csvPath = GetCSVPath();
    if (!wxFileExists(csvPath))
    {
        if (grid->GetNumberRows() > 0)
            grid->DeleteRows(0, grid->GetNumberRows());
        AdjustColumnWidths();
        return;
    }

    wxTextFile file(csvPath);
    if (!file.Open())
    {
        wxLogError("Gagal membuka file CSV: %s", csvPath);
        return;
    }

    size_t lineCount = file.GetLineCount();

    if (grid->GetNumberRows() > 0)
        grid->DeleteRows(0, grid->GetNumberRows());

    if (lineCount > 0)
        grid->AppendRows(lineCount);

    for (size_t r = 0; r < lineCount; ++r)
    {
        wxString line = file.GetLine(r);

        wxArrayString cols = parseCSVLine(line);

        while (cols.GetCount() < 6)
            cols.Add(wxEmptyString);

        for (int c = 0; c < 6; ++c)
        {
            grid->SetCellValue(r, c, cols[c]);
        }
    }

    file.Close();
    AdjustColumnWidths();
}

void DATA_PANEL::onAddItemInventory(wxCommandEvent &)
{
    wxString name = nameCtrl->GetValue().Trim(true).Trim(false);
    wxString amt = amtCtrl->GetValue().Trim(true).Trim(false);
    wxString pos = posCtrl->GetStringSelection();
    wxString status = statusCtrl->GetStringSelection();

    if (name.IsEmpty())
    {
        wxMessageBox("Nama barang tidak boleh kosong", "Error", wxICON_ERROR);
        return;
    }
    if (amt.IsEmpty())
    {
        wxMessageBox("Jumlah tidak boleh kosong", "Error", wxICON_ERROR);
        return;
    }

    wxString csvPath = GetCSVPath();
    long nextId = 1;
    if (wxFileExists(csvPath))
    {
        wxTextFile file(csvPath);
        if (file.Open() && file.GetLineCount() > 0)
        {
            wxString last = file.GetLine(file.GetLineCount() - 1);
            wxArrayString cols = parseCSVLine(last);
            if (cols.GetCount() > 0)
            {
                long lastId = 0;
                if (cols[0].ToLong(&lastId))
                    nextId = lastId + 1;
            }
            file.Close();
        }
    }
    wxDateTime now = wxDateTime::Now();
    wxString ts = now.FormatISOCombined(' ');

    wxString action = "";

    wxString line;
    line += wxString::Format("%ld,", nextId);
    line += csvEscape(ts) + ",";
    line += csvEscape(name) + ",";
    line += csvEscape(pos) + ",";
    line += csvEscape(amt) + ",";
    line += csvEscape(status) + ",";
    line += csvEscape(action);

    std::ofstream ofs(csvPath.ToStdString(), std::ios::app);
    if (!ofs.is_open())
    {
        wxMessageBox("Gagal menulis ke file CSV: " + csvPath, "Error", wxICON_ERROR);
        return;
    }
    ofs << line.ToStdString() << "\n";
    ofs.close();

    nameCtrl->Clear();
    amtCtrl->Clear();
    statusCtrl->SetSelection(0);
    posCtrl->SetSelection(0);

    loadDataFromCSV();
}

void DATA_PANEL::OnExportCSV(wxCommandEvent &)
{
    wxString csvPath = GetCSVPath();
    if (!wxFileExists(csvPath))
    {
        wxMessageBox("File sumber tidak ditemukan: " + csvPath, "Error", wxICON_ERROR);
        return;
    }

    wxFileDialog saveDlg(this, "Simpan CSV", "", "export.csv",
                         "CSV files (*.csv)|*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveDlg.ShowModal() != wxID_OK)
        return;

    wxString destPath = saveDlg.GetPath();

    bool ok = wxCopyFile(csvPath, destPath, false);
    if (!ok)
    {
        wxMessageBox("Gagal menyalin file CSV ke: " + destPath, "Error", wxICON_ERROR);
        return;
    }

    wxMessageBox("Export CSV berhasil: " + destPath, "Sukses", wxICON_INFORMATION);
}

void DATA_PANEL::OnSize(wxSizeEvent &event)
{
    Layout();
    AdjustColumnWidths();
    event.Skip();
}
