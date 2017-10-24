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

SoftwareSerial BTSerial(2, 3); // RX, TX

static void LOG(const char *fmt, ... ){
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

