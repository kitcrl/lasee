#include "__linverter_info.h"

void cLInverterInfo::init()
{
	ccErr = true;
	ccErrCnt = 99;
	invNo = 0;
	invCode = 0;
	invStrQty = 0;
	invFrequencyMin = UINT16_MAX;
}

void cLInverterInfo::isError()
{
	ccErrCnt++;
	if (ccErrCnt > 99)
	{
		ccErr = true;
		ccErrCnt = 100;
		solarVolt = 0;
		solarCurrent = 0;
		solarPower = 0;
		invVoltR = 0;
		invVoltS = 0;
		invVoltT = 0;
		invCurrentR = 0;
		invCurrentS = 0;
		invCurrentT = 0;
		invPower = 0;
		invPf = 0;
		invFrequency = 0;
		invTemp = 0;
	}
	else
	{
		ccErr = false;
	}
}