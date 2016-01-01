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

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wininet.h>
#include <math.h>
#include <Objbase.h>
#include <strsafe.h>
#include <iphlpapi.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <ctime>
#include <algorithm>

#pragma comment(lib, "pluginapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "wininet.lib")

namespace std {
#if defined _UNICODE || defined UNICODE
	typedef wstring tstring;
	typedef wstringstream tstringstream;
	typedef wfstream tfstream;
#else
	typedef string tstring;
	typedef stringstream tstringstream;
	typedef fstream tfstream;
#endif
}

using namespace std;

#ifndef TRACE
#define TRACE(x) OutputDebugString(x)
#endif

#include "nsis\pluginapi.h"
#include "defines.h"
#include "utils.h"
#include "OperatingSystem\WindowsOperatingSystem.h"
#include "Events.h"
#include "MachineIdentifiers\MachineNameIdentifier.h"
#include "MachineIdentifiers\NetworkAdapterIdentifier.h"
#include "MachineIdentifiers\VolumeInfoIdentifier.h"
#include "md5.h"
