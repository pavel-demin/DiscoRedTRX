//--------------------------------------------------------------
// File     : main.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// Board    : STM32F746-Discovery-Board
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : CubeHAL
// Funktion : Hauptprogramm
//--------------------------------------------------------------


#include "stm32_ub_system.h"
#include "stm32_ub_sgui.h"

void create_MainWindow_01(void);
void create_ChildWindow_10(void);
void create_ChildWindow_11(void);
void create_ChildWindow_12(void);
void create_ChildWindow_13(void);
void create_ChildWindow_14(void);
void create_ChildWindow_15(void);
void create_ChildWindow_16(void);
void create_ChildWindow_17(void);
void create_ChildWindow_18(void);
void create_ChildWindow_19(void);
void create_ChildWindow_20(void);
void create_ChildWindow_21(void);
void create_ChildWindow_22(void);
void create_ChildWindow_23(void);
void create_ChildWindow_24(void);

// Globale Pointer
SLISTBOX_t *lb;  // pointer auf Listbox
SBUTTON_t *btn;  // pointer auf button


int main(void)
{
  // init vom System
  UB_System_Init();

  // init der SGUI
  SGUI_Init();

  // alle Windows erzeugen
  create_MainWindow_01();
  create_ChildWindow_10();
  create_ChildWindow_11();
  create_ChildWindow_12();
  create_ChildWindow_13();
  create_ChildWindow_14();
  create_ChildWindow_15();
  create_ChildWindow_16();
  create_ChildWindow_17();
  create_ChildWindow_18();
  create_ChildWindow_19();
  create_ChildWindow_20();
  create_ChildWindow_21();
  create_ChildWindow_22();
  create_ChildWindow_23();
  create_ChildWindow_24();

  // erstes Window anzeigen
  SGUI_WindowShow(1);

  while(1)
  {
    SGUI_Do(); // SGUI bearbeiten
  }

}

