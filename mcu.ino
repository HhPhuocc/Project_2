#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiManager.h> 
#include <FirebaseESP32.h>
#include <SoftwareSerial.h>
#define MODEM_RX 3
#define MODEM_TX 1
#define sim800l Serial2
#define DATABASE_URL "gdafas-917a5-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "Nx1h4sEVVcDnjzT85ZNmgA66vd2slTo95xxzFyVJ"
#define bom 19
#define den 18
#define coi 5
#define ACS_Coi 33
#define ACS_Bom 39
#define ACS_Den 36
int lcdColumns = 20;
int lcdRows = 4;
int Khi_khoi, Lua;
int Khi_khoi_tam;
float Doam_tam,Nhietdo_tam;
int hd_coi, hd_bom, hd_den;
float Doam,Nhietdo;
int tt_kc;
int dk_wifi = 1;
float adc_coi = 0, voltage_coi = 0;
float adc_bom = 0, voltage_bom = 0;
float adc_den = 0, voltage_den = 0;
const int menu = 12; int gt_menu =0; 
const int xuong = 4; int gt_xuong =0;
const int ok = 27; int gt_ok =0;
const int back = 14; int gt_back =0;
const int len = 32; int gt_len =0;
const int rst = 23; int gt_reset = 0;

int NewState = 1;
int LastState = 1;
int doAction = 0;
unsigned long StartToCount = 0;
unsigned long CurrentCount = 0;
int demtong, dem_menu = 0; 
int dem_tong_menu1, dem_tong_menu2, dem_tong_menu3, dem_menu_3 = 0;
int ghan_khikhoifb, ghan_nhietdofb;
int ghan_khikhoi =  ghan_khikhoifb = 50;
int ghan_nhietdo =  ghan_nhietdofb = 40;
int tt_bom, tt_den, tt_coi;
int ttbom, ttden, ttcoi;
int macdinh = HIGH; 
int ctt1_xuong, ctt1_len;
int ctt2_xuong, ctt2_len;
int ctt3_xuong, ctt3_len, ctt3_ok;

int dvc = 0;
int nhietdogh = 0;
int khikhoigh = 0;
int tt_menu;
int buttonState_menu;
String phoneNumber = "xxxxxxxx";
unsigned long lastDebounceTime = 0;
unsigned long buttonPressStartTime = 0;

// đặt thời gian của điều khiển thiết bị lên fb
unsigned long millisdv = 0;
unsigned long checkmillisdv = 0;

unsigned long millisghn = 0;
unsigned long checkmillisghn = 0;

unsigned long millisghg = 0;
unsigned long checkmillisghg = 0;

unsigned long millisgfb_ttmenu0 = 0;
unsigned long checkmillisgfb_ttmenu0 = 0;

unsigned long millisnfb_ttmenu0 = 0;
unsigned long checkmillisnfb_ttmenu0 = 0;

unsigned long millisgfb_ttmenu1 = 0;
unsigned long checkmillisgfb_ttmenu1 = 0;

unsigned long millisnfb_ttmenu1 = 0;
unsigned long checkmillisnfb_ttmenu1 = 0;

unsigned long millisfb_dkttmenu = 0;
unsigned long checkmillisfb_dkttmenu = 0;
int tt_wifi;

