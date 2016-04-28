//--------------------------------------------------------------
// File     : main.c
// Datum    : 25.02.2016
// Version  : 0.1
// Autor    : Wolfgang Kiefer
// EMail    : woki@onlinehome.de
// Web      : www.wkiefer.de
// CPU      : STM32F746
// Board    : STM32F746-Discovery-Board
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Lib      : stm32f746g-disco_hal_lib (static)
// Funktion : Hauptprogramm für RedPitaya Interface (Embedded Transceiver from Pavel Demin)
// USART    : COM6
// RX- Pin  : PC7 ==> CN4 D0
// TX- Pin  : PC6 ==> CN4 D1
//--------------------------------------------------------------


#include "stm32_ub_system.h"
#include "stm32_ub_sgui.h"
#include "stm32_ub_uart.h"

#define FFTHeight 90

void create_MainWindow_01(void);
void create_ChildWindow_11(void);// Mode selection
void create_ChildWindow_12SSB(void);// Bandwidth selection
void create_ChildWindow_12CW(void);// Bandwidth selection
void create_ChildWindow_12AM(void);// Bandwidth selection
void create_ChildWindow_13(void);// AGC selection
void create_ChildWindow_14(void);// Frequency input/actualisation
void create_ChildWindow_15(void);// Settings

// Globale Pointer
SWINDOW_t *MAIN;
SLISTBOX_t *lb,*lb1,*lb2,*lb12,*lb22,*lb3,*lb13;  // pointer auf Listbox
SBUTTON_t *btn, *btn9,*btn10,*btn11,*btn12,*btn13;  // pointer auf buttons
SGRAPH_t *graph;
SDROPDOWN_t *dd1;
SRBTN_t *rb1,*rb2,*rb3,*rb161,*rb162,*rb163;
SSLIDER_t *bright, *mic;
SWINDOW_t *ptr11,*ptr12,*ptr12,*ptr13,*ptr14,*ptr15,*ptr16,*ptr17,*ptr_16;

// Globale Steuervariablen
uint8_t ModeNr=2;// 0 = CWL, 1=CWU, 2=LSB, 3=USB, 4=AM, 5=FM, 6=Digi
uint8_t BWNr;
uint8_t BWNrSSB=4;
uint8_t BWNrCW=5;
uint8_t BWNrAM=5;
uint8_t split=0;// 0 RXfrequ =TXfrequ.
uint8_t AGCmode;
uint32_t frequencyA;
uint32_t frequencyB;
uint32_t helpFreq;
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
SBUTTON_t *oldptr;//Zeiger für Frequenzeingabe (Ziffernposition)
char oldpos;// numerischer Zeiger für Frequenzeingabe (Ziffernposition)
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
uint8_t *pointer, *oldpointer=NULL;
char new=1;

char *ModeTxt, *BWTxt, *AGCtxt;
char f1[7];

uint8_t StopCount=9;// Control for Backlight Dim (1..18)
uint8_t  count=0;
uint8_t VolumSet=5;// Control for volume (0 .. 9)
char  VolChar[3]="50";
SSLIDER_t *volume;
uint8_t MicVolumSet=5;// Control for volume (0 .. 9)
char  MicVolChar[3]="50";
SSLIDER_t *Micvolume;
uint8_t Out_Gain =5;
char Mic_txt[9]="00000000";

SGAUGE_t *SValGauge;
uint16_t S_Value = 500;
uint16_t S_max = 100;
uint8_t  max;
char  dBm_text[8]="- 53dBm";
char S_Text[6]="S9+40";
uint16_t count20000=0;// for 100 ms
uint16_t count200000=0;// for 1 s
SLABEL_t *label1, *label2;// S- Meter Texte

//BandWidth values from Pavel Demin:
//CW: 25, 50, 100, 250, 400, 500, 600, 750, 800, 1.0k
//SSB: 1.0k, 1.8k, 2.1k, 2.4k, 2.7k, 2.9k, 3.3k, 3.8k, 4.4k, 5.0k
//AM: 5.0k, 6.0k, 7.0k, 8.0k, 9.0k, 10k, 12k, 16k, 18k, 20k

