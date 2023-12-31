#include <Arduino.h>
#include <avr/wdt.h>
#include <ENV.h>
#include <Ethernet.h>


#include <stdint.h>
#include "__lsocket.h"

#define SPI_SS       7

cLSocket::cLSocket()
{
  init();
}

void cLSocket::init()
{
  eth.init(SPI_SS);
}

void cLSocket::to_ipv4(uint8_t* ip)
{
  uint8_t _ip[4] = {0};
  uint8_t j = 0;
  uint8_t i = 0;
  uint8_t idx = 0;

  for ( i=0 ; *(ip+i) ; i++, j++)
  {
    _ip[j] = *(ip+i);
    if ( (*(ip+i+1) == '.') || (*(ip+i+1) == 0) )
    {
      this->ip[idx] = atoi(_ip);
      j=-1;
      i++;
    }
  }
}



void cLSocket::ready(uint8_t* ip)
{
  _putl->dprintf(__FILE__,__LINE__,"ready \r\n");
  if (eth.linkStatus() == LinkON)
  {
    if (eth.begin(mac) == 0)
    {
      _putl->dprintf(__FILE__,__LINE__,"DHCP Required \r\n");
      to_ipv4(ip);
      eth.begin(mac, this->ip);
    }
    else
    {
      _putl->dprintf(__FILE__,__LINE__,"DHCP Enabled \r\n");
    }
  }
  else
  {
    _putl->dprintf(__FILE__,__LINE__,"Link Down \r\n");
  }
}


int32_t cLSocket::connect(uint8_t* ip, int16_t port)
{
  int32_t e = 0;

  e = clnt.connect((const char*)ip, port);
  return e;
}

int32_t cLSocket::close()
{
  clnt.stop();
  return 0;
}

int32_t cLSocket::connected()
{
  int32_t e = 0;
  e = clnt.connected();
  return e;
}

int32_t cLSocket::read(int32_t fd, uint8_t* b, int32_t sz, int32_t ms)
{
  int32_t e = 0;
  e = clnt.read(b, sz);
  return e;
}

int32_t cLSocket::write(int32_t fd, uint8_t* b, int32_t sz, int32_t ms)
{
  int32_t e = 0;
  e = clnt.write(b, sz);
  return e;
}
