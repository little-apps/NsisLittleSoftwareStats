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

class WindowsHardware {
public:
	WindowsHardware(void) {
		GetScreenRes();
		GetMemory();
		GetDiskSpace();
		GetCPUInfo();
	}

	~WindowsHardware(void) { }

	tstring strScreenRes;
	ULONG nMemTotal, nMemFree;
	ULONG nDiskTotal, nDiskFree;
	tstring strCPUName, strCPUManufacturer;
	int nCPUCores, nCPUArch, nCPUFreq;

private:
	void GetScreenRes() {
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);

		this->strScreenRes = StringFormat(_T("%dx%d"), rc.right, rc.bottom);
	}

	void GetMemory() {
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);

		if (GlobalMemoryStatusEx(&statex)) {
			this->nMemTotal = static_cast<ULONG>(statex.ullTotalPhys / 1024 / 1024);
			this->nMemFree = static_cast<ULONG>(statex.ullAvailPhys / 1024 / 1024);
		}

		return;
	}

	void GetDiskSpace() {
		ULONGLONG nFreeBytes, nTotalBytes;

		this->nDiskTotal = 0;
		this->nDiskFree = 0;

		auto dwSize = GetLogicalDriveStrings(0, nullptr);
		auto pszDrives = new TCHAR[dwSize + 2];
		GetLogicalDriveStrings(dwSize + 2,pszDrives);

		LPCTSTR pszDrv = pszDrives;
		while (*pszDrv) {
			if (GetDriveType(pszDrv) == DRIVE_FIXED) {
				nTotalBytes = 0;
				nFreeBytes = 0;

				GetDiskFreeSpaceEx(pszDrv, nullptr, reinterpret_cast<PULARGE_INTEGER>(&nTotalBytes), reinterpret_cast<PULARGE_INTEGER>(&nFreeBytes));

				this->nDiskTotal += static_cast<ULONG>(nTotalBytes / 1024 / 1024);
				this->nDiskFree += static_cast<ULONG>(nFreeBytes / 1024 / 1024);
			}

			pszDrv += _tcslen (pszDrv) + 1;
		}

		free(pszDrives);
	}

	void GetCPUInfo() {
		HKEY hKey;
		DWORD dwSize;
		auto szTemp = new TCHAR[1024];

		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
			// Get CPU name
			ZeroMemory(szTemp, 1024);
			if (RegQueryValueEx(hKey, _T("ProcessorNameString"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szTemp), &dwSize) == ERROR_SUCCESS) {
				this->strCPUName = szTemp;

				this->strCPUName = ReplaceAll(this->strCPUName, _T("(TM)"), _T(""));
				this->strCPUName = ReplaceAll(this->strCPUName, _T("(R)"), _T(""));
				this->strCPUName = ReplaceAll(this->strCPUName, _T(" "), _T(""));
			}

			// Get CPU manufacturer
			ZeroMemory(szTemp, 1024);
			if (RegQueryValueEx(hKey, _T("VendorIdentifier"), nullptr, nullptr, reinterpret_cast<LPBYTE>(szTemp), &dwSize) == ERROR_SUCCESS)
				this->strCPUManufacturer = szTemp;

			// Get CPU Frequency
			DWORD dwCPUFreq = 0;
			if (RegQueryValueEx(hKey, _T("~MHz"), nullptr, nullptr, reinterpret_cast<LPBYTE>(&dwCPUFreq), &dwSize) == ERROR_SUCCESS)
				this->nCPUFreq = dwCPUFreq;
		} else {
			this->strCPUManufacturer = _T("Unknown");
			this->strCPUName = _T("Unknown");
		}

		SYSTEM_INFO siSysInfo;
		GetNativeSystemInfo(&siSysInfo);

		if (siSysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			this->nCPUArch = 64;
		else
			this->nCPUArch = 32;

		this->nCPUCores = siSysInfo.dwNumberOfProcessors;

		RegCloseKey(hKey);
	}

	
};
