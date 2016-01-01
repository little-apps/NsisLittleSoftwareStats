/*
 * Little Software Stats - NSIS Plugin
 * Copyright (C) 2008-2012 Little Apps (http://www.little-apps.org)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

tstring GetMachineHash();
tstring ReplaceAll(tstring result, const tstring& replaceWhat, const tstring& replaceWithWhat);
tstring StringFormat(const TCHAR *fmt, ...);
tstring Enquoute(tstring s);
tstring GenerateId();
LPSTR ConvertUTF16ToUTF8(LPCWSTR pszTextUTF16);
int SendPost(tstring strUri, wstring strData);