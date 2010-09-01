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
* Description: 
*       Provides methods for predictive search settings item.
*
*/


// INCLUDE FILES
#include "CPbk2PredictiveSearchSettingItem.h"   // This class
#include "Phonebook2PrivateCRKeys.h"

#include <centralrepository.h>


// ================= MEMBER FUNCTIONS =======================

CPbk2PredictiveSearchSettingItem::CPbk2PredictiveSearchSettingItem(     
        TInt aResourceId, 
        TInt& aValue) : 
        CAknBinaryPopupSettingItem( aResourceId, aValue )
    {
    }

void CPbk2PredictiveSearchSettingItem::StoreL()
    {
    CAknEnumeratedTextPopupSettingItem::StoreL();
    TInt value = ExternalValue();
    SetPredictiveSearchModeL( value );
    }

void CPbk2PredictiveSearchSettingItem::SetPredictiveSearchModeL( TBool aPredictiveMode )
    {
    CRepository* rep = CRepository::NewL(TUid::Uid(KCRUidPhonebook));

    TInt err( rep->Set( KPhonebookPredictiveSearchEnablerFlags, aPredictiveMode ) );            
    if ( err )
        {
        rep->Create( KPhonebookPredictiveSearchEnablerFlags, aPredictiveMode );
        }
    
    delete rep;
    }

//  End of File
