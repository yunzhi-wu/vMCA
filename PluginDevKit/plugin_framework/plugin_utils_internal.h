#ifndef PLUGIN_UTILS_INTERNAL_H
#define PLUGIN_UTILS_INTERNAL_H


//-----------------------------------------------------------------------------------------------------------------------------
// File: plugin_utils_internal.h
//
// Description:
// This header file contains utilities for the plugin framework
//
// IMPORTANT: DO NOT MODIFY THIS FILE
//
//-----------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------------------------------------------------------

#include "plugin_constants.h"

#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------------------------------------------------------
//---- Exported Functions -----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

void ErrorHook(char* errorMsg, bool trace = true);
void Trace(char *pcStr, ...);

//-----------------------------------------------------------------------------------------------------------------------------
//---- Internal Functions -----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

extern void EnableMsgTrace(HWND hwnd_msgConsumer, HANDLE h_msgHeap);

//-----------------------------------------------------------------------------------------------------------------------------
//---- Internal Constants -----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

#define WM_APP_PLUGIN_MSG       (WM_APP + 100)

//-----------------------------------------------------------------------------------------------------------------------------
//---- Exported Constants ----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

typedef enum
{
  GLP_NONE    = -1, // No modification of the line pattern, enumerated instead by the LogScrutinizer
  GLP_SOLID,        // ______
  GLP_DASH,         // -------
  GLP_DOT,          // .......
  GLP_DASHDOT,      // -.-.-.-
  GLP_DASHDOTDOT    // -..-..-

}GraphLinePattern_t;

//-----------------------------------------------------------------------------------------------------------------------------
//---- Exported Structures ----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

// Name:        DLL_API_MatchString_V1_t
// Description: Description of a string, content and length. Is used in DLL_API_MatchStrings_V1_t
typedef struct
{
  char                          m_matchString[MAX_STRING_LENGTH];
  unsigned int                  m_length;

} DLL_API_MatchString_V1_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        DLL_API_MatchStrings_V1_t
// Description: This structure describes an array of match strings. The match strings are used by log scrutinizer to
//              aid in the search for text rows which are of interest.
typedef struct
{
  unsigned int                  maxNumOfStrings;
  unsigned int                  numOfStrings;
  DLL_API_MatchString_V1_t*     m_matchStringArray;

} DLL_API_MatchStrings_V1_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_t
// Description: xxx
typedef struct
{
  double                    x1;               // Filled by plugin (in seconds)
  double                    x2;               // -"- -"-          (in seconds)

  float                     y1;               // -"- -"-
  float                     y2;               // -"- -"-

  unsigned int              row;              // -"- -"-

  __int16                   properties;       // Filled by plugin

}GraphicalObject_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        textLabel_t
// Description: This describes a label with size and position holder for an array. To use this structure you must make space for the text starting at position label_a
//              This structure shall not be used by PluginDeveloper, LogScrutinizer internal usage
typedef struct
{
  unsigned char             length;
  char                      label_a;     // Filled by plugin
}textLabel_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GO_BOX_Label_t
// Description: This struture enables LogScrutinizer to either have a BOX with the text embedded in the GO, or simply reference to a text that has been registered before.
//              This structure shall not be used by PluginDeveloper, LogScrutinizer internal usage
typedef struct
{
  union
  {
    unsigned int  labelIndex;
    textLabel_t   textLabel;
  }labelKind;

} GO_Label_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Line_t
// Description: The
typedef struct
{
  GraphicalObject_t     go;

}GraphicalObject_Line_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Line_Ex_t
// Description: A variant of the simple line where you may set the line color and a label on the line
typedef struct
{
  GraphicalObject_t     go;
  unsigned int          lineColorRGB;         // In-case LogScrutinizer should enumerate the colors
  float                 relative_X;           // 0.0, or in case the label should not be located in the middle of the line this parameter shall be used to relatively offset
                                              // 0.1 means 10% from the left/start, and 0.6 means 60% from the left.. that is slightly offset from the middle. 0 means that
  GO_Label_t            label;                // The label, must be placed last in the structure

}GraphicalObject_Line_Ex_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Box_t
// Description: xxx
typedef struct
{
  GraphicalObject_t         go;

}GraphicalObject_Box_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Box_Ex_t
// Description: This structure is used for adding boxes that shall be shown with a label set
typedef struct
{
  GraphicalObject_t         go;

  int                       fillColorRGB;     // -"- -"-  Use RGB(x,x,x) macro
  GO_Label_t                label;            // The label, must be placed last in the structure

}GraphicalObject_Box_Ex_t;

