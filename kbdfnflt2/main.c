#include "global.h"
#include "dispatch.h"
#include <wdmguid.h>

DRIVER_INITIALIZE mydrvEntry;
DRIVER_UNLOAD mydrvUnload;
DRIVER_NOTIFICATION_CALLBACK_ROUTINE keyboardAddedOrRemoved;
ULONG g_poolTag = 0x466E464B;
PVOID g_pNotificationEntry;
int g_numPendingIrps = 0; ///Use KSEMAPHORE instead!!
IO_WORKITEM_ROUTINE testRoutine;

//NTSTATUS hookKeyboard(IN PDRIVER_OBJECT pDriverObject){
//	DbgPrint("Entering Hook Routine...\n");
//
//	//the filter device object   
//	PDEVICE_OBJECT pKeyboardDeviceObject;
//	
//
//
//	DbgPrint("Created keyboard device successfully...\n");
//
//	//////////////////////////////////////////////////////////////////////////////////   
//	//Copy the characteristics of the target keyboard driver into the  filter device    
//	//object because we have to mirror the keyboard device underneath us.   
//	//These characteristics can be determined by examining the target driver using an   
//	//application like DeviceTree in the DDK   
//	//////////////////////////////////////////////////////////////////////////////////   
//	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags | (DO_BUFFERED_IO | DO_POWER_PAGABLE);
//	pKeyboardDeviceObject->Flags = pKeyboardDeviceObject->Flags & ~DO_DEVICE_INITIALIZING;
//	DbgPrint("Flags set succesfully...\n");
//
//	//////////////////////////////////////////////////////////////////////////////////////////////   
//	//Initialize the device extension - The device extension is a custom defined data structure   
//	//for our driver where we can store information which is guaranteed to exist in nonpaged memory.   
//	///////////////////////////////////////////////////////////////////////////////////////////////   
//	RtlSecureZeroMemory(pKeyboardDeviceObject->DeviceExtension, sizeof(DEVICE_EXTENSION));
//	DbgPrint("Device Extension Initialized...\n");
//
//	//Get the pointer to the device extension   
//	PDEVICE_EXTENSION pKeyboardDeviceExtension = (PDEVICE_EXTENSION)pKeyboardDeviceObject->DeviceExtension;
//
//	//////////////////////////////////////////////////////////////////////////////////////////////   
//	//Insert the filter driver onto the device stack above the target keyboard driver underneath and   
//	//save the old pointer to the top of the stack. We need this address to direct IRPS to the drivers   
//	//underneath us on the stack.   
//	///////////////////////////////////////////////////////////////////////////////////////////////   
//
//	UNICODE_STRING uKbdClassDrv;
//	PDRIVER_OBJECT pKbdClassDrv = NULL;
//	RtlInitUnicodeString(&uKbdClassDrv, L"\\Driver\\kbdclass");
//	status = ObReferenceObjectByName(&uKbdClassDrv, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, 0, IoDriverObjectType, KernelMode, NULL, &pKbdClassDrv);
//	if (status){
//		DbgPrint("ObReferenceObjectByName fail%lX", status);
//		IoDeleteDevice(pKeyboardDeviceObject);
//		return status;
//	}
//
//	ObfDereferenceObject(pKbdClassDrv);
//	PDEVICE_OBJECT pExistingKbd = NULL;
//	pExistingKbd = pKbdClassDrv->DeviceObject;
//	if (!pExistingKbd){
//		status = STATUS_OBJECT_NAME_NOT_FOUND;
//		DbgPrint("kbdclass has no devices , fail1");
//		IoDeleteDevice(pKeyboardDeviceObject);
//		return status;
//	}
//
//	PDEVICE_OBJECT pActualLowerDevObj = IoAttachDeviceToDeviceStack(pKeyboardDeviceObject, pExistingKbd);
//	if (!pActualLowerDevObj){
//		status = STATUS_DRIVER_INTERNAL_ERROR;
//		DbgPrint("cannot attach device object , fail");
//		IoDeleteDevice(pKeyboardDeviceObject);
//		return status;
//	}
//
//	pKeyboardDeviceExtension->pKeyboardDevice = pActualLowerDevObj;
//	DbgPrint("lower device object = %p", pActualLowerDevObj);
//	DbgPrint("Filter Device Attached Successfully...\n");
//	return STATUS_SUCCESS;
//}//end HookKeyboard  

