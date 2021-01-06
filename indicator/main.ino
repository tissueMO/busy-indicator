#include <M5StickC.h>
#include <WiFi.h>

// 定数定義
#include "settings.h"
const char *ssid = SSID;
const char *password = PASSWORD;
const char *host = HOSTNAME;
const int port = PORT;
const int displayBrightness = 9;

WiFiClient client;

// 機器固有設定
const char *DEVICE_ID = "101";
const String DEVICES[2] = {"101", "102"};
const int DEVICE_COUNT = 2;

/*
  初回処理
*/
void setup()
{
  // M5StickC 初期化
  M5.begin();

  // LCD スクリーン初期化
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(1);
  M5.Lcd.println("Busy Indicator");
  M5.Axp.ScreenBreath(displayBrightness);

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
  delay(1000);
  M5.update();

  if (M5.BtnA.wasReleased())
  {
    // ボタンが押されたら状態を切り替える
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
    Serial.print("Response: ");
    while (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
  }
  else
  {
    // 1秒おきに状態を取得
    M5.Lcd.fillScreen(BLACK);
    for (int i = 0; i < DEVICE_COUNT; i++)
    {
      Serial.print("Connecting to ");
      Serial.println(host);
      if (!client.connect(host, port))
      {
        // HTTP通信の確立に失敗
        Serial.println("Connection failed.");
        continue;
      }

      // リクエスト生成
      String url = "/get?id=";
      url += DEVICES[i];
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
      Serial.println(request);
      client.print(request);

      // レスポンス返却まで待機
      const unsigned long timeout = millis();
      while (!client.available())
      {
        if (millis() - timeout > 5000)
        {
          Serial.println("...Response Timeout.");
          client.stop();
          return;
        }
      }

      // レスポンス解析
      Serial.print("Response: ");
      int lineCounter = 0;
      bool isBusy = false;
      while (client.available())
      {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        lineCounter++;

        if (lineCounter == 7)
        {
          line.trim();
          if (line.charAt(0) == '1')
          {
            isBusy = true;
            Serial.println(":IsBusy");
          }
        }
      }
      Serial.println();

      if (isBusy)
      {
        // TODO: 現在の位置に電話アイコンを描画
      }
    }
  }
}
