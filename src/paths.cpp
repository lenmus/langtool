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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "paths.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/filename.h"

#include "paths.h"

// the only object
lmPaths* g_pPaths = (lmPaths*) NULL;

//-------------------------------------------------------------------------------------------
// lmPaths implementation
//-------------------------------------------------------------------------------------------

lmPaths::lmPaths(wxString& sBinPath)
{
    //Receives the full path to the langtool executable folder (/z_bin) and
    //extracts the root path
    if (sBinPath == _T(""))
        m_root.AssignCwd();
    else
        m_root.Assign(sBinPath, _T(""), wxPATH_NATIVE);

    m_root.Normalize();
//    wxLogMessage(_T("lmPaths::lmPaths] m_root (before) = '%s'"), m_root.GetFullPath().c_str());
    const wxArrayString& dirs = m_root.GetDirs();
    size_t i = m_root.GetDirCount() - 1;
    for (; i >= 0; --i)
    {
        if (dirs[i] == _T("langtool"))
            break;
        m_root.RemoveLastDir();
    }
    //here m_root points to 'langtool' root folder
//    wxLogMessage(_T("lmPaths::lmPaths] m_root (after) = '%s'"), m_root.GetFullPath().c_str());
    Init();

}

lmPaths::~lmPaths()
{
}


void lmPaths::Init()
{
    wxFileName path;

    //here m_root points to 'langtool' root folder
    wxLogMessage(_T("lmPaths::lmPaths] m_root = '%s'"), m_root.GetFullPath().c_str());

    // Paths in LangTool
    path = m_root;
    path.AppendDir(_T("locale"));
    m_sLocale = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    path = m_root;
    path.AppendDir(_T("layout"));
    m_sLayout = path.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    // Paths in LenMus
    wxFileName oLenMusPath = m_root;        // /<common-root>/langtool
    oLenMusPath.RemoveLastDir();            // /<common-root>
	oLenMusPath.AppendDir(_T("lenmus"));	// /<common-root>/lenmus
    m_sLenMus = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

	oLenMusPath.AppendDir(_T("books"));		// '/<common-root>/lenmus/books'
    m_sBooksRoot = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

	oLenMusPath.RemoveLastDir();
	oLenMusPath.AppendDir(_T("temp"));	// '/<common-root>/lenmus/temp'
    m_sLenMusTemp = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

	oLenMusPath.RemoveLastDir();
	oLenMusPath.AppendDir(_T("locale"));	// '/<common-root>/lenmus/locale'
    m_sLenMusLocale = oLenMusPath.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);

    wxLogMessage(_T("[lmPaths::Init] m_sLocale='%s'"), m_sLocale.c_str() );
    wxLogMessage(_T("[lmPaths::Init] m_sLayout='%s'"), m_sLayout.c_str() );
    wxLogMessage(_T("[lmPaths::Init] m_sLenMus='%s'"), m_sLenMus.c_str() );
    wxLogMessage(_T("[lmPaths::Init] m_sBooksRoot='%s'"), m_sBooksRoot.c_str() );
    wxLogMessage(_T("[lmPaths::Init] m_sLenMusTemp='%s'"), m_sLenMusTemp.c_str() );
    wxLogMessage(_T("[lmPaths::Init] m_sLenMusLocale='%s'"), m_sLenMusLocale.c_str() );
}

