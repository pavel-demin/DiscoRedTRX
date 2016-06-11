//--------------------------------------------------------------
// File     : main.c
// Datum    : 29.05.2016
// Version  : 0.3
// Autor    : Wolfgang Kiefer
// EMail    : woki@onlinehome.de
// Web      : www.wkiefer.de
// CPU      : STM32F746
// Board    : STM32F746-Discovery-Board
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Lib      : stm32f746g-disco_hal_lib (static)
// Funktion : Hauptprogramm für RedPitaya Interface (Embedded Transceiver from Pavel Demin) DiscoRedTRX
// USART    : COM6
// RX- Pin  : PC7 ==> CN4 D0
// TX- Pin  : PC6 ==> CN4 D1
//--------------------------------------------------------------

#include "stm32_ub_system.h"
#include "stm32_ub_sgui.h"
#include "stm32_ub_uart.h"
#include "stm32_ub_qflash.h"
#include "stm32_ub_tim2.h"
#include "ub_sgui_floatedit.h"

void SendFreq();
void FrequShift(int32_t freqShift);
void SetBoxes();
void btn_Select(bool aktiv);

#define FFTHeight 90



void create_MainWindow_01(void);
void create_ChildWindow_11(void); // Mode selection
void create_ChildWindow_12SSB(void); // Bandwidth selection
void create_ChildWindow_12CW(void); // Bandwidth selection
void create_ChildWindow_12AM(void); // Bandwidth selection
void create_ChildWindow_13(void); // AGC selection
void create_ChildWindow_14(void); // Frequency input/actualisation
void create_ChildWindow_15(void); // Settings
void create_ChildWindow_16(void); // Calibrate
void create_ChildWindow_18(void); // CW- Keyer
void create_ChildWindow_19(void); // Band/Memory

// Globale Pointer
SWINDOW_t *MAIN;
SLISTBOX_t *lb,*lb1,*lb2,*lb12,*lb22,*lb3,*lb13,*lb191,*lb192,*lb193,*lb194,*lb195,*lb151,*lb152;  // pointer auf Listbox
SBUTTON_t *btn, *btn9,*btn10,*btn11,*btn12,*btn13,*btn130,*btn151,*Storebtn,*cwbtn, *btnX,*btn191,*btn192,*btn193,*btn194,*btn195,*btn196,*btnCal;  // pointer auf buttons
SGRAPH_t *graph;
SDROPDOWN_t *dd1;
SRBTN_t *rb1,*rb2,*rb3,*rb161,*rb162,*rb163;
SSLIDER_t *bright, *mic;
SWINDOW_t *ptr11,*ptr12,*ptr121,*ptr12CW,*ptr13,*ptr14,*ptr15,*ptr16,*ptr17,*ptr18,*ptr19;

SBUTTON_t* actBtn;

SFLOATEDIT_t* EditStrfValue; // for calibrate

int32_t actShift;
// Globale Steuervariablen
uint8_t ModeNr=2; // 0 = CWL, 1=CWU, 2=LSB, 3=USB, 4=AM, 5=FM, 6=Digi
uint8_t BWNr;
uint8_t BWNrSSB=4;
uint8_t BWNrCW=5;
uint8_t BWNrAM=5;
uint8_t split=0; // 0 RXfrequ =TXfrequ.
uint8_t AGCmode;
uint8_t SelectSweep=0;

char writ[16]="12345          ";
char writ1[16]="12345          ";
char writ2[16]="12345          ";
char writ3[16]="12345          ";
char writ4[16]="12345          ";
char RecDataChar[64];

char band[16][5]={"2200"," 630"," 160","  80","  60","  40","  30","  20","  17","  15","  12","  10","   6","Res1","Res2","Res3"};
char memo[16][5]={"M  1","M  2","M  3","M  4","M  5","M  6","M  7","M  8","M  9","M 10","M 11","M 12","M 13","M 14","M 15","M 16"};

// This area is stored in FlashROM ***********************************************************************************************
int16_t DummyData[2]={12345,2986}; // to go back to default values: put here other values and compile
int16_t SValueCalibrat=0;
int16_t stepline=0,pointsline=0;
uint8_t StopCount=9; // Control for Backlight Dim (1..18)
uint8_t VolumSet=5; // Control for volume (0 .. 9)
uint8_t MicVolumSet=5; // Control for volume (0 .. 9)
uint16_t step=200; // Parameter for sweep function
uint16_t points=120;
int32_t bandfreq[16]={136000,472000,1967000,3660000,5500000,7200000,10100000,14100000,18068000,21000000,24890000,28000000,50080000,77900,100000,7250000};
uint8_t bandmode[16]={0,0,2,2,2,2,3,0,0,0,0,0,0,2,3,4};
uint8_t bandwidth[16]={3,3,3,3,3,3,3,3,3,3,3,3,3,2,3,3};
int32_t memofreq[16]={7000000,7050000,7100000,7150000,5500000,7200000,10100000,14100000,18068000,21000000,24890000,28000000,50080000,77900,100000,7250000};
uint8_t memomode[16]={0,0,2,2,2,2,3,0,0,0,0,0,0,2,3,4};
uint8_t memowidth[16]={3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
int16_t reserve[31]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// *****************************************************************************************************************************
uint8_t BandPtr=0;
uint8_t MemoPtr=0;

char ModeTable[7][5]={" CWL"," CWU"," LSB"," USB"," AM"," FM","Digi"};
char BWCW[10][5]={"1000"," 800"," 750"," 600"," 500"," 400"," 250"," 100","  50","  25"};
char BWSSB[10][5]={"5000","4400","3800","3300","2900","2700","2400","2100","1800","1000"};
char BWAM[10][5]={"20 k","18 k","16 k","12 k","10 k","  9k","  8k","  7k","  6k","  5k"};
char SelBandMode=0;

char BWSwp[7][5]={"20 k","10 k"," 5 k"," 2 k"," 1 k"," 500"," 100"};
uint8_t ModeSwp[7]={4,4,4,3,1,1,1};
uint8_t BWNrSwp[7]={0,4,9,7,0,4,7};
uint16_t BWVSwp[7]={20000,10000,5000,2000,1000,500,100};
char   PtsSwp[4][4]={"240","120"," 64"," 32"};
uint16_t PtsVSwp[4]={240,120,64,32};
uint8_t ModeSwpNr=3;
uint8_t BWNrSweep=3;
uint8_t storcnt=0;

union {
  int32_t frequencyA;
  uint8_t frequencyAChar[4];
} unionA;

union {
  int32_t frequencyB;
  uint8_t frequencyBChar[4];
} unionB;
char Freq[9]="000000000";
char bFreqInput=0;
int32_t helpFreq;
char FrequCharA[13]="A 03.663.000";
char FrequCharB[13]="B 07.200.000";
char FrequChar[13]="B 07.200.000";
char FrequCharhelp[13]="B 07.200.000";
char z1[2]="0";
char z2[2]="0";
char z3[2]="0";
char z4[2]="0";
char z5[2]="0";
char z6[2]="0";
char z7[2]="0";
char z8[2]="0";
SBUTTON_t *bt1,*bt2,*bt3,*bt4,*bt5,*bt6,*bt7,*bt8;
SBUTTON_t *up1,*up2,*up3,*up4,*up5,*up6,*up7,*up8;
SBUTTON_t *dn1,*dn2,*dn3,*dn4,*dn5,*dn6,*dn7,*dn8;
SBUTTON_t *oldptr; //Zeiger für Frequenzeingabe (Ziffernposition)
char oldpos; // numerischer Zeiger für Frequenzeingabe (Ziffernposition)
char n0[2]="0";
char n1[2]="1";
char n2[2]="2";
char n3[2]="3";
char n4[2]="4";
char n5[2]="5";
char n6[2]="6";
char n7[2]="7";
char n8[2]="8";
char n9[2]="9";


uint8_t data_array[480];

uint8_t *pointer=&data_array[0], *oldpointer=NULL;
char new=1;

char *ModeTxt, *BWTxt, *AGCtxt;
char f1[7];

uint16_t timer1=0; // increases all 10 ms

uint8_t  count=0;

char  VolChar[3]="50";
SSLIDER_t *volume;
char  MicVolChar[3]="50";
SSLIDER_t *Micvolume;
SSLIDER_t* CalValue; // for Calibrate

uint8_t Out_Gain =5;
char Mic_txt[9]="00000000";

SGAUGE_t *SValGauge;
int32_t S_max = 100;
int32_t S_akt;
int32_t S_Val;
int32_t maxquer = 100;
uint16_t cnt1=0;
int16_t  maxcnt=0;
char  dBm_text[8]="- 73dBm";
char S_Text[6]="S9   ";
int16_t count50=0; // for 10 ms
SLABEL_t *label1, *label2, *label16; // S- Meter Texte
uint32_t cntr=0;
uint16_t cntr2=0;
uint8_t qspi1=8,qspi2=8,qspi3=8; //Init-,write-, read- status of Flash-ROM


//BandWidth values from Pavel Demin:
//CW: 25, 50, 100, 250, 400, 500, 600, 750, 800, 1.0k
//SSB: 1.0k, 1.8k, 2.1k, 2.4k, 2.7k, 2.9k, 3.3k, 3.8k, 4.4k, 5.0k
//AM: 5.0k, 6.0k, 7.0k, 8.0k, 9.0k, 10k, 12k, 16k, 18k, 20k

void delay10ms(uint16_t cycls){
  uint16_t tim,i;
  tim=timer1;
  for(i=0;i<cycls;i++){
    tim=timer1;
    while(timer1==tim){};
  }
  return;
}

uint8_t storeall(void){
  uint8_t retu;
  if ((retu=UB_QFlash_Erase_SubSector(0))==0)
    return UB_QFlash_Write_Block8b(1024,256,(uint8_t*)&DummyData[0]);
  return retu;
}

uint8_t restoreall(void){
  uint8_t res;
  res=UB_QFlash_Read_Block8b(1024,4,(uint8_t*)&reserve[0]); //
  if((reserve[0]==DummyData[0])&&(reserve[1]==DummyData[1])) // test, if Flash data are valid
    return UB_QFlash_Read_Block8b(1024,256,(uint8_t*)&DummyData[0]); // Yes - restore all
  else return storeall(); // No- first delete Flash then store data
}

uint32_t MakeFreq(char* freqText){ //"A 07.200.000" or "B 07.200.000"
  int32_t help=0;
  uint8_t i,z;
  for(i=2; i<12;i++){
    z=(uint8_t)freqText[i];
    if(z!='.'){
      help=10*help;
      help+=z&15;
    }
  }
  return help;
}

void MakeFrString(char* target, uint32_t frq, uint8_t length){ // result in char[] f1
  char i;
  int32_t freq;

  freq=frq/1000;
  target[length]=0;
  for(i=length;i>0;i--){
    if(i==3) target[i-1]='.'; //decimal point
    else {
      target[i-1]=(freq%10)+48; //"ASCII 0"
      freq=freq/10;
    }
  }

}


void DrawScale(void){
  uint16_t i;
  for(i=0;i<=480;i+=60){
    if(i==240) {
      UB_Graphic2D_DrawRectDMA(i-1,114,4,12,RGB_COL_YELLOW);
      UB_Graphic2D_DrawRectDMA(i,114,2,10,RGB_COL_BLACK);
    }
    UB_Graphic2D_DrawRectDMA(i,114,2,8,0x4208);
  }
  SGUI_TextSetDefFont(&Arial_8x13);
  SGUI_TextSetCursor(10,116);
  helpFreq=MakeFreq((char*)&FrequCharA[0])-step*points/2;
  for(i=0;i<8;i++){
    MakeFrString((char*)&f1[0],helpFreq,6);
    SGUI_TextSetCursor(10+60*i,116);

    if(i==4){
      SGUI_TextSetDefColor(RGB_COL_YELLOW,RGB_COL_BLACK);
      SGUI_TextPrintString((char*)&f1[0]);
      SGUI_TextSetDefColor(RGB_COL_BLACK,SGUI_WINCOL);
    }
    else SGUI_TextPrintString((char*)&f1[0]);
    helpFreq+=step*points/8;
  }
}

void ShowSignalStrength(void){
  int8_t i;
  int32_t S, dBmPlus;
  int32_t  S_Wert;

  maxcnt++;

  S_Wert=S=13-(S_akt/10); // -140 ... 13

  maxcnt=maxcnt&31;
  if(maxcnt==1){

    SGUI_GaugeSetValue(SValGauge,(140+S_Wert)); // immediate reaction

  }
  if(maxcnt==15){
    S=S_Wert; // -140 ... 13
    if(S<0){
      dBm_text[0]='-';
      S=-S;
    }
    else dBm_text[0]='+';
    for(i=3;i>0;i--){
      dBm_text[i]=(S%10)+48; // 48 = "ASCII 0"
      S=S/10;
    }
    if(dBm_text[1]=='0') {
      dBm_text[1]=' '; // remove leading zero
      if(dBm_text[2]=='0') {
       dBm_text[2]=' '; // remove leading zero
      }
    }
    SGUI_LabelSetText(label2,dBm_text); // "-  53dBm"   update String
  }
  if(maxcnt==30){
    maxcnt=0;
    S_Text[3]=' ';
    S_Text[4]=' ';
    S_Text[2]=' ';

    if(S_Wert<-73){
      S_Text[1]=(((127+S_Wert)/6)&15)+48;
    }
    else
    {
      dBmPlus=73+S_Wert;
      S_Text[4]=(dBmPlus%10)+48;
      S_Text[3]=(dBmPlus/10)+48;
      S_Text[2]='+';
      S_Text[1]='9'; //
    }
    SGUI_LabelSetText(label1,S_Text); // "S9+20"
  }
}

void UB_TIMER2_ISR_CallBack(void){ // von TIM2 aufgerufen (Interrupt 5 kHz)
  if(++count == StopCount)
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, 0);
  if(count ==20) {
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, 1);
    count=0;
  }
  if(++count50>=50){
    timer1++; // all 10 ms
    count50=0;
  }
}

