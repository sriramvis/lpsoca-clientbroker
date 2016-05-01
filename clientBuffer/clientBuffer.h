#ifndef clientBuffer_h
#define clientBuffer_h

#include <Arduino.h>
#include "FreeRTOS_ARM.h"
#include "ArduinoJson.h"

typedef struct payload_s {
    int port;
    int type; // buffered or non buffered
    int buffer; // which buffer to read from
    int numItems;
    String *message;
} payload_t;

class clientBuffer {

    public:
        static clientBuffer *getInstance();
        void initialize(String appID, String powerID, String net, String pass);
        void publish(int port, char *message);
        void publish(int port, char *message, bool pubNow);
        //void publishNow(int port, char *message);
        void subscribe(int port, QueueHandle_t queueSub);
        String getAppID();
        String getPowerID();
        //void publishNow(int port, char *message);


    protected:
        clientBuffer();

    private:
        static clientBuffer *_instance;
        //void publish(int port, char *message, bool pubNow);
        String *_appID;
        String *_powerID;

};


#endif