void testRoutine(_In_     PDEVICE_OBJECT pDeviceObject, _In_opt_ PVOID          pContext){
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(pContext);
	PFILE_OBJECT pFileObj;
	PDEVICE_OBJECT pDevObj;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	if (!pContext){
		DbgPrint("pcontext null");
		return;
	}
	PMY_WORK_CONTEXT pWorkContext = (PMY_WORK_CONTEXT)pContext;
	status = IoGetDeviceObjectPointer(pWorkContext->pSymlink, 0, &pFileObj, &pDevObj);
	if (status){
		DbgPrint("failed to access devobj!%lX", status);
		return;
	}
	DbgPrint("not failed to access devobj!%lX", status);
	ObfDereferenceObject(pFileObj);
	ObfDereferenceObject(pDevObj);
	//return;
}


NTSTATUS hookKeyboard(_In_ PDEVICE_OBJECT pKbdFnFltDevice, _In_ PDEVICE_OBJECT pKbdClassDevice){
	UNREFERENCED_PARAMETER(pKbdFnFltDevice);
	UNREFERENCED_PARAMETER(pKbdClassDevice);

	
	//NTSTATUS status = STATUS_UNSUCCESSFUL;
	//UNICODE_STRING uKbdClassDrv;
	//PDRIVER_OBJECT pKbdClassDrv = NULL;
	//PKBDFNFLT_DEVICE_EXTENSION pKbdFnFltExtension = (PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension;

	//RtlInitUnicodeString(&uKbdClassDrv, L"\\Driver\\kbdclass");
	//status = ObReferenceObjectByName(&uKbdClassDrv,
	//	OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
	//	NULL, 0, IoDriverObjectType, KernelMode, NULL, &pKbdClassDrv);

	//if (status)
	//	return status;
	//
	//if (!pKbdClassDrv->DeviceObject){
	//	ObfDereferenceObject(pKbdClassDrv);
	//	DbgPrint("no keyboard devices were found!");
	//	return STATUS_OBJECT_NAME_NOT_FOUND;
	//}
	//
	//PDEVICE_OBJECT pNextKbdDevice = pKbdClassDrv->DeviceObject;
	//while (pNextKbdDevice){
	//	Io
	//}


	//pKbdFnFltExtension->pKbdClassDrv = pKbdClassDrv;


	DbgPrint("Hooked keyboard device %p", pKbdClassDevice);
	return STATUS_SUCCESS;
}

NTSTATUS unhookKeyboard(_In_ PDEVICE_OBJECT pKbdFnFltDevice, _In_ PDEVICE_OBJECT pKbdClassDevice){
	UNREFERENCED_PARAMETER(pKbdFnFltDevice);
	UNREFERENCED_PARAMETER(pKbdClassDevice);
	//NTSTATUS status = STATUS_UNSUCCESSFUL;
	//UNICODE_STRING uKbdClassDrv;
	//PDRIVER_OBJECT pKbdClassDrv = NULL;
	//PKBDFNFLT_DEVICE_EXTENSION pKbdFnFltExtension = (PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension;

	//RtlInitUnicodeString(&uKbdClassDrv, L"\\Driver\\kbdclass");
	//status = ObReferenceObjectByName(&uKbdClassDrv,
	//	OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
	//	NULL, 0, IoDriverObjectType, KernelMode, NULL, &pKbdClassDrv);
	//IoGetR
	//if (status)
	//	return status;
	//
	//if (!pKbdClassDrv->DeviceObject){
	//	ObfDereferenceObject(pKbdClassDrv);
	//	DbgPrint("no keyboard devices were found!");
	//	return STATUS_OBJECT_NAME_NOT_FOUND;
	//}
	//
	//PDEVICE_OBJECT pNextKbdDevice = pKbdClassDrv->DeviceObject;
	//while (pNextKbdDevice){
	//	Io
	//}


	//pKbdFnFltExtension->pKbdClassDrv = pKbdClassDrv;


	DbgPrint("Unhooked keyboard device %p", pKbdClassDevice);
	return STATUS_SUCCESS;
}



