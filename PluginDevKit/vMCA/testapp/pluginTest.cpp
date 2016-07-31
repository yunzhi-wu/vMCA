// pluginTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include "plugin_api_v1.h"
#include "plugin_base.h"

#define ROW_MAX_SIZE    1028

int _tmain(int argc, _TCHAR* argv[])
{
  HMODULE                         hModule;
  DLL_API_GetPluginAPIVersion_t   DLL_API_GetPluginAPIVersion;
  DLL_API_PluginVersion_t         DLL_API_Version;
  FILE*                           dataFile_h;

  // DLL_API V1

  TCHAR  path[1028];

  GetCurrentDirectory(1028, path);

  errno_t  status = fopen_s(&dataFile_h, "test_data.txt", "r");

  if (errno != 0)
  {
    return 0;
  }

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
      plugIn_p = DLL_API_CreatePlugin_V1();

      //--------------------  TEST PLOTS ----------------------------

      CList_LSZ*  plotList_p;
      CPlot*      plot_p;

      if (plugIn_p->GetPlots(&plotList_p))
      {
        if (!plotList_p->isEmpty())
        {
          char tempString[2048];
          unsigned length;

          plot_p = (CPlot*)plotList_p->GetHead();

          while (plot_p != NULL)
          {
            plot_p->pvPlotClean();
            plot_p->PlotBegin();


            int testLoop = 0;
            while(fgets(tempString, 2048, dataFile_h))
            {
              length = (unsigned int)strlen(tempString);
              plot_p->PlotRow(tempString, &length, testLoop);
              ++testLoop;
            }

            plot_p->PlotEnd();

            CList_LSZ*  subPlotList_p;
            CList_LSZ*  graphList_p;

            char* plotTitle_p;
            char* X_AxisLabel_p;

            plot_p->GetTitle(&plotTitle_p, &X_AxisLabel_p);

            printf("\n\nPlot:%s X-Label:%s\n", plotTitle_p, X_AxisLabel_p);

            plot_p->GetSubPlots(&subPlotList_p);

            if (!subPlotList_p->isEmpty())
            {
              char*                       subPlotTitle_p;
              char*                       Y_AxisLabel_p;
              GraphicalObject_Extents_t   extents;

              CSubPlot* subPlot_p = (CSubPlot*)subPlotList_p->GetHead();

              while (subPlot_p != NULL)
              {
                subPlot_p->GetTitle(&subPlotTitle_p, &Y_AxisLabel_p);
                subPlot_p->GetExtents(&extents);

                unsigned int properties = subPlot_p->GetProperties();

                printf("  SubPlot:%s Y-Label:%s  X_min:%-2.1f X_max:%-2.1f Y_min:%-2.1f Y_max:%-2.1f\n",
                  subPlotTitle_p,
                  Y_AxisLabel_p,
                  extents.x_min,
                  extents.x_max,
                  extents.y_min,
                  extents.y_max);

                subPlot_p->GetGraphs(&graphList_p);

                if (!graphList_p->isEmpty())
                {
                  CGraph* graph_p = (CGraph*)graphList_p->GetHead();

                  while (graph_p != NULL)
                  {
                    graph_p->GetExtents(&extents);

                    printf("    Graph:%s X_min:%-2.1f X_max:%-2.1f Y_min:%-2.1f Y_max:%-2.1f\n",
                      graph_p->GetName(),
                      extents.x_min,
                      extents.x_max,
                      extents.y_min,
                      extents.y_max);

                    GraphicalObject_t* go_p = graph_p->GetFirstGraphicalObject();

                    while (go_p != NULL)
                    {
                      printf("      Graphical Object    x1:%-2.1f y1:%-2.1f  ->  x2:%-2.1f y2:%-2.1f\n",
                        go_p->x1,
                        go_p->y1,
                        go_p->x2,
                        go_p->y2);

                      go_p = graph_p->GetNextGraphicalObject();
                    }

                    graph_p = (CGraph*)graphList_p->GetNext((CListObject*)graph_p);

                  }// while graph_p
                }// if graphList_p->isEmpty()


                if (properties & SUB_PLOT_PROPERTY_SEQUENCE)
                {
                  CDecorator* decorator_p;

                  subPlot_p->GetDecorator(&decorator_p);

                  if (decorator_p->GetNumOfObjects() > 0)
                  {
                    GraphicalObject_t* go_p = decorator_p->GetFirstGraphicalObject();

                    while (go_p != NULL)
                    {
                      printf("      Decorator    x1:%-2.1f y1:%-2.1f  ->  x2:%-2.1f y2:%-2.1f\n",
                        go_p->x1,
                        go_p->y1,
                        go_p->x2,
                        go_p->y2);

                      go_p = decorator_p->GetNextGraphicalObject();
                    }
                  }
                }

                subPlot_p = (CSubPlot*)subPlotList_p->GetNext((CListObject*)subPlot_p);

              }// while (subPlot_p != NULL)

            }

            plot_p = (CPlot*)plotList_p->GetNext(plot_p);
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

  fclose(dataFile_h);

  return 0;
}
