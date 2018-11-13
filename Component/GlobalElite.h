#pragma once
#include <objbase.h>

static HMODULE g_hModule = NULL;   // DLL module handle
static long g_ComponentCount = 0;     // Count of active components
static long g_ServerLockCount = 0;    // Count of locks

const char FriendlyName[] = "My own component, yay!!!";
const char VerIndProgID[] = "CLSID_Component";
const char ProgID[] = "CLSID_Component.1";
