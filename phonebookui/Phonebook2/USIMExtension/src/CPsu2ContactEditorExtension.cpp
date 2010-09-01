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
* Description:  Phonebook 2 USIM UI contact editor extension.
*
*/


#include "CPsu2ContactEditorExtension.h"

// Phonebook 2
#include "CPsu2CharConv.h"
#include "CPsu2FdnContactEditorPhoneNumberField.h"
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2USimUIRes.rsg>
#include <Pbk2UIControls.rsg>
#include <Pbk2CommonUi.rsg>
#include <Pbk2UIControls.hrh>
#include <MPbk2ContactEditorControl.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <etelmmerr.h>
#include <gsmerror.h>
#include <exterror.h>
#include <eikmenub.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <barsread.h>
#include <eikbtgpc.h>
#include <charconv.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Matches field types.
 *
 * @param aFieldTypeList        The field type list to use.
 * @param aField                The field to match.
 * @return                      The matched field type.
 */
const MVPbkFieldType* MatchFieldType
    (const MVPbkFieldTypeList& aFieldTypeList,
    const MVPbkBaseContactField& aField)
    {
    const MVPbkFieldType* result = NULL;

    for (TInt matchPriority = 0;
        matchPriority <= aFieldTypeList.MaxMatchPriority() && !result;
        ++matchPriority)
        {
        result = aField.MatchFieldType(matchPriority);
        }

    return result;
    }

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aField            The field whose type to check.
 * @param aFieldTypeList    List of field types.
 * @param aResourceId       Selector's resource id.
 * @return                  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkBaseContactField& aField,
        const MVPbkFieldTypeList& aFieldTypeList,
        const TInt aResourceId)
    {
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, aResourceId);

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL(resReader, aFieldTypeList);
    CleanupStack::PopAndDestroy(); // resReader
    // Get the field type
    const MVPbkFieldType* fieldType = MatchFieldType
        (aFieldTypeList, aField);

    TBool ret = EFalse;
    if ( fieldType )
        {
        ret = selector->IsFieldTypeIncluded(*fieldType);
        }
    delete selector;
    return ret;
    }

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aFieldType        The field type to check.
 * @param aFieldTypeList    Field type list.
 * @param aResourceId       Selector's resource id.
 * @return ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkFieldType& aFieldType,
        const MVPbkFieldTypeList& aFieldTypeList,
        const TInt aResourceId)
    {
    // Get the field type
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, aResourceId);

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL(resReader, aFieldTypeList);
    CleanupStack::PopAndDestroy(); // resReader

    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    delete selector;
    return ret;
    }

/// Text was too long (name or second name)
const TInt KErrTextTooLong = KErrGsm0707TextStringTooLong;

/// Number too long (this shouldn't be possible. It probably means that
/// ETel store GetInfo returns incorrect max length.
const TInt KErrNumberTooLong = KErrGsm0707DialStringTooLong;

/// Number contained characters that couldn't be saved
const TInt KErrInvalidNumber = KErrGsmCCUnassignedNumber;

/// (U)SIM store is full
const TInt KErrContactStoreFull = KErrMMEtelMaxReached;

/// Second name file is full in USIM
const TInt KErrSecondNameStoreFull = KErrGsmSimServSneFull;

/// Additional number file is full in USIM
const TInt KErrAdditionalNumberStoreFull = KErrGsmSimServAnrFull;

