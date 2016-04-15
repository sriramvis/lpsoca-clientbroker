#include "clientBuffer.h"

#define QUEUE_SIZE 10
#define BUFF_SIZE 1 // #number of messages to buffer before sending
#define STACK_SIZE 800
#define JBUFF_SIZE 500 // Buffer size for JSON decoding

#define DEBUG 0
#if DEBUG
  #define vDebug(x) SerialUSB.print(x)
  #define vDebugln(x) SerialUSB.println(x)
#else
  #define vDebug(x)
  #define vDebugln(x)
#endif

QueueHandle_t xSendQueue = NULL;
QueueHandle_t xReceiveQueue = NULL;

QueueHandle_t _receiveQueues[MAX_PORTS];

void jsonDecode(char json[]);
void jsonAndSend(payload_t buffMsgs[], String pdid, String appid);
String connectandsend(String data,String path,String servername);
void Wifi_init(String Networkname, String password);
void registerWithServer(String serverName, String port, String appId, String dueId);
static void sendTask(void *arg);


static String *_appID;
static String *_powerID;

#define CAL_FACTOR (F_CPU/7000)
static void delayMS(uint32_t millis) {
  uint32_t iterations = millis * CAL_FACTOR;
  uint32_t i;
  for(i = 0; i < iterations; ++i) {
    asm volatile("nop\n\t");
  }
}

static void sendTask(void *arg) {
  vDebugln("sendTask started");

  payload_t buffMsgs[BUFF_SIZE];

  int counter = 0;
  payload_t toSend;

  while (1) {
    //SerialUSB.println("Starting While loop in send");
    //payload_t toSend;
    xQueueReceive(xSendQueue, &toSend, portMAX_DELAY);
    vDebugln("Received from Queue");
    vDebugln(toSend.port);
    buffMsgs[counter] = toSend;

    if(counter >= BUFF_SIZE - 1) {
      counter = 0; // reset counter
      vDebugln("About to call jsonAndSend");
      jsonAndSend(buffMsgs, *_powerID, *_appID);
      // Send the messages
    } else {
      vDebugln("Increasing Counter");
      counter++;
    }
  }
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

void clientBuffer::initialize(String appID, String powerID, String net, String pass)
{
  _appID = new String(appID);
  _powerID = new String(powerID);
  xSendQueue = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
  //xReceiveQueue = xQueueCreate(QUEUE_SIZE, sizeof(String));
  Wifi_init(net, pass);
  registerWithServer("192.168.43.12", "3001", appID, powerID);
  xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);
  // Create send and Receive Tasks  

  // clear all ports
  for(int i=0; i < MAX_PORTS; i++){
    _receiveQueues[i] = NULL;
  }
}

void clientBuffer::publish(int port, char *Message)
{
  payload_t payload;

  payload.port = port;
  payload.message = new String(Message);

  if(xQueueSendToBack(xSendQueue, &payload, portMAX_DELAY) != pdTRUE) {
    vDebugln("Unable to place on Queue. Queue full");
    // Deal with this case
    // If this happens then the queue is full
    // Should never happen though
  }
  vDebugln("Published on Queue");
}

void clientBuffer::subscribe(int port, QueueHandle_t receiveQueue){
  if(port < MAX_PORTS){
    _receiveQueues[port] = receiveQueue;  
  }
}

/************************************
 ***** HELPER FUNCTIONS *************/

void registerWithServer(String serverName, String port, String appId, String dueId){
  char a;
  int i=0;
  char c;
  String path = "/register/" + dueId + "/" + appId;
  
  Serial1.println("AT+CIPSTART=\"TCP\",\"" + serverName + "\"," + port); 
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
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
  delayMS(150);
  String data = "";
  String t1 = "POST "+path+" HTTP/1.1\r\nHost:"+serverName+":" + port +"\r\nContent-Type:application/json\r\nContent-Length:"+String(data.length())+"\r\n";
  //String t3 = "{\"data\":[[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[31,1],[32,3],[33,3],[34,3],[35,3],[36,0],[37,1],[38,2],[39,3],[40,3],[41,3],[42,5],[43,11],[44,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3]],\"sampleRate\":44000}";
  vDebugln(data.length());
  vDebugln(t1.length()+data.length()+4);
  Serial1.print("AT+CIPSEND=");
  Serial1.println(t1.length()+data.length()+4);
  vDebugln("t1:");
  vDebugln(t1);
  while(true){
       if(Serial1.available()){
    a=Serial1.read();
    vDebug(a);
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
      vDebug(a);
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
  vDebugln("");
  vDebugln("Registered!!!");
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
    }
    if(a=='v'){
      x=i;
    }
    if(a=='e'&&i==x+1){
      break;
    }
    i++;
  }
 
  vDebugln("done!!");
  // not expecting any response
  i=0;
  String str;
  // flush response
  while(Serial1.available()){
    a=Serial1.read();
    vDebug(a);
  }
  Serial1.println("AT+CIPCLOSE");
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     i=3;
      break;
    }
  }
  vDebugln("Connection closed");
}

