/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines the sim store properties
*
*/



#ifndef TVPBKSIMSTOREPROPERTY_H
#define TVPBKSIMSTOREPROPERTY_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

/// The default value for all signed store propery values
const TInt8 KVPbkSimStorePropertyUndefined = -1;

// DATA TYPES
namespace VPbkSimStoreImpl {
enum TVPbkSimStoreCaps
    {
    // Same as defined in etelmm.h
    KReadAccess			    = 0x20000000,
    KWriteAccess		    = 0x10000000,
    KDeleteAll			    = 0x08000000,
    KNotifyEvent		    = 0x04000000,
    KRestrictedWriteAccess  = 0x00000001,
    KSecondNameUsed         = 0x00000002,
    KAdditionalNumUsed      = 0x00000004,
    KGroupingUsed		    = 0x00000008,
    KEmailAddressUsed       = 0x00000020,
    KEntryControlUsed       = 0x00000010,
    };
} // namespace VPbkSimStoreImpl

/**
*  The basic store properties.
*
*/
struct TVPbkGsmStoreProperty
    {
    /// The maximum amount of contacts in the store
    TInt iTotalEntries;
    /// The current amount of contacts in the store
    TInt iUsedEntries;
    /// The maximum length of the GSM number field. This is a dynamic value
    /// and can change according to state of EXT1 file in the SIM card.
    TInt iMaxNumLength;
    /// The maximum lenght of the name field of the contact.
    TInt iMaxTextLength;
    /// A collection of flags in TVPbkSimStoreCaps
    TUint32 iCaps;
    };

/**
*  The 3G ADN properties. 
*  If the field information is not supported then the value is 
*  KVPbkSimStorePropertyUndefined
*
*/
struct TVPbkUSimStoreProperty
    {
    /// The maximum lenght of the email address.
    TInt16 iMaxEmailLength;
    /// The maximum length of the second name.
    TInt16 iMaxScndNameLength;
    /// The maximum length of the additional phone number. This is a dynamic
    /// value and can change according to state of EXT1 file in the SIM card.
    TInt16 iMaxAnrLength;
    /// The maximum length of the group name
    TInt16 iMaxGroupNameLength;
    /// The maximum number of email addresses in the contact. The value 
    /// doesn't change if the EMAIL file(s) becomes full in the SIM card
    TInt8 iMaxNumOfEmails;
    /// The maximum number of second names in the contact. The value doesn't
    /// change if the SNE file becomes full in the SIM card
    TInt8 iMaxNumOfScndNames;
    /// The maximum number of additional numbers in the contact. The value 
    /// doesn't change if the ANR file becomes full in the SIM card
    TInt8 iMaxNumOfAnrs;
    /// The maximum number of group names in the contact.
    TInt8 iMaxNumOfGroupNames;
    };

#endif      // TVPBKSIMSTOREPROPERTY_H
            
// End of File
