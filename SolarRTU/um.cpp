#include "arduino.h"
#include "um.h"



#ifdef INCLUDE_WIRELESS_2560
extern volatile unsigned long timer0_millis;
#endif



LaseeTimer timer;      // Size : 14


u8 rxBuf[UINT8_MAX]; // Size : 256
u8 rxIdx = 0;
byte mac[6];
char debugBuf[UINT8_MAX]; // Size : 256





RTUInfo rtuInfo;                     // Size : 27
WeatherInfo weatherInfo;             // Size : 17
PRInfo prInfo[MAX_PR_COUNT];         // Size : 79 * 5
WHMInfo whmInfo;                     // Size : 41
JunctionBoxInfo jBoxInfo;
InverterInfo invInfo[MAX_INV_COUNT]; // Size : 256 * 20

RS485Device rs485Device; // Size : 1
Inverter inverter;       // Size : 71
Weather weather;         // Size : 5
PR pr;                   // Size : 6
WHM whm;                 // Size : 5
JunctionBox jBox;        // Size : 








#ifdef INCLUDE_WIRELESS_AVR128DA64
HardwareSerial &RS485Serial = Serial2; // Inverter (RS485)
HardwareSerial &CatM1Serial = Serial0; // Cat.M1 (RS232)
HardwareSerial &DebugSerial = Serial1; // Debug(Check) (TTL)
#elif defined(INCLUDE_CABLE_V1)
HardwareSerial &RS485Serial = Serial3; // Inverter
HardwareSerial &DebugSerial = Serial1; // Debug(Check) (TTL)
IPAddress laseeIp(192, 168, 0, 100);
#elif defined(INCLUDE_CABLE_V2) || defined(INCLUDE_CABLE_V3)
//// working
HardwareSerial &ExtDeviceSerial = Serial1; // External Device (RS485)
HardwareSerial &DebugSerial = Serial3;     // Debug(Check) (TTL)
HardwareSerial &RS485Serial = Serial4;     // Inverter (RS485)
IPAddress laseeIp(192, 168, 0, 9);
#elif defined(INCLUDE_WIRELESS_2560)
HardwareSerial &RS485Serial = Serial;  // Inverter (RS485) Size : 157
HardwareSerial &CatM1Serial = Serial1; // Cat.M1 (RS232) Size : 157
HardwareSerial &DebugSerial = Serial3; // Debug(Check) (TTL) Size : 157
#endif


cRTU::cRTU()
{
  _putl = new cLUtil(&Serial3);
  _psck = new cLSocket();
}

void cRTU::isBlackout()
{
#ifdef INCLUDE_WIRELESS
  if (digitalRead(BLACKOUT))
  {
    if (!(timer.blackout)-- && rtuInfo.blackout == false)
    {
      rtuInfo.blackout = true;
      timer.blackout = BLACKOUT_TIME;
    }
  }
  else
  {
    rtuInfo.blackout = false;
    timer.blackout = BLACKOUT_TIME;
  }

  if (rtuInfo.blackout != rtuInfo.blackoutPre)
  {
    rtuInfo.blackoutPre = rtuInfo.blackout;
    server.serverFlag = true;

    sprintf_P(debugBuf, (const char *)F("Blackout Status: %d"), rtuInfo.blackout);
    DebugSerial.print(debugBuf);
  }
#endif
}

