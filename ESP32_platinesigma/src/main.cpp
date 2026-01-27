#include <Arduino.h>
#include <WiFi.h>
 
#if ESP_IDF_VERSION_MAJOR >= 5
    #include <esp_eap_client.h>
#else
    #include "esp_wpa2.h" //Nur bei Schulnetz notwendig
#endif
 
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "credentials.h"
 
AsyncWebServer server(80);
 
void setupEAPWiFi() {
    WiFi.mode(WIFI_STA);
    #if ESP_IDF_VERSION_MAJOR >= 5
        esp_eap_client_clear_ca_cert();
        esp_eap_client_clear_certificate_and_key();
 
        // esp_eap_client_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_EAP);
        esp_eap_client_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_MSCHAPV2);
        // esp_eap_client_set_identity((const unsigned char *)WIFI_USER, strlen(WIFI_USER));
        esp_eap_client_clear_identity();
        esp_eap_client_set_username((const unsigned char *)WIFI_USER, strlen(WIFI_USER));
        esp_eap_client_set_password((const unsigned char *)WIFI_PASSWORD, strlen(WIFI_PASSWORD));
        esp_eap_client_set_disable_time_check(true);
        esp_wifi_sta_enterprise_enable();
    #else
        //setzen von username und password
        esp_wifi_sta_wpa2_ent_set_username((uint8_t *) WIFI_USER,
                strlen(WIFI_USER)); //provide username
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *) WIFI_PASSWORD,
                strlen(WIFI_PASSWORD)); //provide password
        esp_wifi_sta_wpa2_ent_enable();
    #endif
    //starten der Netzwerkverbindung
    WiFi.begin(WIFI_SSID);
    WiFi.setHostname("myESPdevice"); //set Hostname for your device
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address set: ");
    Serial.println(WiFi.localIP()); //print LAN IP  
}
 
void setFileSystem() {
    if(!LittleFS.begin(true)){
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    Serial.println("LittleFS mounted successfully");
}
 
void setupWebServer() {
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
   
    // Route für die Startseite
    server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, Sigma");
    });
 
    // Starte den Webserver
    server.begin();
}
 
 
 
void setup()
{
  Serial.begin(115200);
  setupEAPWiFi();
 
 
  setFileSystem();
 
  setupWebServer();
 
 
}
 
void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}