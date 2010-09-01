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

// Virtual Phonebook
#include <CVPbkTopContactManager.h>

CPbk2NonTopContactSelector* CPbk2NonTopContactSelector::NewL()
    {
    return new (ELeave) CPbk2NonTopContactSelector();
    }

CPbk2NonTopContactSelector::CPbk2NonTopContactSelector()
: iContinue(ETrue)
    {
    }

CPbk2NonTopContactSelector::~CPbk2NonTopContactSelector()
    {
    }

TBool CPbk2NonTopContactSelector::IsContactIncluded(
        const MVPbkBaseContact& aContact )
    {
    TBool isContactIncluded( 
            !CVPbkTopContactManager::IsTopContact( aContact ) );
	
    // Continue until we found first contact that is not top contact
	// we can stop selector when we found first contact that is not top contact
	// because top contacts are sorted to top of the contact list.
    iContinue = !isContactIncluded;
    return isContactIncluded;
    
    }

TAny* CPbk2NonTopContactSelector::ContactSelectorExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KVPbkOptimizedSelectorExtensionUid )
        {
        return static_cast<MVPbkOptimizedSelector*>( this );
        }
    return NULL;
    }

TBool CPbk2NonTopContactSelector::Continue() const
    {
    return iContinue;
    }


// End of File
