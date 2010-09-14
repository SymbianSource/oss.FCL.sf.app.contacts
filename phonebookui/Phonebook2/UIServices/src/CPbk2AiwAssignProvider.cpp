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
* Description:  Phonebook 2 Assign UI service AIW provider.
*
*/


#include "CPbk2AiwAssignProvider.h"

// Phonebook 2
#include "CPbk2ServerAppConnection.h"
#include "Pbk2UIServices.hrh"
#include <Pbk2IPCPackage.h>
#include <pbk2uiservicesres.rsg>
#include <pbk2uicontrols.rsg>
#include <Pbk2DataCaging.hrh>
#include <Pbk2MimeTypeHandler.h>
#include <Pbk2InternalUID.h>

// Virtual Phonebook
#include <VPbkPublicUid.h>

// System includes
#include <AiwMenu.h>
#include <AiwContactAssignDataTypes.h>
#include <s32mem.h>
#include <eikspane.h>
#include <akntitle.h>
#include <avkon.hrh>
#include <featmgr.h>
#include <aknViewAppUi.h>

using namespace AiwContactAssign;

/// Unnamed namespace for local definitions
namespace {

_LIT(KPbk2UiServicesDllResFileName, "Pbk2UiServicesRes.rsc");
_LIT(KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc");

#ifdef _DEBUG

enum TPanicCode
    {
    ENullPointer,
    EInvalidArgument,
    EDataTypeNotSupported
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2AiwAssignProvider" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG


/**
 * Verifies whether the given command id is handled here.
 * This should be done by AIW.
 *
 * @param aCmdId    The command id to check.
 * @return  ETrue if command is handled by this provider
 */
inline TBool IsCorrectAiwCmd( const TInt aCmdId )
    {
    TBool ret = EFalse;
    switch ( aCmdId )
        {
        case KAiwCmdAssign:                     // FALLTHROUGH
        case EPbk2AiwCmdAssign:                 // FALLTHROUGH
        case EPbk2AiwCmdDataSaveCreateNew:      // FALLTHROUGH
        case EPbk2AiwCmdDataSaveAddToExisting:
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

/**
 * Finds a AIW semantic id from the AIW in param list.
 *
 * @param aInParamList    AIW in param list.
 * @param aSemanticId     The semantic id to look for.
 * @return  ETrue if found, EFalse otherwise.
 */
inline TBool FindSemanticId
        ( const CAiwGenericParamList& aInParamList,
          const TGenericParamId aSemanticId )
    {
    TBool ret = EFalse;

    TInt index = 0;
    if ( aInParamList.FindFirst( index, aSemanticId ) )
        {
        ret = ETrue;
        }

    return ret;
    }

/**
 * Validates that param's data is valid
 *
 * @param aInParamList  The data passed by the consumer.
 * @param aSemanticId   The param which data to be validate.
 * @return ETrue if data is valid, EFalse otherwise
 */
inline TBool ValidateData
        ( const CAiwGenericParamList& aInParamList, 
          TGenericParamId aSemanticId )
    {
    TBool result = ETrue;
    
    // EGenericParamMIMEType param data validation
    if ( EGenericParamMIMEType == aSemanticId )
        {
        TInt index = 0;

        const TAiwGenericParam* mimeTypeParam =
            aInParamList.FindFirst( index, EGenericParamMIMEType,
                EVariantTypeDesC );

        TInt error = KErrNone;
        TInt mimeType = Pbk2MimeTypeHandler::EMimeTypeNotSupported;
        if ( mimeTypeParam )
            {            
            TRAP( error,
                {                
                if ( mimeTypeParam->Value().AsData().Ptr() )
                    {
                    // 8-bit
                    mimeType = Pbk2MimeTypeHandler::MapMimeTypeL
                        ( mimeTypeParam->Value().AsData() );
                    }
                else if ( mimeTypeParam->Value().AsDes().Ptr() )
                    {
                    // 16-bit
                    mimeType = Pbk2MimeTypeHandler::MapMimeTypeL
                        ( mimeTypeParam->Value().AsDes() );
                    }
                } ); // TRAP
            }
        
        // No MIME type found if leaves with KErrNotFound.
        // Not supported type if mimeType is not found.
        if ( error == KErrNotFound  || 
             Pbk2MimeTypeHandler::EMimeTypeNotSupported == mimeType )
            {
            result = EFalse;
            }            
        }
        
    return result;
    }

/**
 * Validates that consumer passed all required data.
 * Checks also if required param EGenericParamMIMEType data is valid.
 * If invalid data is given leave KErrArgument will occur.
 *
 * @param aInParamList  The data passed by the consumer.
 * @return  EGenericParamUnspecified if no valid data is found,
 *          AIW semantic id of found data otherwise.
 */
inline TGenericParamId ValidateParams
        ( const CAiwGenericParamList& aInParamList )
    {
    TGenericParamId ret = EGenericParamUnspecified;

    if ( FindSemanticId( aInParamList, EGenericParamPhoneNumber ) )
        {
        ret = EGenericParamPhoneNumber;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamEmailAddress ) )
        {
        ret = EGenericParamEmailAddress;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamSIPAddress ) )
        {
        ret = EGenericParamSIPAddress;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamURL ) )
        {
        ret = EGenericParamURL;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamWVID ) )
        {
        ret = EGenericParamWVID;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamSpeedDialIndex ) )
        {
        ret = EGenericParamSpeedDialIndex;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamXSpId ) )
        {
        ret = EGenericParamXSpId;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamContactItem ) )
        {
        ret = EGenericParamContactItem;
        }
    if ( ret == EGenericParamUnspecified )
        {
        // The in param list must have a MIME type with the data and file name
        if ( FindSemanticId( aInParamList, EGenericParamMIMEType ) && 
             ValidateData( aInParamList, EGenericParamMIMEType ) )
            {            
            if ( FindSemanticId( aInParamList, EGenericParamFile ) )
                {
                // In this case, return EGenericParamFile
                ret = EGenericParamFile;
                }
            }
        }

    return ret;
    }
    
