#pragma once
#include <iostream>
#include <objbase.h>

class ClassFactoryCstm : IClassFactory
{
public:
	// Constructor
	ClassFactoryCstm();
	// Destructor
	~ClassFactoryCstm();
	
	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv) ;         
	virtual ULONG   __stdcall AddRef() ;
	virtual ULONG   __stdcall Release() ;

	// Interface IClassFactory
	virtual HRESULT __stdcall CreateInstance(IUnknown* pUnknownOuter, const IID& iid, void** ppv) ;
	virtual HRESULT __stdcall LockServer(BOOL bLock) ; 
private:
	long ref_count ;
};

