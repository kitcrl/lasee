#ifndef __LSOCKET_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__
#define __LSOCKET_H_F2DBDC40_6196_4E67_A689_D31A9310BEC0__

#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <Ethernet.h>
#include "__lutil.h"

class cLSocket
{
public:
  cLSocket();
  void init();
  void ready(uint8_t* ip=0);
  int32_t connect(uint8_t* ip, int16_t port);
  int32_t connected();
  int32_t close();
  int32_t read(int32_t fd, uint8_t* b, int32_t sz, int32_t ms=0);
  int32_t write(int32_t fd, uint8_t* b, int32_t sz, int32_t ms=0);
  void to_ipv4(uint8_t* ip);

  EthernetClass   eth;
  EthernetClient  clnt;

  uint8_t mac[6];
  uint32_t ip[4];


  cLUtil*   _putl;


};



#endif
