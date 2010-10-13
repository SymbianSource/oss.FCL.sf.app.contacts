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
* Description:  Phonebook 2 call command object.
*
*/


// INCLUDE FILES
#include "CPbk2CallCmd.h"

// Phonebook 2
#include "CPbk2CallTypeSelector.h"
#include <Pbk2Commands.hrh>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactLink.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <Pbk2UIControls.rsg>
#include <MVPbkContactFieldUriData.h>

// System includes
#include <coemain.h>
#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>
#include <aiwdialdataext.h>
#include <spsettingsvoiputils.h>

//SPSettings
#include <spsettings.h>
#include <spproperty.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ExecuteLD = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2CallCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

/**
 * Field type matcher.
 *
 * @param aFieldTypeList    List of field types.
 * @param aFieldType        Field type to search for.
 * @param aResourceId       Field type selector resources.
 * @return  ETrue if match was found.
 */
TBool MatchesFieldTypeL
        ( const MVPbkFieldTypeList& aFieldTypeList,
          const MVPbkFieldType& aFieldType,
          TInt aResourceId )
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL
        ( reader, aFieldTypeList );

    // Check if the field type is the one needed
    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    CleanupStack::PopAndDestroy(); // resource buffer
    delete selector;
    return ret;
    }

/**
 * Sets a phone number to AIW dial data.
 *
 * @param aDialData     Dial data to modify.
 * @param aContact      Contact containing the number.
 */
void SetDialDataNumberL
        ( CAiwDialDataExt& aDialData, const MVPbkStoreContact& aContact )
    {
    const MVPbkFieldTypeList& fieldTypeList =
            aContact.ParentStore().StoreProperties().SupportedFields();
    const MVPbkStoreContactFieldCollection& fieldCollection  =
        aContact.Fields();
    TInt fieldCount( fieldCollection.FieldCount() );
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field = fieldCollection.FieldAt( i );
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if( MatchesFieldTypeL( fieldTypeList, *fieldType,
            R_PHONEBOOK2_PHONENUMBER_SELECTOR ) )
            {
            const MVPbkContactFieldTextData* textData =
                &MVPbkContactFieldTextData::Cast
                    ( field.FieldData() );
            const TDesC& phoneNumber( textData->Text().Left
                ( AIWDialData::KMaximumPhoneNumberLength ) );
            aDialData.SetPhoneNumberL( phoneNumber );
            }
        }
    }

/**
 * Sets a phone number to AIW dial data.
 *
 * @param aDialData     Dial data to modify.
 * @param aField        Contact field containing the number.
 */
void SetDialDataNumberL
        ( CAiwDialDataExt& aDialData, const MVPbkStoreContactField& aField )
    {
    TVPbkFieldStorageType dataType = aField.FieldData().DataType();
    
    if ( dataType == EVPbkFieldStorageTypeText )
        {
        const MVPbkContactFieldTextData* textData =
            &MVPbkContactFieldTextData::Cast( aField.FieldData() );
        const TDesC& phoneNumber( textData->Text().Left
            ( AIWDialData::KMaximumPhoneNumberLength ) );
        aDialData.SetPhoneNumberL( phoneNumber );
        }
    
    if ( dataType == EVPbkFieldStorageTypeUri )
        {
        const MVPbkContactFieldUriData* textData =
            &MVPbkContactFieldUriData::Cast( aField.FieldData() );
        const TDesC& phoneNumber( textData->Text().Left
            ( AIWDialData::KMaximumPhoneNumberLength ) );
        aDialData.SetPhoneNumberL( phoneNumber );
        }
    }
} /// namespace