/// Email file is full in USIM
const TInt KErrEmailStoreFull = KErrGsmSimServEmailFull;

} /// namespace

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::CPsu2ContactEditorExtension
// --------------------------------------------------------------------------
//
CPsu2ContactEditorExtension::CPsu2ContactEditorExtension
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl ) :
            iContactManager( aContactManager ),
            iContact( aContact ),
            iEditorControl( aEditorControl )
    {
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::~CPsu2ContactEditorExtension
// --------------------------------------------------------------------------
//
CPsu2ContactEditorExtension::~CPsu2ContactEditorExtension()
    {
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ConstructL
// --------------------------------------------------------------------------
//
inline void CPsu2ContactEditorExtension::ConstructL()
    {
    iContactOrigin = ContactOrigin();
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::NewL
// --------------------------------------------------------------------------
//
CPsu2ContactEditorExtension* CPsu2ContactEditorExtension::NewL
        ( CVPbkContactManager& aContactManager,
          MVPbkStoreContact& aContact,
          MPbk2ContactEditorControl& aEditorControl )
    {
    CPsu2ContactEditorExtension* self =
        new (ELeave) CPsu2ContactEditorExtension
            (aContactManager, aContact, aEditorControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::CreateEditorFieldL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField* CPsu2ContactEditorExtension::CreateEditorFieldL
    ( MVPbkStoreContactField& aField,
      const MPbk2FieldProperty& aFieldProperty,
      MPbk2ContactEditorUiBuilder& aUiBuilder,
      CPbk2IconInfoContainer& aIconInfoContainer )
    {
    MPbk2ContactEditorField* field = NULL;

    if (iContactOrigin == EPsu2FdnContact)
        {
        if (aFieldProperty.CtrlType() == EPbk2FieldCtrlTypePhoneNumberEditor)
            {
            field = CPsu2FdnContactEditorPhoneNumberField::NewL
                ( aField, aFieldProperty, aUiBuilder, aIconInfoContainer );
            }
        }
    return field;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // FDN contact has additional rules for menu filtering
    if (iContactOrigin == EPsu2FdnContact)
        {
        if (aResourceId == R_PBK2_CONTACTEDITOR_MENUPANE ||
            aResourceId == R_PBK2_CONTACTEDITOR_CONTEXT_MENUPANE)
            {
            // No add/delete item menu in FDN
            aMenuPane->SetItemDimmed(EPbk2CmdAddItem, ETrue);
            aMenuPane->SetItemDimmed(EPbk2CmdDeleteItem, ETrue);
            }
        }

    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ProcessCommandL
// --------------------------------------------------------------------------
//
TBool CPsu2ContactEditorExtension::ProcessCommandL
        ( TInt aCommandId )
    {
    switch( aCommandId )
        {
        // If Pbk2 goes to the backgound, child applications are closed
        case EAknCmdHideInBackground:
            {
            iEndKeyWasPressed = ETrue;
            }
            break;
        
        default:
            break;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::OkToDeleteContactL
// --------------------------------------------------------------------------
//
TBool CPsu2ContactEditorExtension::OkToDeleteContactL
        ( MPbk2ContactEditorEventObserver::TParams& /*aParams*/ )
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::OkToSaveContactL
// --------------------------------------------------------------------------
//
TBool CPsu2ContactEditorExtension::OkToSaveContactL
        ( MPbk2ContactEditorEventObserver::TParams& /*aParams*/ )
    {
    TBool ret = ETrue;

    // FDN contact has additional rules for saving
    if (iContactOrigin == EPsu2FdnContact)
        {
        // Test is there a filled phone number field
        TBool controlIsEmpty = ETrue;
        TInt startIndex = 0; // find from start
        TInt numberControl = iEditorControl.IndexOfCtrlType
            (EPbk2FieldCtrlTypePhoneNumberEditor, startIndex);
        if (numberControl != KErrNotFound)
            {
            controlIsEmpty = iEditorControl.IsControlEmptyL(numberControl);
            }
        if (controlIsEmpty)
            {
            if( iEndKeyWasPressed )
                {
                iEndKeyWasPressed = EFalse;
                }
            else
                {
                ShowInformationNoteL(R_QTN_FDN_NO_ALLOWED_SAVE, EFalse);
                }
            ret = EFalse;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ModifyButtonGroupContainerL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::ModifyButtonGroupContainerL
        ( CEikButtonGroupContainer& aButtonGroupContainer )
    {
    // Get FDN editor middle softkey pointer
    CCoeControl* msk = aButtonGroupContainer.ControlOrNull( EAknSoftkeyEmpty );
    
    // FDN editor has different middle softkey
    if ( iContactOrigin == EPsu2FdnContact && !msk )
        {
        aButtonGroupContainer.SetCommandSetL
            ( R_PSU_FDN_SOFTKEYS_OPTIONS_DONE_EMPTY );
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::DoRelease
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::DoRelease()
    {
    delete this;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ContactEditorOperationCompleted
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::ContactEditorOperationCompleted
        ( MVPbkContactObserver::TContactOpResult /*aResult*/,
          TParams /*aParams*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ContactEditorExtension::ContactEditorOperationCompleted"));

    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ContactEditorOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::ContactEditorOperationFailed
        ( MVPbkContactObserver::TContactOp /*aOpCode*/,
          TInt aErrorCode, TParams /*aParams*/, TFailParams& aFailParams )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ContactEditorExtension::ContactEditorOperationFailed code: %d"),
        aErrorCode);

    if ( !aFailParams.iErrorNotified &&
            iContactOrigin != EPsu2NotSimContact )
        {
        TRAPD( err, HandleContactEditorOperationFailedL
            ( aErrorCode, aFailParams ) );
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ContactOrigin
// --------------------------------------------------------------------------
//
inline TInt CPsu2ContactEditorExtension::ContactOrigin() const
    {
    TInt ret = EPsu2NotSimContact;

    TVPbkContactStoreUriPtr uri =
        iContact.ParentStore().StoreProperties().Uri();

    TVPbkContactStoreUriPtr adnUri(VPbkContactStoreUris::SimGlobalAdnUri());
    TVPbkContactStoreUriPtr sdnUri(VPbkContactStoreUris::SimGlobalSdnUri());
    TVPbkContactStoreUriPtr fdnUri(VPbkContactStoreUris::SimGlobalFdnUri());

    if (uri.Compare(adnUri,
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        {
        ret = EPsu2AdnContact;
        }
    else if (uri.Compare(sdnUri,
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        {
        ret = EPsu2SdnContact;
        }
    else if (uri.Compare(fdnUri,
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        {
        ret = EPsu2FdnContact;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ShowInformationNote
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::ShowInformationNote
        ( TInt aResourceId, TBool aShowStoreName ) const
    {
    // Somewhat safe to ignore errors in note displaying
    TRAP_IGNORE( ShowInformationNoteL( aResourceId, aShowStoreName ) );
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::ShowInformationNoteL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::ShowInformationNoteL
        ( TInt aResourceId, TBool aShowStoreName ) const
    {
    HBufC* prompt = NULL;
    const TDesC* storeName = NULL;

    if (aShowStoreName)
        {
        // Fetch store name
        TVPbkContactStoreUriPtr uri =
            iContact.ParentStore().StoreProperties().Uri();
        const CPbk2StoreProperty* storeProperty =
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreProperties().FindProperty(uri);

        if (storeProperty)
            {
            storeName = &storeProperty->StoreName();
            }
        else
            {
            storeName = &uri.UriDes();
            }
        }

    if (aShowStoreName)
        {
        prompt = StringLoader::LoadL(aResourceId, *storeName);
        }
    else
        {
        prompt = StringLoader::LoadL(aResourceId);
        }

    if (prompt)
        {
        CleanupStack::PushL(prompt);
        // This is a waiting dialog because the editor might be
        // used from the application server and the information note will
        // disappear if the application server closes before the
        // note timeout has expired, thus causing blinking
        CAknInformationNote* noteDlg = new(ELeave) CAknInformationNote(ETrue);
        noteDlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(); // prompt
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::FocusControlOfType
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::FocusControlOfType
        ( TPbk2FieldCtrlType aType )
    {
    TInt focusIndex = 0;
    TInt startIndex = 0; // find from start
    focusIndex = iEditorControl.IndexOfCtrlType(aType, startIndex);
    if (focusIndex > KErrNotFound)
        {
        iEditorControl.SetFocus(focusIndex);
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::AddLastNameFieldL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::AddLastNameFieldL()
    {
    // First check does the contact have the last name field
    TBool found = EFalse;
    MVPbkStoreContactFieldCollection& fields = iContact.Fields();
    const MVPbkFieldTypeList& fieldTypes = iContactManager.FieldTypes();

    const TInt fieldCount = fields.FieldCount();
    for (TInt i=0; i<fieldCount; ++i)
        {
        if (IsFieldTypeIncludedL(fields.FieldAt(i),
            fieldTypes, R_PHONEBOOK2_LAST_NAME_SELECTOR))
            {
            found = ETrue;
            break;
            }
        }

    if (!found)
        {
        // Get field properties
        CPbk2FieldPropertyArray& fieldProperties =
            Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties();

        // Find the correct field type and then add it to contact
        const TInt fieldPropCount = fieldProperties.Count();
        for (TInt j=0; j<fieldPropCount; ++j)
            {
            const MPbk2FieldProperty& prop = fieldProperties.At(j);

            if (IsFieldTypeIncludedL(prop.FieldType(),
                    fieldTypes, R_PHONEBOOK2_LAST_NAME_SELECTOR))
                {
                MVPbkStoreContactField* field = iContact.CreateFieldLC
                    (prop.FieldType());
                iContact.AddFieldL(field);
                CleanupStack::Pop(); // field
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::HandleTextTooLongL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::HandleTextTooLongL
        ( TFailParams& aFailParams ) const
    {
    CPsu2CharConv* charConv =
        CPsu2CharConv::NewLC( Phonebook2::Pbk2AppUi()->
            ApplicationServices().ContactManager().FsSession(),
        KCharacterSetIdentifierSms7Bit );

    // Default to 'name too long' note
    TInt noteResource = R_QTN_PBSAV_UNICODE_NAME_TOO_LONG;
    TInt offender = KErrNotFound;

    const MVPbkFieldTypeList& fieldTypes =
        iContactManager.FieldTypes();
    const MVPbkStoreContactFieldCollection& fields = iContact.Fields();

    const TInt fieldCount = fields.FieldCount();
    for ( TInt i = 0; i< fieldCount; ++i )
        {
        const MVPbkStoreContactField& field =
            fields.FieldAt( i );

        const MVPbkContactFieldData& fieldData = field.FieldData();

        if ( fieldData.DataType() == EVPbkFieldStorageTypeText )
            {
            const MVPbkContactFieldTextData* textData =
                &MVPbkContactFieldTextData::Cast( fieldData );

            CPsu2CharConv::TPsu2FieldValidity validity;
            charConv->CheckFieldValidityL(
                textData->Text(),
                textData->MaxLength(),
                validity );

            if ( validity == CPsu2CharConv::ETooLong )
                {
                offender = i;

                if ( IsFieldTypeIncludedL( field,
                    fieldTypes, R_PHONEBOOK2_EMAIL_SELECTOR ) )
                    {
                    // We have to change the note
                    noteResource = R_QTN_SIMP_NOTE_EMAIL_TOO_LONG2;
                    }
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( charConv );

    // Set focus to the guilty field
    if ( offender != KErrNotFound )
        {
        iEditorControl.SetFocus( offender );
        }

    ShowInformationNote( noteResource, EFalse );

    // Don't offer relocation service for FDN contact
    // because it is security risk.
    if ( iContactOrigin != EPsu2FdnContact )
        {
        // Request contact relocation
        aFailParams.iErrorCode = KErrNotSupported;
        }
    }

// --------------------------------------------------------------------------
// CPsu2ContactEditorExtension::HandleContactEditorOperationFailedL
// --------------------------------------------------------------------------
//
void CPsu2ContactEditorExtension::HandleContactEditorOperationFailedL
        ( TInt aErrorCode, TFailParams& aFailParams )
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
    "CPsu2ContactEditorExtension::HandleContactEditorOperationFailedL 0x%x" ),
        this );
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
    "CPsu...::HandleContactEditorOperationFailedL ERROR:%d" ),
        aErrorCode );       
            
        
    // Check if SIM specific error code
    switch ( aErrorCode )
        {
        case KErrContactStoreFull:          // FALLTHROUGH
            {
            // The contact does not fit to (U)SIM
            ShowInformationNote( R_QTN_PHOB_NOTE_STORE_FULL, ETrue );

            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = ETrue;
            break;
            }

        case KErrSecondNameStoreFull:       // FALLTHROUGH
        case KErrAdditionalNumberStoreFull: // FALLTHROUGH
        case KErrEmailStoreFull:
            {
            // The contact does not fit to (U)SIM
             ShowInformationNote( R_QTN_PHOB_NOTE_SOME_INFO_NOT_ADDED,
                EFalse );

            // Request contact relocation
            aFailParams.iErrorCode = KErrGeneral;
            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = ETrue;
            break;
            }

        case KErrTextTooLong:
            {
            HandleTextTooLongL( aFailParams );
            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = EFalse;
            break;
            }

        case KErrNumberTooLong:
            {
            // Too long number field.
            // Set focus to a phone number field, which probably
            // (but not surely) was the problematic one.
            FocusControlOfType( EPbk2FieldCtrlTypePhoneNumberEditor );
            ShowInformationNote( R_QTN_PBSAV_ERR_NUMBER_TOO_LONG, EFalse );
            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = EFalse;
            break;
            }

        case KErrInvalidNumber:
            {
            // There was an invalid number that could not be saved.
            // Set focus to a phone number field, which probably
            // (but not surely) was the problematic one
            FocusControlOfType( EPbk2FieldCtrlTypePhoneNumberEditor );
            ShowInformationNote( R_QTN_ERR_SMS_INVALID_NUMB, EFalse );
            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = EFalse;
            break;
            }

        case KErrArgument:
            {
            // Input argument out of range, experienced i.e. when email
            // address too long for (U)SIM. Show general SIM card error
            // because error origin can not be quaranteed
            ShowInformationNote( R_TEXT_FDN_CARD_ERROR, EFalse );

            // Request contact relocation
            aFailParams.iErrorCode = KErrGeneral;
            aFailParams.iErrorNotified = ETrue;
            aFailParams.iCloseEditor = EFalse;
            break;
            }

        case KErrNone:          // FALLTHROUGH
        case KErrNoMemory:
            {
            // These are handled in the actual contact editor
            aFailParams.iErrorNotified = EFalse;
            aFailParams.iCloseEditor = ETrue;
            break;
            }

        default:
            {
            // Show general SIM card error
            ShowInformationNote( R_TEXT_FDN_CARD_ERROR, EFalse );

            // Request contact relocation
            aFailParams.iErrorCode = KErrNotSupported;
            aFailParams.iErrorNotified = EFalse;
            aFailParams.iCloseEditor = ETrue;
            break;
            }
        }
    }

// End of File
