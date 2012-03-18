/*
 * $Id$
 *
 * (C) 2012 see AUTHORS
 *
 * This file is part of mplayerc.
 *
 * Mplayerc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mplayerc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "stdafx.h"
#include "UpdateChecker.h"

#include <afxinet.h>

UpdateChecker::UpdateChecker(CString versionFileURL)
	: versionFileURL(versionFileURL)
{
}

UpdateChecker::~UpdateChecker(void)
{
}

bool UpdateChecker::isUpdateAvailable(const Version& currentVersion)
{
	bool updateAvailable = false;

	CInternetSession internet;
	CHttpFile* versionFile = (CHttpFile*) internet.OpenURL(versionFileURL,
														   1,
														   INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_RELOAD,
														   NULL,
														   0);

	if (versionFile)
	{
		CString latestVersionStr;
		char buffer[101];
		UINT br = 0;
		
		while (br = versionFile->Read(buffer, 50))
		{
			buffer[br] = '\0';
			latestVersionStr += buffer;
		}

		if (parseVersion(latestVersionStr) && isHigherVersion(latestVersion, currentVersion))
			updateAvailable = true;

		delete versionFile;
	}

	return updateAvailable;
}

bool UpdateChecker::isUpdateAvailable()
{
	return isUpdateAvailable(MPC_VERSION);
}

bool UpdateChecker::parseVersion(const CString& versionStr)
{
	bool success = false;

	if (!versionStr.IsEmpty())
	{
		UINT v[4];
		int curPos = 0;
		UINT i = 0;
		CString resToken = versionStr.Tokenize(_T("."), curPos);

		success = !resToken.IsEmpty();

		while (!resToken.IsEmpty() && i < _countof(v) && success)
		{
			if (EOF == _stscanf_s(resToken, _T("%u"), v + i))
				success = false;
			
			resToken = versionStr.Tokenize(_T("."), curPos);
			i++;
		}

		if (success && i == _countof(v))
		{
			latestVersion.major = v[0];
			latestVersion.minor = v[1];
			latestVersion.patch = v[2];
			latestVersion.revision = v[3];
		}
	}

	return success;
}

bool UpdateChecker::isHigherVersion(const Version& v1, const Version& v2) const
{
	return (v1.major > v2.major
		|| (v1.major == v2.major && (v1.minor > v2.minor 
									|| (v1.minor == v2.minor && (v1.patch > v2.patch
																|| (v1.patch == v2.patch && v1.revision > v2.revision))))));
}