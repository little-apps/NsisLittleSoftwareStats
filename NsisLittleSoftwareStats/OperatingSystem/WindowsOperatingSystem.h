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

#include "..\Hardware\WindowsHardware.h"

#ifndef DLLVERSIONINFO
typedef struct _DllVersionInfo
{
	DWORD cbSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformID;
} DLLVERSIONINFO;

#endif

#ifndef DLLGETVERSIONPROC
typedef int (FAR WINAPI *DLLGETVERSIONPROC) (DLLVERSIONINFO *);
#endif

class WindowsOperatingSystem {
public:
	WindowsOperatingSystem() {
		// Get LCID
		this->nLcid = GetThreadLocale();

		// Get operating system info
		GetOSInfo();

		// Get .NET Framework version
		GetFrameworkVer();

		// Get Java version
		GetJavaVer();
	}
	~WindowsOperatingSystem() { }

	tstring strVersion;
	int nArchitecture;
	int nServicePack, nFrameworkSP;
	tstring strFrameworkVer;
	tstring strJavaVer;
	int nLcid;
	WindowsHardware cWindowsHardware;
private:
	void GetOSInfo() {
		OSVERSIONINFOEX osvi;
		SYSTEM_INFO si;
		HKEY hKey;
		DWORD dwSize;
		tstring strOSName;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

		if (GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi)) == FALSE) {
			this->strVersion = _T("Unknown");
			this->nServicePack = 0;
			return;
		}

		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		GetSystemInfo(&si);

		switch (osvi.dwPlatformId) {
			case VER_PLATFORM_WIN32_WINDOWS: 
				{
					switch (osvi.dwMajorVersion) {
						case 4:
							{
								switch (osvi.dwMinorVersion) {
									case 0:	
										{
											if (_tcscmp(osvi.szCSDVersion, _T("B")) == 0 || _tcscmp(osvi.szCSDVersion, _T("C")) == 0)
												strOSName = _T("Windows 95 R2");
											else
												strOSName = _T("Windows 95");
											break;
										}
									case 10:
										{
											if (_tcscmp(osvi.szCSDVersion, _T("A")) == 0)
												strOSName = _T("Windows 98 SE");
											else
												strOSName = _T("Windows 98");
											break;
										}
									case 90:
										{
											strOSName = _T("Windows ME");
											break;
										}
								}
								
								break;
							}
					}
					break;
				}
				case VER_PLATFORM_WIN32_NT: 
					{
						if (osvi.dwMajorVersion == 3)
						{
							strOSName = _T("Windows NT 3.5.1");
						} 
						else if (osvi.dwMajorVersion == 4)
						{
							strOSName = _T("Windows NT 4.0");
						} 
						else if (TestOSVersionMajor(5))
						{
							if (TestOSVersionMinor(0))
							{
								strOSName = _T("Windows 2000");
							}
							else if (TestOSVersionMinor(1))
							{
								strOSName = _T("Windows XP");
							}
							else if (TestOSVersionMinor(2))
							{
								if (TestOSSuite(VER_SUITE_WH_SERVER))
									strOSName = _T("Windows Home Server");
								else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 && TestOSProductType(VER_NT_WORKSTATION))
									strOSName = _T("Windows XP");
								else
								{
									strOSName = GetSystemMetrics(SM_SERVERR2) == 0 ? _T("Windows Server 2003") : _T("Windows Server 2003 R2");
								}
							}
						}
						else
						{
							auto bIsWorkstation = TestOSProductType(VER_NT_WORKSTATION);

							/*
								Because NSIS is a bit outdated, the OS version number will always be v6.2 if it is greater than or equal to v6.3.
								This is even with the manifest listed at https://msdn.microsoft.com/en-us/library/windows/desktop/dn481241%28v=vs.85%29.aspx.
								Therefore, it is required to use ntdll.dll to get an acurrate OS version.
							*/

							DWORD dwNtDllVersionMajor, dwNtDllVersionMinor;

							if (GetNtDllVersion(dwNtDllVersionMajor, dwNtDllVersionMinor))
							{
								// Version 6.x
								if (dwNtDllVersionMajor == 6)
								{
									if (dwNtDllVersionMinor == 0)
									{
										strOSName = bIsWorkstation ? _T("Windows Vista") : _T("Windows Server 2008");
									}
									else if (dwNtDllVersionMinor == 1)
									{
										strOSName = bIsWorkstation ? _T("Windows 7") : _T("Windows Server 2008 R2");
									}
									if (dwNtDllVersionMinor == 2)
									{
										strOSName = bIsWorkstation ? _T("Windows 8") : _T("Windows Server 2012");
									}
									else if (dwNtDllVersionMinor == 3)
									{
										strOSName = bIsWorkstation ? _T("Windows 8.1") : _T("Windows Server 2012 R2");
									}
									else if (dwNtDllVersionMinor == 4)
									{
										strOSName = _T("Windows 10 (Technical Preview)");
									}
								}

								// Version 10.x
								if (dwNtDllVersionMajor == 10)
								{
									strOSName = bIsWorkstation ? _T("Windows 10") : _T("Windows Server 2016");
								}	
							} 
							else
							{
								strOSName = _T("Unknown");
							}

						}

					}
					break;
		}

		this->strVersion = strOSName;
		this->nServicePack = osvi.wServicePackMajor;

		// Get OS Architecture
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			auto szArch = new TCHAR[100];

			ZeroMemory(szArch, 100);

			if (RegQueryValueEx(hKey, _T("PROCESSOR_ARCHITECTURE"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szArch), &dwSize) == ERROR_SUCCESS) {
				if (_tcscmp(szArch, _T("AMD64")) == 0)
					this->nArchitecture = 64;
				else
					this-> nArchitecture = 32;
			} else {
				this->nArchitecture = sizeof(PVOID) == 4 ? 32 : 64;
			}

			RegCloseKey(hKey);
		}
	}

	static BOOL TestOSVersionMajor(const DWORD dwMajor)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;

		//Initialize the OSVERSIONINFOEX structure
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXW));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		osvi.dwMajorVersion = dwMajor;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		//Initialize the condition mask
		VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		// Perform the test
		const auto ret = VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_PLATFORMID, dwlConditionMask);

		//Error checking
		if (!ret)
		{
			if (GetLastError() != ERROR_OLD_WIN_VERSION)
			{
				fprintf(stderr, "VerifyVersionInfo() system call has failed!");
			}
		}

		return ret != FALSE;
	}

	static BOOL TestOSVersionMinor(const DWORD dwMinor)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;

		//Initialize the OSVERSIONINFOEX structure
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXW));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		osvi.dwMinorVersion = dwMinor;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		//Initialize the condition mask
		VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		// Perform the test
		const auto ret = VerifyVersionInfoW(&osvi, VER_MINORVERSION | VER_PLATFORMID, dwlConditionMask);

		//Error checking
		if (!ret)
		{
			if (GetLastError() != ERROR_OLD_WIN_VERSION)
			{
				fprintf(stderr, "VerifyVersionInfo() system call has failed!");
			}
		}

		return ret != FALSE;
	}

	static BOOL TestOSSuite(WORD wSuite)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;

		// Initialize the OSVERSIONINFOEX structure
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXW));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		osvi.wSuiteMask = wSuite;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		// Initialize the condition mask
		VER_SET_CONDITION(dwlConditionMask, VER_SUITENAME, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		// Perform the test
		const auto ret = VerifyVersionInfoW(&osvi, VER_SUITENAME | VER_PLATFORMID, dwlConditionMask);

		//Error checking
		if (!ret)
		{
			if (GetLastError() != ERROR_OLD_WIN_VERSION)
			{
				fprintf(stderr, "VerifyVersionInfo() system call has failed!");
			}
		}

		return ret != FALSE;
	}

	static BOOL TestOSProductType(WORD wProductType)
	{
		OSVERSIONINFOEXW osvi;
		DWORDLONG dwlConditionMask = 0;

		// Initialize the OSVERSIONINFOEX structure
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXW));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
		osvi.wProductType = wProductType;
		osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;

		// Initialize the condition mask
		VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);
		VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

		// Perform the test
		const auto ret = VerifyVersionInfoW(&osvi, VER_PRODUCT_TYPE | VER_PLATFORMID, dwlConditionMask);

		//Error checking
		if (!ret)
		{
			if (GetLastError() != ERROR_OLD_WIN_VERSION)
			{
				fprintf(stderr, "VerifyVersionInfo() system call has failed!");
			}
		}

		return ret != FALSE;
	}

	static BOOL GetNtDllVersion(DWORD &dwMajor, DWORD &dwMinor)
	{
		DWORD dwLen, dwLang;
		BOOL bRet;

		auto szFilename = _T("ntdll.dll");

		LPVOID lpVersion = nullptr;
		LPVOID lpVersionInfo = nullptr;
		static TCHAR szFileVersion[256];
		auto bSuccess = true;

		auto vSize = GetFileVersionInfoSize(szFilename, &dwLen);
		if (vSize)
		{
			lpVersionInfo = malloc(vSize + 1);
			if (GetFileVersionInfo(szFilename, dwLen, vSize, lpVersionInfo))
			{
				_stprintf(szFileVersion, _T("\\VarFileInfo\\Translation"));
				bRet = VerQueryValue(lpVersionInfo, szFileVersion, &lpVersion, reinterpret_cast<UINT *>(&dwLen));
				if (bRet && dwLen == 4)
				{
					memcpy(&dwLang, lpVersion, 4);
					_stprintf(szFileVersion, _T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),
						(dwLang & 0xff00) >> 8, dwLang & 0xff, (dwLang & 0xff000000) >> 24,
						(dwLang & 0xff0000) >> 16);
				}
				else
				{
					_stprintf(szFileVersion, _T("\\StringFileInfo\\%04X04B0\\FileVersion"), GetUserDefaultLangID());
				}

				bRet = VerQueryValue(lpVersionInfo, szFileVersion, &lpVersion, reinterpret_cast<UINT *>(&dwLen));
				if (!bRet)
					bSuccess = false;
			}
			else
				bSuccess = false;
		}
		else
			bSuccess = false;

		if (bSuccess)
		{
			if (dwLen<256)
				_tcscpy(szFileVersion, static_cast<TCHAR *>(lpVersion));
			else
			{
				_tcsncpy(szFileVersion, static_cast<TCHAR *>(lpVersion), 250);
				szFileVersion[250] = 0;
			}

			if (lpVersionInfo) 
				free(lpVersionInfo);

			// Get first token (Major version number)
			auto szToken = _tcstok(szFileVersion, _T("."));
			if (szToken == nullptr)  // End of string
				return FALSE;       // String ended prematurely
			dwMajor = _tstoi(szToken);

			szToken = _tcstok(nullptr, TEXT("."));  // Get second token (Minor version number)
			if (szToken == nullptr)  // End of string
				return FALSE;       // String ended prematurely
			dwMinor = _tstoi(szToken);

			szToken = _tcstok(nullptr, TEXT("."));  // Get third token (Build number)
			if (szToken == nullptr)  // End of string
				return FALSE;       // String ended prematurely
			//dwBuildNumber = atoi(szToken);

			return true;
		}

		if (lpVersionInfo)
			free(lpVersionInfo);

		return false;
	}

	void GetJavaVer() {
		HKEY hKey;
		DWORD dwSize;
		auto szJavaVer = new TCHAR[100];

		ZeroMemory(szJavaVer, 100);

		if (cWindowsHardware.nCPUArch == 64) {
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\JavaSoft\\Java Runtime Environment"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
				if (RegQueryValueEx(hKey, _T("CurrentVersion"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szJavaVer), &dwSize) == ERROR_SUCCESS)
					this->strJavaVer = szJavaVer;

				RegCloseKey(hKey);
			}
		}

		if (this->strJavaVer.empty()) {
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\JavaSoft\\Java Runtime Environment"), 0, KEY_READ|KEY_WOW64_32KEY, &hKey) == ERROR_SUCCESS) {
				ZeroMemory(szJavaVer, 100);

				if (RegQueryValueEx(hKey, _T("CurrentVersion"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szJavaVer), &dwSize) == ERROR_SUCCESS)
					this->strJavaVer = szJavaVer;

				RegCloseKey(hKey);
			} else {
				this->strJavaVer = _T("0.0");
			}
		}
	}

	void GetFrameworkVer() {
		HKEY hKey, hSubKey;
		DWORD dwSize;

		this->nFrameworkSP = 0;

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD dwServicePack = 0;

			if (RegOpenKeyEx(hKey, _T("v4"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("4.0");

				if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v3.5"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("3.5");

				if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v3.0"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("3.0");

				if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v2.0.50727"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("2.0.50727");

				if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} else if (RegOpenKeyEx(hKey, _T("v1.1.4322"), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
				this->strFrameworkVer = _T("1.1.4322");

				if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
					this->nFrameworkSP = dwServicePack;
			} 

			if (hSubKey)
				RegCloseKey(hSubKey);

			RegCloseKey(hKey);
		} else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\.NETFramework\\policy\\v1.0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			DWORD dwServicePack = 0;

			if (RegQueryValueEx(hKey, _T("SP"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwServicePack), &dwSize) == ERROR_SUCCESS)
				this->nFrameworkSP = dwServicePack;

			RegCloseKey(hKey);
		} else {
			this->strFrameworkVer = _T("0.0");
			this->nFrameworkSP = 0;
		}
	}
};