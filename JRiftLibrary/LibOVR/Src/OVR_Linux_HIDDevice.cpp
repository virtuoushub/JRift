/************************************************************************************
Filename    :   OVR_Linux_HIDDevice.cpp
Content     :   Linux HID device implementation.
Created     :   February 26, 2013
Authors     :   Lee Cooper
 
Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.
 
Use of this software is subject to the terms of the Oculus license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

*************************************************************************************/

#include "OVR_Linux_HIDDevice.h"

namespace OVR { namespace Linux {

static const UInt32 MAX_QUEUED_INPUT_REPORTS = 5;
    
//-------------------------------------------------------------------------------------
// **** Linux::DeviceManager

HIDDeviceManager::HIDDeviceManager(DeviceManager* manager)
 :  DevManager(manager)
{
}

HIDDeviceManager::~HIDDeviceManager()
{
}

bool HIDDeviceManager::initializeManager()
{
	//TODO: initialize
	return true;
}
    
bool HIDDeviceManager::Initialize()
{
    return initializeManager();
}

void HIDDeviceManager::Shutdown()
{
    LogText("OVR::Linux::HIDDeviceManager - shutting down.\n");
}

bool HIDDeviceManager::Enumerate(HIDEnumerateVisitor* enumVisitor)
{
    if (!initializeManager())
    {
        return false;
    }
    

#if 0
	CFSetRef deviceSet = IOHIDManagerCopyDevices(HIDManager);
	CFIndex deviceCount = CFSetGetCount(deviceSet);
    
    // Allocate a block of memory and read the set into it.
    IOHIDDeviceRef* devices = (IOHIDDeviceRef*) OVR_ALLOC(sizeof(IOHIDDeviceRef) * deviceCount);
    CFSetGetValues(deviceSet, (const void **) devices);
    

    // Iterate over devices.
    for (CFIndex deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
    {
        IOHIDDeviceRef hidDev = devices[deviceIndex];
        
        if (!hidDev)
        {
            continue;
        }
        
        HIDDeviceDesc devDesc;
                
        if (getPath(hidDev, &(devDesc.Path)) &&
            initVendorProductVersion(hidDev, &devDesc) &&
            enumVisitor->MatchVendorProduct(devDesc.VendorId, devDesc.ProductId) &&
            initUsage(hidDev, &devDesc))
        {
            initStrings(hidDev, &devDesc);
            initSerialNumber(hidDev, &devDesc);

            // Construct minimal device that the visitor callback can get feature reports from.
            Linux::HIDDevice device(this, hidDev);
            
            enumVisitor->Visit(device, devDesc);
        }
    }
    
    OVR_FREE(devices);
    CFRelease(deviceSet);
#endif
    
    return true;
}

OVR::HIDDevice* HIDDeviceManager::Open(const String& path)
{

    Ptr<Linux::HIDDevice> device = *new Linux::HIDDevice(this);

    if (!device->HIDInitialize(path))
    {
        return NULL;
    }

    device->AddRef();
    
    return device;
}
    
#if 0
bool HIDDeviceManager::getFullDesc(IOHIDDeviceRef device, HIDDeviceDesc* desc)
{
        
    if (!initVendorProductVersion(device, desc))
    {
        return false;
    }
        
    if (!initUsage(device, desc))
    {
        return false;
    }
    
    if (!initSerialNumber(device, desc))
    {
        return false;
    }
    
    initStrings(device, desc);
        
    return true;
}
#endif


//-------------------------------------------------------------------------------------
// **** Linux::HIDDevice

HIDDevice::HIDDevice(HIDDeviceManager* manager)
 :  HIDManager(manager), InMinimalMode(false)
{
}

HIDDevice::~HIDDevice()
{
    if (!InMinimalMode)
    {
        HIDShutdown();
    }
}

bool HIDDevice::HIDInitialize(const String& path)
{

#if 0
    DevDesc.Path = path;

    if (!openDevice())
    {
        LogText("OVR::Linux::HIDDevice - Failed to open HIDDevice: %s", path.ToCStr());
        return false;
    }

    // Setup notification for when a device is unplugged and plugged back in.
    if (!setupDevicePluggedInNotification())
    {
        LogText("OVR::Linux::HIDDevice - Failed to setup notification for when device plugged back in.");
        closeDevice(false);
        return false;
    }
    
    HIDManager->DevManager->pThread->AddTicksNotifier(this);

    
    LogText("OVR::Linux::HIDDevice - Opened '%s'\n"
            "                    Manufacturer:'%s'  Product:'%s'  Serial#:'%s'\n",
            DevDesc.Path.ToCStr(),
            DevDesc.Manufacturer.ToCStr(), DevDesc.Product.ToCStr(),
            DevDesc.SerialNumber.ToCStr());
    
    return true;
#endif
	return false;
}

#if 0
bool HIDDevice::initInfo()
{
    // Device must have been successfully opened.
    OVR_ASSERT(Device);
    
    
    // Get report lengths.
    SInt32 bufferLength;
    bool getResult = HIDManager->getIntProperty(Device, CFSTR(kIOHIDMaxInputReportSizeKey), &bufferLength);
    OVR_ASSERT(getResult);
    InputReportBufferLength = (UInt16) bufferLength;

    getResult = HIDManager->getIntProperty(Device, CFSTR(kIOHIDMaxOutputReportSizeKey), &bufferLength);
    OVR_ASSERT(getResult);
    OutputReportBufferLength = (UInt16) bufferLength;

    getResult = HIDManager->getIntProperty(Device, CFSTR(kIOHIDMaxFeatureReportSizeKey), &bufferLength);
    OVR_ASSERT(getResult);
    FeatureReportBufferLength = (UInt16) bufferLength;
    
    
    if (ReadBufferSize < InputReportBufferLength)
    {
        OVR_ASSERT_LOG(false, ("Input report buffer length is bigger than read buffer."));
        return false;
    }
    
    // Get device desc.
    if (!HIDManager->getFullDesc(Device, &DevDesc))
    {
        OVR_ASSERT_LOG(false, ("Failed to get device desc while initializing device."));
        return false;
    }
    
    return true;
}

void HIDDevice::staticDeviceAddedCallback(void* pContext, io_iterator_t iterator)
{
    HIDDevice* pDevice = (HIDDevice*) pContext;
    pDevice->deviceAddedCallback(iterator);
}

void HIDDevice::deviceAddedCallback(io_iterator_t iterator)
{

    if (Device == NULL)
    {
        if (openDevice())
        {
            LogText("OVR::Linux::HIDDevice - Reopened device : %s", DevDesc.Path.ToCStr());
        }
    }

    // Reset callback.
    while (IOIteratorNext(iterator))
    {
    }
}
    
bool HIDDevice::openDevice()
{
    
    // Have to iterate through devices again to generate paths.
	CFSetRef deviceSet = IOHIDManagerCopyDevices(HIDManager->HIDManager);
	CFIndex deviceCount = CFSetGetCount(deviceSet);
    
    // Allocate a block of memory and read the set into it.
    IOHIDDeviceRef* devices = (IOHIDDeviceRef*) OVR_ALLOC(sizeof(IOHIDDeviceRef) * deviceCount);
    CFSetGetValues(deviceSet, (const void **) devices);
    
    
    // Iterate over devices.
    IOHIDDeviceRef device = NULL;

    for (CFIndex deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
    {
        IOHIDDeviceRef tmpDevice = devices[deviceIndex];
        
        if (!tmpDevice)
        {
            continue;
        }
        
        String path;
        if (!HIDManager->getPath(tmpDevice, &path))
        {
            continue;
        }
        
        if (path == DevDesc.Path)
        {
            device = tmpDevice;
            break;
        }
    }
    
    
    OVR_FREE(devices);
    
    if (!device)
    {
        CFRelease(deviceSet);
        return false;
    }
    
    // Attempt to open device.
    if (IOHIDDeviceOpen(device, kIOHIDOptionsTypeSeizeDevice)
        != kIOReturnSuccess)
    {
        CFRelease(deviceSet);
        return false;
    }

    // Retain the device before we release the set.
    CFRetain(device);
    CFRelease(deviceSet);
    
    
    Device = device;

    
    if (!initInfo())
    {
        IOHIDDeviceClose(Device, kIOHIDOptionsTypeSeizeDevice);
        CFRelease(Device);
        Device = NULL;
        return false;
    }
    
    
    // Setup the Run Loop and callbacks.
    IOHIDDeviceScheduleWithRunLoop(Device,
                                   HIDManager->getRunLoop(),
                                   kCFRunLoopDefaultMode);
    
    IOHIDDeviceRegisterInputReportCallback(Device,
                                           ReadBuffer,
                                           ReadBufferSize,
                                           staticHIDReportCallback,
                                           this);

    IOHIDDeviceRegisterRemovalCallback(Device,
                                       staticDeviceRemovedCallback,
                                       this);
    
    return true;
}
#endif
    
void HIDDevice::HIDShutdown()
{

    HIDManager->DevManager->pThread->RemoveTicksNotifier(this);
    
    LogText("OVR::Linux::HIDDevice - HIDShutdown '%s'\n", "TODO:GetPath" );
}

#if 0
bool HIDDevice::setupDevicePluggedInNotification()
{
    
    // Setup notification when devices are plugged in.
    RepluggedNotificationPort = IONotificationPortCreate(kIOMasterPortDefault);
    
    CFRunLoopSourceRef notificationRunLoopSource =
        IONotificationPortGetRunLoopSource(RepluggedNotificationPort);
    
    CFRunLoopAddSource(HIDManager->getRunLoop(),
                       notificationRunLoopSource,
                       kCFRunLoopDefaultMode);
    
    CFMutableDictionaryRef matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
    
    // Have to specify vendorId and productId. Doesn't seem to accept additional
    // things like serial number.
    SInt32 vendorId = DevDesc.VendorId;
    CFNumberRef numberRef = CFNumberCreate(kCFAllocatorDefault,
                                           kCFNumberSInt32Type,
                                           &vendorId);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBVendorID), numberRef);
    CFRelease(numberRef);
    
    SInt32 deviceProductId = DevDesc.ProductId;
    numberRef = CFNumberCreate(kCFAllocatorDefault,
                               kCFNumberSInt32Type,
                               &deviceProductId);
    CFDictionarySetValue(matchingDict, CFSTR(kUSBProductID), numberRef);
    CFRelease(numberRef);
    
    kern_return_t result =
            IOServiceAddMatchingNotification(RepluggedNotificationPort,
                                             kIOMatchedNotification,
                                             matchingDict,
                                             staticDeviceAddedCallback,
                                             this,
                                             &RepluggedNotification);
    
    if (result != KERN_SUCCESS)
    {
        CFRelease(RepluggedNotificationPort);
        return false;
    }
    
    // Iterate through to arm.
    while (IOIteratorNext(RepluggedNotification))
    {
	}
    
    return true;
}

void HIDDevice::closeDevice(bool wasUnplugged)
{
    OVR_ASSERT(Device != NULL);
    
    if (!wasUnplugged)
    {
        // Clear the registered callbacks.
        IOHIDDeviceRegisterInputReportCallback(Device,
                                               ReadBuffer,
                                               InputReportBufferLength,
                                               NULL,
                                               this);
        
        IOHIDDeviceRegisterRemovalCallback(Device, NULL, this);
        
        IOHIDDeviceUnscheduleFromRunLoop(Device,
                                         HIDManager->getRunLoop(),
                                         kCFRunLoopDefaultMode);
        IOHIDDeviceClose(Device, kIOHIDOptionsTypeNone);
    }
    
	CFRelease(Device);
    Device = NULL;
        
    LogText("OVR::Linux::HIDDevice - HID Device Closed '%s'\n", DevDesc.Path.ToCStr());
}

void HIDDevice::staticHIDReportCallback(void* pContext,
                                        IOReturn result,
                                        void* pSender,
                                        IOHIDReportType reportType,
                                        uint32_t reportId,
                                        uint8_t* pReport,
                                        CFIndex reportLength)
{
    HIDDevice* pDevice = (HIDDevice*) pContext;
    return pDevice->hidReportCallback(pReport, (UInt32)reportLength);
}

void HIDDevice::hidReportCallback(UByte* pData, UInt32 length)
{
    
    // We got data.
    if (Handler)
    {
        Handler->OnInputReport(pData, length);
    }
}
    
void HIDDevice::staticDeviceRemovedCallback(void* pContext, IOReturn result, void* pSender)
{
    HIDDevice* pDevice = (HIDDevice*) pContext;
    pDevice->deviceRemovedCallback();
}
    
void HIDDevice::deviceRemovedCallback()
{
    closeDevice(true);    
}

CFStringRef HIDDevice::generateRunLoopModeString(IOHIDDeviceRef device)
{
    const UInt32 safeBuffSize = 256;
    char nameBuff[safeBuffSize];
    OVR_sprintf(nameBuff, safeBuffSize, "%016lX", device);
  
    return CFStringCreateWithCString(NULL, nameBuff, kCFStringEncodingASCII);
}
#endif
    
bool HIDDevice::SetFeatureReport(UByte* data, UInt32 length)
{
    UByte reportID = data[0];

    if (reportID == 0)
    {
        // Not using reports so remove from data packet.
        data++;
        length--;
    }
	return false;
    
/*
	IOReturn result = IOHIDDeviceSetReport( Device,
                                            kIOHIDReportTypeFeature,
                                            reportID,
                                            data,
                                            length);
    
    return (result == kIOReturnSuccess);
	*/
}

bool HIDDevice::GetFeatureReport(UByte* data, UInt32 length)
{
	/*
    CFIndex bufferLength = length;
    
    // Report id is in first byte of the buffer.
	IOReturn result = IOHIDDeviceGetReport(Device, kIOHIDReportTypeFeature, data[0], data, &bufferLength);
	
    return (result == kIOReturnSuccess);
	*/
	return false;
}
   
UInt64 HIDDevice::OnTicks(UInt64 ticksMks)
{
    
    if (Handler)
    {
        return Handler->OnTicks(ticksMks);
    }
    
    return DeviceManagerThread::Notifier::OnTicks(ticksMks);
}

HIDDeviceManager* HIDDeviceManager::CreateInternal(Linux::DeviceManager* devManager)
{
        
    if (!System::IsInitialized())
    {
        // Use custom message, since Log is not yet installed.
        OVR_DEBUG_STATEMENT(Log::GetDefaultLog()->
                            LogMessage(Log_Debug, "HIDDeviceManager::Create failed - OVR::System not initialized"); );
        return 0;
    }

    Ptr<Linux::HIDDeviceManager> manager = *new Linux::HIDDeviceManager(devManager);

    if (manager)
    {
        if (manager->Initialize())
        {
            manager->AddRef();
        }
        else
        {
            manager.Clear();
        }
    }

    return manager.GetPtr();
}
    
} // namespace Linux

//-------------------------------------------------------------------------------------
// ***** Creation

// Creates a new HIDDeviceManager and initializes OVR.
HIDDeviceManager* HIDDeviceManager::Create()
{
    OVR_ASSERT_LOG(false, ("Standalone mode not implemented yet."));
    
    if (!System::IsInitialized())
    {
        // Use custom message, since Log is not yet installed.
        OVR_DEBUG_STATEMENT(Log::GetDefaultLog()->
            LogMessage(Log_Debug, "HIDDeviceManager::Create failed - OVR::System not initialized"); );
        return 0;
    }

    Ptr<Linux::HIDDeviceManager> manager = *new Linux::HIDDeviceManager(NULL);
    
    if (manager)
    {
        if (manager->Initialize())
        {
            manager->AddRef();
        }
        else
        {
            manager.Clear();
        }
    }

    return manager.GetPtr();
}

} // namespace OVR
