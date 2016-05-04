#ifndef NidhishWiFiclient_H
#define NidhishWiFiclient_H

#include "Arduino.h"

int findString(String needle, String haystack);
bool moduleResponds();
void wifi_init();
bool setWifiMode();
bool setWifiTransmissionMode();
bool setWifiConnectionMode();
bool disconnectFromAP();
bool connectToAP(String APname,String APpassword);
bool getIP();
bool setApAutoconnectOn();
bool setApAutoconnectOff();
String wifiInit(String APname,String APpassword);
bool connectToServer(String servername, int port);
bool sendDataLength(String tcpHeader, String path, String servername, String data);
String sendData(String tcpHeader, String data);
bool closeConnection();
String connectAndSend(String data, String path, String servername, String datatype, int port, String* res);
String sendAndReceive(String data, String path, String servername, String datatype);
bool sleep(int time);
bool wakeup();
String wakeupSendSleep(String APname,String APpassword,String data,String path,String servername,String datatype,int port, String* res);
String connectSendSend(String data, String path, String servername, String datatype, int port);

#endif