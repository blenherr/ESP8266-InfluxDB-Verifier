/*
 * ESP8266 InfluxDB Verifier
 * AUTHOR: Benjamin Lenherr
 * https://github.com/blenherr/ESP8266-InfluxDB-Verifier
 *
 * This is a way to verify your InfluxDB configuration
 * on a ESP8266 while AsyncWebServer is running.
 */

#include <Arduino.h>

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESPAsyncWebServer.h>
AsyncWebServer server(80);
#include <LittleFS.h>

const char *WIFI_SSID = "";
const char *WIFI_PASS = "";

#define DEBUG

typedef struct
{
    char url[129];
    char meas[33];
    uint8_t ver;
    char db[33];
    uint8_t auth;
    char user[33];
    char pass[33];
    char buck[33];
    char org[33];
    char tok[101];
} influxData_t;
influxData_t _influxTest;

bool influxTest = false;
uint16_t influxTestResponseCode = 0;
String influxTestResponseString = "";

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println();
    Serial.println();
#endif
    WiFi.mode(WIFI_STA);
#ifdef DEBUG
    Serial.print(F("(WIFI) Connecting to \'"));
    Serial.print(WIFI_SSID);
    Serial.print(F("\' ."));
#endif
    uint8_t wifiTry = 0;
    WiFi.setOutputPower(20.5);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED)
    {
#ifdef DEBUG
        Serial.print(F("."));
#endif
        delay(250);
        wifiTry++;
        if (wifiTry >= 60)
        {
#ifdef DEBUG
            Serial.println(F("failed!"));
            Serial.println(F("(WIFI) Can not connect!"));
            Serial.println(F("(ESP8266) Reboot!"));
#endif
            ESP.restart();
        }
    }
#ifdef DEBUG
    Serial.print(F("connected! IP: "));
    Serial.print(WiFi.localIP().toString());
    Serial.print(F(", RSSI: "));
    Serial.println(WiFi.RSSI());
#endif
    if (WiFi.status() == WL_CONNECTED)
    {
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/html/style.css", "text/css"); });
        server.on("/config.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/html/config.js", "text/javascript"); });
        server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/html/config.html", "text/html"); });
        server.on("/influx_test", HTTP_POST, [](AsyncWebServerRequest *request)
                  {
                    bool error = false;
                    // Url
                    if(!request->hasArg("influx_url") || request->arg("influx_url").length() == 0)
                        error = true;
                    else
                        strcpy(_influxTest.url, request->arg("influx_url").c_str());
                    // Measurement
                    if(!request->hasArg("influx_meas") || request->arg("influx_meas").length() == 0)
                        error = true;
                    else
                        strcpy(_influxTest.meas, request->arg("influx_meas").c_str());
                    // Version
                    if(!request->hasArg("influx_ver") || request->arg("influx_ver").length() == 0)
                        error = true;
                    else
                        _influxTest.ver = request->arg("influx_ver").toInt();
                    // Database
                    if((!request->hasArg("influx_db") || request->arg("influx_db").length() == 0) && request->arg("influx_ver").toInt() == 1)
                        error = true;
                    else
                        strcpy(_influxTest.db, request->arg("influx_db").c_str());
                    // Authentication
                    if((!request->hasArg("influx_auth") || request->arg("influx_auth").length() == 0) && request->arg("influx_ver").toInt() == 1)
                        error = true;
                    else
                        _influxTest.auth = request->arg("influx_auth").toInt();
                    // Username
                    if((!request->hasArg("influx_user") || request->arg("influx_user").length() == 0) && request->arg("influx_auth").toInt() == 1 && request->arg("influx_ver").toInt() == 1)
                        error = true;
                    else
                        strcpy(_influxTest.user, request->arg("influx_user").c_str());
                    // Password
                    if((!request->hasArg("influx_pass") || request->arg("influx_pass").length() == 0) && request->arg("influx_auth").toInt() == 1 && request->arg("influx_ver").toInt() == 1)
                        error = true;
                    else
                        strcpy(_influxTest.pass, request->arg("influx_pass").c_str());
                    // Bucket
                    if((!request->hasArg("influx_buck") || request->arg("influx_buck").length() == 0) && request->arg("influx_ver").toInt() == 2)
                        error = true;
                    else
                        strcpy(_influxTest.buck, request->arg("influx_buck").c_str());
                    // Organization
                    if((!request->hasArg("influx_org") || request->arg("influx_org").length() == 0) && request->arg("influx_ver").toInt() == 2)
                        error = true;
                    else
                        strcpy(_influxTest.org, request->arg("influx_org").c_str());
                    // Token
                    if((!request->hasArg("influx_tok") || request->arg("influx_tok").length() == 0) && request->arg("influx_ver").toInt() == 2)
                        error = true;
                    else
                        strcpy(_influxTest.tok, request->arg("influx_tok").c_str());

                    if (!error){
                        influxTest = true;
                        influxTestResponseCode = 1000;
                    }
                    request->send((error) ? 500 : 200, "text/plain", (error) ? F("Something went wrong") : F("OK")); });
        server.on("/influx_progress", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/plain", influxTestResponseString); });
    }
    LittleFS.begin();
    server.begin();
