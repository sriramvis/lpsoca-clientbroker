#include "clientBuffer.h"
#include "NidhishWiFiclient.h"

#define NUM_PORTS_MAX 16
#define NUM_BUFFS 2
#define QUEUE_SIZE 20
#define BUFF_SIZE 5 // #number of messages to buffer before sending
#define STACK_SIZE 1024
#define JBUFF_SIZE 400 // Buffer size for JSON decoding
#define MAX_MESSAGE_SIZE 200
#define SEND_NOW_Q_SIZE 5
#define BUFFER_MSG 100
#define SEND_NOW 101
#define A 42
#define B 43

QueueHandle_t xSendToFill = NULL;
QueueHandle_t xToSendTask = NULL;
//QueueHandle_t xToFillTask = NULL;
//QueueHandle_t xSendNowQ = NULL;
QueueHandle_t xFromSendTask = NULL; //sendTask Response Queue

//payload_t buffA[BUFF_SIZE]; // prob need mutexes to access this
//payload_t buffB[BUFF_SIZE];

/* 0 -> buffer a
 * 1 -> buffer b
 */
payload_t mBuffer[NUM_BUFFS][BUFF_SIZE];

payload_t msgNow[1];

static QueueHandle_t queueSubArray[NUM_PORTS_MAX];

void jsonDecode(char json[]);
void jsonAndSend(payload_t buffMsgs[], String appID, String powerID, bool isNow);
static void sendTask(void *arg);
static void fillTask(void *arg);
//void readFromBuffA();
//void readFromBuffB();
void readFromBuffer(int idx);
void sendMessageNow(payload_t *payload);
String jsonACK();

static void fillTask(void *arg) {
    SerialUSB.println("fillTask started");
    int idx = 0;
    bool AReady = true;
    bool BReady = true;
    bool ATurn = true;
    bool BTurn = false;
    int response;

    payload_t msg;

    while (1) {
        xQueueReceive(xSendToFill, &msg, portMAX_DELAY);
        /*
        SerialUSB.print("AReady: ");
        SerialUSB.println(AReady);
        SerialUSB.print("BReady: ");
        SerialUSB.println(BReady);
        SerialUSB.print("ATurn: ");
        SerialUSB.println(ATurn);
        SerialUSB.print("BTurn: ");
        SerialUSB.println(BTurn);
        */

        if(!AReady && !BReady) {
            xQueueReceive(xFromSendTask, &response, portMAX_DELAY);
            if (response == A) {
                AReady = true;
            } else if (response == B) {
                BReady = true;
            } else {
                SerialUSB.println("Error: Invalid response");
            }
        } 

        if (AReady && ATurn) {
            mBuffer[0][idx] = msg;
            if (idx == BUFF_SIZE - 1) {
                idx = 0;
                AReady = false;
                ATurn = false;
                BTurn = true;
                msg.buffer = A;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            } else {
                idx++;
            }

        } else if (BReady && BTurn) {
            mBuffer[1][idx] = msg;
            if (idx == BUFF_SIZE - 1) {
                idx = 0;
                BReady = false;
                BTurn = false;
                ATurn = true;
                msg.buffer = B;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            } else {
                idx++;
            }

        } else {
            SerialUSB.println("No ones turn. Should never reach");
        }

    }
}

/* This function will read from Queue
 * And send stuff to server. The pre-alpha version
 * only prints out the values rather than sending them
 */
static void sendTask(void *arg) {

    payload_t msg;
    bool isNow = false;
    const int doneA = A;
    const int doneB = B;

    while (1) {
        xQueueReceive(xToSendTask, &msg, portMAX_DELAY);
        if(msg.type == SEND_NOW) {
            //SerialUSB.println("Sending message now");
            msgNow[0] = msg;
            isNow = true;
            jsonAndSend(msgNow, clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), isNow);
            //SerialUSB.println(*msg.message);
        } else if (msg.buffer == A) {
            //readFromBuffer(0); // A
            isNow = false;
            jsonAndSend(mBuffer[0], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), isNow);
            xQueueSendToBack(xFromSendTask, &doneA, portMAX_DELAY);
        } else if (msg.buffer == B) {
            //readFromBuffer(1); // B
            isNow = false;
            jsonAndSend(mBuffer[1], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), isNow);
            xQueueSendToBack(xFromSendTask, &doneB, portMAX_DELAY);
        } else {
            SerialUSB.println("Should not reach here");
        }
    }

}

/*
// For debugging only
void readFromBuffer (int idx) {
    for (int i = 0; i < BUFF_SIZE; i++) {
        SerialUSB.println(*(mBuffer[idx][i].message));
        delete(mBuffer[idx][i].message);
    }
} */


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

/* Will send message immediately and then send other messages that are currently buffered*/
//void clientBuffer::publishNow(int port, char *message) {

//}

