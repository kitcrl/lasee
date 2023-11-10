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




#include <LaseeEthernet.h>


#include "__lsocket.h"
#include "__lutil.h"


#if defined __cplusplus
}
#endif

#if 0
+----+----+ +----+----+  +----+----+ +----+----+
+----+----+ +----+----+  +----+----+ +----+----+
 ^^^                                          ^
 |||                                          |
 |||                                          +---------------- 
 ||+----------------------------------------------------------- 
 |+------------------------------------------------------------ 
 +------------------------------------------------------------- connected to the server
#endif


#define CONNECTED_TO_SERVER                   0x80000000

class cRTU
{
public :
  cRTU();


  cLUtil*    _putl;
  cLSocket*  _psck;


  Button button;         // Size : 4
  Buzzer buzzer;         // Size : 6
  EEPROMUtil eepromUtil; // Size : 1
  LaseeServer server; // Size : 

  Display display;
  Debug debug; // Size : 259
  ExtDevice extDevice;

  HardwareSerial*  dserial;

  uint32_t   SR;
  char buf[1024];

private :
  u32 curMilliSec;
  u32 invSecondCnt;
  u32 hzSendCnt;
  u32 serverSendCnt;



public :
  void isBlackout();
  void Init();
  void Loop();
  void tcpc();

  //Util util; // Size : 1
};


#endif
