#include "clientBuffer.h"
#include "NidhishWiFiclient.h"

#define NUM_PORTS_MAX 16
#define NUM_BUFFS 2
#define QUEUE_SIZE 20
#define BUFF_SIZE 10 // #number of messages to buffer before sending
#define STACK_SIZE 1024
#define JBUFF_SIZE 400 // Buffer size for JSON decoding
#define MAX_MESSAGE_SIZE 200
#define SEND_NOW_Q_SIZE 5
#define BUFFER_MSG 100
#define SEND_NOW 101
#define A 42
#define B 43

#define RED 6
#define GREEN 7
#define BLUE 8


QueueHandle_t xSendToFill = NULL;
QueueHandle_t xToSendTask = NULL;
QueueHandle_t xFromSendTask = NULL; //sendTask Response Queue

//payload_t buffA[BUFF_SIZE]; // prob need mutexes to access this
//payload_t buffB[BUFF_SIZE];

/* 0 -> buffer a
 * 1 -> buffer b
 */
payload_t mBuffer[NUM_BUFFS][BUFF_SIZE];

static QueueHandle_t queueSubArray[NUM_PORTS_MAX];

//void jsonDecode(char json[]);
//void jsonAndSend(payload_t buffMsgs[], String appID, String powerID, int index);
void encodeAndSend(payload_t bufMsgs[], String appID, String powerID, int numItems) ;
static void sendTask(void *arg);
static void fillTask(void *arg);
//void readFromBuffA(int buff, int items);
//void readFromBuffB();
void readFromBuffer(int idx, int items);
void sendMessageNow(payload_t *payload);
String jsonACK();

void encodeAndSend(payload_t bufMsgs[], String appID, String powerID, int numItems) {

    //int spaces = uxQueueSpacesAvailable();
    const char base = '!';
    String AppendedMsg="";
    word length_msg;
    char port_bitwise;
    for(int i=0;i<numItems;i++) {
        // port_bitwise = char(buffMsgs[i].port);
        // AppendedMsg += port_bitwise;
        // length_msg = word((*(bufMsgs[i].message)).length());
        // AppendedMsg += String(length_msg);
        // ApendedMsg += (*(bufMsgs[i].message));
        SerialUSB.println(buffMsgs[i].port);
        port_bitwise = base + buffMsgs[i].port; 
        //SerialUSB.println(port_bitwise);
        AppendedMsg += port_bitwise;
        length_msg = word((*(buffMsgs[i].message)).length());
        length_bitwise = base + length_msg;
        AppendedMsg += length_bitwise;
        //AppendedMsg += String(length_msg);
        AppendedMsg += (*(buffMsgs[i].message));
    }

    String response = connectAndSend(AppendedMsg, "/more", "54.191.239.210", "text/plain", 80);

    if(response.startsWith("1")) {
        //more messages to come
        SerialUSB.println("Starts with 1, more to come");
    }
    else if(response.startsWith("0")) {
        //no more to come 
        SerialUSB.println("Starts with 0, no more to come");
    }
    else {
        //should never reach here if server is sane
        SerialUSB.println("Server insane");
    }
}


