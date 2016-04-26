//--------------------------------------------------------------
// File     : ub_sgui_stringlist.h
//--------------------------------------------------------------

//--------------------------------------------------------------
#ifndef __STM32F4_UB_SGUI_STRLIST_H
#define __STM32F4_UB_SGUI_STRLIST_H


//--------------------------------------------------------------
// Includes
//--------------------------------------------------------------
#include "stm32_ub_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



//--------------------------------------------------------------
// Anzahl wieviele Strings maximal in einer Liste sein können
//--------------------------------------------------------------
#define   MAX_STRINGLIST_ANZ        100


//--------------------------------------------------------------
// Maximale anzahl der Zeichen pro String
//--------------------------------------------------------------
#define   MAX_STRINGLIST_LEN         20



//--------------------------------------------------------------
// Speicherbedarf pro Eintrag
// (4 bytes für den Pointer + Anzahl der Character)
//--------------------------------------------------------------
#define   STRINGLIST_BYTES_PER_ITEM    4+MAX_STRINGLIST_LEN


//--------------------------------------------------------------
// Typ der Stringliste
//--------------------------------------------------------------
typedef char* StringList_t;   



//--------------------------------------------------------------
// interne Funktionen fuer SGUI
//--------------------------------------------------------------
StringList_t* SGUI_StringListCreate(uint16_t cnt);
StringList_t* SGUI_StringListAdd(StringList_t* ptr, uint16_t cnt);
void SGUI_StringListSet(StringList_t* ptr, uint16_t pos, char *txt);
StringList_t* SGUI_StringListDelete(StringList_t* ptr, uint16_t cnt, uint16_t pos);
StringList_t* SGUI_StringListInsert(StringList_t* ptr, uint16_t cnt, uint16_t pos);





//--------------------------------------------------------------
#endif // __STM32F4_UB_SGUI_STRLIST_H
