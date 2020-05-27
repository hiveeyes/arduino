#######################
Bienenwaage 5.0 CHANGES
#######################


***********
Development
***********
- Update URL to UUID generator. Thanks, @weef!
- Improve MQTT configuration section
- Improve build environment and add documentation
- Fix whitespace
- Add file header
- Improve wording
- Improve configuration for Weather Underground
- Use JSON format for getting information from Weather Underground
- Improve GSM SIM unlocking


****************
2020-04-26 2.0.0
****************
- Port auf ESP32


***************
2019-xx-xx 1.2x
***************
- Version 1.20 Version um Tippfehler bereinigt und für Upload auf GitHub vorbereitet
- Version 1.20 public - User / Login Credentials entfernt


***************
2019-xx-xx 1.1x
***************
- Version 1.14 Version 1.10-1.13 verworfen und auf Basis 1.09 aufgesetzt.
- Version 1.14 OTA Teil entfernt - Test 0k 15:27
- Version 1.14 HX711 Messung  / Gewicht integriert Test 0k 15:38 + 16:15
- Version 1.15 Quelle Code zum debugging Abschnittweise zusammengesetzt
- Version 1.15 MQTT_Test_wg_HX711_V1.1 - V1.1f sind die einzel Schritte dahin
- Version 1.15 u.a. Setup & Haupteil vereinfacht - jetzt DeepSleep in Main Loop aufgerufen, fürher separater Codezweig in SetupPfad
- Version 1.15 Code passt zu Fritzing Bienenwaage 4.2 5V CN3064.fzz
- Version 1.15 Kalibrierung mit Sketch "Kalibrierung f_r_Waage_3.0_Demos_" durchgeführt
- Version 1.15 Aktuell nur ohne Sensor SD18B20 möglich da PIN für Waage benötigt wird
- Version 1.15 Farbcode Anschlußkabel BOSCHE H40A
- Version 1.15 ToDo Test ob Jumper D8 möglich der nach dem Booten geschlossen wird.
- Version 1.16 Zusätzlicher Sleep falls mqtt Connect nicht klappt - 27.04.19 im Test
- Version 1.17 Zusätzlicher Sleep falls GSM oder GPRS Connect nicht klappt, zustäzlich stoppen des Modems bevor mqtt sleep der Version 1.16 bei fehlerhaften mqtt connect. -30.04.19 im Test
- Version 1.18 Geschwindigkeit für SoftwareSerical von  57600  auf 9600 Baud gesenkt (Artikel gefunden der sagt, das es hier zu Problemen auf der TX/RX Leitung kommen kann)
- Version 1.19 production für Einsatz - Arduino Version 1.8.7 / ESP Version 2.3.0 / SDK 1.5.3_16_04_18 / seit 30.05.2019
- Version 1.19 Echtdaten werden an als node-2 geführt deshabl spielwiese/node-2/data.json, anstatt node-4 an den bisher zu Testzwecken übermittelt wurde.


***************
2019-xx-xx 1.0x
***************
- Version 1.01 Spannungs Messung und Deep Sleep
- Version 1.02 MQTT Anbindung
- Version 1.03 Temperatur Meßung
- Version 1.03 Anpassen des Wertes #define MAXBUFFERSIZE (1024) in Adafruit_MQTT.h, im Ordner libraries/Adafruit_MQTT_Library/afruit_MQTT.h
- Version 1.04 WiFiClient clientMQTT anstatt client
- Version 1.05 Add to ESP.deepSleep  WAKE_RF_DEFAULT
- Version 1.06 Grundzüge GSM integriert
- Version 1.01 basiert auf Waage 1.06 mit ESP8266 - ab hier Versuch mit Wemos D1 mini Board.
- Version 1.02 Transmit String json_data in Abhängigkeit der #define Sensor Section.
- Version 1.02 MQTT Topic auf -node3 geändert
- Version 1.02 SerialAT.begin(115200) auf SerialAT.begin(57600) reduziert
- Version 1.03 WUNDERGROUND ist noch nicht funktionsfähig siehe Funktion ..._weatheronly
- Version 1.03 Funktion gsm_info() gibt Informationen zu Signal Qualität & Spannungsversorgung des GSM Moduls auf Console aus.
- Version 1.04 GSM Modul per MOSFET & Daten Pin ein / ausschalten https://forum.arduino.cc/index.php?topic=383081.30 mittels Channel MOSFET
- Version 1.04 Nicht im Einsatz
- Version 1.05 2. China Modul - über Pin D3 initalisieren - ohne MOSFET - GSM Power on/off umgeschrieben
- VERSION 1.05 läuft mit beiden Modulen - versuch sleepEnable zu nutzen
- Version 1.06 Funktionen sleepEnable & sleepoff in gsm_SleepMode2On & gsm_SleepMode2Off umbenannt
- Version 1.06 Funktion gsm_poweron wird aktuell nicht benötigt - wg. Sequenz behalten.
- Version 1.07 separte Version hier nicht hinterlegt - Versuch der Telegramm Einbindung
- Version 1.08 Sleep Dauer abhängig vom Ladezustand des Akkus
- Version 1.08 ToDo:Sleep Timer Abhängig von Wifi / GSM machen oder andere Adressbereich für Ablage der Daten
- Version 1.08 Anpassen des Wertes #define MAXBUFFERSIZE (2048) in Adafruit_MQTT.h, im Ordner libraries/Adafruit_MQTT_Library/afruit_MQTT.h
- Version 1.09 Arduino 1.8.7 (evtl. war vorher 1.8.5, bin mir aber nicht mehr sicher)
- Version 1.09 #define DS18B20_PIN D2 auf D5 geändert
- Version 1.09 DS18820 Adressen auf 28FFCFBDA4160446 korrigiert - Identifikation mit oneWireSearch.ino
