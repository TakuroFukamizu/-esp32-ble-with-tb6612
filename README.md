# esp32-ble-with-tb6612

## Hardwares

- [カムプログラムロボット工作セット](http://www.tamiya.com/japan/products/70227/index.html)
- [AE-TB6612 2ch PWMモータードライバ](http://akizukidenshi.com/download/ds/akizuki/AE-TB6612.pdf)
- [スイッチサイエンス ESPr Developer 32](https://www.switch-science.com/catalog/3210/)
  - [Installation instructions for Mac OS](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/mac.md)
  - ボード名 : ESP32 Dev module

## I/F

### BLE GATT Profile

- DEVICE_NAME : YHD2017W-CP-ONI
- SERVICE_UUID : 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
  - CHARACTERISTIC1_UUID : 6E400002-B5A3-F393-E0A9-E50E24DCCA9E

#### CHARACTERISTIC 1

- PROPERTY : Read, Write without response

| start | stop | size | name | kind | description | 
|-------|-----|-----|-----|-----|-----|
| 0 | 2 | 3 | kind | char | コマンド種別 |
| 3 | 5 | 2 | time | unsigned int16 | 実行時間(ms) |
