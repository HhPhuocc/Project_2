#include <WiFiManager.h> 
#include <WiFi.h>
#include <DHT.h>
#include <FirebaseESP32.h>
#define DATABASE_URL "gdafas-917a5-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "Nx1h4sEVVcDnjzT85ZNmgA66vd2slTo95xxzFyVJ"
#define led_status_conect 22
#define led_status_noconect 21
#define btn1 23
#define flame_sensor 25
#define DHT_PIN 32
#define DHT_TYPE DHT11
#define MQ2_A 34
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long previousMillis = 0;
unsigned long interval = 30000;
volatile bool buttonPressed = false;
unsigned long buttonPressStartTime = 0;
const unsigned long buttonHoldTime = 5000;
bool res;
int NewState = 1;
int LastState = 1;
int doAction = 0;
unsigned long StartToCount = 0;
unsigned long CurrentCount = 0;
FirebaseData fbdo;
/*void IRAM_ATTR buttonInterrupt() {
  int buttonState = digitalRead(btn1);
  if(buttonState == LOW ) {
     buttonPressStartTime = millis();
     buttonPressed = false;
  }
     else
     {
     if((millis() - buttonPressStartTime >= buttonHoldTime)){
     buttonPressed = true;
    }
  }
}*/
void setup() {
    Serial.begin(115200);
    pinMode(led_status_conect, OUTPUT);
    pinMode(led_status_noconect, OUTPUT);
    digitalWrite(led_status_conect, LOW);
    digitalWrite(led_status_noconect, LOW);
    pinMode(btn1, INPUT_PULLUP);
    //attachInterrupt(digitalPinToInterrupt(btn1), buttonInterrupt, CHANGE);
    pinMode(flame_sensor, INPUT);
    pinMode(MQ2_A, INPUT);
    WiFiManager wm;
    bool res;
    res = wm.autoConnect("GDAFAS","68686868");
    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {   
        Serial.println("connected...yeey :)");
       
    }
     Firebase.begin(DATABASE_URL, DATABASE_SECRET);
     dht.begin();   
}

void loop() {
    unsigned long currentMillis = millis();
    unsigned long fbmillis = millis();
    static unsigned long fblastCheckMillis = 0;
    static unsigned long lastCheckMillis = 0;
    if (currentMillis - lastCheckMillis >= 20000) {
        lastCheckMillis = currentMillis;
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to the Internet");
            WiFi.reconnect();
            digitalWrite(led_status_conect, LOW);
            digitalWrite(led_status_noconect, HIGH);
        }
         if (WiFi.status() == WL_CONNECTED) {
             Serial.println("Reconnected to the Internet");
             digitalWrite(led_status_conect, HIGH);
             digitalWrite(led_status_noconect, LOW);
        }
            } 
    /*if (buttonPressed)
    {
      WiFiManager wm;
      wm.resetSettings();
      buttonPressed = false;
      delay(2000);
      ESP.restart();
    }*/
    

    if(fbmillis - fblastCheckMillis >=5000)
    {
      fblastCheckMillis = fbmillis;
      int flamesensor;
      if(digitalRead(flame_sensor)==HIGH)
      {
        flamesensor=0;
      }
      else
      {
        flamesensor=1;
      }
      float temp = dht.readTemperature();
      float hum = dht.readHumidity();
      int Gas_value = map(analogRead(MQ2_A), 0, 4095, 0, 100);
      Firebase.setInt(fbdo, "/Phong 1/Lua", flamesensor);
      Firebase.setFloat(fbdo, "/Phong 1/Nhiet do", temp);
      Firebase.setFloat(fbdo, "/Phong 1/Do am", hum);
      Firebase.setInt(fbdo, "/Phong 1/Khi&khoi", Gas_value);
    }
    LastState = NewState;
    NewState = digitalRead(btn1);
  if (NewState == LOW) {
    //Serial.print( NewState );
    if (LastState == HIGH && NewState == LOW) {
      StartToCount = millis();
      doAction = 1;
    }

    if (LastState == LOW && NewState == LOW) {
      CurrentCount = millis();
      if (CurrentCount - StartToCount > 3000) {
        if (doAction) {
          WiFiManager wm;
          wm.resetSettings();
          //buttonPressed = false;
          //delay(2000);
          ESP.restart();
          doAction = 0;
        }
      }
    }
  }
}
