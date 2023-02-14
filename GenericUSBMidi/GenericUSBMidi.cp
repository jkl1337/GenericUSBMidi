/*	Copyright � 2007 Apple Inc. All Rights Reserved.
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by
 Apple Inc. ("Apple") in consideration of your agreement to the
 following terms, and your use, installation, modification or
 redistribution of this Apple software constitutes acceptance of these
 terms.  If you do not agree with these terms, please do not use,
 install, modify or redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc.
 may be used to endorse or promote products derived from the Apple
 Software without specific prior written permission from Apple.  Except
 as expressly stated in this notice, no other rights or licenses, express
 or implied, are granted by Apple herein, including but not limited to
 any patent rights that may be infringed by your derivative works or by
 other works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */
#include "GenericUSBMidi.h"

// Generated UUID: E472548B-A500-473B-BCE8-8571F98CD88C
#define kFactoryUUID CFUUIDGetConstantUUIDWithBytes(nullptr, 0xE4, 0x72, 0x54, 0x8B, 0xA5, 0x00, 0x47, 0x3B, 0xBC, 0xE8, 0x85, 0x71, 0xF9, 0x8C, 0xD8, 0x8C)

namespace {

struct DeviceEntry {
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t interface;
    uint16_t nPorts;
    const char *vendorName;
    const char *productName;
};

const char *kVendorRoland = "Roland";
const char *kVendorEdirol = "EDIROL";

const DeviceEntry deviceTable[] = {
    // vendor ID, product ID, interface number, number of ports/jacks, vendor name, product name
    { 0x0582, 0x0008, 2, 1, kVendorRoland, "PC-300"},
    { 0x0582, 0x000C, 2, 3, kVendorRoland, "SC-D70" },
//    { 0x0582, 0x0016, 2, 4, kVendorEdirol, "SD-90" },
    { 0x0582, 0x0003, 2, 6, kVendorRoland, "SC-8850" },
    // TODO: The SC-8820 does not have consecutive ports (Aux is port 4)
    { 0x0582, 0x0007, 2, 2, kVendorRoland, "SC-8820" },
    // TODO: The SD-20 only has 2 outputs
    { 0x0582, 0x0027, 0, 3, kVendorEdirol, "SD-20" },
    { 0x0582, 0x0029, 0, 4, kVendorEdirol, "SD-80" },
    { 0x0582, 0x0012, 0, 1, kVendorRoland, "XV-5050" },
    { 0x0582, 0x002D, 0, 1, kVendorRoland, "XV-2020" },
    { 0x0582, 0x002F, 0, 3, kVendorRoland, "VariOS" },
    { 0x0582, 0x0037, 0, 1, kVendorRoland, "Digital Piano" },
    { 0x0582, 0x0000, 2, 3, kVendorRoland, "UA-100" },
    { 0x0582, 0x0002, 2, 4, kVendorEdirol, "UM-4" },
    { 0x0582, 0x0014, 0, 9, kVendorEdirol, "UM-880" },
    { 0x0582, 0x001B, 2, 1, kVendorRoland, "MMP-2" },
    { 0x0582, 0x001D, 0, 1, kVendorRoland, "V-SYNTH" },
    { 0x0582, 0x0023, 0, 6, kVendorEdirol, "UM-550" },
    { 0x00, 0x00 }
};

const DeviceEntry* FindDeviceEntry(const IOUSBDeviceDescriptor *devDesc)
{
    const auto idVendor = USBToHostWord(devDesc->idVendor);
    const auto idProduct = USBToHostWord(devDesc->idProduct);
    
    for (const auto *devEntry = deviceTable; devEntry->idVendor; ++devEntry) {
        if (idVendor == devEntry->idVendor && idProduct == devEntry->idProduct)
            return devEntry;
    }
    return nullptr;
}

}

