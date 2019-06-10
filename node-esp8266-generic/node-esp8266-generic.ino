//Version 1.01 Spannungs Messung und Deep Sleep
//Version 1.02 MQTT Anbindung
//Version 1.03 Temperatur Meßung
//Version 1.03 Anpassen des Wertes #define MAXBUFFERSIZE (1024) in Adafruit_MQTT.h, im Ordner libraries/Adafruit_MQTT_Library/Adafruit_MQTT.h
//Version 1.04 WiFiClient clientMQTT anstatt client
//Version 1.05 Add to ESP.deepSleep  WAKE_RF_DEFAULT
//Version 1.06 Grundzüge GSM integriert
//Version 1.01 basiert auf Waage 1.06 mit ESP8266 - ab hier Versuch mit Wemos D1 mini Board.
//Version 1.02 Transmit String json_data in Abhängigkeit der #define Sensor Section.
//Version 1.02 MQTT Topic auf -node3 geändert
//Version 1.02 SerialAT.begin(115200) auf SerialAT.begin(57600) reduziert
//Version 1.03 WUNDERGROUND ist noch nicht funktionsfähig siehe Funktion ..._weatheronly
//Version 1.03 Funktion gsm_info() gibt Informationen zu Signal Qualität & Spannungsversorgung des GSM Moduls auf Console aus.
//Version 1.04 GSM Modul per MOSFET & Daten Pin ein / ausschalten https://forum.arduino.cc/index.php?topic=383081.30 mittels P-Channel MOSFET
//Version 1.04 Nicht im Einsatz
//Version 1.05 2. China Modul - über Pin D3 initalisieren - ohne MOSFET - GSM Power on/off umgeschrieben
//VERSION 1.05 läuft mit beiden Modulen - versuch sleepEnable zu nutzen
//Version 1.06 Funktionen sleepEnable & sleepoff in gsm_SleepMode2On & gsm_SleepMode2Off umbenannt
//Version 1.06 Funktion gsm_poweron wird aktuell nicht benötigt - wg. Sequenz behalten.
//Version 1.07 separte Version hier nicht hinterlegt - Versuch der Telegramm Einbindung
//Version 1.08 Sleep Dauer abhängig vom Ladezustand des Akkus
//Version 1.08 ToDo:Sleep Timer Abhängig von Wifi / GSM machen oder andere Adressbereich für Ablage der Daten
//Version 1.08 Anpassen des Wertes #define MAXBUFFERSIZE (2048) in Adafruit_MQTT.h, im Ordner libraries/Adafruit_MQTT_Library/Adafruit_MQTT.h
//Version 1.09 Arduino 1.8.7 (evtl. war vorher 1.8.5, bin mir aber nicht mehr sicher)
//Version 1.09 #define DS18B20_PIN D2 auf D5 geändert 
//Version 1.09 DS18820 Adressen auf 28FFCFBDA4160446 korrigiert - Identifikation mit oneWireSearch.ino
//Version 1.14 Version 1.10-1.13 verworfen und auf Basis 1.09 aufgesetzt.
//Version 1.14 OTA Teil entfernt - Test 0k 15:27
//Version 1.14 HX711 Messung  / Gewicht integriert Test 0k 15:38 + 16:15
//Version 1.15 Quelle Code zum debugging Abschnittweise zusammengesetzt
//Version 1.15 MQTT_Test_wg_HX711_V1.1 - V1.1f sind die einzel Schritte dahin
//Version 1.15 u.a. Setup & Haupteil vereinfacht - jetzt DeepSleep in Main Loop aufgerufen, fürher separater Codezweig in SetupPfad
//Version 1.15 Code passt zu Fritzing Bienenwaage 4.2 5V CN3064.fzz
//Version 1.15 Kalibrierung mit Sketch "Kalibrierung f_r_Waage_3.0_Demos_" durchgeführt
//Version 1.15 Aktuell nur ohne Sensor SD18B20 möglich da PIN für Waage benötigt wird
//Version 1.15 Farbecode Anschlußkabel BOSCHE H40A
/*
+E Rot
+A Grün
-E Schwarz
-A Weiß
Shield Lila
*/
//Version 1.15 ToDo Test ob Jumper D8 möglich der nach dem Booten geschlossen wird.
//Version 1.16 Zusätzlicher Sleep falls mqtt Connect nicht klappt - 27.04.19 im Test
//Version 1.17 Zusätzlicher Sleep falls GSM oder GPRS Connect nicht klappt, zustäzlich stoppen des Modems bevor mqtt sleep der Version 1.16 bei fehlerhaften mqtt connect. -30.04.19 im Test
//Version 1.18 Geschwindigkeit für SoftwareSerical von  57600  auf 9600 Baud gesenkt (Artikel gefunden der sagt, das es hier zu Problemen auf der TX/RX Leitung kommen kann)
//Version 1.19 production für Einsatz - Arduino Version 1.8.7 / ESP Version 2.3.0 / SDK 1.5.3_16_04_18 / seit 30.05.2019
//Version 1.19 Echtdaten werden an als node-2 geführt deshabl spielwiese/node-2/data.json, anstatt node-4 an den bisher zu testzwecken übermittelt wurde.
//Version 1.20 Version um Tippfehler bereinigt und für Upload auf GitHub vorbereitet
//Version 1.20 public - User / Login Credentials entfernt


