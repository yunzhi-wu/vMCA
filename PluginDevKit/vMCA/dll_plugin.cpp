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

#define KNOWN_EVENTS_NUM 57
static std::string known_events[KNOWN_EVENTS_NUM] =
{
	"XioWrite",
	"UpMeFoFindThread",
	"ULL1PE_PUCCH_COORD_handleGraphDoneThr",
	"ULL1PE_PUCCH_COORD_handleCellBufferReadyIndThr",
	"ULL1PE_PUCCH_COORD_handleAllocationIndThr",
	"PucchPrbSlot1",
	"PucchPrbSlot0",
	"PucchPrbSframe",
	"PucchFoPrep",
	"PucchAllocJobPrbProcsBuffers",
	"ULL1PE_COMMON_globalEvHndlThr",
	"ULL1PE_CELL_COORD_handleSubframeDoneIndThr",
	"HarqDecoding",
	"ConvDecoderCQI",
	"Deint",
	"SignalMeas",
	"RiDecoding",
	"PuschConstructor",
	"NoiseMeas2",
	"NoiseMeas1",
	"meas2DlDone",
	"FODPrepS",
	"DecPreProcSlave",
	"SoftDemap_1",
	"SoftDemap_0",
	"AntCombFde_1",
	"AntCombFde_0",
	"ChanEst5E_1",
	"ChanEst5E_0",
	"ChanEst4E_1",
	"ChanEst4E_0",
	"PostCe3_1",
	"PostCe3_0",
	"ChanEst3_1_1",
	"ChanEst3_1_0",
	"ChanEst3_0_1",
	"ChanEst3_0_0",
	"ChanEst2SectMeas_1",
	"ChanEst2SectMeas_0",
	"ChanEst2_1_1",
	"ChanEst2_1_0",
	"ChanEst2_0_1",
	"ChanEst2_0_0",
	"ChanEst1SectMeas_1",
	"ChanEst1SectMeas_0",
	"TimeAdv",
	"ChanEst1_1_1",
	"ChanEst1_1_0",
	"ChanEst1_0_1",
	"ChanEst1_0_0",
	"RSGenSlot_1",
	"RSGenSlot_0",
	"bbiTrace_hitTable_thread",
	"bbiMcsUpdaterThread",
	"bbiMcsBufferHandlerThread",
	"bbiCommStackMsg_rxThread",
	"bbiCommStackLinx_rxThread",
};

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

#define MAX_EVENT_NAME_LEN (64)
  char event_name[MAX_EVENT_NAME_LEN];

  m_parser.SetText(row_p, *length_p);

  // In this simple version, only lines contains "LPP_createThread" are processed
  unsigned int parseIndex = 0;
  if (!m_parser.Search("LPP_startThread: ", 17))
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
  if (event_time < 14000000)
  {
	  return;
  }
  else
  {
	  event_time -= 14000000;
  }
  double event_time_d = event_time;
  m_parser.SetParseIndex(parseIndex);
  if (!m_parser.ParseSymbol(MAX_EVENT_NAME_LEN, event_name)) return;
  char *output = NULL;
  
  std::map<std::string, int>::iterator it;
  it = m_events_dict.find(event_name);
  if (it == m_events_dict.end())
  {
	  if (m_lifeLines_number < MAX_NUM_OF_LIFE_LINES - 1)
	  {
		  m_lifeLines_number++;
		  int lifeLineIndex = 0;
		  for (; lifeLineIndex < KNOWN_EVENTS_NUM; lifeLineIndex++)
		  {
			  if (!strcmp(event_name, known_events[lifeLineIndex].c_str()))
			  {
				  break;
			  }
		  }
		  if (lifeLineIndex == KNOWN_EVENTS_NUM)
		  {
			  // unknown event
			  lifeLineIndex += m_events_number;
			  m_events_number++;
		  }
		  lifeLineIndex += 1;
		  m_events_dict.insert(std::pair<std::string, int>(event_name, lifeLineIndex));

		  m_lifeLines_a[lifeLineIndex] = m_sequenceDiagram_p->AddLifeLine((float)lifeLineIndex - 0.25f,
			  (float)lifeLineIndex + 0.25f, event_name, (unsigned int)strlen(event_name),
			  RGB(0x55, 20 + 20 * lifeLineIndex, 20 + 20 * lifeLineIndex));

//		  m_sequenceDiagram_p->AddEvent(m_lifeLines_a[m_lifeLines_number], m_events_number, rowIndex, NULL, 0, RGB(90, 90, 90), false);
		  m_sequenceDiagram_p->AddEvent(m_lifeLines_a[lifeLineIndex], event_time_d, rowIndex, NULL, 0, RGB(90, 90, 90), false);
	  }
  }
  else
  {
	  int lifeLineIndex = it->second;
//	  m_sequenceDiagram_p->AddEvent(m_lifeLines_a[lifeLineDest], m_events_number, rowIndex, NULL, 0, RGB(90, 90, 90), false);
	  m_sequenceDiagram_p->AddEvent(m_lifeLines_a[lifeLineIndex], event_time_d, rowIndex, NULL, 0, RGB(90, 90, 90), false);
  }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

void CPlot_LPP_Ziplog::pvPlotEnd(void)
{

}

//-------------------------------------------------------------------------------------------------------------------------------------------------
