//--------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/fs_zip.h"
#include "wx/xrc/xmlres.h"          // use the xrc resource system
#include "wx/cmdline.h"             // to parse command line arguments
#include <wx/log.h>

#include "main_frame.h"
#include "paths.h"
#include "command.h"
#include "dlg_compile_book.h"

#include <iostream>
using namespace std;

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
//#if !defined(__WXMSW__) && !defined(__WXPM__)
//    #include "../sample.xpm"
//#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

private:
    // command line
    wxArrayString *m_fnames;
    bool ProcessCmdLine(wxChar** argv, int argc = 0);

    FILE* m_logfile;
};

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    m_logfile = NULL;

    // set information about this application
    const wxString sAppName = _T("langtool");
    SetVendorName(_T("LenMus"));
    SetAppName(sAppName);

    // Add support for zip files
    wxFileSystem::AddHandler(new wxZipFSHandler);

    // Get program directory and set up global paths object
    #ifdef __WXMSW__
    // On Windows, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOSX__
    // On Mac OS X, the path to the LenMus program is in argv[0]
    wxString sHomeDir = wxPathOnly(argv[0]);
    #endif
    #ifdef __MACOS9__
    // On Mac OS 9, the initial working directory is the one that
    // contains the program.
    wxString sHomeDir = wxGetCwd();
    #endif
    #ifdef __WXGTK__
    // On Mac OS 9, the initial working directory is the one that
    // contains the program.
    wxString sHomeDir = wxGetCwd();
    #endif
    wxFileName oRootPath(sHomeDir);     //sHomeDir is 'build' folder
    //oRootPath.RemoveLastDir();          //now we are in the langtool root
    g_pPaths = new lmPaths(sHomeDir);


    // get and process command line
    if (ProcessCmdLine(argv, argc))
        return false;   //batch mode. terminate

    // Interactive mode. Create GUI
    ltMainFrame *frame = new ltMainFrame(_T("LangTool - eMusicBooks and Lang files processor"), sHomeDir);

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    #if defined(__WXDEBUG__)
        #ifdef __WXGTK__
            //in Debug build, use a window to show wxLog messages. This is
            //the only way I found to see wxLog messages with Code::Blocks
            wxLogWindow* pMyLog = new wxLogWindow(frame, _T("Debug window: wxLogMessages"));
            wxLog::SetActiveTarget(pMyLog);
            pMyLog->Flush();
        #else
            wxLog::SetActiveTarget( new wxLogStderr );
        #endif
    #endif

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

int MyApp::OnExit()
{
    // delete all objects used by the App

    // path names
    delete g_pPaths;

    if (m_logfile)
        fclose(m_logfile);

    return 0;
}

