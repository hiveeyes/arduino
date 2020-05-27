/*

Bienenwaage 5.0 TTGO-T-CALL
https://github.com/hiveeyes/arduino/tree/master/node-esp32-generic


Telemetry | GSM or WiFi
-----------------------
sensors:  | Scale
          | Temperature
          | Battery
transp:   | GSM via MQTT
          | WiFi via MQTT

Copyright (C) 2019-2020 by Stefan Ulmer


GNU GPL v3 License
------------------
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

The license can be viewed at
http://www.gnu.org/licenses/gpl-3.0.txt



Farbcodes Anschlußkabel BOSCHE H40A
-----------------------------------
+E Rot
+A Grün
-E Schwarz
-A Weiß
Shield Lila

Hinweise
--------
- Sketch ist für TTGO-T-CALL mit dem SIM800 Modul ausgelegt.
- Die WPA2 Verbindung wurde noch nicht mit dem ESP32 verifiziert.
- Der Sensor Count für den Temperatur Sensor gibt 0 aus - Messung selbst.
- #define MAXBUFFERSIZE (2048) in Adafruit_MQTT.h anpassen,
  da ansonsten nicht alle Daten übermittelt werden können.

*/

#include <Arduino.h>

#define GSM_ENABLED             true    // Bei FALSE wird automatisch WIFI aktiviert
#define WEIGHT                  true
#define SENSOR_DS18B20          true
#define SENSOR_BATTERY_LEVEL    false  // Falls Spannungsmessung über Spannungsteiler erfolgen soll, wenn kein SIM800 Modul verwendet wird.
#define DEEPSLEEP_ENABLED       true  // Code ist aktuell nur auf TRUE ausgelegt, falls False, muß noch im main() ein Delay eingebaut werden.
#define SLEEP_TIMER             true  // SleepDauer abhängig vom Ladezustand, Sleep_Timer noch nicht mit Wifi verifziert.
#define WUNDERGROUND            false  // Funktioniert aktuell nur mit GSM_ENABLED false
#define WIFI_ENTERPRISE         false  // Unterstützung von WPA2 Enterprise Verschlüsselung, falls FALSE wird der WifiManager verwendet,
#define SLEEP_SHORT             false   // Steuerung der Sleepdauer - 5 min/15 min oder 15 min/60 min


#if GSM_ENABLED

  #define WIFI_ACTIVE false

  #define TINY_GSM_MODEM_SIM800
  #define TINY_GSM_RX_BUFFER   1024

  #include <Wire.h>
  #include <TinyGsmClient.h>
  #define SerialAT Serial1


  // TTGO T-Call pins
    #define MODEM_RST            5
    #define MODEM_PWKEY          4
    #define MODEM_POWER_ON       23
    #define MODEM_TX             27
    #define MODEM_RX             26
    #define I2C_SDA              21
    #define I2C_SCL              22

    /* Daten für Netzclub SIM Karte
    const char apn[]  = "pinternet.interkom.de";  //Abhängig vom Netzprovider - ggfs.auf eigene Werte anpassen.
    const char user[] = "";                       //Abhängig vom Netzprovider
    const char pass[] = "";                       //Abhängig vom Netzprovider
    */

    /*Daten für Telekom SIM
    const char apn[]  = "internet.telekom"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
    const char user[] = "t-mobile"; // GPRS User
    const char pass[] = "tm"; // GPRS Password
    */

    // Daten für Discotel SIM
    const char apn[]  = "internet"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
    const char user[] = ""; // GPRS User
    const char pass[] = ""; // GPRS Password


    // Unlock SIM card using PIN
    #define GSM_USE_PIN true
    const char simPIN[]   = "1234"; // PIN gegen eigenen Wert austauschen


  int gsm_csq; //Variable für GSM Signal Stärke

#else

  #define WIFI_ACTIVE true
  long wifi_rssi ; // Wifi Signalstärke

#endif

#if SENSOR_BATTERY_LEVEL
  int adc_level;
#endif

// Variablen für Spanungsmessung, werden unabhängig der Messmethode (Spannungsteiler o. SIM800 Modul benötigt)
int volt_level;
float voltage;


#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>

#if WIFI_ACTIVE
  #include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#endif