//unhookKeyboard;


NTSTATUS keyboardAddedOrRemoved(PVOID pNotification, PVOID pContext){
	OBJECT_ATTRIBUTES symlinkAttr;
	UNICODE_STRING uSymlinkTarget;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	HANDLE hSymlink = INVALID_HANDLE_VALUE;
	ULONG maxTargetNameLength = 0;
	PWCHAR pTargetNameBuf = NULL;
	
	PDEVICE_INTERFACE_CHANGE_NOTIFICATION pChngNotification = (PDEVICE_INTERFACE_CHANGE_NOTIFICATION)pNotification;
	PDEVICE_OBJECT pKbdFnFltDev = (PDEVICE_OBJECT)pContext;
	//PDEVICE_OBJECT pKbdClassDev = NULL;
	if (!pChngNotification){
		DbgPrint("pChngNotification = NULL!");
		return STATUS_INVALID_PARAMETER;
	}
	if (!pKbdFnFltDev){
		DbgPrint("pKbdFnFltDev = NULL!");
		return STATUS_INVALID_PARAMETER;
	}
	if (!pKbdFnFltDev->DeviceExtension){
		DbgPrint("pKbdFnFltDev->DeviceExtension = NULL!");
		return STATUS_DRIVER_INTERNAL_ERROR;
	}
	if (!pChngNotification->SymbolicLinkName){
		DbgPrint("pChngNotification->SymbolicLinkName = NULL!");
		return STATUS_DRIVER_INTERNAL_ERROR;
	}

	//DbgPrint("Symlink name address: %p", pChngNotification->SymbolicLinkName);
	//DbgPrint("Symlink name: %wZ", *pChngNotification->SymbolicLinkName);

	
	//DbgPrint("size:%u", pChngNotification->Size);
	//((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->workContext.pItem = ((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->pWorkitem;
	//((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->workContext.pSymlink = pChngNotification->SymbolicLinkName;
	//IoQueueWorkItem(((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->pWorkitem, testRoutine, DelayedWorkQueue, &((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->workContext);
	//PFILE_OBJECT pFileObj = NULL;
	//PDEVICE_OBJECT pDevObj = NULL;
	////FILE_READ_ATTRIBUTES
	//status = IoGetDeviceObjectPointer(pChngNotification->SymbolicLinkName, GENERIC_READ, &pFileObj, &pDevObj);
	//if (status)
	//	DbgPrint("failed to access devobj!%lX", status);
	//OBJECT_TYPE
	//POBJECT_SYMBOLIC_LINK
	//LARGE_INTEGER interval;
	//OBJECT_DIRECTORY_INFORMATION
	//interval.QuadPart = -50000000;
	//KeDelayExecutionThread(KernelMode, FALSE, &interval);
	//POBJECT_SYMBOLIC_LINK pTestObj;
	//status = ObReferenceObjectByName(pChngNotification->SymbolicLinkName,
	//	OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
	//	NULL, 0, NULL, KernelMode, NULL, &pTestObj);
	////IoGetR
	//if (status){
	//	DbgPrint("could not query object address%lX!", status);
	//	return status;
	//}
	//ObfDereferenceObject(pTestObj);
	
	InitializeObjectAttributes(&symlinkAttr, pChngNotification->SymbolicLinkName, OBJ_KERNEL_HANDLE, NULL, NULL);
	status = ZwOpenSymbolicLinkObject(&hSymlink, GENERIC_READ, &symlinkAttr);
	if (status){
		DbgPrint("Could not open symlink!%lX", status);
		return status;
	}
	//IoDriverObjectType
	
	//OBJECT_TYPE_LIST
	RtlInitUnicodeString(&uSymlinkTarget, L"\\");
	maxTargetNameLength = 0;
	status = ZwQuerySymbolicLinkObject(hSymlink, &uSymlinkTarget, &maxTargetNameLength);
	if ((STATUS_BUFFER_TOO_SMALL != status) && status){
		DbgPrint("Could not query symlink!%lX", status);
		ZwClose(hSymlink);
		return status;
	}
	
	pTargetNameBuf = ExAllocatePoolWithTag(NonPagedPool, maxTargetNameLength, g_poolTag);
	if (!pTargetNameBuf){
		DbgPrint("Could not allocate pool for target name!");
		ZwClose(hSymlink);
		return status;
	}
	
	RtlInitEmptyUnicodeString(&uSymlinkTarget, pTargetNameBuf, (USHORT)maxTargetNameLength);
	status = ZwQuerySymbolicLinkObject(hSymlink, &uSymlinkTarget, &maxTargetNameLength);
	if (status){
		DbgPrint("NTSTATUS %lX", status);
		ExFreePoolWithTag(pTargetNameBuf, g_poolTag);
		ZwClose(hSymlink);
		return status;
	}
	//FILE_READ
	DbgPrint("Device: %wZ", uSymlinkTarget);
	//IoInitializeWorkItem
	//PIO_WORKITEM
	//IoGetDeviceObjectPointer(&uSymlinkTarget, GENERIC_READ, )
	//KEYBOA
	//status = ObReferenceObjectByName(&uSymlinkTarget, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
	//	NULL, 0, IoDeviceObjectType, KernelMode, NULL, &pKbdClassDev);
	
	//ObReferenceObjectByName

	ExFreePoolWithTag(pTargetNameBuf, g_poolTag);
	ZwClose(hSymlink);
	
	//OBJECT_HEADER_NAME_INFO
	
	//if (status){
	//	DbgPrint("NTSTATUS %lX", status);
	//	return status;
	//}

	//ObfDereferenceObject(pKbdClassDev);

	////if (!status)
	////	//((PKBDFNFLT_DEVICE_EXTENSION)(pKbdFnFltDev->DeviceExtension))->pKeyboardDevice = pKbdClassDev;
	////	ObfDereferenceObject(pKbdClassDev);

	////DbgPrint("NTSTATUS %lX", status);
	////ExFreePoolWithTag(pTargetNameBuf, g_poolTag);
	////ZwClose(hSymlink);
	////	return status;
	////}

	//
	////ObQueryNameString
	//
	//if (sizeof(GUID) == RtlCompareMemory(&GUID_DEVICE_INTERFACE_ARRIVAL, &pChngNotification->Event, sizeof(GUID))){
	//	DbgPrint("Device with symlink \"%wZ\" was attached/enabled.", *pChngNotification->SymbolicLinkName);
	//	hookKeyboard((PDEVICE_OBJECT)pContext, (PDEVICE_OBJECT)pKbdClassDev);
	//}else if (sizeof(GUID) == RtlCompareMemory(&GUID_DEVICE_INTERFACE_REMOVAL, &pChngNotification->Event, sizeof(GUID))){
	//	DbgPrint("Device with symlink \"%wZ\" was removed.", *pChngNotification->SymbolicLinkName);
	//	unhookKeyboard((PDEVICE_OBJECT)pContext, (PDEVICE_OBJECT)pKbdClassDev);
	//}
	////else{
	////	DbgPrint("Neither was a device removed nor was one attached.");
	////}
	////IoAttachDevice
	///*
	//LARGE_INTEGER interval;
	//interval.QuadPart = -50000000;
	//
	//DbgPrint("device_name:%wZ", *pChngNotification->SymbolicLinkName);
	//KeDelayExecutionThread(KernelMode, FALSE, &interval);*/
	return STATUS_SUCCESS;
}

