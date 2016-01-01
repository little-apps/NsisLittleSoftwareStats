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

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif

HINSTANCE g_hInstance;
HWND g_hwndParent;

BOOL APIENTRY DllMain( HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void __declspec(dllexport) TrackInstallation(HWND hwndParent, int string_size, 
	TCHAR *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	g_hwndParent=hwndParent;

	EXDLL_INIT();


	// note if you want parameters from the stack, pop them off in order.
	// i.e. if you are called via exdll::myFunction file.dat poop.dat
	// calling popstring() the first time would give you file.dat,
	// and the second time would give you poop.dat. 
	// you should empty the stack of your parameters, and ONLY your
	// parameters.
	LPTSTR szURI = new TCHAR[string_size];
	LPTSTR szFormat = new TCHAR[string_size];
	LPTSTR szAppID = new TCHAR[string_size];
	LPTSTR szAppVer = new TCHAR[string_size];

	tstring strUri, strFormat;
	tstring strApplicationId, strApplicationVer;
	tstring strUniqueId, strSessionId;

	popstring(szURI);
	popstring(szFormat);
	popstring(szAppID);
	popstring(szAppVer);

	WindowsOperatingSystem cWindowsOS;

	strUniqueId = GetMachineHash();
	strSessionId = GenerateId();

	strUri = szURI;
	strFormat = szFormat;

	strApplicationId = szAppID;
	strApplicationVer = szAppVer;

	if (strUri.empty() || 
		strFormat.empty() ||
		strApplicationId.empty() || 
		strApplicationVer.empty()) {
			pushint(10);
			return;
	}

	Events cEvents;

	EventData evStart(_T("strApp"), strSessionId);

	evStart.Add(_T("ID"), strUniqueId);
	evStart.Add(_T("aid"), strApplicationId);
	evStart.Add(_T("aver"), strApplicationVer);

	evStart.Add(_T("osv"), cWindowsOS.strVersion);
	evStart.Add(_T("ossp"), cWindowsOS.nServicePack);
	evStart.Add(_T("osar"), cWindowsOS.nArchitecture);
	evStart.Add(_T("osjv"), cWindowsOS.strJavaVer);
	evStart.Add(_T("osnet"), cWindowsOS.strFrameworkVer);
	evStart.Add(_T("osnsp"), cWindowsOS.nFrameworkSP);
	evStart.Add(_T("oslng"), cWindowsOS.nLcid);

	evStart.Add(_T("osscn"), cWindowsOS.cWindowsHardware.strScreenRes);
	evStart.Add(_T("cnm"), cWindowsOS.cWindowsHardware.strCPUName);
	evStart.Add(_T("car"), cWindowsOS.cWindowsHardware.nCPUArch);
	evStart.Add(_T("cbr"), cWindowsOS.cWindowsHardware.strCPUManufacturer);
	evStart.Add(_T("cfr"), cWindowsOS.cWindowsHardware.nCPUFreq);
	evStart.Add(_T("ccr"), cWindowsOS.cWindowsHardware.nCPUCores);
	evStart.Add(_T("mtt"), cWindowsOS.cWindowsHardware.nMemTotal);
	evStart.Add(_T("mfr"), cWindowsOS.cWindowsHardware.nMemFree);
	evStart.Add(_T("dtt"), cWindowsOS.cWindowsHardware.nDiskTotal);
	evStart.Add(_T("dfr"), cWindowsOS.cWindowsHardware.nDiskFree);

	cEvents.Add(evStart);

	EventData evInst(_T("ist"), strSessionId);

	evInst.Add(_T("ID"), strUniqueId);
	evInst.Add(_T("aid"), strApplicationId);
	evInst.Add(_T("aver"), strApplicationVer);

	cEvents.Add(evInst);

	cEvents.Add(EventData(_T("stApp"), strSessionId));

	wstring strData;

#if defined _UNICODE || defined UNICODE
	strData = cEvents.Serialize(strFormat);
#else
	string strDataMB = cEvents.Serialize(strFormat);
	strData.assign(strDataMB.begin(), strDataMB.end());
#endif

	pushint(SendPost(strUri, strData));

	return;
}

void __declspec(dllexport) TrackUninstallation(HWND hwndParent, int string_size, 
	TCHAR *variables, stack_t **stacktop,
	extra_parameters *extra)
{
	g_hwndParent=hwndParent;

	EXDLL_INIT();


	// note if you want parameters from the stack, pop them off in order.
	// i.e. if you are called via exdll::myFunction file.dat poop.dat
	// calling popstring() the first time would give you file.dat,
	// and the second time would give you poop.dat. 
	// you should empty the stack of your parameters, and ONLY your
	// parameters.
	LPTSTR szURI = new TCHAR[string_size];
	LPTSTR szFormat = new TCHAR[string_size];
	LPTSTR szAppID = new TCHAR[string_size];
	LPTSTR szAppVer = new TCHAR[string_size];

	tstring strUri, strFormat;
	tstring strApplicationId, strApplicationVer;
	tstring strUniqueId, strSessionId;

	popstring(szURI);
	popstring(szFormat);
	popstring(szAppID);
	popstring(szAppVer);

	WindowsOperatingSystem cWindowsOS;

	strUniqueId = GetMachineHash();
	strSessionId = GenerateId();

	strUri = szURI;
	strFormat = szFormat;

	strApplicationId = szAppID;
	strApplicationVer = szAppVer;

	if (strUri.empty() || 
		strFormat.empty() ||
		strApplicationId.empty() || 
		strApplicationVer.empty()) {
			pushint(10);
			return;
	}

	Events cEvents;
	EventData evStart(_T("strApp"), strSessionId);

	evStart.Add(_T("ID"), strUniqueId);
	evStart.Add(_T("aid"), strApplicationId);
	evStart.Add(_T("aver"), strApplicationVer);

	evStart.Add(_T("osv"), cWindowsOS.strVersion);
	evStart.Add(_T("ossp"), cWindowsOS.nServicePack);
	evStart.Add(_T("osar"), cWindowsOS.nArchitecture);
	evStart.Add(_T("osjv"), cWindowsOS.strJavaVer);
	evStart.Add(_T("osnet"), cWindowsOS.strFrameworkVer);
	evStart.Add(_T("osnsp"), cWindowsOS.nFrameworkSP);
	evStart.Add(_T("oslng"), cWindowsOS.nLcid);

	evStart.Add(_T("osscn"), cWindowsOS.cWindowsHardware.strScreenRes);
	evStart.Add(_T("cnm"), cWindowsOS.cWindowsHardware.strCPUName);
	evStart.Add(_T("car"), cWindowsOS.cWindowsHardware.nCPUArch);
	evStart.Add(_T("cbr"), cWindowsOS.cWindowsHardware.strCPUManufacturer);
	evStart.Add(_T("cfr"), cWindowsOS.cWindowsHardware.nCPUFreq);
	evStart.Add(_T("ccr"), cWindowsOS.cWindowsHardware.nCPUCores);
	evStart.Add(_T("mtt"), cWindowsOS.cWindowsHardware.nMemTotal);
	evStart.Add(_T("mfr"), cWindowsOS.cWindowsHardware.nMemFree);
	evStart.Add(_T("dtt"), cWindowsOS.cWindowsHardware.nDiskTotal);
	evStart.Add(_T("dfr"), cWindowsOS.cWindowsHardware.nDiskFree);

	cEvents.Add(evStart);

	EventData evInst(_T("ust"), strSessionId);

	evInst.Add(_T("ID"), strUniqueId);
	evInst.Add(_T("aid"), strApplicationId);
	evInst.Add(_T("aver"), strApplicationVer);

	cEvents.Add(evInst);

	cEvents.Add(EventData(_T("stApp"), strSessionId));

	wstring strData;

#if defined _UNICODE || defined UNICODE
	strData = cEvents.Serialize(strFormat);
#else
	string strDataMB = cEvents.Serialize(strFormat);
	strData.assign(strDataMB.begin(), strDataMB.end());
#endif

	pushint(SendPost(strUri, strData));

	return;
}

#ifdef __cplusplus
}
#endif