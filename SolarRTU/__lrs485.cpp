#include "__lrs485.h"

cLRS485::cLRS485()
{
  init();
}

void cLRS485::init()
{
  invtr.init();
}

int32_t cLRS485::read(uint8_t* b, int32_t sz, int32_t ms=0)
{
  int32_t e = 0;

  return e;
}
int32_t cLRS485::write(uint8_t* b, int32_t sz, int32_t ms=0)
{
  int32_t e = 0;

  return e;
}

