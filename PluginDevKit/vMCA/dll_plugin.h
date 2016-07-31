
#include <map>
#include "plugin_api_v1.h"

class CPlugin_LPP_Ziplog : public CPlugin_DLL_API_V1
{
public:
  CPlugin_LPP_Ziplog();   
};

class CPlot_LPP_Ziplog : public CPlot
{
public:
  CPlot_LPP_Ziplog();
  ~CPlot_LPP_Ziplog();

  virtual void                        pvPlotBegin(void);
  virtual void                        pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex);
  virtual void                        pvPlotEnd(void);
  virtual void                        pvPlotClean(void);

private:

  CTextParser                         m_parser;

  std::map<std::string, int>          m_events_dict;

  unsigned int                        m_lifeLines_number;
  unsigned int                        m_events_number;

#define MAX_NUM_OF_LIFE_LINES   50
  lifeLine_h                          m_lifeLines_a[MAX_NUM_OF_LIFE_LINES];

  CSequenceDiagram*                   m_sequenceDiagram_p;

  unsigned int                        m_subPlotID_Sequence;
};