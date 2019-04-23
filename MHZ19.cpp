/*
  MHZ19.h - MH-Z19 CO2 sensor library for ESP32 with Hardware Sreial
  version 1.0
  
  License MIT
*/

#include "MHZ19.h"
#include "Arduino.h"

#define WAIT_READ_TIMES 100
#define WAIT_READ_DELAY 10

// public

MHZ19::MHZ19()
{
}

MHZ19::MHZ19(int hw){
    begin(hw);
}

MHZ19::~MHZ19()
{
}

void MHZ19::begin(int hw){
    _hw_pin = hw;
}
void MHZ19::setAutoCalibration(boolean autocalib)
{
	writeCommand(autocalib ? autocalib_on : autocalib_off);
}

void MHZ19::calibrateZero()
{
	writeCommand(zerocalib);
}

void MHZ19::calibrateSpan(int ppm)
{
	if (ppm < 1000)
		return;

	uint8_t cmd[MHZ19::REQUEST_CNT];
	for (int i = 0; i < MHZ19::REQUEST_CNT; i++)
	{
		cmd[i] = spancalib[i];
	}
	cmd[3] = (uint8_t)(ppm / 256);
	cmd[4] = (uint8_t)(ppm % 256);
	writeCommand(cmd);
}

int MHZ19::getPPM(MHZ19_POTOCOL protocol)
{
	int data;
	switch (protocol)
	{
	case MHZ19_POTOCOL::UART:
		data = getSerialData(MHZ19_UART_DATA::PPM);
		break;
//    case MHZ19_POTOCOL::PWM:
//        data = getPwmData();
//        break;
	}
	return data;
}

int MHZ19::getTemperature()
{
	return getSerialData(MHZ19_UART_DATA::TEMPERATURE);
}

int MHZ19::getStatus()
{
	return getSerialData(MHZ19_UART_DATA::STAT);
}

boolean MHZ19::isWarming()
{
	return (getStatus() <= 1);
}

//protected
void MHZ19::writeCommand(uint8_t cmd[])
{
	writeCommand(cmd, NULL);
}

void MHZ19::writeCommand(uint8_t cmd[], uint8_t *response)
{
    HardwareSerial mhz19_serial(_hw_pin);
    mhz19_serial.begin(9600);
	mhz19_serial.write(cmd, REQUEST_CNT);
	mhz19_serial.write(mhz19_checksum(cmd));
	mhz19_serial.flush();

	if (response != NULL)
	{
		int i = 0;
		while (mhz19_serial.available() <= 0)
		{
			if (++i > WAIT_READ_TIMES)
			{
				Serial.println("error: can't get MH-Z19 response.");
				return;
			}
			yield();
			delay(WAIT_READ_DELAY);
		}
		mhz19_serial.readBytes(response, MHZ19::RESPONSE_CNT);
	}
}

//private

int MHZ19::getSerialData(MHZ19_UART_DATA flg)
{
	uint8_t buf[MHZ19::RESPONSE_CNT];
	for (int i = 0; i < MHZ19::RESPONSE_CNT; i++)
	{
		buf[i] = 0x0;
	}

	writeCommand(getppm, buf);
	int co2 = 0, co2temp = 0, co2status = 0;

	// parse
	if (buf[0] == 0xff && buf[1] == 0x86 && mhz19_checksum(buf) == buf[MHZ19::RESPONSE_CNT - 1])
	{
		co2 = buf[2] * 256 + buf[3];
		co2temp = buf[4] - 40;
		co2status = buf[5];
	}
	else
	{
		co2 = co2temp = co2status = -1;
	}

	switch (flg)
	{
	case MHZ19_UART_DATA::TEMPERATURE:
		return co2temp;
		break;
	case MHZ19_UART_DATA::STAT:
		return co2status;
		break;
	case MHZ19_UART_DATA::PPM:
	default:
		return co2;
		break;
	}
}
//
//int MHZ19::getPwmData()
//{
//    unsigned long th, tl, ppm = 0;
//
//    do
//    {
//        th = pulseIn(_pwm_pin, HIGH, 1004000) / 1000;
//        tl = 1004 - th;
//        switch (PWM_DATA_SELECT)
//        {
//        case MHZ19_PWM_DATA::CALC_2000_PPM:
//            ppm = 2000 * (th - 2) / (th + tl - 4);
//            break;
//        case MHZ19_PWM_DATA::CALC_5000_PPM:
//            ppm = 5000 * (th - 2) / (th + tl - 4);
//            break;
//        }
//    } while (th == 0);
//
//    return ppm;
//}

void MHZ19::setPwmData(MHZ19_PWM_DATA type)
{
	PWM_DATA_SELECT = type;
}

uint8_t MHZ19::mhz19_checksum(uint8_t com[])
{
	uint8_t sum = 0x00;
	for (int i = 1; i < MHZ19::REQUEST_CNT; i++)
	{
		sum += com[i];
	}
	sum = 0xff - sum + 0x01;
	return sum;
}
