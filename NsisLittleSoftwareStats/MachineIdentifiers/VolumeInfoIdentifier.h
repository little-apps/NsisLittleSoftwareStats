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

class VolumeInfoIdentifier : MachineIdentifier {
public:
	VolumeInfoIdentifier(void) { }
	~VolumeInfoIdentifier(void) { }
	LPBYTE GetIdentifierHash() {
		LPTSTR szId = new TCHAR[50];
		TCHAR szVolumeName[MAX_PATH + 1];
		TCHAR szFileSystemName[MAX_PATH + 1];
		DWORD dwSerialNumber = 0;
		DWORD dwMaxComponentLen = 0;
		DWORD dwFileSystemFlags = 0;

		ZeroMemory(szId, 50);
		ZeroMemory(szVolumeName, MAX_PATH);
		ZeroMemory(szFileSystemName, MAX_PATH);

		if (GetVolumeInformation(
			NULL,
			szVolumeName,
			sizeof(szVolumeName),
			&dwSerialNumber,
			&dwMaxComponentLen,
			&dwFileSystemFlags,
			szFileSystemName,
			sizeof(szFileSystemName))) {
				_stprintf_s(szId, 50, _T("%lu"), dwSerialNumber); 
		}


		return this->ComputeHash(szId);
	}
};