void clientBuffer::initialize(String appID, String powerID, String net, String pass)
{
    _appID = new String(appID);
    _powerID = new String(powerID);
    xSendToFill = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
    xToSendTask = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
    xFromSendTask = xQueueCreate(QUEUE_SIZE, sizeof(int));
    Wifi_init(net, pass);
    xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(fillTask, NULL, STACK_SIZE, NULL, 1, NULL);
    /* Note the register call is commented out as for testing we are just
     * echoing messages sent up the server
     */
    //String toSend = "/register/" + *_appID+"/"+*_powerID;
    //SerialUSB.println("About to send register info");
    //SerialUSB.println(toSend);
    //String msgToSend = "{\"darklord\": \"Darklord's Will\"}";
    //String response = connectandsend(msgToSend, toSend, "54.191.239.210"); //find out exact format from Vinod
    //SerialUSB.println("Finished Sending reg info");
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
    payload.type = BUFFER_MSG;
    payload.message = new String(Message);


    if(xQueueSendToBack(xSendToFill, &payload, portMAX_DELAY) != pdTRUE) {
        SerialUSB.println("Unable to place on Queue. Queue full");
    }
    //SerialUSB.println("Published Message");
}

/* Publishes message immediately. No Buffering */
void clientBuffer::publishNow(int port, char *message)
{
    payload_t payload;
    payload.port = port;
    payload.message = new String(message);
    payload.type = SEND_NOW;

    if (xQueueSendToBack(xToSendTask, &payload, portMAX_DELAY) != pdTRUE) {
        SerialUSB.print("Something went horribly wrong.");
        SerialUSB.print(" Unable to send message at the current time.\n");
    }

    // pass onto Queue

}

void clientBuffer::subscribe(int port, QueueHandle_t queueSub) {
    queueSubArray[port] = queueSub; //queueSub handle is created by Will/Cef
}

/************************************
 ***** HELPER FUNCTIONS *************/

/* 
String jsonACK() {

    StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["MaxSize"] = 200;
    root["NumMessages"] = 3;
    root["ACK"] = 1;
    JsonArray& msg = root.createNestedArray("msg");
    char buffer[JBUFF_SIZE];
    root.printTo(buffer, sizeof(buffer));
    return String(buffer);
} */

void jsonAndSend(payload_t buffMsgs[], String appID, String powerID, bool isNow) {
    SerialUSB.println("jsonAndSend");
    StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    //root["MaxSize"] = 200;
    //root["NumMessages"] = 3;
    //root["ACK"] = 1;
    JsonArray& msg = root.createNestedArray("msg");
    //SerialUSB.println("Created json Buffer");

    if(!isNow) {
        for (int i = 0; i < BUFF_SIZE; i++) {
            JsonArray &m = msg.createNestedArray();
            m.add(buffMsgs[i].port);
            m.add(*(buffMsgs[i].message));
        }
    }
    else if(isNow) {
        JsonArray &m = msg.createNestedArray();
        m.add(buffMsgs[0].port);
        m.add(*(buffMsgs[0].message));
    }
    else {
        //should never reach here
        SerialUSB.println("isNow is neither true nor false. Schrodinger's cat, basically.");
    }

    //SerialUSB.println("Done with for loop");

    char buffer[JBUFF_SIZE];

    root.printTo(buffer, sizeof(buffer));

    //SerialUSB.print("Message to be sent: ");
    //SerialUSB.println(buffer);

    //send here with Nidhish's new send function
    //String response = connectandsend(String(buffer), "/up/" + appID + "/" + powerID, "54.191.239.210","net","pass","application/json",80); //instead of application/json can also use text/plain and others...
    String response = connectandsend(String(buffer), "/echo", "54.191.239.210", "Cameron's iPhone", "gilfoyle", "application/json", 80);

    //SerialUSB.println("After connect and Send in JSON");
    if(!isNow) {
        for (int i = 0; i < BUFF_SIZE; i++) {
            delete (buffMsgs[i].message);
        }
    }
    else if(isNow){
        //what to do here? MsgNow has not been malloced
        delete(buffMsgs[0].message);
    }
    else {
        //should never reach here
        SerialUSB.println("isNow is neither true nor false. Schrodinger's cat, basically.");
    }

    SerialUSB.println("Received Response. About to decode");

    //response.toCharArray(buffer, JBUFF_SIZE);

    jsonDecode(buffer);

}

void jsonDecode(char json[]) {
    SerialUSB.println("Entering Json Decoding");
    //SerialUSB.println(json);
    StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);

    if (!root.success()) {
        SerialUSB.println("parseObject() in json decoder failed");
        return;
    }

    JsonArray& msgs = root["msg"].asArray();
    int more = root["More"];
    int port;

    for (JsonArray::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
        JsonVariant val = *it;
        JsonArray& a = val.asArray();
        port = a[0];
        String message_port = a[1];
        //SerialUSB.println(message_port);
        String *message = new String (message_port); //Will be deleted by Cef/Will
        xQueueSendToBack(queueSubArray[port], &message, portMAX_DELAY); // this could be an issue
    }

    //String ACK = jsonACK();
    /*
    if (!more) {
        SerialUSB.println("Sleeping");
    } else {
        SerialUSB.println("More messages incoming");
    } */

}








