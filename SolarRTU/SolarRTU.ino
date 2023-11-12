#include "um.h"

#ifdef INCLUDE_WIRELESS_2560
uint8_t mcusr_mirror __attribute__ ((section (".noinit")));
void get_mcusr(void) \
  __attribute__((naked)) \
  __attribute__((section(".init3")));
void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}
#endif

cRTU*    rtu;

void setup()
{
  rtu = new cRTU();
  rtu->Init();
}

void loop()
{
  rtu->Loop();
}

/*
세팅 방법
Wireless, Cable
AVR DA-series ( no bootloader )
AVR128DA64
BOD : Enabled
Wire : 1x Wire, Master or Slave

Cable MAC :
Wire : 2x Wire, Master or Slave

Wireless_2560 : 
ATMega2560

*/

/*
TODO :

1. 구글드라이브에 프로토콜 별 로그 비교하기 (다 달라서 하기 어려움)
2. 각 프로토콜마다 단위 확인하기 (로그 없어서 어려움)
3. 세팅프로그램 각 프로토콜 별로 점검하기 (안되있는거 많음)
4. 세팅프로그램으로 값 다 들어오는지 확인하기
5. git document 정리하기
6. 무선에서 Display Include 하면 RTU 동작안함

*/



/*
발전소 검증 목록

010.
011. O
014. O ( PF 확인 )
021.
022.
023.
024.
025.
031.
042.
043.
050.
052. O
056.
061. O
062. O
066.
071. O
083.
084.
091.
092.
095.
101. O
102.
103. O
104.
105.
111.
112.
113.
114.
115. O
121.
122.
125.
126. O
131.
132.
141.
142.
145.
151.
152.
156.
161.
163.
165.
171.
176.
181. O
182.
190.
191.
195.
202.
206.
211.
212. O ( PF 확인 )
213.
215.
216.
221.
231.
236.
241.
242.
246. O
252. O

Inverter + Weatehr. O


*/