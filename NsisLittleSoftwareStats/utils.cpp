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

#include "stdafx.h"

tstring GetMachineHash() {
	LPBYTE szHash = new BYTE[48];
	LPBYTE pIdentifierHash;

	ZeroMemory(szHash, 48);

	MachineNameIdentifier cNameId;
	pIdentifierHash = cNameId.GetIdentifierHash();

	memcpy(szHash, pIdentifierHash, 16);

	NetworkAdapterIdentifier cNetAdaptId;
	pIdentifierHash = cNetAdaptId.GetIdentifierHash();

	memcpy(szHash+16, pIdentifierHash, 16);

	VolumeInfoIdentifier cVolInfo;
	pIdentifierHash = cVolInfo.GetIdentifierHash();

	memcpy(szHash+32, pIdentifierHash, 16);

	MD5 cMD5(szHash, 48);

	return tstring(cMD5.hexdigest());
}

tstring ReplaceAll(tstring result, const tstring& replaceWhat, const tstring& replaceWithWhat) {
	while (TRUE) {
		const int pos = result.find(replaceWhat);
		if (pos == -1) 
			break;
		result.replace(pos,replaceWhat.size(),replaceWithWhat);
	}

	return result;
}

tstring GenerateId() {
	GUID guid;
	BYTE *str;
	tstring unique;

	CoCreateGuid(&guid);

#if defined _UNICODE || defined UNICODE
	UuidToString(static_cast<UUID*>(&guid), static_cast<RPC_WSTR*>(&str));
#else
	UuidToString(static_cast<UUID*>(&guid), static_cast<RPC_CSTR*>(&str));
#endif

	unique = tstring(reinterpret_cast<LPTSTR>(str));

#if defined _UNICODE || defined UNICODE
	RpcStringFree(static_cast<RPC_WSTR*>(&str));
#else
	RpcStringFree(static_cast<RPC_CSTR*>(&str));
#endif

	unique = ReplaceAll(unique, _T("-"), _T(""));

	transform(unique.begin(), unique.end(), unique.begin(), toupper);

	return unique;
}

tstring StringFormat(const TCHAR *fmt, ...) {
	int needed, used;
	va_list ap;
	tstring output = _T("");

	va_start(ap, fmt);

	needed = _vsctprintf(fmt, ap) + 1;

	vector<tstring::value_type> str(needed, NULL);

	used = _vsntprintf_s(&str[0], str.size(), needed, fmt, ap);

	if (used > 0)
		output = tstring(str.begin(), str.end() - 1);

	va_end(ap);

	return output;
}

tstring Enquoute(tstring s) {
	if (s.empty())
		return tstring(_T("\"\""));

	int i;
	int nLen = s.length();
	tstringstream str;
	tstring t;

	str << _T('"');

	for (i = 0; i < nLen; i++) {
		auto c = s[i];

		if (c == '\\' || c == '"' || c == '>') {
			str << _T("\\");
			str << c;
		} else if (c == '\b')
			str << _T("\\b");
		else if (c == '\t')
			str << _T("\\t");
		else if (c == '\n')
			str << _T("\\n");
		else if (c == '\f')
			str << _T("\\f");
		else if (c == '\r')
			str << _T("\\r");
		else {
			if (c < _T(' ')) {
				str << StringFormat(_T("\\u%0.4X"), c);
			} else {
				str << c;
			}
		}
	}

	str << _T('"');

	return str.str();
}

LPSTR ConvertUTF16ToUTF8(LPCWSTR pszTextUTF16) {
	// Special case of NULL or empty input string

	if ( pszTextUTF16 == nullptr || *pszTextUTF16 == _T('\0') )
		// Return empty string
		return "";

	// Consider WCHAR's count corresponding to total input string length,
	// including end-of-string (L'\0') character.

	auto cchUTF16 = wcslen(pszTextUTF16);

	if (cchUTF16 <= 0) {
		return "";
	}

	// Consider also terminating \0
	++cchUTF16;

	// WC_ERR_INVALID_CHARS flag is set to fail if invalid input character is encountered.
	// This flag is supported on Windows Vista and later.
	// Don't use it on Windows XP and previous.
	OSVERSIONINFOEX osvi;
	DWORD dwConversionFlags;

	ZeroMemory(&osvi, sizeof(osvi));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&osvi)))
		// Failed to get OS version
		return "";

	if (osvi.dwMajorVersion >= 6 && osvi.dwMinorVersion >= 0)
		dwConversionFlags = WC_ERR_INVALID_CHARS;
	else
		dwConversionFlags = 0;

	// Get size of destination UTF-8 buffer, in CHAR's (= bytes)

	auto cbUTF8 = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		pszTextUTF16,           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's, including end-of-string \0
		nullptr,                   // unused - no conversion required in this step
		0,                      // request buffer size
		nullptr, nullptr              // unused
		);

	if (cbUTF8 == 0)
		return "";

	// Allocate destination buffer for UTF-8 string
	auto cchUTF8 = cbUTF8; // sizeof(CHAR) = 1 byte
	auto szUTF8 = new CHAR[cchUTF8];

	ZeroMemory(szUTF8, cchUTF8);

	// Do the conversion from UTF-16 to UTF-8
	auto result = ::WideCharToMultiByte(
		CP_UTF8,                // convert to UTF-8
		dwConversionFlags,      // specify conversion behavior
		pszTextUTF16,           // source UTF-16 string
		static_cast<int>( cchUTF16 ),   // total source string length, in WCHAR's, including end-of-string \0
		szUTF8,                 // destination buffer
		cbUTF8,                 // destination buffer size, in bytes
		nullptr, nullptr              // unused
		); 

	if (result == 0) 
		return "";

	// Return resulting UTF-8 string
	return szUTF8;
}