uint32_t MakeFreq(char* freqText){//"A 07.200.000" or "B 07.200.000"
  uint32_t help=0;
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

void MakeFrString(char* target, uint32_t frq, uint8_t length){// result in char[] f1
  char i;
uint32_t freq;

  freq=frq/1000;
  target[length]=0;
  for(i=length;i>0;i--){
    if(i==3) target[i-1]='.';//decimal point
    else {
      target[i-1]=(freq%10)+48;//"ASCII 0"
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
  helpFreq=MakeFreq((char*)&FrequCharA[0])-36000;
  for(i=0;i<8;i++){
    MakeFrString((char*)&f1[0],helpFreq,6);
    SGUI_TextSetCursor(10+60*i,116);

    if(i==3){
      SGUI_TextSetDefColor(RGB_COL_YELLOW,RGB_COL_BLACK);
      SGUI_TextPrintString((char*)&f1[0]);
      SGUI_TextSetDefColor(RGB_COL_BLACK,SGUI_WINCOL);
    }
    else SGUI_TextPrintString((char*)&f1[0]);
    helpFreq+=12000;
  }
}


void ShowFFT(void){
uint16_t i;
uint8_t calc;
  DrawScale();
  for(i=0;i<480;i++){
    calc=(uint8_t)((pointer[i+2] * FFTHeight) / 256);
    SGUI_GraphWriteColumn(graph,1,i,calc);// draw one column
  }
}

void ShowSignalStrength(void){
uint8_t i, SNr;
uint16_t S, dBmPlus;
int16_t  S_Wert;

  for(i=1;i<4;i++){      //  0    => 0 dBm
    if(pointer[i]==0x0A) {//  1000 => -100 dBm
      pointer[i]=0;
      break;
    }
  }
  S_Value=atoi(pointer[1]);// attention: S-Value is negative!!

  if(S_max>S_Value) {
    S_max=S_Value;//  quickly up
    max=0;
  }
  else max--;
  if(max>9){
    max=0;
    S_max+=5;//       slow down    Test- value 5 ***

  }
  if(count20000>20000){// timer 100 millisec
    count20000=0;
    S_Wert=1270-S_max;
    if(S_Wert<0) S_Wert=0;
    SGUI_GaugeSetValue(SValGauge,S_Wert);
    count200000++;
    if(count200000>10){// timer 1 sec
      count200000=0;
      S=S_max/10;
      for(i=3;i>0;i--){
          dBm_text[i]=(S%10)+48;//"ASCII 0"
          S=S/10;
      }
      if(dBm_text[1]=='0')dBm_text[1]=' ';
      SGUI_LabelSetText(label2,dBm_text);// "-  53dBm"   update String
      SNr=9;
      S_Text[3]=' ';
      S_Text[4]=' ';
      S_Text[2]=' ';
      while(S_Wert<=730) {
        SNr--;
        S_Wert+=6;
      }
      if(SNr==9){
        dBmPlus=(730-S_Wert)/10;
        S_Text[4]=(dBmPlus%10)+48;
        S_Text[3]=dBmPlus/10+48;
        S_Text[2]='+';
      }
      S_Text[1]=SNr+48;//
      SGUI_LabelSetText(label1,S_Text);// "S9+20"
    }
  }

}

void UB_TIMER2_ISR_CallBack(void){// von TIM2 aufgerufen (Interrupt 200 kHz)
  if(++count == StopCount)
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, 0);
  if(count ==20) {
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, 1);
    count=0;
  }
}

int main(void)
{
  // init vom System
  UB_System_Init();

  // init der SGUI
  SGUI_Init();// Graphic User Interface
  UB_Uart_Init();// UART
  UB_TIMER2_Init(499,39);//Frequenz 5 kHz - for LCD Backlight dimming
  UB_TIMER2_Start();

  FrequCharA[12]=FrequCharB[12]=FrequChar[12]=0;
  // alle Windows erzeugen
  create_MainWindow_01();
//  create_ChildWindow_10();
  create_ChildWindow_11();
  create_ChildWindow_12SSB();
  create_ChildWindow_12CW();
  create_ChildWindow_12AM();
  create_ChildWindow_13();
  create_ChildWindow_14();
  create_ChildWindow_15();
  //create_ChildWindow_16();

  // erstes Window anzeigen
  SGUI_WindowShow(1);
  while(1)
  {
    SGUI_Do(); // SGUI bearbeiten
    pointer=UB_Uart_ReceiveUART6();
    if(pointer != NULL)
      //pointer=(char*) &TestArray[0];
      if(pointer != oldpointer){
        oldpointer=pointer;
        if(pointer[0]=='D')
           ShowFFT();
        if(pointer[0]=='S')
            ShowSignalStrength();
      }
  }

}


//--------------------------------------------------------------
// Funktions-Handler der Buttons
//--------------------------------------------------------------
void btn_fkt(bool aktiv) {
  if(aktiv==false) {
   // SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
    SGUI_WindowShow(1);// main-window anzeigen
    oldpointer=NULL;
    new=1;

  }
}

void Brightness(void){
  StopCount=SGUI_SliderGetValue(bright);
}

void MicVol(void){
  MicVolumSet=SGUI_SliderGetValue(mic);// ((Send microphone volume control to Red Pitaya))
  UB_Uart_SendByte(COM6,'R');
  MicVolChar[0]=(char)('0' + MicVolumSet);
  //MicVolChar[1]=(char)('0' + MicVolumSet % 10);
  UB_Uart_SendByte(COM6,MicVolChar[0]);
  //UB_Uart_SendByte(COM6,MicVolChar[1]);
  UB_Uart_SendByte(COM6,0x0A);//     LF*/
}

void Volume(void){
  VolumSet=SGUI_SliderGetValue(volume);// ((Send volume control to Red Pitaya))
  UB_Uart_SendByte(COM6,'P');
  VolChar[0]=(char)('0' + VolumSet);
    //VolChar[1]=(char)('0' + VolumSet % 10);
  UB_Uart_SendByte(COM6,VolChar[0]);
    //UB_Uart_SendByte(COM6,VolChar[1]);
  UB_Uart_SendByte(COM6,0x0A);//     LF*/
}

void ModeSelectRdy(uint16_t zeile){
  uint8_t nr;// Mode Nr.
  char txt[3]="M0";

  ModeNr = zeile;
  ModeTxt=SGUI_ListboxGetItem(lb1,zeile);
  SGUI_ButtonSetText(btn11,ModeTxt);
  nr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb1);
  txt[1]=nr+48;//                + "0"
  UB_Uart_SendString(COM6,&txt[0],LF);// Send Mode control to Red Pitaya

  if(ModeNr<=1){
    BWNr=BWNrCW;
    SGUI_ListboxSetAktivItemNr(lb12, BWNrCW);
    BWTxt=SGUI_ListboxGetItem(lb12,BWNrCW);
  }
  else if(ModeNr<=3){
    BWNr=BWNrSSB;
    SGUI_ListboxSetAktivItemNr(lb2, BWNrSSB);
    BWTxt=SGUI_ListboxGetItem(lb2,BWNrSSB);

  }

  else{
    BWNr=BWNrAM;
    SGUI_ListboxSetAktivItemNr(lb22, BWNrAM);
    BWTxt=SGUI_ListboxGetItem(lb22,BWNrAM);
  }
  SGUI_ButtonSetText(btn12,BWTxt);// aktuelle Bandbreite anzeigen
  UB_Uart_SendByte(COM6,'F');    // Send bandwidth control to Red Pitaya
  UB_Uart_SendByte(COM6,'9'-BWNr);//'9' ... '0'
  UB_Uart_SendByte(COM6,0x0A);//     LF

}

