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
* Description:  Phonebook 2 public publish and subscribe keys.
*
*/


#ifndef PHONEBOOK2PUBLICPSKEYS_H
#define PHONEBOOK2PUBLICPSKEYS_H

// --------------------------------------------------------------------------
// Phonebook Publish & Subscribe UIDs
// --------------------------------------------------------------------------

const TUint32 KPbk2PSUidPublic = 0x101F849C;

// --------------------------------------------------------------------------
// Phonebook System State API
// --------------------------------------------------------------------------
//
const TUint32 KPhonebookOpenCompleted = 0x00000001;

// Enumeration for KPhonebookOpenCompleted
enum TPhonebookOpenCompletedStatus
    {
    EPhonebookClosed,
    EPhonebookOpenCompleted
    };

#endif // PHONEBOOK2PUBLICPSKEYS_H

// End of File
