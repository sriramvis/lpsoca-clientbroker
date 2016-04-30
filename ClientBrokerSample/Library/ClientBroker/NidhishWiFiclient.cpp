#include "NidhishWiFiclient.h"

int findString(String needle, String haystack) {
  int foundpos = -1;
  for (int i = 0; i <= haystack.length() - needle.length(); i++) {
    if (haystack.substring(i,needle.length()+i) == needle) {
      foundpos = i;
                                                           }
                                                                 }
  return foundpos;
                                               }


bool moduleResponds(){
  Serial1.println("AT");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Module responds");
        return true;   
               }
    else{
        SerialUSB.println("Wi-Fi Module doesn't respond!");
        return false;
        }   
    
                     }

void wifi_init(){
  Serial1.begin(115200);
  SerialUSB.begin(0);
  while(!SerialUSB);
              }

bool setWifiMode(){
  Serial1.println("AT+CWMODE=1");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Mode set");
        return true;   
               }
    else{
        SerialUSB.println("Couldn't set Wi-Fi Mode!");
        return false;
        }         
                 }

bool setWifiTransmissionMode(){
  Serial1.println("AT+CIPMODE=0");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Transmission Mode Set");
        return true;   
               }
    else{
        SerialUSB.println("Couldn't set Wi-Fi Transmission Mode!");
        return false;
        }         
                              }

bool setWifiConnectionMode(){
  Serial1.println("AT+CIPMUX=0");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Connection Mode Set");
        return true;   
               }
  x = findString("link is builded", response);
    if(x!=-1){
         SerialUSB.println("Wi-Fi Connection Mode Already Set");
         return true;   
             }
    else{
        SerialUSB.println("Couldn't set Wi-Fi Connection Mode!");
        return false;
        }         
                            }

bool disconnectFromAP(){
  Serial1.println("AT+CWQAP");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Disconnected from AP");
        return true;   
               }
    else{
        SerialUSB.println("Wi-Fi Couldn't Disconnect from AP!");
        return false;
        }         
                       }

bool connectToAP(String APname,String APpassword){
  SerialUSB.println("Wi-Fi Connecting to AP...");
  Serial1.setTimeout(10000);
  Serial1.println("AT+CWJAP=\""+APname+"\",\""+APpassword+"\"");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
    if(x != -1){
        SerialUSB.println("Wi-Fi Connected to AP");
        Serial1.setTimeout(1000);
        return true;   
               }
    else{
        SerialUSB.println("Wi-Fi Couldn't Connect to AP!");
        Serial1.setTimeout(1000);
        return false;
        }         
    
                                                  }
                  
bool getIP(){
  Serial1.println("AT+CIFSR");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
  if(x != -1){
      SerialUSB.println("Wi-Fi Got IP");
      return true;   
             }
  else{
      SerialUSB.println("Wi-Fi Couldn't get IP!");
      return false;
      }         
            }

String wifiInit(String APname,String APpassword){
int counter=0;
wifi_init();
while(!moduleResponds()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Module Response Timeout!");
        return "Error: 1";  //Wi-Fi Module Response Timeout!
                  }
                        }
counter = 0;
while(!setWifiMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Mode Response Timeout!");
        return "Error: 2";  //Setting Wi-Fi Mode Response Timeout!
                  }
                      }
counter = 0;
while(!setWifiTransmissionMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Transmission Mode Response Timeout!");
        return "Error: 3";  //Setting Wi-Fi Transmission Mode Response Timeout!
                  }
                                 }
counter = 0;
while(!setWifiConnectionMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Connection Mode Response Timeout!");
        return "Error: 4";  //Setting Wi-Fi Connection Mode Response Timeout!
                  }
                               }
counter = 0;
while(!disconnectFromAP()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Disconnection from AP Timeout!");
        return "Error: 5";  //Wi-Fi Disconnection from AP Timeout!
                  }
                          }
counter = 0;
while(!connectToAP(APname,APpassword)){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Connection to AP Timeout!");
        return "Error: 6";  //Wi-Fi Connection to AP Timeout!
                  }
                                      }
counter = 0;
while(!getIP()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Getting IP Timeout!");
        return "Error: 7";  //Wi-Fi Getting IP Timeout!
                  }
               }

 return "OK";                                    }

bool connectToServer(String servername, int port){
Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\","+ String(port));
Serial1.flush();
SerialUSB.println("Wi-Fi Connecting to Server...");
String response = Serial1.readStringUntil(Serial1.available());
int x = findString("OK", response);
if(x!=-1){
    SerialUSB.println("Wi-Fi Connected to Server");
    return true;   
         }
x = findString("ALREADY CONNECTED", response);
if(x!=-1){
    SerialUSB.println("Wi-Fi Already Connected to Server");
    return true;   
         }
else{
    SerialUSB.println("Wi-Fi Couldn't connect to Server!");
    return false;
    }         
 
                                                }

