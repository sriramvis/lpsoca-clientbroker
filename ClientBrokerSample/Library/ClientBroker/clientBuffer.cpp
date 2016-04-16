#include "clientBuffer.h"
#include "NidhishWiFiclient.h"

#define NUM_PORTS_MAX 16
#define QUEUE_SIZE 10
#define BUFF_SIZE 5 // #number of messages to buffer before sending
#define STACK_SIZE 1024
#define JBUFF_SIZE 400 // Buffer size for JSON decoding

QueueHandle_t xSendQueue = NULL;
QueueHandle_t xReceiveQueue = NULL;

static QueueHandle_t queueSubArray[NUM_PORTS_MAX];

void jsonDecode(char json[]);
void jsonAndSend(payload_t buffMsgs[], String appID, String powerID);
static void sendTask(void *arg);


static void sendTask(void *arg) {
    SerialUSB.println("sendTask started");

    payload_t buffMsgs[BUFF_SIZE];

    int counter = 0;
    payload_t toSend;

    while (1) {
        xQueueReceive(xSendQueue, &toSend, portMAX_DELAY);
        buffMsgs[counter] = toSend;

        if(counter == BUFF_SIZE - 1) {
            counter = 0; // reset counter
            jsonAndSend(buffMsgs, clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID());
        } else {
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
    Wifi_init(net, pass);
    xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);	
}

String clientBuffer::getAppID() {
    return *_appID;
}

String clientBuffer::getPowerID() {
    return *_powerID;
}


void clientBuffer::publish(int port, char *Message)
{
    payload_t payload;

    payload.port = port;
    payload.message = new String(Message);


    if(xQueueSendToBack(xSendQueue, &payload, portMAX_DELAY) != pdTRUE) {
        SerialUSB.println("Unable to place on Queue. Queue full");
    }
    SerialUSB.println("Published on Queue");
}

void clientBuffer::subscribe(int port, QueueHandle_t queueSub) {
    queueSubArray[port] = queueSub; //queueSub handle is created by Will/Cef
    String port_s = String(port);
    String toSend = "/register/" + *_appID+"/"+port_s+"/"+*_powerID;
    SerialUSB.println("About to send register info");
    SerialUSB.println(toSend);
    String msgToSend = "{\"darklord\": \"Darkload's Load\"}";
    //String response = connectandsend(msgToSend, toSend, "54.191.239.210"); //find out exact format from Vinod
    SerialUSB.println("Finished Sending reg info");


}

/************************************
 ***** HELPER FUNCTIONS *************/


void jsonAndSend(payload_t buffMsgs[], String appID, String powerID) {
    SerialUSB.println("jsonAndSend Decode");
    StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonArray& msg = root.createNestedArray("msg");
    SerialUSB.println("Created json Buffer");

    for (int i = 0; i < BUFF_SIZE; i++) {
        JsonArray &m = msg.createNestedArray();
        m.add(buffMsgs[i].port);
        m.add(*(buffMsgs[i].message));
    }

    SerialUSB.println("Done with for loop");

    char buffer[JBUFF_SIZE];

    root.printTo(buffer, sizeof(buffer));

    SerialUSB.print("Message to be sent: ");
    SerialUSB.println(buffer);

    String response = connectandsend(String(buffer), "/up/" + appID + "/" + powerID, "54.191.239.210");

    SerialUSB.println("After connect and Send in JSON");

    for (int i = 0; i < BUFF_SIZE; i++) {
        delete (buffMsgs[i].message);
    }

    SerialUSB.println("Received Response. About to decode");

    response.toCharArray(buffer, JBUFF_SIZE);

    jsonDecode(buffer);

}

void jsonDecode(char json[]) {
    SerialUSB.println("Entering Json Decoding");
    SerialUSB.println(json);
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
        SerialUSB.println("parseObject() in json decoder failed");
        return;
    }

    JsonArray& msgs = root["msg"].asArray();
    int port;


    for (JsonArray::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
        JsonVariant val = *it;
        JsonArray& a = val.asArray();
        port = a[0];
        String message_port = a[1];
        String *message = new String (message_port); //Will be deleted by Cef/Will
        xQueueSendToBack(queueSubArray[port], &message, portMAX_DELAY);
    }

}













