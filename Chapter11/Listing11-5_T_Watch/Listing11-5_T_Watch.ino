/*******************************************************************************
 * Sketch name: remote ILI9341 TFT LCD screen - server
 * Description: Display images from remote ESP32-CAM
 * Created on:  February 2023
 * Author:      Neil Cameron
 * Book:        ESP32 Communication
 * Chapter :    11 - ESP32_CAM camera
 ******************************************************************************/
#define LILYGO_WATCH_2020_V2        // define T-Watch model
#include <LilyGoWatch.h>        // include library
TTGOClass * ttgo;           // associate objects with libraries
TFT_eSPI * tft; 
#include <TJpg_Decoder.h>
//#include <TFT_eSPI.h>
//TFT_eSPI tft = TFT_eSPI();
#include <WebSocketsServer.h>             // WebSocket server library
WebSocketsServer websocket = WebSocketsServer(81);  // WebSocket on port 81
char ssidAP[] = "ESP32CAM";
char passwordAP[] = "pass1234";
unsigned long last;
int FPS, FPSs[50], sum = 0, N = 0;        // circular buffer array
float mean;

void setup()
{
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();            // initialise ttgo object
  ttgo->openBL();           // turn on backlight
  ttgo->bl->adjust(64);         // reduce brightness from 255
  tft = ttgo->tft;  
  WiFi.softAP(ssidAP, passwordAP);        // initialise softAP WLAN
  Serial.print("server softAP address ");
  Serial.println(WiFi.softAPIP());        // server IP address
  websocket.begin();            // initialise WebSocket
  websocket.onEvent(wsEvent);         // wsEvent called by
//  tft.begin();
  tft->setRotation(2);
  tft->fillScreen(TFT_BLACK);
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(true);
  TJpgDec.setCallback(tftOutput);
  tft->setTextColor(TFT_RED);          // font colour display FPS
  tft->setTextSize(2);
  for (int i=0; i<50; i++) FPSs[i] = 0;     // initialise circular buffer
}

bool tftOutput (int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  if (y >= 240) return 0;
  tft->pushImage(x, y, w, h, bitmap);
  return 1;
}
              // function called on WebSocket event
void wsEvent(uint8_t num, WStype_t type, uint8_t * RXmsg, size_t length)
{
  last = millis();          // update last image time
  TJpgDec.drawJpg(0,0, RXmsg, length);    // display image on screen
  FPS = millis() - last;        // interval between images
  circular();           // update circular buffer
  tft->setCursor(120,220);       // position cursor bottom right
  tft->print("FPS ");tft->print(mean, 1);   // display FPS
}

void circular()           // function for circular buffer
{
  sum = sum - FPSs[N];          // subtract oldest value from sum
  sum = sum + FPS;          // add current value to sum
  FPSs[N] = FPS;            // update circular buffer
  N++;              // increment buffer position
  if(N > 50-1) N = 0;         // back to "start" of circular buffer
  mean = 1000.0/(sum/50.0);       // moving average FPS
}

void loop()
{
  websocket.loop();         // handle WebSocket data
}