//--------------------------------------------------------------
// Funktions-Handler für Listbox
//--------------------------------------------------------------
void lb_fkt(uint16_t aktiv_nr) {
  if(aktiv_nr==0) SGUI_WindowShow(10);
  if(aktiv_nr==1) SGUI_WindowShow(11);
  if(aktiv_nr==2) SGUI_WindowShow(12);
  if(aktiv_nr==3) SGUI_WindowShow(13);
  if(aktiv_nr==4) SGUI_WindowShow(14);
  if(aktiv_nr==5) SGUI_WindowShow(15);
  if(aktiv_nr==6) SGUI_WindowShow(16);
  if(aktiv_nr==7) SGUI_WindowShow(17);
  if(aktiv_nr==8) SGUI_WindowShow(18);
  if(aktiv_nr==9) SGUI_WindowShow(19);
  if(aktiv_nr==10) SGUI_WindowShow(20);
  if(aktiv_nr==11) SGUI_WindowShow(21);
  if(aktiv_nr==12) SGUI_WindowShow(22);
  if(aktiv_nr==13) SGUI_WindowShow(23);
  if(aktiv_nr==14) SGUI_WindowShow(24);
}
//--------------------------------------------------------------
// Funktions-Handler der OK-Buttons
//--------------------------------------------------------------
void btn_fkt(bool aktiv) {
  if(aktiv==false) {
    SGUI_ListboxSetAktivItemNr(lb,-1); // disable all items
	SGUI_WindowShowPrev(); // main-window anzeigen
  }
}
//--------------------------------------------------------------
void create_MainWindow_01(void) {
  SLABEL_t *label;
  STEXT_t *txt;

  SGUI_WindowCreateMain(1); // Main-Window (Nr=1)
  label=SGUI_LabelCreate(10,10,220,35); // ueberschrift
  SGUI_LabelSetText(label,"SGUI V:1.3");
  SGUI_TextSetCursor(10,60);
  SGUI_TextCreateString("Object-List:");    // Beschriftung

  lb=SGUI_ListboxCreate(10,80,150,130); // auswahlfeld
  SGUI_ListboxAddItem(lb,"Button");
  SGUI_ListboxAddItem(lb,"ChkBox");
  SGUI_ListboxAddItem(lb,"DropDown");
  SGUI_ListboxAddItem(lb,"FloatEdit");
  SGUI_ListboxAddItem(lb,"Gauge");
  SGUI_ListboxAddItem(lb,"IntEdit");
  SGUI_ListboxAddItem(lb,"Text");
  SGUI_ListboxAddItem(lb,"Label");
  SGUI_ListboxAddItem(lb,"Led");
  SGUI_ListboxAddItem(lb,"Listbox");
  SGUI_ListboxAddItem(lb,"Panel");
  SGUI_ListboxAddItem(lb,"RadioBtn");
  SGUI_ListboxAddItem(lb,"SelectBtn");
  SGUI_ListboxAddItem(lb,"Slider");
  SGUI_ListboxAddItem(lb,"Graph");
  SGUI_ListboxSetHandler(lb,lb_fkt);

  SGUI_TextSetCursor(10,220);
  txt=SGUI_TextCreateString("scroll Listbox and select");
  SGUI_TextSetFont(txt, &Arial_7x10);
}
//--------------------------------------------------------------
void create_ChildWindow_10(void) {
  SGUI_WindowCreateChild(10,20,20,200,200); // Child-Window (Nr=10)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Button-Demo");    // Beschriftung

  btn=SGUI_ButtonCreate(10,30,120,25); // button
  SGUI_ButtonSetText(btn,"Push");
  btn=SGUI_ButtonCreate(10,60,120,25); // button
  SGUI_ButtonSetText(btn,"PushPull");
  SGUI_ButtonSetMode(btn,SBUTTON_PUSHPULL);
  btn=SGUI_ButtonCreate(10,90,120,25); // button
  SGUI_ButtonSetText(btn,"Flat");
  SGUI_ButtonSetStyle(btn,STYLE_FLAT);
  btn=SGUI_ButtonCreate(10,120,120,25); // button
  SGUI_ButtonSetText(btn,"Color");
  SGUI_ButtonSetStyle(btn,STYLE_FLAT);
  SGUI_ButtonSetFrameSize(btn,2);
  SGUI_ButtonSetColor(btn,RGB_COL_RED,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_11(void) {
  SCHECKBOX_t *box;

  SGUI_WindowCreateChild(11,20,20,200,200); // Child-Window (Nr=11)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Checkbox-Demo");    // Beschriftung

  SGUI_CheckboxCreate(10,30,25); // checkbox
  SGUI_CheckboxCreate(50,30,25); // checkbox
  SGUI_CheckboxCreate(90,30,25); // checkbox
  box=SGUI_CheckboxCreate(10,60,25); // checkbox
  SGUI_CheckboxSetStyle(box,STYLE_RAISED);
  box=SGUI_CheckboxCreate(50,60,25); // checkbox
  SGUI_CheckboxSetStyle(box,STYLE_FLAT);
  box=SGUI_CheckboxCreate(90,60,25); // checkbox
  SGUI_CheckboxSetStyle(box,STYLE_FLAT);
  SGUI_CheckboxSetFrameSize(box,2);
  SGUI_CheckboxSetColor(box,RGB_COL_RED,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_12(void) {
  SDROPDOWN_t *dd;

  SGUI_WindowCreateChild(12,20,20,200,200); // Child-Window (Nr=12)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("DropDown-Demo");    // Beschriftung

  dd=SGUI_DropdownCreate(10,30,120,25); // dropdown
  SGUI_DropdownAddItem(dd,"DD_Item0");
  SGUI_DropdownAddItem(dd,"DD_Item1");
  SGUI_DropdownAddItem(dd,"DD_Item2");
  dd=SGUI_DropdownCreate(10,60,120,25); // dropdown
  SGUI_DropdownSetStyle(dd,STYLE_FLAT);
  SGUI_DropdownAddItem(dd,"DD_Item0");
  SGUI_DropdownAddItem(dd,"DD_Item1");
  SGUI_DropdownAddItem(dd,"DD_Item2");
  dd=SGUI_DropdownCreate(10,90,120,25); // dropdown
  SGUI_DropdownSetArrowVisible(dd,false);
  SGUI_DropdownAddItem(dd,"DD_Item0");
  SGUI_DropdownAddItem(dd,"DD_Item1");
  SGUI_DropdownAddItem(dd,"DD_Item2");
  dd=SGUI_DropdownCreate(10,120,120,25); // dropdown
  SGUI_DropdownSetStyle(dd,STYLE_FLAT);
  SGUI_DropdownSetFrameSize(dd,2);
  SGUI_DropdownSetColor(dd,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_DropdownAddItem(dd,"DD_Item0");
  SGUI_DropdownAddItem(dd,"DD_Item1");
  SGUI_DropdownAddItem(dd,"DD_Item2");

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_13(void) {
  SFLOATEDIT_t *fe;

  SGUI_WindowCreateChild(13,20,20,200,200); // Child-Window (Nr=13)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("FloatEdit-Demo");    // Beschriftung

  SGUI_FloatEditCreate(10,30,150,25); // FloatEdit
  fe=SGUI_FloatEditCreate(10,60,150,25); // FloatEdit
  SGUI_FloatEditSetStyle(fe,STYLE_FLAT);
  fe=SGUI_FloatEditCreate(10,90,150,25); // FloatEdit
  SGUI_FloatEditSetArrowVisible(fe,false);
  SGUI_FloatEditSetNumFormat(fe,5,true);
  SGUI_FloatEditSetAlignment(fe,SFLOATEDIT_ALIGN_RIGHT);
  fe=SGUI_FloatEditCreate(10,120,150,25); // FloatEdit
  SGUI_FloatEditSetStyle(fe,STYLE_FLAT);
  SGUI_FloatEditSetFrameSize(fe,2);
  SGUI_FloatEditSetColor(fe,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_FloatEditSetStep(fe,1.234);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_14(void) {
  SGAUGE_t *gauge;

  SGUI_WindowCreateChild(14,20,20,200,200); // Child-Window (Nr=14)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Gauge-Demo");    // Beschriftung

  gauge=SGUI_GaugeCreate(10,30,150,25); // gauge
  SGUI_GaugeSetValue(gauge,30);
  gauge=SGUI_GaugeCreate(10,60,150,25); // gauge
  SGUI_GaugeSetStyle(gauge,STYLE_RAISED);
  SGUI_GaugeSetValue(gauge,40);
  gauge=SGUI_GaugeCreate(10,90,150,25); // gauge
  SGUI_GaugeSetStyle(gauge,STYLE_FLAT);
  SGUI_GaugeSetValue(gauge,50);
  gauge=SGUI_GaugeCreate(10,120,150,25); // gauge
  SGUI_GaugeSetStyle(gauge,STYLE_FLAT);
  SGUI_GaugeSetFrameSize(gauge,2);
  SGUI_GaugeSetColor(gauge,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_GaugeSetValue(gauge,60);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_15(void) {
  SINTEDIT_t *ie;

  SGUI_WindowCreateChild(15,20,20,200,200); // Child-Window (Nr=15)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("IntEdit-Demo");    // Beschriftung

  SGUI_IntEditCreate(10,30,150,25); // IntEdit
  ie=SGUI_IntEditCreate(10,60,150,25); // IntEdit
  SGUI_IntEditSetStyle(ie,STYLE_FLAT);
  ie=SGUI_IntEditCreate(10,90,150,25); // IntEdit
  SGUI_IntEditSetArrowVisible(ie,false);
  SGUI_IntEditSetNumFormat(ie,5,true);
  SGUI_IntEditSetAlignment(ie,SFLOATEDIT_ALIGN_RIGHT);
  ie=SGUI_IntEditCreate(10,120,150,25); // IntEdit
  SGUI_IntEditSetStyle(ie,STYLE_FLAT);
  SGUI_IntEditSetFrameSize(ie,2);
  SGUI_IntEditSetColor(ie,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_IntEditSetStep(ie,12);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_16(void) {
  STEXT_t *txt;

  SGUI_WindowCreateChild(16,20,20,200,200); // Child-Window (Nr=16)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Text-Demo");    // Beschriftung

  SGUI_TextSetCursor(10,30);
  SGUI_TextCreateString("Das ist ein String"); // string
  SGUI_TextCursorLinefeed();
  SGUI_TextCreateString("Integer="); // string
  SGUI_TextCreateInt(123,1,false);   // int
  SGUI_TextCursorLinefeed();
  SGUI_TextCreateString("Integer="); // string
  SGUI_TextCreateInt(123,5,true); // int
  SGUI_TextCursorLinefeed();
  SGUI_TextCreateString("Float="); // string
  SGUI_TextCreateFloat(12.345,1,false); // float
  SGUI_TextCursorLinefeed();
  SGUI_TextCreateString("Float="); // string
  SGUI_TextCreateFloat(12.345,5,true); // float
  SGUI_TextCursorLinefeed();
  txt=SGUI_TextCreateString("Farbe/Font"); // string
  SGUI_TextSetColor(txt,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_TextSetFont(txt,&Arial_13x19);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_17(void) {
  SLABEL_t *label;

  SGUI_WindowCreateChild(17,20,20,200,200); // Child-Window (Nr=17)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Label-Demo");    // Beschriftung

  label=SGUI_LabelCreate(10,30,120,25); // label
  SGUI_LabelSetText(label,"String");
  label=SGUI_LabelCreate(10,60,120,25); // label
  SGUI_LabelSetStyle(label,STYLE_RAISED);
  SGUI_LabelSetInt(label,123);
  label=SGUI_LabelCreate(10,90,120,25); // label
  SGUI_LabelSetStyle(label,STYLE_FLAT);
  SGUI_LabelSetFloat(label,12.345);
  label=SGUI_LabelCreate(10,120,120,25); // label
  SGUI_LabelSetStyle(label,STYLE_FLAT);
  SGUI_LabelSetFrameSize(label,2);
  SGUI_LabelSetColor(label,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_LabelSetInt(label,12);
  SGUI_LabelSetNumFormat(label,5,true);
  SGUI_LabelSetAlignment(label, SLABEL_ALIGN_RIGHT);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_18(void) {
  SLED_t *led;

  SGUI_WindowCreateChild(18,20,20,200,200); // Child-Window (Nr=18)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Led-Demo");    // Beschriftung

  SGUI_LedCreate(10,30,25); // led
  led=SGUI_LedCreate(50,30,25); // led
  SGUI_LedOn(led);
  led=SGUI_LedCreate(90,30,25); // led
  SGUI_LedSetStyle(led,STYLE_LOWERED);
  led=SGUI_LedCreate(10,60,25); // led
  SGUI_LedSetStyle(led,STYLE_FLAT);
  led=SGUI_LedCreate(50,60,25); // led
  SGUI_LedSetStyle(led,STYLE_FLAT);
  SGUI_LedSetFrameSize(led,2);
  SGUI_LedSetColor(led,RGB_COL_RED,RGB_COL_BLACK,RGB_COL_GREEN);
  led=SGUI_LedCreate(10,90,25); // led
  SGUI_LedSetTyp(led,SLED_RECT);
  led=SGUI_LedCreate(50,90,25); // led
  SGUI_LedSetTyp(led,SLED_RECT);
  SGUI_LedOn(led);
  led=SGUI_LedCreate(90,90,25); // led
  SGUI_LedSetTyp(led,SLED_RECT);
  SGUI_LedSetStyle(led,STYLE_LOWERED);
  led=SGUI_LedCreate(10,120,25); // led
  SGUI_LedSetTyp(led,SLED_RECT);
  SGUI_LedSetStyle(led,STYLE_FLAT);
  led=SGUI_LedCreate(50,120,25); // led
  SGUI_LedSetTyp(led,SLED_RECT);
  SGUI_LedSetStyle(led,STYLE_FLAT);
  SGUI_LedSetFrameSize(led,2);
  SGUI_LedSetColor(led,RGB_COL_RED,RGB_COL_BLACK,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_19(void) {
  SLISTBOX_t *list;

  SGUI_WindowCreateChild(19,20,20,200,200); // Child-Window (Nr=19)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Listbox-Demo");    // Beschriftung

  list=SGUI_ListboxCreate(10,30,120,60); // listbox
  SGUI_ListboxSetStyle(list,STYLE_FLAT);
  SGUI_ListboxAddItem(list,"LB_Item0");
  SGUI_ListboxAddItem(list,"LB_Item1");
  SGUI_ListboxAddItem(list,"LB_Item2");
  list=SGUI_ListboxCreate(10,100,120,60); // listbox
  SGUI_ListboxSetStyle(list,STYLE_FLAT);
  SGUI_ListboxSetFrameSize(list,2);
  SGUI_ListboxSetColor(list,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_ListboxSetSliderVisible(list,false);
  SGUI_ListboxAddItem(list,"LB_Item0");
  SGUI_ListboxAddItem(list,"LB_Item1");

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_20(void) {
  SPANEL_t *panel;

  SGUI_WindowCreateChild(20,20,20,200,200); // Child-Window (Nr=20)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Panel-Demo");    // Beschriftung

  SGUI_PanelCreate(10,30,60,60); // panel
  panel=SGUI_PanelCreate(10,100,60,60); // panel
  SGUI_PanelSetStyle(panel,STYLE_RAISED);
  panel=SGUI_PanelCreate(80,30,60,60); // panel
  SGUI_PanelSetStyle(panel,STYLE_FLAT);
  panel=SGUI_PanelCreate(80,100,60,60); // panel
  SGUI_PanelSetStyle(panel,STYLE_FLAT);
  SGUI_PanelSetFrameSize(panel,2);
  SGUI_PanelSetColor(panel,RGB_COL_RED,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_21(void) {
  SRBTN_t *rb;

  SGUI_WindowCreateChild(21,20,20,200,200); // Child-Window (Nr=21)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("RadioBtn-Demo");    // Beschriftung

  SGUI_RadioButtonCreate(10,30,25); // Radio-Button
  SGUI_RadioButtonCreate(50,30,25); // Radio-Button
  SGUI_RadioButtonCreate(90,30,25); // Radio-Button
  rb=SGUI_RadioButtonCreate(10,60,25); // Radio-Button
  SGUI_RadioButtonSetGroup(rb,2);
  SGUI_RadioButtonSetStyle(rb,STYLE_RAISED);
  rb=SGUI_RadioButtonCreate(50,60,25); // Radio-Button
  SGUI_RadioButtonSetGroup(rb,2);
  SGUI_RadioButtonSetStyle(rb,STYLE_FLAT);
  rb=SGUI_RadioButtonCreate(90,60,25); // Radio-Button
  SGUI_RadioButtonSetGroup(rb,2);
  SGUI_RadioButtonSetStyle(rb,STYLE_FLAT);
  SGUI_RadioButtonSetFrameSize(rb,2);
  SGUI_RadioButtonSetColor(rb,RGB_COL_RED,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_22(void) {
  SSELECTBTN_t *sb;

  SGUI_WindowCreateChild(22,20,20,200,200); // Child-Window (Nr=22)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("SelectBtn-Demo");    // Beschriftung

  sb=SGUI_SelectButtonCreate(10,30,150,25); // Select-Button
  SGUI_SelectButtonAddItem(sb,"SB_Item0");
  SGUI_SelectButtonAddItem(sb,"SB_Item1");
  SGUI_SelectButtonAddItem(sb,"SB_Item2");
  sb=SGUI_SelectButtonCreate(10,60,150,25); // Select-Button
  SGUI_SelectButtonSetStyle(sb,STYLE_FLAT);
  SGUI_SelectButtonAddItem(sb,"SB_Item0");
  SGUI_SelectButtonAddItem(sb,"SB_Item1");
  SGUI_SelectButtonAddItem(sb,"SB_Item2");
  sb=SGUI_SelectButtonCreate(10,90,150,25); // Select-Button
  SGUI_SelectButtonSetArrowVisible(sb,false);
  SGUI_SelectButtonAddItem(sb,"SB_Item0");
  SGUI_SelectButtonAddItem(sb,"SB_Item1");
  SGUI_SelectButtonAddItem(sb,"SB_Item2");
  sb=SGUI_SelectButtonCreate(10,120,150,25); // Select-Button
  SGUI_SelectButtonSetStyle(sb,STYLE_FLAT);
  SGUI_SelectButtonSetFrameSize(sb,2);
  SGUI_SelectButtonSetColor(sb,RGB_COL_BLACK,RGB_COL_RED,RGB_COL_GREEN);
  SGUI_SelectButtonAddItem(sb,"SB_Item0");
  SGUI_SelectButtonAddItem(sb,"SB_Item1");
  SGUI_SelectButtonAddItem(sb,"SB_Item2");

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_23(void) {
  SSLIDER_t *sl;

  SGUI_WindowCreateChild(23,20,20,200,200); // Child-Window (Nr=23)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Slider-Demo");    // Beschriftung

  SGUI_SliderCreate(10,30,150,25); // slider
  sl=SGUI_SliderCreate(10,60,150,25); // slider
  SGUI_SliderSetStyle(sl,STYLE_FLAT);
  sl=SGUI_SliderCreate(10,90,150,25); // slider
  SGUI_SliderSetArrowVisible(sl,false);
  sl=SGUI_SliderCreate(10,120,150,25); // slider
  SGUI_SliderSetStyle(sl,STYLE_FLAT);
  SGUI_SliderSetFrameSize(sl,2);
  SGUI_SliderSetColor(sl,RGB_COL_RED,RGB_COL_GREEN);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}
//--------------------------------------------------------------
void create_ChildWindow_24(void) {
  SGRAPH_t *graph;

  SGUI_WindowCreateChild(24,20,20,200,200); // Child-Window (Nr=24)
  SGUI_TextSetCursor(10,10);
  SGUI_TextCreateString("Graph-Demo");    // Beschriftung

  graph=SGUI_GraphCreate(10,30,160,120); // graph
  SGUI_GraphCreateDataArray(graph,2,10);

  SGUI_GraphSetCHValue(graph,0,0,10); // Kurve-1
  SGUI_GraphSetCHValue(graph,0,1,5);
  SGUI_GraphSetCHValue(graph,0,2,20);
  SGUI_GraphSetCHValue(graph,0,3,15);
  SGUI_GraphSetCHValue(graph,0,4,30);
  SGUI_GraphSetCHColor(graph,1,RGB_COL_BLUE);

  SGUI_GraphSetCHValue(graph,1,0,20); // Kurve-2
  SGUI_GraphSetCHValue(graph,1,1,15);
  SGUI_GraphSetCHValue(graph,1,2,5);
  SGUI_GraphSetCHValue(graph,1,3,30);
  SGUI_GraphSetCHValue(graph,1,4,55);
  SGUI_GraphSetCHColor(graph,1,RGB_COL_RED);

  btn=SGUI_ButtonCreate(10,170,50,25); // ok-button
  SGUI_ButtonSetText(btn,"OK");
  SGUI_ButtonSetHandler(btn,btn_fkt);
}


