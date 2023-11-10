#ifndef __LUTIL_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __LUTIL_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#include <Arduino.h>
#include <stdarg.h>

class cLUtil
{
public:
  cLUtil(HardwareSerial*);
  void dprintf(char* type, char* fmt, ...);
  HardwareSerial*  srl;
};

#endif