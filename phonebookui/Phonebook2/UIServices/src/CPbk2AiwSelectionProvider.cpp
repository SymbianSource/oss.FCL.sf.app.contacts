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
* Description:  Phonebook 2 selection service AIW provider.
*
*/


#include "CPbk2AiwSelectionProvider.h"

// Phonebook 2
#include "CPbk2ServerAppConnection.h"
#include <Pbk2IPCPackage.h>

// Virtual Phonebook
#include <RVPbkContactFieldDefaultPriorities.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <AiwContactSelectionDataTypes.h>
#include <s32mem.h>
#include <eikspane.h>
#include <akntitle.h>
#include <avkon.hrh>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Verifies whether the given command id is handled here.
 * This should be done by AIW.
 *
 * @param aCmdId    The command id to check.
 * @return  ETrue if command is handled by this provider.
 */
inline TBool IsCorrectAiwCmd( const TInt aCmdId )
    {
    TBool ret = EFalse;
    switch ( aCmdId )
        {
        case KAiwCmdSelect:
            {
            ret = ETrue;
            break;
            }

        default:
            {
            // Do nothing
            break;
            }
        }
    return ret;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::CPbk2AiwSelectionProvider
// --------------------------------------------------------------------------
//
CPbk2AiwSelectionProvider::CPbk2AiwSelectionProvider()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::CPbk2AiwSelectionProvider(0x%x)"), this );
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::~CPbk2AiwSelectionProvider
// --------------------------------------------------------------------------
//
CPbk2AiwSelectionProvider::~CPbk2AiwSelectionProvider()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::~CPbk2AiwSelectionProvider(0x%x)"), this );
    delete iConnection;
    delete iEventParamList;
    delete iConfigurationPackage;
    delete iInstructionsPackage;
    delete iContactViewFilterBuffer;
    delete iDefaultPrioritiesBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwSelectionProvider* CPbk2AiwSelectionProvider::NewL()
    {
    CPbk2AiwSelectionProvider* self =
        new ( ELeave ) CPbk2AiwSelectionProvider;
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::InitialiseL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::InitialiseL
        ( MAiwNotifyCallback& /*aFrameworkCallback*/,
          const RCriteriaArray& /*aInterest*/ )
    {
    // Nothing needs to be done
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::HandleServiceCmdL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::HandleServiceCmdL
        ( const TInt& aCmdId, const CAiwGenericParamList& aInParamList,
          CAiwGenericParamList& /*aOutParamList*/, TUint aCmdOptions /*= 0*/,
          const MAiwNotifyCallback* aCallback /* = NULL*/ )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::HandleServiceCmdL(0x%x)"), this );
   if ( IsCorrectAiwCmd( aCmdId ) )
        {
        if ( !aCallback )
            {
            // The callback is mandatory
            User::Leave( KErrArgument );
            }

        if ( !iConnection )
            {
            iConnection = CPbk2ServerAppConnection::NewL( *this );
            }
        else if( iConnection->IsActive() )
            {
            
            if ( aCmdOptions & KAiwOptCancel )
		            {
		            // Client wants to cancel existing service
		            iConnection->CancelFetch();
		            }
            
            // Request is already active for the same client, do nothing
            // as Phonebook supports only one request per client at a time.
            return;
            }    
            
        iCallback = aCallback;
        iInParamList = &aInParamList;
            
        if ( !iEventParamList )
            {
            iEventParamList = CAiwGenericParamList::NewL();
            }

        CreateInstructionPackageL( aInParamList );
        ValidateInstructionPackageL();
        CreateConfigurationPackageL( aInParamList );

        // Launch
        iConnection->LaunchFetchL
            ( iConfigurationPackage, iInstructionsPackage );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::OperationCompleteL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::OperationCompleteL
    ( const TDesC8& aContactLinks, TInt /*aExtraResultData*/,
      const TDesC& aField )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::OperationCompleteL(0x%x)"), this );
    iEventParamList->Reset();
    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamContactLinkArray,
            TAiwVariant( aContactLinks ) ) );

    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamContactFieldData,
            TAiwVariant( aField ) ) );

     if ( iCallback )
        {
        // Must cast this because of AIW bug
        const_cast<MAiwNotifyCallback*>( iCallback )->HandleNotifyL
            ( KAiwCmdSelect, KAiwEventCompleted, *iEventParamList,
              *iInParamList );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::OperationCanceledL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::OperationCanceledL()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::OperationCanceledL(0x%x)"), this );
    iEventParamList->Reset();

    // Must cast this because of AIW bug
    const_cast<MAiwNotifyCallback*>( iCallback )->HandleNotifyL
        ( KAiwCmdSelect, KAiwEventCanceled, *iEventParamList,
          *iInParamList );
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::OperationErrorL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::OperationErrorL( TInt aError )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::OperationErrorL(0x%x, %d)"), this, aError );
    iEventParamList->Reset();
    iEventParamList->AppendL(
            TAiwGenericParam( EGenericParamError,
                TAiwVariant( aError ) ) );

    // Must cast this because of AIW bug
    const_cast<MAiwNotifyCallback*>( iCallback )->HandleNotifyL
        ( KAiwCmdSelect, KAiwEventError, *iEventParamList,
          *iInParamList );
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwSelectionProvider::OkToExitL
        ( TInt /*aCommandId*/, TPbk2ExitCommandParams aExitParameter )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::OkToExitL(0x%x)"), this );
    TBool okToExit = ETrue;

    iEventParamList->Reset();
    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamApplication,
            TAiwVariant( aExitParameter ) ) );

    // Must cast this because of AIW design error
    okToExit = const_cast<MAiwNotifyCallback*>( iCallback )
        ->HandleNotifyL( KAiwCmdSelect, KAiwEventQueryExit,
            *iEventParamList, *iInParamList );

    return okToExit;
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::AcceptSelectionL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwSelectionProvider::AcceptSelectionL
        ( TInt aNumberOfSelectedContacts, HBufC8& /*aContactLink*/ )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
        "CPbk2AiwSelectionProvider::AcceptSelectionL(0x%x)"), this );
    iEventParamList->Reset();
    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamSelectedCount,
            TAiwVariant(aNumberOfSelectedContacts ) ) );
    TBool acceptOutput = const_cast<MAiwNotifyCallback*>(iCallback)->
            HandleNotifyL( KAiwCmdSelect, KAiwEventOutParamCheck,
                *iEventParamList, *iInParamList );

    return acceptOutput;
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::PackParameterToBufferL
// --------------------------------------------------------------------------
//
HBufC8* CPbk2AiwSelectionProvider::PackParameterToBufferL
        ( TGenericParamId aParamId,
          const CAiwGenericParamList& aInParamList) const
    {
    HBufC8* result = NULL;

    TInt paramIndex = 0;
    const TAiwGenericParam* paramData =
        aInParamList.FindFirst( paramIndex, aParamId );

    if ( paramData && paramData->Value().AsData().Ptr() )
        {
        result = paramData->Value().AsData().AllocL();
        }
    else if ( paramData && paramData->Value().AsDes().Ptr() )
        {
        // Data was given as 16-bit, transform it into 8-bit
        const TInt size( paramData->Value().AsDes().Size() );
        result = HBufC8::NewL( size );
        result->Des().Copy( paramData->Value().AsDes() );
        }

    return result;
    }


// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::CreateConfigurationPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::CreateConfigurationPackageL
        ( const CAiwGenericParamList& aInParamList )
    {
    // Store URIs
    HBufC8* storeUriBuffer = PackParameterToBufferL
        ( EGenericParamContactStoreUriArray, aInParamList );
    CleanupStack::PushL( storeUriBuffer );

    // Contact links
    HBufC8* linkBuffer = PackParameterToBufferL
        ( EGenericParamContactLinkArray, aInParamList );
    CleanupStack::PushL( linkBuffer );

    // Title pane
    HBufC* titlePaneText = ClientTitlePaneL();
    CleanupStack::PushL( titlePaneText );

    // Default priorities and contact view filter buffers are
    // allready created elsewhere

    TInt length = Pbk2IPCPackage::CountPackageSize( storeUriBuffer );
    length += Pbk2IPCPackage::CountPackageSize( linkBuffer );
    length += Pbk2IPCPackage::CountPackageSize( iDefaultPrioritiesBuffer );
    length += Pbk2IPCPackage::CountPackageSize( iContactViewFilterBuffer );
    length += Pbk2IPCPackage::CountPackageSize( titlePaneText );

    delete iConfigurationPackage;
    iConfigurationPackage = NULL;
    iConfigurationPackage = HBufC8::NewL( length );
    TPtr8 bufferPtr( iConfigurationPackage->Des() );

    RDesWriteStream writeStream( bufferPtr  );
    writeStream.PushL();
    Pbk2IPCPackage::ExternalizeL( storeUriBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( linkBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( iDefaultPrioritiesBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( iContactViewFilterBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( titlePaneText, writeStream );
    writeStream.CommitL();

    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( 3 ); // titlePaneText, linkBuffer,
                                      // storeUriBuffer
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::CreateInstructionPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::CreateInstructionPackageL
        ( const CAiwGenericParamList& aInParamList )
    {
    delete iInstructionsPackage;
    iInstructionsPackage = NULL;
    iInstructionsPackage = PackParameterToBufferL
        ( EGenericParamContactSelectionData, aInParamList );
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::ValidateInstructionPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwSelectionProvider::ValidateInstructionPackageL()
    {
    TInt version = KErrNotFound;
    RVPbkContactFieldDefaultPriorities* priorities = NULL;
    CVPbkFieldTypeSelector* viewFilter = NULL;

    if ( iInstructionsPackage )
        {
        // Version check
        version = TAiwContactSelectionDataBase::
            SelectionDataTypeFromBuffer( *iInstructionsPackage );
        if ( version == EAiwContactSelectionDataTypeNotDefined )
            {
            User::Leave( KErrArgument );
            }

        switch ( version )
            {
            case EAiwSingleItemSelectionV1:
                {
                TAiwSingleItemSelectionDataV1Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwSingleItemSelectionDataV1 ) );

                priorities = data().DefaultPriorities();
                break;
                }

            case EAiwSingleItemSelectionV2:
                {
                TAiwSingleItemSelectionDataV2Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwSingleItemSelectionDataV2 ) );

                priorities = data().DefaultPriorities();
                break;
                }
            case EAiwSingleItemSelectionV3:
                {
                TAiwSingleItemSelectionDataV3Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwSingleItemSelectionDataV3 ) );

                priorities = data().DefaultPriorities();
                viewFilter = data().FetchFilter();
                break;
                }
            case EAiwSingleEntrySelectionV1:
                {
                TAiwSingleEntrySelectionDataV1Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwSingleEntrySelectionDataV1 ) );
                break;
                }
            case EAiwSingleEntrySelectionV2:
                {
                TAiwSingleEntrySelectionDataV2Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwSingleEntrySelectionDataV2 ) );

                viewFilter = data().FetchFilter();
                break;
                }
            case EAiwMultipleEntrySelectionV1:
                {
                TAiwMultipleEntrySelectionDataV1Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwMultipleEntrySelectionDataV1 ) );
                break;
                }
            case EAiwMultipleEntrySelectionV2:
                {
                TAiwMultipleEntrySelectionDataV2Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwMultipleEntrySelectionDataV2 ) );

                viewFilter = data().FetchFilter();
                break;
                }
            case EAiwMultipleItemSelectionV1:
                {
                TAiwMultipleItemSelectionDataV1Pckg data;
                data.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwMultipleItemSelectionDataV1 ) );

                priorities = data().DefaultPriorities();
                viewFilter = data().FetchFilter();
                break;
                }
            default:
                {
                User::Leave( KErrArgument );
                break;
                }
            }
        }
    else
        {
        // Perform default fetch
        TAiwMultipleEntrySelectionDataV1 data;
        TAiwMultipleEntrySelectionDataV1Pckg dataPckg( data );
        iInstructionsPackage = dataPckg.AllocL();
        }

    delete iDefaultPrioritiesBuffer;
    iDefaultPrioritiesBuffer = NULL;
    if ( priorities )
        {
        iDefaultPrioritiesBuffer = priorities->ExternalizeLC();
        CleanupStack::Pop(); // iDefaultPrioritiesBuffer
        }

    delete iContactViewFilterBuffer;
    iContactViewFilterBuffer = NULL;
    if ( viewFilter )
        {
        iContactViewFilterBuffer = viewFilter->ExternalizeLC();
        CleanupStack::Pop(); // iContactViewFilterBuffer
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwSelectionProvider::ClientTitlePaneL
// --------------------------------------------------------------------------
//
HBufC* CPbk2AiwSelectionProvider::ClientTitlePaneL()
    {
    HBufC* result = NULL;

    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();

    if ( statusPane &&
         statusPane->PaneCapabilities(
            TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>( statusPane->
            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

        if ( titlePane->Text() )
            {
            result = titlePane->Text()->AllocL();
            }
        }

    return result;
    }

// End of File
