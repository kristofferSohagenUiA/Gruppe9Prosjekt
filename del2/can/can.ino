//Kilder:
//Vi har tatt utgangspunkt i disse programmene:
//https://github.com/skpang/Teensy32_CAN-Bus_BME280_temperature_and_humidity/blob/master/Teensy32_CAN-Bus_BME280_temperature_and_humidity.ino
//https://github.com/skpang/Teensy32_OLED_BME280_temperature_display/blob/master/Teensy32_OLED_BME280_temperature_display.ino

#include <Metro.h>

#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeMono12pt7b.h>
#include <splash.h>

#include <MPU9250_WE.h>
#include <Wire.h>

#include <FlexCAN.h>

//--OLED----------------------
const uint8_t oledMosi = 11;
const uint8_t oledClk = 13;
const uint8_t oledDc = 6;
const uint8_t oledReset = 5;
const uint8_t oledCs = 10;

Adafruit_SSD1306 disp(128, 64, oledMosi, oledClk, oledDc, oledReset, oledCs);

//--IMU--------------------------------
MPU9250_WE imu = MPU9250_WE(0x68);

//--METRO--------------------------------
Metro canSend = Metro(1000);
Metro dispAccel = Metro(500);

//--CAN-------------------------------

float accel_z;
uint8_t bytes[sizeof(accel_z)];
bool startSend = false;

CAN_message_t outMsg;
int send = 0;
uint16_t sendID = 0x22;

void setup() {
  Serial.begin(115200);
  //--OLED---------------------------
  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();

  //--IMU---------------------------
  Wire.begin();


  imu.autoOffsets(); //Auto kalibrering av IMU
  imu.setSampleRateDivider(5);
  imu.setAccRange(MPU9250_ACC_RANGE_2G);
  imu.enableAccDLPF(true);
  imu.setAccDLPF(MPU9250_DLPF_6);

  //--CAN--------------------------
  Can0.begin(250000);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

}

void loop() {

  CAN_message_t inMsg;
  while (Can0.available()) //Sjekk om det er inkommende data på bussen
  {
    Can0.read(inMsg); //Les inn meldingen
    if (inMsg.id == 0x21) //Sjekk om meldingen har ID 0x21
    {
      startSend = true; //Start sending av IMU data
    }
  }

  if (dispAccel.check() == 1) //Sjekk om det har gått et halvt sekund siden siste oppdatering av OLED ved hjelp av METRO bibloteket
  {
    xyzFloat accel = imu.getGValues() * -9.81; //Hent akselerasjonsdata og gjør det om fra G til SI-enheter
    accel_z = accel.z; //Lagre dataen i en float
    display(send, sendID, accel_z); //Oppdater OLED skjem med akselerasjonsdata
  }

if ((canSend.check() == 1) && (startSend == true)) //Sjekk om det er gått et sekund siden forrige sending og om start meldingen er mottatt
  {
    
    xyzFloat accel = imu.getGValues() * -9.81; //Hent akselerasjonsdata og gjør det om fra G til SI-enheter
    accel_z = accel.z; //Lagre dataen i en float
    memcpy(bytes, &accel_z, sizeof(accel_z)); //Kopiere over dataen til en byte array

    outMsg.len = 8; //Sette meldingslengden
    outMsg.id = sendID; //Sette meldings ID som skal sendes på

    outMsg.buf[0] = bytes[0]; //Klagjøre bufferen for sending
    outMsg.buf[1] = bytes[1];
    outMsg.buf[2] = bytes[2];
    outMsg.buf[3] = bytes[3];
    outMsg.buf[4] = 0;
    outMsg.buf[5] = 0;
    outMsg.buf[6] = 0;
    outMsg.buf[7] = 0;
    Can0.write(outMsg); //Sende CAN data
    send++; //Oppdatere hvor mange CAN meldinger som har blitt sendt
    display(send, sendID, accel_z); //Oppdatere displayet
  }
}



void display(int antall, int id, float z) //Funksjon for opretting og oppdatering av OLED brukergrensesnitt
{
  disp.clearDisplay();
  disp.setTextSize(1);
  disp.setTextColor(SSD1306_WHITE);
  disp.drawRoundRect(0, 0, 128, 64, 10, SSD1306_WHITE);
  disp.drawFastHLine(5, 11, 118, SSD1306_WHITE);
  disp.setCursor(13, 2);
  disp.print("MAS245 - Gruppe 9");
  disp.setCursor(0, 14);
  disp.println(" CAN-statistikk");
  disp.println(" --------------");
  disp.print(" Antall sendt: ");
  disp.println(antall);
  disp.print(" Sendt med ID: 0x");
  disp.println(id, HEX);
  disp.println(" --------------");
  disp.print(" az:");
  disp.print(z);
  disp.print(" m/s^2");
  disp.display();
}