// Implementation of the factory function for this type.
extern "C" void *NewGenericUSBMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID);
extern "C" void *NewGenericUSBMidiDriver(CFAllocatorRef allocator, CFUUIDRef typeID)
{
	// If correct type is being requested, allocate an
	// instance of TestType and return the IUnknown interface.
	if (CFEqual(typeID, kMIDIDriverTypeID)) {
		GenericUSBMidiDriver *result = new GenericUSBMidiDriver;
		return result->Self();
	} else {
		// If the requested type is incorrect, return nullptr.
		return nullptr;
	}
}

// __________________________________________________________________________________________________

GenericUSBMidiDriver::GenericUSBMidiDriver() :
USBVendorMIDIDriver(kFactoryUUID)
{
}

GenericUSBMidiDriver::~GenericUSBMidiDriver()
{
}

// __________________________________________________________________________________________________

bool		GenericUSBMidiDriver::MatchDevice(USBDevice *		inUSBDevice)
{
    return FindDeviceEntry(inUSBDevice->GetDeviceDescriptor());
}

MIDIDeviceRef	GenericUSBMidiDriver::CreateDevice(USBDevice *		inUSBDevice,
                                                 USBInterface *	inUSBInterface)
{
    auto vendorName = CFSTR("Unknown");
    auto productName = CFSTR("Unknown");
    auto nPorts = 1;
    
    auto devEntry = FindDeviceEntry(inUSBDevice->GetDeviceDescriptor());
    if (devEntry) {
        vendorName = CFStringCreateWithCString(nullptr, devEntry->vendorName, kCFStringEncodingUTF8);
        productName = CFStringCreateWithCString(nullptr, devEntry->productName, kCFStringEncodingUTF8);
        nPorts = devEntry->nPorts;
    }

    MIDIDeviceRef dev;
    MIDIEntityRef ent;

    {
        auto deviceName = CFStringCreateWithFormat(nullptr, nullptr, CFSTR("%@ %@"), vendorName, productName);
        MIDIDeviceCreate(Self(), deviceName, vendorName, productName, &dev);
        CFRelease(deviceName);
    }

	// make entity for each port, with 1 source, 1 destination
	for (int port = 1; port <= nPorts; ++port) {
        CFStringRef str;
        if (nPorts > 1) {
            str = CFStringCreateWithFormat(nullptr, nullptr, CFSTR("Port %d"), port);
        } else {
            CFRetain(productName);
            str = productName;
        }
        MIDIDeviceAddEntity(dev, str, false, 1, 1, &ent);
		CFRelease(str);
	}
    CFRelease(productName);
    CFRelease(vendorName);
    
	return dev;
}

USBInterface *	GenericUSBMidiDriver::CreateInterface(USBMIDIDevice *device)
{
    auto devEntry = FindDeviceEntry(device->GetUSBDevice()->GetDeviceDescriptor());
    return devEntry ? device->mUSBDevice->FindInterface(devEntry->interface, 0) : nullptr;
}

void		GenericUSBMidiDriver::StartInterface(USBMIDIDevice *usbmDev)
{
}

void		GenericUSBMidiDriver::StopInterface(USBMIDIDevice *usbmDev)
{
}

void		GenericUSBMidiDriver::HandleInput(USBMIDIDevice *usbmDev, MIDITimeStamp when, Byte *readBuf, ByteCount readBufSize)
{
	USBMIDIHandleInput(usbmDev, when, readBuf, readBufSize);
}

UInt32	GenericUSBMidiDriver::PrepareOutput(USBMIDIDevice *usbmDev, WriteQueue &writeQueue,
                                              Byte *destBuf)
{
	auto n =
    USBMIDIPrepareOutput(usbmDev, writeQueue, destBuf, usbmDev->mOutPipe.mMaxPacketSize);
	if (n < usbmDev->mOutPipe.mMaxPacketSize) {
		memset(destBuf + n, 0, usbmDev->mOutPipe.mMaxPacketSize - n);
	}
	return usbmDev->mOutPipe.mMaxPacketSize;
}
