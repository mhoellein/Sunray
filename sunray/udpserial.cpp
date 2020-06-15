// Ardumower Sunray 
// Copyright (c) 2013-2020 by Alexander Grau, Grau GmbH
// Licensed GPLv3 for open source use
// or Grau GmbH Commercial License for commercial use (http://grauonline.de/cms2/?page_id=153)

#include "udpserial.h"
#include "WiFiEsp.h"
#include "WiFiEspUdp.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SERIAL)
UdpSerial udpSerial;
#endif

#define REMOTE_IP 192,168,2,56
unsigned int localPort = 4211;  // local port 
unsigned int remotePort = 4210;  // remote port to connect to
IPAddress remoteIP(REMOTE_IP);


WiFiEspUDP Udp;
char packetBuffer[100];          // buffer to packet
int idx = 0;
bool udpStarted = false;
bool udpActive = false;


void UdpSerial::begin(unsigned long baud){  
  SerialUSB.begin(baud);
}  

void UdpSerial::beginUDP(){  
  Udp.begin(localPort);  
  Udp.beginPacket(remoteIP, remotePort);
  // Udp.endPacket();
  udpStarted = true;  
}

size_t UdpSerial::write(uint8_t data){
  if ((udpStarted) && (!udpActive)) {
    udpActive = true;
    packetBuffer[idx] = char(data);
    idx++;
    if (idx == 99){
      packetBuffer[idx] = '\0';      
      Udp.write(packetBuffer);              
      idx = 0;            
    }
    udpActive = false;
  }  
  SerialUSB.write(data);
  return 1; 
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



