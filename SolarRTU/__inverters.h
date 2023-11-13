#ifndef __INVERTERS_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __INVERTERS_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#include <stdio.h>
#include <stdint.h>
#include "__lutil.h"
#include "__linverter_info.h"

enum
{
  __INVERTERS_HANHWA_SUNGROW = 0,
  __INVERTERS_HANHWA_GOODWE_MI,
  __INVERTERS_HANHWA_GOODWE_HT,
};

int32_t make_inverter_packet(cLUtil* p, uint16_t type, uint8_t* packet, int32_t sz);
int32_t assign_inverter_packet(cLUtil* p, uint16_t type, uint8_t* packet, int32_t sz, cLInverterInfo* pinfo);

#endif
