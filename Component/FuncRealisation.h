#pragma once
#include "Intfaces.h"
#include <iostream>

using namespace std;

//const __declspec(dllexport) int AEqualsZeroException = 1;
//const __declspec(dllexport) int NegativeDescriminantException = 2;
//const __declspec(dllexport) int EmptyStringException = 3;

class AEqualsZeroException {};
class NegativeDescriminantException {};
class EmptyStringException {};

class FuncRealisation : public IX, public IY, public IZ
{
public:
	FuncRealisation();
	~FuncRealisation();
	// IUnknown
	virtual HRESULT __stdcall QueryInterface(const IID& iid, void** ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();

	// Interface INumRoots
	virtual void __stdcall Fx() { cout << "Fx" << endl; }

	// Interface ISolveEquat
	virtual void __stdcall Fy() { cout << "Fy" << endl; }

	// Interface IWho
	virtual void __stdcall Fz() { cout << "Fz" << endl; }

private:
	// Reference count
	long ref_count;
};

