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
* Description:
*
*/


#include "CPbk2NonTopContactSelector.h"
#include <MVPbkBaseContact.h>
#include <featmgr.h>

// Virtual Phonebook
#include <CVPbkTopContactManager.h>

CPbk2NonTopContactSelector* CPbk2NonTopContactSelector::NewL()
    {
    CPbk2NonTopContactSelector* self = new (ELeave) CPbk2NonTopContactSelector();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CPbk2NonTopContactSelector::~CPbk2NonTopContactSelector()
    {
    }

TBool CPbk2NonTopContactSelector::IsContactIncluded(
        const MVPbkBaseContact& aContact )
    {
    TBool isContactIncluded( !CVPbkTopContactManager::IsTopContact( aContact ) );

    if( iMyCardSupported )
        {
        // this is temporary solution to hide own contact from phonebook contacts list,
        // TODO remove this code when we can hide own contact with contact model
    
        MVPbkBaseContact& contact = const_cast<MVPbkBaseContact&>( aContact );
        TAny* extension = contact.BaseContactExtension( 
                    KVPbkBaseContactExtension2Uid );
    
        if( isContactIncluded && extension )
            {
            MVPbkBaseContact2* baseContactExtension =
                    static_cast<MVPbkBaseContact2*>( extension );
            TInt error( KErrNone );
            isContactIncluded =
                    ( !baseContactExtension->IsOwnContact( error ) );
            }
        }
    
    return isContactIncluded;
    }


CPbk2NonTopContactSelector::CPbk2NonTopContactSelector()
    {
    }

void CPbk2NonTopContactSelector::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iMyCardSupported =
            FeatureManager::FeatureSupported( KFeatureIdffContactsMycard );    
    FeatureManager::UnInitializeLib();
    }

// End of File
