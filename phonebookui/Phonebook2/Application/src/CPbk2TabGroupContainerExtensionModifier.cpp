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
* Description:  Phonebook 2 Tab group container extension modifier.
*
*/


// INCLUDE FILES
#include "CPbk2TabGroupContainerExtensionModifier.h"

// Phonebook 2
#include <MPbk2AppUiExtension.h>

// Virtual Phonebook

// System includes


// --------------------------------------------------------------------------
// CPbk2TabGroupContainerExtensionModifier::
//      CPbk2TabGroupContainerExtensionModifier
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainerExtensionModifier::
    CPbk2TabGroupContainerExtensionModifier(
        MPbk2AppUiExtension& aAppUiExtension) :
    iAppUiExtension ( aAppUiExtension )
    {
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainerExtensionModifier::
//    ~CPbk2TabGroupContainerExtensionModifier
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainerExtensionModifier::
    ~CPbk2TabGroupContainerExtensionModifier()
    {
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainerExtensionModifier::NewL
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainerExtensionModifier* 
    CPbk2TabGroupContainerExtensionModifier::NewLC(
        MPbk2AppUiExtension& aAppUiExtension )
    {
    CPbk2TabGroupContainerExtensionModifier* self = 
        new ( ELeave ) CPbk2TabGroupContainerExtensionModifier( 
            aAppUiExtension );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainerExtensionModifier::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainerExtensionModifier::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2TabGroupContainerExtensionModifier::CreateViewNodeTabL
// --------------------------------------------------------------------------
//
void CPbk2TabGroupContainerExtensionModifier::CreateViewNodeTabL(
        const CPbk2ViewNode& aNode, 
        CAknTabGroup& aAknTabGroup,
        CPbk2IconInfoContainer& aTabIcons,
        TInt aViewCount )
    {
    iAppUiExtension.ApplyDynamicPluginTabGroupContainerChangesL(
            aNode, aAknTabGroup, aTabIcons, aViewCount);
    }

// End of File
