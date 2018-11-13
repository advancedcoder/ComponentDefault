//
// Registry.cpp
//

#include <objbase.h>
#include <assert.h>

#include "Registry.h"

////////////////////////////////////////////////////////
//
// Internal helper functions prototypes
//

// Set the given key and its value.
BOOL setKeyAndValue(const char* pszPath,
	const char* szSubkey,
	const char* szValue);

// Convert a CLSID into a char string.
void CLSIDtochar(const CLSID& clsid,
	char* szCLSID,
	int length);

// Delete szKeyChild and all of its descendents.
LONG recursiveDeleteKey(HKEY hKeyParent, const char* szKeyChild);

wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

////////////////////////////////////////////////////////
//
// Constants
//

// Size of a CLSID as a string
const int CLSID_STRING_SIZE = 39;

/////////////////////////////////////////////////////////
//
// Public function implementation
//

//
// Register the component in the registry.
//
HRESULT RegisterServer(HMODULE hModule,            // DLL module handle
	const CLSID& clsid,         // Class ID
	const char* szFriendlyName, // Friendly Name
	const char* szVerIndProgID, // Programmatic
	const char* szProgID)       //   IDs
{
	// Get server location.
	char szModule[512];
	LPWSTR new_szModule = convertCharArrayToLPCWSTR(szModule);
	DWORD dwResult =
		::GetModuleFileName(hModule,
			new_szModule,
			sizeof(szModule) / sizeof(char));
	assert(dwResult != 0);

	// Convert the CLSID into a char.
	char szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

	// Build the key CLSID\\{...}
	char szKey[64];
	strcpy(szKey, "CLSID\\");
	strcat(szKey, szCLSID);

	// Add the CLSID to the registry.
	setKeyAndValue(szKey, NULL, szFriendlyName);

	// Add the server filename subkey under the CLSID key.
	setKeyAndValue(szKey, "InprocServer32", szModule);

	// Add the ProgID subkey under the CLSID key.
	setKeyAndValue(szKey, "ProgID", szProgID);

	// Add the version-independent ProgID subkey under CLSID key.
	setKeyAndValue(szKey, "VersionIndependentProgID",
		szVerIndProgID);

	// Add the version-independent ProgID subkey under HKEY_CLASSES_ROOT.
	setKeyAndValue(szVerIndProgID, NULL, szFriendlyName);
	setKeyAndValue(szVerIndProgID, "CLSID", szCLSID);
	setKeyAndValue(szVerIndProgID, "CurVer", szProgID);

	// Add the versioned ProgID subkey under HKEY_CLASSES_ROOT.
	setKeyAndValue(szProgID, NULL, szFriendlyName);
	setKeyAndValue(szProgID, "CLSID", szCLSID);

	return S_OK;
}

//
// Remove the component from the registry.
//
LONG UnregisterServer(const CLSID& clsid,         // Class ID
	const char* szVerIndProgID, // Programmatic
	const char* szProgID)       //   IDs
{
	// Convert the CLSID into a char.
	char szCLSID[CLSID_STRING_SIZE];
	CLSIDtochar(clsid, szCLSID, sizeof(szCLSID));

	// Build the key CLSID\\{...}
	char szKey[64];
	strcpy(szKey, "CLSID\\");
	strcat(szKey, szCLSID);

	// Delete the CLSID Key - CLSID\{...}
	LONG lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szKey);
	assert((lResult == ERROR_SUCCESS) ||
		(lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.

 // Delete the version-independent ProgID Key.
	lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szVerIndProgID);
	assert((lResult == ERROR_SUCCESS) ||
		(lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.

 // Delete the ProgID key.
	lResult = recursiveDeleteKey(HKEY_CLASSES_ROOT, szProgID);
	assert((lResult == ERROR_SUCCESS) ||
		(lResult == ERROR_FILE_NOT_FOUND)); // Subkey may not exist.

	return S_OK;
}

///////////////////////////////////////////////////////////
//
// Internal helper functions
//

// Convert a CLSID to a char string.
void CLSIDtochar(const CLSID& clsid,
	char* szCLSID,
	int length)
{
	assert(length >= CLSID_STRING_SIZE);
	// Get CLSID
	LPOLESTR wszCLSID = NULL;
	HRESULT hr = StringFromCLSID(clsid, &wszCLSID);
	assert(SUCCEEDED(hr));

	// Covert from wide characters to non-wide.
	wcstombs(szCLSID, wszCLSID, length);

	// Free memory.
	CoTaskMemFree(wszCLSID);
}

//
// Delete a key and all of its descendents.
//
LONG recursiveDeleteKey(HKEY hKeyParent,           // Parent of key to delete
	const char* lpszKeyChild)  // Key to delete
{
	// Open the child.
	HKEY hKeyChild;
	LPCWSTR new_lpszKeyChild = convertCharArrayToLPCWSTR(lpszKeyChild);
	LONG lRes = RegOpenKeyEx(hKeyParent, new_lpszKeyChild, 0,
		KEY_ALL_ACCESS, &hKeyChild);
	if (lRes != ERROR_SUCCESS)
	{
		return lRes;
	}

	// Enumerate all of the decendents of this child.
	FILETIME time;
	char szBuffer[256];
	DWORD dwSize = 256;
	LPWSTR new_szBuffer = convertCharArrayToLPCWSTR(szBuffer);
	while (RegEnumKeyEx(hKeyChild, 0, new_szBuffer, &dwSize, NULL,
		NULL, NULL, &time) == S_OK)
	{
		// Delete the decendents of this child.
		lRes = recursiveDeleteKey(hKeyChild, szBuffer);
		if (lRes != ERROR_SUCCESS)
		{
			// Cleanup before exiting.
			RegCloseKey(hKeyChild);
			return lRes;
		}
		dwSize = 256;
	}

	// Close the child.
	RegCloseKey(hKeyChild);

	// Delete this child.
	return RegDeleteKey(hKeyParent, new_lpszKeyChild);
}

//
// Create a key and set its value.
//   - This helper function was borrowed and modifed from
//     Kraig Brockschmidt's book Inside OLE.
//
BOOL setKeyAndValue(const char* szKey,
	const char* szSubkey,
	const char* szValue)
{
	HKEY hKey;
	char szKeyBuf[1024];

	// Copy keyname into buffer.
	strcpy(szKeyBuf, szKey);

	// Add subkey name to buffer.
	if (szSubkey != NULL)
	{
		strcat(szKeyBuf, "\\");
		strcat(szKeyBuf, szSubkey);
	}
	
	LPCWSTR new_szKeyBuf = convertCharArrayToLPCWSTR(szKeyBuf);

	// Create and open key and subkey.
	long lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT,
		new_szKeyBuf,
		0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL,
		&hKey, NULL);
	if (lResult != ERROR_SUCCESS)
	{
		return FALSE;
	}

	// Set the Value.
	if (szValue != NULL)
	{
		RegSetValueEx(hKey, NULL, 0, REG_SZ,
			(BYTE *)szValue,
			strlen(szValue) + 1);
	}

	RegCloseKey(hKey);
	return TRUE;
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}