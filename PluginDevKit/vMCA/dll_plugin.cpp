//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------
// File: dll_plugin.cpp
//
// Description: This is Plugin Example 4
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
  return (CPlugin_DLL_API_V1*)new CPlugin_LPP_Ziplog;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlugin_LPP_Ziplog::CPlugin_LPP_Ziplog()
{
  SetPluginName( "LPP Ziplog EvenSeq");
  SetPluginVersion("v1.0");
  SetPluginAuthor("Yunzhi York Wu");

  SetPluginFeatures(SUPPORTED_FEATURE_PLOT);

  RegisterPlot(new CPlot_LPP_Ziplog());
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_LPP_Ziplog::CPlot_LPP_Ziplog()
  :  CPlot(4)
{
  SetTitle("Events Sequence", "Time");

  m_subPlotID_Sequence = RegisterSubPlot("Sequence Diagram", "Unit");

  SetSubPlotProperties(m_subPlotID_Sequence, SUB_PLOT_PROPERTY_SEQUENCE);

  m_lifeLines_number = 0;
  m_events_number = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

CPlot_LPP_Ziplog::~CPlot_LPP_Ziplog()
{
  PlotClean();
  m_lifeLines_number = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_LPP_Ziplog::pvPlotClean(void)
{
  m_sequenceDiagram_p = NULL;     // The CSequenceDiagram class instance is removed by the plugin framework

  memset(m_lifeLines_a, 0, sizeof(lifeLine_h) * MAX_NUM_OF_LIFE_LINES);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_LPP_Ziplog::pvPlotBegin(void)
{
  m_sequenceDiagram_p = AddSequenceDiagram(m_subPlotID_Sequence, "Test", 1000);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_LPP_Ziplog::pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex)
{
  /*
  This is to plot the events in zip log in the format like:
  [ 0][6f13514d,17773539]:LPP_createThread: ULL1PE_COMMON_globalEvHndlThr <ID: 9, bfnAtCreate: 0x6f13514c, parentPid: 0x3f,

  17773539 will be the time.
  ULL1PE_COMMON_globalEvHndlThr will be the name of the life line

  */

#define MAX_EVENT_NAME_LEN (32)
  char event_name[MAX_EVENT_NAME_LEN];

  m_parser.SetText(row_p, *length_p);

  // In this simple version, only lines contains "LPP_createThread" are processed
  unsigned int parseIndex = 0;
  if (!m_parser.Search("LPP_createThread: ", 18))
  {
	  return;
  }
  else
  {
	  parseIndex = m_parser.GetParseIndex();
  }
  m_parser.ResetParser();
  if (!m_parser.Search(",", 1)) return;
  int event_time = 0;
  if (!m_parser.ParseInt(&event_time)) return;
  // TODO: check the range of the event_time
  m_parser.SetParseIndex(parseIndex);
  if (!m_parser.ParseSymbol(MAX_EVENT_NAME_LEN, event_name)) return;

  std::map<std::string, int>::iterator it;
  it = m_events_dict.find(event_name);
  if (it == m_events_dict.end())
  {
	  if (m_lifeLines_number < MAX_NUM_OF_LIFE_LINES - 1)
	  {
		  m_lifeLines_number++;

		  m_events_dict.insert(std::pair<std::string, int>(event_name, m_lifeLines_number));

		  m_lifeLines_a[m_lifeLines_number] = m_sequenceDiagram_p->AddLifeLine((float)m_lifeLines_number - 0.25f,
			  (float)m_lifeLines_number + 0.25f, event_name, (unsigned int)strlen(event_name),
			  RGB(0x55, 20 + 20 * m_lifeLines_number, 20 + 20 * m_lifeLines_number));
	  }
  }
  else
  {
	  int lifeLineDest = it->second;
	  m_sequenceDiagram_p->AddEvent(m_lifeLines_a[lifeLineDest], event_time, rowIndex, NULL, 0, RGB(90, 90, 90), false);
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_LPP_Ziplog::pvPlotEnd(void)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