#if WIFI_ENTERPRISE

  //#include "user_interface.h"      // Bibliotheken zur Unterstützung der WPA2 Verschlüsselung
  //#include "wpa2_enterprise.h"
  #include "esp_wpa2.h"

  // SSID to connect to
  static const char* ssid = "Your_WPA2SSID";
  // Username for authentification
  static const char* username = "Your_User";
  // Password for authentication
  static const char* password = "Your_Password";

#endif

#if SLEEP_TIMER

  #include <EEPROM.h>
  int power_save_mode = 0;
  int address = 1;     // Willkürlich von mir festgelegt das im EEPROM an Adresse 1 der Wert für den Ladezustand abgelegt wird.
  int voltbe4;         // Wert der vorherigen Messung
  int voltbelow = 75;  // Schwellwert für die Aktivierung des PowerSave Modus (Verlängerung des SleepTimers auf 60 min)

#endif


#if SLEEP_SHORT
  int sleepTimerShort = 300; //5-Minuten SleepTimer
  int sleepTimerLong  = 900; //15-Minuten SleepTimer
#else
  int sleepTimerShort = 900; //15-Minuten SleepTimer
  int sleepTimerLong  = 3600; //60-Minuten SleepTimer
#endif

int sleepTimeS;
#define uS_TO_S_FACTOR 1000000LL

// Boot Counter - Wert wird mit jedem WakeUp des ESP hochgezählt, 0 bei Kaltstart
RTC_DATA_ATTR int bootCount = 0;

// Adafruit MQTT
// https://github.com/adafruit/Adafruit_MQTT_Library
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// JSON serializer
// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>

// ----
// MQTT
// ----

// Note that the "testdrive" channel as outlined within "MQTT_TOPIC" is not
// authenticated and can be used anonymously.
//
// To publish data to a personal data channel, please ask for appropriate
// credentials at https://community.hiveeyes.org/ or hello@hiveeyes.org.
//
// Documentation:
// https://community.hiveeyes.org/t/messdaten-an-die-hiveeyes-plattform-ubermitteln/1813
// https://community.hiveeyes.org/t/zugangsdaten-anfragen-und-account-erstellen/2193

// How often to retry connecting to the MQTT broker
#define MQTT_RETRY_COUNT    5

// How long to delay between MQTT (re)connection attempts (seconds)
#define MQTT_RETRY_DELAY    1.5f

// The address of the MQTT broker to connect to
#define MQTT_BROKER         "swarm.hiveeyes.org"
#define MQTT_PORT           1883

// A MQTT client ID, which should be unique across multiple devices for a user.
// Maybe use your MQTT_USERNAME and the date and time the sketch was compiled
// or just use an UUID (https://www.uuidtools.com/) or other random value.

// Hier eigenen Wert eintragen.
#define MQTT_CLIENT_ID      "random-123"

// The credentials to authenticate with the MQTT broker.
// Eigene Login-Daten beim netten Team von Hiveeyes anfragen (s.o.)
#define MQTT_USERNAME       "DeinLoginName"
#define MQTT_PASSWORD       "DeinKennwort"

// The MQTT topic to transmit sensor readings to.
#define MQTT_TOPIC          "hiveeyes/testdrive/spielwiese/node-1/data.json"


#if GSM_ENABLED


// I2C for SIM800 (to keep it running when powered from battery)
TwoWire I2CPower = TwoWire(0);

  // Client für GSM hier aufrufen
  TinyGsm modem(SerialAT);
  TinyGsmClient client(modem);
  TinyGsmClient clientMQTT(modem);


    #define IP5306_ADDR          0x75
    #define IP5306_REG_SYS_CTL0  0x00

    bool setPowerBoostKeepOn(int en){
    I2CPower.beginTransmission(IP5306_ADDR);
    I2CPower.write(IP5306_REG_SYS_CTL0);
    if (en) {
        I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
    } else {
        I2CPower.write(0x35); // 0x37 is default reg value
    }
    return I2CPower.endTransmission() == 0;
    }

#endif

#if WIFI_ACTIVE

  WiFiClient client;
  WiFiClient clientMQTT;

#endif


// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details
Adafruit_MQTT_Client mqtt(&clientMQTT, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

// Setup MQTT publishing handler
Adafruit_MQTT_Publish mqtt_publisher = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC);


