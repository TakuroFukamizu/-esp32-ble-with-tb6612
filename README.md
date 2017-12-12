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
  - CHARACTERISTIC2_UUID : 6E400003-B5A3-F393-E0A9-E50E24DCCA9E

#### CHARACTERISTIC 1

iOSアプリからロボットに動作を指示するIF  

- PROPERTY : Read, Write without response
- VALUE : mixed

| start | size | name | kind | description | 
|-------|-----|-----|-----|-----|
| 0     |   1 | kind | unsigned int8  | コマンド種別 |
| 1     |   2 | time | unsigned int16 | 実行時間(ms) |

**※ time のバイトオーダーは Little Endian**

##### kind

| value | kind           | need time param? | description      | 
|-------|----------------|------------------|------------------|
| 0x01  | CMD_FORWARD    | YES              | 指定した時間前進 | 
| 0x02  | CMD_BACK       | YES              | 指定した時間後進 | 
| 0xFF  | CMD_STOP       | NO               | 停止 | 
| 0x11  | CMD_SPIN_TURN  | NO               | 超信地旋回で180度回転 |
| 0x12  | CMD_TURN_LEFT  | YES              | 指定した時間左回転 | 
| 0x13  | CMD_TURN_RIGHT | YES              | 指定した時間右回転 | 
| 0x21  | CMD_SRV_ON     | NO               | クラッピーを起こす | 
| 0x22  | CMD_SRV_OFF    | NO               | クラッピーを倒す　 | 


#### CHARACTERISTIC 2

ロボットからiOSアプリに信号を伝えるIF  

- PROPERTY : Read, Notify
- VALUE : chars

notifyされた時はロボット本体の「ダルマさんが転んだマニュアルボタン」が押された時。  
この際、iOSアプリは内部のステータス管理を更新した後、`CHARACTERISTIC 1` でロボットに `CMD_SPIN_TURN` を指示する