#define GSM_ENABLED             true    //Bei FALSE wird automatisch WIFI aktiviert
#define WEIGHT                  true
#define SENSOR_DS18B20          false  // Aktuell ohne Funktion, da zu wenig PINS zwangsweise false da ich pin D5 für die Waage benötige
#define SENSOR_BATTERY_LEVEL    false  // falls Spannungsmessung über Spannungsteiler erfolgen soll, wenn kein SIM800 Modul verwendet wird.
#define DEEPSLEEP_ENABLED       true   // Code ist aktuell nur auf TRUE ausgelegt, falls False, muß noch im main() ein Delay eingebaut werden.
#define SLEEP_TIMER             true   // SleepDauer abhängig vom Ladezustand, Sleep_Timer noch nicht mit Wifi verifziert.
#define WUNDERGROUND            false  //funktionert aktuell nur mit GSM_ENABLED false


#if GSM_ENABLED

  #define WIFI_ACTIVE false
  
  #define TINY_GSM_MODEM_SIM800
  #include <TinyGsmClient.h>
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(D7, D6); // RX, TX
  
  const char apn[]  = "pinternet.interkom.de";  //Abhängig vom Netzprovider - ggfs.auf eigene Provider  anpassen.
  const char user[] = "";                       //Abhängig vom Netzprovider - in Abhängigkeit vom Provider eintragen
  const char pass[] = "";                       //Abhängig vom Netzprovider - in Abhängigkeit vom Provider eintragen
  
  int gsm_csq;
  
#else

  #define WIFI_ACTIVE true
  
#endif

#if SENSOR_BATTERY_LEVEL
  int adc_level;
#endif

// Variablen für Spanungsmessung, werden unabhängig der Messmethode (Spannungsteiler o. SIM800 Modul benötigt)
int volt_level;
float voltage;

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>


//Library for WifiManager
#include <ESP8266WebServer.h>

#if WIFI_ACTIVE
  #include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#endif

#if SLEEP_TIMER

  #include <EEPROM.h>
  int power_save_mode;
  int address = 1;     // Willkürlich von mir festgelegt das im EEPROM an Adresse 1 der Wert für den Ladezustand abgelegt wird.
  int voltbe4;         // Wert der vorherigen Messung 
  int voltbelow = 75;  // Schwellwert für die Aktivierung des PowerSave Modus (Verlängerung des SleepTimers auf 60 min)
  
#endif

int sleepTimeS = 900;  // 15-Minuten SleepTimer als Startwert

// Adafruit MQTT
// https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// JSON serializer
// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>

// ----
// MQTT
// ----

// How often to retry connecting to the MQTT broker
#define MQTT_RETRY_COUNT    5

// How long to delay between MQTT (re)connection attempts (seconds)
#define MQTT_RETRY_DELAY    1.5f

// The address of the MQTT broker to connect to
#define MQTT_BROKER         "swarm.hiveeyes.org"
#define MQTT_PORT           1883

// A MQTT client ID, which should be unique across multiple devices for a user.
// Maybe use your MQTT_USERNAME and the date and time the sketch was compiled
// or just use an UUID (https://www.uuidgenerator.net/) or other random value.
#define MQTT_CLIENT_ID      "xxxxxxxx-yyyy-xxxx-yyyy-zzzzzzzzzzzz"                   // Hier eigenen Werte eintragen


// The credentials to authenticate with the MQTT broker
#define MQTT_USERNAME       "DeinLoginName"                                            // Eigenen Login Daten bei den netten Team von Hiveeyes via hello@hiveeyes.org anfragen.
#define MQTT_PASSWORD       "DeinKennwort"

// The MQTT topic to transmit sensor readings to.
// Note that the "testdrive" channel is not authenticated and can be used anonymously.
// To publish to a protected data channel owned by you, please ask for appropriate
// credentials at https://community.hiveeyes.org/ or hello@hiveeyes.org.
    
