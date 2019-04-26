/*----------------------------------------------------------
    MH-Z19 CO2 sensor  SAMPLE
  ----------------------------------------------------------*/

#include "MHZ19.h"

const int hw_pin = 2 ;
/*
UART0 ( default RX=GPIO3, TX=GPIO1 ) 
UART1 ( default RX=GPIO9, TX=GPIO10 ) **cannot use default pin!
UART2 ( default RX=GPIO16, TX=GPIO17 )
*/
MHZ19 *mhz19_uart = new MHZ19(hw_pin);

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  setup
  ----------------------------------------------------------*/
void setup()
{
    Serial.begin(115200);
    mhz19_uart->begin(hw_pin);
    mhz19_uart->setAutoCalibration(false);
    while (mhz19_uart->isWarming() == -1)
    {
        Serial.print("MH-Z19 now warming up...  status:");
        Serial.println(mhz19_uart->getStatus());
        delay(1000);
    }
}

/*----------------------------------------------------------
    MH-Z19 CO2 sensor  loop
  ----------------------------------------------------------*/
void loop()
{
    int co2ppm = mhz19_uart->getPPM(MHZ19_POTOCOL::UART);
    int temp = mhz19_uart->getTemperature();

    Serial.print("co2: ");
    Serial.println(co2ppm);
    Serial.print("temp: ");
    Serial.println(temp);

    delay(5000);
}
