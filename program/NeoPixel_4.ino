
#include <Adafruit_NeoPixel.h>
#include "led_data.h"
#define LED 3
#define land_input 2


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, LED, NEO_GRB + NEO_KHZ800);
long startTime ;
int step = 0;
int maxStep;

void setup() {
  Serial.begin(9600);
  maxStep = sizeof(ledData) / 10;
  pixels.begin();
  pinMode(land_input , INPUT_PULLUP);

  dockSetup();
  while (!digitalRead(land_input)) {}//飛び立つまでループ
  startTime = millis();
}

void loop() {
  if (ledData[step][0] < millis() - startTime) {



    while (getTime() < getChengeStartTime(step) + getChengeTime(step)) {
      //    変更時間と現在時間から色を計算
      float nextRed = getRed(step - 1) + ((getTime() - getChengeStartTime(step))  * (getRed(step) - getRed(step - 1)) / getChengeTime(step));
      float nextGreen = getGreen(step - 1) + ((getTime() - getChengeStartTime(step))  * (getGreen(step) - getGreen(step - 1)) / getChengeTime(step));
      float nextBlue = getBlue(step - 1) + ((getTime() - getChengeStartTime(step))  * (getBlue(step) - getBlue(step - 1)) / getChengeTime(step));
      //      色をセット
      pixels.setPixelColor( 0, nextRed, nextGreen, nextBlue);
      pixels.setPixelColor( 1, nextRed, nextGreen, nextBlue);
      pixels.show();
      delay(1);
    }
    pixels.setPixelColor(0, getRed(step), getGreen(step), getBlue(step));
    pixels.show();
    step++;
  }
  //  発光パターンのデータが無くなったら無限ループ
  if (step == maxStep)endData();

}


void dockSetup() {

  //Telloの発射場にセットされた事を確認↓
  //  発射場にセットされて2秒間は黄色点滅して、その間にセット解除されたらやり直し
  while (true) {
    pixels.setPixelColor(0, 100, 0, 0);
    pixels.show();

    if (!digitalRead(land_input)) {
      pixels.setPixelColor(0, 100, 100, 0);
      pixels.show();
      if (digitalRead(land_input))continue;
      delay(500);

      pixels.setPixelColor(0, 0, 0, 0);
      pixels.show();
      if (digitalRead(land_input))continue;
      delay(500);

      pixels.setPixelColor(0, 100, 100, 0);
      pixels.show();
      if (digitalRead(land_input))continue;
      delay(500);

      pixels.setPixelColor(0, 0, 0, 0);
      pixels.show();
      if (digitalRead(land_input))continue;
      delay(500);

      if (!digitalRead(land_input))break;
    }

  }
  //  セットされて2秒経過すると緑に光って準備完了
  pixels.setPixelColor(0, 0, 100, 0);
  pixels.show();
  delay(200);
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.show();

  //Telloの発射場にセットされた事を確認↑
}


終了時に無限ループ
void endData() {
  Serial.println("END");
  while (true) {
    delay(100);
  }
}

//パターンデーターのget関数↓↓
int getChengeStartTime(int step) {
  if (step == -1)error("getChengeStartTime : step == -1");
  return pgm_read_word(ledData[step]) * 10;
}
int getChengeTime(int step) {
  if (step == -1)error("getChengeTime : step== -1");
  return pgm_read_word(ledData[step] + 1) * 10;
}
int getRed(int step) {
  if (step == -1)return 0;
  return pgm_read_word(ledData[step] + 2);
}
int getGreen(int step) {
  if (step == -1)return 0;
  return pgm_read_word(ledData[step] + 3);
}
int getBlue(int step) {
  if (step == -1)return 0;
  return pgm_read_word(ledData[step] + 4);
}
//パターンデーターのget関数↑↑

long getTime() {
  return millis() - startTime;
}


//問題が発生した場合は赤色と黄色で交互点滅して内容をコンソール出力
void error(String ErrorCode) {
  Serial.println("ERROR : " + ErrorCode);
  while (true) {
    pixels.setPixelColor( 0, 100, 0, 0);
    pixels.show();
    delay(2000);
    pixels.setPixelColor( 0, 0, 100, 0);
    pixels.show();
    delay(2000);
  }
}
