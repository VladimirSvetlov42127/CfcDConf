#pragma once

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#define CANT_READ_ADDRESS			"Невозможно прочитать данный адрес!"

#define CHECK_ADDRESS(x) { if(x == nullptr) { MsgBox::error(CANT_READ_ADDRESS);	return; }}		// Проверка адреса 
#define CHECK_ADDRESS_SHOW_MESSAGE(x, mesg) { if(x == nullptr) { MsgBox::error(mesg);	return; }}		// Проверка адреса 
#define CHECK_ADDRESS_RETURN_NULLPTR(x) { if(x == nullptr) { MsgBox::error(CANT_READ_ADDRESS);	return nullptr; }}		// Проверка адреса 
#define CHECK_ADDRESS_RETURN_BOOL(x) { if(x == nullptr) { MsgBox::error(CANT_READ_ADDRESS);	return false; }}		// Проверка адреса 
#define CHECK_ADDRESS_RETURN_VALUE(x, val) { if(x == nullptr) { MsgBox::error(CANT_READ_ADDRESS);	return val; }}		// Проверка адреса 
