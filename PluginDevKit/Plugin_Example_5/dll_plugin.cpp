
//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------
// File: dll_plugin.cpp
//
// Description: This is Plugin Example 5
//              It shows how to create a simple plugin to generate a graph out of text rows matching e.g. "Time:0 Value:1"

//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <stdio.h>
#include "plugin_api_v1.h"
#include "plugin_utils.h"
#include "plugin_text_parser.h"
#include "dll_plugin.h"


CPlugin_DLL_API_V1* DLL_API_Factory(void)
{
  return (CPlugin_DLL_API_V1*)new CPlugin_Example_5;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlugin_Example_5::CPlugin_Example_5()
{
  SetPluginName( "Plugin Example 5");
  SetPluginVersion("v1.0");
  SetPluginAuthor("Robert Klang");

  SetPluginFeatures(SUPPORTED_FEATURE_PLOT);

  RegisterPlot(new CPlot_Example_5());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_Example_5::CPlot_Example_5()
  :  CPlot(4)
{
  SetTitle("Plugin Example 5", "Time");

  m_subPlotID = RegisterSubPlot("Values", "Unit");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_Example_5::~CPlot_Example_5()
{
  PlotClean();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_5::pvPlotClean(void)
{
  m_prevValue   = -1;
  m_prevTime    = -1;

  m_graphAdded  = false;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_5::pvPlotBegin(void)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_5::pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex)
{
  if (!m_graphAdded)
  {
    m_valueGraph_p = AddGraph(m_subPlotID, "Value graph", 1000);
    m_graphAdded = true;
  }

  // The "data" string in the Example_5 text log looks like this:  Time:0 Value:1

  // 1. Search for a string starting with text Time:
  // 2. Parse the integer value
  // 3. Search for Value:
  // 4. Parse the integer value
  
  // Initialize the parser with the string
  m_parser.SetText(row_p, *length_p);

  // Make sure the parser starts looking at index 0
  m_parser.ResetParser();
  
  // 1. Search for "Time:"
  if (m_parser.Search("Time:", 5))
  {
    bool  status;
    int   time;
    int   value;

    // 2. Parse the INT value that comes after the "Time:" string
    status = m_parser.ParseInt(&time);

    // 3. Search for the string "Value:"
    if (status && m_parser.Search("Value:", 6))
    {
      // 4. Parse the INT value that comes after the "Value:" string
      status = m_parser.ParseInt(&value);

      if (status)
      {
        // If this was the first value we need to initialize the first point of the graph
        if (m_prevValue == -1 && m_prevTime == -1)
        {
          m_prevValue = value;
          m_prevTime  = time;
        }

        // Add a line to the graph (x1,Y1) -> (x2,y2)
        m_valueGraph_p->AddLine((double)m_prevTime, (float)m_prevValue, (double)time, (float)value, rowIndex);

        m_prevValue = value;
        m_prevTime  = time;
      }
    }
  }  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool CPlot_Example_5::vPlotGraphicalObjectFeedback(
    const char*         row_p, 
    const unsigned int  length, 
    const double        time, 
    const unsigned int  rowIndex,
    const CGraph*       graphRef_p,
    char*               feedbackText_p, 
    const unsigned int  maxFeedbackTextLength)
{
  int go_time;

  m_parser.SetText(row_p, length);

  // Make sure the parser starts looking at index 0
  m_parser.ResetParser();
  
  // 1. Search for "Time:"
  if (!m_parser.Search("Time:", 5)) return false;

  if (!m_parser.ParseInt(&go_time)) return false;

  sprintf_s(feedbackText_p, maxFeedbackTextLength, "The go time is: %d, input time is:%f\nGreat stuff", go_time, time);

  return true;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_5::pvPlotEnd(void)
{
  
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
