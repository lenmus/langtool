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
#ifndef __LANGTOOL_COMMAND_H__
#define __LANGTOOL_COMMAND_H__

#ifdef __GNUG__
#pragma interface "command.cpp"
#endif

extern void CmdCompileBook(wxString sBook, bool fLanguages[],
                           const wxString& sLocalePath,
                           wxTextCtrl* pGui, int nDbgOpt);

extern void CmdGeneratePot(wxString sBook, const wxString& sLocalePath,
                           wxTextCtrl* pGui, int nDbgOpt);

extern void CmdGenerateInstaller(bool fLanguages[], const wxString& sLocalePath,
                           wxTextCtrl* pGui, int nDbgOpt);


#endif  // __LANGTOOL_COMMAND_H__
