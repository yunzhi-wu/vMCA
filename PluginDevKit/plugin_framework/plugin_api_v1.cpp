//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// File: plugin_api_v1.cpp
//
// Description:
// Implementation of the DLL API used by LogScrutinizer to check API version and to create the
// plugin API class (class CPlugin_DLL_API_V1)
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Include files
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "plugin_api_v1.h"
#include "plugin_base.h"
#include "plugin_utils.h"
#include "plugin_text_parser.h"
#include "plugin_utils_internal.h"

#include <stdlib.h>

#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------

__declspec(dllexport) int __cdecl DLL_API_GetPluginAPIVersion(DLL_API_PluginVersion_t* version_p)
{
  version_p->version = DLL_API_VERSION_V1;
  return 1;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

__declspec(dllexport) int __cdecl DLL_API_AttachConfiguration(DLL_API_AttachConfiguration_t* configuration_p)
{
  EnableMsgTrace(configuration_p->hwnd_traceConsumer, configuration_p->h_traceHeap);
  return 1;
}


//-------------------------------------------------------------------------------------------------------------------------------------------------

__declspec(dllexport) CPlugin_DLL_API_V1* __cdecl DLL_API_CreatePlugin_V1(void)
{
  return DLL_API_Factory();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

__declspec(dllexport) void __cdecl DLL_API_DeletePlugin_V1(CPlugin_DLL_API_V1* plugIn_p)
{
  delete plugIn_p;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlugin_DLL_API_V1::~CPlugin_DLL_API_V1()
{
  if (!m_decoders.isEmpty())
  {
    m_decoders.DeleteAll();
  }

  if (!m_plots.isEmpty())
  {
    m_plots.DeleteAll();
  }
}