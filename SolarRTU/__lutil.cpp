#include "__lutil.h"

cLUtil::cLUtil(HardwareSerial* p)
{
  srl = p;
}

void cLUtil::dprintf(char* type, char* fmt, ...)
{
  char msg[1024] = {0};
	va_list args;
  sprintf(msg, "%8s | ", type==0?"":type);
  va_start (args, fmt);
  vsprintf(&msg[11], fmt, args);
  va_end(args);

  srl->print(msg);

}
