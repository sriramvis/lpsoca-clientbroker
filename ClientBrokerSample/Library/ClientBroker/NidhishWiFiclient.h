String connectandsend(String data,String path,String servername){
  firse:
  char a;
 int i=0;
 bool firse=false;
 

 while(firse==false){ 
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
     firse=true;
      break;
  }
  if(a=='D'){
    firse=false;
    break;
  }
     
    }
    if(firse==true){
      break;
    }

 }
    i=0;

 
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
   Serial1.begin(115200);
  SerialUSB.println("Serial1 begin");

  
  Serial1.println("AT");
 while(true){
   if(Serial1.available()){
    a=Serial1.read();
    SerialUSB.print(a);
    }
    if(a=='O'){
      i=1;
    }
    else if(a=='K'&&i==1){
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


 bool again = false;
 i=0;



  while(again==false){
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
      again=true;
      break;
    }
    if(a=='L'){
      again=false;
      break;
    }
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

 String wakesendsleep(String Networkname,String password,String data,String path,String servername){
 Serial1.begin(115200);
  char a;
 int i=0;

 pinMode(36,OUTPUT);
 digitalWrite(36,HIGH);
 for(int l=0;l<1000000;l++){
asm("nop\r\n");}
digitalWrite(36,LOW);
for(int l=0;l<1000000;l++){
asm("nop\r\n");}
digitalWrite(36,HIGH);



while(true){
       if(Serial1.available()){
    a=Serial1.read();
   // SerialUSB.print(a);
    }
    if(a=='i'){
      i=1;
    }
    if(a=='d'&&i==1){
     i=3;
      break;
  }
     
    }
    i=0;

Wifi_init(Networkname,password);
 String str = connectandsend(data,path,servername);

    i=0;
     Serial1.println("AT+GSLP=10000");

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
//    delay(150);
SerialUSB.println("");
 return str;
}
 