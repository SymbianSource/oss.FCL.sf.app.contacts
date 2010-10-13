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
* Description:  Phonebook 2 address selection.
*
*/


#include <CPbk2AddressSelect.h>

// Phonebook 2
#include "CPbk2SelectFieldDlg.h"
#include "MPbk2ControlKeyObserver.h"
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2FieldPropertyArray.h>
#include <TPbk2AddressSelectParams.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ApplicationServices2.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2AppUi.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2Commands.rsg>
#include <CPbk2ApplicationServices.h>
#include <TPbk2StoreContactAnalyzer.h>

// Virtual Phonebook
#include <CVPbkFieldFilter.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <CVPbkContactManager.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>

// System includes
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <StringLoader.h>

//SpSettings
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spsettingsvoiputils.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ExecuteLD
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AddressSelect");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

#define KOneVOIPServiceAvailable    1

const TInt KFirstField = 0;
const TInt KDefaultTitleFormat = MPbk2ContactNameFormatter::EUseSeparator;

/**
 * Returns index of given field in store contact field collection.
 *
 * @param aCollection   Store contact field collection.
 * @param aField        Store contact field to search for.
 * @return  Index of the given field.
 */
inline TInt IndexOfField(
        const MVPbkStoreContactFieldCollection& aCollection,
        const MVPbkStoreContactField& aField )
    {
    TInt ret = KErrNotFound;
    const TInt count = aCollection.FieldCount();

    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkStoreContactField* field = aCollection.FieldAtLC( i );
        if ( aField.IsSame( *field ) )
            {
            ret = i;
            CleanupStack::PopAndDestroy(); // field
            break;
            }
        CleanupStack::PopAndDestroy(); // field
        }

    return ret;
    }

} /// namespace

// MODULE DATA STRUCTURES

/**
 * Special field selection dialog class for CPbk2AddressSelect.
 * The main purpose of this class is to
 * get #include of MPbk2ControlKeyObserver
 * away from public header CPbk2AddressSelect.h.
 */
NONSHARABLE_CLASS(CPbk2AddressSelect::CSelectFieldDlg) :
        public CPbk2SelectFieldDlg,
        private MPbk2ControlKeyObserver
    {
    public:  // Construction

        /**
         * Constructor.
         *
         * @param aParent   Parent.
         */
        CSelectFieldDlg( CPbk2AddressSelect& aParent ) :
                iParent( aParent )
            {
            SetObserver( this );
            }

    private: // From MPbk2ControlKeyObserver
        TKeyResponse Pbk2ControlKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

    private: // Data
        /// Ref: Parent
        CPbk2AddressSelect& iParent;
    };


