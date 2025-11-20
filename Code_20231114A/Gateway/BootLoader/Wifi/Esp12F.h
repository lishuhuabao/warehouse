#ifndef __ESP12F_H_
#define __ESP12F_H_

typedef struct{
	u16 cnt;
	u08 step;
	bool flag;
}wifiCfgSt;

extern wifiCfgSt wifiCfgS;
extern bool TcpClosedFlag;

void esp12f_config(void);

#endif







