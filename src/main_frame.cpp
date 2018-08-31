//---------------------------------------------------------------------------------------
//    LenMus langtool: A tool for compiling and translating LenMus eBooks
//    Copyright (c) 2002-2016
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include <wx/filedlg.h>

#include "main_frame.h"
#include "installer.h"
#include "ebook_processor.h"
#include "help_processor.h"
#include "dlg_compile_book.h"
#include "paths.h"
#include "command.h"

//global variable
ltMainFrame* g_pmain_frame = (ltMainFrame*)NULL;

// IDs for the controls and the menu commands
enum
{
    // menu items
    MENU_QUIT = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    MENU_ABOUT = wxID_ABOUT,

    // Installer menu
    MENU_INSTALLER = wxID_HIGHEST + 100,

    // eBooks menu
    MENU_COMPILE_BOOK,
    MENU_GENERATE_PO,

    k_menu_options,

};

//---------------------------------------------------------------------------------------
// event tables and other macros for wxWidgets
//---------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ltMainFrame, wxFrame)

    // File menu
    EVT_MENU(MENU_QUIT,  ltMainFrame::OnQuit)
    EVT_MENU(MENU_ABOUT, ltMainFrame::OnAbout)

    // eBooks menu
    EVT_MENU(MENU_COMPILE_BOOK, ltMainFrame::OnCompileBook)
    EVT_MENU(MENU_GENERATE_PO, ltMainFrame::OnGeneratePO)

    // Installer menu
    EVT_MENU(MENU_INSTALLER, ltMainFrame::OnInstaller)

    //Options menu
    EVT_MENU(k_menu_options, ltMainFrame::OnOptions)

END_EVENT_TABLE()

//---------------------------------------------------------------------------------------
// main frame implementation
//---------------------------------------------------------------------------------------

