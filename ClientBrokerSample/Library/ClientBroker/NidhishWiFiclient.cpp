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
  int x;
  Serial1.println("AT");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  
  if(response.length() > 2){
    x = findString("OK", response);
                           }  
  else{
    SerialUSB.println("Wi-Fi Module respond Timeout!");
    return false;
      }

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
              }

bool setWifiMode(){
  int x;
  Serial1.println("AT+CWMODE=1");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());

  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Mode setting Timeout!");
    return false;
      }
  
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
  int x;
  Serial1.println("AT+CIPMODE=0");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Transmission Mode Timeout!");
    return false;
      }
  
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
  int x;
  Serial1.println("AT+CIPMUX=0");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi set Connection Mode Timeout!");
    return false;
      }
  if(x != -1){
    SerialUSB.println("Wi-Fi Connection Mode Set");
    return true;   
             }
  if(response.length() > 15){
    x = findString("link is builded", response);
  }else{
    SerialUSB.println("Wi-Fi set Connection Mode Timeout!");
    return false;
  }
  
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
  int x;
  Serial1.println("AT+CWQAP");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Disconnection Timeout!");
    return false;
      }
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
  int x;
  SerialUSB.println("Wi-Fi Connecting to AP...");
  Serial1.setTimeout(10000);
  Serial1.println("AT+CWJAP=\""+APname+"\",\""+APpassword+"\"");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Connecting to AP Timeout!");
    return false;
      }
  
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
  int x;
  Serial1.println("AT+CIFSR");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Getting IP Timeout!");
    return false;
      }
  if(x != -1){
    SerialUSB.println("Wi-Fi Got IP");
    return true;   
             }
  else{
    SerialUSB.println("Wi-Fi Couldn't get IP!");
    return false;
      }         
            }

bool setApAutoconnectOn(){
  int x;
  Serial1.println("AT+CWAUTOCONN=1");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi setiing AP Autoconnect Timeout!");
    return false;
      }
  if(x != -1){
    SerialUSB.println("Wi-Fi AP Autoconnect set");
    return true;   
             }
  else{
    SerialUSB.println("Wi-Fi AP Autoconnect Couldn't set!");
    return false;
      }         
                      }

bool setApAutoconnectOff(){
  int x;
  Serial1.println("AT+CWAUTOCONN=0");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi setiing AP Autoconnect Timeout!");
    return false;
      }
  if(x != -1){
    SerialUSB.println("Wi-Fi AP Autoconnect set");
    return true;   
             }
  else{
    SerialUSB.println("Wi-Fi AP Autoconnect Couldn't set!");
    return false;
      }         
                      }

String wifiInit(String APname,String APpassword){
int counter=0;
wifi_init();
while(!moduleResponds()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Module Response Failed!");
        return "Error: 1";  //Wi-Fi Module Response Failed!
                  }
                        }
counter = 0;
while(!setWifiMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Mode Failed!");
        return "Error: 2";  //Setting Wi-Fi Mode Failed!
                  }
                      }
counter = 0;
while(!setWifiTransmissionMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Transmission Mode Failed!");
        return "Error: 3";  //Setting Wi-Fi Transmission Mode Failed!
                  }
                                 }
counter = 0;
while(!setWifiConnectionMode()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Setting Wi-Fi Connection Mode Failed!");
        return "Error: 4";  //Setting Wi-Fi Connection Mode Failed!
                  }
                               }
counter = 0;
while(!disconnectFromAP()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Disconnection from AP Failed!");
        return "Error: 5";  //Wi-Fi Disconnection from AP Failed!
                  }
                          }
counter = 0;
while(!connectToAP(APname,APpassword)){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Connecting to AP Failed!");
        return "Error: 6";  //Wi-Fi Connecting to AP Failed!
                  }
                                      }
counter = 0;
while(!getIP()){
  counter++;
    if(counter>=5){
        SerialUSB.println("Wi-Fi Getting IP Failed!");
        return "Error: 7";  //Wi-Fi Getting IP Failed!
                  }
               }

 return "OK";                                    }

bool connectToServer(String servername, int port){
  int x;
  Serial1.setTimeout(2000);
  Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\","+ String(port));
  Serial1.flush();
  SerialUSB.println("Wi-Fi Connecting to Server...");
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Connecting to Server Timeout!");
    return false;
      }

  if(x!=-1){
    SerialUSB.println("Wi-Fi Connected to Server");
    Serial1.setTimeout(1000);
    return true;   
           }
  if(response.length() > 17){
     x = findString("ALREADY CONNECTED", response);
  }
  else{
    SerialUSB.println("Wi-Fi Connecting to Server Timeout!");
    return false;
      }
  if(x!=-1){
    SerialUSB.println("Wi-Fi Already Connected to Server");
    Serial1.setTimeout(1000);
    return true;   
           }
  else{
    SerialUSB.println("Wi-Fi Couldn't connect to Server!");
    Serial1.setTimeout(1000);
    return false;
      }         
 
                                                }

