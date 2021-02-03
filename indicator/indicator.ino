#include <M5StickC.h>
#include <WiFi.h>
#include "settings.h"

// 機器固有設定
const int DEVICE_INDEX = 0;
const int DEVICE_COUNT = 2;
const char *DEVICE_ID = "101";

// ネットワーク設定
const char *ssid = SSID;
const char *password = PASSWORD;
const char *host = HOSTNAME;
const int port = PORT;
WiFiClient client;
const int RESPONSE_TIMEOUT_MILLIS = 5000;

// 現在の状態管理フラグ
bool deviceBusy = false;

// Lcd描画設定
const int displayBrightness = 12;
TFT_eSprite canvas = TFT_eSprite(&M5.Lcd);

// 関数プロトタイプ宣言
void updateLatest();
void turnBusy();
void clearLcd();
void drawLcd(int deviceIndex, bool isBusy);

/*
  初回処理
*/
void setup()
{
  lastPressTime = millis() - PRESS_MILLIS_THRESHOLD;
  lastGetTime = millis() - CHECK_INTERVAL_MILLIS;

  // M5StickC 初期化
  M5.begin();

  // LCD スクリーン初期化
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Busy Indicator");
  M5.Axp.ScreenBreath(displayBrightness);

  // ダブルバッファ用スプライト作成
  canvas.createSprite(M5.Lcd.width(), M5.Lcd.height());
  canvas.setSwapBytes(true);

  // Wi-Fi 接続
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    // 接続待ち
    delay(500);
    Serial.print(".");
  }

  // 接続成功後
  Serial.println();
  Serial.println("Wi-Fi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.localIP());

  // サーバーから自機の最新の状態を取得
  updateLatest();
}

/*
  ループ処理
*/
void loop()
{
  M5.update();

  if (M5.BtnA.wasPressed())
  {
    clearLcd();
    turnBusy();
  }

  if (M5.BtnB.wasPressed())
  {
    clearLcd();
    updateLatest();
  }

  drawLcd(DEVICE_INDEX, deviceBusy);
  delay(1);
}

/**
 * サーバーから自機の最新の状態を取得します。
 */
void updateLatest()
{
  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client.connect(host, port))
  {
    // HTTP通信の確立に失敗
    Serial.println("Connection failed.");
    return;
  }

  // リクエスト生成
  String url = String("/get");
  Serial.print("Requesting URL: ");
  Serial.print(host);
  Serial.println(url);

  // GET リクエスト実行
  String request =
      String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" +
      "Content-Type: application/json\r\n" +
      "Content-Length: 0\r\n" +
      "\r\n";
  client.print(request);

  // レスポンス返却まで待機
  const unsigned long requestedTime = millis();
  while (!client.available())
  {
    if (millis() - requestedTime > RESPONSE_TIMEOUT_MILLIS)
    {
      Serial.println("...Response Timeout.");
      client.stop();
      return;
    }
  }

  // レスポンス解析
  int lineCounter = 0;
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    lineCounter++;

    if (lineCounter == 7)
    {
      // 取得した最新の情報を判定
      line.trim();
      for (int i = 0; i < DEVICE_COUNT; i++)
      {
        // 端末側で保持する現在の状態を更新
        if (i == DEVICE_INDEX)
        {
          deviceBusy = (line.charAt(i) == '1');
          Serial.println(deviceBusy ? ":Busy" : ":NotBusy");
        }
      }
    }
  }
}

/**
 * 自機の状態を切り替えます。
 */
void turnBusy()
{
  Serial.print("Connecting to ");
  Serial.println(host);
  if (!client.connect(host, port))
  {
    // HTTP通信の確立に失敗
    Serial.println("Connection failed.");
    return;
  }

  // リクエスト生成
  String url = String("/turn?id=") + DEVICE_ID;
  Serial.print("Requesting URL: ");
  Serial.print(host);
  Serial.println(url);

  // GET リクエスト送信
  String request =
      String("GET ") + url + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" +
      "Content-Type: text/html; charset=utf-8\r\n" +
      "Content-Length: 0\r\n" +
      "\r\n";
  client.print(request);

  // 端末側で保持する現在の状態を更新
  deviceBusy = !deviceBusy;
}

/**
 * LCDを暗転させます。
 */
void clearLcd()
{
  M5.Lcd.startWrite();
  canvas.fillSprite(BLACK);
  canvas.pushSprite(0, 0);
  M5.Lcd.endWrite();
}

/**
 * 画面に現在の状態を描画します。
 */
void drawLcd(int deviceIndex, bool isBusy)
{
  M5.Lcd.startWrite();
  canvas.fillSprite(BLACK);

  if (isBusy)
  {
    // 現在の位置に電話アイコンを描画
    canvas.pushImage(
        imgWidth * deviceIndex + (M5.Lcd.width() / DEVICE_COUNT - imgWidth) * deviceIndex + (M5.Lcd.width() / DEVICE_COUNT - imgWidth) / 2,
        (M5.Lcd.height() - imgHeight) / 2,
        imgWidth,
        imgHeight,
        img);
  }

  // 区切り線を描画
  canvas.drawLine(
      M5.Lcd.width() / 2,
      0,
      M5.Lcd.width() / 2,
      M5.Lcd.height(),
      WHITE);

  canvas.pushSprite(0, 0);
  M5.Lcd.endWrite();
}
