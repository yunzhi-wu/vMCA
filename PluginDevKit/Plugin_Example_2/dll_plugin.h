
#include "plugin_api_v1.h"

class CPlugin_Example_2 : public CPlugin_DLL_API_V1
{
public:
  CPlugin_Example_2();   
};


//-------------------------------------------------------------------------------------------------------------------------------------------------

class Q87_Decoder : public CDecoder
{
public:
  Q87_Decoder();

  virtual bool pvDecode(char* row_p, unsigned int* length_p, const unsigned int maxLength);
};