int main(void)
{
  // init vom System
  UB_System_Init();

  // init der SGUI
  SGUI_Init(); // Graphic User Interface
  UB_Uart_Init(); // UART
  UB_TIMER2_Init(499,39); //Frequenz 5 kHz - for LCD Backlight dimming and timer1
  UB_TIMER2_Start();

  qspi1=UB_QFlash_Init();
  delay10ms(30);
  qspi3=restoreall(); //************************* restore data from Flash ROM ***************************
  FrequCharA[12]=FrequCharB[12]=FrequChar[12]=0;
  // alle Windows erzeugen
  create_MainWindow_01();
  create_ChildWindow_11();
  create_ChildWindow_12SSB();
  create_ChildWindow_12CW();
  create_ChildWindow_12AM();
  create_ChildWindow_13();
  create_ChildWindow_14();
  create_ChildWindow_15();
  create_ChildWindow_16();
  create_ChildWindow_18();
  create_ChildWindow_19();
  SelBandMode=1;
  SGUI_ListboxSetAktivItemNr(lb192,0); // select Memory 1
  SGUI_WindowShow(1); // erstes Window anzeigen

  btn_Select(true); // give data from Memory 1 to Red Pitaya

  while(1)
  {
    SGUI_Do(); // SGUI bearbeiten

    switch (bFreqInput){
  case 1:
      SGUI_ButtonSetAktiv(actBtn,true);
      bFreqInput=2;
      break;
  case 2:
      if(SGUI_ButtonIsAktiv(actBtn)){
      if (timer1>=100){ // 1 Sekunde
        timer1=0;
        bFreqInput=3;
      }
      }
      else{
        SGUI_ButtonSetAktiv(actBtn,false);
        bFreqInput=0;
      }
      break;
  case 3:
      if(SGUI_ButtonIsAktiv(actBtn)){
      if (timer1>=20){ // 0,2 Sekunden
        timer1=0;
        FrequShift(actShift);
        SendFreq();
      }
    }
    else {
      SGUI_ButtonSetAktiv(actBtn, false);
      bFreqInput=0; // Eingabe beendet
    }
      break;
  default:
      SGUI_ButtonSetAktiv(actBtn, false);
      bFreqInput=0; // Eingabe beendet
    }
    if(SelectSweep==0){
      S_Val=UB_Uart_ReceiveValue(COM6);
      if((S_Val>0)&&(S_Val<1530)){
        S_akt=S_Val + SValueCalibrat;
        // Testhilfe
        ShowSignalStrength();
      }
    }
    if(16==SGUI_WindowGetActivNr()){
      if(cntr==100000){
        cntr=0;
        sprintf(&writ[0],"%5d",SValueCalibrat);
        SGUI_LabelSetText(label16,writ);
      }
    }
    cntr2++;
    cntr++;
    if(cntr>=100001) cntr=0;
  }
}


//--------------------------------------------------------------
// Funktions-Handler der Buttons
//--------------------------------------------------------------
void btn_fkt(bool aktiv) {
  if(aktiv==false) {
   // SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
    new=1;

  }
}

void Brightness(void){
  StopCount=SGUI_SliderGetValue(bright);
}

void MicVol(void){
  MicVolumSet=SGUI_SliderGetValue(mic); // ((Send microphone volume control to Red Pitaya))
  UB_Uart_SendByte(COM6,4);
  UB_Uart_SendByte(COM6,MicVolumSet);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
}

void Volume(void){
  VolumSet=SGUI_SliderGetValue(volume); // ((Send speaker volume control to Red Pitaya))
  UB_Uart_SendByte(COM6,3);
  UB_Uart_SendByte(COM6,VolumSet);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
}

void ModeSelectRdy(uint16_t zeile){ // Listbox Mode
  uint8_t nr; // Mode Nr.
  ModeNr = zeile;
  ModeTxt=SGUI_ListboxGetItem(lb1,zeile);
  SGUI_ButtonSetText(btn11,ModeTxt);
  nr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb1);
  UB_Uart_SendByte(COM6,6); // Send Mode control to Red Pitaya
  UB_Uart_SendByte(COM6,nr);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8

  if(ModeNr<=1){
    BWNr=BWNrCW=nr;
    SGUI_ListboxSetAktivItemNr(lb12, BWNrCW);
    BWTxt=SGUI_ListboxGetItem(lb12,BWNrCW);
  }
  else if(ModeNr<=3){
    BWNr=BWNrSSB=nr;
    SGUI_ListboxSetAktivItemNr(lb2, BWNrSSB);
    BWTxt=SGUI_ListboxGetItem(lb2,BWNrSSB);

  }

  else{
    BWNr=BWNrAM=nr;
    SGUI_ListboxSetAktivItemNr(lb22, BWNrAM);
    BWTxt=SGUI_ListboxGetItem(lb22,BWNrAM);
  }
  SGUI_ButtonSetText(btn12,BWTxt); // aktuelle Bandbreite anzeigen
  UB_Uart_SendByte(COM6,5); // Send filter bandwidth control to Red Pitaya
  UB_Uart_SendByte(COM6,9-BWNr); //'9' ... '0'
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8

}

void BWSelect(bool aktiv) { // Button BW
  if(aktiv==true){
    if(ModeNr<=1){
      SGUI_WindowShow(120); // CW
      SGUI_ListboxSetAktivItemNr(lb12, BWNr);
    }
    else if (ModeNr<=3){
      SGUI_WindowShow(121); // SSB
      SGUI_ListboxSetAktivItemNr(lb2, BWNr );
    }
    else {
      SGUI_WindowShow(122); // AM,FM,Digi
      SGUI_ListboxSetAktivItemNr(lb22, BWNr);
    }
  }
  else{
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void CW_Keyer(bool aktiv) {

  if(aktiv==true){
    qspi2=storeall();
    delay10ms(100);
    qspi3=restoreall();
    SGUI_WindowShow(18); //

  }
  else{
    SGUI_WindowShow(1); // main-window anzeigen
  }
}

void BWSelectRdy(uint16_t zeile){


  BWNr = zeile;
  if(ModeNr <=1){ // CW
    BWTxt=SGUI_ListboxGetItem(lb12,zeile);
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb12);
  }
  else if(ModeNr<=3){ // SSB
    BWTxt=SGUI_ListboxGetItem(lb2,zeile);
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb2);
  }
  else{
    BWTxt=SGUI_ListboxGetItem(lb22,zeile); // AM, FM, Digi
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb22);
  }
  SGUI_ButtonSetText(btn12,BWTxt); // aktuelle Bandbreite anzeigen
  UB_Uart_SendByte(COM6,5); // Send filter bandwidth control to Red Pitaya
  UB_Uart_SendByte(COM6,9-BWNr); //'9' ... '0'
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
}


void AGCSelect(bool aktiv) {
  if(aktiv==true){
    SGUI_WindowShow(13);
  }
  else{
    //SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void AGCSelectRdy(uint16_t zeile){ // for future extensions
  uint8_t nr;
  char txt[3]="G0";

  AGCmode = zeile;
  AGCtxt=SGUI_ListboxGetItem(lb13,zeile);
  SGUI_ButtonSetText(btn13,AGCtxt);
  nr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb13);
  txt[1]=nr+48; // + "0"
  //UB_Uart_SendString(COM6,&txt[0],LF); // Send AGC control to Red Pitaya
}

