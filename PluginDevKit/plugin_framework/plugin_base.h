#ifndef PLUGIN_BASE_H
#define PLUGIN_BASE_H

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// File: plugin_base.h
//
// Description:
// This header file contains the base classes that shall be used to create and support decoders and plots
//
// Inherit from these base classes and implement the "pure virtual functions" which are mandatory to implement. The plugin framework will call
// the subclassed pure virtual functions to let the plugin decode text rows and create graphical objects.
//
// The base classes contains member functions that shall be used to add e.g. graphical objects such as lines
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "plugin_constants.h"
#include "plugin_utils.h"
#include "plugin_base_internal.h"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_LABEL_LENGTH 256      // Max string length of a label (including the 0/EOL)

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct
{
  char  name[PLUGIN_INFO_V1_NAME_MAX_SIZE];
  char  version[PLUGIN_INFO_V1_VERSION_MAX_SIZE];
  char  author[PLUGIN_INFO_V1_AUTHOR_MAX_SIZE];
  char  helpURL[PLUGIN_INFO_V1_HELP_URL_MAX_SIZE];

  unsigned int supportedFeatures;

} DLL_API_PluginInfo_V1_t;


typedef struct
{
  int   version;

} DLL_API_PluginVersion_t;

typedef struct
{
  HWND    hwnd_traceConsumer;
  HANDLE  h_traceHeap;

} DLL_API_AttachConfiguration_t;

