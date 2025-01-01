/*
 *  Released under "The GNU General Public License (GPL-2.0)"
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *  Based on iSightDefender concept by Stephen Checkoway (https://github.com/stevecheckoway/iSightDefender)
 */

#ifndef PCIDeviceStub_h
#define PCIDeviceStub_h

#ifdef DEBUG
#define DebugLog(args...) do { IOLog("FakePCIID: " args); } while (0)
#else
#define DebugLog(args...) do { } while (0)
#endif
#define AlwaysLog(args...) do { IOLog("FakePCIID: " args); } while (0)

#include <IOKit/pci/IOPCIDevice.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

// We want ioreg to still see the normal class hierarchy for hooked
// provider IOPCIDevice
//
// Normal:
//  IOPCIDevice : IOService : IORegistryEntry : OSObject
//
// Without this hack:
//  PCIDeviceStub : IOPCIDevice : IOService : IORegistryEntry : OSObject
//

#define hack_OSDefineMetaClassAndStructors(className, superclassName) \
    hack_OSDefineMetaClassAndStructorsWithInit(className, superclassName, )

#define hack_OSDefineMetaClassAndStructorsWithInit(className, superclassName, init) \
    hack_OSDefineMetaClassWithInit(className, superclassName, init) \
    OSDefineDefaultStructors(className, superclassName)

// trick played with getMetaClass to return IOPCIDevice::gMetaClass...
#define hack_OSDefineMetaClassWithInit(className, superclassName, init)       \
    /* Class global data */                                                   \
    className ::MetaClass className ::gMetaClass;                             \
    const OSMetaClass * const className ::metaClass =                         \
        & className ::gMetaClass;                                             \
    const OSMetaClass * const className ::superClass =                        \
        & superclassName ::gMetaClass;                                        \
    /* Class member functions */                                              \
    className :: className(const OSMetaClass *meta)                           \
        : superclassName (meta) { }                                           \
    className ::~ className() { }                                             \
    const OSMetaClass * className ::getMetaClass() const                      \
        { return &IOPCIDevice::gMetaClass; }                                  \
    /* The ::MetaClass constructor */                                         \
    className ::MetaClass::MetaClass()                                        \
        : OSMetaClass(#className, className::superClass, sizeof(className))   \
        { init; }


class FakePCIID;

//#define HOOK_ALL

class PCIDeviceStub : public IOPCIDevice
{
    OSDeclareDefaultStructors(PCIDeviceStub);
    typedef IOPCIDevice super;

protected:
    static int getIntegerProperty(IORegistryEntry* entry, const char* aKey, const char* alternateKey);
    
public:
    virtual UInt32 configRead32(IOPCIAddressSpace space, UInt8 offset);
    virtual UInt16 configRead16(IOPCIAddressSpace space, UInt8 offset);
    virtual UInt8 configRead8(IOPCIAddressSpace space, UInt8 offset);

#ifdef HOOK_ALL
    virtual void configWrite32(IOPCIAddressSpace space, UInt8 offset, UInt32 data);
    virtual void configWrite16(IOPCIAddressSpace space, UInt8 offset, UInt16 data);
    virtual void configWrite8(IOPCIAddressSpace space, UInt8 offset, UInt8 data);
    
    virtual UInt32 configRead32(UInt8 offset);
    virtual UInt16 configRead16(UInt8 offset);
    virtual UInt8 configRead8(UInt8 offset);
    
    UInt32 extendedConfigRead32(IOByteCount offset);
    UInt16 extendedConfigRead16(IOByteCount offset);
    UInt8 extendedConfigRead8(IOByteCount offset);
    
    virtual UInt32 ioRead32(UInt16 offset, IOMemoryMap * map = 0);
    virtual UInt16 ioRead16(UInt16 offset, IOMemoryMap * map = 0);
    virtual UInt8 ioRead8(UInt16 offset, IOMemoryMap * map = 0);
    
    virtual IODeviceMemory* getDeviceMemoryWithRegister(UInt8 reg);
    virtual IOMemoryMap* mapDeviceMemoryWithRegister(UInt8 reg, IOOptionBits options = 0);
    virtual IODeviceMemory* ioDeviceMemory(void);
    virtual UInt32 extendedFindPCICapability(UInt32 capabilityID, IOByteCount * offset = 0);
#endif
};

#endif
