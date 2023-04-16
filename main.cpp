#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <Wifi.h>
#include <esp_now.h>

#include <protocol2copy.h>

#define SDA 21
#define SCL 22
#define LED 2
#define POTI 33

struct_message nowMessageSend;
struct_message nowMessageRecv;
esp_now_peer_info_t peerInfo;

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(SCL, SDA, U8X8_PIN_NONE);

volatile uint16_t actionCount;
volatile uint16_t bpm;
volatile uint16_t timelineCycleState[2];
volatile uint16_t timelineControlState;
volatile uint8_t source;
volatile boolean dataToDisplay;
volatile uint16_t potVal;
volatile uint8_t loopVolume;
volatile uint8_t lastLoopVolume;

const uint8_t POTI_DEBOUNCE = 200;
volatile long nowTime;
volatile long lastTime;


void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) { 
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("BOARD ID: ");
  Serial.println(source);
  memcpy(&nowMessageRecv, incomingData, sizeof(nowMessageRecv));
  source = nowMessageRecv.sourceID;
  if (source == 1){
    bpm = nowMessageRecv.sendBPM;
    actionCount = nowMessageRecv.sendActionCount;
  }
  else if (source == 3){
    timelineCycleState[0] = nowMessageRecv.sendTimelineCycleState[0];
    timelineCycleState[1] = nowMessageRecv.sendTimelineCycleState[1];
    timelineControlState = nowMessageRecv.sendTimelineControlState;
  }
  dataToDisplay = true;
}

void setup() {
  Serial.begin(115200);
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setCursor(0,0);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  memcpy(peerInfo.peer_addr, MAC_ESP3, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  pinMode(POTI, INPUT);
}

void loop() {
  nowTime = millis();
  if (nowTime > lastTime + POTI_DEBOUNCE){
    potVal = analogRead(POTI);
    loopVolume = map(potVal,0,4095,0,127);
    if (loopVolume < (lastLoopVolume - 1) || loopVolume > (lastLoopVolume + 1)){
      dataToDisplay = true;
      lastLoopVolume = loopVolume;
      nowMessageSend.sourceID = 2;
      nowMessageSend.destinationID = 3;
      nowMessageSend.messageType = LOOP_LEVEL; 
      nowMessageSend.sendLoopVolume = loopVolume;
      esp_now_send(0, (uint8_t *) &nowMessageSend, sizeof(nowMessageSend));
      Serial.print("I've sent new loop volume: ");
      Serial.println(loopVolume);
    }
    lastTime = nowTime;
  }
  if (dataToDisplay){
    u8x8.clearLine(2);
    u8x8.setCursor(0,2);
    u8x8.print("BPM:  ");
    u8x8.print(bpm);
    u8x8.clearLine(3);
    u8x8.setCursor(0,3);
    u8x8.print("BAR:  ");
    u8x8.print(actionCount);
    u8x8.clearLine(5);
    u8x8.setCursor(0,5);
    u8x8.print("CYC:  ");
    u8x8.print(timelineCycleState[0]);
    u8x8.print(".");
    u8x8.print(timelineCycleState[1]);
    u8x8.clearLine(6);
    u8x8.setCursor(0,6); 
    u8x8.print("VOL:  "); 
    u8x8.print(loopVolume); 
    u8x8.clearLine(7);
    u8x8.setCursor(0,7);
    if (timelineControlState & (1 << 7)){
      Serial.println("UNDO REQ");
      u8x8.print("UNDO REQ");
    }
    else if (timelineControlState & (1 << 6)){
      Serial.println("REC REQ");
      u8x8.print("REC REQ");
    }
    else if (timelineControlState & (1 << 5)){
      Serial.println("RECORDING");
      u8x8.print("RECORDING");
    }
    else if (timelineControlState & (1 << 4)){
      Serial.println("PLAY REQ");
      u8x8.print("PLAY REQ");
    }
    else if (timelineControlState & (1 << 3)){
      Serial.println("PLAYING");
      u8x8.print("PLAYING");
    }
    else if (timelineControlState & (1 << 2)){
      Serial.println("clocked");
      u8x8.print("clocked");
    }
    else if (timelineControlState & (1 << 1)){
      Serial.println("gotPulse");
      u8x8.print("gotPulse");
    }
    else if (timelineControlState & 1){
      Serial.println("alive");
      u8x8.print("alive");
    }     
    dataToDisplay = false;
  }
}