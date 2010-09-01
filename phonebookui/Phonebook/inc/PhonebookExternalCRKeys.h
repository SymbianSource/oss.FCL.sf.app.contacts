/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  External Central Repository keys for Phonebook
*
*/


#ifndef __PhonebookExternalCRKeys_H__
#define __PhonebookExternalCRKeys_H__

#include <settingsinternalcrkeys.h> // includes KRCSEPreferredTelephony

/**
 * This file contains external CR keys used by Phonebook
 * defined as const TUint32. Direct use of them is not
 * possible since they are defined as TUid, and that is
 * initialized static data, which is forbidden to have
 * on ARM environment.
 */


///////////////////////////////////
// External Central Repository UIDs
//

// Rich call settings category from SettingsInternalCRKeys.h
const TUint32 KPbkCRUidRichCallSettings = 0x101f883b;


///////////////////////////////////
// External Central Repository keys
//


#endif // __PhonebookExternalCRKeys_H__
