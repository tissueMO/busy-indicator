//########################################################
//  プライベートな Wi-Fi 接続設定などの定義 [サンプル]
//  settings.h としてコピーした上で適宜設定を行って下さい。
//########################################################

#define SSID xxxxxxx
#define PASSWORD xxxxxxx
#define HOSTNAME xxxxxxx
#define PORT 80

// RGB565 Dump(little endian)
#include <pgmspace.h>
const uint16_t imgWidth = 64;
const uint16_t imgHeight = 64;
const unsigned short img[4096] PROGMEM;
