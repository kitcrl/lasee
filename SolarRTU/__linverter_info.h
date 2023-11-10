#ifndef __LINVERTER_INFO_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __LINVERTER_INFO_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#include "Arduino.h"
#include <ENV.h>

class cLInverterInfo
{
public:
    u8 invNo;
    u16 invCode;
    boolean ccErr; // Communication Error
    u8 ccErrCnt;
    int16_t invTemp;
    int16_t inner;
    u8 invStrQty;
    u16 solarVolt;
    u16 solarCurrent;
    u32 solarPower;
    
    u16 invVoltR;
    u16 invVoltS;
    u16 invVoltT;
    u16 invVoltRMax;
    u16 invVoltSMax;
    u16 invVoltTMax;
    u16 invCurrentR;
    u16 invCurrentS;
    u16 invCurrentT;
    u32 invPower;
    u32 invPowerMax;
    u32 invPowerReactive;
    u16 invFrequency;
    u16 invFrequencyMax;
    u16 invFrequencyMin;
    u16 invPf;
    u16 resistance;
    u16 currentLeak;
    u8 model[10];
    u8 serialNum[20];
    u16 alarm[2];
    u32 daily;
    u64 total;
    u16 status[5];
    u8 calcType;

    u16 stringVolt[MAX_STR_COUNT];
    u16 stringCurrent[MAX_STR_COUNT];
    u8 strType;
    u16 strState[5];

    void init();
    void isError();
};

#endif