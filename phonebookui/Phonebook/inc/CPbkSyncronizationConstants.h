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
* Description: 
*     Phonebook contact syncronization field constants.
*
*/


#ifndef __CPbkSyncronizationConstants_H__
#define __CPbkSyncronizationConstants_H__

//  INCLUDES


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * Phonebook Contact editor syncronization field content. 
 */

_LIT(KPbkContactSyncPrivate, "private");

_LIT(KPbkContactSyncPublic, "public");

_LIT(KPbkContactSyncNoSync, "none");

/**
 * Syncronization ordering.
 */
enum TPbkSyncronizationConf
    {
    EPbkSyncConfPrivate = 0,
    EPbkSyncConfPublic,
    EPbkSyncConfNoSync
    };


#endif // __CPbkSyncronizationConstants_H__
            
// End of File