void mydrvUnload(IN PDRIVER_OBJECT pDriverObject){
	PKBDFNFLT_DEVICE_EXTENSION pKeyboardDeviceExtension = NULL;
	pKeyboardDeviceExtension = (PKBDFNFLT_DEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;
	DbgPrint("Driver Unload Called...\n");
	if (g_pNotificationEntry)
		IoUnregisterPlugPlayNotificationEx(g_pNotificationEntry);

	if (!pDriverObject->DeviceObject){
		DbgPrint("DevObj NULL!\n");
		return;
	}

	if (!pKeyboardDeviceExtension){
		DbgPrint("KBDFNFLT_DEVICE_EXTENSION NULL!\n");
		IoDeleteDevice(pDriverObject->DeviceObject);
		return;
		//		IoDetachDevice(pKeyboardDeviceExtension->pKeyboardDevice);
	}

	if (pKeyboardDeviceExtension->pWorkitem)
		IoUninitializeWorkItem(pKeyboardDeviceExtension->pWorkitem);

	if (pDriverObject->DeviceObject)
		IoDeleteDevice(pDriverObject->DeviceObject);

	

	////Get the pointer to the device extension   
	//if (pDriverObject->DeviceObject){
	//	pKeyboardDeviceExtension = (PKBDFNFLT_DEVICE_EXTENSION)pDriverObject->DeviceObject->DeviceExtension;



	//}

	//DbgPrint("Keyboard hook detached from device...\n");

	/////////////////////////////////////////////////////////////////   
	////Wait for our tagged IRPs to die before we remove the device   
	/////////////////////////////////////////////////////////////////   
	//DbgPrint("There are %d tagged IRPs\n", g_numPendingIrps);
	//DbgPrint("Waiting for tagged IRPs to die...\n");

	////Create a timer   
	//KTIMER kTimer;
	//LARGE_INTEGER  timeout;
	//timeout.QuadPart = -1000000; //.1 s   
	//KeInitializeTimer(&kTimer);

	//while (g_numPendingIrps > 0){
	//	//Set the timer   
	//	KeSetTimer(&kTimer, timeout, NULL);
	//	KeWaitForSingleObject(&kTimer, Executive, KernelMode, FALSE, NULL);
	//}

	//Delete the device   
	




	

	
	DbgPrint("Tagged IRPs dead...Terminating...\n");

	return;
}

NTSTATUS mydrvEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath){
	UNREFERENCED_PARAMETER(pRegPath);
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	DbgPrint("Keyboard Filter Driver - DriverEntry\nCompiled at " __TIME__ " on " __DATE__ "\n");
	//DbgPrint("sizeof(DEVICE_EXTENSION): %llu", sizeof(KBDFNFLT_DEVICE_EXTENSION));
	//DbgPrint("offset, size: %llu, %llu", FIELD_OFFSET(KBDFNFLT_DEVICE_EXTENSION, pKeyboardDevice), FIELD_SIZE(KBDFNFLT_DEVICE_EXTENSION, pKeyboardDevice));
	//DbgPrint("offset, size: %llu, %llu", FIELD_OFFSET(KBDFNFLT_DEVICE_EXTENSION, pKbdClassDrv), FIELD_SIZE(KBDFNFLT_DEVICE_EXTENSION, pKbdClassDrv));
	//DbgPrint("offset, size: %llu, %llu", FIELD_OFFSET(KBDFNFLT_DEVICE_EXTENSION, pDevObjString), FIELD_SIZE(KBDFNFLT_DEVICE_EXTENSION, pDevObjString));
	//DbgPrint("offset, size: %llu, %llu", FIELD_OFFSET(KBDFNFLT_DEVICE_EXTENSION, kState), FIELD_SIZE(KBDFNFLT_DEVICE_EXTENSION, kState));
	//DbgPrint("offset, size: %llu, %llu", FIELD_OFFSET(KBDFNFLT_DEVICE_EXTENSION, workitem), 0);//FIELD_SIZE(KBDFNFLT_DEVICE_EXTENSION, workitem));


	/////////////////////////////////////////////////////////////////////////////////////////   
	// Fill in IRP dispatch table in the DriverObject to handle I/O Request Packets (IRPs)    
	/////////////////////////////////////////////////////////////////////////////////////////   

	// For a filter driver, we want pass down ALL IRP_MJ_XX requests to the driver which   
	// we are hooking except for those we are interested in modifying.   
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
#pragma warning(suppress: 28168)
		pDrvObj->MajorFunction[i] = dispatchPassDown;
	DbgPrint("Filled dispatch table with generic pass down routine...\n");
	
	//Explicitly fill in the IRP's we want to hook   
	pDrvObj->MajorFunction[IRP_MJ_READ] = dispatchRead;

	//Create a keyboard device object  
	PDEVICE_OBJECT pKbdFnFltDevice = NULL;
	status = IoCreateDevice(pDrvObj, sizeof(KBDFNFLT_DEVICE_EXTENSION) + IoSizeofWorkItem(), NULL, //no name   
		FILE_DEVICE_KEYBOARD, 0, TRUE, &pKbdFnFltDevice);
	//Make sure the device was created ok   
	if (status)
		return status;

	GUID kbdDevClass = GUID_CLASS_KEYBOARD;
	PIO_WORKITEM pWorkitem = (PIO_WORKITEM)&((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->workitem;

	IoInitializeWorkItem(pKbdFnFltDevice, pWorkitem);
	((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->pWorkitem = pWorkitem;
	//PIO_WORKITEM pWorkitem = ExAllocatePoolWithTag(NonPagedPool, IoSizeofWorkItem(), g_poolTag);
	//if (!pWorkitem){
	//	
	//}
	DbgPrint("%p", &((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->pKeyboardDevice);
	DbgPrint("%p", &((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->pKbdClassDrv);
	DbgPrint("%p", &((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->pDevObjString);
	DbgPrint("%p", &((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->keyState);
	DbgPrint("%p", &((PKBDFNFLT_DEVICE_EXTENSION)pKbdFnFltDevice->DeviceExtension)->workitem);

	//IoInitializeWorkItem(&pKbdFnFltDevice)
	//PVOID pNotificationEntry = NULL;
	status = IoRegisterPlugPlayNotification(EventCategoryDeviceInterfaceChange,
		PNPNOTIFY_DEVICE_INTERFACE_INCLUDE_EXISTING_INTERFACES,
		&kbdDevClass, pDrvObj, keyboardAddedOrRemoved, pKbdFnFltDevice, &g_pNotificationEntry);
	if (status){
		DbgPrint("something went wrong    %lX", status);
		IoUninitializeWorkItem(pWorkitem);
		IoDeleteDevice(pKbdFnFltDevice);
		return status;
	}



	//Go ahead and hook the keyboard now   
	//status = hookKeyboard(pDrvObj);
	//if (status){
	//	DbgPrint("something went wrong%lX", status);
	//	return status;
	//}

	DbgPrint("Hooked IRP_MJ_READ routine...\n");

	// Set the DriverUnload procedure   
	pDrvObj->DriverUnload = mydrvUnload;
	DbgPrint("Set DriverUnload function pointer...\n");
	DbgPrint("Exiting Driver Entry......\n");
	return STATUS_SUCCESS;
}