bool sendDataLength(String tcpHeader, String path, String servername, String data){
  int x;
  Serial1.print("AT+CIPSEND=");
  Serial1.println(tcpHeader.length()+data.length()+4);
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString(">", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Sending data length Timeout!");
    return false;
      }

  if(x != -1){
    SerialUSB.println("Wi-Fi ready to send data");
    return true;
             }
  else{
    SerialUSB.println("Wi-Fi is not ready to send data, Issue with the link!");
    return false;
       }
                                                                                 }

String sendData(String tcpHeader, String data){
  int x;
  SerialUSB.println("Wi-Fi Sending data...");
  Serial1.setTimeout(5000);
  Serial1.println(tcpHeader);
  Serial1.flush();
  Serial1.println(data);
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Sending data Timeout!");
    return "";
      }

  if(x != -1){
    SerialUSB.println("Wi-Fi data sent");
    response.remove(0, findString("alive",response) + 9);
    Serial1.setTimeout(1000);
    return response;
             }
  else{
    SerialUSB.println("Wi-Fi couldn't send the data!");
    Serial1.setTimeout(1000);
    return "";
       }

                                                               }

bool closeConnection(){
  int x;
  Serial1.println("AT+CIPCLOSE");
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Server Connection Closing Timeout!");
    return false;
      }

  if(x != -1){
    SerialUSB.println("Wi-Fi Server Connection Closed");
    return true;
             }
  else{
    SerialUSB.println("Wi-Fi Server Connection Can't be Closed!");
    return false;
       }
                      }

String connectAndSend(String data, String path, String servername, String datatype, int port, String* res){
  int counter = 0;
  String response = "";
  String tcpHeader = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";

  // while(!connectToServer(servername, port)){
  //   counter++;
  //   if(counter>=5){
  //     SerialUSB.println("Wi-Fi Connection to Server Failed!");
  //     return "Error: 8";  //Wi-Fi Connection to Server Failed!
  //           }
  //                        }

  // counter = 0;
  // while(!sendDataLength(tcpHeader,path,servername,data)){

  //   counter++;
  //   if(counter>=5){
  //     SerialUSB.println("Wi-Fi Sending Data length Failed!");
  //     return "Error: 9";  //Wi-Fi Sending Data length Failed!
  //           }
  //                        }
  // counter = 0;
  // while((response = sendData(tcpHeader,data)) == ""){  
    
  // counter++;
  // if(counter>=5){
  //   SerialUSB.println("Wi-Fi Data Sending Failed!");
  //   return "Error: 10";  //Wi-Fi Data Sending Failed!
  //               }
  //                                                  }
  while(true){
    if(connectToServer(servername, port)){
      if(sendDataLength(tcpHeader,path,servername,data)){
        if((response = sendData(tcpHeader,data)) != ""){
            break;
        }
        else{
          counter++;
        }
      }
      else{
        counter++;
      }
    }
    else{
      counter++;
    }
    if(counter >= 5){
      SerialUSB.println("Wi-Fi Data Sending Failed!");
      return "Error: 10";
    }
  }
  

  counter = 0;
  while(!closeConnection()){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Server Disconnection Failed!");
      return "Error: 11";  //Wi-Fi Server Disconnection Failed!
            }
                          }

  *res = response;
  return "Success";
                                                                                            }

String sendAndReceive(String data, String path, String servername, String datatype){
  int counter = 0;
  String response;
  String tcpHeader = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:"+datatype+"\r\nContent-Length:"+String(data.length())+"\r\n";
  
  while(!sendDataLength(tcpHeader,path,servername,data)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Sending Data length Failed!");
      return "Error: 9";  //Wi-Fi Sending Data length Failed!
            }
                         }
  counter = 0;
  while((response = sendData(tcpHeader,data)) == ""){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Data Sending Failed!");
      return "Error: 10";  //Wi-Fi Data Sending Failed!
            }
                    }

  return response;
                                                                                    }

bool sleep(int time){
  int x;
  Serial1.println("AT+GSLP="+String(time));
  Serial1.flush();
  String response = Serial1.readStringUntil(Serial1.available());
  if(response.length() > 2){
    x = findString("OK", response);
                           } 
  else{
    SerialUSB.println("Wi-Fi Sleeping Timeout!");
    return false;
      }

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

String wakeupSendSleep(String APname,String APpassword,String data,String path,String servername,String datatype,int port, String* res){
  String str = "";
  String response;
  wifi_init();
  int counter = 0;
  wakeup();
  String APconnect = wifiInit(APname, APpassword);
  if(APconnect == "OK"){
  str = connectAndSend(data,path,servername,datatype,port,res);

  while(!sleep(1000)){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi sleeping Failed!");
      return "Error: 13";  //Wi-Fi sleeping Failed!
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
      SerialUSB.println("Wi-Fi Connection to Server Failed!");
      return "Error: 8";  //Wi-Fi Connection to Server Failed!
            }
                         }

  response = sendAndReceive(data,path,servername,datatype);
  response = response + "\n" + sendAndReceive(data,path,servername,datatype);

  counter = 0;
  while(!closeConnection()){
    counter++;
    if(counter>=5){
      SerialUSB.println("Wi-Fi Server Disconnection Failed!");
      return "Error: 11";  //Wi-Fi Server Disconnection Failed!
            }
                    }

  return response;
                                                                                            }
