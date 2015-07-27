#include "global.h"

extern int g_numPendingIrps;

USHORT scanCode[4];
BOOLEAN end2aKey = FALSE;
BOOLEAN ctrlPressed = FALSE;
BOOLEAN altPressed = FALSE;
BOOLEAN endPressed = FALSE;
BOOLEAN l33tMode = FALSE;

int counter = 0;
USHORT previousEndKey = 0x0000;
IO_COMPLETION_ROUTINE readCompletion;

NTSTATUS dispatchPassDown(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp){
	NTSTATUS status;
	DbgPrint("Entering DispatchPassDown Routine...\n");
	//pass the irp down to the target without touching it   
	IoSkipCurrentIrpStackLocation(pIrp);
	status = IofCallDriver(((PKBDFNFLT_DEVICE_EXTENSION)pDeviceObject->DeviceExtension)->pKeyboardDevice, pIrp);
	if (status)
		DbgPrint("IRP passed down with status 0x%lX", status);

	return status;
}//end DriverDispatcher  

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
// THE THEORY BEHIND THE HOOK:   
// 1. The Operating System sends an empty IRP packet down the device stack for the keyboard.   
//   
// 2. The IRP is intercepted by the ReadDispatcher routine in the filter driver. While in   
//    this routine, the IRP is "tagged" with a "completion routine".  This is a callback routine    
//    which basically says "I want another go at this packet later when its got some data".   
//    ReadDispatcher then sends the IRP on it's down the device stack to the drivers underneath.   
//   
// 3. When the tagged, empty IRP reaches the bottom of the stack at the hardware / software    
//    interface, it waits for a keypress.   
//   
// 4. When a key on the keyboard is pressed, the IRP is filled with the scan code for the    
//    pressed key and sent on its way back up the device stack.   
//   
// 5. On its way back up the device stack, the completion routines that the IRP was tagged   
//    with on its way down the stack are called and the IRP is packed passed into them. This    
//    gives the filter driver an opportunity to extract the scan code information stored    
//    in the packet from the user's key press.   
//   
// NOTE: Other IRPs other than IRP_MJ_READ are simply passed down to the drivers underneath   
//      without modification.   
//   
//   
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@   
   
 
   
   
   
/*****************************************************************************************************  
// This is the acutal hook routine which we will redirect the keyboard's read IRP's to  
//  
// NOTE: The DispatchRead, DispatchWrite, and DispatchDeviceControl routines of lowest-level device   
// drivers, and of intermediate drivers layered above them in the system paging path, can be called at   
// IRQL = APC_LEVEL and in an arbitrary thread context. The DispatchRead and/or DispatchWrite routines,  
// and any other routine that also processes read and/or write requests in such a lowest-level device   
// or intermediate driver, must be resident at all times. These driver routines can neither be pageable   
// nor be part of a driver's pageable-image section; they must not access any pageable memory. Furthermore,  
// they should not be dependent on any blocking calls (such as KeWaitForSingleObject with a nonzero  
// time-out).   
*******************************************************************************************************/ 

NTSTATUS dispatchRead(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp){   
	NTSTATUS status = STATUS_UNABLE_TO_DECOMMIT_VM;
    /////////////////////////////////////////////////////////////////////////   
    //NOTE: The theory is that empty keyboard IRP's are sent down through   
    //the device stack where they wait until a key is pressed. The keypress   
    //completes the IRP. It is therefore necessary to capture the empty   
    //IRPS on the way down to the keyboard and "tag" them with a callback   
    //function which will be called whenever a key is pressed and the IRP is   
    //completed. We "tag" them by setting a "completion routine" using the   
    //kernel API IoSetCompletionRoutine   
    ////////////////////////////////////////////////////////////////////////   
    //DbgPrint("Entering DispatchRead Routine...\n");   
       
    //Each driver that passes IRPs on to lower drivers must set up the stack location for the    
    //next lower driver. A driver calls IoGetNextIrpStackLocation to get a pointer to the next-lower   
    //driver’s I/O stack location   
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(pIrp);   
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(pIrp);   
    *nextIrpStack = *currentIrpStack;   
   
    //Set the completion callback   
    IoSetCompletionRoutine(pIrp, readCompletion, pDeviceObject, TRUE, TRUE, TRUE);   
   
    //track the # of pending IRPs   
     g_numPendingIrps++;   
 
    //Pass the IRP on down to the driver underneath us   
    status = IofCallDriver(((PKBDFNFLT_DEVICE_EXTENSION) pDeviceObject->DeviceExtension)->pKeyboardDevice ,pIrp);   
	return status;
}//end DispatchRead   
   
   
   
