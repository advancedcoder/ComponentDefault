#include <iostream>
#include "FuncRealisation.h"
#include "GlobalElite.h"
#define TWOROOTS 2
#define ONEROOT 1
#define NOROOT 0

using namespace std;

FuncRealisation::FuncRealisation() : ref_count(1)
{
	InterlockedIncrement(&g_ComponentCount);
}

FuncRealisation::~FuncRealisation()
{
	InterlockedDecrement(&g_ComponentCount);
	cout << "Component: Destroy self." << endl;
}

HRESULT __stdcall FuncRealisation::QueryInterface(const IID& iid, void** ppv)
{
	if (iid == IID_IUnknown)
	{
		*ppv = static_cast<IX*>(this);
	}
	else if (iid == IID_IX)
	{
		*ppv = static_cast<IX*>(this);
		cout << "Component: Return pointer to INumRoots." << endl;
	}
	else if (iid == IID_IY)
	{
		*ppv = static_cast<IY*>(this);
		cout << "Component: Return pointer to ISolveEquat." << endl;
	}
	else if (iid == IID_IZ)
	{
		*ppv = static_cast<IZ*>(this);
		cout << "Component: Return pointer to IWho." << endl;
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown*>(*ppv)->AddRef();

	return S_OK;
}

ULONG __stdcall FuncRealisation::AddRef()
{
	return InterlockedIncrement(&ref_count);
}

ULONG __stdcall FuncRealisation::Release()
{
	if (InterlockedDecrement(&ref_count) == 0)
	{
		delete this;
		return 0;
	}
	return ref_count;
}