bool MyApp::ProcessCmdLine(wxChar** argv, int argc)
{
    //Return FALSE for displaying GUI. TRUE for termination; in this
    //case, batch processing has been run.

    // get and process command line. Params:
    //
    // action:
    //  -c   compile ebook
    //  -p   generate/update pot strings
    //  -i   generate/update installer strings
    //
    // params for 'c' action
    //      -book       full-path to lenmus book source folder
    //      -out        full-path to lenmus locale folder
    //      -langs      list of languages, spaces separated or 'all'
    //
    // params for 'p' action
    //      -book       full-path to lenmus book source folder
    //      -out        full-path to langtool locale folder
    //
    // params for 'i' action
    //      -out        full-path to langtool locale folder
    //      -langs      list of languages, spaces separated or 'all'
    //
    // examples
    //  Compile book GeneralExercises for languages es,en,nl:
    //      langtool -c -book ..\..\books\src\GeneralExercises\GeneralExercises.xml -out ..\books -langs "es en nl"
    //
    //  Generate/Update strings file for book GeneralExercises:
    //      langtool -p -book ..\..\books\src\GeneralExercises\GeneralExercises.xml
    //
    //  Generate/Update installer file for all languages:
    //      langtool -i -langs="all"

    // The structure wxCmdLineEntryDesc is used to describe the one command line switch,
    // option or parameter.
    static const wxCmdLineEntryDesc cmdLineDesc[] = {
        {wxCMD_LINE_SWITCH, "v",     "verbose", "be verbose"},
        {wxCMD_LINE_SWITCH, "c",     NULL,      "compile ebook"},
        {wxCMD_LINE_SWITCH, "p",     NULL,      "generate/update strings for POT files and create missing PO files"},
        {wxCMD_LINE_SWITCH, "i",     NULL,      "generate/update installer strings"},
        {wxCMD_LINE_OPTION, "book",  NULL,      "Source eBook to process (full path)"},
        {wxCMD_LINE_OPTION, "out",   NULL,      "Path to store results"},
        {wxCMD_LINE_OPTION, "langs", NULL,      "languages"},
        //{wxCMD_LINE_PARAM,  NULL, NULL, "input files",
        // wxCMD_LINE_VAL_STRING,
        // wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE},
        // End of command list
        {wxCMD_LINE_SWITCH, "h",     "help",    "show this help text",
                            wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        {wxCMD_LINE_NONE}
    };

    // default values for params
    wxString sBook = _T("nil");
    wxString sOut = _T("nil");
    wxString sLangs = _T("all");

    //analyse the command line
    wxCmdLineParser oParser(cmdLineDesc, argc, argv);
    int nResp = oParser.Parse();
    bool fCompileBook = oParser.Found(_T("c"));
    bool fGeneratePot = oParser.Found(_T("p"));
    bool fGenerateInstaller = oParser.Found(_T("i"));

    if (nResp == -1)
    {
        //help was given, terminating
        return true;    //terminate
    }
    else if (nResp > 0)
    {
        wxLogMessage(_T("Invalid command line arguments: syntax error detected, aborting."));
        return true;    //terminate
    }
    else    //nResp == 0. Everything is ok; proceed
    {
        oParser.Found(_T("book"), &sBook);
        oParser.Found(_T("out"), &sOut);
        oParser.Found(_T("langs"), &sLangs);
        if (oParser.Found(_T("h")) || oParser.Found("help"))
        {
            //help was given, terminating
            return true;    //terminate
        }
    }


    bool fUseGUI = !( fCompileBook || fGeneratePot || fGenerateInstaller )
                   && nResp == 0;
    if (fUseGUI)
        return false;

    //run in batch mode and terminate
    m_logfile = fopen ("log.txt" , "w");
    //wxLog::SetActiveTarget( new wxLogStderr(m_logfile) );
    wxLogMessage(_T("Langtool version x.x"));
    wxLogMessage(_T("book=<%s>"), sBook.c_str());
    wxLogMessage(_T("out=<%s>"), sOut.c_str());
    wxLogMessage(_T("langs=<%s>"), sLangs.c_str());
    wxString actions;
    if (fCompileBook) actions += _T("<compile book> ");
    if (fGeneratePot) actions += _T("<generate pot strings> ");
    if (fGenerateInstaller) actions += _T("<generate installer strings> ");
    wxLogMessage(_T("Actions: %s"), actions.c_str());

    bool fLanguages[eLangLast];
    if (sLangs == _T("all"))
    {
        for(int i=0; i < eLangLast; i++)
            fLanguages[i] = true;
    }
    else
    {
        for(int i=0; i < eLangLast; i++)
            fLanguages[i] = false;

        fLanguages[eLangEnglish] = sLangs.Contains(_T("en"));
        fLanguages[eLangBasque] = sLangs.Contains(_T("eu"));
        fLanguages[eLangChinese] = sLangs.Contains(_T("zh_CN"));
        fLanguages[eLangDutch] = sLangs.Contains(_T("nl"));
        fLanguages[eLangFrench] = sLangs.Contains(_T("fr"));
        fLanguages[eLangGalician] = sLangs.Contains(_T("gl_ES"));
        fLanguages[eLangGerman] = sLangs.Contains(_T("de"));
        fLanguages[eLangGreek] = sLangs.Contains(_T("el"));
        fLanguages[eLangItalian] = sLangs.Contains(_T("it"));
        fLanguages[eLangRussian] = sLangs.Contains(_T("ru"));
        fLanguages[eLangSpanish] = sLangs.Contains(_T("es"));
        fLanguages[eLangTurkish] = sLangs.Contains(_T("tr"));

        for(int i=0; i < eLangLast; i++)
            wxLogMessage(_T("langs[%d] = %s"), i, (fLanguages[i] ? _T("true") : _T("false") ));
    }

    int nDbgOpt = 0;

    if (fCompileBook)
        CmdCompileBook(sBook, fLanguages, g_pPaths->GetLocalePath(), NULL /*no gui*/, nDbgOpt);

    if (fGeneratePot)
        CmdGeneratePot(sBook, g_pPaths->GetLocalePath(), NULL /*no gui*/, nDbgOpt);

    if (fGenerateInstaller)
        CmdGenerateInstaller(fLanguages, g_pPaths->GetLenMusLocalePath(), NULL /*no gui*/, nDbgOpt);

    return true;    //terminate
}