// --------------------------------------------------------------------------
// CPbk2AddressSelect::CSelectFieldDlg::Pbk2ControlKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2AddressSelect::CSelectFieldDlg::Pbk2ControlKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // Forward call to virtual function in CPbk2AddressSelect interface
    return iParent.Pbk2ControlKeyEventL( aKeyEvent,aType );
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::CPbk2AddressSelect
// --------------------------------------------------------------------------
//
CPbk2AddressSelect::CPbk2AddressSelect( 
    TPbk2AddressSelectParams& aParams,
    const TArray<MVPbkStoreContact*>* aStoreContactsArray,
    const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray ):
    iParams( aParams ),
    iStoreContactsArray( aStoreContactsArray ),
    iPresenceIconsArray( aPresenceIconsArray )
    {
    __ASSERT_DEBUG
        ( !iFieldDlg && !iDestroyedPtr,
          Panic( EPanicPostCond_Constructor ) );
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::~CPbk2AddressSelect
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AddressSelect::~CPbk2AddressSelect()
    {
    // Tell ExecuteLD this object is already destroyed
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }

    // Set eliminator pointer to NULL
    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }

    delete iFieldDlg;
    delete iFieldFilter;
    delete iFieldTypeSelector;
    iFieldFilterArray.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AddressSelect* CPbk2AddressSelect::NewL
        ( TPbk2AddressSelectParams& aParams )
    {
    CPbk2AddressSelect* self = new ( ELeave ) CPbk2AddressSelect(
        aParams, NULL, NULL );
    CleanupStack::PushL( self );
    self->ConstructL( NULL );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AddressSelect* CPbk2AddressSelect::NewL
        ( TPbk2AddressSelectParams& aParams,
          CVPbkFieldTypeSelector& aFieldTypeSelector,
          const TArray<MVPbkStoreContact*>* aStoreContactsArray,
          const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray )
    {
    CPbk2AddressSelect* self = new ( ELeave ) CPbk2AddressSelect( aParams,
        aStoreContactsArray, aPresenceIconsArray );
    CleanupStack::PushL( self );
    self->ConstructL( &aFieldTypeSelector );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelect::ConstructL(
    CVPbkFieldTypeSelector* aFieldTypeSelector )
    {
    // Read the resource referenced in address select resource struct
    const TInt fieldTypeSelectorRes = iParams.iResReader.ReadInt32();
    TResourceReader selectorReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        ( selectorReader, fieldTypeSelectorRes );
    if ( aFieldTypeSelector == NULL )
        {
        // Give that resource reader to the field type selector
        iFieldTypeSelector = CVPbkFieldTypeSelector::NewL
            ( selectorReader, iParams.iContactManager.FieldTypes() );
        }
    else
        {
        iFieldTypeSelector = CVPbkFieldTypeSelector::NewL(
            *aFieldTypeSelector );
        }
    CleanupStack::PopAndDestroy(); // selectorReader

    iNoAddressesForNamePromptResource = iParams.iResReader.ReadInt32();
    iNoAddressesPromptResource = iParams.iResReader.ReadInt32();
    iSoftKeyResource = iParams.iResReader.ReadInt32();

    const CVPbkFieldFilter::TConfig config
        ( const_cast<MVPbkStoreContactFieldCollection&>
            ( iParams.iContact.Fields() ),
                iFieldTypeSelector, NULL );

    iFieldFilter = CVPbkFieldFilter::NewL( config );

    if ( iStoreContactsArray != NULL && iStoreContactsArray->Count() != 0 )
        {
        TInt count = iStoreContactsArray->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            const CVPbkFieldFilter::TConfig conf
                ( const_cast<MVPbkStoreContactFieldCollection&>
                    ( iStoreContactsArray->operator[](i)->Fields() ),
                      iFieldTypeSelector, NULL );
            CVPbkFieldFilter* fieldFilter = CVPbkFieldFilter::NewL( conf );
            CleanupStack::PushL( fieldFilter );
            iFieldFilterArray.AppendL( fieldFilter );
            CleanupStack::Pop(); //fieldfilter
            }
        }

    CPbk2ApplicationServices* appServices =
        CPbk2ApplicationServices::InstanceLC();
    iAttributeManager =
        &appServices->ContactManager().ContactAttributeManagerL();
    CleanupStack::PopAndDestroy(); // appServices
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkStoreContactField* CPbk2AddressSelect::ExecuteLD()
    {
    __ASSERT_DEBUG( !iFieldDlg, Panic( EPanicPreCond_ExecuteLD ) );

    // "D" function semantics
    CleanupStack::PushL( this );
    TBool thisDestroyed = EFalse;
    // Ensure that thisDestroyed will be ETrue if this object is destroyed.
    // See in destructor how this is done.
    iDestroyedPtr = &thisDestroyed;

    SelectFieldL();

    MVPbkStoreContactField* returnedField = NULL;
    if ( iSelectedField )
        {
        // We cannot return iSelectedField directly since if its NULL,
        // it's value changes to 0xdedede before it is returned.
        // Therefore we must test iSelectedField before assigning it
        // to returnedField.
        returnedField = iSelectedField;
        }

    if ( thisDestroyed )
        {
        // This object has already been destroyed
        CleanupStack::Pop( this );
        returnedField = NULL;
        }
    else
        {
        CleanupStack::PopAndDestroy( this );
        }

    return returnedField;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::AttemptExitL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AddressSelect::AttemptExitL( TBool aAccept )
    {
    if ( iFieldDlg )
        {
        iFieldDlg->AttemptExitL( aAccept );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::Pbk2ControlKeyEventL
// --------------------------------------------------------------------------
//
EXPORT_C TKeyResponse CPbk2AddressSelect::Pbk2ControlKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse ret = EKeyWasNotConsumed;

    if ( aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend )
        {
        // Event is Send key, tell field selection dialog to accept
        // current selection
        AttemptExitL( ETrue );
        ret = EKeyWasConsumed;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::SetCba
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AddressSelect::SetCba( TInt aCbaResourceId )
    {
    iSoftKeyResource = aCbaResourceId;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2AddressSelect::RequestExitL( TInt aCommandId )
    {
    if ( aCommandId == EEikBidCancel )
        {
        AttemptExitL( EFalse );
        }
    else if ( aCommandId == EEikBidOk )
        {
        AttemptExitL( ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2AddressSelect::ForceExit()
    {
    TRAPD( err, AttemptExitL( EFalse ) );
    if ( err != KErrNone )
        {
        // If not nicely then use the force
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2AddressSelect::ResetWhenDestroyed(
        MPbk2DialogEliminator** aSelfPtr )
    {
    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::AddressField
// Returns true if aField is an applicable address field.
// --------------------------------------------------------------------------
//
TBool CPbk2AddressSelect::AddressField
        ( const MVPbkStoreContactField& aField ) const
    {
    TBool ret = EFalse;

    // Return true if field belongs to the selector
    ret = ( iFieldFilter->FindField( aField ) ==
        KErrNotFound ) ? EFalse : ETrue;

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::NoAddressesL
// Called if there are no applicable address fields in a contact passed
// to ExecuteLD.
// --------------------------------------------------------------------------
//
void CPbk2AddressSelect::NoAddressesL
        ( TPbk2AddressSelectParams aParams ) const
    {
    if ( !aParams.iSuppressWarnings )
        {
        HBufC* prompt = NULL;
        HBufC* name = aParams.iNameFormatter.GetContactTitleOrNullL
            ( iParams.iContact.Fields(), KDefaultTitleFormat );

        if ( name )
            {
            CleanupStack::PushL(name);
            prompt = StringLoader::LoadL
                ( iNoAddressesForNamePromptResource, *name );
            CleanupStack::PopAndDestroy(); // name
            }
        else
            {
            prompt = StringLoader::LoadL( iNoAddressesPromptResource );
            }

        if ( prompt )
            {
            CleanupStack::PushL( prompt );
            // This is a waiting dialog because the address select might be
            // used from the application server and the information note will
            // disappear if the application server closes before the
            // note timeout has expired, thus causing blinking
            CAknInformationNote* noteDlg =
                new ( ELeave ) CAknInformationNote( ETrue );
            noteDlg->ExecuteLD( *prompt );
            CleanupStack::PopAndDestroy(); // prompt
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::SelectFieldL
// --------------------------------------------------------------------------
//
inline void CPbk2AddressSelect::SelectFieldL()
    {
    // If currently focused field is an applicable field
    if ( iParams.iFocusedField && AddressField( *iParams.iFocusedField ) )
        {
        // Applicable field was focused -> return field
        SetSelectedFieldL( iParams.iFocusedField );
        }
    else
        {
        TBool found = EFalse;
        // Focus is in on some other field, first check default field
        if ( iParams.iDefaultPriorities &&
             iParams.iUseDefaultDirectly &&
             iParams.iDefaultPriorities->Count() > 0 )
            {
            found = SelectFromDefaultFieldsL();
            }

        if ( !found )
            {
            // No direct call to focused or default number, we have to
            // select from applicable fields
            SelectFromApplicableFieldsL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::SelectFromApplicableFieldsL
// --------------------------------------------------------------------------
//
inline void CPbk2AddressSelect::SelectFromApplicableFieldsL()
    {
    TInt indexOfDefault = IndexOfDefaultFieldL();
    TInt fieldCount = iFieldFilter->FieldCount();
    
    // check also addiotnal contacts
    if ( iFieldFilterArray.Count() > 0 )
        {
        TInt count = iFieldFilterArray.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            fieldCount += iFieldFilterArray[i]->FieldCount();
            }
        }

    // Different actions for different number of applicable fields found
    if ( fieldCount == 0 )
        {
        // No applicable addresses found
        NoAddressesL( iParams );
        }
    else if ( fieldCount == 1 && !iParams.iQueryAlways )
        {
        // Exactly one applicable address found, just return it
        if ( iFieldFilter->FieldCount() != 0 )
        	{
         	SetSelectedFieldL( &iFieldFilter->FieldAt( KFirstField ) ); 
        	}
        else
        	{
        	SetSelectedFieldL( &iFieldFilterArray[0]->FieldAt( KFirstField ) );
        	}
        }
    else if (fieldCount > 1 || iParams.iQueryAlways )
        {
        HBufC* title = LoadDialogTitleL();
  
        // Run the address selection dialog
        CleanupStack::PushL( title );
        iFieldDlg = new ( ELeave ) CSelectFieldDlg( *this );
        iFieldDlg->ResetWhenDestroyed( &iFieldDlg );
        const MVPbkStoreContactField* field = NULL;
        field = iFieldDlg->ExecuteLCD
            ( *iFieldFilter, 
              iFieldFilterArray,
              iPresenceIconsArray,
              iParams.iContactManager,
              iParams.iFieldPropertyArray,
              iSoftKeyResource,
              iParams.iCommMethod,
              *title,
              indexOfDefault );

        SetSelectedFieldL( field );

        CleanupStack::PopAndDestroy( 2 ); // field, title
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::IndexOfDefaultFieldL
// --------------------------------------------------------------------------
//
inline TInt CPbk2AddressSelect::IndexOfDefaultFieldL()
    {
    TInt ret = KErrNotFound;

    const MVPbkStoreContactField* field = FindDefaultFieldLC();
    if ( field )
        {
        ret = iFieldFilter->FindField( *field );
        }
    CleanupStack::PopAndDestroy(); // field

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::SelectFromDefaultFieldsL
// --------------------------------------------------------------------------
//
inline TBool CPbk2AddressSelect::SelectFromDefaultFieldsL()
    {
    TBool found = EFalse;

    const MVPbkStoreContactField* field = FindDefaultFieldLC();
    if ( field )
        {
        SetSelectedFieldL( field );
        found = ETrue;
        }
    CleanupStack::PopAndDestroy(); // field

    return found;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::FindDefaultFieldLC
// --------------------------------------------------------------------------
//
MVPbkStoreContactField* CPbk2AddressSelect::FindDefaultFieldLC()
    {
    MVPbkStoreContactField* ret = NULL;

    // Go through the default priorities array, and
    // check does a specific default exist
    if ( iParams.iDefaultPriorities )
        {
        const TInt defaultCount = iParams.iDefaultPriorities->Count();

        for (TInt i=0; i<defaultCount; ++i)
            {
            TVPbkDefaultType defaultType =
                (TVPbkDefaultType) iParams.iDefaultPriorities->At(i);

            // Create an attribute prototype out of the attribute identifier
            CVPbkDefaultAttribute* attr =
                CVPbkDefaultAttribute::NewL( defaultType );
            CleanupStack::PushL( attr );

            if ( iAttributeManager->HasContactAttributeL
                    ( *attr, iParams.iContact ) )
                {
                const MVPbkStoreContactFieldCollection& fields =
                    iParams.iContact.Fields();

                // Get the field with attribute
                const TInt fieldCount = fields.FieldCount();
                for ( TInt j = 0; j < fieldCount; ++j )
                    {
                    if ( iAttributeManager->HasFieldAttributeL
                            ( *attr, fields.FieldAt( j ) ) )
                        {
                        ret = fields.FieldAtLC( j );
                        CleanupStack::Pop(); // ret
                        break;
                        }
                    }
                }

            CleanupStack::PopAndDestroy( attr );
            }
        }

    CleanupDeletePushL( ret );
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::SetSelectedFieldL
// --------------------------------------------------------------------------
//
inline void CPbk2AddressSelect::SetSelectedFieldL
        ( const MVPbkStoreContactField* aField )
    {
    if ( aField )
        {
        const MVPbkStoreContactFieldCollection& fields =
            iParams.iContact.Fields();
        TInt index = IndexOfField( fields, *aField );
        if ( index >= 0 )
            {
            // field from main contact
            iSelectedField = fields.FieldAtLC( index );
            CleanupStack::Pop();
            }
        else if ( iStoreContactsArray != NULL &&
                  iStoreContactsArray->Count() > 0 )
            {
            // field from additional contact
            TInt count = iStoreContactsArray->Count();
            for( TInt i = 0; i < count; i++ )
                {
                const MVPbkStoreContactFieldCollection& xspFields =
                    iStoreContactsArray->operator[](i)->Fields();
                index = IndexOfField( xspFields, *aField );
                if ( index >= 0 )
                    {
                    iSelectedField = xspFields.FieldAtLC( index );
                    CleanupStack::Pop();
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::IsVoiceCallExistL
// --------------------------------------------------------------------------
//
inline TBool CPbk2AddressSelect::IsVoiceCallExistL()
    {
    TInt countFields = iParams.iContact.Fields().FieldCount();
    TPbk2StoreContactAnalyzer analyzer( iParams.iContactManager, NULL );

    for ( TInt i = 0; i < countFields; i++ )
        {
        const MVPbkStoreContactField& field =
            iParams.iContact.Fields().FieldAt( i );
        
        // If the field is voice call, then return ETrue.
        if ( analyzer.IsFieldTypeIncludedL( field, R_PHONEBOOK2_PHONENUMBER_SELECTOR ) )
            {
            return ETrue;
            }
        }
    
    // No voice call filed, return EFalse.    
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::GetVoiceAndVOIPCallDialogTitleL
// --------------------------------------------------------------------------
//
inline HBufC* CPbk2AddressSelect::GetVoiceAndVOIPCallDialogTitleL()
    {
    HBufC* title (NULL);
    
    // If the popped up dialog is started in namelist view.
    // Then, the title of the dialog should obey such rules:
    // 1. When there are only VoIP addresses for contact or if VoIP is preferred.
    // Then show "Internet call:"
    // 2. In other cases show "Call:". This would be shown when:
    // 2.1 VoIP is not preferred and there's at least one Voice call number for the contact
    // 2.2 So even in case when there's only voice call numbers.
    // The title should be "Call:" instead of "Voice call:" 
    if ( iParams.iTitleResId == R_QTN_PHOB_QTL_CALL_TO_NAME )
        {
        // Check whether VoIP is preferred.
        CSPSettingsVoIPUtils* sPSettings = CSPSettingsVoIPUtils::NewLC();
        if ( sPSettings->IsPreferredTelephonyVoIP() ||
             !IsVoiceCallExistL() )
        	{
        	title = GetVOIPDialogTitleL();
        	}
        else
        	{
        	title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_CALL );
        	}
        CleanupStack::PopAndDestroy( sPSettings ); 
        }
    else if ( iParams.iCommMethod == VPbkFieldTypeSelectorFactory::EVoiceCallSelector )
        {                               
        title =  StringLoader::LoadL( R_QTN_CCA_POPUP_VOICE_CALL );
        }
    else if ( iParams.iCommMethod == VPbkFieldTypeSelectorFactory::EVOIPCallSelector )
        {
        title = GetVOIPDialogTitleL();
        }	
    return title;
    }
	
// --------------------------------------------------------------------------
// CPbk2AddressSelect::LoadDialogTitleL
// --------------------------------------------------------------------------
//
inline HBufC* CPbk2AddressSelect::LoadDialogTitleL()
    {
    HBufC* title = NULL;
    switch( iParams.iCommMethod )
        {
        case VPbkFieldTypeSelectorFactory::EVoiceCallSelector:
        case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
            {
            title = GetVoiceAndVOIPCallDialogTitleL();
            break;
            }
        	
        case VPbkFieldTypeSelectorFactory::EUniEditorSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_MESSAGE );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EEmailEditorSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_EMAIL );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_CHAT );
            break;
            }    
        case VPbkFieldTypeSelectorFactory::EURLSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_URL );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_VIDEO_CALL );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EFindOnMapSelector:
            {
            if ( AreGeoFieldsForAddressesL() )
            	{
                title = StringLoader::LoadL( R_QTN_CCA_POPUP_SHOW_ON_MAP );
            	}
            else
            	{
            	title = StringLoader::LoadL( R_QTN_CCA_POPUP_FIND_ON_MAP );
            	}
            break;
            }
        case VPbkFieldTypeSelectorFactory::EAssignFromMapSelector:
            {
            title = StringLoader::LoadL( R_QTN_CCA_POPUP_ASSIGN_FROM_MAP );
            break;
            }
        case VPbkFieldTypeSelectorFactory::EPocSelector:
            {
            title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_PUSH_TO_TALK );
            break;
            }       
        default:
            {
            HBufC* entryTitle = iParams.iNameFormatter.GetContactTitleL
                ( iParams.iContact.Fields(), KDefaultTitleFormat );
            CleanupStack::PushL( entryTitle );
            
            if ( iParams.iTitleResId )
                {
                if ( iParams.iIncludeContactNameInPrompt )
                    {
                    title = StringLoader::LoadL
                        ( iParams.iTitleResId, *entryTitle );
                    }
                else
                    {
                    title = StringLoader::LoadL
                        ( iParams.iTitleResId );
                    }
                CleanupStack::PopAndDestroy( entryTitle );
                }
            else
                {
                title = entryTitle; // takes ownership of entryTitle
                CleanupStack::Pop( entryTitle );
                entryTitle = NULL;
                }
            break;
            }
        }
    
    return title;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::AreGeoFieldsForAddressesL
// --------------------------------------------------------------------------
//
TBool CPbk2AddressSelect::AreGeoFieldsForAddressesL()
    {
    TBool result = EFalse;
    TBool generalAddress = ETrue;
    TBool homeAddress = ETrue;
    TBool workAddress = ETrue;
    TInt startIndex = 0;

    TPbk2StoreContactAnalyzer analyzer( iParams.iContactManager, NULL );

    TInt countFields = iParams.iContact.Fields().FieldCount();
    for ( TInt i = 0; i < countFields; i++ )
        {
        const MVPbkStoreContactField& field =
            iParams.iContact.Fields().FieldAt( i );
        TInt countProps =
            field.BestMatchingFieldType()->VersitProperties().Count();
        TArray<TVPbkFieldVersitProperty> props =
            field.BestMatchingFieldType()->VersitProperties();
        for ( TInt ii = 0; ii < countProps; ii++ )
            {
            if ( props[ ii ].Name() == EVPbkVersitNameADR )
                {
                if ( props[ ii ].Parameters().Contains(
                    	EVPbkVersitParamHOME ) )
                    {
                    homeAddress = EFalse;
                    }
                else if ( props[ ii ].Parameters().Contains(
                    	EVPbkVersitParamWORK ) )
                    {
                    workAddress = EFalse;
                    }
                else
                    {
                    generalAddress = EFalse;
                    }
                }
            }
        }

    if ( !homeAddress )
        {
        if ( analyzer.HasFieldL( R_PHONEBOOK2_HOME_GEO_SELECTOR,
        		startIndex, &iParams.iContact ) != KErrNotFound )
        	{
        	homeAddress = ETrue;
        	}
        }
    if ( !workAddress )
        {
        if ( analyzer.HasFieldL( R_PHONEBOOK2_WORK_GEO_SELECTOR,
        		startIndex, &iParams.iContact ) != KErrNotFound )
        	{
        	workAddress = ETrue;
        	}
        }
    if ( !generalAddress )
        {
        if ( analyzer.HasFieldL( R_PHONEBOOK2_GENERAL_GEO_SELECTOR,
        		startIndex, &iParams.iContact ) != KErrNotFound )
        	{
        	generalAddress = ETrue;
        	}
        }

    if ( generalAddress && homeAddress && workAddress )
    	{
    	result = ETrue;
    	}
    else
    	{
    	result = EFalse;
    	}

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AddressSelect::GetVOIPDialogTitleL
// --------------------------------------------------------------------------
//
inline HBufC* CPbk2AddressSelect::GetVOIPDialogTitleL()
    {
    //Usecase : If we have only one voip service, the voip(Internet Call)
    //dialog title must be "ServiceName" appended with "call:". 
    //eg : If we have a service named SKYPE installed in the Phone
    //and if SKYPE supports VOIP, dialog title should be 
    //"SKYPE call:". 
    //If we have more than one voip service, then the VOIP dialog title must be
    //should be R_QTN_PHOB_TITLE_POPUP_VOIP_CALL 
    //as defined in the rss   
    
    //Check whether we have more than one voip service configured
    //in the Phone
    HBufC* title (NULL);
    TPtrC tempTitle;
    RIdArray idArray;
    CleanupClosePushL(idArray);
    CSPSettings* settings = CSPSettings::NewLC();
    TInt numServSupportInternetCall (0); //Num of Service that support InternetCall
    TServiceId serviceId (0); //Stores the last found Service Id of VOIP Service
    TInt findResult = settings->FindServiceIdsL(idArray);
    if ( findResult == KErrNone )
    	{
	    for (TInt i = 0; i < idArray.Count(); ++i)
	        {
	        TBool supported( EFalse );
	        CSPEntry* entry = CSPEntry::NewLC();
	        TServiceId id = idArray[i];
	        User::LeaveIfError( settings->FindEntryL(id, *entry) );
	        const CSPProperty* property = NULL;
	        
	        //Mandatory Settings for VOIP
	        if (entry->GetProperty(property, EPropertyServiceAttributeMask) == KErrNone)
	            {
	            TInt value = 0;
	            property->GetValue(value);
	            if ( value & ESupportsInternetCall )
	                {
	                serviceId = id;
	                numServSupportInternetCall++;
	                }
	            }
	        CleanupStack::PopAndDestroy(); // entry
	        }
	    CleanupStack::PopAndDestroy(2); // settings, idArray    
	    
	    if ( KOneVOIPServiceAvailable == numServSupportInternetCall )
	        { 
	        // get the XSP ServiceName 
	        // CPbk2ServiceManager stores all the brandinfo
	        // related to the services configured to the phone
	        // use this to show uniform icon & name throughout PhoneBook
	        MPbk2ApplicationServices2* servicesExtension = 
	            reinterpret_cast<MPbk2ApplicationServices2*>
	                ( Phonebook2::Pbk2AppUi()->ApplicationServices().
	                    MPbk2ApplicationServicesExtension(
	                        KMPbk2ApplicationServicesExtension2Uid ) );
	        CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
	        const CPbk2ServiceManager::RServicesArray& services = servMan.Services();    
	        for ( TInt i = 0; i < services.Count(); i++ )
	            {
	            const CPbk2ServiceManager::TService& service = services[i];
	            //Found the appropriate service info
	            if ( service.iServiceId == serviceId )
	                {
	                tempTitle.Set( service.iDisplayName );
	                break;
	                }
	            }            
	        }
    	}
    if ( tempTitle.Length() )
        {
        title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_VOIP_CALL_SINGLE_SERVICE, 
                tempTitle);
        }
    else
        {
        title = StringLoader::LoadL( R_QTN_PHOB_TITLE_POPUP_VOIP_CALL );
        }
    
    return title;
    }
    
// End of File
