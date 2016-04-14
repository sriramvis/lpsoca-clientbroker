#ifndef clientBuffer_h
#define clientBuffer_h

#include <Arduino.h>
#include "FreeRTOS_ARM.h"
#include "ArduinoJson.h"

typedef struct payload_s {
	int port;
	String *message;
} payload_t;

class clientBuffer {

public:
	static clientBuffer *getInstance();
	void initialize(String appID, String powerID, String net, String pass);
	void publish(int port, char *message);

protected:
	clientBuffer();

private:
	static clientBuffer *_instance;
};


#endif