String connectandsend(String data,String path,String servername){
  char a;
  int i=0;
  char c;
  
  Serial1.println("AT+CIPSTART=\"TCP\",\"" + servername + "\",3001");
  //SerialUSB.println("sdfsfdds");
 
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
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
  delayMS(150);
  String t1 = "POST "+path+" HTTP/1.1\r\nHost:"+servername+":3001"+"\r\nContent-Type:application/json\r\nContent-Length:"+String(data.length())+"\r\n";
  //String t3 = "{\"data\":[[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[31,1],[32,3],[33,3],[34,3],[35,3],[36,0],[37,1],[38,2],[39,3],[40,3],[41,3],[42,5],[43,11],[44,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3],[9,3],[10,3],[11,5],[12,11],[13,5],[14,6],[15,7],[16,7],[17,7],[18,7],[19,7],[20,7],[21,7],[22,7],[23,7],[24,7],[25,7],[26,7],[27,7],[28,7],[29,7],[30,7],[0,1],[1,3],[2,3],[3,3],[4,3],[5,0],[6,1],[7,2],[8,3]],\"sampleRate\":44000}";
  vDebugln(data.length());
  vDebugln(t1.length()+data.length()+4);
  Serial1.print("AT+CIPSEND=");
  Serial1.println(t1.length()+data.length()+4);
  vDebugln("t1:");
  vDebugln(t1);
  while(true){
       if(Serial1.available()){
    a=Serial1.read();
    vDebug(a);
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
      vDebug(a);
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
  vDebugln("");
  vDebugln("Working!!!");
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
    }
    if(a=='v'){
      x=i;
    }
    if(a=='e'&&i==x+1){
      break;
    }
    i++;
  }
 
  vDebugln("done!!");
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
  Serial1.println("AT+CIPCLOSE");
  while(true){
    if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
    }
    if(a=='O'){
      i=1;
    }
    if(a=='K'&&i==1){
     i=3;
      break;
    }
  }
  vDebugln("Connection closed");
  return str;
}

void jsonAndSend(payload_t buffMsgs[], String pdid, String appid) {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  // JsonArray& port = root.createNestedArray("port");
  JsonArray& msg = root.createNestedArray("msg");

  for (int i = 0; i < BUFF_SIZE; i++) {
    JsonArray& m = msg.createNestedArray();
    vDebugln(buffMsgs[i].port);
    vDebugln(*(buffMsgs[i].message));
    m.add(buffMsgs[i].port);
    m.add(*(buffMsgs[i].message));
  }

  char buffer[JBUFF_SIZE];

  root.printTo(buffer, sizeof(buffer));

  vDebugln("About to send json up");
  vDebugln(buffer);

  // String response = connectandsend(String(buffer), "/power/45", "54.191.239.210");
  String endpoint = String("/up/");
  endpoint.concat(pdid);
  endpoint.concat("/");
  endpoint.concat(appid);
  String response = connectandsend(String(buffer), endpoint, "192.168.43.12");

  // Free here
  for (int i = 0; i < BUFF_SIZE; i++) {
    delete (buffMsgs[i].message);
  }

  vDebugln("Received Response. About to decode");

  response.toCharArray(buffer, JBUFF_SIZE);

  jsonDecode(buffer);

}

inline void notifyPortQueue(int port, String message){
  if(port < MAX_PORTS && _receiveQueues[port] != NULL){
    String *m = new String(message);
     if(xQueueSendToBack(_receiveQueues[port], &m, portMAX_DELAY) != pdTRUE ){
        // fail to notify port 
     }
  }
}

void jsonDecode(char json[]) {
  vDebugln("Entering Json Decoding");
  vDebugln(json);
  StaticJsonBuffer<1024> jsonBuffer;
  //String ports[10];
  //String messages[10];
  JsonObject& root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    vDebugln("parseObject() in json decoder failed");
    return;
  } else {
    vDebugln("parsed Object:");
    root.printTo(SerialUSB);

    // get msg array
    JsonArray& msgs = root["msg"];
    int arraySize = msgs.size();
    for(int i=0; i<arraySize; i++){
      JsonArray& m = msgs[i];
      if(m.size() == 2){ // has both port and message
        int port = m[0];
        String message = m[1];
        vDebug("port: ");
        vDebugln(port);
        vDebugln("message: " + message);
        notifyPortQueue(port, message);
      }
    }

  }

  // JsonArray& ports = root["port"].asArray();
  // JsonArray& messages = root["message"].asArray();

  // String pdid = root["pdid"];
  // String appid = root["appid"];

  // SerialUSB.println(pdid);
  // SerialUSB.println(appid);

  // for (JsonArray::iterator it=ports.begin(); it!=ports.end(); ++it) {
  //   String val = *it;
  //   SerialUSB.println(val);
  // }

  // for (JsonArray::iterator it=messages.begin(); it!=messages.end(); ++it) {
  //   String msg = *it;
  //   SerialUSB.println(msg);
  // }

  // Need to create hard copies of these extracted items before sending them off
}

void Wifi_init(String Networkname,String password){
  vDebugln("1");
  char a;
  int i=0;
  pinMode(40,OUTPUT);
  pinMode(36,OUTPUT);
  digitalWrite(40,HIGH);
  digitalWrite(36,HIGH);
  delayMS(50);
  digitalWrite(36,LOW);
  delayMS(500);
  digitalWrite(36,HIGH);
  delayMS(500);
 
  vDebugln("1");
  Serial1.begin(115200);
  vDebugln("Serial1 begin");
  delayMS(1000);
  Serial1.println("AT+RST");
  while(true){
   if(Serial1.available()){
      a=Serial1.read();
      vDebug(a);
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
      vDebug(a);
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
      vDebug(a);
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
      vDebug(a);
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
      vDebug(a);
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
      vDebug(a);
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
 delayMS(150);
 Serial1.println("AT+CWJAP=\""+Networkname+"\",\""+password+"\"");
 while(true){
   if(Serial1.available()){
    a=Serial1.read();
    vDebug(a);
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
      vDebug(a);
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
  
  vDebugln("");
  vDebugln("Connected!!!");
}












