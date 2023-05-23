/*
 * Name: libDHT
 * License: MIT license. See details in DHT.cpp.
 * Location: https://github.com/ADiea/libDHT
 * Maintainer: ADiea (https://github.com/ADiea)
 *
 * Descr: Arduino compatible DHT 11/22, AM2302 lib with dewpoint,
 *        heat-index and other goodies.
 *
 * Features & History:
 * See DHT.cpp
 */

#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define DHT_DEBUG 0 //Change to 1 for debug output

/*From datasheet: http://www.micro4you.com/files/sensor/DHT11.pdf
 * '0' if HIGH lasts 26-28us,
 * '1' if HIGH lasts 70us
 */
#define ONE_DURATION_THRESH_US 30

#define DHTLIB_DHT11_WAKEUP         18
#define DHTLIB_DHT22_WAKEUP         5

// how many timing transitions we need to keep track of. 2 * number bits + extra
#define MAXTIMINGS 85

#define DHT_AUTO 0
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

#define READ_INTERVAL_DHT11_DSHEET 1000
#define READ_INTERVAL_DHT22_DSHEET 2000
#define READ_INTERVAL_DONT_CARE 2200 /*safe value*/
#define READ_INTERVAL_LONG 4000

#define DEW_ACCURATE 0
#define DEW_FAST 1
#define DEW_ACCURATE_FAST 2
#define DEW_FASTEST 3

#define WAKEUP_DHT11 18
#define WAKEUP_DHT22 1

// Reference: http://epb.apogee.net/res/refcomf.asp
enum ComfortState
{
	Comfort_OK = 0,
	Comfort_TooHot = 1,
	Comfort_TooCold = 2,
	Comfort_TooDry = 4,
	Comfort_TooHumid = 8,
	Comfort_HotAndHumid = 9,
	Comfort_HotAndDry = 5,
	Comfort_ColdAndHumid = 10,
	Comfort_ColdAndDry = 6
};

enum ErrorDHT
{
	errDHT_OK = 0,
	errDHT_Timeout,
	errDHT_Checksum,
	errDHT_Other,
};

class DHT
{
public:
	DHT(uint8_t pin, uint8_t type = DHT_AUTO, boolean pullup = false,
			uint16_t maxIntervalRead = READ_INTERVAL_DONT_CARE)
		: m_kSensorPin(pin), m_kSensorType(type),
		  m_bPullupEnabled(pullup), m_lastreadtime(0), m_maxIntervalRead(maxIntervalRead)
	{
		m_lastError = errDHT_Other;
		m_lastTemp = NAN;
		m_lastHumid = NAN;

		if (DHT11 == m_kSensorType)
		{
			if (READ_INTERVAL_DONT_CARE == maxIntervalRead)
			{
				m_maxIntervalRead = READ_INTERVAL_DHT11_DSHEET;
			}
			m_wakeupTimeMs = WAKEUP_DHT11;
		}
		else if (DHT22 == m_kSensorType || DHT21 == m_kSensorType)
		{
			if (READ_INTERVAL_DONT_CARE == maxIntervalRead)
			{
				m_maxIntervalRead = READ_INTERVAL_DHT22_DSHEET;
			}
			m_wakeupTimeMs = WAKEUP_DHT22;
		}

		//In computing these constants the following reference was used
		//http://epb.apogee.net/res/refcomf.asp
		//It was simplified as 4 straight lines and added very little skew on
		//the vertical lines (+0.1 on x for C,D)
		//The for points used are(from top left, clock wise)
		//A(30%, 30*C) B(70%, 26.2*C) C(70.1%, 20.55*C) D(30.1%, 22.22*C)
		//On the X axis we have the rel humidity in % and on the Y axis the temperature in *C

		//Too hot line AB
		m_tooHot_m = -0.095;
		m_tooHot_b = 32.85;
		//Too humid line BC
		m_tooHumid_m =  -56.5;
		m_tooHumid_b = 3981.2;
		//Too cold line DC
		m_tooCold_m = -0.04175;
		m_tooHCold_b = 23.476675;
		//Too dry line AD
		m_tooDry_m = -77.8;
		m_tooDry_b = 2364;
	};

	void begin(void);

	float readTemperature(bool bFarenheit = false);
	float readHumidity(void);
	bool readTempAndHumidity(float* temp, float* humid, bool bFarenheit = false);

	inline float convertCtoF(float c){ return c * 1.8f + 32; }
	inline float convertFtoC(float f){ return (f-32)/1.8f; }

	float getHeatIndexC(float tempCelsius, float percentHumidity);
	float getHeatIndexF(float tempFahrenheit, float percentHumidity);
	double getDewPoint(float tempCelsius, float percentHumidity,
			uint8_t algType = DEW_ACCURATE_FAST);

	float getComfortRatio(float tCelsius, float humidity, ComfortState& comfort);

	inline bool isTooHot(float tCelsius, float humidity)
		{return (tCelsius > (humidity * m_tooHot_m + m_tooHot_b));}
	inline bool isTooHumid(float tCelsius, float humidity)
		{return (tCelsius > (humidity * m_tooHumid_m + m_tooHumid_b));}
	inline bool isTooCold(float tCelsius, float humidity)
		{return (tCelsius < (humidity * m_tooCold_m + m_tooHCold_b));}
	inline bool isTooDry(float tCelsius, float humidity)
		{return (tCelsius < (humidity * m_tooDry_m + m_tooDry_b));}

	inline ErrorDHT getm_lastError() { return m_lastError; }
private:
	bool read(void);
	void updateInternalCache();
	
	inline float distanceTooHot(float tCelsius, float humidity)
		{return tCelsius - (humidity * m_tooHot_m + m_tooHot_b);}
	inline float distanceTooHumid(float tCelsius, float humidity)
		{return tCelsius - (humidity * m_tooHumid_m + m_tooHumid_b);}
	inline float distanceTooCold(float tCelsius, float humidity)
		{return (humidity * m_tooCold_m + m_tooHCold_b) - tCelsius;}
	inline float distanceTooDry(float tCelsius, float humidity)
		{return (humidity * m_tooDry_m + m_tooDry_b) - tCelsius;}	
	
	uint8_t m_kSensorPin, m_kSensorType;
	uint8_t m_data[6];
	unsigned long m_lastreadtime;
	uint8_t m_wakeupTimeMs;

	boolean m_bPullupEnabled;
	ErrorDHT m_lastError;

	//The datasheet advises to read no more than one every 2 seconds.
	//However if reads are done at greater intervals the sensor's output
	//will be less subject to self-heating
	//Reference: http://www.kandrsmith.org/RJS/Misc/dht_sht_how_fast.html
	uint16_t m_maxIntervalRead;

	//Represent the 4 line equations:
	//dry, humid, hot, cold, using the y = mx + b formula
	float m_tooHot_m, m_tooHot_b;
	float m_tooCold_m, m_tooHCold_b;
	float m_tooDry_m, m_tooDry_b;
	float m_tooHumid_m, m_tooHumid_b;

	float m_lastTemp, m_lastHumid;
};
#endif