void BWSelect(bool aktiv) {
  if(aktiv==true){
    if(ModeNr<=1){
      SGUI_WindowShow(16);// CW
      SGUI_ListboxSetAktivItemNr(lb12, BWNrCW);
    }
    else if (ModeNr<=3){
      SGUI_WindowShow(12);// SSB
      SGUI_ListboxSetAktivItemNr(lb2, BWNrSSB );
    }
    else {
      SGUI_WindowShow(17);// AM,FM,Digi
      SGUI_ListboxSetAktivItemNr(lb22, BWNrAM);
    }
  }
  else{
    SGUI_WindowShow(1); // main-window anzeigen
    oldpointer=NULL;
  }
}

void BWSelectRdy(uint16_t zeile){


  BWNr = zeile;
  if(ModeNr <=1){//  CW
    BWTxt=SGUI_ListboxGetItem(lb12,zeile);
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb12);
  }
  else if(ModeNr<=3){// SSB
    BWTxt=SGUI_ListboxGetItem(lb2,zeile);
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb2);
  }
  else{
    BWTxt=SGUI_ListboxGetItem(lb22,zeile);// AM, FM, Digi
    BWNr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb22);
  }
  SGUI_ButtonSetText(btn12,BWTxt);// aktuelle Bandbreite anzeigen
  UB_Uart_SendByte(COM6,'F');    // Send bandwidth control to Red Pitaya
  UB_Uart_SendByte(COM6,'9'-BWNr);//'9' ... '0'
  UB_Uart_SendByte(COM6,0x0A);//     LF
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

void AGCSelectRdy(uint16_t zeile){
  uint8_t nr;
  char txt[3]="G0";

  AGCmode = zeile;
  AGCtxt=SGUI_ListboxGetItem(lb13,zeile);
  SGUI_ButtonSetText(btn13,AGCtxt);
  nr=(uint8_t)SGUI_ListboxGetAktivItemNr(lb13);
  txt[1]=nr+48;//                + "0"
  UB_Uart_SendString(COM6,&txt[0],LF);//        Send AGC control to Red Pitaya
}

void PreampRdy(){
  if(SGUI_RadioButtonIsAktiv(rb1))
    UB_Uart_SendString(COM6,"P0",LF);
  else if(SGUI_RadioButtonIsAktiv(rb2))
    UB_Uart_SendString(COM6,"P9",LF);
  else UB_Uart_SendString(COM6,"P1",LF);

}