//-----------------------------------------------------------------------------------------------------------------------------
// Description: the GraphicalObject_LifeLine_Box_t is typically just a GraphicalObject_Box_Ex_t with some additional values attached to it.
//              When plotting LogScrutinizer may chose to work with the object as box intead of lifeLine

typedef struct
{
  GraphicalObject_t         go;

  float                     y_center;
  float                     y_execTop;        // Used to determine exec box size
  float                     y_execBottom;     // Used to determine exec box size

  int                       fillColorRGB;     // -"- -"-  Use RGB(x,x,x) macro
  float                     relative_X;       // 0.0, or in case the label should not be located in the middle of the line this parameter shall be used to relatively offset
  GO_Label_t                label;            // The label, must be placed last in the structure

}GraphicalObject_LifeLine_Box_t;

//-----------------------------------------------------------------------------------------------------------------------------
// Description: the GraphicalObject_LifeLine_Line_t

typedef struct
{
  GraphicalObject_t         go;

  int                       lineColorRGB;     // -"- -"-  Use RGB(x,x,x) macro
  float                     relative_X;       // 0.0, or in case the label should not be located in the middle of the line this parameter shall be used to relatively offset
  GO_Label_t                label;            // The label, must be placed last in the structure

}GraphicalObject_LifeLine_Line_t;

//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Extents_t
// Description: xxx
typedef struct
{
  double                    x_min;    // in seconds
  double                    x_max;    // in seconds

  float                     y_min;
  float                     y_max;

}GraphicalObject_Extents_t;


//-----------------------------------------------------------------------------------------------------------------------------

// Name:        GraphicalObject_Message_Ex_t
// Description: A variant of the simple line where you may set the line color and a label on the line
typedef struct
{
  GraphicalObject_Line_Ex_t line;

  unsigned char         lifeLine_Src;
  unsigned char         lifeLine_Dest;

}GraphicalObject_Message_Ex_t;

//-----------------------------------------------------------------------------------------------------------------------------
//---- Module/Local Constants ----------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

#define GRAPHICAL_OBJECT_BYTE_STREAM_MAX_SIZE (1024 * 1000)    // Max size of one stream is 1MB


//-----------------------------------------------------------------------------------------------------------------------------
//---- Module/Local Varibles ----------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

static unsigned int g_graphID = 1;

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CListObject
// Description: The base-class for a list object that shall be managed by the CList_LSZ. Inherit from this class
//-----------------------------------------------------------------------------------------------------------------------------

class CList_LSZ;

class CListObject
{
public:
  CListObject() {m_parentList_p = NULL; m_next_p = NULL; m_previous_p = NULL;}
  virtual ~CListObject() {;}

  bool                  isInList(void) {return (m_parentList_p != NULL ? true : false);}

private:
  class CList_LSZ*      m_parentList_p;
  class CListObject*    m_previous_p;
  class CListObject*    m_next_p;

  friend class CList_LSZ;
};

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CList_LSZ
// Description: The CList_LSZ is used by the plugin framework to store all kinds of elements. This list is also used internally
//              by logscrutinizer, mainly to access lists exported by the plugin.
//-----------------------------------------------------------------------------------------------------------------------------

class CList_LSZ
{
public:
  CList_LSZ() {m_head_p = NULL; m_tail_p = NULL; m_items = 0;}

  bool                  isEmpty(void) {return (m_items == 0 ? true : false);}

