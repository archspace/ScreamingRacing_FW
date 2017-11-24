/*
  Software serial multple serial test

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo and Micro support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created back in the mists of time
 modified 25 May 2012
 by Tom Igoe
 based on Mikal Hart's example

 This example code is in the public domain.

 */
#include <SoftwareSerial.h>

#define Motor_R_In1 10
#define Motor_R_In2 11
#define Motor_R_En  12
#define Motor_L_In1 5
#define Motor_L_In2 6
#define Motor_L_En  7
#define Led_FR  9
#define Led_FL  9
#define Led_MR  16
#define Led_ML  17
#define Tx_Sim  3
#define Rx_Sim  2  

SoftwareSerial BTSerial(Rx_Sim, Tx_Sim); // RX, TX

void VehicleSetup()
{
  pinMode(Motor_R_In1, OUTPUT);
  pinMode(Motor_R_In2, OUTPUT);
  pinMode(Motor_R_En, OUTPUT);
  pinMode(Motor_L_In1, OUTPUT);
  pinMode(Motor_L_In2, OUTPUT);
  pinMode(Motor_L_En, OUTPUT);

  pinMode(Led_FR, OUTPUT);
  //pinMode(Led_FL, OUTPUT);
  pinMode(Led_MR, OUTPUT);
  pinMode(Led_ML, OUTPUT);

  digitalWrite(Motor_R_En, HIGH);
  digitalWrite(Motor_L_En, HIGH);
}

void VehicleCtrl(uint8_t *ptr)
{
  uint8_t *Buf = ptr;
  uint8_t Motor_R_DIR, Motor_R_SPEED;
  uint8_t Motor_L_DIR, Motor_L_SPEED;
  uint8_t Led_R_Front, Led_L_Front;
  uint8_t Led_R_Middle, Led_L_Middle;

  Led_L_Front = *Buf++;
  Led_R_Front = *Buf++;
  Led_L_Middle = *Buf++;
  Led_R_Middle = *Buf++;
  Motor_L_DIR = *Buf++;
  Motor_L_SPEED = *Buf++;
  Motor_R_DIR = *Buf++;
  Motor_R_SPEED = *Buf++;

  //------------------------------------------//
  // Motor Control Left
  //------------------------------------------//
  if(Motor_L_DIR == 1){
    analogWrite(Motor_L_In1, Motor_L_SPEED);
    digitalWrite(Motor_L_In2, LOW); 
  }
  else if(Motor_L_DIR ==2){
    analogWrite(Motor_L_In2, Motor_L_SPEED);
    digitalWrite(Motor_L_In1, LOW); 
  }
  else{
    analogWrite(Motor_L_In1, 0);
    analogWrite(Motor_L_In2, 0); 
  }

  //------------------------------------------//
  // Motor Control Right
  //------------------------------------------//
  if(Motor_R_DIR == 1){
    analogWrite(Motor_R_In1, Motor_R_SPEED);
    digitalWrite(Motor_R_In2, LOW); 
  }
  else if(Motor_R_DIR == 2){
    analogWrite(Motor_R_In2, Motor_R_SPEED);
    digitalWrite(Motor_R_In1, LOW); 
  }
  else{
    analogWrite(Motor_R_In1, 0);
    analogWrite(Motor_R_In2, 0); 
  }

  //------------------------------------------//
  // Led Control
  //------------------------------------------//
  if(Led_L_Front == 1 && Led_R_Front == 1){
    digitalWrite(Led_FR, HIGH);
    digitalWrite(Led_FL, HIGH);
  }
  else{
    digitalWrite(Led_FR, LOW);
    digitalWrite(Led_FL, LOW);
  }

  if(Led_R_Middle == 1){
    digitalWrite(Led_MR, HIGH);
  }
  else{
    digitalWrite(Led_MR, LOW);
  }

  if(Led_L_Middle == 1){
    digitalWrite(Led_ML, HIGH);
  }
  else{
    digitalWrite(Led_ML, LOW);
  }
}

void LOG(const char *fmt, ... ){
    char tmp[128]; // resulting string limited to 128 chars
    va_list args;
    va_start (args, fmt );
    vsnprintf(tmp, 128, fmt, args);
    va_end (args);
    Serial.print(tmp);
}

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600); //Sugguest to set the same baud rate with software  serial part
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println("PC USB detect!");
  
    // set the data rate for the SoftwareSerial port
    BTSerial.begin(9600);

#if TEST
    Serial.println("AT+RX");
    BTSerial.write("AT+RX");
    while(BTSerial.available()) {
        LOG("%c", (char)BTSerial.read());
    }
#endif
    Serial.println("BLE init done");

    VehicleSetup();
}

uint8_t buf[10];
int ret;
void loop() { // run over and over
    //Receive data from BT Serial UART
    if (BTSerial.available()) {
#if TEST
        LOG("%c", (char)BTSerial.read());
#else
        ret = BTSerial.readBytes(buf, 8);
        if (ret == 8) {
            LOG("F_LED: %d, F_LED: %d, M_LED: %d, M_LED: %d\n"
                  "L motor: %d, speed: %d\n"
                  "R motor: %d, speed: %d\n",
                  buf[0], buf[1], buf[2], buf[3],
                  buf[4], buf[5], buf[6], buf[7]);
            VehicleCtrl(&buf[0]);
        } else {
            LOG("Wrong command!\n");
        }
#endif
    }
    // Receive data from command line for debug --> BT 
    if (Serial.available()) {
        BTSerial.write(Serial.read());
    }
}