bool sendDataLength(String tcpHeader, String path, String servername, String data){
  Serial1.print("AT+CIPSEND=");
  Serial1.println(tcpHeader.length()+data.length()+4);
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString(">", response);
  if(x != -1){
    SerialUSB.println("Wi-Fi ready to send data");
    return true;
             }
  else{
    SerialUSB.println("Wi-Fi is not ready to send data!");
    return false;
       }
                                                                    }

String sendData(String tcpHeader, String data){
  SerialUSB.println("Wi-Fi Sending data...");
  Serial1.setTimeout(5000);
  Serial1.println(tcpHeader);
  Serial1.flush();
  Serial1.println(data);
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
  if(x != -1){
    SerialUSB.println("Wi-Fi data sent");
    response.remove(0, findString("alive",response) + 9);
    Serial1.setTimeout(1000);
    return response;
             }
  else{
    SerialUSB.println("Wi-Fi data couldn't send!");
    Serial1.setTimeout(1000);
    return "";
       }

                                                               }

bool closeConnection(){
  Serial1.println("AT+CIPCLOSE");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
  if(x != -1){
    SerialUSB.println("Wi-Fi Server Connection Closed");
    return true;
             }
  else{
    SerialUSB.println("Wi-Fi Server Connection Can't be Closed!");
    return false;
       }
                      }

String connectAndSend(String data, String path, String servername, String datatype, int port){
  int counter = 0;
  String response;
  String tcpHeader = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";

  while(!connectToServer(servername, port)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Connection to Server Timeout!");
      return "Error: 8";  //Wi-Fi Connection to Server Timeout!
            }
                         }
  counter = 0;
  while(!sendDataLength(tcpHeader,path,servername,data)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Sending Data length Timeout!");
      return "Error: 9";  //Wi-Fi Sending Data length Timeout!
            }
                         }
  counter = 0;
  while((response = sendData(tcpHeader,data)) == ""){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Data Sending Timeout!");
      return "Error: 10";  //Wi-Fi Data Sending Timeout!
            }
                    }
  counter = 0;
  while(!closeConnection()){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Server Disconnection Timeout!");
      return "Error: 11";  //Wi-Fi Server Disconnection Timeout!
            }
                    }

  return response;
                                                                                            }

String sendAndReceive(String data, String path, String servername, String datatype){
  int counter = 0;
  String response;
  String tcpHeader = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";
  
  while(!sendDataLength(tcpHeader,path,servername,data)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Sending Data length Timeout!");
      return "Error: 9";  //Wi-Fi Sending Data length Timeout!
            }
                         }
  counter = 0;
  while((response = sendData(tcpHeader,data)) == ""){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Data Sending Timeout!");
      return "Error: 10";  //Wi-Fi Data Sending Timeout!
            }
                    }

  return response;
                                                                                    }

bool sleep(int time){
  Serial1.println("AT+GSLP="+String(time));
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  int x = findString("OK", response);
  if(x != -1){
    SerialUSB.println("Wi-Fi went to Deep Sleep");
    return true;
             }
  else{
    SerialUSB.println("Wi-Fi couldn't Seep Sleep!");
    return false;
      }
                    }

bool wakeup(){
  Serial1.setTimeout(2000);
  pinMode(36,OUTPUT);
  digitalWrite(36,HIGH);
  digitalWrite(36,LOW);
  for(int l=0;l<1000000;l++){
  asm("nop\r\n");}
  digitalWrite(36,HIGH);
  for(int l=0;l<1000000;l++){
  asm("nop\r\n");}
  SerialUSB.println("Wi-Fi woke up");
             }

String wakeupSendSleep(String APname,String APpassword,String data,String path,String servername,String datatype,int port){
  String str = "";
  wifi_init();
  int counter = 0;
  wakeup();
  String APconnect = wifiInit(APname, APpassword);
  if(APconnect == "OK"){
  str = connectAndSend(data,path,servername,datatype,port);

  while(!sleep(1000)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi sleeping Timeout!");
      return "Error: 13";  //Wi-Fi sleeping Timeout!
                  }
                   }
                      }
  else{
    return APconnect;
      }
  return str;
                                                                                                                        }

String connectSendSend(String data, String path, String servername, String datatype, int port){
  int counter = 0;
  String response;
  String tcpHeader = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";

  while(!connectToServer(servername, port)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Connection to Server Timeout!");
      return "Error: 8";  //Wi-Fi Connection to Server Timeout!
            }
                         }

  response = sendAndReceive(data,path,servername,datatype);
  response = response + "\n" + sendAndReceive(data,path,servername,datatype);

  counter = 0;
  while(!closeConnection()){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Server Disconnection Timeout!");
      return "Error: 11";  //Wi-Fi Server Disconnection Timeout!
            }
                    }

  return response;
                                                                                            }