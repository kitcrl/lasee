#include "__lrs485.h"

cLRS485::cLRS485(HardwareSerial* p)
{
  srl = p;
}

void cLRS485::init()
{
  srl->begin(9600, (SERIAL_8N1 | SERIAL_RS485));
}

int32_t cLRS485::read(uint8_t* b, int32_t sz, int32_t ms=0)
{
  int32_t i = 0;
  for ( i=0 ; i<sz ; i++)
  {
    *(b+i) = srl->read();
    //_putl->dprintf("RS485", "read : %02X %d\r\n", *(b+i), (int8_t)*(b+i) );

    if ( ( (int8_t)*(b+i) ) == -1 ) break;
  }
  //_putl->dprintf("RS485", "read : %d \r\n", i);

  *(b+i) = 0;
  return i;
}
int32_t cLRS485::write(uint8_t* b, int32_t sz, int32_t ms=0)
{
  int32_t e = 0;

  e = srl->write(b, sz);
	srl->flush();
  return e;
}