#ifdef DEBUG
    Serial.println(F("(SERVER) Running!"));
#endif
}

void loop()
{
    if (influxTest)
    {
        influxTestResponseCode = 1000;
        influxTestResponseString = "";

        WiFiClient client;
        HTTPClient http;

        if (_influxTest.ver == 1)
        {
            if (_influxTest.auth)
                http.begin(client, String(_influxTest.url) + F("/write?db=") + String(_influxTest.db) + F("&u=") + String(_influxTest.user) + F("&p=") + String(_influxTest.pass));
            else
                http.begin(client, String(_influxTest.url) + F("/write?db=") + String(_influxTest.db));
        }
        else if (_influxTest.ver == 2)
        {
            http.begin(client, String(_influxTest.url) + F("/api/v2/write?org=") + String(_influxTest.org) + F("&bucket=") + String(_influxTest.buck));
            http.addHeader(F("Authorization"), F("Token ") + String(_influxTest.tok));
        }
        else
            return;

        http.addHeader("Content-Type", "text/plain; charset=utf-8");
        http.addHeader("Accept", "application/json");

        const char *headerKeys[] = {"X-Influxdb-Version", "x-influxdb-version"};
        const size_t numberOfHeaders = 2;
        http.collectHeaders(headerKeys, numberOfHeaders);

        int httpResponseCode = http.POST("");

        String versionUpper = http.header("X-Influxdb-Version");
        String versionLower = http.header("x-influxdb-version");
        versionUpper.replace("v", "");
        versionLower.replace("v", "");
#ifdef DEBUG
        Serial.println(http.getString());
#endif
        if (httpResponseCode >= 200 && httpResponseCode < 300)
            influxTestResponseString = F("{\"version\":\"") + versionUpper + versionLower + F("\", \"result\":\"Verified!\"}");
        else if (httpResponseCode >= 400 && httpResponseCode < 500)
        {
            if (http.getString().length() > 0)
            {
                StaticJsonDocument<128> doc;
                DeserializationError error = deserializeJson(doc, http.getString());
                if (error)
                    influxTestResponseString = F("{\"version\":\"") + versionUpper + versionLower + F("\", \"result\":\"This Wemos server deserializeJson() failed!\"}");
                else
                {
                    String result;
                    if (doc.containsKey("error"))
                    {
                        result = doc["error"].as<String>();
                        result.replace("\"", "\\\"");
                    }
                    else if (doc.containsKey("message"))
                    {
                        result = doc["message"].as<String>();
                        result.replace("\"", "\\\"");
                    }
                    else
                        result = F("This Wemos server deserializeJson() failed!");
                    const char firstChar = result.charAt(0);
                    result.setCharAt(0, toupper(firstChar));
                    influxTestResponseString = F("{\"version\":\"") + versionUpper + versionLower + F("\", \"result\":\"") + result + F("\"}");
                }
            }
        }
        else
        {
            String errorString = http.errorToString(httpResponseCode);
            const char firstChar = errorString.charAt(0);
            errorString.setCharAt(0, toupper(firstChar));
            influxTestResponseString = F("{\"version\":\"unknown\", \"result\":\"") + errorString + F("\"}");
        }
#ifdef DEBUG
        Serial.print(F("(INFLUX) Send string: "));
        Serial.println(influxTestResponseString);
#endif
        http.end();
        influxTest = false;
    }
}
