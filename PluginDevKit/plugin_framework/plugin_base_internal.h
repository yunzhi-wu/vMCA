#ifndef PLUGIN_BASE_INTERNAL_H
#define PLUGIN_BASE_INTERNAL_H

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// File: plugin_base_internal.h
//
// Description:
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "plugin_constants.h"
#include "plugin_utils.h"
#include "plugin_utils_internal.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CLASS:       CPlot_Internal
// Description:
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CPlot_Internal : public CListObject
{
friend class CPlot;

public:
  CPlot_Internal(unsigned int numOfMatchStrings);
  virtual ~CPlot_Internal();

  virtual void pvPlotBegin(void) = 0;
  virtual void pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex) = 0;
  virtual void pvPlotEnd(void) = 0;
  virtual void pvPlotClean(void) = 0;
  virtual bool vPlotExtractTime(const char* row_p, const unsigned int length, double* time_p) = 0;

protected: // Utilities

public:    // INTERNAL, Do not use
  void                                PlotBegin(void);
  void                                PlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex);
  void                                PlotEnd(void);
  void                                PlotClean(void);

  bool                                GetSubPlots(CList_LSZ** subPlots_pp) {*subPlots_pp = &m_subPlots; return (m_subPlots.isEmpty() ? false : true);}
  void                                GetTitle(char** title_pp, char** X_AxisLabel_pp) {*title_pp = m_title, *X_AxisLabel_pp = m_X_AxisLabel;}
  DLL_API_MatchStrings_V1_t*          GetMatchStrings(void) {return &m_matchStrings;}

private:
  CPlot_Internal(){;}

  CList_LSZ                           m_subPlots; // List of CGraph elements
  CSubPlot*                           m_subPlotRefs[MAX_NUM_OF_SUB_PLOTS];         //Direct reference to the subplots (performance)
  char                                m_title[MAX_PLOT_NAME_LENTGH];
  char                                m_X_AxisLabel[MAX_PLOT_STRING_LENTGH];

protected:
  DLL_API_MatchStrings_V1_t           m_matchStrings;
};

#endif