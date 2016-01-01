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

#include "MachineIdentifier.h"

class NetworkAdapterIdentifier : MachineIdentifier {
public:
	NetworkAdapterIdentifier(void) { }
	~NetworkAdapterIdentifier(void) { }

	LPBYTE GetIdentifierHash() {
		LPTSTR szNetworkAddress = new TCHAR[13];
		PIP_ADAPTER_INFO pAdapterInfo, pAdapter = NULL;
		DWORD dwRetVal = 0;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

		pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			TRACE(_T("Error allocating memory needed to call GetAdaptersinfo()"));
			return NULL;
		}

		// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			free(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);

			if (pAdapterInfo == NULL) {
				TRACE(_T("Error allocating memory needed to call GetAdaptersinfo()"));
				return NULL;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;

			while (pAdapter) {
				if (pAdapter->Type != MIB_IF_TYPE_LOOPBACK) {
					ZeroMemory(szNetworkAddress, 13);
					_stprintf_s(szNetworkAddress, 13, _T("%.2X%.2X%.2X%.2X%.2X%.2X"), 
						pAdapter->Address[0], 
						pAdapter->Address[1], 
						pAdapter->Address[2], 
						pAdapter->Address[3], 
						pAdapter->Address[4], 
						pAdapter->Address[5]
					);

					break;
				}

				pAdapter = pAdapter->Next;
			}
		} else {
			TRACE(_T("GetAdaptersInfo() call failed"));
			return NULL;
		}

		if (pAdapterInfo)
			free(pAdapterInfo);

		return this->ComputeHash(szNetworkAddress);
	}
};