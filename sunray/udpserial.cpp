// Ardumower Sunray 
// Copyright (c) 2013-2020 by Alexander Grau, Grau GmbH
// Licensed GPLv3 for open source use
// or Grau GmbH Commercial License for commercial use (http://grauonline.de/cms2/?page_id=153)

#include "udpserial.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
UdpSerial udpSerial;
#endif


void UdpSerial::begin(unsigned long baud){
  SerialUSB.begin(baud);
}  

size_t UdpSerial::write(uint8_t data){
  return SerialUSB.write(data);
}
  
  
int UdpSerial::available(){
  return SerialUSB.available();
}


int UdpSerial::read(){
  return SerialUSB.read();
}


int UdpSerial::peek(){
  return SerialUSB.peek();
}

void UdpSerial::flush(){
  SerialUSB.flush();
}



