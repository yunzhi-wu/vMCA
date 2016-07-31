//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------
// File: dll_plugin.cpp
//
// Description: This is Plugin Example 3
//              It shows how to create a simple plugin to generate a graph (lines and boxes) out of text rows matching e.g. "Time:0 Value:1"

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
  return (CPlugin_DLL_API_V1*)new CPlugin_Example_3;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlugin_Example_3::CPlugin_Example_3()
{
  SetPluginName("Plugin Example 3");
  SetPluginVersion("v1.0");
  SetPluginAuthor("Robert Klang");

  SetPluginFeatures(SUPPORTED_FEATURE_PLOT);

  RegisterPlot(new CPlot_Example_3());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_Example_3::CPlot_Example_3()
  :  CPlot(4)
{
  SetTitle("Plugin Example 3", "Time");

  m_subPlotID_Boxes                 = RegisterSubPlot("Values", "Unit");    
  m_subPlotID_Lines                 = RegisterSubPlot("Values", "Unit");
  m_subPlotID_Lines_OverrideColor   = RegisterSubPlot("Values Override Color", "Unit");
  m_subPlotID_Lines_OverridePattern = RegisterSubPlot("Values Override Pattern", "Unit");

  SetSubPlotProperties(m_subPlotID_Boxes, SUB_PLOT_PROPERTY_SCHEDULE);
  SetSubPlotProperties(m_subPlotID_Lines, SUB_PLOT_PROPERTY_NO_LEGEND_COLOR);   // As in this sub-plot the line color is defined by the plugin and not LogScrutinizer  
  
  // Add the predefined labels. 
  // Note: Having predefined labels save space compared to add them added as text, and typically also improve the speed of creating graphical objects since
  // the label text doesn't need to be copied

  m_subPlot_Boxes_labelIndex_0      = AddLabel(m_subPlotID_Boxes, "Box:1", 5);
  m_subPlot_Lines_labelIndex_0      = AddLabel(m_subPlotID_Lines, "Line:1", 6);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_Example_3::~CPlot_Example_3()
{
  PlotClean();
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_3::pvPlotClean(void)
{
  memset(&m_lines_a[0], 0, sizeof(graphInfo_t) * 2);
  memset(&m_boxes_a[0], 0, sizeof(graphInfo_t) * 2);

  // By setting prevX to -1.0 we know that it is uninitialized
  m_lines_a[0].prevX = -1.0;
  m_lines_a[1].prevX = -1.0;
  m_boxes_a[0].prevX = -1.0;
  m_boxes_a[1].prevX = -1.0;

  Trace("CPlot_Example_3 pvPlotClean\n");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_3::pvPlotBegin(void)
{
  Trace("CPlot_Example_3 pvPlotBegin\n");

  // In CPlot_Example_3 only the graphs where we make override of either color or pattern are added at pvPlotBegin. 
  // The graphs that are created for the arrays m_lines_a and m_boxes_a are added "dynamically" as the text log is parsed. This is mainly just
  // to point at different ways of adding graphs can be done (and not always adding them at pvPlotBegin).

  m_graph_OverrideColor_1_p = AddGraph(m_subPlotID_Lines_OverrideColor, "Override Color 1", 1000);
  m_graph_OverrideColor_1_p->SetGraphColor(RGB(0x55, 0xf0, 0xf0));

  m_graph_OverrideColor_2_p = AddGraph(m_subPlotID_Lines_OverrideColor, "Override Color 2", 1000);
  m_graph_OverrideColor_2_p->SetGraphColor(RGB(0xff, 0x00, 0xf0));

  m_graph_OverridePattern_1_p = AddGraph(m_subPlotID_Lines_OverridePattern, "Override Pattern 1", 1000);
  m_graph_OverridePattern_1_p->SetLinePattern(GLP_DASHDOT);

  m_graph_OverridePattern_2_p = AddGraph(m_subPlotID_Lines_OverridePattern, "Override Pattern 2", 1000);
  m_graph_OverridePattern_2_p->SetLinePattern(GLP_DASH);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_3::pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex)
{
  bool  status  = true;
  int   index   = 0;

  // Example strings in the log
  //
  // "Line:1 Time:0 Value:1",
  // "Box:1 Time1:0 Time2:1",

  // Initialize the parser with the string
  m_parser.SetText(row_p, *length_p);

  // Make sure the parser starts looking at index 0
  m_parser.ResetParser();

  if (m_parser.Search("Box:", 4))
  {
    // Get the box index
    status = m_parser.ParseInt(&index);

    if (m_boxes_a[index].graph_p == NULL)
    {
      char graphName[16];
      sprintf_s(graphName, 16, "box:%d", index);

      // Notice here that the box graph is added when values pops up in the text log for the first time. 

      m_boxes_a[index].graph_p = AddGraph(m_subPlotID_Boxes, graphName, 1000);
    }

    // 1. Search for "Time:"
    if (m_parser.Search("Time1:", 5))
    {
      int   time1;
      int   time2;

      // 2. Parse the INT value that comes after the "Time:" string
      status = m_parser.ParseInt(&time1);

      // 3. Search for the string "Value:"
      if (status && m_parser.Search("Time2:", 5))
      {
        // 4. Parse the INT value that comes after the "Value:" string
        status = m_parser.ParseInt(&time2);

        if (status)
        {
          // Add a box to the graph
          // Note that when drawing boxes it is up to the plugin to align them y-wise. Here we force
          // boxes for index0 to be at     Y:0 - Y:1,
          // and boxes for index1 to be at Y:2 - Y:3

          if (index == 1)
          {
            m_boxes_a[index].graph_p->AddBox((double)time1, (float)0.0, (double)time2, (float)1.0, rowIndex, m_subPlot_Boxes_labelIndex_0, RGB(90, 90, time1 * 10));
          }
          else
          {
            m_boxes_a[index].graph_p->AddBox((double)time1, (float)2.0, (double)time2, (float)3.0, rowIndex);
          }

          return;
        }
      }
    }
  }

  m_parser.ResetParser();

  if (m_parser.Search("Line:", 5))
  {
    // Get the line index, in this example it is either 0 or 1

    status = m_parser.ParseInt(&index);

    if (m_lines_a[index].graph_p == NULL)
    {
      char graphName[16];
      sprintf_s(graphName, 16, "line:%d", index);

      // Notice here that the line graph is added when values pops up in the text log for the first time. 

      m_lines_a[index].graph_p = AddGraph(m_subPlotID_Lines, graphName, 1000);
    }

    // 1. Search for "Time:"
    if (m_parser.Search("Time:", 5))
    {
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
          if (m_lines_a[index].prevX == -1)
          {
            m_lines_a[index].prevY   = (float)value;
            m_lines_a[index].prevX   = (double)time;
          }

          // Add the "same" line to all of the three graphs, however these lines will look differently.
          // Lines added to:
          
          // Shown in 2nd sub-plot
          //  -  m_lines_a,                     will be color and pattern enumerated by logScrutinizer,
          //       m_lines_a[0]                  - uses dynamically added label string
          //       m_lines_a[1]                  - uses predefined label (index based)

          // Shown in 3rd sub-plot
          //  -  m_graph_OverrideColor_1_p,     will have a preset color (but pattern enumerated by logScrutinizer)
          //  -  m_graph_OverrideColor_2_p,     -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  

          // Shown in 4th sub-plot
          //  -  m_graph_OverridePattern_1_p,   will have a preset pattern (but color enumerated by logScrutinizer)
          //  -  m_graph_OverridePattern_2_p,   -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  -"-  

          if (index == 1)
          {
            // Add a line to the graph (x1,Y1) -> (x2,y2)

            char temp[128];
            sprintf_s(temp, 128, "Index:1 (%d)", time);

            m_lines_a[index].graph_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex, temp, (unsigned int)strlen(temp), RGB(150, 255 - time * 20, time * 20), (float)0.1 * (float)time);

            m_graph_OverrideColor_1_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex);
            m_graph_OverridePattern_1_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex);
          }
          else
          {
            m_lines_a[index].graph_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex, m_subPlot_Lines_labelIndex_0, -1, (float)0.5);

            m_graph_OverrideColor_2_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex);
            m_graph_OverridePattern_2_p->AddLine((double)m_lines_a[index].prevX, (float)m_lines_a[index].prevY, (double)time, (float)value, rowIndex);
          }

          m_lines_a[index].prevY  = (float)value;
          m_lines_a[index].prevX  = (double)time;
        }

        return;
      }
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_Example_3::pvPlotEnd(void)
{
  Trace("CPlot_Example_3 pvPlotEnd\n");
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
