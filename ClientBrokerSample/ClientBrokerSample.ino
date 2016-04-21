#include "Library/ClientBroker/clientBuffer.h"

//#include <FreeRTOS_ARM.h>
#include "Base64.h"

char *msg1 = "Hello";
char *msg2 = "Those semicolons";
char *msg3 = "World";
char *msg4 = "low power";
char *msg5 = "You just brought piss to a shit fight";

char cmd[91];
char *encoded;              

#define RECEIVE_QUEUE_SIZE 5

QueueHandle_t receivePort1;
QueueHandle_t receivePort2;

int ledBlinkDelay = 200;
int ledColor = 4;

static void test(void *arg) {
  cmd[90] = '\0';
  int inputLen = 0; 
  int encodedLen = 0; 
  while (1) {    

    // generate random data
    for(int i=0; i<10; i++){
      // place task id
      cmd[9*i] = byte(random(1,4));

      cmd[9*i + 1] = 0x11;
      cmd[9*i + 2] = 0;// byte(random(64,255));
      cmd[9*i + 3] = 0x22;
      cmd[9*i + 4] = 0;//byte(random(64,255));
      cmd[9*i + 5] = 0x33;
      cmd[9*i + 6] = 0;//byte(random(64,255));
      cmd[9*i + 7] = 0x44;
      cmd[9*i + 8] = 0;//byte(random(64,255));
      
      /*for(int j=1; j<9; j += 2){
        cmd[9*i+j] = (((byte(j/2)+1) & 0b00001111) << 4) | byte(random(16));
        cmd[9*i+j+1] = byte(random(255));
      }*/
    }

    inputLen = sizeof(cmd);
    encodedLen = base64_enc_len(inputLen);
    encoded = new char[encodedLen+1];
    base64_encode(encoded, cmd, inputLen); 
    encoded[encodedLen] = '\0';

    clientBuffer::getInstance()->publish(1, encoded);
//    clientBuffer::getInstance()->publish(4, msg1);
//    clientBuffer::getInstance()->publish(3, msg3);
//    clientBuffer::getInstance()->publish(2, msg4);
//    clientBuffer::getInstance()->publish(1, packet);

    /*
      clientBuffer::getInstance()->publish(1, msg1);
      clientBuffer::getInstance()->publish(2, msg2);
      clientBuffer::getInstance()->publish(3, msg3);
      clientBuffer::getInstance()->publish(1, msg4);
      clientBuffer::getInstance()->publish(3, msg5);
    */
    delete encoded;
    vTaskDelay(10000);
  }
}

static void blinkLed(void *arg) {
  while (1) {
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

static void receive1(void *arg) {
  String *receiveString = NULL;
  int led;
  while (1) {
    xQueueReceive(receivePort1, &receiveString, portMAX_DELAY);
    if (receiveString != NULL) {
      SerialUSB.println("port 1: " + *receiveString);
      ledBlinkDelay = receiveString->toInt();
      delete receiveString;
    }
    receiveString = NULL;
  }
}

static void receive2(void *arg) {
  String *receiveString = NULL;
  while (1) {
    xQueueReceive(receivePort2, &receiveString, portMAX_DELAY);
    if (receiveString != NULL) {
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

  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  SerialUSB.begin(0);

  while (!SerialUSB);

  receivePort1 = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(String *));
  receivePort2 = xQueueCreate(RECEIVE_QUEUE_SIZE, sizeof(String *));

  clientBuffer::getInstance()->initialize("app1", "due2", "powerpi", "12345678");
  clientBuffer::getInstance()->subscribe(1, receivePort1);
  clientBuffer::getInstance()->subscribe(2, receivePort2);

  xTaskCreate(test, NULL, 500, NULL, 1, NULL);
  xTaskCreate(blinkLed, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(receive1, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(receive2, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskStartScheduler();

  while (1);

  //Wifi_init(appid, pdid);

}

void loop() {
  // put your main code here, to run repeatedly:

}