void PreampRdy(){ // for future improvements
  UB_Uart_SendByte(COM6,11); // Send preamp/attenuator control to Red Pitaya
  if(SGUI_RadioButtonIsAktiv(rb1)){
    UB_Uart_SendByte(COM6,0); // Send preamp off control to Red Pitaya
  }
  else if(SGUI_RadioButtonIsAktiv(rb2)){
    UB_Uart_SendByte(COM6,1); // Send preamp on control to Red Pitaya
  }
  else {
    UB_Uart_SendByte(COM6,2); // Send attenuator on control to Red Pitaya
  }
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
}

void Settings(bool aktiv) {
  if(aktiv==true){
    SGUI_WindowShow(15);
    SGUI_ListboxSetAktivItemNr(lb151,stepline);
    SGUI_ListboxSetAktivItemNr(lb152,pointsline);
  }
  else{
    //SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

// ** Routinen für Frequenzeingabe **_____________________________________________________________
void SendFreq(){
  uint8_t i;

  if(split==0){ //RXfrequ = TXfrequ = FrequA
    unionA.frequencyA=MakeFreq(FrequCharA); // make frequency A as integer
    UB_Uart_SendByte(COM6,1); // Send frequency control to Red Pitaya
    for(i=0;i<4;i++){
      UB_Uart_SendByte(COM6, unionA.frequencyAChar[i]);
    }
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    UB_Uart_SendByte(COM6,2); // Send frequency control to Red Pitaya
    for(i=0;i<4;i++){
      UB_Uart_SendByte(COM6,unionA.frequencyAChar[i]);
    }
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }

  else{ // RXfrequ=FrequA   TXfreq=FreqB
    UB_Uart_SendByte(COM6,1); // Send frequency control to Red Pitaya
    for(i=0;i<4;i++){
      UB_Uart_SendByte(COM6,unionA.frequencyAChar[i]);
    }
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    unionB.frequencyB=MakeFreq(FrequCharB); // make frequency B as integer
    UB_Uart_SendByte(COM6,2); // Send frequency control to Red Pitaya
    for(i=0;i<4;i++){
      UB_Uart_SendByte(COM6,unionB.frequencyBChar[i]);
    }
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }


}

void SetFreq(char* targ){

  FrequChar[11]=z1[0];
  FrequChar[10]=z2[0];
  FrequChar[9]=z3[0];
  FrequChar[7]=z4[0];
  FrequChar[6]=z5[0];
  FrequChar[5]=z6[0];
  FrequChar[3]=z7[0];
  FrequChar[2]=z8[0];
  strncpy(targ,(char*)&FrequChar[0], 12);

}

void SetFreqBtns0(void){
  SGUI_ButtonSetText(bt1,(char*)z1);
  SGUI_ButtonSetText(bt2,(char*)z2);
  SGUI_ButtonSetText(bt3,(char*)z3);
  SGUI_ButtonSetText(bt4,(char*)z4);
  SGUI_ButtonSetText(bt5,(char*)z5);
  SGUI_ButtonSetText(bt6,(char*)z6);
  SGUI_ButtonSetText(bt7,(char*)z7);
  SGUI_ButtonSetText(bt8,(char*)z8);
}


void btn_Freqfkt(bool aktiv) {
  if(aktiv==false) {
    if(FrequChar[0]=='A'){
      SetFreq((char*)&FrequCharA[0]); // Frequenz zurückschreiben
    }
    else {
      SetFreq((char*)&FrequCharB[0]);
    }
    SendFreq();
    //create_MainWindow_01();
    SGUI_WindowShow(1);
    oldpointer=NULL; // FFT anzeigen
    new=1;
    //ShowFFT();
  }
}


void ModeSelect(bool aktiv) {
  if(aktiv==true){
    SGUI_WindowShow(11);
  }
  else{
    //SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void PrepFreqBtns(void){ // Prepare Frequency Buttons
  oldptr=bt8;
  oldpos=8;
  SGUI_ButtonSetColor(bt1,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt2,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt3,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt4,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt5,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt6,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt7,RGB_COL_BLACK,RGB_COL_GREY);
  SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,RGB_COL_GREY);
  z1[0]=FrequChar[11];
  z2[0]=FrequChar[10];
  z3[0]=FrequChar[9];
  z4[0]=FrequChar[7];
  z5[0]=FrequChar[6];
  z6[0]=FrequChar[5];
  z7[0]=FrequChar[3];
  z8[0]=FrequChar[2];
}
void frequA(bool aktiv){ // Button FrequA pressed
  if(aktiv==true){
    strncpy((char*)&FrequChar[0],(char*)&FrequCharA[0],12);

    PrepFreqBtns();
    SGUI_WindowShow(14);
    SetFreqBtns0();
    SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE); // Cursor setzen
  }
}

void frequB(bool aktiv){ // Button FrequB pressed
  if(aktiv==true){
    strncpy((char*)&FrequChar[0],(char*)&FrequCharB[0],12);
    PrepFreqBtns();
    SGUI_WindowShow(14);
    SetFreqBtns0();
    SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE); // Cursor setzen
  }
}

void ExchangeAB(bool aktiv){
  if(aktiv==true){
    strncpy((char*)&FrequChar[2],(char*)&FrequCharB[2],10);
    strncpy((char*)&FrequCharB[2],(char*)&FrequCharA[2],10);
    strncpy((char*)&FrequCharA[2],(char*)&FrequChar[2],10);
    SGUI_ButtonSetText(btn9,(char*)FrequCharA);
    SGUI_ButtonSetText(btn10,(char*)FrequCharB);
    SendFreq();
  }
}


void CopyAtoB(bool aktiv){
  if(aktiv==true){
    strncpy((char*)&FrequCharB[2],(char*)&FrequCharA[2],10);
    SGUI_ButtonSetText(btn10,(char*)FrequCharB);
    SendFreq();
  }
}

void CopyBtoA(bool aktiv){
  if(aktiv==true){
    strncpy((char*)&FrequCharA[2],(char*)&FrequCharB[2],10);
    SGUI_ButtonSetText(btn9,(char*)FrequCharA);
    SendFreq();
  }
}

void Split(bool aktiv){ //toggle split flag
  if(aktiv==true){
    if(split==0) split=1;
    else split=0;
    SendFreq();
  }
}

void Tune(bool aktiv){ // +++++ Tune command to Red Pitaya +++++
  if(aktiv==true){
    UB_Uart_SendByte(COM6,7); // Set PTT Bit
    UB_Uart_SendByte(COM6,1);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    UB_Uart_SendByte(COM6,8); // Set Transmitter on
    UB_Uart_SendByte(COM6,1);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }
  else{
    UB_Uart_SendByte(COM6,8); // Set Transmitter off
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    UB_Uart_SendByte(COM6,7); // Reset PTT Bit
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }
}

void PTT(bool aktiv){ // +++++ PTT to Red Pitaya +++++
  if(aktiv==true){
    if(ModeNr<2){ // only CW- modes:
      UB_Uart_SendByte(COM6,8); // Set Transmitter off
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0); // instead of CRC8
    }
    UB_Uart_SendByte(COM6,7); // Set PTT Bit
    UB_Uart_SendByte(COM6,1);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    if(ModeNr>1){ //non CW- modes
      UB_Uart_SendByte(COM6,8); // Set Transmitter on
      UB_Uart_SendByte(COM6,1);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0);
      UB_Uart_SendByte(COM6,0); // instead of CRC8
    }

  }
  else{
    UB_Uart_SendByte(COM6,8); // Set Transmitter off
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    UB_Uart_SendByte(COM6,7); // Reset PTT Bit
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }

}

void Mute(bool aktiv){ // +++++ PTT to Red Pitaya +++++
  if(aktiv==true){
    UB_Uart_SendByte(COM6,3); // Set Volume to Null
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }
  else{
    UB_Uart_SendByte(COM6,3); // Set Volume to old value
    UB_Uart_SendByte(COM6,VolumSet);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }

}
void BandMemSet(bool aktiv){ // +++++
  if(aktiv==true){
    SGUI_WindowShow(19);
    SGUI_ListboxSetAktivItemNr(lb191, 0);
    SGUI_ListboxSetAktivItemNr(lb192, 0);
    SGUI_ListboxSetAktivItemNr(lb193, 0);
    SGUI_ListboxSetAktivItemNr(lb194, 0);
  }
}
void Sweep(bool aktiv){ // +++++
  uint32_t i,j;

  if(aktiv==true){
    SelectSweep=1;

    UB_Uart_SendByte(COM6,6); // Send Mode control to Red Pitaya
    UB_Uart_SendByte(COM6,ModeSwpNr);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8

    UB_Uart_SendByte(COM6,5); // Send BW control to Red Pitaya
    UB_Uart_SendByte(COM6,9-BWNrSweep);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8

    unionA.frequencyA-=step*points/2;
    for(i=0;i<480;i++){
      UB_Uart_SendByte(COM6,1); // Send frequency control to Red Pitaya
      for(j=0;j<4;j++){
        UB_Uart_SendByte(COM6, unionA.frequencyAChar[j]);
      }
      UB_Uart_SendByte(COM6,0); // instead of CRC8
      S_Val=0;
      delay10ms(8); // wait 100 ms
      j=0;
      while(( j<10)&&(S_Val<=0)){
        j++;
        delay10ms(2); // wait 20 ms
        S_Val=UB_Uart_ReceiveValue(COM6);
      }
      if((S_Val>0)&&(S_Val<1530)){
        S_akt=(1530-S_Val);
      }
      for(j=0;j<480/points;j++){
        data_array[i]=S_akt/6;
        SGUI_GraphWriteColumn(graph,1,i,S_akt/17); // draw one column
        i++;
      }
      unionA.frequencyA+=step;
    }
  }
  // restore old values:
  SendFreq();
  UB_Uart_SendByte(COM6,6); // Send Mode control to Red Pitaya
  UB_Uart_SendByte(COM6,ModeNr);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
  UB_Uart_SendByte(COM6,5); // Send BW control to Red Pitaya
  UB_Uart_SendByte(COM6,9-BWNr);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0);
  UB_Uart_SendByte(COM6,0); // instead of CRC8
  SelectSweep=0;

}
void BtnCalibrate(bool aktiv);

