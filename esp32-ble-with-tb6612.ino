
// ESP32 Dev Module (ESPr Developer 32)

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <Wire.h>

//------------------------------------------
// TB6612 Moter Driver
//------------------------------------------

//for Arduino Uno
//#define TB_A1    8  // moterA IN1
//#define TB_A2    9  // moterA IN2
//#define TB_Ap   10  // moterA PWM
//#define TB_B1    7  // moterB IN1
//#define TB_B2    6  // moterB IN2
//#define TB_Bp    5  // moterB PWM
//#define TB_STBY 11  // STBY
//for ESP32
#define TB_A1   16  // moterA IN1
#define TB_A2   22  // moterA IN2
#define TB_Ap    0  // moterA PWM
#define TB_B1   17  // moterB IN1
#define TB_B2    5  // moterB IN2
#define TB_Bp   18  // moterB PWM
#define TB_STBY  2  // STBY

void setup_tb6612() {
  pinMode( TB_A1, OUTPUT );
  pinMode( TB_A2, OUTPUT );
  pinMode( TB_Ap, OUTPUT );
  pinMode( TB_B1, OUTPUT );
  pinMode( TB_B2, OUTPUT );
  pinMode( TB_Bp, OUTPUT );
  pinMode( TB_STBY, OUTPUT );
   
  digitalWrite( TB_A1, LOW );
  digitalWrite( TB_A2, LOW );
  digitalWrite( TB_Ap, LOW );
  digitalWrite( TB_B1, LOW );
  digitalWrite( TB_B2, LOW );
  digitalWrite( TB_Bp, LOW );
  digitalWrite( TB_STBY, HIGH );
}

void cmd_motorA_forward() {
  Serial.println("motorA forward");
  digitalWrite( TB_Ap, HIGH );
  digitalWrite( TB_A1, HIGH );
  digitalWrite( TB_A2, LOW );
}
void cmd_motorA_back() {
  Serial.println("motorA back");
  digitalWrite( TB_Ap, HIGH );
  digitalWrite( TB_A1, LOW );
  digitalWrite( TB_A2, HIGH );
}
void cmd_motorA_stop() {
  Serial.println("motorA stop");
  digitalWrite( TB_A2, LOW );
}

void cmd_motorB_forward() {
  Serial.println("motorB forward");
  digitalWrite( TB_Bp, HIGH );
  digitalWrite( TB_B1, HIGH );
  digitalWrite( TB_B2, LOW );
}
void cmd_motorB_back() {
  Serial.println("motorB back");
  digitalWrite( TB_Bp, HIGH );
  digitalWrite( TB_B1, LOW );
  digitalWrite( TB_B2, HIGH );
}
void cmd_motorB_stop() {
  Serial.println("motorB stop");
  digitalWrite( TB_Bp, LOW );
}

void cmd_forward() {
  Serial.println("all forward");
  
  digitalWrite( TB_Ap, HIGH );
  digitalWrite( TB_A1, HIGH );
  digitalWrite( TB_A2, LOW );
  
  digitalWrite( TB_Bp, HIGH );
  digitalWrite( TB_B1, HIGH );
  digitalWrite( TB_B2, LOW );
}

void cmd_back() {
  Serial.println("all forward");
  
  digitalWrite( TB_Ap, HIGH );
  digitalWrite( TB_A1, LOW );
  digitalWrite( TB_A2, HIGH );
  
  digitalWrite( TB_Bp, HIGH );
  digitalWrite( TB_B1, LOW );
  digitalWrite( TB_B2, HIGH );
}

void cmd_stop() {
  Serial.println("all stop");
  
  digitalWrite( TB_Ap, LOW );

  digitalWrite( TB_Bp, LOW );
}


//------------------------------------------
// BLE
//------------------------------------------

#define TXRX_BUF_LEN 5
#define DEVICE_NAME  "MyESP32"
#define SERVICE_UUID         "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC2_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#define CMD_FORWARD "f" //0x66
#define CMD_BACK    "b" //0x62
#define CMD_STOP    "s" //0x73

//Characteristic
BLECharacteristic *pCharacteristic;
BLECharacteristic *pCharacteristic2;

//Data
uint8_t tx_buf[TXRX_BUF_LEN];

//Timer
volatile SemaphoreHandle_t timerSemaphore;

void IRAM_ATTR onTimer(){
    //print
    Serial.print("Send:");
    for(int i=0;i<TXRX_BUF_LEN;i++){
        tx_buf[i] += 1;
        Serial.print(tx_buf[i]);Serial.print(",");
    }
    Serial.println();

    //update notify
    pCharacteristic->setValue(tx_buf, TXRX_BUF_LEN); //send hex
    pCharacteristic->notify();

    xSemaphoreGiveFromISR(timerSemaphore, NULL);
}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pChara) {
      std::string value = pChara->getValue();
//      int value = pChara->getValue(); //cannot convert 'std::__cxx11::string {aka std::__cxx11::basic_string<char>}' to 'int' in initialization

      if (value.length() > 0) {
        if (value == CMD_FORWARD) cmd_forward();
        else if (value == CMD_BACK) cmd_back();
        else if (value == CMD_STOP) cmd_stop();
        else {
          Serial.println("*********");
          Serial.print("New value: ");
          for (int i=0; i<value.length(); i++) Serial.print(value[i]);
          Serial.println();
          Serial.println("*********");
        }
      }
    }
};

void setup_ble() {
    // BLEデバイス名の定義
    BLEDevice::init(DEVICE_NAME);
    // BLEサーバーの定義
    BLEServer *pServer = BLEDevice::createServer();
    // BLEサービスの定義
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Characteristicの定義
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY
                        );
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic2 = pService->createCharacteristic(
                        CHARACTERISTIC2_UUID,
                        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
                        );
    pCharacteristic2->addDescriptor(new BLE2902());
    pCharacteristic2->setCallbacks(new MyCallbacks());
    pCharacteristic2->setValue("off");

    // Start
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}


//------------------------------------------
// Main logics
//------------------------------------------

void setup() {
    Serial.begin(115200);

    setup_tb6612();
    cmd_stop();
    
    setup_ble();

//    //Timer
//    timerSemaphore = xSemaphoreCreateBinary();
//    hw_timer_t * timer = timerBegin(0, 80, true);
//    timerAttachInterrupt(timer, &onTimer, true);
//    timerAlarmWrite(timer, 1000000, true); //1sec notify
//    timerAlarmEnable(timer);
//
//    //Initialize array
//    for(int i=0;i<TXRX_BUF_LEN;i++){
//        tx_buf[i] = 0;
//    }
}

void loop() {
}
