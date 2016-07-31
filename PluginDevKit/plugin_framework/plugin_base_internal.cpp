//-----------------------------------------------------------------------------------------------------------------------------
// File:        plugin_base_internal.cpp
//
// Description:
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"

#include "plugin_api_v1.h"
#include "plugin_utils.h"
#include "plugin_utils_internal.h"
#include "plugin_text_parser.h"
#include <stdlib.h>

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS: CPlot_Internal
// Description: See plugin_base.h file
//-----------------------------------------------------------------------------------------------------------------------------

CPlot_Internal::CPlot_Internal(unsigned int numOfMatchStrings)
{
  m_matchStrings.maxNumOfStrings      = numOfMatchStrings;
  m_matchStrings.numOfStrings         = 0;

  m_title[0]                          = 0;
  m_X_AxisLabel[0]                    = 0;

  memset(m_subPlotRefs, 0, sizeof(CSubPlot*) * MAX_NUM_OF_SUB_PLOTS);

  m_matchStrings.m_matchStringArray   = (DLL_API_MatchString_V1_t*)malloc(sizeof(DLL_API_MatchString_V1_t) * numOfMatchStrings);
  memset(m_matchStrings.m_matchStringArray, 0, sizeof(DLL_API_MatchString_V1_t) * numOfMatchStrings);
}

//-----------------------------------------------------------------------------------------------------------------------------

CPlot_Internal::~CPlot_Internal()
{
  m_subPlots.DeleteAll();
}

//-----------------------------------------------------------------------------------------------------------------------------

void CPlot_Internal::PlotBegin(void) 
{
  pvPlotBegin();
}

//-----------------------------------------------------------------------------------------------------------------------------

void CPlot_Internal::PlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex) 
{
  pvPlotRow(row_p, length_p, rowIndex);
}

//-----------------------------------------------------------------------------------------------------------------------------

void CPlot_Internal::PlotClean(void)
{
  pvPlotClean();

  // Keep the registered sub-plots however clean/reset their contents
  CSubPlot* subPlot_p = (CSubPlot*)m_subPlots.GetHead();
  while (subPlot_p != NULL)
  {
    subPlot_p->Clean();
    subPlot_p = (CSubPlot*)m_subPlots.GetNext((CListObject*)subPlot_p);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

void CPlot_Internal::PlotEnd(void)
{
  CSubPlot* subPlot_p = (CSubPlot*)m_subPlots.GetHead();

  while (subPlot_p != NULL)
  {
    subPlot_p->CalcExtents();

    unsigned int properties = subPlot_p->GetProperties();

    if (properties & SUB_PLOT_PROPERTY_SEQUENCE)
    {
      bool    lifeLineExists  = false;
      float   y_values        = 0.0f;

      // Go through the decorator lifelines and setup the X coordinates

      CDecorator* decorator_p = NULL;

      subPlot_p->GetDecorator(&decorator_p);

      if (decorator_p->GetNumOfObjects() > 0)
      {
        GraphicalObject_Extents_t extents;

        subPlot_p->GetExtents(&extents);

        // Debugging
        if (extents.x_max == 0.0 && extents.x_min == 0.0)
        {
          extents.x_max = 1.0;
        }

        double x_width = extents.x_max - extents.x_min;
        double x_lifeline_width = x_width * 0.1;  // 10% of the total width is devoted to the lifeline objects

        extents.x_min -= x_lifeline_width;

        GraphicalObject_t* go_p = decorator_p->GetFirstGraphicalObject();

        while (go_p != NULL)
        {
          if (go_p->properties & GRAPHICAL_OBJECT_KIND_DECORATOR_LIFELINE)
          {
            lifeLineExists = true;

            go_p->x1  = extents.x_min;
            go_p->x2  = extents.x_min + x_lifeline_width;

            go_p = decorator_p->GetNextGraphicalObject();
          }
        }

        if (lifeLineExists)
        {
          // Update the extents such that the life lines will fit
          subPlot_p->SetExtents(&extents);
        }

      }

    }

    subPlot_p = (CSubPlot*)m_subPlots.GetNext((CListObject*)subPlot_p);

  }// while subPlots

  pvPlotEnd();
}