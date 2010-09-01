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
* Description:  Phonebook 2 application server assign service.
*
*/


#include "CPbk2AssignService.h"

// Phonebook 2
#include "CPbk2SingleContactDataAssigner.h"
#include "CPbk2MultiContactDataAssigner.h"
#include "CPbk2ContactAttributeAssigner.h"
#include "TPbk2ServerMessageDataRetriever.h"
#include "CPbk2ServerAppAppUi.h"
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactLinkArray.h>

// System includes
#include <AiwContactAssignDataTypes.h>

using namespace AiwContactAssign;


/// Unnamed namespace for local definitions
namespace {

_LIT(KPanicText, "CPbk2AssignService");

enum TPanicCode
    {
    EInvalidRequest,
    EInvalidAssignType
    };

// RMessage IPC-argument slot order
const TInt KResponseSlot = 0;
const TInt KPackedResultsSlot = 0;
const TInt KExtraResultsSlot = 1;
const TInt KResultSizeSlot = 0;
const TInt KAssignInstructionsSlot = 2;

const TInt KExitAcceptedSlot = 0;           // ok to exit protocol
const TInt KExitCommandIdSlot = 1;          // ok to exit protocol
const TInt KCompleteExitCommandIdSlot = 0;  // ok to exit protocol


// Assign types
enum TAssignType
    {
    EInvalidType,
    ESingleContactDataAssign,
    EMultipleContactDataAssign,
    EContactAttributeAssign,
    };

/**
 * Retrieves assign type from given message.
 *
 * @param aMessage  Message.
 * @return  Assign type.
 */
TAssignType AssignTypeL( const RMessage2& aMessage )
    {
    TAssignType result = EInvalidType;

    TAiwContactAssignDataBasePckg dataPckg;
    aMessage.ReadL( KAssignInstructionsSlot, dataPckg );

    switch ( TAiwContactAssignDataBase::AssignDataTypeFromBuffer
            ( dataPckg ) )
        {
        case EAiwSingleContactAssignV1:
            {
            result = ESingleContactDataAssign;

            // Perform additional check for multiple preselected contacts
            TPbk2ServerMessageDataRetriever dataRetriever;
            CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
                ( *CEikonEnv::Static()->EikAppUi() );
            MVPbkContactLinkArray* preselectedContacts =
                dataRetriever.GetPreselectedContactLinksL
                    ( aMessage,
                      appUi.ApplicationServices().ContactManager() );
            if ( preselectedContacts && preselectedContacts->Count() > 1 )
                {
                // Multiple preselected contacts found, switch to
                // multi assign mode
                result = EMultipleContactDataAssign;
                }
            delete preselectedContacts;
            break;
            }
        case EAiwMultipleContactAssignV1:
            {
            result = EMultipleContactDataAssign;
            break;
            }
        case EAiwContactAttributeAssignV1:
            {
            result = EContactAttributeAssign;
            break;
            }
        default:
            {
            aMessage.Panic( KPanicText, EInvalidAssignType );
            break;
            }
        }
    return result;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AssignService::CPbk2AssignService
// --------------------------------------------------------------------------
//
CPbk2AssignService::CPbk2AssignService( const RMessage2& aExitMsg ) :
        iExitMsg( aExitMsg )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::~CPbk2AssignService
// --------------------------------------------------------------------------
//
CPbk2AssignService::~CPbk2AssignService()
    {
    if ( !iAssignCompleteMsg.IsNull() )
        {
        iAssignCompleteMsg.Complete( KErrServerTerminated );
        }

    delete iUiService;
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignService* CPbk2AssignService::NewL( const RMessage2& aExitMsg )
    {
    CPbk2AssignService* self =
        new ( ELeave ) CPbk2AssignService( aExitMsg );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::LaunchServiceL( const RMessage2& aMessage )
    {
    if ( ( iAssignCompleteMsg.Handle() != KNullHandle ) &&
         ( iExitMsg.Handle() == KNullHandle ) )
        {
        aMessage.Panic( KPanicText, EInvalidRequest );
        }

    delete iUiService;
    iUiService = NULL;

    switch ( AssignTypeL( aMessage ) )
        {
        case ESingleContactDataAssign:
            {
            iUiService = CPbk2SingleContactDataAssigner::NewL
                ( aMessage, *this );
            break;
            }
        case EMultipleContactDataAssign:
            {
            iUiService = CPbk2MultiContactDataAssigner::NewL
                ( aMessage, *this );
            break;
            }
        case EContactAttributeAssign:
            {
            iUiService = CPbk2ContactAttributeAssigner::NewL
                ( aMessage, *this );
            break;
            }
        default:
            {
            aMessage.Panic( KPanicText, EInvalidRequest );
            break;
            }
        }

    iAssignCompleteMsg = aMessage;
    iUiService->LaunchServiceL();
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::CancelService
// --------------------------------------------------------------------------
//
void CPbk2AssignService::CancelService( const RMessage2& aMessage )
    {
    // If iAssignCompleteMsg is already completed do nothing
    if ( !iAssignCompleteMsg.IsNull() )
        {
        iUiService->CancelService();
        }

    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::GetResultsL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::GetResultsL( const RMessage2& aMessage )
    {
    if ( iUiService )
        {
        MPbk2UiService::TServiceResults results;
        iUiService->ServiceResults(&results);
        if ( results.iLinkArray )
            {
            HBufC8* packedResults = results.iLinkArray->PackLC();
            TPckg<TInt> extraData(results.iExtraData);
            TRAP_IGNORE(
                aMessage.WriteL(KPackedResultsSlot, *packedResults );
                aMessage.WriteL(KExtraResultsSlot, extraData );
            );
            CleanupStack::PopAndDestroy(); // packedResults
            }
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::GetResultSizeL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::GetResultSizeL( const RMessage2& aMessage )
    {
    if ( iUiService )
        {
        MPbk2UiService::TServiceResults results;
        iUiService->ServiceResults(&results);
        if ( results.iLinkArray )
            {
            HBufC8* packedResults = results.iLinkArray->PackLC();
            TPtr8 ptr = packedResults->Des();
            TPckg<TInt> buffer( ptr.Size() );
            TRAP_IGNORE( aMessage.WriteL(
                KResultSizeSlot, buffer ) );
            CleanupStack::PopAndDestroy(); // packedResults
            }
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::TryExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::TryExitServiceL( const RMessage2& aMessage )
    {
    TBool accepted = EFalse;
    TPckg<TBool> acceptedPkg( accepted );
    aMessage.ReadL( KExitAcceptedSlot, acceptedPkg );

    if ( iUiService && accepted )
        {
        TInt exitId = KErrNotFound;
        TPckg<TInt> exitPkg( exitId );
        aMessage.ReadL( KExitCommandIdSlot, exitPkg );

        iUiService->ExitServiceL( exitId );
        }

    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::TryAcceptServiceL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::TryAcceptServiceL( const RMessage2& aMessage )
    {
    // Accept protocol not supported in assign
    aMessage.Complete( KErrNotSupported );
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::ServiceComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignService::ServiceComplete()
    {
    if ( !iAssignCompleteMsg.IsNull() )
        {
        TPckg<TBool> complete( EFalse );
        TRAP_IGNORE( iAssignCompleteMsg.WriteL( KResponseSlot, complete ) );
        iAssignCompleteMsg.Complete( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::ServiceCanceled
// --------------------------------------------------------------------------
//
void CPbk2AssignService::ServiceCanceled()
    {
    if ( !iAssignCompleteMsg.IsNull() )
        {
        TPckg<TBool> cancel( ETrue );
        TRAP_IGNORE( iAssignCompleteMsg.WriteL( KResponseSlot, cancel ) );
        iAssignCompleteMsg.Complete( KErrCancel );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::ServiceAborted
// --------------------------------------------------------------------------
//
void CPbk2AssignService::ServiceAborted()
    {
    if ( !iAssignCompleteMsg.IsNull() )
        {
        iAssignCompleteMsg.Complete( KErrAbort );

        // There is nothing to do if ProcessCommandL leaves.
        // Of course it shouldn't leave in practice because it's
        // exit command.
        TRAP_IGNORE( ExitApplicationL() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::ServiceError
// --------------------------------------------------------------------------
//
void CPbk2AssignService::ServiceError( TInt aErrorCode )
    {
    if ( !iAssignCompleteMsg.IsNull() )
        {
        TPckg<TInt> error( aErrorCode );
        TRAP_IGNORE( iAssignCompleteMsg.WriteL( KResponseSlot, error ) );
        iAssignCompleteMsg.Complete( aErrorCode );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::CompleteExitMessage
// --------------------------------------------------------------------------
//
void CPbk2AssignService::CompleteExitMessage( TInt aExitCommandId )
    {
    TPckg<TInt> exitCommandId( aExitCommandId );
    if ( iUiService && iExitMsg.Handle() != KNullHandle )
        {
        TRAP_IGNORE( iExitMsg.WriteL(
            KCompleteExitCommandIdSlot, exitCommandId ) );
        iExitMsg.Complete( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::CompleteAcceptMsg
// --------------------------------------------------------------------------
//
void CPbk2AssignService::CompleteAcceptMsg
        ( const TDesC8& /*aMarkedEntries*/, const TDesC8& /*aLinkData*/ )
    {
    // Not supported in assign
    }

// --------------------------------------------------------------------------
// CPbk2AssignService::ExitApplicationL
// --------------------------------------------------------------------------
//
void CPbk2AssignService::ExitApplicationL()
    {
    // Exit application
    CEikonEnv* eikonEnv = CEikonEnv::Static();
    if ( eikonEnv )
        {
        CEikAppUi* appUi = eikonEnv->EikAppUi();
        MEikCommandObserver* cmdObs =
            static_cast<MEikCommandObserver*>( appUi );

        cmdObs->ProcessCommandL( EAknCmdExit );
        }
    }

// End of File
