
// SERVER
#include <SPI.h>
#include <RH_NRF24.h>
#include <Servo.h>

Servo servoLeftArm;
Servo servoRightArm;
Servo servoLeftLeg;
Servo servoRightLeg;

// data transfer array
uint8_t flexValues[6];

// Singleton instance of the radio driver
RH_NRF24 nrf24;

void setup() 
{
  Serial.begin(9600);

  // Attach servos
  servoLeftArm.attach(2);
  servoRightArm.attach(3);
  servoLeftLeg.attach(4);
  servoRightLeg.attach(5);

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
      // Serial.println("got request: ");
      Serial.println(buf[0]);
      Serial.println(buf[1]);
      Serial.println(buf[2]);
      Serial.println(buf[3]);
      Serial.println(buf[4]);
      Serial.println(buf[5]);

      // Write data values to servos
      servoLeftArm.write(buf[0]);
      servoRightArm.write(buf[1]);
      servoLeftLeg.write(buf[2]);
      servoRightLeg.write(buf[3]);

    }
    else
    {
      Serial.println("recv failed");
    }
  }
  delay(10);
}