void cRTU::Init()
{
  #ifdef INCLUDE_WIRELESS_2560
  wdt_enable(8);
  #endif

  #ifdef INCLUDE_WIRELESS
  pinMode(TEST_LED, OUTPUT);
  pinMode(BLACKOUT, INPUT);
  pinMode(PIN_CAT_ON, OUTPUT);
  // pinMode(PIN_WIFI_ON, OUTPUT);
  pinMode(PIN_RS485_EN, OUTPUT);
  digitalWrite(PIN_CAT_ON, HIGH);
  // digitalWrite(PIN_WIFI_ON, HIGH);
  digitalWrite(PIN_RS485_EN, LOW);
  #elif defined(INCLUDE_CABLE_V1)
  pinMode(PIN_RS485_EN, OUTPUT);
  digitalWrite(PIN_RS485_EN, LOW);
  #elif defined(INCLUDE_CABLE_V2) || defined(INCLUDE_CABLE_V3)
  pinMode(SW_LEFT, INPUT_PULLUP);
  pinMode(SW_RIGHT, INPUT_PULLUP);
  pinMode(SW_UP, INPUT_PULLUP);
  pinMode(SW_DOWN, INPUT_PULLUP);
  ExtDeviceSerial.begin(9600, (SERIAL_8N1 | SERIAL_RS485));
  #endif

  pinMode(SEND_SWITCH, INPUT);
  curMilliSec = 0;
  invSecondCnt = 0;
  hzSendCnt = 0;
  serverSendCnt = 0;

  buzzer.init();
  button.init();
  timer.init();
  rtuInfo.init();
  #if defined(INCLUDE_CABLE) || defined(INCLUDE_WIRELESS_DISPLAY)
  display.init();
  #endif

  for (u8 i = 0; i < MAX_INV_COUNT; i++)
  {
    invInfo[i].init();
  }

  for (u8 i = 0; i < MAX_PR_COUNT; i++)
  {
    prInfo[i].init();
  }

  weatherInfo.init();
  whmInfo.init();
  debug.init();
  eepromUtil.readEEPROM();
  server.init();
  rs485Device.init();

  #if defined(INCLUDE_WIRELESS_DISPLAY)
  display.run();
  #endif

  sprintf_P(debugBuf, (const char *)F("\nSystem Start..\nver.%04x\nmcno : %04d\ninvQty : %02d\nweatherQty : %02d\nprQty : %02d\nwhmQty : %02d\n\n"),
            FIRM_VER, rtuInfo.mcno, rtuInfo.invQty, rtuInfo.weatherQty, rtuInfo.prQty, rtuInfo.whmQty);
  DebugSerial.print(debugBuf);

  sprintf_P(debugBuf, (const char *)F("Mac Addr : %02x-%02x-%02x-%02x-%02x-%02x"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  DebugSerial.println(debugBuf);

  for (u8 i = 0; i < rtuInfo.invQty; i++)
  {
    sprintf_P(debugBuf, (const char *)F("inv Id[%d].Code = %d"), invInfo[i].invNo, invInfo[i].invCode);
    DebugSerial.println(debugBuf);
  }

  if (rtuInfo.weatherQty)
  {
    sprintf_P(debugBuf, (const char *)F("weather Id[%d].Code = %d"), weatherInfo.weatherNo, weatherInfo.weatherCode);
    DebugSerial.println(debugBuf);
  }

  for (u8 i = 0; i < rtuInfo.prQty; i++)
  {
    sprintf_P(debugBuf, (const char *)F("pr Id[%d].Code = %d"), prInfo[i].prNo, prInfo[i].prCode);
    DebugSerial.println(debugBuf);
  }

  if (rtuInfo.whmQty)
  {
    sprintf_P(debugBuf, (const char *)F("whm Id[%d].Code = %d"), whmInfo.no, whmInfo.code);
    DebugSerial.println(debugBuf);
  }

  wdt_enable(8); // https://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html



  server.beginEthernet();
}

#if 0
void cRTU::Loop()
{
  wdt_reset();
  buzzer.run();
  curMilliSec = millis();
  if (curMilliSec / SECOND != invSecondCnt)
  {
    invSecondCnt = curMilliSec / SECOND;
    // display.run();
#if defined(INCLUDE_CABLE) || defined(INCLUDE_WIRELESS_DISPLAY)
    display.run();
#endif

    if (curMilliSec / (u32)((u32)SECOND * (u32)SEND_TIME_SERVER_HAEZOOM) != hzSendCnt)
    {
      hzSendCnt = curMilliSec / (u32)((u32)SECOND * (u32)SEND_TIME_SERVER_HAEZOOM);
#if HZ_CONTROL_MODE
      server.serverHaezoomFlag = true;
#endif
    }

    isBlackout();
    if (curMilliSec >= (u32)((u32)SECOND * (u32)SEND_TIME_SERVER))
    {
      server.serverFlag = true;

#if defined(INCLUDE_WIRELESS_2560)
      timer0_millis = 0;
#else
      set_millis(curMilliSec % (u32)((u32)SECOND * (u32)SEND_TIME_SERVER));
      curMilliSec = millis();
      invSecondCnt = curMilliSec / SECOND;
      hzSendCnt = curMilliSec / (u32)((u32)SECOND * (u32)SEND_TIME_SERVER_HAEZOOM);
#endif
    }

    int serverBuzzerFlag = server.send();
    buzzer.play(serverBuzzerFlag);
    rs485Device.send();
  }

  rs485Device.recv();
  if (server.recv() == 1)
  {
    inverter.init();
  }
  if (debug.recv() == 1)
  {
    server.serverFlag = true;
    DebugSerial.println("Debug Send");
  }
  extDevice.recv();

#if defined(INCLUDE_CABLE_V2) || defined(INCLUDE_CABLE_V3)

  display.buttonEvent();
#endif
  if (button.isButtonClick())
  {
    buzzer.play(BUZZER_TIC);
    server.serverFlag = true;
  }
}
#endif


#define SERVER_IP    "192.168.0.9"
#define SERVER_PORT   80


void raw_buffer(uint8_t* b, int32_t s, uint8_t* o)
{
  int32_t i = 0;

  for ( i=0 ; i<s ; i++ )
  {
    sprintf(o+i*3, "%02X ", *(b+i));
  }
}


void cRTU::tcpc()
{
  int32_t e = 0;
  uint8_t raw[1024] = {0};

  if ( (SR & CONNECTED_TO_SERVER) == 0 )
  {
    _putl->dprintf("SOCKET", "Connect to the %s:%d\r\n", SERVER_IP, SERVER_PORT);
    e = _psck->connect(SERVER_IP, SERVER_PORT);
    if ( !e )
    {
      SR &= ~CONNECTED_TO_SERVER;
      _putl->dprintf("SOCKET", " connect fail (%d) \r\n", e);
    }
    else
    {
      SR |= CONNECTED_TO_SERVER;
      _putl->dprintf("SOCKET", " connect success (%d) \r\n", e);
    }
    _putl->dprintf("SOCKET", "SR -> %08X\r\n", SR);
  }
  else
  {
    e = _psck->connected();
    if ( e == 0 )
    {
      _putl->dprintf("SOCKET", "SR -> %08X DISCONNECTED \r\n", SR);
      SR &= ~CONNECTED_TO_SERVER;
    }
    else
    {
      e = _psck->write(0, "Hello World", 11);
      _putl->dprintf("SOCKET", "write : %d %s\r\n", e, "Hello World");

      e = _psck->read(0, buf, 1024);
      if ( e > 0 ) _putl->dprintf("SOCKET", "read : %d, %s \r\n", e, buf);

      raw_buffer(buf, e, raw);
      _putl->dprintf("SOCKET", "raw %s \r\n", raw);
    }
  }


}

#if 1
void cRTU::Loop()
{
  wdt_reset();

  tcpc();

  return;



  buzzer.run();
  buzzer.play(true);
  return;
  if ( (SR & CONNECTED_TO_SERVER) == 0 )
  {
    server.send();
    SR |= CONNECTED_TO_SERVER;
  }
}
#endif

