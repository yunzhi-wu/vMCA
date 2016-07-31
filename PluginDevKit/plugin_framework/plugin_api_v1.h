#ifndef PLUGIN_API_V1_H
#define PLUGIN_API_V1_H

//-----------------------------------------------------------------------------------------------------------------------------
// File: plugin_api_v1.h
//
// Description:
// This header file contains the DLL API used by LogScrutinizer to check API version and create the
// plugin API class (class CPlugin_DLL_API_V1)
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------

#include "plugin_base.h"
#include "plugin_utils.h"


//-----------------------------------------------------------------------------------------------------------------------------
// API: GetPluginAPIVersion
//
// Description: This function is the first called by LogScrutinizer to establish which interface the DLL plugin is using.
//              The function GetPluginAPIVersion will remain unchanged through all
//              API changes. This function is implemented in the plugin framework and responds with DLL_API_VERSION_V1

#define __DLL_API_GetPluginAPIVersion__  "DLL_API_GetPluginAPIVersion"

typedef int (__cdecl *DLL_API_GetPluginAPIVersion_t)(DLL_API_PluginVersion_t*);         // Implemented in dll_api_v1.cpp

//-----------------------------------------------------------------------------------------------------------------------------
// API: DLL_API_AttachConfiguration
//
// Description: This function setup attach configuration, such as logging and command channels

#define __DLL_API_AttachConfiguration__  "DLL_API_AttachConfiguration"

typedef void (__cdecl *DLL_API_AttachConfiguration_V1_t)(DLL_API_AttachConfiguration_t*);         // Implemented in dll_api_v1.cpp

//-----------------------------------------------------------------------------------------------------------------------------
// API: DLL_API_CreatePlugin_V1
//
// Description: Initiates the creation of a sub-class of the base class CPlugin_DLL_API_V1
//              Calls the factory function DLL_API_Factory(...), which must be implemented by the sub-classed dll plugin.

#define __DLL_API_CreatePlugin_V1__  "DLL_API_CreatePlugin_V1"

typedef CPlugin_DLL_API_V1* (__cdecl *DLL_API_CreatePlugin_V1_t)(void);                 // Implemented in dll_api_v1.cpp

//-----------------------------------------------------------------------------------------------------------------------------
// API: DLL_API_DeletePlugin_V1
//
// Description: Triggers the destructor of the sub-classed dll plugin.

#define __DLL_API_DeletePlugin_V1__  "DLL_API_DeletePlugin_V1"

typedef void (__cdecl *DLL_API_DeletePlugin_V1_t)(CPlugin_DLL_API_V1* plugIn_p);        // Implemented in dll_api_v1.cpp


//-----------------------------------------------------------------------------------------------------------------------------
// Function: DLL_API_Factory
//
// Description: A prototype function called by LogScrutinizer, through DLL API DLL_API_CreatePlugin_V1, which shall create the plugin
//              sub-class. Its a (sort of) factory pattern where this function is called to create the desired instance of a class, unknown to
//              the caller (LogScrutinizer)
//
// IMPORTANT:   Must be implemented in the sub-classed plugin DLL

extern CPlugin_DLL_API_V1*    DLL_API_Factory(void);                                    // IMPLEMENTs THIS in your custom plugin.cpp file

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

#endif  // DLL_API_V1_H