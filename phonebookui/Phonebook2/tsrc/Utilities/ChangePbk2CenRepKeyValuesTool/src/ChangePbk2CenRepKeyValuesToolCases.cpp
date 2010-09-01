/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include "ChangePbk2CenRepKeyValuesTool.h"
#include "stifunitmacros.h"
#include "centralrepository.h" 

const TUint32 KCRUidPhonebook = 0x101f8794;
const TUint32 KPhonebookAddFavoritiesVisibility	= 0x00000006;
const TInt KVisibilityAlwaysOFF =  0;
const TInt KVisibilityUntilFirstFavorite = 1;
const TInt KVisibilityAlwaysON = 2;

/**
* Enable Phonebook to be sent to the background up on exit.
*/
const TInt KVPbkLVAlwaysOnPhonebook = 0x00000020;

// ============================ MEMBER FUNCTIONS ===============================

void CChangePbk2CenRepKeyValuesTool::Setup()
    {
    STIF_LOG("Setup");
       
    // Read local variation flags
    iRepository = CRepository::NewL( TUid::Uid( KCRUidPhonebook ) );    
    }
    
void CChangePbk2CenRepKeyValuesTool::Teardown()
    {
    STIF_LOG("Teardown");
    
    delete iRepository;
    iRepository = NULL;
    }

// TESTCASE("SetTCPromotionAlwaysOFF", "CChangePbk2CenRepKeyValuesTool", Setup, SetTCPromotionAlwaysOFF, Teardown)
// TESTCASE("SetTCPromotionAlwaysON", "CChangePbk2CenRepKeyValuesTool", Setup, SetTCPromotionAlwaysON, Teardown)
// TESTCASE("SetTCPromotionTillFirstTimeUse", "CChangePbk2CenRepKeyValuesTool", Setup, SetTCPromotionTillFirstTimeUse, Teardown)
// TESTCASE("SetPbk2AlwaysON", "CChangePbk2CenRepKeyValuesTool", Setup, SetPbk2AlwaysON, Teardown)
// TESTCASE("SetPbk2AlwaysOFF", "CChangePbk2CenRepKeyValuesTool", Setup, SetPbk2AlwaysOFF, Teardown)

void CChangePbk2CenRepKeyValuesTool::SetTCPromotionAlwaysOFF()
    {    
    TInt res = iRepository->Set(KPhonebookAddFavoritiesVisibility, KVisibilityAlwaysOFF);    
    STIF_ASSERT(res == KErrNone);
    }

void CChangePbk2CenRepKeyValuesTool::SetTCPromotionAlwaysON()
    {
    TInt res = iRepository->Set(KPhonebookAddFavoritiesVisibility, KVisibilityAlwaysON);    
    STIF_ASSERT(res == KErrNone);    
    }

void CChangePbk2CenRepKeyValuesTool::SetTCPromotionTillFirstTimeUse()
    {
    TInt res = iRepository->Set(KPhonebookAddFavoritiesVisibility, KVisibilityUntilFirstFavorite);    
    STIF_ASSERT(res == KErrNone);    
    }
    
void CChangePbk2CenRepKeyValuesTool::SetPbk2AlwaysON()
    {
    SetLocalVariationFeatureL(KVPbkLVAlwaysOnPhonebook, ETrue); // ETrue: ON
    STIF_ASSERT(IsLocalVariationFeatureEnabledL(KVPbkLVAlwaysOnPhonebook));    
    }
    
void CChangePbk2CenRepKeyValuesTool::SetPbk2AlwaysOFF()
    {
    SetLocalVariationFeatureL(KVPbkLVAlwaysOnPhonebook, EFalse); // EFalse: OFF
    STIF_ASSERT(!IsLocalVariationFeatureEnabledL(KVPbkLVAlwaysOnPhonebook));
    }

//  [End of File] - do not remove