//---------------------------------------------------------------*************---------------***** M A I N   W I N D O W *******************************
void create_MainWindow_01(void) { // ***************************************************************
  SBUTTON_t *btn2, *btn4, *btn5,*btn5a, *btn6,*btn7,*btn8,*btnA,*btnB; // , *btn3

  MAIN=SGUI_WindowCreateMain(1); // Main-Window (Nr=1)
  btn=SGUI_ButtonCreate(400,212,80,60); // button
  SGUI_ButtonSetFont(btn, &Arial_16x25);
  SGUI_ButtonSetColor(btn, RGB_COL_GREEN, RGB_COL_RED);
  SGUI_ButtonSetText(btn,"PTT");
  SGUI_ButtonSetMode(btn,SBUTTON_PUSHPULL);
  SGUI_ButtonSetHandler(btn,PTT);


  btn2=SGUI_ButtonCreate(310,212,80,60); // button
  SGUI_ButtonSetFont(btn2, &Arial_16x25);
  ;
  SGUI_ButtonSetText(btn2,"Mute");
  SGUI_ButtonSetMode(btn2,SBUTTON_PUSHPULL);
  SGUI_ButtonSetHandler(btn2,Mute);

  btn8=SGUI_ButtonCreate(2,0,60,22); // button
  SGUI_ButtonSetColor(btn8, RGB_COL_GREEN, RGB_COL_RED);
  SGUI_ButtonSetText(btn8,"Tune");
  SGUI_ButtonSetMode(btn8,SBUTTON_PUSHPULL);
  SGUI_ButtonSetHandler(btn8,Tune);

  btn5=SGUI_ButtonCreate(66,0,90,22); // button
  SGUI_ButtonSetText(btn5,"Settings");
  SGUI_ButtonSetHandler(btn5,Settings);

  btn5a=SGUI_ButtonCreate(160,0,90,22); // button
  SGUI_ButtonSetText(btn5a,"Sweep");
  SGUI_ButtonSetHandler(btn5a,Sweep);

  btn6=SGUI_ButtonCreate(254,0,72,22); // button
  SGUI_ButtonSetText(btn6,"Keyer");
  SGUI_ButtonSetHandler(btn6,CW_Keyer);

  btn7=SGUI_ButtonCreate(330,0,90,22); // button
  SGUI_ButtonSetHandler(btn7,BandMemSet);
  SGUI_ButtonSetText(btn7,"Band/Mem");

  graph=SGUI_GraphCreate(0,23,480,FFTHeight); // graph

  SGUI_GraphSetStyle(graph,STYLE_FLAT);
  SGUI_GraphSetFrameSize(graph,0);
  SGUI_GraphSetCHColor(graph,0,RGB_COL_BLUE);
  SGUI_GraphSetCHVisible(graph,0,true);

 // ShowFFT();
  btn10=SGUI_ButtonCreate(280,134,168,28); // Frequency B
  SGUI_ButtonSetFont(btn10, &Arial_13x19);
  SGUI_ButtonSetStyle(btn10,STYLE_FLAT);
  SGUI_ButtonSetColor(btn10, 0xFA40, 0x4208); // Rot- Orange
  SGUI_ButtonSetText(btn10,(char*)FrequCharB); // "B 07.197.000"
  SGUI_ButtonSetHandler(btn10,frequB);
  SGUI_TextSetCursor(452,140);
  SGUI_TextCreateString("Hz");

  btn11=SGUI_ButtonCreate(4,134,65,28); // button mode
  SGUI_ButtonSetFont(btn11,&Arial_11x18);
  SGUI_ButtonSetText(btn11,"LSB");
  SGUI_ButtonSetHandler(btn11,ModeSelect);

  btn12=SGUI_ButtonCreate(74,134,85,28); // button Bandwidth
  SGUI_ButtonSetFont(btn12,&Arial_11x18);
  SGUI_ButtonSetText(btn12,"2900 Hz");
  SGUI_ButtonSetHandler(btn12,BWSelect);

  btn13=SGUI_ButtonCreate(165,134,85,28); // button AGC
  SGUI_ButtonSetFont(btn13,&Arial_11x18);
  SGUI_ButtonSetText(btn13,"Medium");
  SGUI_ButtonSetHandler(btn13,AGCSelect);

  SGUI_TextSetCursor(378,172);
  SGUI_TextCreateString("Volume");

  volume=SGUI_SliderCreate(176,165,200,30); // Volume
  SGUI_SliderSetColor(volume,RGB_COL_GREY,0x076CE);
  SGUI_SliderSetMinMax(volume,0,10);
  SGUI_SliderSetStep(volume,1);
  SGUI_SliderSetValue(volume,VolumSet);
  SGUI_SliderSetHandler(volume, Volume);

 // SGUI_TextSetCursor(180,200);
 // SGUI_TextCreateString("Hz");
  label1=SGUI_LabelCreate(4,165,68,23); // S-Wert
  SGUI_LabelSetStyle(label1, STYLE_FLAT);
  SGUI_LabelSetText(label1,"S9+05");

  label2=SGUI_LabelCreate(74,165,98,23); // Feldstärke
  SGUI_LabelSetStyle(label2, STYLE_FLAT);
  SGUI_LabelSetText(label2,"- 68dBm");

  SValGauge=SGUI_GaugeCreate(4,188,168,20); // S- Meter
  SGUI_GaugeSetStyle(SValGauge,STYLE_FLAT);
  SGUI_GaugeSetColor(SValGauge,0x076CE,RGB_COL_GREY);
  SGUI_GaugeSetMinMax(SValGauge,0,153);
  SGUI_GaugeSetValue(SValGauge,67); // equals -73 dBm

  btn9=SGUI_ButtonCreate(4,214,168,28); // Frequency A
  SGUI_ButtonSetFont(btn9, &Arial_13x19);
  SGUI_ButtonSetStyle(btn9,STYLE_FLAT);
  SGUI_ButtonSetColor(btn9,RGB_COL_YELLOW , 0x4208); //
  SGUI_ButtonSetText(btn9, (char*)FrequCharA); // uint16_t i; dunkelgrau  0x630C   0x076CE lindgrün"A 59.123.456"
  SGUI_ButtonSetHandler(btn9,frequA);

  SGUI_TextSetCursor(180,220);
  SGUI_TextCreateString("Hz");

  btn5=SGUI_ButtonCreate(4,250,72,22); // button
  SGUI_ButtonSetMode(btn5,SBUTTON_PUSHPULL);
  SGUI_ButtonSetText(btn5,"Split");
  SGUI_ButtonSetAktiv(btn5,false);
  SGUI_ButtonSetHandler(btn5,Split);
  btn6=SGUI_ButtonCreate(80,250,60,22); // button
  SGUI_ButtonSetText(btn6,"A><B");
  SGUI_ButtonSetHandler(btn6,ExchangeAB);
  btnA=SGUI_ButtonCreate(148,250,60,22); // button
  SGUI_ButtonSetText(btnA,"A->B");
  SGUI_ButtonSetHandler(btnA,CopyAtoB);
  btnB=SGUI_ButtonCreate(216,250,60,22); // button
  SGUI_ButtonSetText(btnB,"A<-B");
  SGUI_ButtonSetHandler(btnB,CopyBtoA);

  btn151=SGUI_ButtonCreate(422,0,50,22); // button
  SGUI_ButtonSetText(btn151,"Cal");
  SGUI_ButtonSetHandler(btn151,BtnCalibrate);
}


