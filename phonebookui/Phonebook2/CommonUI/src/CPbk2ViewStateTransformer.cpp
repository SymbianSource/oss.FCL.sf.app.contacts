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
* Description:  Phonebook 2 view state transformer.
*
*/



#include "CPbk2ViewStateTransformer.h"

// Phonebook 2
#include <CPbk2MigrationInterface.h>
#include <CPbk2ViewState.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>


/// Unused namespace for local definitions
namespace {

/**
 * Custom cleanup item.
 *
 * @param aObj  Pointer to item pushed on to the stack.
 */
void CleanupResetAndDestroy( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
        }
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ViewStateTransformer::CPbk2ViewStateTransformer
// --------------------------------------------------------------------------
//
CPbk2ViewStateTransformer::CPbk2ViewStateTransformer
        ( CVPbkContactManager& aContactManager ) :
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewStateTransformer::~CPbk2ViewStateTransformer
// --------------------------------------------------------------------------
//
CPbk2ViewStateTransformer::~CPbk2ViewStateTransformer()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewStateTransformer::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewStateTransformer* CPbk2ViewStateTransformer::NewLC(
        CVPbkContactManager& aContactManager)
    {
    CPbk2ViewStateTransformer* self =
        new( ELeave ) CPbk2ViewStateTransformer( aContactManager );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewStateTransformer::TransformLegacyViewStateToPbk2ViewStateLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState*
        CPbk2ViewStateTransformer::TransformLegacyViewStateToPbk2ViewStateLC
            ( const TDesC8& aCustomMessage )
    {
    // Function converts a Phonebook 1 view state
    // into Phonebook 2 view state
    CPbk2ViewState* pbk2ViewState = CPbk2ViewState::NewLC();

    // Check does the migration support implementation exist:
    RImplInfoPtrArray implementations;
    CleanupStack::PushL( TCleanupItem
        ( CleanupResetAndDestroy, &implementations ) );
    REComSession::ListImplementationsL(
            TUid::Uid( KPbk2MigrationSupportInterfaceUID ),
            implementations );

    CPbk2MigrationInterface* migrationSupport = NULL;
    if (implementations.Count() > 0)
        {
        // There should be only one implementation for migration
        // support component and we would like to use that one
        migrationSupport = CPbk2MigrationInterface::NewLC
            ( iContactManager );        
       
        // Fills pbk2ViewState with data from aCustomMessage
        migrationSupport->ConvertViewStateL
            ( aCustomMessage, *pbk2ViewState );

        // Destroy migration support instance
        CleanupStack::PopAndDestroy(); //migrationSupport
        }
    CleanupStack::PopAndDestroy(); // implementations

    return pbk2ViewState;
    }

// End of File