// --------------------------------------------------------------------------
// CPbk2CallCmd::CPbk2CallCmd
// --------------------------------------------------------------------------
//
CPbk2CallCmd::CPbk2CallCmd( MVPbkStoreContact*& aContact,
        MVPbkStoreContactField* aSelectedField,
        MPbk2ContactUiControl& aControl,
        const TInt aCommandId, CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionSelector ) :
            iContact( aContact ),
            iSelectedField( aSelectedField ),
            iControl( &aControl ),
            iCommandId( aCommandId ),
            iServiceHandler( aServiceHandler ),
            iSelector( aSelector ),
            iActionSelector( aActionSelector )
            
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2CallCmd::CPbk2CallCmd(0x%x)"), this);

    iControl->RegisterCommand( this );
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::~CPbk2CallCmd
// --------------------------------------------------------------------------
//
CPbk2CallCmd::~CPbk2CallCmd()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2CallCmd::~CPbk2CallCmd(0x%x)"), this);

    delete iSelectedField;
    if( iControl )
        {
        iControl->RegisterCommand( NULL );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CallCmd* CPbk2CallCmd::NewL( MVPbkStoreContact*& aContact,
        MVPbkStoreContactField* aSelectedField,
        MPbk2ContactUiControl& aControl,
        const TInt aCommandId, CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector )
    {
    CPbk2CallCmd* self = new (ELeave ) CPbk2CallCmd
        ( aContact, aSelectedField, aControl, aCommandId,
          aServiceHandler, aSelector, VPbkFieldTypeSelectorFactory::EEmptySelector );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CallCmd* CPbk2CallCmd::NewL( MVPbkStoreContact*& aContact,
        MVPbkStoreContactField* aSelectedField,
        MPbk2ContactUiControl& aControl,
        const TInt aCommandId, CAiwServiceHandler& aServiceHandler,
        CPbk2CallTypeSelector& aSelector,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aActionSelector )
    {
    CPbk2CallCmd* self = new (ELeave ) CPbk2CallCmd
        ( aContact, aSelectedField, aControl, aCommandId,
          aServiceHandler, aSelector, aActionSelector );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CallCmd::ExecuteLD()
    {
    PBK2_DEBUG_PRINT
        (PBK2_DEBUG_STRING("CPbk2CallCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );

    // Setup dial data
    CAiwDialDataExt* dialData = CAiwDialDataExt::NewLC();
    dialData->SetWindowGroup
        ( CCoeEnv::Static()->RootWin().Identifier() );
    
    // If there is a default service, use the service 
    TUint serviceId;
    CSPSettingsVoIPUtils* sPSettings = CSPSettingsVoIPUtils::NewLC();
    if ( !sPSettings->GetPreferredService( serviceId ) )
        {
        dialData->SetServiceId( serviceId );
        }
    CleanupStack::PopAndDestroy( sPSettings );
    
    MVPbkContactLink* contactLink = NULL;

    if ( iCommandId == EPbk2CmdCall )
        {
        // Call was launched with send key, which means
        // Phonebook has performed address select by itself
        // and there is selected field instance
        __ASSERT_DEBUG( iSelectedField &&
            ( iSelectedField->FieldData().DataType() ==
                EVPbkFieldStorageTypeText || 
                iSelectedField->FieldData().DataType() ==
                    EVPbkFieldStorageTypeUri ),
                        Panic( EPanicPreCond_ExecuteLD ) );

        // Create field link
        contactLink = iSelectedField->CreateLinkLC();

        // Set number
        SetDialDataNumberL( *dialData, *iSelectedField );

        // We also have select call type by ourselves
        SetCallTypeL( *dialData );
        
        // If field data has service prefix, extract it and find matched service id
        // from SP setting, then set the matched service id to daildata.
        // Service prefix from field data has higher priority over preferred service
        TPtrC xspId;        
        if ( ExtractXspId( iSelectedField, xspId ) )
        	{
        	TUint srcId = GetMatchedServiceIdL( xspId );
        	
            if ( srcId != (TUint)KErrNotFound )
            	{
            	dialData->SetServiceId( srcId );
            	} 
        	}      
        }
    else
        {
        if ( iSelectedField )
            {
            // If selected field is set create field link from it.
            contactLink = iSelectedField->CreateLinkLC();
            }
        else
            {
            // Otherwise use contact
            contactLink = iContact->CreateLinkLC();
            }

        if ( !contactLink )
            {
            // Temporary contact, set number here
            SetDialDataNumberL( *dialData, *iContact );
            }
        }

    if ( contactLink )
        {
        HBufC8* linkBuffer = contactLink->PackLC();
        dialData->SetContactLinkL( *linkBuffer );
        }
    else
        {
        CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();
        MPbk2ContactNameFormatter& nameFormatter =
            appServices->NameFormatter();
        const MVPbkStoreContactFieldCollection& fieldCollection  =
            iContact->Fields();
        HBufC* title = nameFormatter.GetContactTitleOrNullL(
                        fieldCollection,
                        MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
        CleanupStack::PopAndDestroy();
        CleanupStack::PushL( title );
            
        if ( title )
            {
            dialData->SetNameL( *title );
            }
        }

    // Get an empty in param list..
    CAiwGenericParamList& inParamList = iServiceHandler.InParamListL();
    // ..and pass it to dial data in order for it to fill it
    dialData->FillInParamListL( inParamList );

    if ( contactLink )
        {
        CleanupStack::PopAndDestroy( 3 ); // linkBuffer, contactLink, dialData
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 ); // title, dialData
        }


    // If the command id is EPbk2CmdCall it means the call was launched
    // with send key and we therefore must use different AIW command
    // than when the call was selected from the menu
   if ( iCommandId == EPbk2CmdCall )
       {
        iServiceHandler.ExecuteServiceCmdL(
            KAiwCmdCall,
            inParamList,
            iServiceHandler.OutParamListL(),
            0,      // No options used.
            NULL);  // No need for callback
       }
   else // the call was launched from menu
       {
        // Relay the command to AIW for handling
        iServiceHandler.ExecuteMenuCmdL(
            iCommandId,
            inParamList,
            iServiceHandler.OutParamListL(),
            0,      // No options used.
            NULL);  // No need for callback
       }

    // Update UI control
    if( iControl )
        {
        TInt fieldIndex = iControl->FocusedFieldIndex();
        iControl->ResetFindL();
        if( iContact )
            {
            iControl->SetFocusedContactL( *iContact );
            }
        iControl->SetFocusedFieldIndex( fieldIndex );
        }

    // Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2CallCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if( iControl == &aUiControl )
        {
        iControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2CallCmd::AddObserver( MPbk2CommandObserver& /*aObserver*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::SetCallTypeL
// --------------------------------------------------------------------------
//
void CPbk2CallCmd::SetCallTypeL( CAiwDialDataExt& dialData )
    { 
    switch( iActionSelector )
        {
        CAiwDialData::TCallType callType;
        case VPbkFieldTypeSelectorFactory::EEmptySelector:
            callType = iSelector.SelectCallTypeL
                ( iCommandId, *iContact, iControl->FocusedField(), *iSelectedField );
            dialData.SetCallType( callType );
            break;
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
            dialData.SetCallType( CAiwDialData::EAIWForcedCS );
            break;
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            callType = iSelector.SelectVoipCallTypeL( iSelectedField );
            dialData.SetCallType( callType );
            break;
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            dialData.SetCallType( CAiwDialData::EAIWForcedVideo );
            break;
        default:
            dialData.SetCallType ( CAiwDialData::EAIWForcedCS);
            break;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::ExtractXspId
// --------------------------------------------------------------------------
//
TBool CPbk2CallCmd::ExtractXspId(
    const MVPbkStoreContactField* aSelectedField, TPtrC& aXSPId ) const
    {
    TBool found = EFalse;
    _LIT( KColon, ":" );

	const MVPbkContactFieldData& fieldData = aSelectedField->FieldData();
	TPtrC data  = GetFieldData( fieldData );	  
    TInt pos = data.Find( KColon );
    
    if ( pos > 0 )
        {
        aXSPId.Set( data.Left( pos ) );
        found = ETrue;
        }

    return found;
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::GetMatchedServiceIdL
// --------------------------------------------------------------------------
//
TServiceId CPbk2CallCmd::GetMatchedServiceIdL( const TDesC& aXSPId )
    {
    TUint ret = ( TUint )KErrNotFound;
    CDesCArrayFlat* nameArray = NULL;

    RIdArray ids;
    CleanupClosePushL( ids );

    nameArray = new (ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( nameArray );

    CSPSettings* settings = CSPSettings::NewLC();

    settings->FindServiceIdsL( ids );
    settings->FindServiceNamesL( ids, *nameArray );

    const TInt count = nameArray->MdcaCount();
    for ( TInt i=0; i < count; i++ )
        {
        // Find the mathched service
        TPtrC name = nameArray->MdcaPoint( i );
        if ( !name.CompareF( aXSPId ) )
            {
            // Service found
            ret = ids[i];
            break;
            }
        }
    CleanupStack::PopAndDestroy( 3 ); // ids, nameArray, settings

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CallCmd::GetFieldData
// --------------------------------------------------------------------------
//
TPtrC CPbk2CallCmd::GetFieldData( const MVPbkContactFieldData& aFieldData ) const
	{
	TPtrC text ( KNullDesC() );
	
    switch ( aFieldData.DataType() )
        {
        // Text storage type
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast( aFieldData );
            text.Set( textData.Text() );
            break;
            }
        // URI storage type
        case EVPbkFieldStorageTypeUri:
            {
            const MVPbkContactFieldUriData& textData = MVPbkContactFieldUriData::Cast( aFieldData );
            text.Set( textData.Text() );
            break;
            }
        }
    
    return text;
	}

// End of File
