/*----------------------------------------------------------
    MH-Z19 CO2 sensor  Zero calibration SAMPLE

  !! Caution: This program is not confirmed !!
  ----------------------------------------------------------*/

#include "MHZ19.h"

const int hw_pin = 2;
//UART2 ( default RX=GPIO16, TX=GPIO17 )

const int waitingMinutes = 30;  //waiting 30 minutes

MHZ19 mhz19;

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  setup
  ----------------------------------------------------------*/
void setup() {
  Serial.begin(9600);
  mhz19.begin(hw_pin);
  mhz19.setAutoCalibration(false);
  while ( mhz19.isWarming() ) {
    Serial.print("MH-Z19 now warming up...  status:"); Serial.println(mhz19.getStatus());
    delay(1000);
  }
  Serial.println();
}

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  loop
  ----------------------------------------------------------*/
long cnt = 0;
const long waitingSeconds = waitingMinutes * 60L;
void loop() {
  if ( ++cnt % 60 == 0) {
    Serial.print(cnt / 60); Serial.println("min.");
    Serial.print("co2: "); Serial.print(mhz19.getPPM(MHZ19_POTOCOL::UART)); Serial.println("ppm now.");
  } else {
    Serial.print(".");
  }
  delay(1000);

  if (cnt > waitingSeconds) {
    Serial.println("");
    mhz19.calibrateZero();
    Serial.println("1st zero calibration now .");

    delay(60000);
    mhz19.calibrateZero();  // Just in case
    Serial.println("2nd zero calibration now .");

    for ( int i = 0; i < 10; i++) {
      Serial.print("co2: "); Serial.print(mhz19.getPPM(MHZ19_POTOCOL::UART)); Serial.println("ppm now.");
      delay(10000);
    }
    cnt = 0;
    return;
  }

}