volatile bool buttonPressed = false;
volatile bool buttonPressed_rst = false;
void IRAM_ATTR buttonInterrupt() {
  
 
  
  
    buttonPressed = true;
    tt_menu = 1;
  
}
/*void IRAM_ATTR buttonInterrupt_rst() {
  int buttonState_rst;
  buttonState_rst = digitalRead(rst);
  if(buttonState_rst == LOW)
  {
    buttonPressStartTime = millis();
  }
  else{
  if(millis() - buttonPressStartTime >= 5000){
    buttonPressed_rst = true;}
  }
}*/
void callPhone(String number) {
  sim800l.println("ATD" + number + ";");
  delay(15000);  
  sim800l.println("ATH");  // Kết thúc cuộc gọi
  delay(1000);
}
FirebaseData fbdo;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 
uint8_t custChar[8][8] = {
  {31, 31, 31, 0, 0, 0, 0, 0},      // Small top line - 0
  {0, 0, 0, 0, 0, 31, 31, 31},      // Small bottom line - 1
  { B11111,
    B00000,
    B00000,
    B00000,                         // This shows an alternative
    B00000,                         // way of defining a custome character,
    B00000,                         // a bit more 'visually' perhaps?
    B00000,
    B11111,
  },
  //{31, 0, 0, 0, 0, 0, 0, 31},     // Small lines top and bottom -2
  {0, 0, 0, 0, 0, 0,  0, 31},       // Thin bottom line - 3
  {31, 31, 31, 31, 31, 31, 15, 7},  // Left bottom chamfer full - 4
  {28, 30, 31, 31, 31, 31, 31, 31}, // Right top chamfer full -5
  {31, 31, 31, 31, 31, 31, 30, 28}, // Right bottom chamfer full -6
  {7, 15, 31, 31, 31, 31, 31, 31},  // Left top chamfer full -7
};

// Define our numbers 0 thru 9
// 254 is blank and 255 is the "Full Block"
uint8_t bigNums[10][6] = {
  {7, 0, 5, 4, 1, 6},         //0
  {0, 5, 254, 1, 255, 1},     //1
  {0, 2, 5, 7, 3, 1},         //2
  {0, 2, 5, 1, 3, 6},         //3
  {7, 3, 255, 254, 254, 255}, //4
  {7, 2, 0, 1, 3, 6},         //5
  {7, 2, 0, 4, 3, 6},         //6
  {0, 0, 5, 254, 7, 254},   //7
  {7, 2, 5, 4, 3, 6},         //8
  {7, 2, 5, 1, 3, 6},         //9
};

