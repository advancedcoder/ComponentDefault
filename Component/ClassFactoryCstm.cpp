#include <iostream>
#include "FuncRealisation.h"
#include "ClassFactoryCsmt.h"
#include "GlobalElite.h"

using namespace std;

ClassFactoryCstm::ClassFactoryCstm() : ref_count(1) {}


ClassFactoryCstm::~ClassFactoryCstm()
{
	cout << "Class factory: Destroy self." << endl;
}

// IUnknown
HRESULT __stdcall ClassFactoryCstm::QueryInterface(const IID& iid, void** ppv)
{
	if ((iid == IID_IUnknown) || (iid == IID_IClassFactory))
	{
		*ppv = static_cast<IClassFactory*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

ULONG   __stdcall ClassFactoryCstm::AddRef()
{
	return InterlockedIncrement(&ref_count);
}

ULONG   __stdcall ClassFactoryCstm::Release()
{
	if (InterlockedDecrement(&ref_count) == 0)
	{
		delete this;
		return 0;
	}

	return ref_count;
}

// Interface IClassFactory
HRESULT __stdcall ClassFactoryCstm::CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv)
{
	cout << "Class factory: Create component." << endl;
	// Cannot aggregate.
	if (pUnknownOuter != NULL)
	{
		return CLASS_E_NOAGGREGATION;
	}

	// Create component.
	FuncRealisation* pFuncReal = new FuncRealisation;
	if (pFuncReal == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Get the requested interface.
	HRESULT hr = pFuncReal->QueryInterface(iid, ppv);

	// Release the IUnknown pointer.
	// (If QueryInterface failed, component will delete itself.)
	pFuncReal->Release();
	return hr;
}

HRESULT __stdcall ClassFactoryCstm::LockServer(BOOL bLock)
{
	if (bLock)
	{
		InterlockedIncrement(&g_ServerLockCount);
	}
	else
	{
		InterlockedDecrement(&g_ServerLockCount);
	}
	return S_OK;
}