int SendPost(tstring strUri, wstring strData) {
	HINTERNET hInt = nullptr, hConn = nullptr, hReq = nullptr;
	int flags;
	DWORD dwSize, dwFlags;

	strData.insert(0, L"data=");

	try {
		auto szHdr = ConvertUTF16ToUTF8(L"Content-Type: application/x-www-form-urlencoded");
		auto szData = ConvertUTF16ToUTF8(strData.c_str());

		if (strcmp(szData, "") == 0) // Error converting to UTF8
			return FALSE;

		hInt = InternetOpen(API_USERAGENT, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

		if (hInt == nullptr)
			return FALSE;

		// Set HTTP request timeout
		if (API_TIMEOUT > 0) {
			auto nPostTimeout = API_TIMEOUT;
			InternetSetOption(hInt, INTERNET_OPTION_CONNECT_TIMEOUT, &nPostTimeout, sizeof nPostTimeout);
			InternetSetOption(hInt, INTERNET_OPTION_SEND_TIMEOUT, &nPostTimeout, sizeof nPostTimeout);
			InternetSetOption(hInt, INTERNET_OPTION_RECEIVE_TIMEOUT, &nPostTimeout, sizeof nPostTimeout);
		}

		// Crack URI
		URL_COMPONENTS urlComp;

		ZeroMemory(&urlComp, sizeof urlComp);
		urlComp.dwStructSize = sizeof urlComp;

		// Set required component lengths to non-zero so that they are cracked.
		urlComp.dwHostNameLength = static_cast<DWORD>(-1);
		urlComp.dwSchemeLength = static_cast<DWORD>(-1);
		urlComp.dwUrlPathLength = static_cast<DWORD>(-1);
		urlComp.dwExtraInfoLength = static_cast<DWORD>(-1);

		if (!InternetCrackUrl(strUri.c_str(), strUri.length(), 0, &urlComp)) {
			InternetCloseHandle(hInt);
			return FALSE;
		}

		tstring strHost = urlComp.lpszHostName;

		hConn = InternetConnect(hInt, strHost.substr(0, strHost.find_first_of('/')).c_str(), urlComp.nPort, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 1);

		if (hConn == nullptr) {
			InternetCloseHandle(hInt);
			return FALSE;
		}

		if (urlComp.nScheme == INTERNET_SCHEME_HTTPS)
			flags = INTERNET_FLAG_NO_UI | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		else
			flags = INTERNET_FLAG_NO_UI;

		hReq = HttpOpenRequest(hConn, _T("POST"), urlComp.lpszUrlPath, nullptr, nullptr, nullptr, flags, 1);

		if (hReq == nullptr) {
			InternetCloseHandle(hInt);
			InternetCloseHandle(hConn);

			return FALSE;
		}

		if (urlComp.nScheme == INTERNET_SCHEME_HTTPS) {
			dwSize = sizeof dwFlags;
			if (InternetQueryOption(hReq, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, &dwSize)) {
				dwFlags = dwFlags | SECURITY_FLAG_IGNORE_UNKNOWN_CA;
				if (!InternetSetOption(hReq, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof dwSize)) {
					InternetCloseHandle(hInt);
					InternetCloseHandle(hConn);
					InternetCloseHandle(hReq);

					return FALSE;
				}
			} else {
				InternetCloseHandle(hInt);
				InternetCloseHandle(hConn);
				InternetCloseHandle(hReq);

				return FALSE; //InternetQueryOption failed
			}
		}

		if (HttpSendRequestA(hReq, szHdr, strlen(szHdr), szData, strlen(szData))) {
			if (API_READRESPONSE == TRUE) {
				// Read server Response
				DWORD dwTotalRead = 0, dwBytesRead = READBUFFERSIZE;
				BYTE szBuff[READBUFFERSIZE];

				ZeroMemory(szBuff, READBUFFERSIZE);

				while (dwBytesRead > 0) {
					if (!InternetReadFile(hReq, szBuff, READBUFFERSIZE, &dwBytesRead)) {
						InternetCloseHandle(hInt);
						InternetCloseHandle(hConn);
						InternetCloseHandle(hReq);

						return FALSE;
					}

					if (dwBytesRead > 0 && dwBytesRead <= READBUFFERSIZE) {
						dwTotalRead += dwBytesRead;

						/* Do what you want with the response here */

						/* Note: The response is multibyte so use mbstowcs() to convert to wide string */

						InternetCloseHandle(hInt);
						InternetCloseHandle(hConn);
						InternetCloseHandle(hReq);
					}
				}
			}
		} else {
			InternetCloseHandle(hInt);
			InternetCloseHandle(hConn);
			InternetCloseHandle(hReq);

			return FALSE;
		}
	} catch(...) {
		InternetCloseHandle(hInt);
		InternetCloseHandle(hConn);
		InternetCloseHandle(hReq);

		return FALSE;
	};

	InternetCloseHandle(hInt);
	InternetCloseHandle(hConn);
	InternetCloseHandle(hReq);

	return TRUE;
}