#if WEIGHT

  #include <HX711.h>
  #include <RunningMedian.h>  // http://playground.arduino.cc/Main/RunningMedian

  // Aktuelle HX711 Library verwendet deshalb anstatt #define -> const int und ; am Ende ;-)
  //#define SCALE_DOUT_PIN_A D5 // DT
  //#define SCALE_SCK_PIN_A D3 // SCK

  const int SCALE_DOUT_PIN_A = 33;// DT
  const int SCALE_SCK_PIN_A = 32;// SCK

  //#define SCALE_DOUT_PIN_B D2 // DT
  //#define SCALE_SCK_PIN_B D1 // SCK

  const int SCALE_DOUT_PIN_B = 14;// DT
  const int SCALE_SCK_PIN_B = 25;// SCK

  HX711 scale_A;
  HX711 scale_B;

  long AktuellesGewicht_A = 0;
  long AktuellesGewicht_B = 0;

  float Taragewicht_A = 226743;  // Hier ist der Wert aus der Kalibrierung einzutragen
  float Skalierung_A = 41.647;  // Hier ist der Wert aus der Kalibrierung einzutragen

  float Taragewicht_B = -238537;  // Hier ist der Wert aus der Kalibrierung einzutragen
  float Skalierung_B = -42.614;  // Hier ist der Wert aus der Kalibrierung einzutragen

  long Gewicht_A = 999999;
  float GewichtEinzelmessung_A;

  long Gewicht_B = 999999;
  float GewichtEinzelmessung_B;

  // create RunningMedian object with 10 readings
  RunningMedian GewichtSamples_A = RunningMedian(10);
  RunningMedian GewichtSamples_B = RunningMedian(10);

#endif

#if SENSOR_DS18B20

  // Temperatur Sensoren DS18B20 connectet to Pin 4
  #define DS18B20_PIN 4

  // 1-Wire library
  #include <OneWire.h>

  // DS18B20/DallasTemperature library
  #include <DallasTemperature.h>

  // For communicating with any 1-Wire device (not just DS18B20)
  OneWire oneWire(DS18B20_PIN);

  // Initialize DallasTemperature library with reference to 1-Wire object
  DallasTemperature sensors(&oneWire);

  // Device Adresses - dedected by oneWireSearch.ino or Multiple similar
  // Update device adress for your Sensor

  uint8_t Sensor1 [8] = { 0x28, 0xFF, 0xCF, 0xBD, 0xA4, 0x16, 0x04, 0x46 };  // Nr. 3 grüne Schrift
  uint8_t Sensor2 [8] = { 0x28, 0xFF, 0x67, 0x65, 0x51, 0x16, 0x04, 0xEE }; // langes Kabel

  // Define Variable to hold temperature value for Sensor1
  float temps1;

  // Define Variable to hold temperature value for Sensor2
  float temps2;

#endif

#if WUNDERGROUND

  #include <HTTPClient.h>

  // Weatherundeground
  const char* WUG_SERVER = "api.weather.com";

  // Define variable as "char" as they will be extracted via finder.string of XML Data
  float currentTemp;
  float currentHumidity;

  // Weather Underground API key.
  String WUG_API_KEY = "6532...";

  // Weather Underground Station.
  // Alternative stations: IMNCHEN1945, IBAYERNM74
  String WUG_STATION_ID = "IMUNICH323";

#endif

void gsm_setup();
void gsm_connect();
void gsm_disconnect();
void gsm_info();
void gsm_poweron();
void gsm_SleepMode2On();
void gsm_SleepModeOff();
bool mqtt_connect();
void transmit_readings();
void setup_weight();
void read_weight();
void setup_tempsensor();
void read_tempsensor();
void read_battery_level();
void power4sleep();
bool read_weatherunderground();