// frame constructor
ltMainFrame::ltMainFrame(const wxString& title, const wxString& sRootPath)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600,400))
    , m_sRootPath(sRootPath)
{
//    // set the frame icon
//    SetIcon(wxICON(LangTool));

    m_sLenMusPath = g_pPaths->GetLenMusPath();

	// create the wxTextCtrl
	m_pText = new wxTextCtrl(this, -1,
        _T("This program is an utility to compile eMusicBooks and to\n")
        _T("create and to manage Lang translation files\n\n"),
		wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxHSCROLL | wxTE_DONTWRAP);
	m_pText->SetBackgroundColour(*wxWHITE);

    // create a menu bar

    // the Help menu
    wxMenu* pHelpMenu = new wxMenu;
    pHelpMenu->Append(MENU_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

    // the File menu
    wxMenu* pFileMenu = new wxMenu;
    pFileMenu->Append(MENU_QUIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // the Installer menu
    wxMenu* pInstMenu = new wxMenu;
    pInstMenu->Append(MENU_INSTALLER, _T("&Generate installer"), _T("Generate 'Installer' strings"));

    // the Options menu
    wxMenu* pOptMenu = new wxMenu;
    pOptMenu->Append(k_menu_options, _T("&Options"), _T("Options"));

    // items in the eBooks menu
    wxMenu* pBooksMenu = new wxMenu;
    pBooksMenu->Append(MENU_COMPILE_BOOK, _T("&Compile eBook"), _T("Convert eBook to LMB format"));
    pBooksMenu->Append(MENU_GENERATE_PO, _T("&Generate .pot file"), _T("Generate the POT file for the eBook and create PO file if doesn' exists"));

    // now append the freshly created menus to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(pFileMenu, _T("&File"));
    menuBar->Append(pBooksMenu, _T("&eBooks"));
    menuBar->Append(pInstMenu, _T("&Installer"));
    menuBar->Append(pOptMenu, _T("&Paths"));
    menuBar->Append(pHelpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);

    // create status bar
    CreateStatusBar(2);

    //set global pointer
    g_pmain_frame = this;
}


// event handlers

//---------------------------------------------------------------------------------------
void ltMainFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

//---------------------------------------------------------------------------------------
void ltMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( wxT("This program is part of the LenMus project (www.lenmus.org)\n\n")
                wxT("This program is an utility to compile eMusicBooks and to\n")
                wxT("create and to manage translation files\n\n")
                wxT("It uses the libxml2 library, a widely-used, powerful cross-platform\n")
                wxT("XML processor, written by Daniel Veillard for \n")
                wxT("the GNOME project. It is included in this program by using the\n")
                wxT("wxXml2 wrapper for wxWidgets, written by Francesco Montorsi.\n\n")
                wxT("This release is compiled with %s, \n")
                wxT("and libxml2 version %s.\n\n\n")
                wxT("Copyright (c) 2002-2018.\n")
                wxT("Licenced under GNU GPL v3+ licence terms.\n"),
                wxVERSION_STRING, lmEbookProcessor::GetLibxml2Version().wc_str() );

    wxMessageBox(msg, _T("About LangTool"), wxOK | wxICON_INFORMATION, this);
}

//---------------------------------------------------------------------------------------
void ltMainFrame::OnInstaller(wxCommandEvent& WXUNUSED(event))
{
    // loop to generate the file for each language
    for (int i=0; i < eLangLast; i++)
        GenerateLanguage(i);
}

//---------------------------------------------------------------------------------------
void ltMainFrame::OnOptions(wxCommandEvent& WXUNUSED(event))
{
    // ask for output path
    wxString outpath = g_pPaths->GetLocalePath();
    const wxString& dir = ::wxDirSelector("Choose folder for generated eBooks",
                                          outpath,      //default path
                                          0,            //style
                                          wxDefaultPosition,
                                          this);        //parent window
    if ( !dir.empty() )
    {
        wxMessageBox(dir);
    }
}

//---------------------------------------------------------------------------------------
void ltMainFrame::OnGeneratePO(wxCommandEvent& WXUNUSED(event))
{
    // ask for the file to covert
    wxString sFilter = wxT("*.xml");
    wxString sPath = ::wxFileSelector(_T("Choose the file to convert"),
                                        wxT(""),    //default path
                                        wxT(""),    //default filename
                                        wxT("xml"),    //default_extension
                                        sFilter,
                                        wxFD_OPEN,        //flags
                                        this);
    if ( sPath.IsEmpty() ) return;

    //get book name
    wxFileName oFN(sPath);
    const wxString sBookName = oFN.GetFullPath();

    //issue command
    ::wxBeginBusyCursor();
    int nDbgOpt=0;
    CmdGeneratePot(sBookName, g_pPaths->GetLocalePath(), m_pText, nDbgOpt);
    ::wxEndBusyCursor();
}

//---------------------------------------------------------------------------------------
void ltMainFrame::GenerateLanguage(int i)
{
    wxLocale* pLocale = new wxLocale();
    wxString sNil = _T("");
    wxString sLang = g_tLanguages[i].sLang;
    wxString sLangName = g_tLanguages[i].sLangName;

    pLocale->Init(_T(""), sLang, _T(""), true, true);
    wxString sPath = g_pPaths->GetLenMusLocalePath();
    pLocale->AddCatalogLookupPathPrefix( sPath );
    wxString sCatalog = _T("lenmus_") + pLocale->GetName();
    if (! pLocale->AddCatalog( sCatalog )) {
        wxMessageBox(wxString::Format(_T("Fail adding catalog %s"), sCatalog.wc_str()));
    }
    wxString sContent = lmInstaller::GetInstallerStrings(sLang, sLangName);
    wxMessageBox(sContent);
    sPath = sNil + _T(".\\") + sLang + _T(".nsh");
    PutContentIntoFile(sPath, sContent);

    delete pLocale;

}

//---------------------------------------------------------------------------------------
void ltMainFrame::PutContentIntoFile(wxString sPath, wxString sContent)
{
    wxFile* pFile = new wxFile(sPath, wxFile::write);
    if (!pFile->IsOpened()) {
        //todo
        pFile = (wxFile*)NULL;
        return;
    }
    pFile->Write(sContent);
    pFile->Close();
    delete pFile;


}

//---------------------------------------------------------------------------------------
void ltMainFrame::OnCompileBook(wxCommandEvent& WXUNUSED(event))
{
    //default options
    lmCompileBookOptions rOptions;
    wxFileName oBooksSrc(g_pPaths->GetBooksRootPath());
    oBooksSrc.AppendDir(_T("src"));
    rOptions.sSrcPath = oBooksSrc.GetFullPath();
    rOptions.sDestPath = g_pPaths->GetLenMusLocalePath();

    //show dialog to ask for book name and other options
    lmDlgCompileBook oDlg(this, &rOptions);
    int retcode = oDlg.ShowModal();
    if (retcode != wxID_OK)
        return;

    //get source path
    if ( rOptions.sSrcPath.IsEmpty() )
        return;

    //get debug options
    int nDbgOpt = 0;
    if (rOptions.fDump) nDbgOpt |= eDumpTree;
    if (rOptions.fLogTree) nDbgOpt |= eLogTree;

    //get book name
    wxFileName oFN(rOptions.sSrcPath);
    const wxString sBookName = oFN.GetFullPath();

    LogMessage(wxString::Format(_T("Preparing to process eMusicBook. src='%s', dest='%s'\n"),
               rOptions.sSrcPath.wc_str(), rOptions.sDestPath.wc_str() ));

    //issue command
    ::wxBeginBusyCursor();
    wxString sLocalePath = g_pPaths->GetLocalePath();
    CmdCompileBook(sBookName, rOptions.fLanguage, sLocalePath, m_pText, nDbgOpt);
    ::wxEndBusyCursor();
}

//---------------------------------------------------------------------------------------
void ltMainFrame::LogMessage(const wxString& sMsg)
{
    m_pText->AppendText(sMsg);
}

//---------------------------------------------------------------------------------------
const char* ltMainFrame::find_file_name(int argc, char* argv[], int idx)
{
    int i;
    int file_name_idx = 0;
    for(i = 1; i < argc; ++i)
    {
        if(argv[i][0] != '-')
        {
            if(file_name_idx == idx) return argv[i];
            ++file_name_idx;
        }
    }
    return 0;
}