  class CListObject*    GetHead(void) {return m_head_p;}
  class CListObject*    GetTail(void) {return m_tail_p;}
  class CListObject*    GetNext(CListObject* listObject_p) {return listObject_p->m_next_p;}
  class CListObject*    GetPrevious(CListObject* listObject_p) {return listObject_p->m_previous_p;}
  unsigned int          GetCount(void) {return m_items;}

  void                  InsertAfter(CListObject* afterListObject_p, CListObject* listObject_p);
  void                  InsertBefore(CListObject* beforeListObject_p, CListObject* listObject_p);
  void                  InsertHead(CListObject* listObject_p);
  void                  InsertTail(CListObject* listObject_p);

  void                  TakeOut(CListObject* listObject_p);
  void                  DeleteAll(void);

private:
  class CListObject*    m_head_p;
  class CListObject*    m_tail_p;

  unsigned int          m_items;
};

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CGO_Label
// Description: This class is used to manage list of added labels

class CGO_Label : CListObject
{
public:

  CGO_Label(const char* label_p, const unsigned int labelLength)
  {
    m_label_p     = NULL;
    m_labelLength = 0;

    if (label_p != NULL && labelLength != 0)
    {
      m_label_p = (char*) malloc(labelLength);

      if (m_label_p != NULL)
      {
        memcpy(m_label_p, label_p, labelLength);
        m_labelLength = labelLength;
      }
    }

    if (m_label_p == NULL)
    {
      ErrorHook("Failed to create GO_Label, out of memory");
    }
  }

  ~CGO_Label()
  {
    if (m_label_p != NULL)
    {
      free(m_label_p);
    }
  }

  CGO_Label() {m_label_p = NULL; m_labelLength = 0;}

  char*           m_label_p;
  unsigned int    m_labelLength;
};

//-----------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

// Object byte stream head and tail is used mainly for tracking objects added to byte stream. Although it is a general
// mechanism it is mainly intended for graphical objects.

typedef struct
{
  __int16   tag;          // The tag is used to verify that no memory overwrites occured
  __int16   size;         // The size of the next object in the byte stream

}ObjectByteStreamHead_t;

typedef struct
{
  __int16   tag;          // The tag is used to verify that no memory overwrites occured

}ObjectByteStreamTail_t;

#define OBJECT_BYTE_STREAM_HEAD_TAG    ((__int16)0x5555)
#define OBJECT_BYTE_STREAM_TAIL_TAG    ((__int16)0xEEEE)

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CByteStream
// Description: This class is an byte buffer/stream writer/reader.
//              m_ref_p keep track of the current location in the stream. Each read/write moves the m_ref_p forward
//              Several CByteStream (in a list) is used by CByteStreamManager to create a unlimited stream size
//-----------------------------------------------------------------------------------------------------------------------------
class CByteStream : CListObject
{
public:
  CByteStream(unsigned int size);
  virtual ~CByteStream();

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    AddBytes
  // Description: Add data to the byte stream, shall be used to store data in the byte stream. Returns ref to store data
  //              It modifies the m_ref_p, as such multiple adds will fill the byte stream.
  //              The returned reference, if not NULL, is within the byte stream. As such it is just to move the object to the retured reference
  //              The CByteStream class will add head and tail to secure the data, however that is not visible in the returned reference
  inline BYTE*    AddBytes(unsigned int size);

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function:    GetBytes
  // Description: Used to manage e.g. graphical objects in byte streams. Retrieve the next bytes from the byte stream. As the CByteStream class
  //              keep track of the objects stored in the byte stream it is not possbile to request the size of the object that shall be retrieved.
  //              It modifies the m_ref_p
  //              The data reference is within the bytes stream
  inline BYTE*    GetBytes(void);

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Function: ResetRef
  // Description: Used to reset m_ref_p to start over reading from the start of the byte stream
  void            ResetRef(void)  {m_ref_p = m_byteStream_p;}

  unsigned int    GetTotalSize(void) {return m_totalSize;}      // The current buffer size available in this byte stream
  unsigned int    GetUsedSize(void) {return m_usedSize;}        // The current used buffer size in this byte stram

private:
  CByteStream();

