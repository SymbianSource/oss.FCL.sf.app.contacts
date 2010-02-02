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
* Description:  Phonebook 2 application server fetch service.
*
*/


#include "CPbk2FetchService.h"

// Phonebook 2
#include "CPbk2EntryFetcher.h"
#include "CPbk2ItemFetcher.h"

// Virtual Phonebook
#include <MVPbkContactLinkArray.h>

// System includes
#include <AiwContactSelectionDataTypes.h>
#include <eikappui.h>
#include <eikenv.h>
#include <avkon.hrh>

// Debug
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

_LIT(KPanicText, "CPbk2FetchService");

enum TPanicCode
    {
    EInvalidRequest,
    EInvalidSelectionType
    };

// RMessage IPC-argument slot order
const TInt KResponseSlot = 0;
const TInt KPackedResultsSlot = 0;
const TInt KExtraResultsSlot = 1;
const TInt KFieldContentSlot = 2;
const TInt KResultArraySizeSlot = 0;
const TInt KResultFieldSizeSlot = 1;
const TInt KFetchInstructionsSlot = 1;

const TInt KAcceptServiceSlot = 0;          // acception protocol
const TInt KMarkedEntriesCountSlot = 0;     // acception protocol
const TInt KSelectedLinksSlot = 1;          // acception protocol
const TInt KSelectedContactSlot = 1;        // acception protocol

const TInt KExitAcceptedSlot = 0;           // ok to exit protocol
const TInt KExitCommandIdSlot = 1;          // ok to exit protocol
const TInt KCompleteExitCommandIdSlot = 0;  // ok to exit protocol

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FetchService::CPbk2FetchService
// --------------------------------------------------------------------------
//
CPbk2FetchService::CPbk2FetchService
        ( const RMessage2& aExitMsg, const RMessage2& aAcceptMsg ) :
            iExitMsg( aExitMsg), iAcceptMsg( aAcceptMsg )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::CPbk2FetchService()") );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::~CPbk2FetchService