/**
 * Selects correct address select selector matching the passed parameters.
 *
 * @param aInParamList  Parameter list.
 * @return  Selector resource id.
 */
TInt SelectSelectorResourceIdFromParams
        ( const CAiwGenericParamList& aInParamList )
    {
    TInt resourceId = KErrNotFound;

    if ( FindSemanticId( aInParamList, EGenericParamPhoneNumber ) )
        {
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            resourceId = R_PBK2_AIW_PHONENUMBER_SELECTOR_VOIP;
            }
        else
            {
            resourceId = R_PBK2_AIW_PHONENUMBER_SELECTOR;
            }
        }
    else if ( FindSemanticId( aInParamList, EGenericParamEmailAddress ) )
        {
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            resourceId = R_PBK2_AIW_EMAIL_SELECTOR_VOIP;
            }
        else
            {
            resourceId = R_PBK2_AIW_EMAIL_SELECTOR;
            }
        }
    else if ( FindSemanticId( aInParamList, EGenericParamSIPAddress ) )
        {
        if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
            {
            resourceId = R_PBK2_AIW_SIP_SELECTOR_VOIP;
            }
        else
            {
            resourceId = R_PBK2_AIW_SIP_SELECTOR;
            }
        }
    else if ( FindSemanticId( aInParamList, EGenericParamURL ) )
        {
        resourceId = R_PBK2_AIW_URL_SELECTOR;
        }
    else if ( FindSemanticId( aInParamList, EGenericParamWVID ) )
        {
        // Not supported yet
        }
    else if ( FindSemanticId( aInParamList, EGenericParamDateTime ) )
        {
        // Not supported yet
        }
    else if ( FindSemanticId( aInParamList, EGenericParamXSpId ) )
        {
        resourceId = R_PBK2_AIW_IMPP_SELECTOR;
        }
    else
        {
        TInt index = 0;

        // Lets then try MIME type param
        const TAiwGenericParam* mimeTypeParam =
            aInParamList.FindFirst( index, EGenericParamMIMEType,
                EVariantTypeDesC );

        if ( mimeTypeParam )
            {
            // If MIME type is specified, then the selector resource
            // depends on the type of MIME
            TInt mimeType( 0 );
            if ( mimeTypeParam->Value().AsData().Ptr() )
                {
                // 8-bit
                mimeType = Pbk2MimeTypeHandler::MapMimeTypeL
                    ( mimeTypeParam->Value().AsData() );
                }
            else if ( mimeTypeParam->Value().AsDes().Ptr() )
                {
                // 16-bit
                mimeType = Pbk2MimeTypeHandler::MapMimeTypeL
                    ( mimeTypeParam->Value().AsDes() );
                }

            switch ( mimeType )
                {
                case Pbk2MimeTypeHandler::EMimeTypeImage:
                    {
                    resourceId = R_PBK2_AIW_THUMBNAIL_SELECTOR;
                    break;
                    }
                case Pbk2MimeTypeHandler::EMimeTypeAudio:   // FALLTHROUGH
                case Pbk2MimeTypeHandler::EMimeTypeVideo:
                    {
                    resourceId = R_PBK2_AIW_RINGTONE_SELECTOR;
                    break;
                    }
                default:
                    {
                    // Do nothing
                    break;
                    }
                }
            }
        }

    return resourceId;
    }

