#include "Library/ClientBroker/clientBuffer.h"

//#include <FreeRTOS_ARM.h>

  char *msg1 = "Hello";
  char *msg2 = "Those semicolons";
  char *msg3 = "World";
  char *msg4 = "low power";
  char *msg5 = "You just brought piss to a shit fight";

#define RECEIVE_QUEUE_SIZE 5

  QueueHandle_t receivePort1;
  QueueHandle_t receivePort2;

  int ledBlinkDelay = 200;
  int ledColor = 7;

static void test(void *arg) {
  while(1){
    clientBuffer::getInstance()->publish(1, msg1);
    clientBuffer::getInstance()->publish(2, msg2);
    clientBuffer::getInstance()->publish(3, msg3);
    clientBuffer::getInstance()->publish(1, msg4);
    clientBuffer::getInstance()->publish(3, msg5);

    vTaskDelay(10000);
  }
}

static void blinkLed(void *arg){
  while(1){
    vTaskDelay(ledBlinkDelay);
    digitalWrite(6, (ledColor & 1) ? HIGH : LOW);
    digitalWrite(7, (ledColor & 2) ? HIGH : LOW);
    digitalWrite(8, (ledColor & 4) ? HIGH : LOW);
    vTaskDelay(ledBlinkDelay);
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, LOW);
  }
}

static void receive1(void *arg){
  String *receiveString = NULL;
  int led;
  while(1){
        xQueueReceive(receivePort1, &receiveString, portMAX_DELAY);
        if(receiveString != NULL){
          SerialUSB.println("port 1: " + *receiveString);
          ledBlinkDelay = receiveString->toInt();
          delete receiveString;
        }
        receiveString = NULL;
  }
}

static void receive2(void *arg){
  String *receiveString = NULL;
  while(1){
        xQueueReceive(receivePort2, &receiveString, portMAX_DELAY);
        if(receiveString != NULL){
          SerialUSB.println("port 2: " + *receiveString);
          ledColor = receiveString->toInt();
          delete receiveString;
        }
        receiveString = NULL;
  }
}

void setup() {
  // put your setup code here, to run once:
  //char *Message = "What's going on";
  //String test = String(Message);

  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);

  SerialUSB.begin(0);

  while(!SerialUSB);

  receivePort1 = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(String *));
  receivePort2 = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(String *));

  clientBuffer::getInstance()->initialize("app1", "due2", "Nexus5", "password");
  clientBuffer::getInstance()->subscribe(1, receivePort1);
  clientBuffer::getInstance()->subscribe(2, receivePort2);

  xTaskCreate(test, NULL, 500, NULL, 1, NULL);
  xTaskCreate(blinkLed, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(receive1, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(receive2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskStartScheduler();

  while(1);

  //Wifi_init(appid, pdid);

}

void loop() {
  // put your main code here, to run repeatedly:

}
