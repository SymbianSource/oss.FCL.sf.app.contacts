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
* Description:  Phonebook 2 contact assigner factory.
*
*/


// INCLUDE FILES
#include "Pbk2ContactAssignerFactory.h"

// Phonebook 2
#include "CPbk2ContactTextDataAssigner.h"
#include "CPbk2ContactImppDataAssigner.h"
#include "CPbk2ContactImageAssigner.h"
#include "CPbk2ContactRingingToneAssigner.h"
#include "CPbk2ContactEmptyDataAssigner.h"
#include "CPbk2ContactSpeedDialAttributeAssigner.h"
#include "CPbk2ContactSpeedDialAttributeUnassigner.h"
#include <Pbk2MimeTypeHandler.h>

// Virtual Phonebook
#include <VPbkPublicUid.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>

// --------------------------------------------------------------------------
// Pbk2ContactAssignerFactory::CreateContactDataAssignerL
// --------------------------------------------------------------------------
//
MPbk2ContactAssigner*
    Pbk2ContactAssignerFactory::CreateContactDataAssignerL
        ( MPbk2ContactAssignerObserver& aObserver,
          TInt aMimeType,
          const MVPbkFieldType* aFieldType,
          CVPbkContactManager& aContactManager,
          CPbk2FieldPropertyArray& aFieldProperties )
    {
    MPbk2ContactAssigner* assigner = NULL;

    if ( aMimeType == Pbk2MimeTypeHandler::EMimeTypeImage )
        {
        assigner = CPbk2ContactImageAssigner::NewL
            ( aObserver, aContactManager, aFieldProperties );
        }
    else if ( ( aMimeType == Pbk2MimeTypeHandler::EMimeTypeAudio )
            || ( aMimeType == Pbk2MimeTypeHandler::EMimeTypeVideo ) )
        {
        assigner = CPbk2ContactRingingToneAssigner::NewL( aObserver );
        }
    else if ( aFieldType )
        {
        TVPbkFieldVersitProperty versitProp;
        versitProp.SetName(EVPbkVersitNameIMPP);
        if (aFieldType->Matches(versitProp, 0))
            {
            assigner = CPbk2ContactImppDataAssigner::NewL( aObserver );
            }
        else
            {
            assigner = CPbk2ContactTextDataAssigner::NewL( aObserver );
            }
        }
    else
        {
        assigner = CPbk2ContactEmptyDataAssigner::NewL( aObserver );
        }

    return assigner;
    }

// --------------------------------------------------------------------------
// Pbk2ContactAssignerFactory::CreateContactAttributeAssignerL
// --------------------------------------------------------------------------
//
MPbk2ContactAssigner*
    Pbk2ContactAssignerFactory::CreateContactAttributeAssignerL
        ( MPbk2ContactAssignerObserver& aObserver,
          CVPbkContactManager& aContactManager,
          TPbk2AttributeAssignData aAttributeAssignData,
          TBool aAttributeRemoval )
    {
    MPbk2ContactAssigner* assigner = NULL;

    if ( aAttributeAssignData.iAttributeUid ==
         TUid::Uid( KVPbkSpeedDialAttributeImplementationUID ) )
        {
        if ( !aAttributeRemoval )
            {
            assigner = CPbk2ContactSpeedDialAttributeAssigner::NewL
                ( aObserver, aContactManager );
            }
        else
            {
            assigner = CPbk2ContactSpeedDialAttributeUnassigner::NewL
                ( aObserver, aContactManager );
            }
        }

    return assigner;
    }

// End of File