#define MQTT_TOPIC          "hiveeyes/xxxxxxxx-yyyy-xxxx-yyyy-zzzzzzzzzzzz/spielwiese/node-1/data.json" 
//#define MQTT_TOPIC        "hiveeyes/-------MQTT-CLIENT-ID-von-oben------/spielwiese/node-1/data.json"     //ggfs. beim Pfad beim Team von hiveeyes noch nachfragen.

#if GSM_ENABLED

  // Client für GSM hier aufrufen
  TinyGsm modem(SerialAT);
  TinyGsmClient client(modem);
  TinyGsmClient clientMQTT(modem);

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
  
  #define SCALE_DOUT_PIN_A D5 // DT
  #define SCALE_SCK_PIN_A D3 // SCK

  #define SCALE_DOUT_PIN_B D2 // DT
  #define SCALE_SCK_PIN_B D1 // SCK

  HX711 scale_A(SCALE_DOUT_PIN_A, SCALE_SCK_PIN_A);
  HX711 scale_B(SCALE_DOUT_PIN_B, SCALE_SCK_PIN_B);

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

  // Temperatur Sensoren DS18B20 connectet to Pin D5
  #define DS18B20_PIN D5
  
  // 1-Wire library
  #include <OneWire.h>
  
  // DS18B20/DallasTemperature library
  #include <DallasTemperature.h>
  
  // For communicating with any 1-Wire device (not just DS18B20)
  OneWire oneWire(DS18B20_PIN);
  
  // Initialize DallasTemperature library with reference to 1-Wire object
  DallasTemperature sensors(&oneWire);
  
  // Device Adresses - dedected by oneWireSearch.ino or Multiple similar
  // Update device adress
  
  uint8_t Sensor1 [8] = { 0x28, 0xFF, 0xCF, 0xBD, 0xA4, 0x16, 0x04, 0x46 };  // Nr. 3 grüne Schrift
  uint8_t Sensor2 [8] = { 0x28, 0xFF, 0x67, 0x65, 0x51, 0x16, 0x04, 0xEE }; // langes Kabel
  
  // Define Variable to hold temperature value for Sensor1
  float temps1;
  
  // Define Variable to hold temperature value for Sensor2
  float temps2;

#endif

#if WUNDERGROUND

  // Weatherundeground
  const char* weatherhost = "api.wunderground.com";
  
  // TextFinder Lib used for text extractation of XML Data
  #include <TextFinder.h>
  
  // Define variable as "char" as they will be extracted via finder.string of XML Data
  char currentTemp[8];
  char currentHumidity[8];
  
  // Weatherunderground Station - Alternative Station IMNCHEN1945
  const char*  WUG_Station = "IBAYERNM74";

#endif

void gsm_setup();
void gsm_connect();
void gsm_disconnect();
void gsm_info();
void gsm_poweron();
void gsm_SleepMode2On();
void gsm_SleepModeOff();
bool mqtt_connect();
void transmit_reaadings();
void setup_weight();
void read_weight();
void setup_tempsensor();
void read_tempsensor();
void read_battery_level();
void power4sleep();
void read_weatherunderground();

void setup() {
  
Serial.begin(115200);

#if WIFI_ACTIVE

  Serial.println("Booting Wifi Mode ");

  // Start Wifi Manager to connect
  WiFiManager wifiManager;

  wifiManager.setConfigPortalTimeout(60);
  wifiManager.autoConnect("AutoConnectAP");

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");

    Serial.println("ESP8266 in sleep for 1 min mode");
    ESP.deepSleep(60 * 1000000, WAKE_RF_DEFAULT);
    delay(100);

  }

  Serial.println(WiFi.localIP().toString());
  Serial.println(WiFi.SSID());

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

#endif

#if GSM_ENABLED

  Serial.println(" ->");
  Serial.println("Booting GSM Mode");

  // gsm_poweron();
  // delay(1000); // Eine Sekunde warten bis GSM Modul mit Strom versorgt ist;

  gsm_setup();

  gsm_SleepModeOff();

  gsm_connect();

  gsm_info();

#endif

setup_weight();

setup_tempsensor();

}

void loop() {
  
  if (!mqtt_connect()) {
    return;
  }

  read_weight();

  read_tempsensor();

  read_battery_level();

  read_weatherunderground();

  #ifdef SLEEP_TIMER
    power4sleep();
  #endif

  transmit_readings();

  delay(1000);

  #if GSM_ENABLED
    gsm_disconnect();
  #endif

  #if DEEPSLEEP_ENABLED
      ESP.deepSleep(sleepTimeS * 1000000, WAKE_RF_DEFAULT);
      delay(100);
  #endif

  //Hier fehlt aktuell noch: Was machen wir wenn kein DeepSleep, sprich wir in Main() bleiben
  //delay(60*1000)

}