void setup() {

Serial.begin(115200);

//Increment boot number and print it every reboot
++bootCount;
Serial.println("Boot number: " + String(bootCount));

#if WIFI_ACTIVE

  #if WIFI_ENTERPRISE

    Serial.println("Booting WPA2 Enterprise Wifi Mode ");

    // WPA2 Connection starts here
    // Setting ESP into STATION mode only (no AP mode or dual mode)

    /* Code für WPA2 für esp8266 - achtung auch wpa2_enterprise.h bei WIFI_ENTERPRISE notwendig
    wifi_set_opmode(STATION_MODE);
    struct station_config wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char*)wifi_config.ssid, ssid);
    wifi_station_set_config(&wifi_config);
    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();
    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
    wifi_station_set_enterprise_username((uint8*)username, strlen(username));
    wifi_station_set_enterprise_password((uint8*)password, strlen(password));
    wifi_station_connect();

    */
  WiFi.disconnect(true);  //disconnect form wifi to set new wifi connection
  WiFi.mode(WIFI_STA); //init wifi mode
  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username)); //provide identity
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username)); //provide username --> identity and username is same
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password)); //provide password
  esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
  esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
  WiFi.begin(ssid);

    // WPA2 Connection ends here
    /*
    // Wait for connection AND IP address from DHCP
    Serial.println();
    Serial.println("Waiting for connection and IP Address from DHCP");


    while (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      Serial.print(".");
    }*/
    // Wait for connection AND IP address from DHCP
    Serial.println();
    Serial.println("Waiting for connection and IP Address from DHCP");
    while (WiFi.status() != WL_CONNECTED) {
      delay(2000);
      Serial.print(".");
    }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /*
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");

      Serial.println("ESP8266 in sleep for 1 min mode");
      esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
	  esp_deep_sleep_start();
      delay(100);
    }
    */

  #else

    Serial.println("Booting Wifi Mode ");

    // Start Wifi Manager to connect
    WiFiManager wifiManager;

    wifiManager.setConfigPortalTimeout(60);
    wifiManager.autoConnect("AutoConnectAP");

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");

      Serial.println("ESP8266 in sleep for 1 min mode");
      //Go to sleep now
      esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
	  esp_deep_sleep_start();
      delay(100);
    }

  #endif

  Serial.println(WiFi.localIP().toString());
  Serial.println(WiFi.SSID());

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  wifi_rssi = WiFi.RSSI();
  Serial.print("Wifi Signalstärke: ");
  Serial.println(wifi_rssi);

#endif

#if GSM_ENABLED

  Serial.println(" ->");
  Serial.println("Booting GSM Mode");

  gsm_setup();

  gsm_SleepModeOff();

  gsm_connect();

  gsm_info();

#endif

Serial.println(" ->");
  Serial.println("Setup Weight");

setup_weight();


#ifdef SENSOR_DS18B20
  Serial.println(" ->");
  Serial.println("Setup Temp Sensor");

  setup_tempsensor();

#endif

}

void loop() {


  Serial.println("MQTT Connect");

  if (!mqtt_connect()) {
    return;
  }

  Serial.println("Read Weight");
  read_weight();

  Serial.println("Read Temperatur Sensor");
  read_tempsensor();

  Serial.println("Read Battery Level");
  read_battery_level();

  Serial.println("Read Weatherunderground");
  read_weatherunderground();

  Serial.println("Set Sleep Timer");
  power4sleep();

  Serial.println("Transmit Reading");
  transmit_readings();

  delay(1000);

  #if GSM_ENABLED
    gsm_disconnect();
  #endif

  #if DEEPSLEEP_ENABLED
      Serial.println("Going to sleep ");
      esp_sleep_enable_timer_wakeup(sleepTimeS * uS_TO_S_FACTOR);
	    esp_deep_sleep_start();
      delay(100);
  #endif

}

void setup_weight() {

  #if WEIGHT

  scale_A.begin(SCALE_DOUT_PIN_A, SCALE_SCK_PIN_A);
  scale_B.begin(SCALE_DOUT_PIN_B, SCALE_SCK_PIN_B);

    // Waage A Setup
    scale_A.set_offset(Taragewicht_A);
    scale_A.set_scale(Skalierung_A);

    // Waage B Setup
    scale_B.set_offset(Taragewicht_B);
    scale_B.set_scale(Skalierung_B);
  #endif

}