//@@@@@@@@@@@@@@@@@@@@@@@@   
// IRQL = DISPATCH_LEVEL   
//@@@@@@@@@@@@@@@@@@@@@@@@@   
NTSTATUS readCompletion(IN PDEVICE_OBJECT pDeviceObject, IN PIRP pIrp, IN PVOID Context){
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(pDeviceObject);

	//if the request has completed, extract the value of the key   

	if (pIrp->IoStatus.Status == STATUS_SUCCESS){
		PKEYBOARD_INPUT_DATA keys = (PKEYBOARD_INPUT_DATA)pIrp->AssociatedIrp.SystemBuffer;
		int numKeys = (int)pIrp->IoStatus.Information / sizeof(KEYBOARD_INPUT_DATA);
		if (!keys){
			DbgPrint("Key error, keys=NULL");
		}
		else{
			DbgPrint("Key pressed.%x", keys->MakeCode);

			for (int i = 0; i < numKeys; i++){
				if (keys[i].MakeCode == 0x0052){
					keys[i].MakeCode = 0x0050;
				}
				if (keys[i].MakeCode == 0x001d){
					if (keys[i].Flags == KEY_MAKE){
						ctrlPressed = TRUE;
					}
					else if (keys[i].Flags == KEY_BREAK){
						ctrlPressed = FALSE;
					}
					if (end2aKey == FALSE){
						previousEndKey = 0x0000;
						endPressed = FALSE;
					}
				}
				else if (keys[i].MakeCode == 0x0038){
					if (keys[i].Flags == KEY_MAKE){
						altPressed = TRUE;
					}
					else if (keys[i].Flags == KEY_BREAK){
						altPressed = FALSE;
					}
					if (end2aKey == FALSE){
						previousEndKey = 0x0000;
						endPressed = FALSE;
					}
				}
				else if ((keys[i].MakeCode == 0x004f) || (keys[i].MakeCode == 0x002a)){
					if (keys[i].MakeCode == 0x002a){
						if (previousEndKey == 0x0000){
							end2aKey = TRUE;
							endPressed = TRUE;
						}
						else if (previousEndKey == 0x004f){
							end2aKey = FALSE;
							endPressed = FALSE;
						}
					}
					else if (keys[i].MakeCode == 0x004f){
						if (end2aKey == TRUE){
							if (previousEndKey == 0x002a){
								endPressed = TRUE;
							}
						}
						else{
							if (previousEndKey == 0x0000){
								endPressed = TRUE;
							}
						}
					}

					previousEndKey = keys[i].MakeCode;
				}
				else if ((keys[i].MakeCode == 0x0002) && (keys[i].Flags == KEY_BREAK)){
					counter = 0;
					scanCode[0] = 0x0000;
					scanCode[1] = 0x0000;
					scanCode[2] = 0x0000;
					scanCode[3] = 0x0000;
					if (end2aKey == FALSE){
						previousEndKey = 0x0000;
						endPressed = FALSE;
					}
				}
				else{
					ctrlPressed = FALSE;
					altPressed = FALSE;
					endPressed = FALSE;
				
					if (end2aKey == FALSE){
						previousEndKey = 0x0000;
					}
				}
				if (keys[i].Flags == KEY_BREAK){
					scanCode[counter] = keys[i].MakeCode;
					counter++;
					if (counter >= 4){
						if ((scanCode[0] == 0x0002) && (scanCode[1] == 0x0004) && (scanCode[2] == 0x0004) && (scanCode[3] == 0x0008)){
							if (!l33tMode){
								l33tMode = TRUE;
								DbgPrint("leet m0de activated");
							}
							else{
								l33tMode = FALSE;
								DbgPrint("l33t mode off");
							}
						}
						counter = 0;
						scanCode[0] = 0x0000;
						scanCode[1] = 0x0000;
						scanCode[2] = 0x0000;
						scanCode[3] = 0x0000;
					}
				}

				if (l33tMode){
					if (keys[i].Flags == KEY_E0){
						DbgPrint("grdgrd, key e0 %x", keys[i].MakeCode);
					}
					if (keys[i].Flags == KEY_E1){
						DbgPrint("grdgrd, key e1 %x", keys[i].MakeCode);
					}
					DbgPrint("huubzu, %x", keys[i].MakeCode);
					switch (keys[i].MakeCode){
					case 0x001e:{
						keys[i].MakeCode = 0x0005; //a
						break;
					}
					case 0x0012:{
						keys[i].MakeCode = 0x0004; //e
						break;
					}
					case 0x0022:{
						keys[i].MakeCode = 0x000a; //g
						break;
					}
					case 0x0026:{
						keys[i].MakeCode = 0x0002; //l
						break;
					}
					case 0x0018:{
						keys[i].MakeCode = 0x000b; //o
						break;
					}
					case 0x0013:{
						//keys[i].MakeCode = 0x0003; //r
						break;
					}
					case 0x001f:{
						keys[i].MakeCode = 0x0006; //s
						break;
					}
					case 0x0014:{
						//keys[i].MakeCode = 0x0008; //t
						break;
					}
								/*case 0x0052:{
								keys[i].MakeCode = 0x0050;
								break;
								}*/
					default:{
						break;
					}
					}

					/*
					a=4,
					c=(
					e=3,€,³
					g=9
					i=!
					l=1
					o=0
					r=2
					s=5,§,$
					t=7*/
				}

				UCHAR port = 0x64;

				if ((ctrlPressed == TRUE) && (altPressed == TRUE) && (endPressed == TRUE)){
					DbgPrint("all three keys are pressed");
					WRITE_PORT_UCHAR(&port, (UCHAR)0xFE);
				}
			}//end for   
		}
	}//end if   

	//Mark the Irp pending if necessary   
	if (pIrp->PendingReturned)
		IoMarkIrpPending(pIrp);

	//Remove the Irp from our own count of tagged (pending) IRPs 
	KIRQL oldIrql;
	KeRaiseIrql(HIGH_LEVEL, &oldIrql);
	g_numPendingIrps--;
	KeLowerIrql(oldIrql);

	return pIrp->IoStatus.Status;
}//end OnReadCompletion  