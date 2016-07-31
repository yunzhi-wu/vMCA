// plugin_main.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved)
{
  static int status = 0;

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
    status = 1;
    break;
	case DLL_THREAD_ATTACH:
    status++;
    break;
	case DLL_THREAD_DETACH:
    status--;
    break;
	case DLL_PROCESS_DETACH:
    status = 0;
		break;
	}
	return TRUE;
}