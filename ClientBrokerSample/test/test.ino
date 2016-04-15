#include <clientBuffer.h>

//#include <FreeRTOS_ARM.h>

  char *msg1 = "Hello";
  char *msg2 = "Those semicolons";
  char *msg3 = "World";
  char *msg4 = "low power";
  char *msg5 = "Silicon Valley";

  QueueHandle_t receiveQueue;

static void test(void *arg) {
  //SerialUSB.println("About to publish first message");
  clientBuffer::getInstance()->publish(0, msg1);
  //SerialUSB.println("Published first message");
  clientBuffer::getInstance()->publish(0, msg2);
  //SerialUSB.println("Published second message");
  clientBuffer::getInstance()->publish(0, msg3);
  clientBuffer::getInstance()->publish(0, msg4);
  //SerialUSB.println("About to publish last message");
  clientBuffer::getInstance()->publish(0, msg5);
  //SerialUSB.println("Published Last Message");
  clientBuffer::getInstance()->publish(0, msg1);
  clientBuffer::getInstance()->publish(0, msg2);
  clientBuffer::getInstance()->publish(0, msg3);
  clientBuffer::getInstance()->publish(0, msg4);
  //SerialUSB.println("About to publish last message");
  clientBuffer::getInstance()->publish(0, msg5);
  int i = 0;
  //while (true) {
  //  clientBuffer::getInstance()->publish(42, msg1);
  //}
  SerialUSB.println("Published Last Message");
  while(1);
}

static void readQ(void *arg) {
  SerialUSB.println("Starting Read Task");
  String *response;
  while (1) {
    SerialUSB.println("About to receive message from server. Test code");
    xQueueReceive(receiveQueue, &response, portMAX_DELAY);
    SerialUSB.println("Response is: ");
    SerialUSB.println(*response);
    delete(response);  
  }
}

void setup() {
  // put your setup code here, to run once:
  //char *Message = "What's going on";
  //String test = String(Message);

  SerialUSB.begin(0);

  while(!SerialUSB);

  receiveQueue = xQueueCreate(10, sizeof(String*));


  clientBuffer::getInstance()->initialize("42", "43", "Android_5x", "cmusv2016");
  clientBuffer::getInstance()->subscribe(0, receiveQueue);
  SerialUSB.println("Finished Subscribing");
  

  xTaskCreate(test, NULL, 500, NULL, 1, NULL);
  xTaskCreate(readQ, NULL, 500, NULL, 1, NULL);
  vTaskStartScheduler();

  while(1);

  //Wifi_init(appid, pdid);

}

void loop() {
  // put your main code here, to run repeatedly:

}
