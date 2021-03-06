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

class MachineNameIdentifier : MachineIdentifier {
public:
	MachineNameIdentifier(void) { }
	~MachineNameIdentifier(void) { }
	LPBYTE GetIdentifierHash() {
		auto szId = new TCHAR[100];
		DWORD nLength = 100;

		ZeroMemory(szId, 100);

		GetComputerName(szId, &nLength);

		return this->ComputeHash(szId);
	}
};