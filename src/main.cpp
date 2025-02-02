#include <config.h>
#include <Arduino.h>
HardwareSerial mySerial(2);



void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  


}

void loop() {

  while (mySerial.available() > 0){
    // get the byte data from the master
    char data = mySerial.read();
    Serial.print(data);
  }
  delay(1);
  
}
