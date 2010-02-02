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
* Description:  A factory for creating contact editor strategy
*
*/



// INCLUDE FILES
#include "Pbk2ContactEditorStrategyFactory.h"

#include <TPbk2ContactEditorParams.h>
#include "CPbk2ContactEditorNewContact.h"
#include "CPbk2ContactEditorEditContact.h"
#include "cpbk2contacteditornewowncontact.h"

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// Pbk2ContactEditorStrategyFactory::CreateL
// Creates a strategy according the editor flags. Creating new contact has
// a different startegy than editing contact
// --------------------------------------------------------------------------
//
MPbk2ContactEditorStrategy* Pbk2ContactEditorStrategyFactory::CreateL(
    TPbk2ContactEditorParams& aParams, CPbk2PresentationContact* aContact)
    {
    if (aParams.iFlags & TPbk2ContactEditorParams::ENewContact)
        {
        if( aParams.iFlags & TPbk2ContactEditorParams::EOwnContact )
            {
            return CPbk2ContactEditorNewOwnContact::NewL( aParams, aContact );
            }
        return CPbk2ContactEditorNewContact::NewL(aParams, aContact);
        }
    return CPbk2ContactEditorEditContact::NewL(aParams, aContact);
    }

//  End of File  
