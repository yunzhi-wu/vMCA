// pluginTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include "plugin_api_v1.h"
#include "plugin_base.h"

extern char TestStrings[][512];

int _tmain(int argc, _TCHAR* argv[])
{
  HMODULE                         hModule;
  DLL_API_GetPluginAPIVersion_t   DLL_API_GetPluginAPIVersion;
  DLL_API_PluginVersion_t         DLL_API_Version;

  // DLL_API V1

#ifdef _WIN64

#ifdef _DEBUG
  hModule = LoadLibraryA("..\\build\\x64\\debug\\dll_plugin");
#else
  hModule = LoadLibraryA("..\\build\\x64\\release\\dll_plugin");
#endif

#else  // WIN32

#ifdef _DEBUG
  hModule = LoadLibraryA("..\\build\\Win32\\debug\\dll_plugin");
#else
  hModule = LoadLibraryA("..\\build\\Win32\\release\\dll_plugin");
#endif
#endif

  if (hModule == NULL)
  {
    printf("ERROR, failed to load the dll. Wrong path?\n");
    return -1;
  }

  DLL_API_GetPluginAPIVersion = (DLL_API_GetPluginAPIVersion_t) GetProcAddress(hModule, __DLL_API_GetPluginAPIVersion__);

  if (DLL_API_GetPluginAPIVersion != NULL)
  {
    DLL_API_GetPluginAPIVersion(&DLL_API_Version);
  }

  if (DLL_API_Version.version == DLL_API_VERSION_V1)
  {
    CPlugin_DLL_API_V1* plugIn_p;

    DLL_API_CreatePlugin_V1_t DLL_API_CreatePlugin_V1 = (DLL_API_CreatePlugin_V1_t) GetProcAddress(hModule, __DLL_API_CreatePlugin_V1__);

    if (DLL_API_CreatePlugin_V1 != NULL)
    {
      CList_LSZ*  decoderList_p;
      CDecoder*   decoder_p;

      plugIn_p = DLL_API_CreatePlugin_V1();

       if (plugIn_p->GetDecoders(&decoderList_p))
      {

        if (!decoderList_p->isEmpty())
        {
          char tempString[2048];
          unsigned length;

          decoder_p = (CDecoder*)decoderList_p->GetHead();

          while (decoder_p != NULL)
          {
            if (strcmp(decoder_p->GetMatchString(&length), "Time:") == 0)
            {
              // Time:1 Value:5.5
              strcpy_s(tempString, 1024, "Time:1 Value:704");
              length = (unsigned int)strlen(tempString);
              printf("DECODE result\nINPUT:%s\n", tempString);
              decoder_p->Decode(tempString, &length, 2048);
              printf("DECODE result\nOUTPUT:%s\n\n", tempString);
            }

            decoder_p = (CDecoder*)decoderList_p->GetNext(decoder_p);
          }
        }
      }
    }

    DLL_API_DeletePlugin_V1_t DLL_API_DeletePlugin_V1 = (DLL_API_DeletePlugin_V1_t) GetProcAddress(hModule, __DLL_API_DeletePlugin_V1__);

    if (DLL_API_DeletePlugin_V1 != NULL)
    {
      DLL_API_DeletePlugin_V1(plugIn_p);
      plugIn_p = NULL;
    }
  }

  // Place a break point here below

  if (hModule != NULL)
  {
    FreeLibrary(hModule);
  }

	return 0;
}