void setup_weight() {

  #if WEIGHT
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
      GewichtEinzelmessung_A = scale_A.read();
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

  // Set GSM module baud rate
  SerialAT.begin(9600);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  // Unlock your SIM card with a PIN - falls SIM mit PIN belegt ist hier die PIN hinterlegen.
  // modem.simUnlock("1234");

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

  Serial.print("GPRS Connecting to ");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);

  // Falls die Verbindung mit dem GSM & GPRS Netz nicht klappt, legt sich der der ESP schlafen und wir starten im Anschluß von vorn.
  // GSM Modem stoppen

     gsm_disconnect();

    // Sleep für 60 Sekunden

    ESP.deepSleep(60 * 1000000, WAKE_RF_DEFAULT);
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

  Serial.println("Batterie Level ");
  Serial.println(volt_level);

  Serial.println("Batterie Level ");
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

          ESP.deepSleep(60 * 1000000, WAKE_RF_DEFAULT);
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
  sensors.setResolution(TEMP_12_BIT);

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
    sleepTimeS = 3600;
    power_save_mode = 1;
  }
  else
  {
    // Wenn Ladungswert über  75% ist die Sleep Dauer auf 15 min (=900 Sekunden) gesetzt
    sleepTimeS = 900;
    power_save_mode = 0;
  }

    Serial.print("Set Sleep Timer to:");
    Serial.println(sleepTimeS);
    Serial.print("Set Power Save to:");
    Serial.println(power_save_mode);
    
#endif

}

void read_weatherunderground() {

  //Funktion liefert aktuelle Temperatur und Luftfreuchte einer Wetterstation in der Umgebung von Weatherunderground 
  // Bei mir ist die Station IBAYERNM74 - ggfs. über die Webseite von Weatherunderground eine in der nähe gelegenen Station auswählen.

#if WUNDERGROUND

  if (client.connect(weatherhost, 80))
  {
    String wurl = "/weatherstation/WXCurrentObXML.asp?ID=IBAYERNM74" ;
    client.print(String("GET ") + wurl + " HTTP/1.1\r\n" + "Host: " + weatherhost + "\r\n" +  "Connection: close\r\n\r\n");

    while (!client.available()) {
      //  delay(200);
      delay(10000);
      Serial.print("client not availabel connect: ");
    }

    // Read all the lines of the reply from server and print them to Serial
    TextFinder finder(client);  // Keine Ahnung wozu - aber ohne intanzierung läuft der Textfinder nicht

    //   while (client.available()) {
    String line = client.readStringUntil('\r');

    finder.getString("<temp_c>", "</temp_c>", currentTemp, 8) ;

    finder.getString("<relative_humidity>", "</relative_humidity>", currentHumidity, 8) ;

    Serial.println("closing connection");
  }

#endif

}

void transmit_readings() {

  // Build JSON object containing sensor readings
  // TODO: How many data points actually fit into this?
  StaticJsonBuffer<1024> jsonBuffer;


  // Create telemetry payload by manually mapping sensor readings to telemetry field names.
  // Note: For more advanced use cases, please have a look at the TerkinData C++ library
  //       https://hiveeyes.org/docs/arduino/TerkinData/README.html
  JsonObject& json_data = jsonBuffer.createObject();

  #if WUNDERGROUND
    json_data["WXD_Temp"]        = currentTemp;
    json_data["WXD_Humi"]        = currentHumidity;
  #endif

  #if SENSOR_DS18B20
    json_data["Temp1"]           = temps1;
    json_data["Temp2"]           = temps2;
  #endif

  #if WEIGHT
    json_data["Waage1"]          = AktuellesGewicht_A;
    json_data["Waage2"]          = AktuellesGewicht_B;
  #endif

  #if GSM_ENABLED
    json_data["Signal"]          = gsm_csq;
  #endif

  #if SLEEP_TIMER
     json_data["power"]          = power_save_mode;
  #endif

  json_data["volt_level"]        = volt_level;
  json_data["voltage"]           = voltage;

  // Debugging
  json_data.printTo(Serial);


  // Serialize data
  int json_length = json_data.measureLength();
  char payload[json_length + 1];
  json_data.printTo(payload, sizeof(payload));

  // Publish data
  // TODO: Refactor to TerkinTelemetry
  if (mqtt_publisher.publish(payload)) {
    Serial.println("MQTT publish succeeded");
  } else {
    Serial.println("MQTT publish failed");
  }

}
