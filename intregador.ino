#include <Arduino.h>
#include <SoftwareSerial.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <FirebaseClient.h>
#include <WiFiClientSecure.h>

#define WIFI_SSID "WIFI BIS"
#define WIFI_PASSWORD ""

#define DATABASE_SECRET "NKd1sRTRksjFkRGWzVdbNV0sH6YZrNViGfg5u5W1"
#define DATABASE_URL "https://greengo-ec-default-rtdb.firebaseio.com"

WiFiClientSecure ssl;
DefaultNetwork network;
AsyncClientClass client(ssl, getNetwork(network));

FirebaseApp app;
RealtimeDatabase Database;
AsyncResult result;
LegacyToken dbSecret(DATABASE_SECRET);

SoftwareSerial mySerial(4, 5);         // RX, TX para SIM808
SoftwareSerial arduinoSerial(14, 12);  // RX, TX para Arduino (GPIO 12, GPIO 14)

String latitude = "", longitude = "";
int rpm = 0;
float temperatura = 0;
float oxigenacion = 0;
float combustion = 0;
int emisionGases = 0;
int consumoElectrico = 0;
int velocidad = 0;

String gpsData[5];
bool gpsUpdated = false;

unsigned long lastGPSUpdate = 0;
unsigned long lastArduinoUpdate = 0;
const unsigned long gpsInterval = 500;
const unsigned long arduinoInterval = 1400;

void printError(int code, const String &msg) {
  Serial.printf("Error, msg: %s, code: %d\n", msg.c_str(), code);
}

void sendGPSData() {
  if (!latitude.equals("00.0000") && !longitude.equals("00000.00000") && latitude.length() > 0 && longitude.length() > 0) {
    bool status = Database.set<String>(client, "/buses/id_bus/GPS/lat", latitude);
    if (status)
      Serial.println("Latitud enviada correctamente");
    else
      printError(client.lastError().code(), client.lastError().message());

    status = Database.set<String>(client, "/buses/id_bus/GPS/lng", longitude);
    if (status)
      Serial.println("Longitud enviada correctamente");
    else
      printError(client.lastError().code(), client.lastError().message());
  }
}

void sendArduinoData() {
  Serial.println("Revoluciones: " + String(rpm) + ", Temperatura: " + String(temperatura) + ", Oxigenacion: " + String(oxigenacion) + ", Combustion: " + String(combustion) + ", Emision de gases: " + String(emisionGases) + ", Consumo Electrico: " + String(consumoElectrico) + ", velocidad: " + String(velocidad));

  bool status = Database.set<int>(client, "/buses/id_bus/RPM", rpm);
  if (status)
    Serial.println("RPM enviado correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<float>(client, "/buses/id_bus/temperatura", temperatura);
  if (status)
    Serial.println("Temperatura enviada correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<float>(client, "/buses/id_bus/oxigenacion", oxigenacion);
  if (status)
    Serial.println("Oxigenación enviada correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<float>(client, "/buses/id_bus/combustion", combustion);
  if (status)
    Serial.println("Combustión enviada correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<int>(client, "/buses/id_bus/emision_de_gases", emisionGases);
  if (status)
    Serial.println("Emisión de gases enviada correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<int>(client, "/buses/id_bus/consumo_electrico", consumoElectrico);
  if (status)
    Serial.println("Consumo eléctrico enviado correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());

  status = Database.set<int>(client, "/buses/id_bus/velocidad", velocidad);
  if (status)
    Serial.println("Velocidad enviada correctamente");
  else
    printError(client.lastError().code(), client.lastError().message());
}

void getGPS() {
  if (latitude.length() == 0 && longitude.length() == 0) {
    mySerial.println("AT+CGPSPWR=1");
    delay(250);
  }

  String comando = "AT+CGPSINF=2";
  mySerial.println(comando);
  delay(250);

  if (comando.startsWith("AT+CGPSINF=2")) {
    if (mySerial.available()) {
      String respuesta = mySerial.readString();
      String coordenadas = respuesta.substring(40);
      delay(100);
      coordenadas.replace("N,", "");
      coordenadas.replace(",W,", "");
      coordenadas.replace(",E,", "");
      coordenadas.replace("S,", "");

      int commaIndex = coordenadas.indexOf(',');
      latitude = coordenadas.substring(0, commaIndex);
      longitude = coordenadas.substring(commaIndex + 1);

      gpsUpdated = true;
    }
  }
}

void setup() {
    Serial.begin(9600);
    mySerial.begin(9600);
    arduinoSerial.begin(9600);
    delay(2000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        Serial.print(".");
        delay(1000);
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.print("Connected with IP: ");
        Serial.println(WiFi.localIP());
        Serial.println();
    } else {
        Serial.println();
        Serial.println("Failed to connect to Wi-Fi");
        while (true);
    }

    Serial.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    ssl.setInsecure();
#if defined(ESP8266)
    ssl.setBufferSizes(1024, 1024);
#endif

    initializeApp(client, app, getAuth(dbSecret));
    app.getApp<RealtimeDatabase>(Database);
    Database.url(DATABASE_URL);
    client.setAsyncResult(result);
}

void loop() {
    unsigned long currentMillis = millis();

    // Manejo de datos GPS
    if (currentMillis - lastGPSUpdate >= gpsInterval) {
        lastGPSUpdate = currentMillis;
        getGPS();
        if (gpsUpdated) {
            sendGPSData();
            gpsUpdated = false;
        }
    }

    // Manejo de datos del Arduino
    if (currentMillis - lastArduinoUpdate >= arduinoInterval) {
        lastArduinoUpdate = currentMillis;

        bool arduinoDataReceived = false;

        if (arduinoSerial.available()) {
            String data = arduinoSerial.readString();

            int commaIndex = 0;
            int nextCommaIndex = data.indexOf(',', commaIndex);
            rpm = data.substring(commaIndex, nextCommaIndex).toInt();

            commaIndex = nextCommaIndex + 1;
            nextCommaIndex = data.indexOf(',', commaIndex);
            temperatura = data.substring(commaIndex, nextCommaIndex).toFloat();

            commaIndex = nextCommaIndex + 1;
            nextCommaIndex = data.indexOf(',', commaIndex);
            oxigenacion = data.substring(commaIndex, nextCommaIndex).toFloat();

            commaIndex = nextCommaIndex + 1;
            nextCommaIndex = data.indexOf(',', commaIndex);
            combustion = data.substring(commaIndex, nextCommaIndex).toFloat();

            commaIndex = nextCommaIndex + 1;
            nextCommaIndex = data.indexOf(',', commaIndex);
            emisionGases = data.substring(commaIndex, nextCommaIndex).toInt();

            commaIndex = nextCommaIndex + 1;
            nextCommaIndex = data.indexOf(',', commaIndex);
            consumoElectrico = data.substring(commaIndex, nextCommaIndex).toInt();

            commaIndex = nextCommaIndex + 1;
            velocidad = data.substring(commaIndex).toInt();

            arduinoDataReceived = true;
        }

        if (arduinoDataReceived) {
            sendArduinoData();
        } else {
            Serial.println("No hay conexión con el carro");
        }
    }
}