//------------------------------------------------------------------------------- Mode selection ---------------------------------------------------
void create_ChildWindow_11(void) {
char i;

  ptr11=SGUI_WindowCreateChild(11,0,30,200,240); // Child-Window (Nr=11)
  SGUI_WindowSetColor(ptr11,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,4);
  SGUI_TextCreateString("Enter mode");    // Beschriftung

  lb1=SGUI_ListboxCreate(4,40,130,180); // Mode
  SGUI_ListboxSetFont(lb1,&Arial_16x25);
  SGUI_ListboxSetStyle(lb1, STYLE_FLAT);
  SGUI_ListboxSetSliderVisible(lb1,true);
  for(i=0;i<7;i++){
    SGUI_ListboxAddItem(lb1,&ModeTable[i][0]);
  }
  SGUI_ListboxSetHandler(lb1,ModeSelectRdy);


  btn=SGUI_ButtonCreate(140,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_12SSB(void) {
  char i;

  ptr121=SGUI_WindowCreateChild(121,20,0,460,270); // Child-Window (Nr=121)
  SGUI_WindowSetColor(ptr121,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb2=SGUI_ListboxCreate(150,10,140,258); // Bandwith SSB
  SGUI_ListboxSetStyle(lb2, STYLE_FLAT);
  SGUI_ListboxSetFont(lb2,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb2,true);
  for(i=0;i<9;i++)
    SGUI_ListboxAddItem(lb2,&BWSSB[i][0]);

  SGUI_ListboxSetHandler(lb2,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

void create_ChildWindow_12CW(void) {
  char i;

  ptr12CW=SGUI_WindowCreateChild(120,20,0,460,270); // Child-Window (Nr=16)
  SGUI_WindowSetColor(ptr12CW,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb12=SGUI_ListboxCreate(150,10,140,258); // Bandwith  CW lb12
  SGUI_ListboxSetStyle(lb12, STYLE_FLAT);
  SGUI_ListboxSetFont(lb12,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb12,true);
  for(i=0;i<9;i++)
    SGUI_ListboxAddItem(lb12,&BWCW[i][0]);

  SGUI_ListboxSetHandler(lb12,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

void create_ChildWindow_12AM(void) {
  char i;

  ptr17=SGUI_WindowCreateChild(122,20,0,460,270); // Child-Window (Nr=122)
  SGUI_WindowSetColor(ptr17,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb22=SGUI_ListboxCreate(150,10,140,258); // Bandwith AM lb22
  SGUI_ListboxSetStyle(lb22, STYLE_FLAT);
  SGUI_ListboxSetFont(lb22,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb22,true);
  for(i=0;i<9;i++)
    SGUI_ListboxAddItem(lb22,&BWAM[i][0]);
  SGUI_ListboxSetHandler(lb22,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}


//--------------------------------------------------------------**************************************************************
void create_ChildWindow_13(void) { // AGC (for future)

  ptr13=SGUI_WindowCreateChild(13,0,30,240,200); // Child-Window (Nr=13)
  SGUI_WindowSetColor(ptr13,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,10);
  SGUI_TextCreateString("Enter AGC");    // Beschriftung

  lb13=SGUI_ListboxCreate(4,40,160,150); // AGC
  SGUI_ListboxSetStyle(lb13, STYLE_FLAT);
  SGUI_ListboxSetFont(lb13,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb13,false);
  SGUI_ListboxAddItem(lb13,"Fast");
  SGUI_ListboxAddItem(lb13,"Medium");
  SGUI_ListboxAddItem(lb13,"Slow");
  SGUI_ListboxAddItem(lb13,"None");
  SGUI_ListboxSetHandler(lb13,AGCSelectRdy);

  btn130=SGUI_ButtonCreate(160,4,55,55); // ok-button
  SGUI_ButtonSetText(btn130,"OK");
  SGUI_ButtonSetHandler(btn130,btn_fkt);
}

// Helper Frequenzeingabe

void SetColorBtn(SBUTTON_t* ptr, char n){
  SendFreq();                    // aktuelle Frequenz an RedPitaya senden
  SGUI_ButtonSetColor(ptr,RGB_COL_BLACK,0x076CE); // Cursor setzen (lindgrün)
  if(oldptr == ptr) return;
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,RGB_COL_GREY); // alten Cursor löschen
  oldptr=ptr;
  oldpos=n;
}

void FrequShift(int32_t freqShift){

  if(FrequChar[0]=='A') {
    unionA.frequencyA=MakeFreq(FrequChar); // make frequency as integer
    helpFreq=unionA.frequencyA+freqShift;
    if((0<=helpFreq)&&(helpFreq <=61440000)){
      unionA.frequencyA=  helpFreq;
      itoa(helpFreq+100000000,Freq,10); // to create leading Zeroes
      z1[0]=Freq[8];
      z2[0]=Freq[7];
      z3[0]=Freq[6];
      z4[0]=Freq[5];
      z5[0]=Freq[4];
      z6[0]=Freq[3];
      z7[0]=Freq[2];
      z8[0]=Freq[1];
      SetFreqBtns0();
      SetFreq((char*)&FrequCharA[0]); // write back frequency
    }
    else return;
  }
  else{
    unionB.frequencyB=MakeFreq(FrequChar);
    helpFreq=unionB.frequencyB+freqShift;
    if((0<=helpFreq)&&(helpFreq <=61440000)){
      unionB.frequencyB=  helpFreq;
      itoa(helpFreq+100000000,Freq,10);
      z1[0]=Freq[8];
      z2[0]=Freq[7];
      z3[0]=Freq[6];
      z4[0]=Freq[5];
      z5[0]=Freq[4];
      z6[0]=Freq[3];
      z7[0]=Freq[2];
      z8[0]=Freq[1];
      SetFreqBtns0();
      SetFreq((char*)&FrequCharB[0]); // Frequenz zurückschreiben
    }
  }
}
void ComputeBtn(int32_t freqShift, SBUTTON_t* ptr, char n, SBUTTON_t* qbtn){
  FrequShift(freqShift);
  SetColorBtn(ptr,n);
  actBtn=qbtn;
  actShift=freqShift;
  bFreqInput=1;
  timer1=0;
}


void fktup1(bool aktiv){ // Einer
  if(aktiv==true) {

    if(z1[0]<'9'){
      z1[0]++;
      SGUI_ButtonSetText(bt1,z1);
    }
    ComputeBtn(1,bt1,1,up1);
  }
}

void fktdn1(bool aktiv){
  if(aktiv==true) {

    if(z1[0]>'0'){
      z1[0]--;
      SGUI_ButtonSetText(bt1,z1);
    }
    ComputeBtn(-1,bt1,1,dn1);
  }
}
void fktbt1(bool aktiv){
  SetColorBtn(bt1,1);
}

void fktup2(bool aktiv){ // Zehner
  if(aktiv==true) {

    if(z2[0]<'9'){
      z2[0]++;
      SGUI_ButtonSetText(bt2,z2);
    }
    ComputeBtn(10,bt2,2,up2);
  }
}
void fktdn2(bool aktiv){
  if(aktiv==true) {

    if(z2[0]>'0'){
      z2[0]--;
      SGUI_ButtonSetText(bt2,z2);
    }
    ComputeBtn(-10,bt2,2,dn2);
  }
}
void fktbt2(bool aktiv){
  SetColorBtn(bt2,2);
}

void fktup3(bool aktiv){ // Hunderter
  if(aktiv==true) {

    if(z3[0]<'9'){
      z3[0]++;
      SGUI_ButtonSetText(bt3,z3);
    }
    ComputeBtn(100,bt3,3,up3);
  }
}
void fktdn3(bool aktiv){
  if(aktiv==true) {

    if(z3[0]>'0'){
      z3[0]--;
      SGUI_ButtonSetText(bt3,z3);
    }
    ComputeBtn(-100,bt3,3,dn3);
  }
}
void fktbt3(bool aktiv){
  SetColorBtn(bt3,3);
}

void fktup4(bool aktiv){
  if(aktiv==true) {
    if(z4[0]<'9'){
      z4[0]++;
      SGUI_ButtonSetText(bt4,z4);
    }
    ComputeBtn(1000,bt4,4,up4);
  }
}
void fktdn4(bool aktiv){
  if(aktiv==true) {

    if(z4[0]>'0'){
      z4[0]--;
      SGUI_ButtonSetText(bt4,z4);
    }
    ComputeBtn(-1000,bt4,4,dn4);
  }
}
void fktbt4(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt4)) return;
  SetColorBtn(bt4,4);
}

void fktup5(bool aktiv){
  if(aktiv==true) {

    if(z5[0]<'9'){
      z5[0]++;
      SGUI_ButtonSetText(bt5,z5);
    }
    ComputeBtn(10000,bt5,5,up5);
  }
}
void fktdn5(bool aktiv){
  if(aktiv==true) {

    if(z5[0]>'0'){
      z5[0]--;
      SGUI_ButtonSetText(bt5,z5);
    }
    ComputeBtn(-10000,bt5,5,dn5);
  }
}
void fktbt5(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt5)) return;
  SetColorBtn(bt5,5);
}
void fktup6(bool aktiv){
  if(aktiv==true) {

    if(z6[0]<'9'){
      z6[0]++;
      SGUI_ButtonSetText(bt6,z6);
    }
    ComputeBtn(100000,bt6,6,up6);
  }
}
void fktdn6(bool aktiv){
  if(aktiv==true) {
    if(z6[0]>'0'){
      z6[0]--;
      SGUI_ButtonSetText(bt6,z6);
    }
    ComputeBtn(-100000,bt6,6,dn6);
  }
}
void fktbt6(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt6)) return;
  SetColorBtn(bt6,6);
}

void fktup7(bool aktiv){
  if(aktiv==true) {
    if(z7[0]<'9'){
      z7[0]++;
      SGUI_ButtonSetText(bt7,z7);
    }
    ComputeBtn(1000000,bt7,7,up7);

  }
}
void fktdn7(bool aktiv){
  if(aktiv==true) {
    if(z7[0]>'0'){
      z7[0]--;
      SGUI_ButtonSetText(bt7,z7);
    }
    ComputeBtn(-1000000,bt7,7,dn7);
  }
}
void fktbt7(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt7)) return;
  SetColorBtn(bt7,7);
}
void fktup8(bool aktiv){
  if(aktiv==true) {
    if(z8[0]<'6'){
      z8[0]++;
      SGUI_ButtonSetText(bt8,z8);
    }
    ComputeBtn(10000000,bt8,8,up8);
  }
}
void fktdn8(bool aktiv){
  if(aktiv==true) {
    if(z8[0]>'0'){
      z8[0]--;
      SGUI_ButtonSetText(bt8,z8);
    }
    ComputeBtn(-10000000,bt8,8,dn8);

  }
}
void fktbt8(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt8)) return;
  SetColorBtn(bt8,8);
}

void Setptr(char* n){

  SGUI_ButtonSetText(oldptr,n);
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,RGB_COL_GREY); // alten Cursor löschen
  switch (oldpos)  {
    case 8: {
      z8[0]=n[0];
      oldptr=bt7;
      break;
    }
    case 7: {
      z7[0]=n[0];
      oldptr=bt6;
      break;
    }
    case 6: {
      z6[0]=n[0];
      oldptr=bt5;
      break;
    }
    case 5: {
      z5[0]=n[0];
      oldptr=bt4;
      break;
    }
    case 4: {
      z4[0]=n[0];
      oldptr=bt3;
      break;
    }
    case 3: {
      z3[0]=n[0];
      oldptr=bt2;
      break;
    }
    case 2: {
      z2[0]=n[0];
      oldptr=bt1;
      break;
    }
    case 1: {
      z1[0]=n[0];
    }
  }
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,0x076CE); // neuen Cursor setzen
  if(oldpos>1) oldpos--;

}

void fkt00a(bool aktiv){
  if(aktiv==true) {
    Setptr(n0);
  }
}
void fkt01(bool aktiv){
  if(aktiv==true) {
    Setptr(n1);
  }
}
void fkt02(bool aktiv){
  if(aktiv==true) {
    Setptr(n2);
  }
}
void fkt03(bool aktiv){
  if(aktiv==true) {
    Setptr(n3);
  }
}
void fkt04(bool aktiv){
  if(aktiv==true) {
    Setptr(n4);
  }
}
void fkt05(bool aktiv){
  if(aktiv==true) {
    Setptr(n5);
  }
}
void fkt06(bool aktiv){
  if(aktiv==true) {
    Setptr(n6);
  }
}
void fkt07(bool aktiv){
  if(aktiv==true) {
    Setptr(n7);
  }
}
void fkt08(bool aktiv){
  if(aktiv==true) {
    Setptr(n8);
  }
}
void fkt09(bool aktiv){
  if(aktiv==true) {
    Setptr(n9);
  }
}

