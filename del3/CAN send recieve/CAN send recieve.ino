//Sources:
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

//--CAN-------------------------------

float accel_z;
float accel_SI;
uint8_t bytes[sizeof(accel_z)];
uint8_t data[4];

CAN_message_t outMsg;
uint16_t sendID = 0x22;

void setup() {
  Serial.begin(115200);

  //--OLED---------------------------
  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();

  //--IMU---------------------------
  Wire.begin();

  delay(1000);

  imu.autoOffsets();
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

// CAN recieve
  CAN_message_t inMsg;
  while (Can0.available()) //Sjekk om det er inkommende data
  {
    Can0.read(inMsg); //Les inn meldingen

    if (inMsg.id == 0x22) //Sjekk om meldingen har riktig ID
    {
      data[0] = inMsg.buf[0]; //Les inn meldingen til data[]
      data[1] = inMsg.buf[1];
      data[2] = inMsg.buf[2];
      data[3] = inMsg.buf[3];

      memcpy(&accel_SI, data, 4); //Kopier over data[] til accel_SI for omgjøring til float
      Serial.println(accel_SI); //Print verdien i serial monitor
      display(accel_z, accel_SI); //Skriv verdien til OLED display
    }
  }


// CAN send
if (canSend.check() == 1) //Bruk METRO til å sjekke om det er ett sekund siden forrige melding
  {
    
    xyzFloat accelG = imu.getGValues(); //Hente IMU verdi i G, z-akse
    accel_z = accelG.z; //Legge inn imu verdien i en float


    memcpy(bytes, &accel_z, sizeof(accel_z)); //Gjør om float til byte array for sending over CAN

    outMsg.len = 4; //Definere meldingslengde
    outMsg.id = 0x21; //Definere meldings ID

    outMsg.buf[0] = bytes[0]; //Legge inn verdiene i sendingsbufferen
    outMsg.buf[1] = bytes[1];
    outMsg.buf[2] = bytes[2];
    outMsg.buf[3] = bytes[3];

    Can0.write(outMsg); //Sende CAN melding ut på nettverket
    Serial.println(accel_z); //Printe meldingen som ble sendt i serial monitor
  }
}

void display(float send, float recieve) //Denne funksjonen lager og oppdaterer OLED skjermen
{
  disp.clearDisplay();
  disp.setTextSize(1);
  disp.setTextColor(SSD1306_WHITE);
  disp.drawRoundRect(0, 0, 128, 64, 10, SSD1306_WHITE);
  disp.drawFastHLine(5, 11, 118, SSD1306_WHITE);
  disp.setCursor(13, 2);
  disp.print("MAS245-Gruppe 9/2");
  disp.setCursor(0, 14);
  disp.println(" CAN-kommunikasjon");
  disp.println(" --------------");
  disp.print(" Sendt: ");
  disp.print(send);
  disp.println("G");
  disp.print(" Mottatt: ");
  disp.print(recieve);
  disp.println("m/s^2");
  disp.display();
}