void read_weight() {

  #if WEIGHT

    //clearRunningMedian Sample
    GewichtSamples_A.clear();
    GewichtSamples_B.clear();

    // read x times weight and take median
    // do this till running median sample is full
    while (GewichtSamples_A.getCount() < GewichtSamples_A.getSize()) {

      // wait between samples
      Serial.print(".");
      delay(180);

      // power HX711 / load cell
     scale_A.power_up();
     scale_B.power_up();

      delay(2);  // wait for stabilizing

      // read raw data input of HX711
      Serial.print("Read Scale A.");
      GewichtEinzelmessung_A = scale_A.read();
      Serial.print("Read Scale B.");
      GewichtEinzelmessung_B = scale_B.read();

      // switch off HX711 / load cell
      scale_A.power_down();
      scale_B.power_down();

      // calculate weight in kg
      Gewicht_A = (GewichtEinzelmessung_A - Taragewicht_A) / Skalierung_A;
      Gewicht_B = (GewichtEinzelmessung_B - Taragewicht_B) / Skalierung_B;

      // use calculated kg values for median statistic
      GewichtSamples_A.add(Gewicht_A);
      GewichtSamples_B.add(Gewicht_B);

      AktuellesGewicht_A = GewichtSamples_A.getMedian();
      AktuellesGewicht_B = GewichtSamples_B.getMedian();

      Serial.print("Gewicht A: ");
      Serial.print(AktuellesGewicht_A);
      Serial.println(" g");

      Serial.print("Gewicht B: ");
      Serial.print(AktuellesGewicht_B);
      Serial.println(" g");

    }
  #endif
}


void gsm_setup() {

#if GSM_ENABLED


  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);

  // Keep power when running from battery
  bool isOk = setPowerBoostKeepOn(1);
  Serial.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

// Unlock your SIM card with a PIN if needed
 if (GSM_USE_PIN && modem.getSimStatus() != 3) {
    modem.simUnlock(simPIN);
    Serial.print("GetSimStatus:");
    Serial.print(simPIN);
  }

#endif

}

void gsm_connect() {

#if GSM_ENABLED

  Serial.print("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println("Connected to mobil network");

  gsm_info();

  Serial.print("GPRS Connecting to ");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);

  // Falls die Verbindung mit dem GSM & GPRS Netz nicht klappt, legt sich der der ESP für 60 sec schlafen und wir starten im Anschluß von vorn.
  // GSM Modem stoppen

     gsm_disconnect();

    // Sleep für 60 Sekunden
     esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
	   esp_deep_sleep_start();
     delay(100);

    return;
  }
  Serial.println("GPRS Connection established");

#endif

}


void gsm_info() {

#if GSM_ENABLED

  gsm_csq = modem.getSignalQuality();
  volt_level = modem.getBattPercent();
  voltage = modem.getBattVoltage() / 1000.0F;

  Serial.println();
  Serial.println("Signal Qualität");
  Serial.println(gsm_csq);

  Serial.println("Akku Level ");
  Serial.println(volt_level);

  Serial.println("Akku Wert Spannung ");
  Serial.println(voltage);

#endif

}



void gsm_disconnect() {

#if GSM_ENABLED

  // For dem Sleep  - Verbindung kappen
  Serial.println("GPRS Disconnect");
  modem.gprsDisconnect();

  // For dem Sleep  - Modem RadioOFF schalten
  Serial.println("GSM Radio Off ");
  modem.radioOff();

  delay(1000);

  Serial.println("GSM Sleep Mode 2 Enablen ");
  gsm_SleepMode2On();

#endif

}

void gsm_SleepMode2On() {

#if GSM_ENABLED

  modem.sendAT(GF("+CSCLK=2"));
  delay(1000);

#endif

}

void gsm_SleepModeOff() {

#if GSM_ENABLED

  modem.sendAT(GF("+CSCLK=0"));
  delay(1000);

#endif

}

bool mqtt_connect() {

  // If already connected, don't do anything and signal success
  if (mqtt.connected()) {
    return true;
  }

  Serial.println("Connecting to MQTT broker");

  // Reconnect loop
  uint8_t retries = MQTT_RETRY_COUNT;
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {

    Serial.println("Anzahl MQTT Connection Retries:");
    Serial.println(ret);
    Serial.println(String(mqtt.connectErrorString(ret)).c_str());

    retries--;
    if (retries == 0) {
      Serial.println("Giving up connecting to MQTT broker");

      // Falls keine MQTT Verbindung aufgebaut werden kann, hängen wir hier ewig in der Schleife
      // da solange die Funktion mqtt.connect aufgerufen wird
      // deshalb fangen wir von vorne an in dem wir den ESP 1min schlafen geht und die Schleife von vorne beginnt

      // GSM Modem stoppen

          gsm_disconnect();

      // Sleep für 60 Sekunden.

          esp_sleep_enable_timer_wakeup(60 * uS_TO_S_FACTOR);
	        esp_deep_sleep_start();
          delay(100);

      return false;
    }

    Serial.println("Retry MQTT Connection in x Seconds:");
    Serial.println(MQTT_RETRY_DELAY);

    // Wait some time before retrying
    delay(MQTT_RETRY_DELAY * 1000);
  }

  if (mqtt.connected()) {

    Serial.println("Successfully connected to MQTT broker");
    Serial.println(MQTT_BROKER);

    return true;
  }

  // Giving up on further connection attempts
  return false;
}

