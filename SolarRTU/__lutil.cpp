#include "__lutil.h"

cLUtil::cLUtil(HardwareSerial* p)
{
  srl = p;
}

void cLUtil::dprintf(uint8_t* fn, int32_t ln, const char* fmt,...)
{
  char msg[512] = {0};
	va_list args;
  sprintf(msg, "%12s %4d | ", fn==0?"":fn, ln);
  va_start (args, fmt);
  vsprintf(&msg[strlen(msg)], fmt, args);
  va_end(args);

  srl->print(msg);

}


uint16_t cLUtil::CRC16(uint8_t* nData, uint16_t wLength)
{
	uint8_t nTemp;
	uint16_t wCRCWord = 0xFFFF;

	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= pgm_read_word(&gCRCTable[nTemp]);
	}
	wCRCWord = wCRCWord % 0x100 * 0x100 + wCRCWord / 0x100;
	return wCRCWord;
}


int32_t cLUtil::raw_buffer(uint8_t* b, int32_t s, uint8_t* o)
{
  int32_t i = 0;

  for ( i=0 ; i<s ; i++ )
  {
    sprintf(o+i*3, "%02X ", *(b+i));
  }

  return i*3;
}

int32_t cLUtil::change_seq(uint8_t* b, int32_t bit)
{
  int32_t i = 0;
  int32_t r = 0;
  for ( i=(bit/8)-1 ; i>=0 ; i-- )
  {
    r |= *(b+i) << (i*8);
  }

  return r;
}

