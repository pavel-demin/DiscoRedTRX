//--------------------------------------------------------------
// File     : ub_sgui_stringlist.c
// Datum    : 27.07.2015
// Version  : 1.0
// Autor    : UB
// EMail    : mc-4u(@)t-online.de
// Web      : www.mikrocontroller-4u.de
// CPU      : STM32F746
// IDE      : OpenSTM32
// GCC      : 4.9 2015q2
// Module   : keine
// Funktion : Stringlisten verwalten Arrays von Strings
//            Es gibt funktionen um String der Liste hinzuzufügen
//            zu löschen und zu überschreiben
//--------------------------------------------------------------


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "ub_sgui_stringlist.h"



//--------------------------------------------------------------
// eine leere StringList neu erzeugen
// cnt = Anzahl der Einträge (Strings)
// Speicherbedarf (bytes) = (cnt*4) + (cnt*MAX_STRINGLIST_LEN)
//--------------------------------------------------------------
StringList_t* SGUI_StringListCreate(uint16_t cnt)
{
  StringList_t* ptr;
  uint32_t n;
  
  if(cnt==0) return NULL;
  if(cnt>=MAX_STRINGLIST_ANZ) return NULL;
  
  // speicher reservieren 
  // zuerst 'anz' Pointer auf char reservieren
  ptr = (char **) malloc(cnt * sizeof(char *));  
  if(ptr==NULL) return NULL;
  // dann 'anz' strings reservieren
  for (n = 0; n < cnt; n += 1) {
    ptr[n] = (char *) malloc(MAX_STRINGLIST_LEN * sizeof(char));    
    if(ptr[n]==NULL) return NULL;
  }
  
  // init aller Werte
  for (n = 0; n < cnt; n += 1) {
    ptr[n][0]=0x00; 
  }  
   
  return ptr;
}

//--------------------------------------------------------------
// Einen leeren Eintrag an eine bestehende Stringliste anhängen
// cnt = Anzahl der aktuellen Einträge (Strings)
// Speicherbedarf (bytes) = 4 + MAX_STRINGLIST_LEN
//--------------------------------------------------------------
StringList_t* SGUI_StringListAdd(StringList_t* ptr, uint16_t cnt)
{
  uint32_t n;
  StringList_t *dummy;
  StringList_t *ptr_new;
  
  if(ptr==NULL) return NULL;
  if(cnt==0) return NULL;
  if(cnt>=(MAX_STRINGLIST_ANZ-1)) return NULL;
    
  // dummy Stringliste erzeugen            
  dummy=SGUI_StringListCreate(cnt);  
  if(dummy==NULL) return NULL;
  // alle Strings kopieren    
  for(n=0;n<cnt;n++) {
    strcpy(dummy[n], ptr[n]);
  }  
  // speicher der stringliste freigeben
  for (n = 0; n < cnt; n += 1) {
    free((void *) ptr[n]);
  }
  free((void *) ptr);   
  // neue Stringliste erzeugen (mit einem Eintrag mehr)
  ptr_new=SGUI_StringListCreate(cnt+1);
  if(ptr_new==NULL) return NULL;  
  // Strings zurück kopieren    
  for(n=0;n<cnt;n++) {
    strcpy(ptr_new[n], dummy[n]);
  }  
  // speicher vom dummy wieder freigeben
  for (n = 0; n < cnt; n += 1) {
    free((void *) dummy[n]);
  }
  free((void *) dummy);
  
  return ptr_new;  
}

//--------------------------------------------------------------
// Einen Eintrag (String) einer bestehenden Stringliste ändern
// txt = String
// pos = Position in der Stringliste [0...n] zum ändern
// Speicherbedarf (bytes) = 0
//--------------------------------------------------------------
void SGUI_StringListSet(StringList_t* ptr, uint16_t pos, char *txt)
{
  uint32_t len,n;
    
  if((ptr==NULL) || (txt==NULL)) return;
 
  // string hinzufuegen
  len=strlen(txt);
  if(len==0) {
    ptr[pos][0]=0x00;
  }
  else if(len<(MAX_STRINGLIST_LEN-1)) {
    strcpy(ptr[pos],txt);
  }
  else {
    for(n=0;n<(MAX_STRINGLIST_LEN-1);n++) {
      ptr[pos][n]=txt[n];   
    }
    ptr[pos][n]=0x00;
  }       
}

