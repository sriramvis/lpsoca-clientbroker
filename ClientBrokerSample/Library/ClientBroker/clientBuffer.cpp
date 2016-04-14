#include "clientBuffer.h"
#include "NidhishWiFiclient.h"

#define NUM_PORTS_MAX 16
#define QUEUE_SIZE 10
#define BUFF_SIZE 5 // #number of messages to buffer before sending
#define STACK_SIZE 800
#define JBUFF_SIZE 400 // Buffer size for JSON decoding

QueueHandle_t xSendQueue = NULL;
QueueHandle_t xReceiveQueue = NULL;

static QueueHandle_t queueSubArray[NUM_PORTS_MAX];

void jsonDecode(char json[]);
void jsonAndSend(payload_t buffMsgs[]);
//String connectandsend(String data,String path,String servername);
//void Wifi_init(String Networkname, String password);
static void sendTask(void *arg);


static void sendTask(void *arg) {
  SerialUSB.println("sendTask started");

	payload_t buffMsgs[BUFF_SIZE];

	int counter = 0;
  payload_t toSend;

	while (1) {
    //SerialUSB.println("Starting While loop in send");
		//payload_t toSend;
		xQueueReceive(xSendQueue, &toSend, portMAX_DELAY);
    //SerialUSB.println("Received from Queue");
		//SerialUSB.println(*(toSend.port));
		buffMsgs[counter] = toSend;

		if(counter == BUFF_SIZE - 1) {
			counter = 0; // reset counter
      //SerialUSB.println("About to call jsonAndSend");
			jsonAndSend(buffMsgs);
			// Send the messages
		} else {
      //SerialUSB.println("Increasing Counter");
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
	xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);
  //initQueueSubArray();
	// Create send and Receive Tasks	
}


void clientBuffer::publish(int port, char *Message)
{
	payload_t payload;

	payload.port = new String(port);
	//payload.appid = _appID;
	//payload.pdid = _powerID;
	payload.message = new String(Message);


	if(xQueueSendToBack(xSendQueue, &payload, 0) != pdTRUE) {
		SerialUSB.println("Unable to place on Queue. Queue full");
		// Deal with this case
		// If this happens then the queue is full
		// Should never happen though
	}
  //SerialUSB.println("Published on Queue");
}

void clientBuffer::subscribe(int port, QueueHandle_t queueSub) {
  // Queue takes char*
  // Make sure char* doesn't get overwritten before its read
  // Add another method that Will can call 
  // Create register when they call this

  queueSubArray[port] = queueSub;

}

/************************************
 ***** HELPER FUNCTIONS *************/


void jsonAndSend(payload_t buffMsgs[]) {
  StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  JsonArray& msg = root.createNestedArray("msg");

  for (int i = 0; i < BUFF_SIZE; i++) {
    JsonArray &m = msg.createNestedArray();
    m.add(*(buffMsgs[i].port));
    m.add(*(buffMsgs[i].message));
  }

  char buffer[JBUFF_SIZE];

  root.printTo(buffer, sizeof(buffer));

  SerialUSB.println("About to send json up");
  SerialUSB.println(buffer);

  String response = connectandsend(String(buffer), "/power/45", "10.42.0.1");

  // Free here

  for (int i = 0; i < BUFF_SIZE; i++) {
    delete (buffMsgs[i].port);
    delete (buffMsgs[i].message);
  }

  SerialUSB.println("Received Response. About to decode");

  response.toCharArray(buffer, JBUFF_SIZE);
  SerialUSB.println(buffer);

  jsonDecode(buffer);

}

void jsonDecode(char json[]) {
  SerialUSB.println("Entering Json Decoding");
  SerialUSB.println(json);
  StaticJsonBuffer<500> jsonBuffer;
  //String ports[10];
  //String messages[10];
  JsonObject& root = jsonBuffer.parseObject(json);

  if (!root.success()) {
    SerialUSB.println("parseObject() in json decoder failed");
    return;
  }

  JsonArray& msgs = root["msg"].asArray();

  //String pdid = root["pdid"];
  //String appid = root["appid"];

  //SerialUSB.println(pdid);
  //SerialUSB.println(appid);

  for (JsonArray::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    JsonVariant val = *it;
    JsonArray& a = val.asArray();
    int port = a[0];
    //switch-case block for port number here
    String message_port = a[1];
    //String *message = new String (message_port);
    //buffers for string data here
    SerialUSB.println(port);
    SerialUSB.println(message_port);
  }


  // Need to create hard copies of these extracted items before sending them off
}