static void fillTask(void *arg) {
    //SerialUSB.println("fillTask started");
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

        /* Should just need to check for who's turn it is */
        /* if past this point, one buffer is being filled */

        if (AReady && ATurn) {
            mBuffer[0][idx] = msg;
            if ((msg.type == SEND_NOW) || (idx == BUFF_SIZE - 1)) {
                /* Flush Buffer A */
                msg.buffer = A;
                msg.numItems = idx + 1;
                idx = 0;
                ATurn = false;
                AReady = false;
                BTurn = true;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            }
            /*
            if (msg.type == SEND_NOW) {
                /* Send everything in the buffer
                msg.buffer = A;
                msg.idx = idx;
                idx = 0;
                Aturn = false;
                AReady = false;
                BTurn = true;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            } else if (idx == BUFF_SIZE - 1) {
                msg.idx = idx;
                idx = 0;
                AReady = false;
                ATurn = false;
                BTurn = true;
                msg.buffer = A;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
                */
            else {
                idx++;
            }

        } else if (BReady && BTurn) {
            mBuffer[1][idx] = msg;
            if ((msg.type == SEND_NOW) || (idx == BUFF_SIZE - 1)) {
                msg.buffer = B;
                msg.numItems = idx + 1;
                idx = 0;
                BTurn = false;
                BReady = false;
                ATurn = true;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            }
            /*
            if (msg.type == SEND_NOW) {
                msg.buffer = B;
                msg.idx = idx;
                idx = 0;
                Bturn = false;
                BReady = false;
                ATurn = true;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
            } else if (idx == BUFF_SIZE - 1) {
                idx = 0;
                BReady = false;
                BTurn = false;
                ATurn = true;
                msg.buffer = B;
                xQueueSendToBack(xToSendTask, &msg, portMAX_DELAY);
                */
            else {
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
    //bool isNow = false;
    const int doneA = A;
    const int doneB = B;

    while (1) {
        xQueueReceive(xToSendTask, &msg, portMAX_DELAY);
        if (msg.buffer == A) {
            readFromBuffer(0, msg.numItems); // A
            //isNow = false;
            //jsonAndSend(mBuffer[0], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), isNow);
            encodeAndSend(mBuffer[0], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), msg.numItems);
            xQueueSendToBack(xFromSendTask, &doneA, portMAX_DELAY);
        } else if (msg.buffer == B) {
            readFromBuffer(1, msg.numItems); // B
            //isNow = false;
            //jsonAndSend(mBuffer[1], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), isNow);
            encodeAndSend(mBuffer[1], clientBuffer::getInstance()->getAppID(), clientBuffer::getInstance()->getPowerID(), msg.numItems);
            xQueueSendToBack(xFromSendTask, &doneB, portMAX_DELAY);
        } else {
            //SerialUSB.println("Should not reach here");
        }
    }
}


// For debugging only
void readFromBuffer (int idx, int items) {
    for (int i = 0; i < items; i++) {
        SerialUSB.println(*(mBuffer[idx][i].message));
        delete(mBuffer[idx][i].message);
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

/* Shall send message immediately and then send other messages that are currently buffered*/
//void clientBuffer::publishNow(int port, char *message) {

//}

void clientBuffer::initialize(String appID, String powerID, String net, String pass)
{
    _appID = new String(appID);
    _powerID = new String(powerID);
    xSendToFill = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
    xToSendTask = xQueueCreate(QUEUE_SIZE, sizeof(payload_t));
    xFromSendTask = xQueueCreate(QUEUE_SIZE, sizeof(int));
    //Wifi_init(net, pass);
    xTaskCreate(sendTask, NULL, STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(fillTask, NULL, STACK_SIZE, NULL, 1, NULL);
    String toSend = "/register/" + *_appID+"/"+*_powerID;//tell Vinod to set up for all ports!
    //SerialUSB.println("About to send register info");
    //SerialUSB.println(toSend);
    String msgToSend = "{\"darklord\": \"Darklord's Will\"}";
    //String response = connectandsend(msgToSend, toSend, "54.191.239.210"); //find out exact format from Vinod
    //SerialUSB.println("Finished Sending reg info");
}


String clientBuffer::getAppID() {
    return *_appID;
}

String clientBuffer::getPowerID() {
    return *_powerID;
}


void clientBuffer::publish(int port, char *message)
{
    publish(port, message, false);
}

void clientBuffer::publish(int port, char *message, bool pubNow) {

    payload_t payload;

    payload.port = port;
    if (pubNow) {
        payload.type = SEND_NOW;
    } else {
        payload.type = BUFFER_MSG;
    }
    payload.message = new String(message);

    if(xQueueSendToBack(xSendToFill, &payload, portMAX_DELAY) != pdTRUE) {
        SerialUSB.println("Unable to place on Queue. Queue full");
    }

}

/* Publishes message immediately. No Buffering */

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

/*void jsonAndSend(payload_t buffMsgs[], String appID, String powerID, int index) {
    //SerialUSB.println("jsonAndSend");
    StaticJsonBuffer<JBUFF_SIZE> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
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

    //SerialUSB.println("After connect and Send in JSON"); if(!isNow) {
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

} */

/*void jsonDecode(char json[]) {
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
    } 

} */