// --------------------------------------------------------------------------
//
CPbk2FetchService::~CPbk2FetchService()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::~CPbk2FetchService()") );

    if ( !iFetchCompleteMsg.IsNull() )
        {
        iFetchCompleteMsg.Complete( KErrServerTerminated );
        }

    delete iUiService;
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchService* CPbk2FetchService::NewL
        ( const RMessage2& aExitMsg, const RMessage2& aAcceptMsg )
    {
    CPbk2FetchService* self =
            new ( ELeave ) CPbk2FetchService( aExitMsg, aAcceptMsg );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::LaunchServiceL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::LaunchServiceL( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::LaunchServiceL()") );

    if ( ( iFetchCompleteMsg.Handle() != KNullHandle ) &&
         ( iAcceptMsg.Handle() == KNullHandle ) )
        {
        aMessage.Panic( KPanicText(), EInvalidRequest );
        }

    TPbk2FetchType fetchType = FetchTypeL( aMessage );

    switch ( fetchType )
        {
        case EItemFetch:    // FALLTHROUGH
        case ECallItemFetch:
        case EMultipleItemFetch:
            {
            LaunchItemFetchL( aMessage, fetchType );
            break;
            }
        case EEntryFetch:   // FALLTHROUGH
        case EMultipleEntryFetch:
            {
            LaunchEntryFetchL( aMessage, fetchType );
            break;
            }
        default:
            {
            aMessage.Panic( KPanicText(), EInvalidRequest );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::CancelService
// --------------------------------------------------------------------------
//
void CPbk2FetchService::CancelService( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::CancelService()") );

    // If iFetchCompleteMsg is already completed do nothing.
    if ( !iFetchCompleteMsg.IsNull() )
        {
        iUiService->CancelService();
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::GetResultsL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::GetResultsL( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::GetResultsL()") );

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
        
        if ( results.iFieldContent != NULL )
            {
            TRAP_IGNORE(
                aMessage.WriteL( KFieldContentSlot, *results.iFieldContent );
            );
            }
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::GetResultSizeL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::GetResultSizeL( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::GetResultSizeL()") );

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
                KResultArraySizeSlot, buffer ) );
            CleanupStack::PopAndDestroy(); // packedResults
            }

        if ( results.iFieldContent != NULL )
            {
            TPckg<TInt> buffer( results.iFieldContent->Des().Length() );
            TRAP_IGNORE( aMessage.WriteL(
                KResultFieldSizeSlot, buffer ) );
            }
        else
            {
            TPckg<TInt> buffer( 0 );
            TRAP_IGNORE( aMessage.WriteL(
                KResultFieldSizeSlot, buffer ) );
            }
        }
    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::TryExitServiceL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::TryExitServiceL( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::TryExitServiceL()") );

    TBool okToExit = EFalse;
    TPckg<TBool> exitPkg( okToExit );
    aMessage.ReadL( KExitAcceptedSlot, exitPkg );

    if ( iUiService && okToExit )
        {
        TInt exitId = KErrNotFound;
        TPckg<TInt> commandIdPkg( exitId );
        aMessage.ReadL( KExitCommandIdSlot, commandIdPkg );


        iUiService->ExitServiceL( exitId );
        }

    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::TryAcceptServiceL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::TryAcceptServiceL( const RMessage2& aMessage )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::TryAcceptServiceL()") );

    TBool accepted = EFalse;
    TPckg<TBool> acceptedPkg( accepted );
    aMessage.ReadL( KAcceptServiceSlot, acceptedPkg );

    if ( iUiService && accepted )
        {
        HBufC8* buffer = HBufC8::NewLC(
            aMessage.GetDesMaxLengthL( KSelectedContactSlot ) );
        TPtr8 ptr = buffer->Des();
        aMessage.ReadL( KSelectedContactSlot, ptr );
        iUiService->AcceptDelayedContactsL( *buffer );
        CleanupStack::PopAndDestroy(); // buffer
        }

    aMessage.Complete( KErrNone );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::ServiceComplete
// --------------------------------------------------------------------------
//
void CPbk2FetchService::ServiceComplete()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::ServiceComplete()") );

    if ( !iFetchCompleteMsg.IsNull() )
        {
        TPckg<TBool> complete( EFalse );
        TRAP_IGNORE( iFetchCompleteMsg.WriteL( KResponseSlot, complete ) );
        iFetchCompleteMsg.Complete( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::ServiceCanceled
// --------------------------------------------------------------------------
//
void CPbk2FetchService::ServiceCanceled()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::ServiceCanceled()") );

    if ( !iFetchCompleteMsg.IsNull() )
        {
        TPckg<TBool> cancel( ETrue );
        TRAP_IGNORE( iFetchCompleteMsg.WriteL( KResponseSlot, cancel ) );
        iFetchCompleteMsg.Complete( KErrCancel );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::ServiceAborted
// --------------------------------------------------------------------------
//
void CPbk2FetchService::ServiceAborted()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::ServiceAborted()") );

    if ( !iFetchCompleteMsg.IsNull() )
        {
        iFetchCompleteMsg.Complete( KErrAbort );

        // Exit application
        CEikonEnv* eikonEnv = CEikonEnv::Static();
        if ( eikonEnv )
            {
            CEikAppUi* appUi = eikonEnv->EikAppUi();
            MEikCommandObserver* cmdObs =
                static_cast<MEikCommandObserver*>( appUi );

            // Dialog is closed so there is nothing to do if
            // ProcessCommandL leaves. Of course it shouldn't leave in
            // practice because it's exit command.
            TRAP_IGNORE( cmdObs->ProcessCommandL( EAknCmdExit ) );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::ServiceError
// --------------------------------------------------------------------------
//
void CPbk2FetchService::ServiceError( TInt aErrorCode )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::ServiceError(%d)"), aErrorCode );

    TPckg<TInt> error( aErrorCode );
    TRAP_IGNORE( iFetchCompleteMsg.WriteL( KResponseSlot, error ) );
    iFetchCompleteMsg.Complete( aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::CompleteExitMessage
// --------------------------------------------------------------------------
//
void CPbk2FetchService::CompleteExitMessage( TInt aExitCommandId )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::CompleteExitMessage()") );

    TPckg<TInt> exitCommandId( aExitCommandId );
    if ( iUiService && iExitMsg.Handle() != KNullHandle )
        {
        TRAP_IGNORE( iExitMsg.WriteL(
            KCompleteExitCommandIdSlot, exitCommandId ) );
        iExitMsg.Complete( KErrNone );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::CompleteAcceptMsg
// --------------------------------------------------------------------------
//
void CPbk2FetchService::CompleteAcceptMsg
        ( const TDesC8& aMarkedEntries, const TDesC8& aLinkData )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ("CPbk2FetchService::CompleteAcceptMsg()") );

    if ( iUiService && iAcceptMsg.Handle() != KNullHandle )
        {
        // Verify that we have enough room in the buffer
        TInt maxSize = 0;
        TRAP_IGNORE( 
            maxSize = iAcceptMsg.GetDesMaxLengthL( KSelectedLinksSlot ) );
        TInt bufferSize = aLinkData.Size();

        if ( bufferSize > maxSize )
            {
            iAcceptMsg.Complete( KErrOverflow );
            }
        else
            {
            TRAP_IGNORE(
                {
                iAcceptMsg.WriteL( KMarkedEntriesCountSlot, aMarkedEntries );
                iAcceptMsg.WriteL( KSelectedLinksSlot, aLinkData );                
                });
            iAcceptMsg.Complete( KErrNone );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::LaunchEntryFetchL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::LaunchEntryFetchL
        ( const RMessage2& aMessage, TPbk2FetchType aFetchType )
    {
    delete iUiService;
    iUiService = NULL;
    iUiService = CPbk2EntryFetcher::NewL( aMessage, *this, aFetchType );
    iUiService->LaunchServiceL();

    iFetchCompleteMsg = aMessage;
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::LaunchItemFetchL
// --------------------------------------------------------------------------
//
void CPbk2FetchService::LaunchItemFetchL
        ( const RMessage2& aMessage, TPbk2FetchType aFetchType )
    {
    delete iUiService;
    iUiService = NULL;
    iUiService = CPbk2ItemFetcher::NewL( aMessage, *this, aFetchType );
    iUiService->LaunchServiceL();

    iFetchCompleteMsg = aMessage;
    }

// --------------------------------------------------------------------------
// CPbk2FetchService::FetchTypeL
// --------------------------------------------------------------------------
//
TPbk2FetchType CPbk2FetchService::FetchTypeL( const RMessage2& aMessage )
    {
    TPbk2FetchType result = EInvalidType;

    TInt length = aMessage.GetDesLengthL( KFetchInstructionsSlot );
    if ( length > 0 )
        {
        HBufC8* dataBuf = HBufC8::NewLC( length );
        TPtr8 ptr = dataBuf->Des();
        aMessage.ReadL( KFetchInstructionsSlot, ptr );

        TAiwContactSelectionDataType dataType =
            TAiwContactSelectionDataBase::SelectionDataTypeFromBuffer( ptr );

        switch ( dataType )
            {
            case EAiwSingleEntrySelectionV1:        // FALLTHROUGH
            case EAiwSingleEntrySelectionV2:
                {
                result = EEntryFetch;
                break;
                }
            case EAiwMultipleEntrySelectionV1:      // FALLTHROUGH
            case EAiwMultipleEntrySelectionV2:
                {
                result = EMultipleEntryFetch;
                break;
                }
            case EAiwSingleItemSelectionV1:
                {
                TAiwSingleItemSelectionDataV1Pckg data;
                data.Copy( ptr );
                result = EItemFetch;

                TAiwAddressSelectType addressSelectType =
                    data().AddressSelectType();
                if ( addressSelectType == EAiwCallItemSelect
                     || addressSelectType == EAiwVoIPItemSelect )
                    {
                    result = ECallItemFetch;
                    }
                break;
                }
            case EAiwSingleItemSelectionV2:
                {
                result = EItemFetch;
                break;
                }
            case EAiwSingleItemSelectionV3:
                {
                TAiwSingleItemSelectionDataV3Pckg data;
                data.Copy( ptr );
                result = EItemFetch;

                TAiwAddressSelectType addressSelectType =
                    data().AddressSelectType();
                if ( addressSelectType == EAiwCallItemSelect
                     || addressSelectType == EAiwVoIPItemSelect )
                    {
                    result = ECallItemFetch;
                    }
                break;
                }
            case EAiwMultipleItemSelectionV1:
                {
                result = EMultipleItemFetch;
                break;
                }
            default:
                {
                aMessage.Panic( KPanicText, EInvalidSelectionType );
                break;
                }
            }
        CleanupStack::PopAndDestroy( dataBuf );
        }
    else
        {
        aMessage.Panic( KPanicText, EInvalidSelectionType );
        }

    return result;
    }

// End of File