void setup_tempsensor() {

#if SENSOR_DS18B20

  Serial.println("Temperaturmessung mit dem DS18B20 ");

  // Start Dallas Tempeartur Library / Instanz
  sensors.begin();  // DS18B20 starten

  // Präzision auf 12 Bit
  //sensors.setResolution(TEMP_12_BIT);
  sensors.setResolution(12);

  //Anzahl Sensoren ausgeben
  Serial.print("Sensoren: ");
  Serial.println(sensors.getDeviceCount());

#endif

}

void read_tempsensor() {

#if SENSOR_DS18B20

  // Temperatursensor(en) auslesen
  sensors.requestTemperatures();

  // Read Temperature form each Sensor
  temps1 = sensors.getTempC(Sensor1);
  temps2 = sensors.getTempC(Sensor2);

#endif

}

void read_battery_level() {

#if SENSOR_BATTERY_LEVEL

  // Read the battery level from the ESP8266 analog input pin.
  // Analog read level is 10 bit 0-1023 (0V-1V).
  // Our 1M & 220K voltage divider takes the max
  // MY 1M & 3,3 K
  // LiPo value of 4.2V and drops it to 0.758V max.
  // This means our minimum analog read value should be 580 (3.14V)
  // and the maximum analog read value should be 774 (4.2V).

   // Read Value from A0 - max 3.3V, daher etwas andere Teiler
  // 1024 / 3,3  * 1,73 = 537 1,73V laut spannungsteiler bei 4,2 V mit 4,7k & 3,3k
  // 1024 / 3,3  * 1,3 = 400 1,3V lt. spannungsteiler bei 3,14 V mit 4,7k & 3,3 k



  adc_level = analogRead(A0);
  Serial.print("ADC_level: ");
  Serial.println(adc_level);

  // Map Value to % Level
  volt_level = map(adc_level, 400, 537, 0, 100);
  Serial.println(volt_level);

  // Calculate Voltage Value
  Serial.print("Voltage: ");
  voltage = adc_level * 4.2 / 537 ;
  Serial.println( voltage );

  // Give operating system / watchdog timer some breath
  yield();
#endif
}

void power4sleep() {

#if SLEEP_TIMER

  EEPROM.begin(512);

  //letzten Ladungsstand Wert aus EEprom auslesen
  voltbe4 = EEPROM.read(address);
  Serial.print("Read Value:");
  Serial.println(voltbe4);

  //aktuellen Ladungsstand Wert in EEprom ablegen
  EEPROM.write(address, volt_level );
  EEPROM.commit();

  if (volt_level < voltbelow && voltbe4 < voltbelow )
  {
    // Wenn Ladungswert unter 75% wird die Sleep Dauer auf 60 min (=3600 Sekunden) verlängert
    //sleepTimeS = 3600;

    // Wert wird Abhängig vom #define SLEEP_SHORT gesetzt

    sleepTimeS = sleepTimerLong;
    power_save_mode = 1;

  }
  else
  {
    // Wenn Ladungswert über  75% ist die Sleep Dauer auf 15 min (=900 Sekunden) gesetzt
    // sleepTimeS = 900;

    // Wert wird Abhängig vom #define SLEEP_SHORT gesetzt

    sleepTimeS = sleepTimerShort;
    power_save_mode = 0;

  }

    Serial.print("Set Sleep Timer to:");
    Serial.println(sleepTimeS);
    Serial.print("Set Power Save to:");
    Serial.println(power_save_mode);

#endif

}

