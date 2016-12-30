#include <Nefry.h>
#include <Nefry_Milkcocoa.h>

Nefry_Milkcocoa *milkcocoa;

char *datastore;
void onpush(DataElement *elem);

void setup() {
  Nefry.setLed(20,0,0);   // 起動時は赤
  pinMode(D2, INPUT);
  milkcocoa->print();//Setup ModuleにAPPIDの表示をする
  milkcocoa = Nefry_Milkcocoa::begin();
  datastore = Nefry.getConfStr(3);
  Nefry.println( milkcocoa->on( datastore, "push", onpush) ? "Milkcocoa onPush OK" : "Milkcocoa onPush NG" );

  Nefry.setLed(0,20,0);   // 正常起動したら１秒間緑
  Nefry.ndelay(1000);
  Nefry.print("A0:");
  Nefry.println(A0);
  Nefry.print("D0:");
  Nefry.println(D0);
  Nefry.print("D1:");
  Nefry.println(D1);
  Nefry.print("D2:");
  Nefry.println(D2);
  Nefry.print("D3:");
  Nefry.println(D3);
  Nefry.ndelay(1000);
}

int presence0 = 0;
int presence1 = 0;
long analog0 = 0;
unsigned long sendtime = 0;

void loop() {
  Nefry.setLed(0,0,20);   // 最初は青
  if (!milkcocoa->loop(10000)) {
    if (!Nefry.autoConnect(10)) {
      Nefry.reset();  // WiFi接続できなかったらリセット
    }
    milkcocoa->loop(10000);
  }
  int a0 = analogRead(A0);
  Nefry.print("a0:");
  Nefry.println(a0);
  analog0 += a0;
  int presence = digitalRead(D2);
  Nefry.print("presence:");
  Nefry.println(presence);
  if (presence) {
    presence1++;
    Nefry.setLed(0,20,0);   // 在室時はLEDは緑色
  } else {
    presence0++;
    Nefry.setLed(10,20,0); // いない時はLEDは黄色
  }
  unsigned long now = millis();
  unsigned long diff = now - sendtime;
  if ( diff < 60 || diff > 60*60*1000 ) {  // 60秒*60
    DataElement elem = DataElement();
    elem.setValue("presence", presence1 * 100 / (presence0 + presence1)); // 在室時間をパーセンテージで
    elem.setValue("analog", a0);
    if (!milkcocoa->push(datastore, &elem)) {
      Nefry.reset();  // 送信できなかったらリセット
    }
    presence0 = 0;
    presence1 = 0;
    analog0 = 0;
    sendtime = now;
  }
  Nefry.ndelay(1000);
}

void onpush(DataElement *elem) {//MilkcocoaのOn関数と同一
  Nefry.print("onpush:");
  Nefry.println(elem->getInt("int"));
  Nefry.println(elem->getFloat("float"));
  Nefry.println(elem->getString("char"));
}

