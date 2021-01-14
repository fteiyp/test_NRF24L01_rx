// #include <Arduino.h>
// #include <Servo.h>
// #include <SPI.h>
// #include <RF24.h>

// void blinkPin();
// void moveServo();

// RF24 radio(7,8);
// Servo theServo;
// const byte address[6] = "00001";

// void setup() {
//   theServo.attach(11);
//   pinMode(9, OUTPUT);
//   Serial.begin(9600);
//   // radio stuff
//   radio.begin();
//   radio.openReadingPipe(0, address); // 00001
//   radio.setPALevel(RF24_PA_MIN);
//   radio.startListening();
// }

// void loop() {
//   if (radio.available()) {
//     char text[32] = "";
//     radio.read(&text, sizeof(text));
//     Serial.println(text);
//   }
//   // moveServo();
//   blinkPin();
//   delay(400);
// }

// void blinkPin() {
//   static boolean goingup = true;
//   if (goingup) {
//     digitalWrite(9, HIGH);
//     goingup = false;
//   } else {
//     digitalWrite(9, LOW);
//     goingup = true;
//   }
// }

// void moveServo() { 
//    if (Serial.available()) {
//     int value = Serial.parseInt();
//     theServo.write(value);
//   }
// }

// ==== === ==== ===== ==== ==== === ==== ===== ==== ==== === ==== ===== ==== //


// SERVER
#include <SPI.h>
#include <RH_NRF24.h>

// data transfer array
uint8_t flexValues[6];

// Singleton instance of the radio driver
RH_NRF24 nrf24;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) 
    ; // wait for serial port to connect. Needed for Leonardo only
  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
}

void loop()
{
  if (nrf24.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (nrf24.recv(buf, &len))
    {
      // NRF24::printBuffer("request: ", buf, len);
      Serial.println("got request: ");
      Serial.println(buf[0]);
      Serial.println(buf[1]);
      Serial.println(buf[2]);
      Serial.println(buf[3]);
      Serial.println(buf[4]);
      Serial.println(buf[5]);

      // Serial.println((char*)buf);
      
      // Send a reply
      uint8_t data[] = "And hello back to you";
      nrf24.send(data, sizeof(data));
      nrf24.waitPacketSent();
      Serial.println("Sent a reply");
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
