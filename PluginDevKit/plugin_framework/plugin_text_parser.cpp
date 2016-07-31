#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>

#include "plugin_text_parser.h"

//-----------------------------------------------------------------------------------------------------------------------------

inline bool isDigit(unsigned int ch)
{
  if ((ch >= '0' && ch <= '9') || ch == '-')
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------

inline bool isFloatDigit(unsigned int ch)
{
  if ((ch >= '0' && ch <= '9') ||
       ch == '-' ||
       ch == '.')
  {
    return true;
  }
  else
  {
    return false;
  }
}


//-----------------------------------------------------------------------------------------------------------------------------

inline bool isHexDigit(unsigned int ch)
{
  if ((ch >= '0' && ch <= '9') ||
      (ch >= 'A' && ch <= 'F') ||
      (ch >= 'a' && ch <= 'f') ||
       ch == '-' ||
       ch == '.' ||
       ch == 'x' ||
       ch == 'X')
  {
    return true;
  }
  else
  {
    return false;
  }
}

#define MAX_INT_STRING 24

//-----------------------------------------------------------------------------------------------------------------------------

bool CTextParser::ParseInt(int* value_p)
{
  char                  value[MAX_INT_STRING]     = "";
  register const char*  loopText_p    = &m_text_p[m_parseIndex];
  register const char*  loopTextEnd_p = &m_text_p[m_textLength];
  const char*           startpoint_p  = loopText_p;

  while (loopText_p < loopTextEnd_p && !isDigit(*loopText_p))
    ++loopText_p;

  if (loopText_p == loopTextEnd_p)
  {
    return false;
  }

  unsigned int index = 0;
  while (loopText_p < loopTextEnd_p && index < (MAX_INT_STRING - 1) && isDigit(*loopText_p))
  {
    value[index] = *loopText_p;
    ++loopText_p;
    ++index;
  }

  m_parseIndex += (int)(loopText_p - startpoint_p);

  value[index] = 0;

  *value_p = strtol(value, NULL, 10);

  return true;
}

//-----------------------------------------------------------------------------------------------------------------------------

bool CTextParser::ParseHexInt(unsigned long* value_p)
{
  char                  value[MAX_INT_STRING]     = "";
  register const char*  loopText_p    = &m_text_p[m_parseIndex];
  register const char*  loopTextEnd_p = &m_text_p[m_textLength];
  const char*           startpoint_p  = loopText_p;

  while (loopText_p < loopTextEnd_p && !isHexDigit(*loopText_p))
    ++loopText_p;

  if (loopText_p == loopTextEnd_p)
  {
    return false;
  }

  unsigned int index = 0;
  while (loopText_p < loopTextEnd_p && index < (MAX_INT_STRING - 1) && isHexDigit(*loopText_p))
  {
    value[index] = *loopText_p;
    ++loopText_p;
    ++index;
  }

  m_parseIndex += (int)(loopText_p - startpoint_p);

  value[index] = 0;

  *value_p = strtoul(value, NULL, 16);

  return true;
}

#define MAX_INT64_STRING 21

//-----------------------------------------------------------------------------------------------------------------------------

bool CTextParser::Parse_INT64(__int64* value_p)
{
  char                  value[25]     = "";
  register const char*  loopText_p    = &m_text_p[m_parseIndex];
  register const char*  loopTextEnd_p = &m_text_p[m_textLength];
  const char*           startpoint_p  = loopText_p;

  while (loopText_p < loopTextEnd_p && !isDigit(*loopText_p))
    ++loopText_p;

  if (loopText_p == loopTextEnd_p)
  {
    return false;
  }

  unsigned int index = 0;
  while (loopText_p < loopTextEnd_p && index < (MAX_INT64_STRING - 1) && isDigit(*loopText_p))
  {
    value[index] = *loopText_p;
    ++loopText_p;
    ++index;
  }

  m_parseIndex += (int)(loopText_p - startpoint_p);

  value[index] = 0;

  *value_p = _atoi64(value);

  return true;
}

//-----------------------------------------------------------------------------------------------------------------------------

bool CTextParser::ParseFloat(float* value_p)
{
  char                  value[25]     = "";
  register const char*  loopText_p    = &m_text_p[m_parseIndex];
  register const char*  loopTextEnd_p = &m_text_p[m_textLength];
  const char*           startpoint_p  = loopText_p;

  while (loopText_p < loopTextEnd_p && !isFloatDigit(*loopText_p))
    ++loopText_p;

  if (loopText_p == loopTextEnd_p)
  {
    return false;
  }

  unsigned int index = 0;
  while (loopText_p < loopTextEnd_p && index < (MAX_INT64_STRING - 1) && isFloatDigit(*loopText_p))
  {
    value[index] = *loopText_p;
    ++loopText_p;
    ++index;
  }

  m_parseIndex += (int)(loopText_p - startpoint_p);

  value[index] = 0;

  _CRT_FLOAT floatVal;

   int status = _atoflt(&floatVal, value);

   *value_p = floatVal.f;

  if (status == 0)
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------------------------------------------------------

void CTextParser::Extract(unsigned int startIndex, unsigned int endIndex, char* value_p)
{
#ifdef _DEBUG
  if (startIndex >= m_textLength || endIndex >= m_textLength || startIndex > endIndex)
  {
    value_p[0] = 0;
    ErrorHook("CTextParser::Extract  Bad input parameters\n", false);
    return;
  }
#endif

  memcpy(value_p, &m_text_p[startIndex], endIndex - startIndex + 1);

  value_p[endIndex - startIndex + 1] = 0;
}

//-----------------------------------------------------------------------------------------------------------------------------

bool CTextParser::Search(char* match_p, unsigned int matchLength, int limitSearchOffset)
{
  unsigned int textLength;

#ifdef _DEBUG
  if (match_p == NULL || matchLength == 0 || m_textLength == 0 || m_text_p == 0)
  {
    ErrorHook("CTextParser::Extract  Bad input parameters\n", false);
    return false;
  }
#endif

  unsigned int  parseIndex  = m_parseIndex;
  m_isSearchMatch           = false;

  if (limitSearchOffset != -1 && ((limitSearchOffset + parseIndex) < m_textLength))
  {
    textLength = limitSearchOffset + parseIndex;
  }
  else
  {
    textLength = m_textLength;  // Move it to a local variable for speed

  }

  // Loop over each letter in the m_text_p

  matchLength--;  // to make it 0 index based

  for (; parseIndex < textLength && (matchLength <= (textLength - parseIndex)); ++parseIndex)
  {
    register const char*  loopText_p    = &m_text_p[parseIndex];                 // Direct reference to the character in the loop evaluation
    register char*        loopSearch_p  = match_p;
    unsigned int          textIndex     = parseIndex;                            // Index to the start/next letter in the current string
    unsigned int          searchIndex   = 0;
    bool                  loop          = true;

    // Loop evaluation, from a letter in the text string see if the filter matches... loop the filter and the text together
    // See if the filter fits well from the current char and on
    while ((*loopSearch_p == *loopText_p) && loop)
    {
      if (searchIndex == matchLength)  // If we have a match for all the letters in the filter then it was success
      {
        m_searchMatch_StartIndex  = parseIndex;
        m_parseIndex              = parseIndex + matchLength + 1;
        m_searchMatch_EndIndex    = m_parseIndex;
        m_isSearchMatch           = true;
        return true;
      }

      ++searchIndex;

      if (textIndex == textLength)
      {
        loop = false;
      }

      ++textIndex;

      ++loopSearch_p;
      ++loopText_p;
    }
  }

  m_parseIndex = textLength - 1; // EOL
  return false;
}

//-----------------------------------------------------------------------------------------------------------------------------