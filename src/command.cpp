//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
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

#ifdef __GNUG__
#pragma implementation "command.h"
#endif

// for (compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"

#include "command.h"
#include "ebook_processor.h"
#include "dlg_compile_book.h"
#include "installer.h"

// All actions that is posible to do via command line


//---------------------------------------------------------------------------------------
void CmdCompileBook(wxString sBook, bool fLanguages[], const wxString& sLocalePath,
                    wxTextCtrl* pGui, int nDbgOpt)
{
    lmEbookProcessor oEBP(nDbgOpt, pGui);

    //Loop to process each selected language
    for(int i=0; i < eLangLast; i++)
    {
        if (fLanguages[i])
        {
            wxLocale* pLocale = (wxLocale*)NULL;
            wxString sLang = g_tLanguages[i].sLang;
            wxFileName oBook(sBook);
            wxString sCtlg = oBook.GetName() + "_" + sLang;
            wxLogMessage(_T("Compliling eBook '%s' for language %s."),
                        sBook.c_str(), sLang.c_str() );
            if (i != 0)
            {

                //get wxLanguage name
                const wxLanguageInfo* pInfo = wxLocale::FindLanguageInfo(sLang);
                wxString sLangName;
                if (pInfo)
                {
                    sLangName = pInfo->Description;
                    wxLogMessage("OK. Language '%s' found in wxLanguageInfo. Name=%s",
                                 sLang.wx_str(), sLangName.wx_str() );
                }
                else
                {
                    wxLogMessage("Language '%s' not found in wxLanguageInfo. Using English.",
                                 sLang.wx_str() );
                    sLangName = "English";
                    sLang = "en";
                }

                // locale object re-initialization
                pLocale = new wxLocale();
                //if (!pLocale->Init("", sLang, "", false, true))
                //use default language because target language might not be installed
                //in this machine
                if (!pLocale->Init(wxLANGUAGE_DEFAULT, wxLOCALE_DONT_LOAD_DEFAULT))
                {
                    wxLogMessage("Language %s can not be set.", sLangName.wx_str());
                }
                else
                {
                    wxLogMessage("OK. Language %s has been set.", sLangName.wx_str());
                    wxFileName oPath;
                    oPath.AssignDir(sLocalePath);
                    oPath.AppendDir(sLang);
                    wxString sPath = oPath.GetFullPath();
                    //pLocale->AddCatalogLookupPathPrefix( sPath );
                    wxString sFile = sPath + sCtlg + ".mo";

                    if (wxFileExists(sFile))
                    {
                        wxLogMessage("OK. Catalog exists. Catalog: '%s'", sFile.wx_str());
                        if (!pLocale->AddCatalog(sFile))
                            wxLogMessage("Failure to load catalog '%s'",
                                         sFile.wx_str(), sPath.wx_str() );
                        else
                            wxLogMessage("OK. Catalog added: '%s'", sFile.wx_str());
                    }
                    else
                    {
                        wxLogMessage("Catalog '%s' not found!", sFile.wx_str());
                    }
                }


                if(! pLocale->IsOk() )
                {
                    wxLogMessage("Failure setting locale for '%s' language. English will be used.",
                             sLang.wx_str() );
                    delete pLocale;
                    pLocale = new wxLocale( wxLANGUAGE_ENGLISH );
                    sLang = "en";
                }
                else
                    wxLogMessage(_T("Locale changed to %s language. Now using %s.mo."),
                                 pLocale->GetName().c_str(), sCtlg.wx_str() );
            }
            int options = k_generate_lmd;
            oEBP.GenerateLMB(sBook, sLang, _T("utf-8"), options);

            delete pLocale;
        }
    }
}