/**
 * Selects correct contact view filter selector matching
 * the passed parameters.
 *
 * @param aInParamList  Parameter list.
 * @return  Selector resource id.
 */
TInt SelectFilterResourceIdFromParams
        ( const CAiwGenericParamList& aInParamList )
    {
    TInt resourceId = KErrNotFound;

    if ( FindSemanticId( aInParamList, EGenericParamSpeedDialIndex ) )
        {
        resourceId = R_PHONEBOOK2_PHONENUMBER_SELECTOR;
        }

    return resourceId;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::CPbk2AiwAssignProvider
// --------------------------------------------------------------------------
//
CPbk2AiwAssignProvider::CPbk2AiwAssignProvider()
        : iUiServicesResourceFile( *CCoeEnv::Static() )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::~CPbk2AiwAssignProvider
// --------------------------------------------------------------------------
//
CPbk2AiwAssignProvider::~CPbk2AiwAssignProvider()
    {
    delete iConnection;
    delete iEventParamList;
    delete iDataPackage;
    delete iConfigurationPackage;
    delete iInstructionsPackage;
    iUiServicesResourceFile.Close();
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::NewL
// --------------------------------------------------------------------------
//
CPbk2AiwAssignProvider* CPbk2AiwAssignProvider::NewL()
    {
    CPbk2AiwAssignProvider* self = new ( ELeave)  CPbk2AiwAssignProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::ConstructL()
    {
    iUiServicesResourceFile.OpenL(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPbk2UiServicesDllResFileName);
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::InitializeMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::InitializeMenuPaneL
        ( CAiwMenuPane& aMenuPane, TInt aIndex,
          TInt /*aCascadeId*/, const CAiwGenericParamList& aInParamList)
    {
    TAiwGenericParam param = ValidateParams(aInParamList);
    if (param != EGenericParamUnspecified)
        {
        // Initialize with the default menu
        TInt menuResourceId = R_PBK2_AIW_ASSIGN;

        CreateInstructionPackageL( aInParamList );

        if (iInstructionsPackage)
            {
            TInt version = TAiwContactAssignDataBase::
                AssignDataTypeFromBuffer( *iInstructionsPackage );
            if (version == EAiwMultipleContactAssignV1)
                {
                // Multi assign has always just one menu option,
                // and it has already been set
                }
            else if (version == EAiwSingleContactAssignV1)
                {
                // Single assign defaults to R_PBK2_AIW_ADD_TO_CONTACTS,
                // but if there is a raised EUseSimpleMenu flag
                // then we show the R_PBK2_AIW_ASSIGN menu
                TAiwSingleContactAssignDataV1Pckg data;
                data.Copy( *iInstructionsPackage );
                TUint assignFlags = data().Flags();
                if ( !( assignFlags & EUseSimpleMenu ) )
                    {
                    if (param == EGenericParamXSpId)
                        {
                        menuResourceId = R_PBK2_AIW_ADD_TO_CONTACTS_XSP;
                        }
                    else
                        {
                        menuResourceId = R_PBK2_AIW_ADD_TO_CONTACTS;
                        }
                    }
                }
            else
                {
                User::Leave( KErrArgument );
                }
            }

        // Show the menu
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC
            (reader, menuResourceId);
        aMenuPane.AddMenuItemsL(reader, KAiwCmdAssign, aIndex);
        CleanupStack::PopAndDestroy(); // reader
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::HandleMenuCmdL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::HandleMenuCmdL
        ( TInt aMenuCmdId, const CAiwGenericParamList& aInParamList,
          CAiwGenericParamList& aOutParamList,
          TUint aCmdOptions, const MAiwNotifyCallback* aCallback )
    {
    // Route to HandleServiceCmdL
    HandleServiceCmdL( aMenuCmdId, aInParamList,
        aOutParamList, aCmdOptions, aCallback );
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::InitialiseL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::InitialiseL
        ( MAiwNotifyCallback& /*aFrameworkCallback*/,
          const RCriteriaArray& /*aInterest*/ )
    {
    // Nothing needs to be done
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::HandleServiceCmdL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::HandleServiceCmdL
        ( const TInt& aCmdId, const CAiwGenericParamList& aInParamList,
        CAiwGenericParamList& /*aOutParamList*/,
        TUint aCmdOptions, const MAiwNotifyCallback* aCallback )
    {
    if ( IsCorrectAiwCmd( aCmdId ) )
        {
        if ( !iConnection )
            {
            iConnection = CPbk2ServerAppConnection::NewL( *this );
            }
        else if( iConnection->IsActive() )
            {
	            	if ( aCmdOptions & KAiwOptCancel )
								{
								// Client wants to cancel existing service
								iConnection->CancelAssign();
								}
            
            // Request is already active for the same client, do nothing
            // as Phonebook supports only one request per client at a time.
            return;
            }
            
        if ( !iEventParamList )
            {
            iEventParamList = CAiwGenericParamList::NewL();
            }

        TGenericParamId semanticId = ValidateParams( aInParamList );
        iCallback = aCallback;

        // Create IPC packages
        CreateInstructionPackageL( aInParamList );
        ValidateInstructionPackageL();
        ValidateMenuServiceL( aCmdId );
        CreateConfigurationPackageL( aInParamList );

        if ( semanticId == EGenericParamSpeedDialIndex )
            {
            LaunchAttributeAssignL( aInParamList, semanticId );
            }
        else
            {
            LaunchDataAssignL( aInParamList, semanticId );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::OperationCompleteL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::OperationCompleteL
        ( const TDesC8& aContactLinks, TInt aExtraResultData,
          const TDesC& /*aField*/ )
    {
    iEventParamList->Reset();
    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamContactLinkArray,
            TAiwVariant( aContactLinks ) ) );

    // aExtraResultData contains index of focused field from editor
    // this is used in cca details view.
    // todo: would it be better to add a new aiw semantic id instead of EGenericParamContactItem?
    iEventParamList->AppendL(
        TAiwGenericParam( EGenericParamContactItem,
            TAiwVariant( aExtraResultData )));

    if (iCallback)
        {
        // Must cast this because of AIW design error
        const_cast<MAiwNotifyCallback*>(iCallback)->HandleNotifyL
            ( KAiwCmdAssign, KAiwEventCompleted, *iEventParamList,
              *iInParamList );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::OperationCanceledL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::OperationCanceledL()
    {
    iEventParamList->Reset();
    if (iCallback)
        {
        // Must cast this because of AIW design error
        const_cast<MAiwNotifyCallback*>(iCallback)->HandleNotifyL
            ( KAiwCmdAssign, KAiwEventCanceled, *iEventParamList,
              *iInParamList );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::OperationErrorL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::OperationErrorL( TInt aError )
    {
    iEventParamList->Reset();
    iEventParamList->AppendL(
            TAiwGenericParam( EGenericParamError,
                TAiwVariant( aError ) ) );

     if ( iCallback )
        {
        // Must cast this because of AIW design error
        const_cast<MAiwNotifyCallback*>( iCallback )->HandleNotifyL
            ( KAiwCmdAssign, KAiwEventError, *iEventParamList,
              *iInParamList );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwAssignProvider::OkToExitL
        ( TInt /*aCommandId*/, TPbk2ExitCommandParams aExitParameter )
    {
    TBool okToExit = ETrue;

    if ( iCallback )
        {
        iEventParamList->Reset();
        iEventParamList->AppendL(
            TAiwGenericParam( EGenericParamApplication,
                TAiwVariant( aExitParameter ) ) );

        // Must cast this because of AIW design error
        okToExit = const_cast<MAiwNotifyCallback*>( iCallback )
            ->HandleNotifyL( KAiwCmdAssign, KAiwEventQueryExit,
                *iEventParamList, *iInParamList );
        }

    return okToExit;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::AcceptSelectionL
// --------------------------------------------------------------------------
//
TBool CPbk2AiwAssignProvider::AcceptSelectionL
        ( TInt /*aNumberOfSelectedContacts*/, HBufC8& /*aContactLink*/ )
    {
    // Assign service does not contain support
    // for verification of the selection
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::PackParameterToBufferL
// --------------------------------------------------------------------------
//
HBufC8* CPbk2AiwAssignProvider::PackParameterToBufferL
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
// CPbk2AiwAssignProvider::PackParameterToBuffer16L
// --------------------------------------------------------------------------
//
HBufC16* CPbk2AiwAssignProvider::PackParameterToBuffer16L
        ( TGenericParamId aParamId,
        const CAiwGenericParamList& aInParamList ) const
    {
    HBufC16* result = NULL;

    TInt paramIndex = 0;
    const TAiwGenericParam* paramData =
        aInParamList.FindFirst(paramIndex, aParamId);

    if ( paramData && paramData->Value().AsData().Ptr() )
        {
        // Data was given as 8-bit, transform it into 16-bit
        const TInt size( paramData->Value().AsData().Size() );
        result = HBufC::NewL( size );
        result->Des().Copy( paramData->Value().AsData() );
        }
    else if ( paramData && paramData->Value().AsDes().Ptr() )
        {
        result = paramData->Value().AsDes().AllocL();
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::PackSelectorToBufferL
// --------------------------------------------------------------------------
//
HBufC8* CPbk2AiwAssignProvider::PackSelectorToBufferL
        ( const CAiwGenericParamList& aInParamList ) const
    {
    HBufC8* result = NULL;

    TInt resourceId = SelectSelectorResourceIdFromParams( aInParamList );
    if ( resourceId != KErrNotFound )
        {
        // Construct a resource buffer pointed to
        // VPBK_FIELD_TYPE_SELECTOR resource, which will
        // contain the suitable fields for the passed data
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );

        RPbk2LocalizedResourceFile resFile =
            RPbk2LocalizedResourceFile( &fs );
        resFile.OpenLC( KPbk2RomFileDrive,
            KDC_RESOURCE_FILES_DIR, KPbk2UiServicesDllResFileName );
        result = resFile.AllocReadLC( resourceId );
        CleanupStack::Pop(); // result
        CleanupStack::PopAndDestroy(2); // resFile, fs
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::PackFilterToBufferL
// --------------------------------------------------------------------------
//
HBufC8* CPbk2AiwAssignProvider::PackFilterToBufferL
        ( const CAiwGenericParamList& aInParamList ) const
    {
    HBufC8* result = NULL;

    TInt resourceId = SelectFilterResourceIdFromParams( aInParamList );
    if ( resourceId != KErrNotFound )
        {
        // Construct a resource buffer pointed to
        // VPBK_FIELD_TYPE_SELECTOR resource, which will
        // contain the suitable fields for the passed data
        RFs fs;
        User::LeaveIfError( fs.Connect() );
        CleanupClosePushL( fs );

        RPbk2LocalizedResourceFile resFile =
            RPbk2LocalizedResourceFile( &fs );
        resFile.OpenLC( KPbk2RomFileDrive,
            KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName );
        result = resFile.AllocReadLC( resourceId );
        CleanupStack::Pop(); // result
        CleanupStack::PopAndDestroy(2); // resFile, fs
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::CreateDataPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::CreateDataPackageL
        ( const CAiwGenericParamList& aInParamList,
          const TGenericParamId aSemanticId )
    {
    // MIME type
    HBufC8* mimeTypeBuffer = PackParameterToBufferL
        ( EGenericParamMIMEType, aInParamList );
    CleanupStack::PushL( mimeTypeBuffer );

    // The data to assign, if any
    HBufC16* dataBuffer = NULL;
    HBufC16* indexBuffer = NULL;
    if ( aSemanticId == EGenericParamXSpId)
        {
        dataBuffer = PackImppParametersToBufferL(aInParamList);
        }
    else if ( aSemanticId == EGenericParamContactItem )
        {
        // TODO: get index from aInParamList AsTInt32()
        indexBuffer = PackParameterToBuffer16L( aSemanticId, aInParamList );
        }
    else if ( aSemanticId != EGenericParamUnspecified )
        {
        dataBuffer = PackParameterToBuffer16L( aSemanticId, aInParamList );
        }
    CleanupStack::PushL( dataBuffer );
    CleanupStack::PushL( indexBuffer );

    TInt length = Pbk2IPCPackage::CountPackageSize( mimeTypeBuffer );
    length += Pbk2IPCPackage::CountPackageSize( dataBuffer );
    length += Pbk2IPCPackage::CountPackageSize( indexBuffer );

    delete iDataPackage;
    iDataPackage = NULL;
    iDataPackage = HBufC8::NewL( length );
    TPtr8 bufferPtr( iDataPackage->Des() );

    RDesWriteStream writeStream( bufferPtr  );
    writeStream.PushL();
    Pbk2IPCPackage::ExternalizeL( mimeTypeBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( dataBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( indexBuffer, writeStream );
    writeStream.CommitL();

    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( 3 ); // dataBuffer, mimeTypeBuffer, indexbuffer
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::CreateAttributePackageL
// --------------------------------------------------------------------------
//
TPbk2AttributeAssignData CPbk2AiwAssignProvider::CreateAttributePackageL
        ( const CAiwGenericParamList& aInParamList,
          const TGenericParamId aSemanticId )
    {
    TPbk2AttributeAssignData ret;
    ret.iAttributeUid = TUid::Uid( KEPOCNullUID );
    ret.iAttributeValue = KErrNotFound;

    switch ( aSemanticId )
        {
        case EGenericParamSpeedDialIndex:
            {
            TInt speedDialIndex = KErrNotFound;
            TInt paramIndex = 0;

            const TAiwGenericParam* paramData =
                aInParamList.FindFirst( paramIndex, aSemanticId );

            if ( paramData )
                {
                speedDialIndex = paramData->Value().AsTInt32();
                }

            ret.iAttributeUid =
                TUid::Uid( KVPbkSpeedDialAttributeImplementationUID );
            ret.iAttributeValue = speedDialIndex;
            break;
            }

        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EInvalidArgument ) );
            break;
            }
        };

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::CreateConfigurationPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::CreateConfigurationPackageL
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

    // Field type selector
    HBufC8* selectorBuffer = PackSelectorToBufferL( aInParamList );
    CleanupStack::PushL( selectorBuffer );

    // Contact view filter
    HBufC8* viewFilterBuffer = PackFilterToBufferL( aInParamList );
    CleanupStack::PushL( viewFilterBuffer );

    // Title pane
    HBufC* titlePaneText = ClientTitlePaneL();
    CleanupStack::PushL( titlePaneText );

    HBufC* orientationType = ClientOrientation();
    CleanupStack::PushL( orientationType );
    TInt length = Pbk2IPCPackage::CountPackageSize( storeUriBuffer );
    length += Pbk2IPCPackage::CountPackageSize( linkBuffer );
    length += Pbk2IPCPackage::CountPackageSize( selectorBuffer );
    length += Pbk2IPCPackage::CountPackageSize( viewFilterBuffer );
    length += Pbk2IPCPackage::CountPackageSize( titlePaneText );
    length += Pbk2IPCPackage::CountPackageSize( orientationType );

    delete iConfigurationPackage;
    iConfigurationPackage = NULL;
    iConfigurationPackage = HBufC8::NewL( length );
    TPtr8 bufferPtr( iConfigurationPackage->Des() );

    RDesWriteStream writeStream( bufferPtr  );
    writeStream.PushL();
    Pbk2IPCPackage::ExternalizeL( storeUriBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( linkBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( selectorBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( viewFilterBuffer, writeStream );
    Pbk2IPCPackage::ExternalizeL( titlePaneText, writeStream );
    Pbk2IPCPackage::ExternalizeL( orientationType, writeStream );
    writeStream.CommitL();

    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( 6 ); // orientationType, titlePaneText,
                                      // viewFilterBuffer, selectorBuffer, 
                                      // linkBuffer, storeUriBuffer 
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::CreateInstructionPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::CreateInstructionPackageL
        ( const CAiwGenericParamList& aInParamList )
    {
    delete iInstructionsPackage;
    iInstructionsPackage = NULL;
    iInstructionsPackage = PackParameterToBufferL
        ( EGenericParamContactAssignData, aInParamList );
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::ValidateInstructionPackageL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::ValidateInstructionPackageL()
    {
    if ( iInstructionsPackage )
        {
        // Version check
        TInt version = TAiwContactAssignDataBase::
            AssignDataTypeFromBuffer( *iInstructionsPackage );
        if ( version == EAiwContactAssignDataTypeNotDefined )
            {
            User::Leave( KErrArgument );
            }

        switch( version )
            {
            case EAiwSingleContactAssignV1:
                {
                // Nothing to validate since for example editor can be
                // on/off and similarly with new contact creation both
                // choises on/off are supported
                break;
                }

            case EAiwMultipleContactAssignV1:
                {
                TAiwMultipleContactAssignDataV1Pckg dataPckg;

                // Reconstruct the instructions
                dataPckg.Copy( iInstructionsPackage->Ptr(),
                        sizeof( TAiwMultipleContactAssignDataV1 ) );

                // Multi assign does not support new contact
                // creation nor does it support contact editor,
                // so disable those
                TUint flags = dataPckg().Flags();
                flags &= ~ECreateNewContact;
                flags |= EDoNotOpenEditor;
                dataPckg().SetFlags( flags );

                delete iInstructionsPackage;
                iInstructionsPackage = NULL;
                iInstructionsPackage = dataPckg.AllocL();
                break;
                }

            case EAiwContactAttributeAssignV1:
                {
                TAiwContactAttributeAssignDataV1Pckg dataPckg;

                // Reconstruct the instructions
                dataPckg.Copy( iInstructionsPackage->Ptr(),
                    sizeof( TAiwContactAttributeAssignDataV1 ) );

                // Attribute assign does not support new contact
                // creation nor does it support contact editor,
                // so disable those
                TUint flags = dataPckg().Flags();
                flags &= ~ECreateNewContact;
                flags |= EDoNotOpenEditor;
                dataPckg().SetFlags( flags );

                delete iInstructionsPackage;
                iInstructionsPackage = NULL;
                iInstructionsPackage = dataPckg.AllocL();
                break;
                }

            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EDataTypeNotSupported ) );
                break;
                }
            }

        }

    // If client does not give instructions then default
    // to multiple contact assign
    if ( !iInstructionsPackage )
        {
        TAiwMultipleContactAssignDataV1Pckg dataPckg;
        iInstructionsPackage = dataPckg.AllocL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::ValidateMenuServiceL
// --------------------------------------------------------------------------
//
void CPbk2AiwAssignProvider::ValidateMenuServiceL( const TInt& aCmdId )
    {
    // Version check
    TInt version = TAiwContactAssignDataBase::
        AssignDataTypeFromBuffer( *iInstructionsPackage );
    if ( version == EAiwContactAssignDataTypeNotDefined )
        {
        User::Leave( KErrArgument );
        }

    switch ( aCmdId )
        {
        case EPbk2AiwCmdAssign:
            {
            // Nothing to validate
            break;
            }

        case EPbk2AiwCmdDataSaveCreateNew:
            {
            // Create new supports only single contact assign
            if ( version != EAiwSingleContactAssignV1 )
                {
                User::Leave( KErrArgument );
                }

            TAiwSingleContactAssignDataV1Pckg dataPckg;
            // Reconstruct the instructions
            dataPckg.Copy( iInstructionsPackage->Ptr(),
                sizeof( TAiwSingleContactAssignDataV1 ) );
            TUint flags = dataPckg().Flags();
            // Make sure the Create New Contact flag is up
            flags |= ECreateNewContact;
            dataPckg().SetFlags( flags );

            delete iInstructionsPackage;
            iInstructionsPackage = NULL;
            iInstructionsPackage = dataPckg.AllocL();
            break;
            }

        case EPbk2AiwCmdDataSaveAddToExisting:
            {
            // Update existing can be done to multiple contacts
            // or to a single contact. We must take care that
            // create new contact flag is down.
            switch ( version )
                {
                case EAiwMultipleContactAssignV1:
                    {
                    TAiwMultipleContactAssignDataV1Pckg dataPckg;
                    // Reconstruct the instructions
                    dataPckg.Copy( iInstructionsPackage->Ptr(),
                        sizeof( TAiwMultipleContactAssignDataV1 ) );
                    TUint flags = dataPckg().Flags();
                    // Make sure the Create New Contact flag is down
                    flags &= ~ECreateNewContact;
                    dataPckg().SetFlags( flags );

                    delete iInstructionsPackage;
                    iInstructionsPackage = NULL;
                    iInstructionsPackage = dataPckg.AllocL();
                    break;
                    }
                case EAiwSingleContactAssignV1:
                    {
                    TAiwSingleContactAssignDataV1Pckg dataPckg;
                    // Reconstruct the instructions
                    dataPckg.Copy( iInstructionsPackage->Ptr(),
                        sizeof( TAiwSingleContactAssignDataV1 ) );
                    TUint flags = dataPckg().Flags();
                    // Make sure the Create New Contact flag is down
                    flags &= ~ECreateNewContact;
                    dataPckg().SetFlags(flags);

                    delete iInstructionsPackage;
                    iInstructionsPackage = NULL;
                    iInstructionsPackage = dataPckg.AllocL();
                    break;
                    }
                default:
                    {
                    __ASSERT_DEBUG( EFalse, Panic( EDataTypeNotSupported ) );
                    break;
                    }    
                };

            break;
            }

        default:
            {
            // Service was used from client applications own menu
            // or as a base service. Nothing to do.
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::ClientTitlePaneL
// --------------------------------------------------------------------------
//
HBufC* CPbk2AiwAssignProvider::ClientTitlePaneL()
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

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::ClientOrientation
// --------------------------------------------------------------------------
//
HBufC* CPbk2AiwAssignProvider::ClientOrientation() const
    {
    HBufC* result = NULL;
    
    CAknViewAppUi* appUi = static_cast<CAknViewAppUi*> ( CEikonEnv::Static()->EikAppUi() );
    
    if( appUi )
        {
        TInt orientationType = appUi->Orientation();
        result = HBufC::New( sizeof(orientationType) );
        TPtr resultPtr = result->Des();
        resultPtr.Num( orientationType );
        }
    return result;
    }
    
// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::LaunchAttributeAssignL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwAssignProvider::LaunchAttributeAssignL
        ( const CAiwGenericParamList& aInParamList,
          const TGenericParamId aSemanticId )
    {
    iAttributeData =
        CreateAttributePackageL( aInParamList, aSemanticId );

    // Check does the client want to assign attribute,
    // or does it want to unassign instead
    TAiwContactAttributeAssignDataV1Pckg data;
    data.Copy( *iInstructionsPackage );
    TBool unassign = data().RemoveAttribute();

    if ( !unassign )
        {
        // Launch assign
        iConnection->LaunchAttributeAssignL
            ( iConfigurationPackage, iAttributeData, iInstructionsPackage );
        }
    else
        {
        // Unassign service requires that contact links are also being sent
        // to the server
        if ( !FindSemanticId
                ( aInParamList, EGenericParamContactLinkArray ) )
            {
            User::Leave( KErrArgument );
            }

        // Launch unassign
        iConnection->LaunchAttributeUnassignL
            ( iConfigurationPackage, iAttributeData, iInstructionsPackage );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::LaunchDataAssignL
// --------------------------------------------------------------------------
//
inline void CPbk2AiwAssignProvider::LaunchDataAssignL
        ( const CAiwGenericParamList& aInParamList,
          const TGenericParamId aSemanticId )
    {
    CreateDataPackageL( aInParamList, aSemanticId );

    // Launch
    iConnection->LaunchAssignL
        ( iConfigurationPackage, iDataPackage, iInstructionsPackage );
    }

// --------------------------------------------------------------------------
// CPbk2AiwAssignProvider::PackImppParametersToBuffer
// --------------------------------------------------------------------------
//
HBufC* CPbk2AiwAssignProvider::PackImppParametersToBufferL(
        const CAiwGenericParamList& aInParamList)
    {
    _LIT(KSeparator, "\n");
    const TInt KParamsCount = 4;
    TGenericParamId paramId[KParamsCount] = {EGenericParamXSpId,
            EGenericParamFirstName, EGenericParamLastName,
            EGenericParamNickname};
    TPtrC params[KParamsCount] = {TPtrC(), TPtrC(), TPtrC(), TPtrC()};
    HBufC* paramsBuf[KParamsCount] = {NULL, NULL, NULL, NULL};

    TInt i = 0;
    TInt separators = 0;
    TInt length = 0;
    // read all parameters
    for (; i < KParamsCount; i++)
        {
        TInt paramIndex = 0;
        const TAiwGenericParam* paramData =
            aInParamList.FindFirst(paramIndex, paramId[i]);
        if (paramData && paramData->Value().AsData().Ptr())
            {
            // Data was given as 8-bit, transform it into 16-bit
            TPtrC8 ptr = paramData->Value().AsData();
            paramsBuf[i] = HBufC::NewLC(ptr.Size());
            paramsBuf[i]->Des().Copy(ptr);
            params[i].Set(*paramsBuf[i]);
            separators = i;
            }
        else if ( paramData && paramData->Value().AsDes().Ptr() )
            {
            params[i].Set(paramData->Value().AsDes());
            separators = i;
            }
        length += params[i].Length();
        }
    
    HBufC* result = HBufC::NewL(length+separators);
    TPtr ptr = result->Des();
    // now separators will be used to count fields
    // but fields is one more than separators
    separators++;
    // create parameter string for server
    for (i = 0; i < separators; i++)
        {
        if (i)
            {
            ptr.Append(KSeparator);
            }
        ptr.Append(params[i]);
        }
    
    // clean cleanup stack
    for (i = KParamsCount-1; i >= 0; i--)
        {
        if (paramsBuf[i])
            {
            CleanupStack::PopAndDestroy(paramsBuf[i]);
            }
        }
    
    return result;
    }

// End of File