//--------------------------------------------------------------
// Einen Eintrag (String) einer bestehenden Stringliste löschen
// cnt = Anzahl der aktuellen Einträge (Strings)
// pos = Position in der Stringliste [0...n] die geslöscht wird
// speicher der frei wird (bytes) = 4 + MAX_STRINGLIST_LEN
//--------------------------------------------------------------
StringList_t* SGUI_StringListDelete(StringList_t* ptr, uint16_t cnt, uint16_t pos)
{
  uint32_t n,m;
  StringList_t *dummy;
  StringList_t *ptr_new;
  
  if(ptr==NULL) return NULL;
  if(cnt==0) return NULL; 
  if(cnt<=pos) return NULL;
  
  // dummy Stringliste erzeugen            
  dummy=SGUI_StringListCreate(cnt);  
  if(dummy==NULL) return NULL;
  // alle Strings kopieren   
  for(n=0;n<cnt;n++) {
    strcpy(dummy[n], ptr[n]);
  }    
  // speicher der stringliste freigeben
  for (n = 0; n < cnt; n += 1) {
    free((void *) ptr[n]);
  }
  free((void *) ptr); 
  // neue Stringliste erzeugen (mit einem Eintrag weniger)
  if(cnt>1) {
    ptr_new=SGUI_StringListCreate(cnt-1);  
    if(ptr_new==NULL) return NULL; 
    // alle notwendigen Strings zurück kopieren   
    m=0;
    for(n=0;n<cnt;n++) {
      if(n!=pos) {
        strcpy(ptr_new[m], dummy[n]);
        m++;
      }
    } 
    // speicher vom dummy wieder freigeben
    for (n = 0; n < cnt; n += 1) {
      free((void *) dummy[n]);
    }
    free((void *) dummy);    
  }
  else {
    // Stringliste ist komplett leer
    ptr_new=NULL;
  }
  
  return ptr_new; 
}

//--------------------------------------------------------------
// Einen leeren Eintrag zu einer bestehenden Stringliste hinzufuegen
// cnt = Anzahl der aktuellen Einträge (Strings)
// pos = Position in der Stringliste [0...n] vom neuen Eintrag
// Speicherbedarf (bytes) = 4 + MAX_STRINGLIST_LEN
//--------------------------------------------------------------
StringList_t* SGUI_StringListInsert(StringList_t* ptr, uint16_t cnt, uint16_t pos)
{
  uint32_t n,m;
  StringList_t *dummy;
  StringList_t *ptr_new;
  
  if(ptr==NULL) return NULL;
  if(cnt==0) return NULL; 
  if(cnt<=pos) return NULL;

  // dummy Stringliste erzeugen            
  dummy=SGUI_StringListCreate(cnt);  
  if(dummy==NULL) return NULL;
  // alle Strings kopieren   
  for(n=0;n<cnt;n++) {
    strcpy(dummy[n], ptr[n]);
  }  
  // speicher der stringliste freigeben
  for (n = 0; n < cnt; n += 1) {
    free((void *) ptr[n]);
  }
  free((void *) ptr);  
  // neue Stringliste erzeugen (mit einem Eintrag mehr)
  ptr_new=SGUI_StringListCreate(cnt+1);
  if(ptr_new==NULL) return NULL; 
  // Strings zurück kopieren 
  m=0;
  for(n=0;n<cnt;n++) {
    if(n==pos) m++;
    strcpy(ptr_new[m], dummy[n]);
    m++;  
  } 
  ptr_new[pos][0]=0x00;
  // speicher vom dummy wieder freigeben
  for (n = 0; n < cnt; n += 1) {
    free((void *) dummy[n]);
  }
  free((void *) dummy);   
  
  return ptr_new;
}
