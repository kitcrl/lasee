#ifndef __UM_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __UM_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#if defined __cplusplus
extern "C"
{
#endif


#include <ENV.h>
#include <avr/wdt.h>

#include <RTUInfo.h>
#include <InverterInfo.h>
#include <WeatherInfo.h>
#include <PRInfo.h>
#include <WHMInfo.h>
#include <JunctionBoxInfo.h>

#include <RS485Device.h>
#include <Inverter.h>
#include <Weather.h>
#include <PR.h>
#include <WHM.h>
#include <JunctionBox.h>

#include <LaseeTimer.h>
#include <Buzzer.h>
#include <Button.h>

#include <EEPROMUtil.h>
#include <Util.h>
#include <ExtDevice.h>

#include <CatM1.h>
#include <CLCD.h>
#include <CLCD_CABLE.h>

#include <Debug.h>




//#include <LaseeEthernet.h>


#include "__lsocket.h"
#include "__lutil.h"
#include "__lrs485.h"

#include "__linverter_info.h"

#if defined __cplusplus
}
#endif

#if 0
+----+----+ +----+----+
+----+----+ +----+----+
 ^^^                 ^
 |||                 |
 |||                 +---------------- 
 ||+---------------------------------- 
 |+----------------------------------- 
 +------------------------------------ connected to the server
#endif


#define CONNECTED_TO_SERVER                   0x8000


typedef struct stPeriodicCall
{
  int count[2];
};

class cRTU
{
public :
  cRTU();

  cLUtil*    _putl;
  cLSocket*  _psck;
  cLRS485*   _prsf;

  Button button;         // Size : 4
  Buzzer buzzer;         // Size : 6
  EEPROMUtil eepromUtil; // Size : 1
  //LaseeServer server; // Size : 

  Display display;
  Debug debug; // Size : 259
  ExtDevice extDevice;

  HardwareSerial*  dserial;

  uint16_t   SR;
  uint8_t buf[1024];
  uint8_t rsbuf[2][64];


  stPeriodicCall  _prdc;

  cLInverterInfo  _invinf;

private :
  u32 curMilliSec;
  u32 invSecondCnt;
  u32 hzSendCnt;
  u32 serverSendCnt;



public :
  void isBlackout();
  void Init();
  void Loop();
  //Util util; // Size : 1
};


#endif
