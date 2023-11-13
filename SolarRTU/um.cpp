#include "arduino.h"
#include "um.h"

#include "__inverters.h"


//#define SERVER_IP    "api.lasee.io"
#define SERVER_IP    "192.168.0.9"
#define SERVER_PORT   80





#ifdef INCLUDE_WIRELESS_2560
extern volatile unsigned long timer0_millis;
#endif



LaseeTimer timer;      // Size : 14


u8 rxBuf[UINT8_MAX]; // Size : 256
u8 rxIdx = 0;
//byte mac[6];
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













static int tcpc(cRTU* p)
{
  int e = 0;
  char raw[1024] = {0};

  if ( (p->SR & CONNECTED_TO_SERVER) == 0 )
  {
    p->_putl->dprintf(__FILE__,__LINE__,"Connect to the %s:%d\r\n", SERVER_IP, SERVER_PORT);
    e = p->_psck->connect(SERVER_IP, SERVER_PORT);
    if ( !e )
    {
      p->SR &= ~CONNECTED_TO_SERVER;
      p->_putl->dprintf(__FILE__,__LINE__," connect fail (%d) \r\n", e);
    }
    else
    {
      p->SR |= CONNECTED_TO_SERVER;
      p->_putl->dprintf(__FILE__,__LINE__," connect success (%d) \r\n", e);
    }
    p->_putl->dprintf(__FILE__,__LINE__,"SR -> %04X\r\n", p->SR);
  }
  else
  {
    e = p->_psck->connected();
    if ( e == 0 )
    {
      p->_putl->dprintf(__FILE__,__LINE__,"SR -> %04X DISCONNECTED \r\n", p->SR);
      p->SR &= ~CONNECTED_TO_SERVER;
    }
  }
  return e;
}







int make_server_packet(cLInverterInfo* p, char* out)
{
  int e = 0;
  int i = 0;

  #if 1
  //*(out+i) = 
  sprintf_P(out, (const char *)F("GET /inverter_v2?data=%04x%02x%04x%02x%04x%04x%04x%04x%08lx%04x%04x%04x%04x%04x%04x%04x%04x%04x%08lx%08lx%08lx%04x%04x%04x%04x%08lx%08lx%08lx%04x%04x"),

    0,  // rtuInfo.mcno,                              // 4
    1,  // invSeq + 1,                                // 2
    p->invCode,                   // 4
    p->ccErr,                     // 2
    p->invTemp,                   // 4
    p->inner,                     // 4
    p->solarVolt,                 // 4
    p->solarCurrent,              // 4
    p->solarPower,                // 8
    p->invVoltR,                  // 4
    p->invVoltS,                  // 4
    p->invVoltT,                  // 4
    p->invCurrentR,               // 4
    p->invCurrentS,               // 4
    p->invCurrentT,               // 4
    p->invVoltRMax,               // 4
    p->invVoltSMax,               // 4
    p->invVoltTMax,               // 4
    p->invPower,                  // 8
    p->invPowerMax,               // 8
    p->invPowerReactive,          // 8
    p->invFrequency,              // 4
    p->invFrequencyMax,           // 4
    p->invFrequencyMin,           // 4
    p->invPf,                     // 4
    p->daily,                     // 8
    (u32)(p->total / UINT32_MAX), // 8
    (u32)(p->total % UINT32_MAX), // 8
    p->resistance,                // 4
    p->currentLeak                // 4 
  );

  e = strlen(out);
  #endif
  return e;
}


int write_to_server(cRTU* p, char* b, int sz)
{
  int e = 0;
  char raw[1024] = {0};

  e = p->_psck->connected();
  if ( e == 0 )
  {
    p->_putl->dprintf(__FILE__,__LINE__,"SR -> %04X DISCONNECTED \r\n", p->SR);
    p->SR &= ~CONNECTED_TO_SERVER;
  }
  else
  {
    e = p->_psck->write(0, b, sz);
    p->_putl->dprintf(__FILE__,__LINE__,"write : %s (%d)\r\n", b, e);

    e = p->_psck->read(0, p->buf, 1024);
    p->buf[e] = 0;
    if ( e > 0 ) p->_putl->dprintf(__FILE__,__LINE__,"read : %s (%d) \r\n", p->buf, e);
  }

  return e;
}

static int rsfp(cRTU* p)
{
  int e = 0;
  int sz = 0;
  char raw[1024] = {0};

  sz = make_inverter_packet(p->_putl, __INVERTERS_HANHWA_SUNGROW, p->rsbuf[0], 64);

  e = p->_prsf->write(p->rsbuf[0],sz);
  if ( e < 0 )
  {
    return e;
  }
  p->_putl->raw_buffer(p->rsbuf[0], e, raw);
  p->_putl->dprintf(__FILE__,__LINE__,"write raw %s (%d)\r\n", raw, e);

  e = p->_prsf->read(p->rsbuf[1], 64);
  if ( e < 0 )
  {
    return e;
  }
  p->_putl->raw_buffer(p->rsbuf[1], e, raw);
  p->_putl->dprintf(__FILE__,__LINE__,"read  raw %s (%d)\r\n", raw, e);


  assign_inverter_packet(p->_putl, __INVERTERS_HANHWA_SUNGROW, p->rsbuf[1], e, &p->_invinf);


  return e;
}




int periodic_call(cRTU* p, stPeriodicCall* prd, int (*f)(cRTU*), int lcount)
{
  int e = 0;

  prd->count[0] ++;
  if ( !( prd->count[0] % 0x1F ) )
  {
    prd->count[1] ++;

    if ( !( prd->count[1] % lcount ) )
    {
      prd->count[1] = 0;
      e = f(p);
    }
    prd->count[0] = 0;
  }
  return e;
}









cRTU::cRTU()
{
  _putl = new cLUtil(&Serial3);
  _psck = new cLSocket();
  _prsf = new cLRS485(&Serial4);

  _prsf->_putl = _psck->_putl = _putl;
}

#if 1
void cRTU::Loop()
{
  int e = 0;
  wdt_reset();

  tcpc(this);
  e = periodic_call(this, &this->_prdc, rsfp, 6000);
  if ( e > 0 )
  {
    uint8_t out[1024];
    e = make_server_packet(&this->_invinf, out);
    _putl->dprintf(__FILE__,__LINE__,"write : %s\r\n", out);
    write_to_server(this, out, e);
  }
}
#endif


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
  eepromUtil.readEEPROM(_psck->mac);
  //server.init();
  //rs485Device.init();

  _prsf->init();

  #if defined(INCLUDE_WIRELESS_DISPLAY)
  display.run();
  #endif

  sprintf_P(debugBuf, (const char *)F("\nSystem Start..\nver.%04x\nmcno : %04d\ninvQty : %02d\nweatherQty : %02d\nprQty : %02d\nwhmQty : %02d\n\n"),
            FIRM_VER, rtuInfo.mcno, rtuInfo.invQty, rtuInfo.weatherQty, rtuInfo.prQty, rtuInfo.whmQty);
  DebugSerial.print(debugBuf);

  //sprintf_P(debugBuf, (const char *)F("Mac Addr : %02x-%02x-%02x-%02x-%02x-%02x"), mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  //DebugSerial.println(debugBuf);

  _putl->dprintf(__FILE__,__LINE__,"Mac Addr : %02x-%02x-%02x-%02x-%02x-%02x \r\n", _psck->mac[0], _psck->mac[1], _psck->mac[2], _psck->mac[3], _psck->mac[4], _psck->mac[5]);

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

  _psck->ready();
  //server.beginEthernet();
}



























///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

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

