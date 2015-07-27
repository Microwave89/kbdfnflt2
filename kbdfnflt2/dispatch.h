#ifndef _DISPATCH_H_ 
#define _DISPATCH_H_ 
 
////////////////////////// 
// PROTOTYPES 
////////////////////////// 
DRIVER_DISPATCH dispatchPassDown;
_Dispatch_type_(IRP_MJ_READ) DRIVER_DISPATCH dispatchRead;
#endif