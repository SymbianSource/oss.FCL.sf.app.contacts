/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Cenrep keys for plugins actions
*
*/



#ifndef FSCACTIONPLUGINCRKEYS_H
#define FSCACTIONPLUGINCRKEYS_H

// Central Repository Uid
const TUid KCRUidFsContactActionService = { 0x20029F45 };

// Action priority Keys
const TUint32 KFscCrUidCallGsm              = 0x20029F37;
const TInt KFscActionPriorityCallGsm = 900;
const TUint32 KFscCrUidCallVoip             = 0x20029F38;
const TInt KFscActionPriorityCallVoip = 800;
const TUint32 KFscCrUidCallVideo            = 0x20029F39;
const TInt KFscActionPriorityCallVideo = 750;
const TUint32 KFscCrUidCallConfNum          = 0x20029F3A;
const TInt KFscActionPriorityCallConfNum = 900;
const TUint32 KFscCrUidConfWizard           = 0x20029F3B;
const TUint32 KFscCrUidCallPtt              = 0x20029F3C;
const TInt KFscActionPriorityCallPtt = 700;
const TUint32 KFscCrUidCreateMsg            = 0x20029F3D;
const TInt KFscActionPrioritySendMsg = 650;
const TUint32 KFscCrUidCreateAudMsg         = 0x20029F3E;
const TInt KFscActionPrioritySendAudio = 200;
const TUint32 KFscCrUidSaveAsContact        = 0x20029F3F;
const TInt KFscActionPrioritySaveAsContact = 950;
const TUint32 KFscCrUidEmail                = 0x20029F40;
const TInt KFscActionPrioritySendEmail = 550;
const TUint32 KFscCrUidPostcard             = 0x20029F41;
const TInt KFscActionPrioritySendPostcard = 475;
const TUint32 KFscCrUidMeetingRequest       = 0x20029F43;
const TInt KFscActionPrioritySendBusinesscard = 600;
const TUint32 KFscCrUidSendBusinesscard       = 0x20028717;

#endif      // FSCACTIONPLUGINCRKEYS_H
  
// End of File
