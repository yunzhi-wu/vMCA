//-----------------------------------------------------------------------------------------------------------------------------
// File:        plugin_base.cpp
//
// Description: Implementation file for plugin base classes (to inherit from)
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"

#include "plugin_api_v1.h"
#include "plugin_base.h"
#include "plugin_utils.h"
#include "plugin_text_parser.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS: CPlot
// Description: See plugin_base.h file
//-----------------------------------------------------------------------------------------------------------------------------

bool CPlot::AddMatchString(char* string_p, unsigned int length)
{
  if (m_matchStrings.numOfStrings < m_matchStrings.maxNumOfStrings)
  {
    memcpy(m_matchStrings.m_matchStringArray[m_matchStrings.numOfStrings].m_matchString, string_p, length);
    m_matchStrings.m_matchStringArray[m_matchStrings.numOfStrings].m_length  = length;

    ++m_matchStrings.numOfStrings;
  }
  else
  {
    ErrorHook("Too many match strings added to plugin");
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------------------------------------------------------

unsigned int CPlot::RegisterSubPlot(char* title_p, char* Y_AxisLabel)
{
  unsigned int numOfSubPlots = m_subPlots.GetCount();

  CSubPlot* subPlot_p = (CSubPlot*) new CSubPlot(title_p, numOfSubPlots, Y_AxisLabel);

  if (subPlot_p != NULL)
  {
    m_subPlots.InsertTail((CListObject*)subPlot_p);
    m_subPlotRefs[numOfSubPlots] = subPlot_p;
    return numOfSubPlots;
  }

  return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------

bool CPlot::SetSubPlotProperties(unsigned int subPlotID, SubPlot_Properties_t properties)
{
  if (m_subPlots.isEmpty())
  {
    return false;
  }

  CSubPlot* subPlot_p = (CSubPlot*)m_subPlots.GetHead();
  while (subPlot_p != NULL)
  {
    if (subPlot_p->GetID() == subPlotID)
    {
      subPlot_p->SetProperties(properties);
      return true;
    }

    subPlot_p = (CSubPlot*)m_subPlots.GetNext((CListObject*)subPlot_p);
  }

  return false;
}

//-----------------------------------------------------------------------------------------------------------------------------

CGraph* CPlot::AddGraph(unsigned int subPlotID, char* name_p, unsigned int estimatedNumOfObjects)
{
  return (m_subPlotRefs[subPlotID]->AddGraph(name_p, estimatedNumOfObjects));
}

//-----------------------------------------------------------------------------------------------------------------------------

unsigned int CPlot::AddLabel(unsigned int subPlotID, const char* label_p, const unsigned int labelLength)
{
  if (labelLength > (MAX_LABEL_LENGTH - 1))
  {
    return -1;
  }

  return (m_subPlotRefs[subPlotID]->AddLabel(label_p, labelLength));
}


//-----------------------------------------------------------------------------------------------------------------------------

CDecorator* CPlot::AddDecorator(unsigned int subPlotID)
{
  return (m_subPlotRefs[subPlotID]->AddDecorator());
}

//-----------------------------------------------------------------------------------------------------------------------------

CSequenceDiagram* CPlot::AddSequenceDiagram(unsigned int subPlotID, char* name_p, unsigned int estimatedNumOfObjects)
{
  return (m_subPlotRefs[subPlotID]->AddSequenceDiagram(name_p, estimatedNumOfObjects));
}

//-----------------------------------------------------------------------------------------------------------------------------