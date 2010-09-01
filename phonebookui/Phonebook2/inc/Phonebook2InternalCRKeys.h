/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 internal central repository keys.
*
*/


#ifndef PHONEBOOK2INTERNALCRKEYS_H
#define PHONEBOOK2INTERNALCRKEYS_H

// --------------------------------------------------------------------------
// Phonebook Central Repository UIDs
// --------------------------------------------------------------------------
//
const TUint32 KCRUidPhonebookInternalConfig                 = 0x101f884a;
// value redefined and used also in CVPbkPhoneNumberMatchStrategy.cpp
const TUint32 KCRUidPhonebookStoreConfiguration             = 0x1020727f;

// --------------------------------------------------------------------------
// Phonebook Internal Configuration API
// --------------------------------------------------------------------------
//
const TUint32 KPhonebookNumberEditorMaxLength               = 0x00000001;
const TUint32 KPhonebookContactRelocationPolicy             = 0x00000002;

// --------------------------------------------------------------------------
// Phonebook 2 Store Configuration API
// --------------------------------------------------------------------------
//
const TUint32 KPhonebookDefaultSavingStoreUri               = 0x00000001;

/// Current configuration key range 0x00000100 to 0x000001ff
const TUint32 KPhonebookCurrentConfigurationFirstKey        = 0x00000100;
const TUint32 KPhonebookCurrentConfigurationLastKey         = 0x000001ff;
// value redefined and used also in CVPbkPhoneNumberMatchStrategy.cpp
const TUint32 KPhonebookCurrentConfigurationPartialKey      = 0x00000100;
// value redefined and used also in CVPbkPhoneNumberMatchStrategy.cpp
const TUint32 KPhonebookCurrentConfigurationMask            = 0xffffff00;

/// Possible configuration key range 0x00000200 to 0x000002ff
const TUint32 KPhonebookSupportedConfigurationFirstKey      = 0x00000200;
const TUint32 KPhonebookSupportedConfigurationLastKey       = 0x000002ff;
const TUint32 KPhonebookSupportedConfigurationPartialKey    = 0x00000200;
const TUint32 KPhonebookSupportedConfigurationMask          = 0xffffff00;

#endif // PHONEBOOK2INTERNALCRKEYS_H

// End of File