//---------------------------------------------------------------------------------------- frequency input ----------------------------------------
void create_ChildWindow_14(void) { // Frequenzeingabe

  SBUTTON_t *bt00,*bt01,*bt02,*bt03,*bt04,*bt05,*bt06,*bt07,*bt08,*bt09;

  oldptr=bt8;
  ptr14=SGUI_WindowCreateChild(14,0,10,480,262); // Child-Window (Nr=14)
  SGUI_WindowSetColor(ptr14,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,170);
  SGUI_TextSetDefFont(&Arial_11x18);
  SGUI_TextCreateString("Enter frequency");    // Beschriftung

  up1=SGUI_ButtonCreate(420,2,54,50); // IntEdit1
  SGUI_ButtonSetStyle(up1,STYLE_FLAT);
  SGUI_ButtonSetText(up1,"+");
  SGUI_ButtonSetHandler(up1,fktup1);
  bt1=SGUI_ButtonCreate(420,52,54,50); //
  SGUI_ButtonSetStyle(bt1,STYLE_FLAT);
  SGUI_ButtonSetFont(bt1,&Arial_16x25);
  SGUI_ButtonSetHandler(bt1,fktbt1);
  SGUI_ButtonSetText(bt1,(char*)z1);
  dn1=SGUI_ButtonCreate(420,102,54,50); //
  SGUI_ButtonSetStyle(dn1,STYLE_FLAT);
  SGUI_ButtonSetText(dn1,"-");
  SGUI_ButtonSetHandler(dn1,fktdn1);

  up2=SGUI_ButtonCreate(362,2,54,50); // IntEdit2
  SGUI_ButtonSetStyle(up2,STYLE_FLAT);
  SGUI_ButtonSetText(up2,"+");
  SGUI_ButtonSetHandler(up2,fktup2);
  bt2=SGUI_ButtonCreate(362,52,54,50); //
  SGUI_ButtonSetStyle(bt2,STYLE_FLAT);
  SGUI_ButtonSetFont(bt2,&Arial_16x25);
  SGUI_ButtonSetHandler(bt2,fktbt2);
  SGUI_ButtonSetText(bt2,(char*)z2);
  dn2=SGUI_ButtonCreate(362,102,54,50); //
  SGUI_ButtonSetStyle(dn2,STYLE_FLAT);
  SGUI_ButtonSetText(dn2,"-");
  SGUI_ButtonSetHandler(dn2,fktdn2);

  up3=SGUI_ButtonCreate(304,2,54,50); // IntEdit3
  SGUI_ButtonSetStyle(up3,STYLE_FLAT);
  SGUI_ButtonSetText(up3,"+");
  SGUI_ButtonSetHandler(up3,fktup3);
  bt3=SGUI_ButtonCreate(304,52,54,50); //
  SGUI_ButtonSetStyle(bt3,STYLE_FLAT);
  SGUI_ButtonSetFont(bt3,&Arial_16x25);
  SGUI_ButtonSetHandler(bt3,fktbt3);
  SGUI_ButtonSetText(bt3,(char*)z3);
  dn3=SGUI_ButtonCreate(304,102,54,50); //
  SGUI_ButtonSetStyle(dn3,STYLE_FLAT);
  SGUI_ButtonSetText(dn3,"-");
  SGUI_ButtonSetHandler(dn3,fktdn3);

  up4=SGUI_ButtonCreate(242,2,54,50); // IntEdit4
  SGUI_ButtonSetStyle(up4,STYLE_FLAT);
  SGUI_ButtonSetText(up4,"+");
  SGUI_ButtonSetHandler(up4,fktup4);
  bt4=SGUI_ButtonCreate(242,52,54,50); //
  SGUI_ButtonSetStyle(bt4,STYLE_FLAT);
  SGUI_ButtonSetFont(bt4,&Arial_16x25);
  SGUI_ButtonSetHandler(bt4,fktbt4);
  SGUI_ButtonSetText(bt4,(char*)z4);
  dn4=SGUI_ButtonCreate(242,102,54,50); //
  SGUI_ButtonSetStyle(dn4,STYLE_FLAT);
  SGUI_ButtonSetText(dn4,"-");
  SGUI_ButtonSetHandler(dn4,fktdn4);

  up5=SGUI_ButtonCreate(184,2,54,50); // IntEdit5
  SGUI_ButtonSetStyle(up5,STYLE_FLAT);
  SGUI_ButtonSetText(up5,"+");
  SGUI_ButtonSetHandler(up5,fktup5);
  bt5=SGUI_ButtonCreate(184,52,54,50); //
  SGUI_ButtonSetStyle(bt5,STYLE_FLAT);
  SGUI_ButtonSetFont(bt5,&Arial_16x25);
  SGUI_ButtonSetHandler(bt5,fktbt5);
  SGUI_ButtonSetText(bt5,(char*)z5);
  dn5=SGUI_ButtonCreate(184,102,54,50); //
  SGUI_ButtonSetStyle(dn5,STYLE_FLAT);
  SGUI_ButtonSetText(dn5,"-");
  SGUI_ButtonSetHandler(dn5,fktdn5);

  up6=SGUI_ButtonCreate(126,2,54,50); // IntEdit6
  SGUI_ButtonSetStyle(up6,STYLE_FLAT);
  SGUI_ButtonSetText(up6,"+");
  SGUI_ButtonSetHandler(up6,fktup6);
  bt6=SGUI_ButtonCreate(126,52,54,50); //
  SGUI_ButtonSetStyle(bt6,STYLE_FLAT);
  SGUI_ButtonSetFont(bt6,&Arial_16x25);
  SGUI_ButtonSetHandler(bt6,fktbt6);
  SGUI_ButtonSetText(bt6,(char*)z6);
  dn6=SGUI_ButtonCreate(126,102,54,50); //
  SGUI_ButtonSetStyle(dn6,STYLE_FLAT);
  SGUI_ButtonSetText(dn6,"-");
  SGUI_ButtonSetHandler(dn6,fktdn6);

  up7=SGUI_ButtonCreate(64,2,54,50); // IntEdit7
  SGUI_ButtonSetStyle(up7,STYLE_FLAT);
  SGUI_ButtonSetText(up7,"+");
  SGUI_ButtonSetHandler(up7,fktup7);
  bt7=SGUI_ButtonCreate(64,52,54,50); //
  SGUI_ButtonSetStyle(bt7,STYLE_FLAT);
  SGUI_ButtonSetFont(bt7,&Arial_16x25);
  SGUI_ButtonSetHandler(bt7,fktbt7);
  SGUI_ButtonSetText(bt7,(char*)z7);
  dn7=SGUI_ButtonCreate(64,102,54,50); //
  SGUI_ButtonSetStyle(dn7,STYLE_FLAT);
  SGUI_ButtonSetText(dn7,"-");
  SGUI_ButtonSetHandler(dn7,fktdn7);

  up8=SGUI_ButtonCreate(6,2,54,50); // IntEdit8
  SGUI_ButtonSetStyle(up8,STYLE_FLAT);
  SGUI_ButtonSetText(up8,"+");
  SGUI_ButtonSetHandler(up8,fktup8);
  bt8=SGUI_ButtonCreate(6,52,54,50); //
  SGUI_ButtonSetStyle(bt8,STYLE_FLAT);
  SGUI_ButtonSetFont(bt8,&Arial_16x25);
  SGUI_ButtonSetHandler(bt8,fktbt8);
  SGUI_ButtonSetText(bt8,(char*)z8);
  dn8=SGUI_ButtonCreate(6,102,54,50); //
  SGUI_ButtonSetStyle(dn8,STYLE_FLAT);
  SGUI_ButtonSetText(dn8,"-");
  SGUI_ButtonSetHandler(dn8,fktdn8);

  // Zifferntasten::

  bt00=SGUI_ButtonCreate(180,160,58,50); //
  SGUI_ButtonSetStyle(bt00,STYLE_FLAT);
  SGUI_ButtonSetFont(bt00,&Arial_16x25);
  SGUI_ButtonSetHandler(bt00,fkt00a);
  SGUI_ButtonSetText(bt00,"0");

  bt01=SGUI_ButtonCreate(238,160,58,50); //
  SGUI_ButtonSetStyle(bt01,STYLE_FLAT);
  SGUI_ButtonSetFont(bt01,&Arial_16x25);
  SGUI_ButtonSetHandler(bt01,fkt01);
  SGUI_ButtonSetText(bt01,"1");

  bt02=SGUI_ButtonCreate(296,160,58,50); //
  SGUI_ButtonSetStyle(bt02,STYLE_FLAT);
  SGUI_ButtonSetFont(bt02,&Arial_16x25);
  SGUI_ButtonSetHandler(bt02,fkt02);
  SGUI_ButtonSetText(bt02,"2");

  bt03=SGUI_ButtonCreate(354,160,58,50); //
  SGUI_ButtonSetStyle(bt03,STYLE_FLAT);
  SGUI_ButtonSetFont(bt03,&Arial_16x25);
  SGUI_ButtonSetHandler(bt03,fkt03);
  SGUI_ButtonSetText(bt03,"3");

  bt04=SGUI_ButtonCreate(412,160,58,50); //
  SGUI_ButtonSetStyle(bt04,STYLE_FLAT);
  SGUI_ButtonSetFont(bt04,&Arial_16x25);
  SGUI_ButtonSetHandler(bt04,fkt04);
  SGUI_ButtonSetText(bt04,"4");

  bt05=SGUI_ButtonCreate(180,210,58,50); //
  SGUI_ButtonSetStyle(bt05,STYLE_FLAT);
  SGUI_ButtonSetFont(bt05,&Arial_16x25);
  SGUI_ButtonSetHandler(bt05,fkt05);
  SGUI_ButtonSetText(bt05,"5");

  bt06=SGUI_ButtonCreate(238,210,58,50); //
  SGUI_ButtonSetStyle(bt06,STYLE_FLAT);
  SGUI_ButtonSetFont(bt06,&Arial_16x25);
  SGUI_ButtonSetHandler(bt06,fkt06);
  SGUI_ButtonSetText(bt06,"6");

  bt07=SGUI_ButtonCreate(296,210,58,50); //
  SGUI_ButtonSetStyle(bt07,STYLE_FLAT);
  SGUI_ButtonSetFont(bt07,&Arial_16x25);
  SGUI_ButtonSetHandler(bt07,fkt07);
  SGUI_ButtonSetText(bt07,"7");

  bt08=SGUI_ButtonCreate(354,210,58,50); //
  SGUI_ButtonSetStyle(bt08,STYLE_FLAT);
  SGUI_ButtonSetFont(bt08,&Arial_16x25);
  SGUI_ButtonSetHandler(bt08,fkt08);
  SGUI_ButtonSetText(bt08,"8");

  bt09=SGUI_ButtonCreate(412,210,58,50); //
  SGUI_ButtonSetStyle(bt09,STYLE_FLAT);
  SGUI_ButtonSetFont(bt09,&Arial_16x25);
  SGUI_ButtonSetHandler(bt09,fkt09);
  SGUI_ButtonSetText(bt09,"9");

  SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE); // Cursor setzen

  btn=SGUI_ButtonCreate(10,200,80,60); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_Freqfkt);
}

void Exit(bool aktiv);

void Exit2(bool aktiv){
  if(aktiv==true) {
    SGUI_ButtonSetText(btnX,"stopped");
    SGUI_ButtonSetHandler(btnX,Exit);
    UB_Uart_SendByte(COM6,9); // Red Pitaya: Exit from main.py
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,1);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }
}

void Exit(bool aktiv){
  if(aktiv==true) {
    SGUI_ButtonSetText(btnX,"shure?");
    SGUI_ButtonSetHandler(btnX,Exit2);
  }
}


