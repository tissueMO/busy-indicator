#include <M5StickC.h>
#include <WiFi.h>
#include "settings.h"

const char *ssid = SSID;
const char *password = PASSWORD;
const char *host = HOSTNAME;
const int port = PORT;
const int displayBrightness = 12;

// 機器固有設定
const int DEVICE_INDEX = 0;
const char *DEVICE_ID = "101";

const String DEVICES[2] = {"101", "102"};
const int DEVICE_COUNT = 2;
bool DEVICES_BUSY[2] = {false, false};

WiFiClient client;

// 重複してボタン押下判定しないようにするミリ秒数閾値
const int PRESS_MILLIS_THRESHOLD = 1000;
// 前回ボタン押下時の時刻
unsigned long lastPressTime;

// HTTPレスポンスが返ってくるまでのタイムアウトミリ秒数
const int RESPONSE_TIMEOUT_MILLIS = 5000;
// 最新の情報を取得する間隔ミリ秒数
const int CHECK_INTERVAL_MILLIS = 3000;
// 前回情報取得時の時刻
unsigned long lastGetTime;

// ダブルバッファ用スプライト
TFT_eSprite canvas = TFT_eSprite(&M5.Lcd);

// 関数プロトタイプ宣言
bool isButtonPressed();

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
}

/*
  ループ処理
*/
void loop()
{
  // 現在の状態を描画
  M5.Lcd.startWrite();
  canvas.fillSprite(BLACK);
  for (int i = 0; i < DEVICE_COUNT; i++)
  {
    if (DEVICES_BUSY[i])
    {
      // 現在の位置に電話アイコンを描画
      canvas.pushImage(
          imgWidth * i + (M5.Lcd.width() / DEVICE_COUNT - imgWidth) * i + (M5.Lcd.width() / DEVICE_COUNT - imgWidth) / 2,
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
  }
  canvas.pushSprite(0, 0);
  M5.Lcd.endWrite();

  // 指定秒数おきに状態を取得
  const int deltaTime = millis() - lastGetTime;
  bool buttonPressed = false;
  if (deltaTime >= CHECK_INTERVAL_MILLIS)
  {
    lastGetTime = millis();

    Serial.print("Connecting to ");
    Serial.println(host);
    if (!client.connect(host, port))
    {
      // HTTP通信の確立に失敗
      Serial.println("Connection failed.");
      return;
    }

    // リクエスト生成
    String url = "/get";
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
      else if (isButtonPressed())
      {
        buttonPressed = true;
        break;
      }
      else
      {
        // Serial.print(".");
      }
    }
    // Serial.println();

    // 途中でボタンが押下されていない場合に限り情報の取得を継続する
    if (!buttonPressed)
    {
      // レスポンス解析
      // Serial.print("Response: ");
      int lineCounter = 0;
      while (client.available())
      {
        String line = client.readStringUntil('\r');
        // Serial.print(line);
        lineCounter++;

        if (lineCounter == 7)
        {
          // 取得した最新の情報を判定
          line.trim();
          for (int i = 0; i < DEVICE_COUNT; i++)
          {
            Serial.print(i);

            if (line.charAt(i) == '1')
            {
              DEVICES_BUSY[i] = true;
              Serial.println(":IsBusy");
            }
            else
            {
              DEVICES_BUSY[i] = false;
              Serial.println(":NotBusy");
            }
          }
        }
      }
      // Serial.println();
    }
  }

  // ボタンが押されたら状態を切り替える
  // if (M5.BtnA.isPressed() && millis() - lastPressTime >= PRESS_MILLIS_THRESHOLD)
  if (buttonPressed || isButtonPressed())
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
    String url = "/turn?id=";
    url += DEVICE_ID;
    Serial.print("Requesting URL: ");
    Serial.print(host);
    Serial.println(url);

    // GET リクエスト実行
    String request =
        String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n" +
        "Content-Type: text/html; charset=utf-8\r\n" +
        "Content-Length: 0\r\n" +
        "\r\n";
    client.print(request);

    // レスポンス解析
    // Serial.print("Response: ");
    // while (client.available())
    // {
    //   String line = client.readStringUntil('\r');
    //   Serial.print(line);
    // }
    // Serial.println();

    // 取得前に状態更新
    DEVICES_BUSY[DEVICE_INDEX] = !DEVICES_BUSY[DEVICE_INDEX];

    return;
  }

  delay(1);
}

/**
 * 状態切替ボタンが押されているかどうかを判定します。
 * ウェイトを挟むループの中でもチェックする必要があります。
 */
bool isButtonPressed()
{
  M5.update();

  if (M5.BtnA.isPressed() && millis() - lastPressTime >= PRESS_MILLIS_THRESHOLD)
  {
    lastPressTime = millis();
    return true;
  }

  return false;
}
