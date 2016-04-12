#include "clientBuffer.h"

#define QUEUE_SIZE 10
#define BUFF_SIZE 5 // #number of messages to buffer before sending
#define STACK_SIZE 600

QueueHandle_t xSendQueue = NULL;
QueueHandle_t xReceiveQueue = NULL;

/* These functions are part of NIDISH'S library
 * We just copied and pasted them hear, since we didn't have
 * his final library. Will later replace these functions with calls to
 * NIDish's library
 */
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
  }
  i=0;
  delay(150);
  String t1 = "POST "+path+" HTTP/1.1\r\nHost:"+servername+":80"+"\r\nContent-Type:application/json\r\nContent-Length:"+String(data.length())+"\r\n";
  //String t3 = "{\"data\":[[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[31,1],[32,3],[33,3],[34,3],[35,3],[36,0],[37,1],[38,2],[39,3],[40,3],[41,3],[42,5],[43,11],[44,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3]],\"sampleRate\":44000}";
  SerialUSB.println(data.length());
  SerialUSB.println(t1.length()+data.length()+4);
  Serial1.print("AT+CIPSEND=");
  Serial1.println(t1.length()+data.length()+4);
  SerialUSB.println("t1:");
  SerialUSB.println(t1);
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
        str+=a;
      }
    }
    if(a=='}'){
      break;
    }
    i++;
    
     
  }
  return str;
}

void jsonAndSend(payload_t buffMsgs[], String pdid, String appid) {
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	root["pdid"] = pdid;
	root["appid"] = appid;

	JsonArray& port = root.createNestedArray("port");
	JsonArray& msg = root.createNestedArray("message");

	for (int i = 0; i < BUFF_SIZE; i++) {
		port.add(buffMsgs[i].port);
		msg.add(buffMsgs[i].message);
	}

	char buffer[256];

	root.printTo(buffer, sizeof(buffer));

	connectandsend(String(buffer), "/power/45", "10.42.0.1");

}


static void sendTask(void *arg) {

	payload_t buffMsgs[BUFF_SIZE];

	int counter = 0;

	while (1) {
		payload_t toSend;
		xQueueReceive(xSendQueue, &toSend, portMAX_DELAY);
		SerialUSB.println(toSend.port);
		buffMsgs[counter] = toSend;

		if(counter == BUFF_SIZE - 1) {
			counter = 0; // reset counter
			jsonAndSend(buffMsgs, toSend.pdid, toSend.appid);
			// Send the messages
		} else {
			counter++;
		}

	}

}

void Wifi_init(String Networkname,String password){
  SerialUSB.println("1");
  char a;
  int i=0;
  pinMode(40,OUTPUT);
  pinMode(36,OUTPUT);
  digitalWrite(40,HIGH);
  digitalWrite(36,HIGH);
  delay(50);
  digitalWrite(36,LOW);
  delay(500);
  digitalWrite(36,HIGH);
  delay(500);
 
  SerialUSB.println("1");
  Serial1.begin(115200);
  SerialUSB.println("Serial1 begin");
  delay(1000);
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
    }
    else if(a=='e'&&i==1){
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
    }
    else if(a=='e'&&i==1){
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
    } 
    else if(a=='e'){
      i=1;
    }
    if(a=='K'&&i==1){
      i=3;
      break;
    } 
    else if(a=='d'){
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
 delay(150);
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


clientBuffer::clientBuffer()
{
	_appID = NULL;
	_powerID = NULL;
}

clientBuffer *clientBuffer::_instance = NULL;

clientBuffer *clientBuffer::getInstance()
{
	if (_instance == NULL) {
		_instance = new clientBuffer;
	}
	return _instance;
}

void clientBuffer::initialize(String appID, String powerID)
{
	_appID = new String(appID);
	_powerID = new String(powerID);
	xSendQueue = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
	//xReceiveQueue = xQueueCreate(QUEUE_SIZE, sizeof(String));
  Wifi_init("lpsoc", "lpsoc2016");
	xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);
	// Create send and Receive Tasks	
}

void clientBuffer::publish(String port, char *Message)
{
	payload_t payload;

	payload.port = port;
	payload.appid = *_appID;
	payload.pdid = *_powerID;
	payload.message = String(Message);


	if(xQueueSendToBack(xSendQueue, &payload, 0) != pdTRUE) {
		SerialUSB.println("Unable to place on Queue. Queue full");
		// Deal with this case
		// If this happens then the queue is full
		// Should never happen though
	}
}