bool read_weatherunderground() {

  bool success = false;

#if WUNDERGROUND and !GSM_ENABLED

  while (!client.available()) {
    //  delay(200);
    delay(10000);
    Serial.print("Waiting for WiFi connection");
  }

  HTTPClient http;
  String wug_uri = "/v2/pws/observations/current?apiKey=" + WUG_API_KEY + "&stationId=" + WUG_STATION_ID + "&format=json&units=m";

  // Make HTTP request.
  http.begin(client, WUG_SERVER, 80, wug_uri);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {

    // Allocate the JsonDocument
    DynamicJsonDocument doc(35 * 1024);

    // Deserialize the JSON document.
    DeserializationError error = deserializeJson(doc, http.getStream());

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      goto wug_end;
    }

    // Extract data from JSON.
    /*
    {
        "observations": [
            {
                "country": "DE",
                "epoch": 1590600612,
                "humidity": 42,
                "lat": 48.122421,
                "lon": 11.665054,
                "metric": {
                    "dewpt": 5,
                    "elev": 528,
                    "heatIndex": 18,
                    "precipRate": 0.0,
                    "precipTotal": 0.0,
                    "pressure": 1027.77,
                    "temp": 18,
                    "windChill": 18,
                    "windGust": 0,
                    "windSpeed": 0
                },
                "neighborhood": "Renkenweg",
                "obsTimeLocal": "2020-05-27 19:30:12",
                "obsTimeUtc": "2020-05-27T17:30:12Z",
                "qcStatus": 1,
                "realtimeFrequency": null,
                "softwareType": "EasyWeatherV1.1.4",
                "solarRadiation": 24.2,
                "stationID": "IMUNICH323",
                "uv": 0.0,
                "winddir": 111
            }
        ]
    }
    */
    JsonObject root = doc.as<JsonObject>();
    JsonObject observation = root["observations"][0].as<JsonObject>();
    currentHumidity = observation["humidity"];
    currentTemp = observation["metric"]["temp"];
    success = true;

  } else {
    Serial.printf("Connection to Weather Underground failed, error: %s", http.errorToString(httpCode).c_str());
  }

wug_end:

  http.end();

#endif

  return success;

}

void transmit_readings() {

  // Build JSON object containing sensor readings
  // TODO: How many data points actually fit into this?

  // json5: StaticJsonBuffer<1024> jsonBuffer;
  // neu für json6

  DynamicJsonDocument doc(512);

// Die folgende  Zeile war im Beispiel - aber das obj wird nirgends verwendet
//  JsonObject obj = doc.as<JsonObject>();

  // Create telemetry payload by manually mapping sensor readings to telemetry field names.
  // Note: For more advanced use cases, please have a look at the TerkinData C++ library
  //       https://hiveeyes.org/docs/arduino/TerkinData/README.html

  // json5: JsonObject& json_data = jsonBuffer.createObject();

  #if WUNDERGROUND
    doc["WXD_Temp"]        = currentTemp;
    doc["WXD_Humi"]        = currentHumidity;
  #endif

  #if SENSOR_DS18B20
    doc["Temp1"]           = temps1;
    doc["Temp2"]           = temps2;
  #endif

  #if WEIGHT
    doc["Waage1"]          = AktuellesGewicht_A;
    doc["Waage2"]          = AktuellesGewicht_B;
  #endif

  #if WIFI_ACTIVE
    doc["Signal"]          = wifi_rssi;
  #endif

  #if GSM_ENABLED
    doc["Signal"]          = gsm_csq;
  #endif

  #if SLEEP_TIMER
     doc["power"]          = power_save_mode;
  #endif

  doc["volt_level"]        = volt_level;
  doc["voltage"]           = voltage;

  // Debugging
  // json5: json_data.printTo(Serial);
  // für json6:


  serializeJsonPretty(doc, Serial);
  Serial.println();

  // Serialize data
  // json5:int json_length = json_data.measureLength();
  // json5:char payload[json_length + 1];
  // json5: json_data.printTo(payload, sizeof(payload));
  // für json6.
  String payload;
  serializeJson(doc, payload);


  // Publish data
  // TODO: Refactor to TerkinTelemetry
  // json5: if (mqtt_publisher.publish(payload)) {
  // für json6:

  if (mqtt_publisher.publish(payload.c_str())) {
    Serial.println("MQTT publish succeeded");
  } else {
    Serial.println("MQTT publish failed");
  }

}