void Settings(bool aktiv) {
  if(aktiv==true){
    SGUI_WindowShow(15);
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
    UB_Uart_SendByte(COM6,'1');// Send frequency control to Red Pitaya
    for(i=2;i<12;i++){
      if(FrequCharA[i] != '.' ) UB_Uart_SendByte(COM6,FrequCharA[i]);
    }
    UB_Uart_SendByte(COM6,0x0A);//     LF
    UB_Uart_SendByte(COM6,'2');// Send frequency control to Red Pitaya
    for(i=2;i<12;i++){
      if(FrequCharA[i] != '.' ) UB_Uart_SendByte(COM6,FrequCharA[i]);
    }
    UB_Uart_SendByte(COM6,0x0A);//     LF
  }

  else{// RXfrequ=FrequA   TXfreq=FreqB
    UB_Uart_SendByte(COM6,'1');// Send frequency control to Red Pitaya
    for(i=2;i<12;i++){
      if(FrequCharA[i] != '.' ) UB_Uart_SendByte(COM6,FrequCharA[i]);
    }
    UB_Uart_SendByte(COM6,0x0A);//     LF
    UB_Uart_SendByte(COM6,'2');// Send frequency control to Red Pitaya
    for(i=2;i<12;i++){
      if(FrequCharB[i] != '.' ) UB_Uart_SendByte(COM6,FrequCharB[i]);
    }
    UB_Uart_SendByte(COM6,0x0A);//     LF
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
    SetFreq((char*)&FrequCharA[0]);// Frequenz zurückschreiben
  }
  else {
    SetFreq((char*)&FrequCharB[0]);
  }
  SendFreq();
  //create_MainWindow_01();
  SGUI_WindowShow(1);
  oldpointer=NULL;// FFT anzeigen
  new=1;
  ShowFFT();
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

void PrepFreqBtns(void){// Prepare Frequency Buttons
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
void frequA(bool aktiv){// Button FrequA pressed
  if(aktiv==true){
    strncpy((char*)&FrequChar[0],(char*)&FrequCharA[0],12);

    PrepFreqBtns();
    SGUI_WindowShow(14);
    SetFreqBtns0();
    SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE);// Cursor setzen
  }
}

void frequB(bool aktiv){// Button FrequB pressed
  if(aktiv==true){
    strncpy((char*)&FrequChar[0],(char*)&FrequCharB[0],12);
    PrepFreqBtns();
    SGUI_WindowShow(14);
    SetFreqBtns0();
    SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE);// Cursor setzen
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

void Split(bool aktiv){//toggle split flag
  if(aktiv==true){
    if(split==0) split=1;
    else split=0;
    SendFreq();
  }
}

void PTT(bool aktiv){// +++++ PTT to Red Pitaya +++++
  if(aktiv==true){
    UB_Uart_SendString(COM6,"T1",LF);
  }
  else UB_Uart_SendString(COM6,"T0",LF);
}
//--------------------------------------------------------------
void create_MainWindow_01(void) {
  SLABEL_t *label;
  SBUTTON_t *btn2, *btn4, *btn5,*btn5a, *btn6,*btn7,*btn8,*btnA,*btnB;// , *btn3
  SGAUGE_t *gauge;

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


  btn4=SGUI_ButtonCreate(2,0,41,22); // button
  SGUI_ButtonSetText(btn4,"Run");

  btn5=SGUI_ButtonCreate(48,0,90,22); // button

  SGUI_ButtonSetText(btn5,"Settings");
  SGUI_ButtonSetHandler(btn5,Settings);

  btn5a=SGUI_ButtonCreate(142,0,90,22); // button
  SGUI_ButtonSetText(btn5a,"Rec/Play");
 // SGUI_ButtonSetHandler(btn5a,Audio);

  btn6=SGUI_ButtonCreate(240,0,72,22); // button

  SGUI_ButtonSetText(btn6,"Freece");

  btn7=SGUI_ButtonCreate(316,0,90,22); // button

  SGUI_ButtonSetText(btn7,"Band/Mem");

  btn8=SGUI_ButtonCreate(414,0,60,22); // button
  SGUI_ButtonSetColor(btn8, RGB_COL_GREEN, RGB_COL_RED);
  SGUI_ButtonSetText(btn8,"Tune");
  SGUI_ButtonSetMode(btn8,SBUTTON_PUSHPULL);

  graph=SGUI_GraphCreate(0,23,480,FFTHeight); // graph

  SGUI_GraphSetStyle(graph,STYLE_FLAT);
  SGUI_GraphSetFrameSize(graph,0);
  SGUI_GraphCreateDataArray(graph,1,480);
  SGUI_GraphSetCHColor(graph,0,RGB_COL_BLUE);
  SGUI_GraphSetCHVisible(graph,0,true);
  ShowFFT();
  btn10=SGUI_ButtonCreate(280,134,168,28); // Frequency B
  SGUI_ButtonSetFont(btn10, &Arial_13x19);
  SGUI_ButtonSetStyle(btn10,STYLE_FLAT);
  SGUI_ButtonSetColor(btn10, 0xFA40, 0x4208);// Rot- Orange
  SGUI_ButtonSetText(btn10,(char*)FrequCharB);// "B 07.197.000"
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

  volume=SGUI_SliderCreate(176,165,200,30);// Volume
  SGUI_SliderSetColor(volume,RGB_COL_GREY,0x076CE);
  SGUI_SliderSetMinMax(volume,0,9);
  SGUI_SliderSetStep(volume,1);
  SGUI_SliderSetValue(volume,VolumSet);
  SGUI_SliderSetHandler(volume, Volume);

 // SGUI_TextSetCursor(180,200);
 // SGUI_TextCreateString("Hz");
    label=SGUI_LabelCreate(4,165,68,23); // S-Wert
    SGUI_LabelSetStyle(label, STYLE_FLAT);
    SGUI_LabelSetText(label,"S9+40");

    label=SGUI_LabelCreate(74,165,98,23); // Feldstärke
    SGUI_LabelSetStyle(label, STYLE_FLAT);
    SGUI_LabelSetText(label,"- 53dBm");

    gauge=SGUI_GaugeCreate(4,188,168,20); // S- Meter
    SGUI_GaugeSetStyle(gauge,STYLE_FLAT);
    SGUI_GaugeSetColor(gauge,0x076CE,RGB_COL_GREY);
    SGUI_GaugeSetValue(gauge,80);

  btn9=SGUI_ButtonCreate(4,214,168,28); // Frequency A
  SGUI_ButtonSetFont(btn9, &Arial_13x19);
  SGUI_ButtonSetStyle(btn9,STYLE_FLAT);
  SGUI_ButtonSetColor(btn9,RGB_COL_YELLOW , 0x4208);//
  SGUI_ButtonSetText(btn9, (char*)FrequCharA);// uint16_t i; dunkelgrau  0x630C   0x076CE lindgrün"A 59.123.456"
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
}


//--------------------------------------------------------------
void create_ChildWindow_11(void) {

  ptr11=SGUI_WindowCreateChild(11,0,30,200,240); // Child-Window (Nr=11)
  SGUI_WindowSetColor(ptr11,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,4);
  SGUI_TextCreateString("Enter mode");    // Beschriftung

  lb1=SGUI_ListboxCreate(4,40,130,180); // Mode
  SGUI_ListboxSetFont(lb1,&Arial_16x25);
  SGUI_ListboxSetStyle(lb1, STYLE_FLAT);
  SGUI_ListboxSetSliderVisible(lb1,true);
  SGUI_ListboxAddItem(lb1," CWL");
  SGUI_ListboxAddItem(lb1," CWU");
  SGUI_ListboxAddItem(lb1," LSB");
  SGUI_ListboxAddItem(lb1," USB");
  SGUI_ListboxAddItem(lb1," AM");
  SGUI_ListboxAddItem(lb1," FM");
  SGUI_ListboxAddItem(lb1,"Digi");
  SGUI_ListboxSetHandler(lb1,ModeSelectRdy);


  btn=SGUI_ButtonCreate(140,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_12SSB(void) {

  ptr12=SGUI_WindowCreateChild(12,20,0,460,270); // Child-Window (Nr=12)
  SGUI_WindowSetColor(ptr12,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb2=SGUI_ListboxCreate(150,10,140,258); // Bandwith SSB
  SGUI_ListboxSetStyle(lb2, STYLE_FLAT);
  SGUI_ListboxSetFont(lb2,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb2,true);
  SGUI_ListboxAddItem(lb2,"5000");
  SGUI_ListboxAddItem(lb2,"4400");
  SGUI_ListboxAddItem(lb2,"3800");
  SGUI_ListboxAddItem(lb2,"3300");
  SGUI_ListboxAddItem(lb2,"2900");
  SGUI_ListboxAddItem(lb2,"2700");
  SGUI_ListboxAddItem(lb2,"2400");
  SGUI_ListboxAddItem(lb2,"2100");
  SGUI_ListboxAddItem(lb2,"1800");
  SGUI_ListboxAddItem(lb2,"1000");
  SGUI_ListboxSetHandler(lb2,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

void create_ChildWindow_12CW(void) {

  ptr16=SGUI_WindowCreateChild(16,20,0,460,270); // Child-Window (Nr=16)
  SGUI_WindowSetColor(ptr16,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb12=SGUI_ListboxCreate(150,10,140,258); // Bandwith  CW lb12
  SGUI_ListboxSetStyle(lb12, STYLE_FLAT);
  SGUI_ListboxSetFont(lb12,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb12,true);
  SGUI_ListboxAddItem(lb12,"1000");
  SGUI_ListboxAddItem(lb12," 800");
  SGUI_ListboxAddItem(lb12," 750");
  SGUI_ListboxAddItem(lb12," 600");
  SGUI_ListboxAddItem(lb12," 500");
  SGUI_ListboxAddItem(lb12," 400");
  SGUI_ListboxAddItem(lb12," 250");
  SGUI_ListboxAddItem(lb12," 100");
  SGUI_ListboxAddItem(lb12,"  50");
  SGUI_ListboxAddItem(lb12,"  25");
  SGUI_ListboxSetHandler(lb12,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

void create_ChildWindow_12AM(void) {

  ptr17=SGUI_WindowCreateChild(17,20,0,460,270); // Child-Window (Nr=17)
  SGUI_WindowSetColor(ptr17,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(4,4);
  SGUI_TextCreateString("Enter BandWidth");    // Beschriftung

  lb22=SGUI_ListboxCreate(150,10,140,258); // Bandwith AM lb22
  SGUI_ListboxSetStyle(lb22, STYLE_FLAT);
  SGUI_ListboxSetFont(lb22,&Arial_16x25);
  SGUI_ListboxSetSliderVisible(lb22,true);
  SGUI_ListboxAddItem(lb22,"20 k");
  SGUI_ListboxAddItem(lb22,"18 k");
  SGUI_ListboxAddItem(lb22,"16 k");
  SGUI_ListboxAddItem(lb22,"12 k");
  SGUI_ListboxAddItem(lb22,"10 k");
  SGUI_ListboxAddItem(lb22," 9 k");
  SGUI_ListboxAddItem(lb22," 8 k");
  SGUI_ListboxAddItem(lb22," 7 k");
  SGUI_ListboxAddItem(lb22," 6 k");
  SGUI_ListboxAddItem(lb22," 5 k");
  SGUI_ListboxSetHandler(lb22,BWSelectRdy);

  btn=SGUI_ButtonCreate(320,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

/*CW: 25, 50, 100, 250, 400, 500, 600, 750, 800, 1.0k
SSB: 1.0k, 1.8k, 2.1k, 2.4k, 2.7k, 2.9k, 3.3k, 3.8k, 4.4k, 5.0k
AM: 5.0k, 6.0k, 7.0k, 8.0k, 9.0k, 10k, 12k, 16k, 18k, 20k*/

//--------------------------------------------------------------
void create_ChildWindow_13(void) {

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

btn=SGUI_ButtonCreate(180,4,50,50); // ok-button
SGUI_ButtonSetText(btn,"OK");
SGUI_ButtonSetHandler(btn,btn_fkt);
}

// Helper Frequenzeingabe

void SetColorBtn(SBUTTON_t* ptr, char n){
  SendFreq();                    // aktuelle Frequenz an RedPitaya senden
  SGUI_ButtonSetColor(ptr,RGB_COL_BLACK,0x076CE);// Cursor setzen (lindgrün)
  if(oldptr == ptr) return;
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,RGB_COL_GREY);// alten Cursor löschen
  oldptr=ptr;
  oldpos=n;
}

void FrequShift(uint32_t freqShift){
char Freq[9]="000000000";

  if(FrequChar[0]=='A') {
    frequencyA=MakeFreq(FrequChar);// make frequency as integer
    helpFreq=frequencyA+freqShift;
    if((0<=helpFreq)&&(helpFreq <=61440000)){
      frequencyA=  helpFreq;
      itoa(helpFreq+100000000,Freq,10);// to create leading Zeroes
      z1[0]=Freq[8];
      z2[0]=Freq[7];
      z3[0]=Freq[6];
      z4[0]=Freq[5];
      z5[0]=Freq[4];
      z6[0]=Freq[3];
      z7[0]=Freq[2];
      z8[0]=Freq[1];
      SetFreqBtns0();
      SetFreq((char*)&FrequCharA[0]);// write back frequency
    }
    else return;
  }
  else{
    frequencyB=MakeFreq(FrequChar);
    helpFreq=frequencyB+freqShift;
    if((0<=helpFreq)&&(helpFreq <=61440000)){
      frequencyB=  helpFreq;
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
      SetFreq((char*)&FrequCharB[0]);// Frequenz zurückschreiben
    }
  }
}



void fktup1(bool aktiv){// Einer
  if(aktiv==true) {

    if(z1[0]<'9'){
      z1[0]++;
      SGUI_ButtonSetText(bt1,z1);
    }
    FrequShift(1);
    SetColorBtn(bt1,1);
  }
}

void fktdn1(bool aktiv){
  if(aktiv==true) {

    if(z1[0]>'0'){
      z1[0]--;
      SGUI_ButtonSetText(bt1,z1);
    }
    FrequShift(-1);
    SetColorBtn(bt1,1);
  }
}
void fktbt1(bool aktiv){
  SetColorBtn(bt1,1);
}

void fktup2(bool aktiv){// Zehner
  if(aktiv==true) {

    if(z2[0]<'9'){
      z2[0]++;
      SGUI_ButtonSetText(bt2,z2);
    }
    FrequShift(10);
    SetColorBtn(bt2,2);
  }
}
void fktdn2(bool aktiv){
  if(aktiv==true) {

    if(z2[0]>'0'){
      z2[0]--;
      SGUI_ButtonSetText(bt2,z2);
    }
    FrequShift(-10);
    SetColorBtn(bt2,2);
  }
}
void fktbt2(bool aktiv){
  SetColorBtn(bt2,2);
}

void fktup3(bool aktiv){// Hunderter
  if(aktiv==true) {

    if(z3[0]<'9'){
      z3[0]++;
      SGUI_ButtonSetText(bt3,z3);
    }
    FrequShift(100);
    SetColorBtn(bt3,3);
  }
}
void fktdn3(bool aktiv){
  if(aktiv==true) {

    if(z3[0]>'0'){
      z3[0]--;
      SGUI_ButtonSetText(bt3,z3);
    }
    FrequShift(-100);
    SetColorBtn(bt3,3);
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
    FrequShift(1000);
    SetColorBtn(bt4,4);
  }
}
void fktdn4(bool aktiv){
  if(aktiv==true) {

    if(z4[0]>'0'){
      z4[0]--;
      SGUI_ButtonSetText(bt4,z4);
    }
    FrequShift(-1000);
    SetColorBtn(bt4,4);
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
    FrequShift(10000);
    SetColorBtn(bt5,5);
  }
}
void fktdn5(bool aktiv){
  if(aktiv==true) {

    if(z5[0]>'0'){
      z5[0]--;
      SGUI_ButtonSetText(bt5,z5);
    }
    FrequShift(-10000);
    SetColorBtn(bt5,5);
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
    FrequShift(100000);
    SetColorBtn(bt6,6);
  }
}
void fktdn6(bool aktiv){
  if(aktiv==true) {
    if(z6[0]>'0'){
      z6[0]--;
      SGUI_ButtonSetText(bt6,z6);
    }
    FrequShift(-100000);
    SetColorBtn(bt6,6);
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
    FrequShift(1000000);
    SetColorBtn(bt7,7);

  }
}
void fktdn7(bool aktiv){
  if(aktiv==true) {
    if(z7[0]>'0'){
      z7[0]--;
      SGUI_ButtonSetText(bt7,z7);
    }
    FrequShift(-1000000);
    SetColorBtn(bt7,7);
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
    FrequShift(10000000);
    SetColorBtn(bt8,8);
  }
}
void fktdn8(bool aktiv){
  if(aktiv==true) {
    if(z8[0]>'0'){
      z8[0]--;
      SGUI_ButtonSetText(bt8,z8);
    }
    FrequShift(-10000000);
    SetColorBtn(bt8,8);

  }
}
void fktbt8(bool aktiv){
  if(SGUI_ButtonIsAktiv(bt8)) return;
  SetColorBtn(bt8,8);
}

void Setptr(char* n){

  SGUI_ButtonSetText(oldptr,n);
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,RGB_COL_GREY);// alten Cursor löschen
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
  SGUI_ButtonSetColor(oldptr,RGB_COL_BLACK,0x076CE);// neuen Cursor setzen
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

//--------------------------------------------------------------
void create_ChildWindow_14(void) {// Frequenzeingabe

  SBUTTON_t *bt00,*bt01,*bt02,*bt03,*bt04,*bt05,*bt06,*bt07,*bt08,*bt09;

  oldptr=bt8;
  ptr14=SGUI_WindowCreateChild(14,0,30,420,240); // Child-Window (Nr=14)
  SGUI_WindowSetColor(ptr14,RGB_COL_BLACK,0x076CE);
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Enter frequency");    // Beschriftung

  up1=SGUI_ButtonCreate(290,50,36,40); // IntEdit1
  SGUI_ButtonSetStyle(up1,STYLE_FLAT);
  SGUI_ButtonSetText(up1,"+");
  SGUI_ButtonSetHandler(up1,fktup1);
  bt1=SGUI_ButtonCreate(290,90,36,40); //
  SGUI_ButtonSetStyle(bt1,STYLE_FLAT);
  SGUI_ButtonSetFont(bt1,&Arial_16x25);
  SGUI_ButtonSetHandler(bt1,fktbt1);
  SGUI_ButtonSetText(bt1,(char*)z1);
  dn1=SGUI_ButtonCreate(290,130,36,40); //
  SGUI_ButtonSetStyle(dn1,STYLE_FLAT);
  SGUI_ButtonSetText(dn1,"-");
  SGUI_ButtonSetHandler(dn1,fktdn1);

  up2=SGUI_ButtonCreate(250,50,36,40); // IntEdit2
  SGUI_ButtonSetStyle(up2,STYLE_FLAT);
  SGUI_ButtonSetText(up2,"+");
  SGUI_ButtonSetHandler(up2,fktup2);
  bt2=SGUI_ButtonCreate(250,90,36,40); //
  SGUI_ButtonSetStyle(bt2,STYLE_FLAT);
  SGUI_ButtonSetFont(bt2,&Arial_16x25);
  SGUI_ButtonSetHandler(bt2,fktbt2);
  SGUI_ButtonSetText(bt2,(char*)z2);
  dn2=SGUI_ButtonCreate(250,130,36,40); //
  SGUI_ButtonSetStyle(dn2,STYLE_FLAT);
  SGUI_ButtonSetText(dn2,"-");
  SGUI_ButtonSetHandler(dn2,fktdn2);

  up3=SGUI_ButtonCreate(210,50,36,40); // IntEdit3
  SGUI_ButtonSetStyle(up3,STYLE_FLAT);
  SGUI_ButtonSetText(up3,"+");
  SGUI_ButtonSetHandler(up3,fktup3);
  bt3=SGUI_ButtonCreate(210,90,36,40); //
  SGUI_ButtonSetStyle(bt3,STYLE_FLAT);
  SGUI_ButtonSetFont(bt3,&Arial_16x25);
  SGUI_ButtonSetHandler(bt3,fktbt3);
  SGUI_ButtonSetText(bt3,(char*)z3);
  dn3=SGUI_ButtonCreate(210,130,36,40); //
  SGUI_ButtonSetStyle(dn3,STYLE_FLAT);
  SGUI_ButtonSetText(dn3,"-");
  SGUI_ButtonSetHandler(dn3,fktdn3);

  up4=SGUI_ButtonCreate(168,50,36,40); // IntEdit4
  SGUI_ButtonSetStyle(up4,STYLE_FLAT);
  SGUI_ButtonSetText(up4,"+");
  SGUI_ButtonSetHandler(up4,fktup4);
  bt4=SGUI_ButtonCreate(168,90,36,40); //
  SGUI_ButtonSetStyle(bt4,STYLE_FLAT);
  SGUI_ButtonSetFont(bt4,&Arial_16x25);
  SGUI_ButtonSetHandler(bt4,fktbt4);
  SGUI_ButtonSetText(bt4,(char*)z4);
  dn4=SGUI_ButtonCreate(168,130,36,40); //
  SGUI_ButtonSetStyle(dn4,STYLE_FLAT);
  SGUI_ButtonSetText(dn4,"-");
  SGUI_ButtonSetHandler(dn4,fktdn4);

  up5=SGUI_ButtonCreate(128,50,36,40); // IntEdit5
  SGUI_ButtonSetStyle(up5,STYLE_FLAT);
  SGUI_ButtonSetText(up5,"+");
  SGUI_ButtonSetHandler(up5,fktup5);
  bt5=SGUI_ButtonCreate(128,90,36,40); //
  SGUI_ButtonSetStyle(bt5,STYLE_FLAT);
  SGUI_ButtonSetFont(bt5,&Arial_16x25);
  SGUI_ButtonSetHandler(bt5,fktbt5);
  SGUI_ButtonSetText(bt5,(char*)z5);
  dn5=SGUI_ButtonCreate(128,130,36,40); //
  SGUI_ButtonSetStyle(dn5,STYLE_FLAT);
  SGUI_ButtonSetText(dn5,"-");
  SGUI_ButtonSetHandler(dn5,fktdn5);

  up6=SGUI_ButtonCreate(88,50,36,40); // IntEdit6
  SGUI_ButtonSetStyle(up6,STYLE_FLAT);
  SGUI_ButtonSetText(up6,"+");
  SGUI_ButtonSetHandler(up6,fktup6);
  bt6=SGUI_ButtonCreate(88,90,36,40); //
  SGUI_ButtonSetStyle(bt6,STYLE_FLAT);
  SGUI_ButtonSetFont(bt6,&Arial_16x25);
  SGUI_ButtonSetHandler(bt6,fktbt6);
  SGUI_ButtonSetText(bt6,(char*)z6);
  dn6=SGUI_ButtonCreate(88,130,36,40); //
  SGUI_ButtonSetStyle(dn6,STYLE_FLAT);
  SGUI_ButtonSetText(dn6,"-");
  SGUI_ButtonSetHandler(dn6,fktdn6);

  up7=SGUI_ButtonCreate(44,50,36,40); // IntEdit7
  SGUI_ButtonSetStyle(up7,STYLE_FLAT);
  SGUI_ButtonSetText(up7,"+");
  SGUI_ButtonSetHandler(up7,fktup7);
  bt7=SGUI_ButtonCreate(44,90,36,40); //
  SGUI_ButtonSetStyle(bt7,STYLE_FLAT);
  SGUI_ButtonSetFont(bt7,&Arial_16x25);
  SGUI_ButtonSetHandler(bt7,fktbt7);
  SGUI_ButtonSetText(bt7,(char*)z7);
  dn7=SGUI_ButtonCreate(44,130,36,40); //
  SGUI_ButtonSetStyle(dn7,STYLE_FLAT);
  SGUI_ButtonSetText(dn7,"-");
  SGUI_ButtonSetHandler(dn7,fktdn7);

  up8=SGUI_ButtonCreate(4,50,36,40); // IntEdit8
  SGUI_ButtonSetStyle(up8,STYLE_FLAT);
  SGUI_ButtonSetText(up8,"+");
  SGUI_ButtonSetHandler(up8,fktup8);
  bt8=SGUI_ButtonCreate(4,90,36,40); //
  SGUI_ButtonSetStyle(bt8,STYLE_FLAT);
  SGUI_ButtonSetFont(bt8,&Arial_16x25);
  SGUI_ButtonSetHandler(bt8,fktbt8);
  SGUI_ButtonSetText(bt8,(char*)z8);
  dn8=SGUI_ButtonCreate(4,130,36,40); //
  SGUI_ButtonSetStyle(dn8,STYLE_FLAT);
  SGUI_ButtonSetText(dn8,"-");
  SGUI_ButtonSetHandler(dn8,fktdn8);

  // Zifferntasten::

  bt00=SGUI_ButtonCreate(338,20,36,40); //
  SGUI_ButtonSetStyle(bt00,STYLE_FLAT);
  SGUI_ButtonSetFont(bt00,&Arial_16x25);
  SGUI_ButtonSetHandler(bt00,fkt00a);
  SGUI_ButtonSetText(bt00,"0");

  bt01=SGUI_ButtonCreate(374,20,36,40); //
  SGUI_ButtonSetStyle(bt01,STYLE_FLAT);
  SGUI_ButtonSetFont(bt01,&Arial_16x25);
  SGUI_ButtonSetHandler(bt01,fkt01);
  SGUI_ButtonSetText(bt01,"1");

  bt02=SGUI_ButtonCreate(338,60,36,40); //
  SGUI_ButtonSetStyle(bt02,STYLE_FLAT);
  SGUI_ButtonSetFont(bt02,&Arial_16x25);
  SGUI_ButtonSetHandler(bt02,fkt02);
  SGUI_ButtonSetText(bt02,"2");

  bt03=SGUI_ButtonCreate(374,60,36,40); //
  SGUI_ButtonSetStyle(bt03,STYLE_FLAT);
  SGUI_ButtonSetFont(bt03,&Arial_16x25);
  SGUI_ButtonSetHandler(bt03,fkt03);
  SGUI_ButtonSetText(bt03,"3");

  bt04=SGUI_ButtonCreate(338,100,36,40); //
  SGUI_ButtonSetStyle(bt04,STYLE_FLAT);
  SGUI_ButtonSetFont(bt04,&Arial_16x25);
  SGUI_ButtonSetHandler(bt04,fkt04);
  SGUI_ButtonSetText(bt04,"4");

  bt05=SGUI_ButtonCreate(374,100,36,40); //
  SGUI_ButtonSetStyle(bt05,STYLE_FLAT);
  SGUI_ButtonSetFont(bt05,&Arial_16x25);
  SGUI_ButtonSetHandler(bt05,fkt05);
  SGUI_ButtonSetText(bt05,"5");

  bt06=SGUI_ButtonCreate(338,140,36,40); //
  SGUI_ButtonSetStyle(bt06,STYLE_FLAT);
  SGUI_ButtonSetFont(bt06,&Arial_16x25);
  SGUI_ButtonSetHandler(bt06,fkt06);
  SGUI_ButtonSetText(bt06,"6");

  bt07=SGUI_ButtonCreate(374,140,36,40); //
  SGUI_ButtonSetStyle(bt07,STYLE_FLAT);
  SGUI_ButtonSetFont(bt07,&Arial_16x25);
  SGUI_ButtonSetHandler(bt07,fkt07);
  SGUI_ButtonSetText(bt07,"7");

  bt08=SGUI_ButtonCreate(338,180,36,40); //
  SGUI_ButtonSetStyle(bt08,STYLE_FLAT);
  SGUI_ButtonSetFont(bt08,&Arial_16x25);
  SGUI_ButtonSetHandler(bt08,fkt08);
  SGUI_ButtonSetText(bt08,"8");

  bt09=SGUI_ButtonCreate(374,180,36,40); //
  SGUI_ButtonSetStyle(bt09,STYLE_FLAT);
  SGUI_ButtonSetFont(bt09,&Arial_16x25);
  SGUI_ButtonSetHandler(bt09,fkt09);
  SGUI_ButtonSetText(bt09,"9");

  SGUI_ButtonSetColor(bt8,RGB_COL_BLACK,0x076CE);// Cursor setzen



  btn=SGUI_ButtonCreate(210,4,60,40); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_Freqfkt);
}
//----------------------------------------------------- Settings ---------
void create_ChildWindow_15(void) {

  ptr15=SGUI_WindowCreateChild(15,0,10,400,260); // Child-Window (Nr=15)
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

  bright=SGUI_SliderCreate(10,150,200,30);// Brigthness
  SGUI_SliderSetColor(bright,RGB_COL_GREY,RGB_COL_BLUE);
  SGUI_SliderSetStep(bright,1);
  SGUI_SliderSetValue(bright,StopCount);
  SGUI_SliderSetMinMax(bright,1,18);
  SGUI_SliderSetHandler(bright,Brightness);

  SGUI_TextSetCursor(10,195);
  SGUI_TextCreateString("Microphone Volume");

  mic=SGUI_SliderCreate(10,215,220,30);// Microphone Volume
  SGUI_SliderSetColor(mic,RGB_COL_GREY,RGB_COL_BLUE);
  SGUI_SliderSetStep(mic,1);
  SGUI_SliderSetValue(mic,MicVolumSet);
  SGUI_SliderSetMinMax(mic,0,9);
  SGUI_SliderSetHandler(mic,MicVol);

  btn=SGUI_ButtonCreate(260,4,50,50); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}

//--------------------------------------------------------------
