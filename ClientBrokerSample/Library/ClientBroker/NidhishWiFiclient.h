//#ifndef NidhishWiFiclient_h
//#define NidhishWiFiclient_h
#include "Arduino.h"


String connectandsend(String data,String path,String servername){
  char a;
 int i=0;
 char c;
  
 Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\",80");
 //SerialUSB.println("sdfsfdds");
 
 while(true){
       if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     i=3;
      break;
  }
//  if(a=='T'){
//    break;
//  }
     
    }
    i=0;
    //delay(150);
 
  String t1 = "POST "+path+" HTTP/1.1\r\nHost:"+servername+"\r\nContent-Type:application/json\r\nContent-Length:"+String(data.length())+"\r\n";
  SerialUSB.println(data.length());
  SerialUSB.println(t1.length()+data.length()+4);
  Serial1.print("AT+CIPSEND=");
  Serial1.println(t1.length()+data.length()+4);
 
  while(true){
       if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='>'){
      break;
    }
    
     
    }


 
  Serial1.println(t1);
  //Serial1.println(t2);
  Serial1.println(data);
while(true){
       if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     i=3;
      break;
  }
     
    }
    i=0;
    int x=0;
    SerialUSB.println("");
    SerialUSB.println("Working!!!");
    while(true){
       if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='v'){
      
      x=i;
    }
    if(a=='e'&&i==x+1){
      
     
      break;

      
  }
    
     i++;
    }
 
    SerialUSB.println("done!!");
     i=0;
     String str;
     while(true){
       if(Serial1.available()){
    a=Serial1.read();
    if(a!='\n'){
    str+=a;}
    
    }
    if(a=='}'){
      break;
    }
    i++;
    
     
    }

   
    
   
//    delay(150);
//  
//  
 Serial1.println("AT+CIPCLOSE");

 while(true){
       if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     i=3;
      break;
  }
     
    }
    SerialUSB.println("");
//    delay(150);
 return str;
}


void Wifi_init(String Networkname,String password){
  char a;
 int i=0;
// pinMode(40,OUTPUT);
//  pinMode(36,OUTPUT);
//  digitalWrite(40,HIGH);
//  digitalWrite(36,HIGH);
//  delay(50);
//  digitalWrite(36,LOW);
//  delay(500);
//  digitalWrite(36,HIGH);
//  delay(500);
 
 
   Serial1.begin(115200);
  SerialUSB.println("Serial1 begin");
//  delay(1000);

  
  Serial1.println("AT");
 while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }
     
    }
    
 
 i=0;
  Serial1.println("AT+GMR");
 while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }
     
    }
 
 i=0;  
  Serial1.println("AT+CWMODE=1");
   while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    
    if(a=='O'){
      i=1;
    }
    else if(a=='g'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }else if(a=='e'&&i==1){
    i=3;
    break;
  }
     
    }
    i=0;  
  Serial1.println("AT+CIPMODE=0");
   while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    
    if(a=='O'){
      i=1;
    }
    else if(a=='g'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }else if(a=='e'&&i==1){
    i=3;
    break;
  }
     
    }
 
 i=0;
  Serial1.println("AT+CIPMUX=0");
    while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    
    if(a=='O'){
      i=1;
    } else if(a=='e'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  } else if(a=='d'){
      i=3;
      break;
    }

     
    }
  i=0;
  
   Serial1.println("AT+CWQAP");
    while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }
     
    }
 again:
 i=0;


  
  Serial1.println("AT+CWJAP=\""+Networkname+"\",\""+password+"\"");
 while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     
      i=3;
      break;
    }
    if(a=='L'){
      goto again;
    }
 }
i=0;
 Serial1.println("AT+CIFSR");
  while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
  }
     
    }
    i=0;

    SerialUSB.println("");
    SerialUSB.println("Connected!!!");
    
 }

 //#endif