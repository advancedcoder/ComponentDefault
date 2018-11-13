#include <iostream>
#include <objbase.h>
#include "Intfaces.h"      
#include "ClassFactoryCsmt.h"
#include "GlobalElite.h"
#include "Registry.h"

using namespace std;

//
// Can DLL unload now?
//
STDAPI DllCanUnloadNow()
{
	if ((g_ComponentCount == 0) && (g_ServerLockCount == 0))
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

//
// Get class factory
//
STDAPI DllGetClassObject(const CLSID& clsid,
	const IID& iid,
	void** ppv)
{
	cout << "DllGetClassObject:\tCreate class factory." << endl;

	// Can we create this component?
	if (clsid != CLSID_Component)
	{
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	// Create class factory.
	ClassFactoryCstm* pFactory = new ClassFactoryCstm ;// No AddRef in constructor
	if (pFactory == NULL)
	{
		return E_OUTOFMEMORY ;
	}

	// Get requested interface.
	HRESULT hr = pFactory->QueryInterface(iid, ppv);
	pFactory->Release() ;

	return NULL;
}

//
// Server registration
//
STDAPI DllRegisterServer()
{
	return RegisterServer(g_hModule, CLSID_Component, FriendlyName, VerIndProgID, ProgID);
}

//
// Server unregistration
//
STDAPI DllUnregisterServer()
{
	return UnregisterServer(CLSID_Component, VerIndProgID, ProgID);
}

///////////////////////////////////////////////////////////
//
// DLL module information
//
BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD dwReason,
	void* lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_hModule = (HMODULE)hModule;
	}
	return TRUE;
}