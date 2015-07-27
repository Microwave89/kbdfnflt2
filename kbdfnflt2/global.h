#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <ntifs.h>
#include <..\ndk\ntndk.h>
#include <initguid.h>
#include "ntddkbd.h"

typedef struct _IO_WORKITEM IO_WORKITEM;
///////////////////////// 
// STRUCTURES 
//////////////////////// 
typedef struct _KEY_STATE{
	BOOLEAN shiftPressed; //if the shift key is pressed  
	BOOLEAN capslockPressed; //if the caps lock key is pressed down 
	BOOLEAN ctrlPressed; //if the control key is pressed down 
	//USHORT bleh;
	BOOLEAN altPressed; //if the alt key is pressed down 
} KEY_STATE, *PKEY_STATE;


typedef struct _MY_WORK_CONTEXT{
	PIO_WORKITEM pItem;
	PUNICODE_STRING pSymlink;
} MY_WORK_CONTEXT, *PMY_WORK_CONTEXT;

#pragma warning (push)
#pragma warning (disable:4200)
typedef struct _KBDFNFLT_DEVICE_EXTENSION {		
	KEY_STATE keyState;	//state of special keys like CTRL, SHIFT, ect 
	PWCHAR pDevObjString; ///For bookkeeping reasons
	PDEVICE_OBJECT pKeyboardDevice; //pointer to next keyboard device on device stack 
	PDRIVER_OBJECT pKbdClassDrv; ///Mainly for ObfDereferenceObject reasons
	MY_WORK_CONTEXT workContext;
	PIO_WORKITEM pWorkitem;
	char workitem[0];
}KBDFNFLT_DEVICE_EXTENSION, *PKBDFNFLT_DEVICE_EXTENSION;
#pragma warning (pop)
#endif