void Sweep1Rdy(uint16_t zeile){ //set sweep step
  stepline=zeile;
  step=BWVSwp[zeile]; // caller: Sweep BW lb151
  ModeSwpNr=ModeSwp[zeile];
  BWNrSweep=BWNrSwp[zeile];
}

void Sweep2Rdy(uint16_t zeile){ // set sweep points
  pointsline=zeile;
  points=PtsVSwp[zeile]; // caller: Sweep points lb152
}

void BtnCalibrate(bool aktiv){
  int16_t value;
  if(aktiv==true){
    SGUI_WindowShow(16);
  }
  else{
    //SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void StoreSettings(bool aktiv){
  if(aktiv==true){
    storeall();
  }
  else{
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

//---------------------------------------------------------------------------------------------------- Settings ---------***************************
void create_ChildWindow_15(void) {
  char i;

  ptr15=SGUI_WindowCreateChild(15,0,10,472,260); // Child-Window (Nr=15)
  SGUI_WindowSetColor(ptr15,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,4);
  SGUI_TextCreateString("Settings");    // Beschriftung

  rb1=SGUI_RadioButtonCreate(10,30,20); // Mode
  SGUI_RadioButtonSetGroup(rb1,1);
  SGUI_RadioButtonSetHandler(rb1,PreampRdy);
  SGUI_TextSetCursor(40,40);
  SGUI_TextCreateString("Normal");
  SGUI_RadioButtonSetAktiv(rb1);
  rb2=SGUI_RadioButtonCreate(10,60,20); // Mode
  SGUI_RadioButtonSetGroup(rb2,1);
  SGUI_TextSetCursor(40,70);
  SGUI_TextCreateString("Preamp +10dB");
  SGUI_RadioButtonSetHandler(rb2,PreampRdy);
  rb3=SGUI_RadioButtonCreate(10,90,20); // Mode
  SGUI_RadioButtonSetGroup(rb3,1);
  SGUI_TextSetCursor(40,100);
  SGUI_TextCreateString("Attenuator -20dB");
  SGUI_RadioButtonSetHandler(rb3,PreampRdy);

  SGUI_TextSetCursor(10,130);
  SGUI_TextCreateString("Brightness");

  bright=SGUI_SliderCreate(10,150,200,30); // Brigthness
  SGUI_SliderSetColor(bright,RGB_COL_GREY,RGB_COL_BLUE);
  SGUI_SliderSetStep(bright,1);
  SGUI_SliderSetValue(bright,StopCount);
  SGUI_SliderSetMinMax(bright,1,18);
  SGUI_SliderSetHandler(bright,Brightness);

  SGUI_TextSetCursor(10,195);
  SGUI_TextCreateString("Microphone Volume");

  mic=SGUI_SliderCreate(10,215,220,30); // Microphone Volume
  SGUI_SliderSetColor(mic,RGB_COL_GREY,RGB_COL_BLUE);
  SGUI_SliderSetStep(mic,1);
  SGUI_SliderSetValue(mic,MicVolumSet);
  SGUI_SliderSetMinMax(mic,0,9);
  SGUI_SliderSetHandler(mic,MicVol);
  SGUI_TextSetCursor(220,70);
  SGUI_TextCreateString("Step size");
  lb151=SGUI_ListboxCreate(240,90,86,160); // Sweep Step = BW lb151
  SGUI_ListboxSetStyle(lb151, STYLE_FLAT);
  SGUI_ListboxSetFont(lb151,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb151,true);
  SGUI_ListboxSetHandler(lb151,Sweep1Rdy);
  for(i=0;i<7;i++){
    SGUI_ListboxAddItem(lb151,&BWSwp[i][0]);
  }
  SGUI_ListboxSetAktivItemNr(lb151,stepline);
  SGUI_TextSetCursor(330,70);
  SGUI_TextCreateString("Sweep steps");
  lb152=SGUI_ListboxCreate(330,90,86,120); // Sweep points lb152
  SGUI_ListboxSetStyle(lb152, STYLE_FLAT);
  SGUI_ListboxSetFont(lb152,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb152,true);
  SGUI_ListboxSetHandler(lb152,Sweep2Rdy);
  for(i=0;i<4;i++){
    SGUI_ListboxAddItem(lb152,&PtsSwp[i][0]);
  }
  SGUI_ListboxSetAktivItemNr(lb152,pointsline);
  btnX=SGUI_ButtonCreate(370,4,85,38); // Exit Button
  SGUI_ButtonSetColor(btnX, RGB_COL_GREEN, RGB_COL_RED);
  SGUI_ButtonSetText(btnX,"Exit");
  SGUI_ButtonSetHandler(btnX,Exit);

  btn=SGUI_ButtonCreate(120,4,90,30); // ok-button
  SGUI_ButtonSetText(btn,"Store");
  SGUI_ButtonSetHandler(btn,StoreSettings);

  btn=SGUI_ButtonCreate(220,4,80,30); // ok-button
  SGUI_ButtonSetText(btn,"Back");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}




void Store(bool aktiv){
  if(aktiv==true){
    SValueCalibrat=(int16_t)SGUI_SliderGetValue(CalValue);
    storeall();
  }
  else {
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void ShowValue(void){

  SValueCalibrat=(int16_t)SGUI_SliderGetValue(CalValue);

}

void create_ChildWindow_16(void) {

  ptr16=SGUI_WindowCreateChild(16,0,10,472,260); // Child-Window (Nr=16)***************************************** Calibrate ************************
  SGUI_WindowSetColor(ptr16,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,4);
  SGUI_TextCreateString("Calibrate S- meter");    // Beschriftung
  Storebtn=SGUI_ButtonCreate(120,220,180,30); // button
  SGUI_ButtonSetStyle(Storebtn,STYLE_RAISED);
  SGUI_ButtonSetText(Storebtn,"Store Data");
  SGUI_ButtonSetHandler(Storebtn,Store);

  CalValue=SGUI_SliderCreate(10,85,400,30); // S_Calibrate
  SGUI_SliderSetColor(CalValue,RGB_COL_GREY,RGB_COL_BLUE);
  SGUI_SliderSetMinMax(CalValue,-300,300);
  SGUI_SliderSetStep(CalValue,10);
  SGUI_SliderSetValue(CalValue,SValueCalibrat);
  SGUI_SliderSetHandler(CalValue,ShowValue);

  label16=SGUI_LabelCreate(10,35,100,35); // S- correction
  SGUI_LabelSetStyle(label16, STYLE_FLAT);

  btn=SGUI_ButtonCreate(390,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

void MorseKey(bool aktiv){
  if(aktiv==true){
    UB_Uart_SendByte(COM6,8); // Set Transmitter on
    UB_Uart_SendByte(COM6,1);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }
  else{
    UB_Uart_SendByte(COM6,8); // Set Transmitter off
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
  }

}

void create_ChildWindow_18(void) { // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ CW Keyer ++++++++++++++++++++++++++++++++++++++++++++++
char text[8]={"......."};



  ptr18=SGUI_WindowCreateChild(18,0,10,400,260); // Child-Window (Nr=18)
  SGUI_WindowSetColor(ptr18,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,4);
  SGUI_TextCreateString("CW- Keyer");    // Beschriftung
  cwbtn=SGUI_ButtonCreate(120,112,120,60); // button
  SGUI_ButtonSetFont(cwbtn, &Arial_16x25);
  SGUI_ButtonSetColor(cwbtn, RGB_COL_GREEN, RGB_COL_RED);
  SGUI_ButtonSetText(cwbtn,"Key");
  SGUI_ButtonSetMode(cwbtn,SBUTTON_PUSH);
  SGUI_ButtonSetHandler(cwbtn,MorseKey);


  btn=SGUI_ButtonCreate(140,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}



void btn_Store(bool aktiv){ // from ChildWindow_19
int8_t nr,line;
char* text;
  if(aktiv==true) {
    nr=SGUI_ListboxGetAktivItemNr(lb191);
    if(storcnt==0){
      if(nr==-1){
        SGUI_TextSetCursor(260,6);
        SGUI_TextSetDefFont(&Arial_11x18);
        SGUI_TextPrintString("first Select"); // error message
        SGUI_TextSetCursor(260,26);
        SGUI_TextPrintString("a line");
        delay10ms(300);
      }
      else {
        UB_Graphic2D_DrawFullRectDMA(260,12,130,41,0x076CE); // delete error message
        SGUI_TextSetDefFont(&Arial_11x18);
        SGUI_TextSetCursor(260,6);
        text=SGUI_ListboxGetItem(lb191,nr);
        SGUI_TextPrintString(text);
        SGUI_TextSetCursor(260,26);
        SGUI_TextPrintString("store ?");
        delay10ms(100);
        storcnt++;
      }
    }
    else {
      if(SelBandMode==0){
        bandfreq[nr+BandPtr]=unionA.frequencyA;
        bandmode[nr+BandPtr]=ModeNr;
        bandwidth[nr+BandPtr]=BWNr;
      }
      else{
        memofreq[nr+MemoPtr]=unionA.frequencyA;
        memomode[nr+MemoPtr]=ModeNr;
        memowidth[nr+MemoPtr]=BWNr;
      }
      storcnt=0;
      SetBoxes();
      qspi2=storeall(); // store in Flash memory
      SGUI_ListboxSetAktivItemNr(lb192,nr);
      SGUI_ListboxSetAktivItemNr(lb192,nr);
      SGUI_ListboxSetAktivItemNr(lb193,nr);
      SGUI_ListboxSetAktivItemNr(lb194,nr);
    }
  }
}

void BandSelectRdy(uint16_t zeile){
  int8_t nr;
  char* text;

  nr=SGUI_ListboxGetAktivItemNr(lb191);
  SGUI_ListboxSetAktivItemNr(lb192,nr);
  SGUI_ListboxSetAktivItemNr(lb193,nr);
  SGUI_ListboxSetAktivItemNr(lb194,nr);
  UB_Graphic2D_DrawFullRectDMA(260,12,130,41,0x076CE);
  SGUI_TextSetCursor(260,6);
  SGUI_TextSetDefFont(&Arial_11x18);
  text=SGUI_ListboxGetItem(lb191,nr);
  SGUI_TextPrintString(text); // Beschriftung

}
void FreqSelectRdy(uint16_t zeile){
  int8_t nr;
  char* text;

  nr=SGUI_ListboxGetAktivItemNr(lb192);
  SGUI_ListboxSetAktivItemNr(lb191,nr);
  SGUI_ListboxSetAktivItemNr(lb193,nr);
  SGUI_ListboxSetAktivItemNr(lb194,nr);
  UB_Graphic2D_DrawFullRectDMA(260,12,130,41,0x076CE);
  SGUI_TextSetCursor(260,6);
  SGUI_TextSetDefFont(&Arial_11x18);
  text=SGUI_ListboxGetItem(lb191,nr);
  SGUI_TextPrintString(text); // Beschriftung

}

void btn_Select(bool aktiv){
  uint8_t nr, zeile; // Band Nr /Memory Nr.

  if(aktiv==true) {
    nr=SGUI_ListboxGetAktivItemNr(lb192);
    if(nr==-1) return; // nothing to do
    if(SelBandMode==0){
      nr+=BandPtr;
      unionA.frequencyA=  bandfreq[nr];
    }
    else{
      nr+=MemoPtr;
      unionA.frequencyA=  memofreq[nr];
    }
    itoa(unionA.frequencyA+100000000,Freq,10); // to create leading Zeroes
    z1[0]=Freq[8];
    z2[0]=Freq[7];
    z3[0]=Freq[6];
    z4[0]=Freq[5];
    z5[0]=Freq[4];
    z6[0]=Freq[3];
    z7[0]=Freq[2];
    z8[0]=Freq[1];

    FrequChar[0]='A';
    SetFreq((char*)&FrequCharA[0]); // write back frequency
    SendFreq();
    if(SelBandMode==0){
      zeile=bandmode[nr];
    }
    else{
      zeile=memomode[nr];
    }
    ModeNr = zeile;
    SGUI_ListboxSetAktivItemNr(lb1,zeile);
    ModeTxt=SGUI_ListboxGetItem(lb1,zeile);
    SGUI_ButtonSetText(btn11,ModeTxt);
    UB_Uart_SendByte(COM6,6); // Send Mode control to Red Pitaya
    UB_Uart_SendByte(COM6,zeile);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8

    if(SelBandMode==0)
      BWNr=bandwidth[nr];
    else
      BWNr=memowidth[nr];

    if(zeile<=1){

      SGUI_ListboxSetAktivItemNr(lb12, BWNr);
      BWTxt=SGUI_ListboxGetItem(lb12,BWNr);
    }
    else if(zeile<=3){

      SGUI_ListboxSetAktivItemNr(lb2, BWNr);
      BWTxt=SGUI_ListboxGetItem(lb2,BWNr);
    }

    else{

      SGUI_ListboxSetAktivItemNr(lb22, BWNr);
      BWTxt=SGUI_ListboxGetItem(lb22,BWNr);
    }

    SGUI_ButtonSetText(btn12,BWTxt); // aktuelle Bandbreite anzeigen
    UB_Uart_SendByte(COM6,5); // Send filter bandwidth control to Red Pitaya
    UB_Uart_SendByte(COM6,9-BWNr); //'9' ... '0'
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0);
    UB_Uart_SendByte(COM6,0); // instead of CRC8
    //SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}



void SetBoxes(){ //SelBandMode 0 = Band   1 = Memo
  int8_t nr,i,k,m;
  char frq[12];

  nr=SGUI_ListboxGetItemCnt(lb191);
  if(nr>0){
    for(i=nr;i>=0;i--)
      SGUI_ListboxDeleteItem(lb191,i);
  }
  if(SelBandMode==0){
    for(i=0;i<8;i++){
      SGUI_ListboxAddItem(lb191,&band[i+BandPtr][0]);
    }
  }
  else{
    for(i=0;i<8;i++){
      SGUI_ListboxAddItem(lb191,&memo[i+MemoPtr][0]);
    }
  }

  nr=SGUI_ListboxGetItemCnt(lb192);
  if(nr>0){
    for(i=nr;i>=0;i--)
      SGUI_ListboxDeleteItem(lb192,i);
  }
  if(SelBandMode==0){

  for(i=0;i<8;i++){
      sprintf(frq,"%9.3f",(float)bandfreq[i+BandPtr]/1000);
      SGUI_ListboxAddItem(lb192,frq);
      }
  }
  else{
    for(i=0;i<8;i++){
      sprintf(frq,"%9.3f",(float)memofreq[i+MemoPtr]/1000);
      SGUI_ListboxAddItem(lb192,frq);
    }
  }
  nr=SGUI_ListboxGetItemCnt(lb193);
  if(nr>0){
    for(i=nr;i>=0;i--)
      SGUI_ListboxDeleteItem(lb193,i);
  }
  if(SelBandMode==0){
    for(i=0;i<8;i++){
        k=bandmode[i+BandPtr];
        SGUI_ListboxAddItem(lb193,&ModeTable[k][0]);
        }
  }
  else{
    for(i=0;i<8;i++){
      k=memomode[i+MemoPtr];
      SGUI_ListboxAddItem(lb193,&ModeTable[k][0]);
    }
  }

  nr=SGUI_ListboxGetItemCnt(lb194);
  if(nr>0){
    for(i=nr;i>=0;i--)
      SGUI_ListboxDeleteItem(lb194,i);
  }
  if(SelBandMode==0){
    for(i=0;i<8;i++){
      k=bandmode[i+BandPtr];
      m=bandwidth[i+BandPtr];
      if(k<2)
        SGUI_ListboxAddItem(lb194,&BWCW[m][0]);
      else if (k>3)
        SGUI_ListboxAddItem(lb194,&BWAM[m][0]);
      else
        SGUI_ListboxAddItem(lb194,&BWSSB[m][0]);
      }
  }
  else{
    for(i=0;i<8;i++){
      k=memomode[i+MemoPtr];
      m=memowidth[i+MemoPtr];
      if(k<2)
        SGUI_ListboxAddItem(lb194,&BWCW[m][0]);
      else if (k>3)
        SGUI_ListboxAddItem(lb194,&BWAM[m][0]);
      else
        SGUI_ListboxAddItem(lb194,&BWSSB[m][0]);
    }
  }

}

void SelBand(bool aktiv){
  if(aktiv==true) {
    SelBandMode=0;
    SetBoxes();
    SGUI_ButtonSetAktiv(btn192,false);
    SGUI_ButtonSetAktiv(btn191,true);
  }
}

void SelMemo(bool aktiv){
  if(aktiv==true) {
    SelBandMode=1;
    SetBoxes();
    SGUI_ButtonSetAktiv(btn191,false);
    SGUI_ButtonSetAktiv(btn192,true);
  }
}

void btn_up(bool aktiv){
  if(aktiv==true){
    if(SelBandMode==0){
      if(BandPtr!=8) BandPtr+=8;
    }
    else{
      if(MemoPtr!=8) MemoPtr+=8;
    }
    SetBoxes();
  }
}

void btn_dn(bool aktiv){
  if(aktiv==true){
    if(SelBandMode==0){
      if(BandPtr!=0) BandPtr-=8;
    }
    else{
      if(MemoPtr!=0) MemoPtr-=8;
    }
    SetBoxes();
  }
}

void create_ChildWindow_19(void) { // Menue Band/Memory ************************************************************************************************

  ptr19=SGUI_WindowCreateChild(19,0,10,476,262); // Child-Window (Nr=19)
  SGUI_WindowSetColor(ptr19,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(20,6);
  SGUI_TextSetDefFont(&Arial_11x18);
  delay10ms(40);
  SGUI_TextCreateString("Select "); // Beschriftung
  btn191=SGUI_ButtonCreate(110,4,70,30); // band-button
  SGUI_ButtonSetText(btn191,"Band");
  SGUI_ButtonSetMode(btn191,SBUTTON_PUSHPULL);
  SGUI_ButtonSetHandler(btn191,SelBand);  // "Band"
  btn192=SGUI_ButtonCreate(184,4,70,30); // memo-button
  SGUI_ButtonSetMode(btn192,SBUTTON_PUSHPULL);
  SGUI_ButtonSetText(btn192,"Memory");
  SGUI_ButtonSetHandler(btn192,SelMemo);  // "Memory"
  if(SelBandMode==1){
    SGUI_ButtonSetAktiv(btn191,false);
    SGUI_ButtonSetAktiv(btn192,true);
  }
  else{
    SGUI_ButtonSetAktiv(btn191,true);
    SGUI_ButtonSetAktiv(btn192,false);
  }
  lb191=SGUI_ListboxCreate(5,45,86,216); // Band lb191
  SGUI_ListboxSetStyle(lb191, STYLE_FLAT);
  SGUI_ListboxSetFont(lb191,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb191,false);
  SGUI_ListboxSetHandler(lb191,BandSelectRdy);

  lb192=SGUI_ListboxCreate(93,45,154,216); // freq lb192
  SGUI_ListboxSetStyle(lb192, STYLE_FLAT);
  SGUI_ListboxSetFont(lb192,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb192,false);
  SGUI_ListboxSetHandler(lb192,FreqSelectRdy);

  lb193=SGUI_ListboxCreate(248,45,68,216); // Mode lb193
  SGUI_ListboxSetStyle(lb193, STYLE_FLAT);
  SGUI_ListboxSetFont(lb193,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb193,false);

  lb194=SGUI_ListboxCreate(318,45,68,216); // Mode lb193
  SGUI_ListboxSetStyle(lb194, STYLE_FLAT);
  SGUI_ListboxSetFont(lb194,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb194,false);

  SetBoxes();
  SGUI_ListboxSetAktivItemNr(lb194,5);
  SGUI_ListboxSetAktivItemNr(lb193,5);
  SGUI_ListboxSetAktivItemNr(lb192,5);
  SGUI_ListboxSetAktivItemNr(lb191,5);
  SGUI_ListboxSetHandler(lb191,BandSelectRdy);

  btn=SGUI_ButtonCreate(400,12,70,40); // back-button (back to main menue)
  SGUI_ButtonSetText(btn,"Back");
  SGUI_ButtonSetHandler(btn,btn_fkt);

  btn193=SGUI_ButtonCreate(400,62,70,40); // Store-button
  SGUI_ButtonSetText(btn193,"Store");
  SGUI_ButtonSetHandler(btn193,btn_Store);

  btn194=SGUI_ButtonCreate(400,112,70,40); // Select-button
  SGUI_ButtonSetText(btn194,"Select");
  SGUI_ButtonSetHandler(btn194,btn_Select);


  btn195=SGUI_ButtonCreate(400,164,70,43); // Up-button
  SGUI_ButtonSetText(btn195,"Up");
  SGUI_ButtonSetHandler(btn195,btn_up);
  btn196=SGUI_ButtonCreate(400,208,70,43); // Down-button
  SGUI_ButtonSetText(btn196,"Down");
  SGUI_ButtonSetHandler(btn196,btn_dn);

}

//--------------------------------------------------------------