typedef unsigned int SubPlot_Properties_t;      // Use SUB_PLOT_PROPERTY_XXX, combine multiple properies with OR

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CLASS:       CDecoder
// Description: Base class for decoders.
//
//              Inherit from this class and implement the mandatory pvDecode(...) function.
//
//              The main purpose of a decoder is to exachange text that might not be easilly understood. E.g. a coded string can be uncoded and displayed
//              in a readable format. The string provided by the LogScrutinizer may be entirely exchanged, or just made longer with the decoded values
//              at the end. Note however that the string may not be larger than MAX_STRING_LENGTH (512 bytes)
//
//              The function pvDecode will be called by the LogScrutinizer for each row that are about to be presented on screen. Note that LogScrutinizer doesn't
//              change the log file with the returned string, it is only what is shown on screen. Further, filters and searches will not match against text that
//              has been provided by a decoder, only the original text in the log file.
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDecoder : public CListObject
{
public:
  CDecoder(char* matchString, unsigned int uniqueID)
  {
    strcpy_s(m_matchString, MAX_STRING_LENGTH, matchString);

    m_length    = (unsigned int)strlen(m_matchString);
    m_uniqueID  = uniqueID;
  }

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    pvDecode
  // Description: Decode a single line. The implementation of this function shall return true if the input/output variable row_p was modified.
  //              Modify the text string directly, max total string length is MAX_STRING_LENGTH  (512 bytes)
  // Parameters
  // Input/Output: row_p          A text row from the log. The content of the text row may be modfied, shrinked, and/or increased up to 64kB (maxLength)
  // Input/Output: length_p       The current length of the text row (exluding EOL), and shall be updated to reflect the changes done to row_p
  // Input:        maxLength      If the decoder modifies the string it shall not become larger than this value (inlcuding the terminating EOL)

  virtual bool pvDecode(char* row_p, unsigned int* length_p, const unsigned int maxLength) = 0;   // PURE VIRTUAL, must be implmented


  // INTERNAL, Do not use
  bool                          Decode(char* row_p, unsigned int* length_p, const unsigned int maxLength) {return pvDecode(row_p, length_p, maxLength);}
  inline unsigned int           GetUniqueID(void) {return m_uniqueID;}
  inline char*                  GetMatchString(unsigned int* length_p) {*length_p = m_length; return m_matchString;}

private:
  CDecoder();

protected:
  char                          m_matchString[MAX_STRING_LENGTH];
  unsigned int                  m_length;
  unsigned int                  m_uniqueID;
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CLASS:       CPlot
// Description: This base class is used to create plots, derive from it.
//
//              A plot is shown in its own tab in the LogScrutinizer. A plot contains 1 or more sub-plots. Each subplot is drawn in its on "sub-window" within the plot tab.
//              To each sub-plot there can be 0 or more graphs added
//
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CPlot : public CPlot_Internal
{
public:
  CPlot(unsigned int numOfMatchStrings)
    :CPlot_Internal(numOfMatchStrings)
  {;}

  virtual ~CPlot(){;}

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: pvPlotBegin
  // Description: Called when a new plot sequence is started. Typically the plugin reset its internal state and prepare for pvPlotRow calls

  virtual void pvPlotBegin(void) = 0;   // PURE VIRTUAL, must be implmented


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: pvPlotRow
  // Description:     This function is called by the LogScrutinizer once for each row in the log. The plugin shall search the text string for
  //                  matches and if suiteble generate graphical objects, and new graphs, and store these in the
  // Parameters
  //    Input:    row_p       A text row from the log.
  //    Input:    length_p    The length of the text row (exluding EOL, which doesn't exist)
  //    Input:    rowIndex    Any graphical object added to a graph needs to be tagged to a row. This is the row index in the log for the text string row_p

  virtual void pvPlotRow(const char* row_p, const unsigned int* length_p, unsigned int rowIndex) = 0;   // PURE VIRTUAL, must be implmented


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: pvPlotEnd
  // Description: Called when a new plot sequence is completed. This is the last call made during the log processing, hence the plugin shall finalize
  //              and add any remanining graphs or graphical objects to the sub-plots

  virtual void pvPlotEnd(void) = 0;   // PURE VIRTUAL, must be implmented


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: pvClean
  // Description: Called when graph memory shall be cleared, typically before a new run or loading a new log text file
  //    Remove all created memory and "reset" the plugin (all sub-plots and graph contents)
  //    Added CGraphs and lines are cleared by the CPlot base class, as such only objects unique (defined) for the sub-class needs to be removed
  //    After this function has been called all previously added graphs are deleted by the CPlot

  virtual void pvPlotClean(void) = 0;   // PURE VIRTUAL, must be implmented


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: vPlotExtractTime
  // Description:     This function is called by the LogScrutinizer when the user is hovering over a point in the graphs. Through the callback the plugin (plot) has the possibility
  //                  to provide additional information.
  //                  You have to set the SUPPORTED_FEATURE_PLOT_GRAPHICAL_OBJECT_FEEDBACK, otherwise LogScrutinizer will not ask the plugin for additional information
  // Parameters
  //    Input:        row_p       The text from the row referenced when adding the graphical object
  //    Input:        length      The length of the text row (exluding EOL, which doesn't exist)
  //    Input:        time        The time endpoint referenced when adding the graphical object
  //    Input:        rowIndex                The index of the row in the log referenced when adding the graphical object
  //    Input:        graphRef_p              Reference to the CGraph where the graphical object belongs.
  //    Input/Output: feedbackText_p          Use this text string reference to add your feedback/information about the graphical object.
  //                                            - Multiple rows are supported, seperate them by inserting 0x0a + 0x0d (/n) at the end of each line
  //                                            - Always zero terminate the string
  //    Input:        maxFeedbackTextLength   The maximum number of characters you may add to the feedback string, including the row seperators and zero termination
  //
  // Return value:    True if feedback was added to the feedbackText_p string

  virtual bool vPlotGraphicalObjectFeedback(      // Override this virtual function if you want to return additional feedback for graphical objects
    const char*         row_p, 
    const unsigned int  length, 
    const double        time, 
    const unsigned int  rowIndex,
    const CGraph*       graphRef_p,
    char*               feedbackText_p, 
    const unsigned int  maxFeedbackTextLength) 
  {
    feedbackText_p[0] = 0;        // If the derived CPlot doesn't implement an override function this will return an empty string 
    return false;                 // Return false to indicate nothing was added
  }


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: vPlotExtractTime
  // Description:     This function is called by the LogScrutinizer to extract time from a certain row in the log. Typically used when setting plot cursor, to know where in time
  //                  to set the cursor in the plot. Implement this virtual function in your plugin to help logscrutinizer get time from a log row
  //                  You have to set the SUPPORTED_FEATURE_PLOT_TIME, otherwise LogScrutinizer will not ask your plugin to extract time
  // Parameters
  //    Input:        row_p       A text row from the log.
  //    Input:        length_p    The length of the text row (exluding EOL, which doesn't exist)
  //    Input/Output: time_p      Use time_p to return the time. If time was successfully extracted from the text row then return true, otherwise false
  //
  // Return value:    True if time was successfully be extracted from row_p

  virtual bool vPlotExtractTime(const char* row_p, const unsigned int length, double* time_p) {*time_p = 0.0; return false;}


protected: // Utilities

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: SetTitle
  // Description: Define the title of the entire plot, and the name of the X axis for all sub-plots within this plot.
  //

  void SetTitle(char* title_p, char* X_AxisLabel_p) {strcpy_s(m_title, MAX_PLOT_STRING_LENTGH, title_p); strcpy_s(m_X_AxisLabel, MAX_PLOT_STRING_LENTGH, X_AxisLabel_p);}


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: AddMatchString
  // Description: A match string is used by LogScrutinizer to improve the performace by doing the inital scan for this string. Add all strings that are
  //              used to extract graphical objects from a log.
  //              If you do not set SUPPORTED_FEATURE_MATCH_STRINGS then there is no use of match strings.
  //
  //              To enable the performance boost you have to set SUPPORTED_FEATURE_MATCH_STRINGS
  //              WARNING: With SUPPORTED_FEATURE_MATCH_STRINGS set, then LogScrutinizer will only call pvPlotRow if at least one of the added match strings have a match
  //              with the text row

  bool AddMatchString(char* string_p, unsigned int length);


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: RegisterSubPlot
  // Description: Call this function to add a subplot to the graph. This shall be done when the DLL is loaded initially
  //              title_p and Y_AxisLabel will be shown in the sub-graph. (X_AxisLabel is defined when the title of the CPlot is set.)
  // Parameters:
  //    Input: title_p
  //    Input: Y_AxisLabel
  //    Output: Sub-plot number, This shall be used when adding graphs

  unsigned int RegisterSubPlot(char* title_p, char* Y_AxisLabel);  // returns the ID of the subPlot (subPlotID)


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: RegisterSubPlot
  // Description: Call this function to add a subplot to the graph. This shall be done when the DLL is loaded initially
  //              title_p and Y_AxisLabel will be shown in the sub-graph. (X_AxisLabel is defined when the title of the CPlot is set.)
  // Parameters:
  //    Input: subPlotID,   the ID returned at the API call RegisterSubPlot
  //    Input: properties,  SUB_PLOT_PROPERTY_XXX, e.g: SUB_PLOT_PROPERTY_SCHEDULE | SUB_PLOT_PROPERTY_NO_LEGEND_COLOR
  //    Output: true if successful

  bool SetSubPlotProperties(unsigned int subPlotID, SubPlot_Properties_t properties);


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: AddGraph
  // Description: Use AddGraph to add additional graphs to the subplot. It is important to specify the corresponding subPlot where this graph is added.
  // Parameters:
  //    Input: subPlotID, The subplot number that was provided when a new sub-plot was registered
  //    Input: name_p, The name of the graph will show up in the legend in LogScrutinizer
  //    Input: estimatedNumOfObjects, this gives the plugin framework some guidance when creating the containers to store graphical objects. In-case you don't know just
  //           use the default value.
  //    Output: CGraph*, This function returns a newly created CGraph object. The CGraph object shall be used to add graphical object.

  // IMPORTANT: All added graphs will be deleted automatically by the plugin framework when the plot is requested to be run from user in LogScrutinizer. This means
  //            that a graph needs to be added (again) after pvPlotClean has been called.

  CGraph* AddGraph(unsigned int subPlotID, char* name_p, unsigned int estimatedNumOfObjects = 1024);


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: AddSequenceDiagram
  // Description: Use AddSequenceDiagram to add a sequence diagram to a sub-plot. There shall only be added one per sub-plot.
  //              To the subPlot you should also add a decorator, start with adding life lines to the decorator and then you draw messages between the lifeLines
  //              using this sequenceDiagram object
  // Parameters:
  //    Input: subPlotID, The subplot number that was provided when a new sub-plot was registered
  //    Input: name_p, The name of the sequenceDiagram will show up in the legend in LogScrutinizer
  //    Input: estimatedNumOfObjects, this gives the plugin framework some guidance when creating the containers to store graphical objects. In-case you don't know just
  //           use the default value. An object is typically a message or an execution
  //    Output: CSequenceDiagram*, This function returns a newly created CSequenceDiagram object. The CSequenceDiagram object shall be used to add messages etc.

  // IMPORTANT: All added sequenceDiagrams will be deleted automatically by the plugin framework when the plot is requested to be (re)run from user in
  //            LogScrutinizer. This means that a sequenceDiagram needs to be added (again calling this function) after pvPlotClean has been called.

  CSequenceDiagram* AddSequenceDiagram(unsigned int subPlotID, char* name_p, unsigned int estimatedNumOfObjects = 1024);


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: AddDecorator
  // Description: Use AddDecorator to be able to add decorational objects such as fixed lines, boxes and labels.
  //              The CDecorator is very similar to the CGraph, although you can add different types of graphical objects to a subplot.
  //              CDecorators are intermixed with CGraphs to create e.g. sequence diagrams and timing diagrams
  // Parameters:
  //    Input: subPlotID, The subplot number that was provided when a new sub-plot was registered
  //    Output: CDecorator*, This function returns a new CDecorator object. The CDecorator object shall be used to add graphical objects.

  // IMPORTANT: All added decorators will be deleted automatically by the plugin framework when the plot is requested to be run from user in LogScrutinizer. This means
  //            that a graph needs to be added (again) after pvPlotClean has been called.

  // NOTE:      Currently there are no available member functions to use (Under construction)

  CDecorator* AddDecorator(unsigned int subPlotID);


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    AddLabel
  // Description: This function add a label to a sub-plot. The returned value can be used to provide a box or line with a label.
  //              Remeber that the label index is unique per sub-plot, so indexes are reused for different sub-plots within the same plugin
  // Parameters
  //    Input: subPlotID,     The subplot number that was provided when a new sub-plot was registered
  //    Input: label_p,       a character string with 0 as EOL
  //    Input: labelLength,   the length of the label_p string, not including the terminating EOL (0). Max length is MAX_LABEL_LENGTH
  //    Returns:              the unique ID of this label. Use it when adding lines or boxes with label index.

  unsigned int AddLabel(unsigned int subPlotID, const char* label_p, const unsigned int labelLength);
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// CLASS:       CPlugin_DLL_API_V1
// Description: Base class for a plugin.
//              Inherit from this class and implement/add registration of decoders and plots at construction
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CPlugin_DLL_API_V1
{
public:
  CPlugin_DLL_API_V1()
  {
    memset(&m_info, 0, sizeof(DLL_API_PluginInfo_V1_t));
    m_features = 0;
  }

  virtual ~CPlugin_DLL_API_V1();

protected:

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    SetPluginName
  // Description: This sets the name of the plugin, this will be seen in the LogScrutinizer workspace after the dll has been loaded
  // Parameters
  //    Input: name_p, a 0 terminated text string

  void SetPluginName(char* name_p) {strcpy_s(m_info.name, PLUGIN_INFO_V1_NAME_MAX_SIZE, name_p);}


  // Function:    SetPluginVersion
  // Description: This sets the version of the plugin, this will be seen in the LogScrutinizer workspace after the dll has been loaded
  // Parameters
  //    Input: version_p, a 0 terminated text string

  void SetPluginVersion(char* version_p) {strcpy_s(m_info.version, PLUGIN_INFO_V1_VERSION_MAX_SIZE, version_p);}


  // Function:    SetPluginAuthor
  // Description: This sets the author of the plugin, this will be seen in the LogScrutinizer workspace after the dll has been loaded
  // Parameters
  //    Input: name_p, a 0 terminated text string

  void SetPluginAuthor(char* author_p) {strcpy_s(m_info.author, PLUGIN_INFO_V1_AUTHOR_MAX_SIZE, author_p);}


  // Function:    SetHelpURL
  // Description: This sets an URL to the help for the plugin, this will available with righ-click on the plugin.
  // Parameters
  //    Input: name_p, a 0 terminated text string

  void SetHelpURL(char* helpURL_p) {strcpy_s(m_info.helpURL, PLUGIN_INFO_V1_HELP_URL_MAX_SIZE, helpURL_p);}


  // Function:    SetPluginFeatures
  // Description: Use this to define which features this plugin supports
  // Parameters
  //    Input: supportedFeatures, a bitmask... see SUPPORTED_FEATURE_xxx  (SUPPORTED_FEATURE_PLOT and SUPPORTED_FEATURE_DECODER)

  void SetPluginFeatures(unsigned int supportedFeatures) {m_info.supportedFeatures = supportedFeatures;}


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    RegisterDecoder
  // Description: This is a utility function that a plugin developer may use to add the decoders to the plugin
  // Parameters
  //    Input: decoder_p, A reference to a subclassed CDecode

  void RegisterDecoder(CDecoder* decoder_p) {m_decoders.InsertTail((CListObject*)(decoder_p));}


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    RegisterPlot
  // Description: This is a utility function that a plugin developer may use to add the plots to the plugin
  // Parameters
  //    Input: plot_p, A reference to a subclassed CPlot

  void RegisterPlot(CPlot* plot_p) {m_plots.InsertTail((CListObject*)(plot_p));}


// Functions directly called by LogScrutinizer  -- DON'T USE
public:
  bool                          GetDecoders(CList_LSZ** list_pp) {*list_pp = &m_decoders; return (m_decoders.isEmpty() ? false : true);}
  bool                          GetPlots(CList_LSZ** list_pp) {*list_pp = &m_plots; return (m_plots.isEmpty() ? false : true);}
  DLL_API_PluginInfo_V1_t*      GetInfo(void) {return &m_info;}

private:
  DLL_API_PluginInfo_V1_t       m_info;
  CList_LSZ                     m_decoders;   // a list of CDecoder
  CList_LSZ                     m_plots;      // a list of CPlot
  unsigned int                  m_features;   // SUPPORTED_FEATURE_xxx  A bitmask of the supported features or:ed together
};


#endif