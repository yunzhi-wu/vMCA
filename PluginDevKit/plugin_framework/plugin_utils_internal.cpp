#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "plugin_utils.h"
#include "plugin_utils_internal.h"

//-----------------------------------------------------------------------------------------------------------------------------
//---- Module/Local Constants ----------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

#define GRAPHICAL_OBJECT_BYTE_STREAM_MAX_SIZE (1024 * 1000)    // Max size of one stream is 1MB


//-----------------------------------------------------------------------------------------------------------------------------
//---- Module/Local Varibles ----------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

static HWND     g_hwnd_msgConsumer  = 0;
static HANDLE   g_h_msgHeap         = 0;

#ifdef _DEBUG
#define         TEMP_TRACE_STRING_SIZE    4096
static char     g_tempTraceString[TEMP_TRACE_STRING_SIZE];
#endif

//-----------------------------------------------------------------------------------------------------------------------------
//---- Error handling functions -------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------

void ErrorHook(char* errorMsg, bool trace)
{
#ifdef _DEBUG
  printf("\n\nErrorHook: %s\n\n", errorMsg);
  OutputDebugString(errorMsg);
#endif

  if (trace && g_h_msgHeap != 0)
  {
    Trace(errorMsg);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------
//---- Messagge handling functions -------------------------------------------------------------------

void EnableMsgTrace(HWND hwnd_msgConsumer, HANDLE h_msgHeap)
{
  g_hwnd_msgConsumer  = hwnd_msgConsumer;
  g_h_msgHeap         = h_msgHeap;
}

//-----------------------------------------------------------------------------------------------------------------------------

void Trace(char *pcStr, ...)
{  
  va_list   tArgumentPointer;
  char*     heapString_p      = NULL;
  int       vslength          = 0;

#ifndef _DEBUG
  if (g_h_msgHeap == 0)
  {
    ErrorHook("MsgHeap, no allocated heap", false);
    return;
  }

  if (g_hwnd_msgConsumer == 0)
  {
    ErrorHook("MsgHeap, no msgConsumer", false);
    return;
  }
#endif

  va_start(tArgumentPointer, pcStr);
  vslength = _vscprintf(pcStr, tArgumentPointer) + 10;

#ifdef _DEBUG
  if (g_h_msgHeap != NULL)
  {
    heapString_p  = (char*) HeapAlloc(g_h_msgHeap, HEAP_ZERO_MEMORY, vslength);
  }
  else if ((vslength - 1) < TEMP_TRACE_STRING_SIZE)
  {
    heapString_p  = g_tempTraceString;
  }
  else
  {
    ErrorHook("MsgHeap, string too large", false);
    return;
  }
#else
  heapString_p = (char*) HeapAlloc(g_h_msgHeap, HEAP_ZERO_MEMORY, vslength);
#endif

  if (heapString_p == NULL)
  {
    ErrorHook("MsgHeap, heap full", false);
  }
  
  vsprintf_s(heapString_p, vslength, pcStr, tArgumentPointer);

  va_end(tArgumentPointer);

#ifdef _DEBUG
  OutputDebugString(heapString_p); // This is visible when running Visual Studio, in the output window
#endif

  if (g_hwnd_msgConsumer != 0)
  {
    PostMessage(g_hwnd_msgConsumer, WM_APP_PLUGIN_MSG, (WPARAM)heapString_p, 0);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS: CByteStream
// Description: See plugin_utils.h file
//-----------------------------------------------------------------------------------------------------------------------------

CByteStream::CByteStream(unsigned int size)
{
  m_ref_p           = NULL;
  m_byteStream_p    = NULL;
  m_end_p           = NULL;

  m_usedSize  = 0;
  m_totalSize = size > GRAPHICAL_OBJECT_BYTE_STREAM_MAX_SIZE ? GRAPHICAL_OBJECT_BYTE_STREAM_MAX_SIZE : size;

  m_byteStream_p = (BYTE*) VirtualAlloc(NULL, m_totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  if (m_byteStream_p != NULL)
  {
    m_end_p = m_byteStream_p + m_totalSize - 1;
    m_ref_p = m_byteStream_p;
  }
  else
  {
    m_totalSize = 0;
    ErrorHook("Out of memory, allocating byte stream");
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

CByteStream::~CByteStream()
{
  if (m_byteStream_p != NULL)
  {
    VirtualFree(m_byteStream_p, 0, MEM_RELEASE);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

BYTE* CByteStream::AddBytes(unsigned int size)
{
  unsigned int  totalSize = size + sizeof(ObjectByteStreamHead_t) + sizeof(ObjectByteStreamTail_t);
  BYTE*         temp_ref  = m_ref_p;

  // Check that this add doesn't take us beyond the size of this bytestream

  if ((m_ref_p + totalSize) > m_end_p)

  {
    return NULL;
  }

  // Also check that the previous object is OK (by looking at the tail), as long as this isn't the first add (then there is nothing before)
  if ((m_ref_p != m_byteStream_p) && ((ObjectByteStreamTail_t*)(m_ref_p - sizeof(ObjectByteStreamTail_t)))->tag != OBJECT_BYTE_STREAM_TAIL_TAG)
  {
    ErrorHook("CByteStream::AddBytes   Corrupt tail at previous object, tail tag doesn't match\n");
    return NULL;
  }

  ObjectByteStreamHead_t* head_p = (ObjectByteStreamHead_t*)m_ref_p;
  ObjectByteStreamTail_t* tail_p = (ObjectByteStreamTail_t*)(m_ref_p + size + sizeof(ObjectByteStreamHead_t));

  head_p->size  = size;
  head_p->tag   = OBJECT_BYTE_STREAM_HEAD_TAG;
  tail_p->tag   = OBJECT_BYTE_STREAM_TAIL_TAG;

  m_ref_p     += totalSize;
  m_usedSize  += totalSize;

  return (temp_ref + sizeof(ObjectByteStreamHead_t));
}

//-----------------------------------------------------------------------------------------------------------------------------
//
// m_ref_p points at next ObjectByteStreamHead_t

BYTE* CByteStream::GetBytes(void)
{
  BYTE*                     user_data_p = NULL;
  ObjectByteStreamHead_t*   head_p;
  ObjectByteStreamTail_t*   tail_p;

  //
  // First make sure we can at least extract the head and tail (assuming user data size = 0)

  if ((m_ref_p - m_byteStream_p + sizeof(ObjectByteStreamHead_t) + sizeof(ObjectByteStreamTail_t) > m_usedSize))
  {
    return NULL;
  }

  // m_ref_p shall point at the next ObjectByteStreamHead_t, check the tag and extract size
  head_p = (ObjectByteStreamHead_t*)m_ref_p;

  if (head_p->tag != OBJECT_BYTE_STREAM_HEAD_TAG)
  {
    ErrorHook("CByteStream::GetBytes   Corrupt head, head tag doesn't match\n");
    return NULL;
  }

  unsigned int size = ((ObjectByteStreamHead_t*)m_ref_p)->size; // extract the user size from the head

  user_data_p = m_ref_p + sizeof(ObjectByteStreamHead_t); // Move m_ref_p to point to where the data starts

  // Verify that user data and tail fit in the byte stream
  BYTE* objectEnd_p = user_data_p + size + sizeof(ObjectByteStreamTail_t) - 1;

  if ((objectEnd_p > m_end_p) || ((unsigned int)(objectEnd_p - m_byteStream_p) > m_usedSize))
  {
    ErrorHook("CByteStream::GetBytes   Corrupt head, head + size + tail outside byte stream\n");
    return NULL;
  }

  //verify tail
  tail_p = (ObjectByteStreamTail_t*)(user_data_p + size);
  if (tail_p->tag != OBJECT_BYTE_STREAM_TAIL_TAG)
  {
    ErrorHook("CByteStream::GetBytes   Corrupt tail, tail tag doesn't match\n");
    return NULL;
  }

  m_ref_p = objectEnd_p + 1;

  return user_data_p;
}

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS: CByteStreamManager
// Description: See plugin_utils.h file
//-----------------------------------------------------------------------------------------------------------------------------

BYTE* CByteStreamManager::AddBytes(unsigned int size)
{
  BYTE *ref_p;

  if ((ref_p = m_currentByteStream_p->AddBytes(size)) == NULL)
  {
    m_currentByteStream_p = new CByteStream(m_allocByteStreamSize);

    if (m_currentByteStream_p == NULL)
    {
      return NULL;
    }

    m_byteStreamList.InsertTail((CListObject*)m_currentByteStream_p);
    ref_p = m_currentByteStream_p->AddBytes(size);
  }

  return ref_p;
}

//-----------------------------------------------------------------------------------------------------------------------------

BYTE* CByteStreamManager::AddBytes_ThreadSafe(unsigned int size)
{
  EnterCriticalSection(&m_criticalSection);

  return AddBytes(size);

  LeaveCriticalSection(&m_criticalSection);
}

//-----------------------------------------------------------------------------------------------------------------------------

BYTE* CByteStreamManager::GetBytes(void)
{
  BYTE *ref_p;

  if ((ref_p = m_currentByteStream_p->GetBytes()) == NULL)
  {
    m_currentByteStream_p = (CByteStream*) m_byteStreamList.GetNext((CListObject*)m_currentByteStream_p);

    if (m_currentByteStream_p == NULL)
    {
      return NULL;
    }

    m_currentByteStream_p->ResetRef();

    ref_p = m_currentByteStream_p->GetBytes();
  }

  return ref_p;
}


//-----------------------------------------------------------------------------------------------------------------------------

CGraph_Internal::CGraph_Internal()
{
  
}

//-----------------------------------------------------------------------------------------------------------------------------

CGraph_Internal::CGraph_Internal(char* name_p, unsigned int subPlotID, unsigned int estimatedNumOfObjects)
{
  m_byteStreamManager_p = new CByteStreamManager(sizeof(GraphicalObject_t) * estimatedNumOfObjects);

  m_subPlotID                 = subPlotID;
  m_numOfObjects              = 0;
  m_enabled                   = true;
  m_id                        = g_graphID++;

  m_isGraphExtentInitialized  = false;
  m_graphExtent.x_max         = 0;
  m_graphExtent.x_min         = 0;
  m_graphExtent.y_max         = 0.0;
  m_graphExtent.y_min         = 0.0;

  m_isOverrideColorSet        = false;
  m_overrideColor             = 0;
  m_overrideLinePattern       = GLP_NONE;

  strcpy_s(m_name, MAX_GRAPH_NAME_LENGTH, name_p);
}

//-----------------------------------------------------------------------------------------------------------------------------

CGraph_Internal::~CGraph_Internal()
{
  if (m_byteStreamManager_p != NULL)
  {
    delete m_byteStreamManager_p;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

bool CGraph_Internal::UpdateExtents(GraphicalObject_t* object_p)
{
  if (m_isGraphExtentInitialized)
  {
    if (object_p->x2 > m_graphExtent.x_max)
    {
      m_graphExtent.x_max = object_p->x2;
    }

    if (object_p->y1 < m_graphExtent.y_min)
    {
      m_graphExtent.y_min = object_p->y1;
    }
    
    if (object_p->y2 < m_graphExtent.y_min)
    {
      m_graphExtent.y_min = object_p->y2;
    }

    if (object_p->y2 > m_graphExtent.y_max)
    {
      m_graphExtent.y_max = object_p->y2;
    }

    if (object_p->y1 > m_graphExtent.y_max)
    {
      m_graphExtent.y_max = object_p->y1;
    }
  }
  else
  {
    m_isGraphExtentInitialized = true;

    m_graphExtent.x_min = object_p->x1;
    m_graphExtent.x_max = object_p->x2;
    m_graphExtent.y_min = object_p->y1;
    m_graphExtent.y_max = object_p->y2;
  }

  return true;
}


//-----------------------------------------------------------------------------------------------------------------------------

GraphicalObject_t* CGraph_Internal::GetFirstGraphicalObject(void)
{
  if (m_numOfObjects == 0 && m_byteStreamManager_p != NULL)
  {
    return NULL;
  }

  m_byteStreamManager_p->ResetRef();

  // This graphical object will need to be converted to the "correct" type, line or box
  // Although the data returned contains the correct number of bytes corresponding to the object
  // stored
  GraphicalObject_t* go_p = (GraphicalObject_t*)m_byteStreamManager_p->GetBytes();

  return (go_p);
}

//-----------------------------------------------------------------------------------------------------------------------------

GraphicalObject_t* CGraph_Internal::GetNextGraphicalObject(void)
{
  if (m_numOfObjects == 0 && m_byteStreamManager_p != NULL)
  {
    return NULL;
  }

  // This graphical object will need to be converted to the "correct" type, line or box
  // Although the data returned contains the correct number of bytes corresponding to the object
  // stored
  GraphicalObject_t* go_p = (GraphicalObject_t*)m_byteStreamManager_p->GetBytes();
  return go_p;
}

//-----------------------------------------------------------------------------------------------------------------------------

void CGraph_Internal::GetOverrides(bool* isOverrideColorSet_p, unsigned int* overrideColor_p, GraphLinePattern_t* m_overrideLinePattern_p)
{
  *isOverrideColorSet_p     = m_isOverrideColorSet;
  *overrideColor_p          = m_overrideColor;
  *m_overrideLinePattern_p  = m_overrideLinePattern;
}

//-----------------------------------------------------------------------------------------------------------------------------
// CLASS: CSubPlot
// Description:
//-----------------------------------------------------------------------------------------------------------------------------

CSubPlot::CSubPlot(char* title_p, unsigned int subPlotID, char* Y_AxisLabel_p)
{
  strcpy_s(m_title, MAX_PLOT_NAME_LENTGH, title_p);
  strcpy_s(m_Y_AxisLabel, MAX_PLOT_STRING_LENTGH, Y_AxisLabel_p);

  m_ID          = subPlotID;
  m_properties  = 0;
  m_decorator_p = NULL;

  Clean();
}

//-----------------------------------------------------------------------------------------------------------------------------

CSubPlot::~CSubPlot()
{
  Clean();

  m_labels.DeleteAll();

  if (m_decorator_p != NULL)
  {
    delete m_decorator_p;
    m_decorator_p = NULL;
  }
}

//---------------------------------------------------------------------------------------------

CGraph* CSubPlot::AddGraph(char* name_p, unsigned int estimatedNumOfObjects)
{
  CGraph* graph_p = (CGraph*) new CGraph(name_p, m_ID, estimatedNumOfObjects);

  m_graphs.InsertTail((CListObject*)graph_p);

  return graph_p;
}

//---------------------------------------------------------------------------------------------

CDecorator* CSubPlot::AddDecorator(void)
{
  if (m_decorator_p == NULL)
  {
    m_decorator_p = new CDecorator(m_ID);
  }

  return m_decorator_p;
}

//---------------------------------------------------------------------------------------------

CSequenceDiagram* CSubPlot::AddSequenceDiagram(char* name_p, unsigned int estimatedNumOfObject)
{
  (void)AddDecorator(); // just make sure that it is added

  CSequenceDiagram* sequenceDiagram_p = (CSequenceDiagram*) new CSequenceDiagram(name_p, m_ID, m_decorator_p, estimatedNumOfObject);

  m_graphs.InsertTail((CListObject*)sequenceDiagram_p);

  return sequenceDiagram_p;
}

//---------------------------------------------------------------------------------------------

unsigned int CSubPlot::AddLabel(const char* label_p, const unsigned int labelLength)
{
  if (label_p != NULL && labelLength != 0 && label_p[labelLength] == 0)
  {
    CGO_Label* newLabel_p = new CGO_Label(label_p, labelLength);

    if (newLabel_p == NULL)
    {
      ErrorHook("Out of memory");
      return 0;
    }

    m_labels.InsertTail((CListObject*)newLabel_p);
    return m_labels.GetCount() - 1;
  }
  else
  {
    ErrorHook("CSubPlot::AddLabel failed, bad input parameters\n");
    return -1;
  }
}

//---------------------------------------------------------------------------------------------

void CSubPlot::Clean(void)
{
  memset(&m_extents, 0, sizeof(GraphicalObject_Extents_t));

  m_graphs.DeleteAll();

  if (m_decorator_p != NULL)
  {
    m_decorator_p->Clean();
    m_decorator_p = NULL;
  }
}

//---------------------------------------------------------------------------------------------

void CSubPlot::CalcExtents(void)
{
  GraphicalObject_Extents_t extents;

  CGraph* graph_p = (CGraph*)m_graphs.GetHead();

  if (graph_p != NULL)
  {
    graph_p->GetExtents(&m_extents);

    while (graph_p != NULL)
    {
      graph_p->GetExtents(&extents);

      if (extents.x_min < m_extents.x_min) {m_extents.x_min = extents.x_min;}
      if (extents.x_max > m_extents.x_max) {m_extents.x_max = extents.x_max;}
      if (extents.y_min < m_extents.y_min) {m_extents.y_min = extents.y_min;}
      if (extents.y_max > m_extents.y_max) {m_extents.y_max = extents.y_max;}

      graph_p = (CGraph*)m_graphs.GetNext((CListObject*)graph_p);
    }
  }
}