//---------------------------------------------------------------------------------------
void CmdGeneratePot(wxString sBook, const wxString& sLocalePath,
                    wxTextCtrl* pGui, int nDbgOpt)
{
    //Create the .cpp file with all strings for eBook sBook
    wxLogMessage(_T("Generating POT strings"));

    //generate lang file
    lmEbookProcessor oEBP(nDbgOpt, pGui);
    wxFileName oFSrc(sBook);
    oFSrc.Normalize();
    oEBP.GenerateLMB(oFSrc.GetFullPath(), _T("en"), _T("utf-8"), k_generate_lang_file);

    //Delete created .lmd, .lms and .toc files
    wxString sFolder = oFSrc.GetName();
    wxFileName oFTmp(sLocalePath);
	oFTmp.AppendDir(_T("src"));	    // 'langtool/locale/src'
    oFTmp.SetName( sFolder );
    wxString sTmp = oFTmp.GetFullPath();
    //remove .lmd files
    sTmp += _T("*.lmd");
    wxString sHtmFile = wxFindFirstFile(sTmp);
    while ( !sHtmFile.empty() )
    {
        ::wxRemoveFile(sHtmFile);
        sHtmFile = wxFindNextFile();
    }
    //remove .lms files
    sTmp = oFTmp.GetFullPath();
    sTmp += _T("*.lms");
    sHtmFile = wxFindFirstFile(sTmp);
    while ( !sHtmFile.empty() )
    {
        ::wxRemoveFile(sHtmFile);
        sHtmFile = wxFindNextFile();
    }
    //remove .toc file
    sTmp = oFTmp.GetFullPath();
    sTmp += _T(".toc");
    ::wxRemoveFile(sTmp);

    //create the PO files for those languages that still do not have PO file
    wxLogMessage(_T("Verifying that all PO files exist"));

    for (int i=0; i < eLangLast; i++)
    {
        wxString sLang =  g_tLanguages[i].sLang;
        wxFileName oFDest(sLocalePath);
        oFDest.AppendDir(sLang);
        oFDest.SetName( oFSrc.GetName() + _T("_") + sLang );
        oFDest.SetExt(_T("po"));
        wxString sCharCode = _T("utf-8");
        if (!oFDest.FileExists())
        {
            //if file does not exist
            wxLogMessage(_T("Creating PO file %s\n"), oFDest.GetFullName().c_str());
            wxString sLangName = g_tLanguages[i].sLangName;
            if (!oEBP.CreatePoFile(oFDest.GetFullPath(), sCharCode, sLangName, sLang, sFolder))
            {
                wxLogMessage(_T("*** Error: PO file can not be created\n"));
            }
        }
    }
    wxLogMessage(_T("All PO files exist"));
}

//---------------------------------------------------------------------------------------
void CmdGenerateInstaller(bool fLanguages[], const wxString& sLocalePath,
                          wxTextCtrl* pGui, int nDbgOpt)
{

    //Loop to process each selected language
    for(int i=0; i < eLangLast; i++)
    {
        if (fLanguages[i])
        {
            wxLocale* pLocale = (wxLocale*)NULL;
            wxString sLang = g_tLanguages[i].sLang;
            wxLogMessage(_T("Generating installer strings for language %s."),
                         sLang.c_str() );
            if (i != 0)
            {
                pLocale = new wxLocale();
                wxString sNil = _T("");
                wxString sLang = g_tLanguages[i].sLang;
                wxString sLangName = g_tLanguages[i].sLangName;

                pLocale->Init(_T(""), sLang, _T(""), true, true);
                wxFileName oPath;
                oPath.AssignDir(sLocalePath);
                oPath.AppendDir(sLang);
                pLocale->AddCatalogLookupPathPrefix( oPath.GetFullPath() );
                wxString sCatalog = _T("lenmus_") + pLocale->GetName();
                wxLogMessage(_T("Using catalog %s%s.mo"), oPath.GetFullPath().c_str(), sCatalog.c_str());

                if (! pLocale->AddCatalog( sCatalog )) {
                    wxLogMessage(_T("Fail adding catalog %s%s"), oPath.GetFullPath().c_str(), sCatalog.c_str());
                }
                wxString sContent = lmInstaller::GetInstallerStrings(sLang, sLangName);
                wxLogMessage(sContent);

                //write content into file
                wxString sPath = _T("../../installer/msw/locale/") + sLang + _T(".nsh");
                wxLogMessage(_T("Writing file <%s>"), sPath.c_str());
                wxFile* pFile = new wxFile(sPath, wxFile::write);
                if (!pFile->IsOpened())
                {
                    //TODO
                    wxLogMessage(_T("Failed to open file <%s>"), sPath.c_str());
                    pFile = (wxFile*)NULL;
                    return;
                }
                pFile->Write(sContent);
                pFile->Close();
                delete pFile;

                delete pLocale;
            }
        }
    }
}

