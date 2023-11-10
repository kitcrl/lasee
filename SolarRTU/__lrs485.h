#ifndef __LRS485_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __LRS485_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <inverter.h>

class cLRS485
{
public:
  cLRS485();
  void init();
  int32_t read(uint8_t* b, int32_t sz, int32_t ms=0);
  int32_t write(uint8_t* b, int32_t sz, int32_t ms=0);

  Inverter invtr;
};



#endif
