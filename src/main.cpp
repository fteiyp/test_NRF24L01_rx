
// SERVER
#include <RF24.h>
#include <Servo.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Servo servoLeftArm;
Servo servoRightArm;
Servo servoLeftLeg;
Servo servoRightLeg;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// For this example, we'll be using 6 addresses; 1 for each TX node
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// Notice that the last byte is the only byte that changes in the last 5
// addresses. This is a limitation of the nRF24L01 transceiver for pipes 2-5
// because they use the same first 4 bytes from pipe 1.
uint64_t address[6] = 
{
  0x7878787878LL,
  0xB3B4B5B6F1LL,
};

struct PayloadStruct
{
  uint8_t nodeID;
  uint8_t val1;
  uint8_t val2;
  uint8_t val3;
  uint8_t val4;
  uint8_t val5;
};
PayloadStruct payload;

/*************************************** SETUP *****************************************/
void setup()
{
  Serial.begin(115200);

  // Attach servos
  servoLeftArm.attach(2);
  servoRightArm.attach(3);
  servoLeftLeg.attach(4);
  servoRightLeg.attach(5);

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while(1) {}
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    while(1) {}
  }

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity of
  // each other.
  radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.
  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(sizeof(payload)); // 2x int datatype occupy 8 bytes
  // Set the addresses for all pipes to TX nodes
  radio.openReadingPipe(0, address[0]);
  radio.openReadingPipe(1, address[1]);
  // put radio in RX mode
  radio.startListening(); 

  // setup OLED display
  display.clearDisplay();
  display.setTextSize(1);               
  display.setTextColor(SSD1306_WHITE); 

} // setup

/*************************************** LOOP *****************************************/

void loop() // loop (reciever)
{
  uint8_t pipe;
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    uint8_t length = radio.getPayloadSize(); // get the size of the payload
    radio.read(&payload, length);            // fetch payload from FIFO
    Serial.print(F("Received "));
    Serial.print(length);                    // print the size of the payload
    Serial.print(F(" length on pipe "));
    Serial.print(pipe);                     // print the pipe number
    Serial.print(F(" from node "));
    Serial.print(payload.nodeID);           // print the payload's origin
    Serial.print(F(". Val1: "));
    Serial.println(payload.val1);      // print the payload's number

    // display values received
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Rx'd ");
    display.print(length);
    display.print(" bytes from node");
    display.print(payload.nodeID);
    display.display();

    if (payload.nodeID == 0) {
      // left hand control
      servoLeftArm.write(payload.val2);
      servoLeftLeg.write(payload.val3);
    } else if (payload.nodeID == 1) {
      // right hand control
      servoRightArm.write(payload.val2);
      servoRightLeg.write(payload.val3);
    } else {
      Serial.print("Error: wrong ID");
    }
  }
  delay(100);
} // loop