  BYTE*           m_byteStream_p;               // The byte stream;
  BYTE*           m_ref_p;                      // The current location in the byte stream    m_byteStream_p <= m_ref <= m_end_p
  BYTE*           m_end_p;                      // Points at the last byte in the stream
  unsigned int    m_usedSize;                   // The bytes actually used in the byte array
  unsigned int    m_totalSize;                  // The total number of bytes the byte stream may contain
};

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CByteStreamManager
// Description: Manages a list of CByteStream objects
//-----------------------------------------------------------------------------------------------------------------------------

class CByteStreamManager
{
public:
  CByteStreamManager(unsigned int allocByteStreamSize)
  {
    m_allocByteStreamSize = allocByteStreamSize;
    m_currentByteStream_p = new CByteStream(m_allocByteStreamSize);
    m_byteStreamList.InsertTail((CListObject*)m_currentByteStream_p);

    InitializeCriticalSectionAndSpinCount(&m_criticalSection, 0x00000400);
  }

  ~CByteStreamManager()
  {
    Clean();

    DeleteCriticalSection(&m_criticalSection);
  }

  BYTE* AddBytes(unsigned int size);

  BYTE* AddBytes_ThreadSafe(unsigned int size);   // Add bytes thread safe

  // Function: GetBytes
  // Description: Since the CObjectByteSteam keep tracks of stored "objects" it is only possible to pull a complete object, and not number of bytes
  //              Returns either a ref to the next object or NULL if there is nothing more stored
  BYTE* GetBytes(void);


  void ResetRef(void)
  {
    m_currentByteStream_p = (CByteStream*)m_byteStreamList.GetHead();

    if (m_currentByteStream_p != NULL)
    {
      m_currentByteStream_p->ResetRef();
    }
  }

  void Clean(void)
  {
    m_byteStreamList.DeleteAll();
  }

private:
  CByteStreamManager(){;}

  CByteStream*          m_currentByteStream_p;                // The byte stream;
  unsigned int          m_allocByteStreamSize;                // Recommended size when creating new byteStreams
  CList_LSZ             m_byteStreamList;                     // List of CByteStreams

  CRITICAL_SECTION      m_criticalSection;                    // Thread protection
};

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CGraph_Internal
// Description: Internal object
//-----------------------------------------------------------------------------------------------------------------------------

class CGraph_Internal : CListObject
{
public:
  CGraph_Internal(char* name_p, unsigned int subPlotID, unsigned int estimatedNumOfObjects);

  friend class CSubPlot;

  // Functions used by LogScrutinizer to fetch graphical objects from the graph
  void                          SetEnableFlag(bool enabled) {m_enabled = enabled;}
  bool                          isEnabled(void) {return m_enabled;}
  GraphicalObject_t*            GetFirstGraphicalObject(void);
  GraphicalObject_t*            GetNextGraphicalObject(void);
  char*                         GetName(void) {return m_name;}
  unsigned int                  GetSubPlotID(void) {return m_subPlotID;}
  unsigned int                  GetNumOfObjects(void) {return m_numOfObjects;}
  void                          GetExtents(GraphicalObject_Extents_t* graphExtent_p) {*graphExtent_p = m_graphExtent;}
  void                          GetOverrides(bool* isOverrideColorSet_p, unsigned int* overrideColor_p, GraphLinePattern_t* m_overrideLinePattern_p);

  void Clean(void)
  {
    if (m_byteStreamManager_p != NULL)
    {
      m_byteStreamManager_p->Clean();
    }

    m_numOfObjects = 0;
  }

protected:
  CGraph_Internal();
  virtual ~CGraph_Internal();           // IMPORTANT: The dll_plugin shall NOT delete the CGraph_Internal

  bool                          UpdateExtents(GraphicalObject_t* object_p);

