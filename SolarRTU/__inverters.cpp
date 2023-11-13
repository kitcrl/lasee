#include "__inverters.h"

int32_t make_inverter_packet(cLUtil* p, uint16_t type, uint8_t* packet, int32_t sz)
{
  int32_t e = -1;
  int32_t i = 0;
  int16_t crc16 = 0;
  switch(type)
  {
    case __INVERTERS_HANHWA_SUNGROW :
      *(packet+i) = 0x01;
      i++;
      *(packet+i) = 0x04;
      i++;
      *(packet+i) = 0x13;
      i++;
      *(packet+i) = 0x87;
      i++;
      *(packet+i) = 0x00;
      i++;
      *(packet+i) = 0x34;
      i++;
      crc16 = p->CRC16(*packet, 6);
      *(packet+i) = crc16 / 0x100;
      i++;
      *(packet+i) = crc16 % 0x100;
      i++;
      e = i;
      break;
    case __INVERTERS_HANHWA_GOODWE_MI :
      break;
    case __INVERTERS_HANHWA_GOODWE_HT :
      break;
  }
  return e;
}


int32_t assign_inverter_packet(cLUtil* p, uint16_t type, uint8_t* packet, int32_t sz, cLInverterInfo* pinfo)
{
  int32_t e = -1;

  pinfo->solarVolt = p->change_seq(packet+25, 16) / 10;
  pinfo->solarCurrent = p->change_seq(packet+27, 16);
  pinfo->solarCurrent += p->change_seq(packet+31, 16);
  pinfo->solarCurrent += p->change_seq(packet+35, 16);
  pinfo->invFrequency = p->change_seq(packet+75, 16) * 10;
  pinfo->stringVolt[0] = pinfo->stringVolt[1] = p->change_seq(packet+25, 16) / 10;
  pinfo->stringVolt[2] = pinfo->stringVolt[3] = p->change_seq(packet+29, 16) / 10;
  pinfo->stringVolt[4] = pinfo->stringVolt[5] = p->change_seq(packet+33, 16) / 10;
  pinfo->solarPower = p->change_seq(packet+37, 32);
  pinfo->invVoltR = p->change_seq(packet+41, 16) / 10;
  pinfo->invVoltS = p->change_seq(packet+43, 16) / 10;
  pinfo->invVoltT = p->change_seq(packet+45, 16) / 10;
  pinfo->invCurrentR = p->change_seq(packet+47, 16);
  pinfo->invCurrentS = p->change_seq(packet+49, 16);
  pinfo->invCurrentT = p->change_seq(packet+51, 16);
  pinfo->invPower = p->change_seq(packet+65, 32);
  pinfo->invPf = p->change_seq(packet+73, 16) * 10;
  pinfo->invTemp = (int16_t)p->change_seq(packet+19, 16);
  pinfo->daily = (u32)p->change_seq(packet+9, 16) * 100;
  pinfo->total = (u64)p->change_seq(packet+11, 32) * 1000;
  pinfo->status[0] = p->change_seq(packet+79, 16);
  pinfo->status[1] = p->change_seq(packet+9, 163);
  pinfo->status[2] = p->change_seq(packet+103, 16);
  pinfo->status[3] = p->change_seq(packet+105, 16);
  pinfo->status[4] = p->change_seq(packet+5, 16);

  pinfo->invVoltRMax = pinfo->invVoltR > pinfo->invVoltRMax ? pinfo->invVoltR : pinfo->invVoltRMax;
	pinfo->invVoltSMax = pinfo->invVoltS > pinfo->invVoltSMax ? pinfo->invVoltS : pinfo->invVoltSMax;
	pinfo->invVoltTMax = pinfo->invVoltT > pinfo->invVoltTMax ? pinfo->invVoltT : pinfo->invVoltTMax;
	pinfo->invPowerMax = pinfo->invPower > pinfo->invPowerMax ? pinfo->invPower : pinfo->invPowerMax;
	pinfo->invFrequencyMax = pinfo->invFrequency > pinfo->invFrequencyMax ? pinfo->invFrequency : pinfo->invFrequencyMax;
	pinfo->invFrequencyMin = pinfo->invFrequency < pinfo->invFrequencyMin ? pinfo->invFrequency : pinfo->invFrequencyMin;

  return e;
}