void printBigNum(int number, int startCol, int startRow) {

  // Position cursor to requested position (each char takes 3 cols plus a space col)
  lcd.setCursor(startCol, startRow);

  // Each number split over two lines, 3 chars per line. Retrieve character
  // from the main array to make working with it here a bit easier.
  uint8_t thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
  }

  // First line (top half) of digit
  for (int cnt = 0; cnt < 3; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }

  // Now position cursor to next line at same start column for digit
  lcd.setCursor(startCol, startRow + 1);

  // 2nd line (bottom half)
  for (int cnt = 3; cnt < 6; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }
}
void menu_tong_1(){
  lcd.setCursor(0, 0);
  lcd.print("      Tuy chinh     "); 
  lcd.setCursor(0, 1);
  lcd.print("> Khi khoi          ");
  lcd.setCursor(0, 2);
  lcd.print("  Nhiet do          ");
  lcd.setCursor(0, 3);
  lcd.print("  Dieu khien        ");
}
void menu_tong_2(){
  lcd.setCursor(0, 0);
  lcd.print("      Tuy chinh     "); 
  lcd.setCursor(0, 1);
  lcd.print("  Khi khoi          ");
  lcd.setCursor(0, 2);
  lcd.print("> Nhiet do          ");
  lcd.setCursor(0, 3);
  lcd.print("  Dieu khien        ");
}
void menu_tong_3(){
  lcd.setCursor(0, 0);
  lcd.print("      Tuy chinh     "); 
  lcd.setCursor(0, 1);
  lcd.print("  Khi khoi          ");
  lcd.setCursor(0, 2);
  lcd.print("  Nhiet do          ");
  lcd.setCursor(0, 3);
  lcd.print("> Dieu khien        ");
}
void menu_1(){
  lcd.setCursor(0, 0);
  lcd.print("      Khi khoi      ");
  lcd.setCursor(0, 1);
  lcd.print("     Nguong bao     ");
  lcd.setCursor(0, 2);
  lcd.print("      ");
  lcd.setCursor(9, 2);
  lcd.print(" ");
  lcd.setCursor(13, 2);
  lcd.print("      ");
  lcd.setCursor(0, 3);
  lcd.print("      ");
  lcd.setCursor(9, 3);
  lcd.print(" ");
  lcd.setCursor(13, 3);
  lcd.print("      ");
   for (int cnt = 0; cnt < sizeof(custChar) / 8; cnt++) {
    lcd.createChar(cnt, custChar[cnt]);
  }
  int donvi_ghan_khikhoi = ghan_khikhoi%10; 
  int chuc_ghan_khikhoi = ghan_khikhoi/10;
  printBigNum(chuc_ghan_khikhoi, 6, 2);
  printBigNum(donvi_ghan_khikhoi, 10, 2);
}
void menu_2(){
  lcd.setCursor(0, 0);
  lcd.print("      Nhiet do      ");
  lcd.setCursor(0, 1);
  lcd.print("     Nguong bao     ");
  lcd.setCursor(0, 2);
  lcd.print("      ");
  lcd.setCursor(9, 2);
  lcd.print(" ");
  lcd.setCursor(13, 2);
  lcd.print("      ");
  lcd.setCursor(0, 3);
  lcd.print("      ");
  lcd.setCursor(9, 3);
  lcd.print(" ");
  lcd.setCursor(13, 3);
  lcd.print("      ");
   for (int cnt = 0; cnt < sizeof(custChar) / 8; cnt++) {
    lcd.createChar(cnt, custChar[cnt]);
  }
  int donvi_ghan_nhietdo = ghan_nhietdo%10; 
  int chuc_ghan_nhietdo = ghan_nhietdo/10;
  printBigNum(chuc_ghan_nhietdo, 6, 2);
  printBigNum(donvi_ghan_nhietdo, 10, 2);
}
void menu3_1(){
  if(tt_bom == 0)
  {
    lcd.setCursor(17, 1);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 1);
    lcd.print("ON ");
  }
  if(tt_den == 0)
  {
    lcd.setCursor(17, 2);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 2);
    lcd.print("ON ");
  }
  if(tt_coi == 0)
  {
    lcd.setCursor(17, 3);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 3);
    lcd.print("ON ");
  }
  lcd.setCursor(0, 0);
  lcd.print("      Dieu khien    ");
  lcd.setCursor(0, 1);
  lcd.print("> Bom    -->     ");
  lcd.setCursor(0, 2);
  lcd.print("  Den    -->     ");
  lcd.setCursor(0, 3);
  lcd.print("  Coi    -->     ");
  
}
void menu3_2(){
  if(tt_bom == 0)
  {
    lcd.setCursor(17, 1);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 1);
    lcd.print("ON ");
  }
  if(tt_den == 0)
  {
    lcd.setCursor(17, 2);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 2);
    lcd.print("ON ");
  }
  if(tt_coi == 0)
  {
    lcd.setCursor(17, 3);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 3);
    lcd.print("ON ");
  }
  lcd.setCursor(0, 0);
  lcd.print("      Dieu khien    ");
  lcd.setCursor(0, 1);
  lcd.print("  Bom    -->     ");
  lcd.setCursor(0, 2);
  lcd.print("> Den    -->     ");
  lcd.setCursor(0, 3);
  lcd.print("  Coi    -->     ");
}
void menu3_3(){
  if(tt_bom == 0)
  {
    lcd.setCursor(17, 1);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 1);
    lcd.print("ON ");
  }
  if(tt_den == 0)
  {
    lcd.setCursor(17, 2);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 2);
    lcd.print("ON ");
  }
  if(tt_coi == 0)
  {
    lcd.setCursor(17, 3);
    lcd.print("OFF");
  }
  else
  {
    lcd.setCursor(17, 3);
    lcd.print("ON ");
  }
  lcd.setCursor(0, 0);
  lcd.print("      Dieu khien    ");
  lcd.setCursor(0, 1);
  lcd.print("  Bom    -->     ");
  lcd.setCursor(0, 2);
  lcd.print("  Den    -->     ");
  lcd.setCursor(0, 3);
  lcd.print("> Coi    -->     ");
}
void hienthi_thongtin(){
  if((Lua==1 && Nhietdo > ghan_nhietdo) || (Khi_khoi > ghan_khikhoi))
  {
     lcd.setCursor(0, 0);
     lcd.print("Trang thai: Canh bao");
  }
   else
   {
     lcd.setCursor(0, 0);
     lcd.print("Trang thai: Bthuong ");
   }
   
  lcd.setCursor(0, 1);
  lcd.print("Do am: ");
  lcd.setCursor(7, 1);
  lcd.print(Doam);
  lcd.setCursor(12, 1);
  lcd.print("        ");
  
  lcd.setCursor(0, 2);
  lcd.print("Khi khoi: ");
  lcd.setCursor(10, 2);
  lcd.print(Khi_khoi);
  lcd.setCursor(12, 2);
  lcd.print("        ");

  lcd.setCursor(0, 3);
  lcd.print("Nhiet do: ");
  lcd.setCursor(10,3);
  lcd.print(Nhietdo);
  lcd.setCursor(15, 3);
  lcd.print("     ");
}
void setup(){
  lcd.init();                
  lcd.backlight();
  sim800l.begin(9600);
  Serial.begin(115200);
  pinMode(menu, INPUT_PULLUP);
  pinMode(xuong, INPUT_PULLUP);
  pinMode(ok, INPUT_PULLUP);
  pinMode(back, INPUT_PULLUP);
  pinMode(len, INPUT_PULLUP);
  pinMode(rst, INPUT_PULLUP);

  pinMode(bom, OUTPUT);
  pinMode(den, OUTPUT);
  pinMode(coi, OUTPUT);

  digitalWrite(bom, LOW);
  digitalWrite(den, LOW);
  digitalWrite(coi, LOW);

  pinMode(ACS_Coi,INPUT); 
  pinMode(ACS_Bom,INPUT); 
  pinMode(ACS_Den,INPUT); 
  attachInterrupt(digitalPinToInterrupt(menu), buttonInterrupt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(rst), buttonInterrupt_rst, FALLING);
  WiFiManager wm;
   bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("GDAFAS","68686868");
    if(!res) {
        Serial.println("Failed to connect");
        lcd.setCursor(0, 0);
        lcd.print("Ket noi that bai ");
        lcd.setCursor(0, 1);
        lcd.print("Vui long thiet   ");
        lcd.setCursor(0, 2);
        lcd.print("lap lai diem truy");
        lcd.setCursor(0, 3);
        lcd.print("cap              ");
        Serial.println(res);
        //ESP.restart();
    } 
    else {
        Serial.println("connected...yeey :)");
        lcd.setCursor(0, 0);
        lcd.print("Doi ti...........   ");
        lcd.setCursor(0, 1);
        lcd.print("He thong dang       ");
        lcd.setCursor(0, 2);
        lcd.print("ket noi den wifi    ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
    }
     Firebase.begin(DATABASE_URL, DATABASE_SECRET);
}

void loop(){
  millisnfb_ttmenu0 = millis();
  millisgfb_ttmenu0 = millis();
  millisgfb_ttmenu1 = millis();
  
  if (buttonPressed)
  {
    gt_menu = !gt_menu;
    buttonPressed = false;
  }
  /*if (buttonPressed_rst)
  {
    WiFiManager wm;
    wm.resetSettings();
    buttonPressed_rst = false;
    delay(3000);
    ESP.restart();
  }*/
   unsigned long currentMillis = millis();
   static unsigned long lastCheckMillis = 0;
   
   if (currentMillis - lastCheckMillis >= 60000) {
        lastCheckMillis = currentMillis;
        if(WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnecting to the Internet");
            WiFi.reconnect();
            lcd.setCursor(0, 0);
            lcd.print("Doi ti...........   ");
            lcd.setCursor(0, 1);
            lcd.print("He thong dang co    ");
            lcd.setCursor(0, 2);
            lcd.print("gang ket noi lai    ");
            lcd.setCursor(0, 3);
            lcd.print("wifi                ");
            dk_wifi = 0;
            tt_wifi = 1;
        }
         if ((WiFi.status() == WL_CONNECTED) && (tt_wifi == 1)) {
             Serial.println("Reconnected to the Internet");
             lcd.setCursor(0, 0);
              lcd.print("Ket noi den wifi    ");
              lcd.setCursor(0, 1);
              lcd.print("thanh cong          ");
              lcd.setCursor(0, 2);
              lcd.print("Dang truy cap lai   ");
              lcd.setCursor(0, 3);
              lcd.print("he thong            ");
              dk_wifi = 1;
              tt_wifi = 0;
        }
            } 
  if(dk_wifi == 1)
  {
  if(gt_menu ==0)
  {
   millisnfb_ttmenu1 = millis();
   if(millisnfb_ttmenu1 - checkmillisnfb_ttmenu1 >=5000)
   {
     Firebase.RTDB.getInt(&fbdo,"/Phong 1/Khi&khoi");
     Khi_khoi_tam = fbdo.intData();
     if(Khi_khoi_tam != 0)
     {
      Khi_khoi = Khi_khoi_tam;
     }
     
     Firebase.RTDB.getFloat(&fbdo,"/Phong 1/Do am");
     Doam_tam=fbdo.floatData();
     if(Doam_tam != 0)
     {
      Doam = Doam_tam;
     }
     Firebase.RTDB.getFloat(&fbdo,"/Phong 1/Nhiet do");
     Nhietdo_tam=fbdo.floatData();
     if(Nhietdo_tam != 0)
     {
      Nhietdo = Nhietdo_tam;
     }

     Firebase.RTDB.getBool(&fbdo,"/Phong 1/Lua");
     Lua=fbdo.boolData();
     checkmillisnfb_ttmenu1 = millis();
   }
   
  
   
   
   hienthi_thongtin();
   dem_tong_menu1 = dem_tong_menu2 = dem_tong_menu3 = 0; 
   dem_menu_3 = ctt1_xuong = ctt1_len = ctt2_xuong = ctt2_len = ctt3_xuong = ctt3_len =ctt3_ok = 0; 
   dem_menu = 0;
   if(Nhietdo > ghan_nhietdo && Lua == 1)
   {
    ttbom = HIGH;
    ttden = HIGH;
    ttcoi = HIGH;
    callPhone(phoneNumber);
    tt_kc = 1;
   }
   else
   {
    ttbom = LOW;
    ttden = LOW;
    ttcoi = LOW;
    tt_kc = 0;
   }
  if(Khi_khoi > ghan_khikhoi)
  {
    ttcoi = HIGH;
    callPhone(phoneNumber);
    tt_kc = 1;
  }
  else if((Khi_khoi > ghan_khikhoi)&&((Nhietdo < ghan_nhietdo) && (Lua == 0)))
  {
    ttcoi = LOW;
    tt_kc = 0;
  }
    digitalWrite(bom, ttbom);
    digitalWrite(den, ttden);
    digitalWrite(coi, ttcoi);
  }
  if(gt_menu == 1)
  { 
    millisdv = millis();
    millisghg = millis();
    millisgfb_ttmenu0 = millis();
    
    millisgfb_ttmenu1 = millis();
    millisnfb_ttmenu1 = millis();
    millisfb_dkttmenu = millis();
    gt_xuong = digitalRead(xuong);
    gt_ok = digitalRead(ok);
    gt_back = digitalRead(back);
    gt_len = digitalRead(len);
    if(gt_xuong != macdinh)
    {
      if(gt_xuong == LOW && ctt1_xuong == 0 && ctt2_xuong == 0 && ctt3_xuong == 0)
      {
        dem_menu++;
        if(dem_menu>2)
        {
          dem_menu=2;
        }
        while(digitalRead(xuong) == LOW);
       }
      if(gt_xuong == LOW && ctt1_xuong == 1)
      {
        ghan_khikhoi--;
        nhietdogh = 1;
        if(ghan_khikhoi<10)
        {
          ghan_khikhoi=10;
        }
       }
      if(gt_xuong == LOW && ctt2_xuong == 1)
      {
        ghan_nhietdo--;
        nhietdogh = 1;
        if(ghan_nhietdo<10)
        {
          ghan_nhietdo=10;
        }
       }
       if(gt_xuong == LOW && ctt3_xuong == 1)
      {
        dem_menu_3++;
        if(dem_menu_3>2)
        {
          dem_menu_3=2;
        }
        while(digitalRead(xuong) == LOW);
       }
     }
     if(gt_len != macdinh)
    {
      if(gt_len == LOW && ctt1_len == 0 && ctt2_len == 0 && ctt3_len == 0)
      {
        dem_menu--;
        if(dem_menu<0)
        {
          dem_menu=0;
        }
        while(digitalRead(len) == LOW);
       }
      if(gt_len == LOW && ctt1_len == 1)
      {
        ghan_khikhoi++;
        nhietdogh =1;
        if(ghan_khikhoi>90)
        {
          ghan_khikhoi=90;
        }
       }
      if(gt_len == LOW && ctt2_len == 1)
      {
        ghan_nhietdo++;
        nhietdogh = 1;
        if(ghan_nhietdo>90)
        {
          ghan_khikhoi=90;
        }
       }
       if(gt_len == LOW && ctt3_len == 1)
      {
        dem_menu_3--;
        if(dem_menu_3<0)
        {
          dem_menu_3=0;
        }
        while(digitalRead(len) == LOW);
       }
     }
     if(dem_menu == 0)
     {
      if(gt_ok != macdinh)
      {
        if(gt_ok == LOW)
        {
          dem_tong_menu1++;
          if(dem_tong_menu1>1)
          {
            dem_tong_menu1 =1;
          }
        }
      }
      if(gt_back != macdinh)
      {
        if(gt_back == LOW)
        {
          dem_tong_menu1--;
          if(dem_tong_menu1<0)
          {
            dem_tong_menu1 =0;
          }
        }
      }
      if(dem_menu == 0 && dem_tong_menu1 == 0)
      {
       menu_tong_1();
       ctt1_xuong = 0;
       ctt1_len = 0;
      }
      if(dem_menu == 0 && dem_tong_menu1 == 1)
      {
       menu_1();
       ctt1_xuong = 1;
       ctt1_len = 1;
      }
     }
     if(dem_menu == 1)
     {
       if(gt_ok != macdinh)
      {
        if(gt_ok == LOW)
        {
          dem_tong_menu2++;
          if(dem_tong_menu2>1)
          {
            dem_tong_menu2 =1;
          }
        }
      }
      if(gt_back != macdinh)
      {
        if(gt_back == LOW)
        {
          dem_tong_menu2--;
          if(dem_tong_menu2<0)
          {
            dem_tong_menu2 =0;
          }
        }
      }
      if(dem_menu == 1 && dem_tong_menu2 == 0)
      {
       menu_tong_2();
       ctt2_len = 0;
       ctt2_xuong = 0;
      }
      if(dem_menu == 1 && dem_tong_menu2 == 1)
      {
       menu_2();
       ctt2_len = 1;
       ctt2_xuong = 1;
      }
     }
     if(dem_menu == 2)
     {
       if(gt_ok != macdinh)
      {
        if(gt_ok == LOW)
        {
          dem_tong_menu3++;
          if(dem_tong_menu3>1)
          {
            dem_tong_menu3 =1;
          }
        }
        if(gt_ok == LOW && ctt3_ok == 1)
        {
          tt_bom = !tt_bom;
          dvc = 1;
        }
        if(gt_ok == LOW && ctt3_ok == 2)
        {
          tt_den = !tt_den;
          dvc = 1;
        }
        if(gt_ok == LOW && ctt3_ok == 3)
        {
          tt_coi = !tt_coi;
          dvc = 1;
        }
      }
      if(gt_back != macdinh)
      {
        if(gt_back == LOW)
        {
          dem_tong_menu3--;
          if(dem_tong_menu3<0)
          {
            dem_tong_menu3 =0;
          }
        }
      }
      if(dem_menu == 2 && dem_tong_menu3 == 0)
      {
        menu_tong_3();
        ctt3_len = 0;
        ctt3_xuong = 0;
        ctt3_ok = 0;
        dem_menu_3 = 0;
      }
      if(dem_menu == 2 && dem_tong_menu3 == 1)
      {
       if(dem_menu_3 == 0)
       {
        menu3_1();
        ctt3_ok = 1;
       }
       if(dem_menu_3 == 1)
       {
        menu3_2();
        ctt3_ok = 2;
       }
       if(dem_menu_3 == 2)
       {
        menu3_3();
        ctt3_ok = 3;
       }
       ctt3_len = 1;
       ctt3_xuong = 1;
      }
     }
     if(tt_bom == 0)
   {
    digitalWrite(bom, LOW);
   }
   else
   {
    digitalWrite(bom, HIGH);
   }
   if(tt_den == 0)
   {
    digitalWrite(den, LOW);
   }
   else
   {
    digitalWrite(den, HIGH);
   }
   if(tt_coi == 0)
   {
    digitalWrite(coi, LOW);
   }
   else
   {
    digitalWrite(coi, HIGH);
   }
   if (millisdv - checkmillisdv >=14000)
   {
     if(dvc == 1)
     {
       Firebase.setInt(fbdo, "/Du lieu/tt_bom", tt_bom);
       Firebase.setInt(fbdo, "/Du lieu/tt_den", tt_den);
       Firebase.setInt(fbdo, "/Du lieu/tt_coi", tt_coi);
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_bom");
       tt_bom = fbdo.intData();
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_den");
       tt_den = fbdo.intData();
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_coi");
       tt_coi = fbdo.intData();
      }
     else 
     { 
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_bom");
       tt_bom = fbdo.intData();
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_den");
       tt_den = fbdo.intData();
       Firebase.RTDB.getInt(&fbdo,"/Du lieu/tt_coi");
       tt_coi = fbdo.intData();
       Firebase.setInt(fbdo, "/Du lieu/tt_bom", tt_bom);
       Firebase.setInt(fbdo, "/Du lieu/tt_den", tt_den);
       Firebase.setInt(fbdo, "/Du lieu/tt_coi", tt_coi);
      }
      dvc =0;
      checkmillisdv = millis();
   }
   
   if(nhietdogh == 0)
   {
     if (millisghg - checkmillisghg >= 10000) {
    
     Firebase.RTDB.getInt(&fbdo,"/Du lieu/ghan_nhietdo");
     ghan_nhietdo = fbdo.intData();
     Firebase.RTDB.getInt(&fbdo,"/Du lieu/ghan_khikhoi");
     ghan_khikhoi = fbdo.intData();
     }
     else if(millisghg - checkmillisghg >= 11000){
     
     nhietdogh = 0;
     Firebase.setInt(fbdo, "/Du lieu/ghan_khikhoi", ghan_khikhoi);
     Firebase.setInt(fbdo, "/Du lieu/ghan_nhietdo", ghan_nhietdo);
     checkmillisghg = millisghg;
     }
     
   }
   else if(nhietdogh == 1)
   {
     if(millisghg - checkmillisghg >= 11000){
     
     nhietdogh = 0;
     Firebase.setInt(fbdo, "/Du lieu/ghan_khikhoi", ghan_khikhoi);
     Firebase.setInt(fbdo, "/Du lieu/ghan_nhietdo", ghan_nhietdo);
     }
     if (millisghg - checkmillisghg >= 12000) {
    
     Firebase.RTDB.getInt(&fbdo,"/Du lieu/ghan_nhietdo");
     ghan_nhietdo = fbdo.intData();
     Firebase.RTDB.getInt(&fbdo,"/Du lieu/ghan_khikhoi");
     ghan_khikhoi = fbdo.intData();
     }
     if(((gt_xuong == 1)||(gt_len == 1))&&(millisghg - checkmillisghg >= 13000))
     {
      checkmillisghg = millisghg;
      nhietdogh = 0;
     }
   }
  }
  if((millisnfb_ttmenu0 - checkmillisnfb_ttmenu0 >= 8000)&&(tt_kc == 0))
  {
    if(tt_menu == 0)
    {
        Firebase.RTDB.getInt(&fbdo,"/Du lieu/gt_menu");
        gt_menu = fbdo.intData();
        Firebase.setInt(fbdo, "/Du lieu/gt_menu", gt_menu);
    }
    if(tt_menu == 1)
    {
        Firebase.setInt(fbdo, "/Du lieu/gt_menu", gt_menu); 
        Firebase.RTDB.getInt(&fbdo,"/Du lieu/gt_menu");
        gt_menu = fbdo.intData();
        tt_menu = 0;
        
    }
    checkmillisnfb_ttmenu0 = millis();
  }
    if(millisgfb_ttmenu0 - checkmillisgfb_ttmenu0 >= 26000)
    {
      //adc_coi = analogRead(ACS_Coi);
      voltage_coi = analogRead(ACS_Coi)*(3.3/4095.0);
     
      //adc_bom = analogRead(ACS_Bom);
      voltage_bom = analogRead(ACS_Bom)*(3.3/4095.0);

      //adc_den = analogRead(ACS_Den);
      voltage_den = analogRead(ACS_Den)*(3.3/4095.0);
     
      if(voltage_coi>2.6)
      {
        hd_coi = 1;
      }
      else
      {
        hd_coi = 0;
      }
      if(voltage_bom>2.6)
      {
        hd_bom = 1;
      }
      else
      {
        hd_bom = 0;
      }
      if(voltage_den>2.6)
      {
        hd_den = 1;
      }
      else
      {
        hd_den = 0;
      }
      Firebase.setInt(fbdo, "/Du lieu/hd_coi", hd_coi);
      Firebase.setInt(fbdo, "/Du lieu/hd_bom", hd_bom);
      Firebase.setInt(fbdo, "/Du lieu/hd_den", hd_den);
      
      checkmillisgfb_ttmenu0 = millis();
    
    }
    if((millisgfb_ttmenu1 - checkmillisgfb_ttmenu1 >= 28000)&&(tt_kc == 0))
    {
      Firebase.RTDB.getString(&fbdo,"/Du lieu/SĐT");
      phoneNumber = fbdo.stringData();
    }
    LastState = NewState;
    NewState = digitalRead(rst);
  if (NewState == LOW) {
    //Serial.print( NewState );
    if (LastState == HIGH && NewState == LOW) {
      StartToCount = millis();
      doAction = 1;
    }

    if (LastState == LOW && NewState == LOW) {
      CurrentCount = millis();
      if (CurrentCount - StartToCount > 3000) {
        if (doAction) {
          WiFiManager wm;
          wm.resetSettings();
          //buttonPressed = false;
          //delay(2000);
          ESP.restart();
          doAction = 0;
        }
      }
    }
  }
    Serial.println(voltage_coi);
    Serial.println(voltage_bom);
    Serial.println(voltage_den);
    Serial.println(phoneNumber);
}
}