  unsigned int                  m_subPlotID;                          // In-case several graphs shall be given similar presentation such as color they must have the same graphGroup
  char                          m_name[MAX_GRAPH_NAME_LENGTH];        // Name of the graph in the plot

  CByteStreamManager*           m_byteStreamManager_p;

  bool                          m_isGraphExtentInitialized;
  GraphicalObject_Extents_t     m_graphExtent;                        // The max and min of the graphical objects
  unsigned int                  m_numOfObjects;

  unsigned int                  m_id;                                 // This is a unique ID
  bool                          m_enabled;                            // Enable/disable presentation of this graph

  bool                          m_isOverrideColorSet;                 // If the member m_overrideColor is set or not
  unsigned int                  m_overrideColor;                      // 0 default
  GraphLinePattern_t            m_overrideLinePattern;                // GLP_NONE default
};

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS:       CSubPlot
// Description:
//-----------------------------------------------------------------------------------------------------------------------------

class CGraph;       //forward declaration to make these classes known
class CSequenceDiagram;
class CDecorator;

class CSubPlot : public CListObject
{
friend class CPlot;
friend class CSubPlotSurface;

public:
  CSubPlot(char* title_p, unsigned int subPlotID, char* Y_AxisLabel_p);
  virtual ~CSubPlot();

  // Functions used by LogScrutinizer to fetch graphical objects from the graph
  void                                Clean(void);

  inline bool                         GetGraphs(CList_LSZ** graphsList_pp) {*graphsList_pp = &m_graphs; return (m_graphs.isEmpty() ? false : true);}
  inline void                         GetDecorator(CDecorator** decorator_pp) {*decorator_pp = m_decorator_p;}
  inline bool                         GetLabels(CList_LSZ** labels_pp) {*labels_pp = &m_labels; return (m_labels.isEmpty() ? false : true);}

  inline void                         GetExtents(GraphicalObject_Extents_t* extents_p) {*extents_p = m_extents;}
  inline void                         SetExtents(GraphicalObject_Extents_t* extents_p) {m_extents = *extents_p;}

  inline void                         GetTitle(char** title_pp, char** Y_AxisLabel_pp) {*title_pp = m_title, *Y_AxisLabel_pp = m_Y_AxisLabel;}
  inline void                         SetProperties(unsigned int properties) {m_properties = properties;}
  inline unsigned int                 GetID(void) {return m_ID;}
  inline unsigned int                 GetProperties(void) {return m_properties;}

  void                                CalcExtents(void);

  // Function: AddGraph
  // Description: A graph is a set of lines, or boxes, that belongs together and represent a variable's value through time
  //              First add a graph, and then add lines or boxes to the CGraph class using it's member functions
  //              The graph must be added each time a plot is re-run, so when the plot is cleaned all graphs are removed and has to be added again
  // Parameters
  //  Input:      name_p,  the name of the graph, it will be displayed as a legend in the subplot
  //  Input:      estimatedNumOfObjects, when growing the byte sequence holding the graphs/lines this is the number of objects it will grow (make space for) each
  //              time it reach the limit of the current size
  //  Returns:    A CGraph reference to be used for adding lines and boxes. Do not delete this reference.
  //
  CGraph*                             AddGraph(char* name_p, unsigned int estimatedNumOfObjects = 1024);

  CDecorator*                         AddDecorator(void);
  CSequenceDiagram*                   AddSequenceDiagram(char* name_p, unsigned int estimatedNumOfObject = 1024);

protected:

  unsigned int                        AddLabel(const char* label_p, const unsigned int labelLength); // Do not try to use this function, use the corresponding in CPlot
  
  CList_LSZ                           m_graphs;             // List of CGraph elements
  CList_LSZ                           m_labels;             // List of character strings, added through AddLabel

  unsigned int                        m_ID;

  char                                m_title[MAX_PLOT_NAME_LENTGH];
  char                                m_Y_AxisLabel[MAX_PLOT_STRING_LENTGH];

  GraphicalObject_Extents_t           m_extents;
  unsigned int                        m_properties;

  CDecorator*                         m_decorator_p;
};

#endif