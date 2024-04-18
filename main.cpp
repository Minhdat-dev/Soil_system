#define BLYNK_TEMPLATE_ID "TMPL6lTcDMR-Z"
#define BLYNK_TEMPLATE_NAME "Khuvuontrenmay"
#define BLYNK_AUTH_TOKEN "acuEj1J1yKsrtyyUdEptl2TCkoGzJEqx"
#define BLYNK_PRINT Serial
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#define DHTPIN 19
#define DHTTYPE DHT11
#define RELAY_PIN 23
#define SOILMOISTURE_PIN 34

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "no answer";
char pass[] = "Tuantop8";
SemaphoreHandle_t xBinarySemaphore;
QueueHandle_t xQueueTemp, xQueueHumidity;
portMUX_TYPE timerMUX = portMUX_INITIALIZER_UNLOCKED;
SemaphoreHandle_t xMutex;
void readSensor(void * params);
void rtos_delay(uint32_t delay_in_ms);
void setup(){
   	 Serial.begin(115200);
  	  pinMode(RELAY_PIN, OUTPUT);
    	pinMode(SOILMOISTURE_PIN, INPUT);
    	Blynk.begin(auth,ssid,pass);
   	 xQueueTemp = xQueueCreate(1, sizeof(float));
  	  xQueueHumidity = xQueueCreate(1, sizeof(float));
    	vSemaphoreCreateBinary(xBinarySemaphore);
   	 xMutex = xSemaphoreCreateMutex();
xTaskCreatePinnedToCore(readSensor, "ReadSensor", 10000, NULL, 3,NULL, 0);
}
void loop(){
  	  Blynk.run();
}
void readSensor(void*params){
 	   float oldtemp =0;
  	  float oldhumi =0;
  	  float h;
   	 float t;
  	  DHT dht(DHTPIN, DHTTYPE);
  	  dht.begin();
    	xSemaphoreTake(xMutex, portMAX_DELAY);
    	vTaskDelay(1000/ portTICK_PERIOD_MS);
   	 for(;;){
        	vTaskDelay(1000/portTICK_PERIOD_MS);
       	 float temphu = analogRead(SOILMOISTURE_PIN);
        	h = map(temphu,0,4095,100,0);
       	 t=dht.readTemperature();
       	 if (isnan(t)||isnan(h)){
          	  Serial.println("Khong doc duoc du lieu tu DHT11!");
            	t = oldtemp;
           	 h= oldhumi;
       	 }
      	  else{
           	 oldtemp=t;
           	 oldhumi = h;
       	 }

       	 Serial.print(F("Nhiet do moi truong: "));
       	 Serial.println(t);
        	Serial.print(F("Do am dat:"));
        	Serial.println(h);
       	 if (h<35){
         	 digitalWrite(RELAY_PIN, LOW);
       	 Serial.println(F("Bat dau tuoi cay"));  
       	 }
       	 else{
           	digitalWrite(RELAY_PIN, HIGH);
           	 Serial.println(F("Dung tuoi cay"));  
      	  }
       	 Blynk.virtualWrite(V0, t);
        	Blynk.virtualWrite(V1, h);
        	rtos_delay(1000);
        	taskYIELD();
    	}
}
void rtos_delay(uint32_t delay_in_ms){
   	 uint32_t current_tick_count = xTaskGetTickCount();
   	 uint32_t delay_in_ticks = (delay_in_ms*configTICK_RATE_HZ)/1000;
  	  while(xTaskGetTickCount()<(current_tick_count+delay_in_ticks));
}
