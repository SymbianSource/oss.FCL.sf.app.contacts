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
* Description:  Phonebook 2 contact info view control.
*
*/


// INCLUDE FILES

#include <CPbk2ContactInfoControl.h>

// Phonebook 2
#include "CPbk2ContactInfoListBox.h"
#include "CPbk2ContactInfoDataSwapper.h"
#include "CPbk2ContactFieldDynamicProperties.h"
#include "CPbk2ThumbnailLoader.h"
#include "Pbk2TitlePanePictureFactory.h"
#include <pbk2uicontrols.rsg>
#include <Pbk2UIControls.hrh>
#include <CPbk2IconArray.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2ViewState.h>
#include <MPbk2ControlObserver.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <MPbk2ContactNavigation.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreSpecificFieldPropertyArray.h>
#include <MPbk2Command.h>
#include <Pbk2Profile.h>

// Virtual Phonebook
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldSelector.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactFieldTextData.h>
#include <vpbkeng.rsg>

// System includes
#include <akntitle.h>
#include <AknsConstants.h>
#include <avkon.hrh>
#include <barsread.h>
#include <bautils.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KNumberOfControls = 1;
const TInt KNoContacts = 0;
const TInt KNumberOfContacts = 1;
const TInt KDefaultTitleFormat = MPbk2ContactNameFormatter::EUseSeparator;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_FocusedField = 1,
    EPanicLogic_FocusedField,
    EPanicPreCond_FocusedFieldIndex,
    EPanicPostCond_FocusedFieldIndex,
    EPanicPreCond_FieldPos,
    EPanicPreCond_OfferKeyEventL,
    EPanicPreCond_SizeChanged,
    EPanicPreCond_CountComponentControls,
    EPanicPreCond_ComponentControl
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2ContactInfoControl");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::CPbk2ContactInfoControl
// --------------------------------------------------------------------------
//
CPbk2ContactInfoControl::CPbk2ContactInfoControl(
        CVPbkContactManager& aManager,
        MPbk2ContactNameFormatter& aNameFormatter,
        MPbk2FieldPropertyArray& aFieldProperties,
        CPbk2StorePropertyArray& aStoreProperties,
        MPbk2ContactNavigation* aNavigator ) :
    iContactManager( aManager ),
    iNameFormatter( aNameFormatter ),
    iNavigator( aNavigator ),
    iFieldProperties( aFieldProperties ),
    iStoreProperties( aStoreProperties ),
    iFocusedFieldIndex( KErrNotFound )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::~CPbk2ContactInfoControl
// --------------------------------------------------------------------------
//
CPbk2ContactInfoControl::~CPbk2ContactInfoControl()
    {
    if( iTitlePane )
        {
        iTitlePane->SetSmallPicture( NULL, NULL, EFalse );
        }
    if (iCommand)
        {
        // inform the command that the control is deleted
        iCommand->ResetUiControl(*this);
        }
    iObservers.Reset();
    delete iThumbnailLoader;
    delete iListBox;
    delete iContact;
    delete iStoreContact;
    delete iRetriever;
    delete iContactLink;
    delete iSpecificFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoControl::ConstructL(
        const CCoeControl* aContainer)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoControl::ConstructL begin"));

    SetContainerWindowL(*aContainer);

    // Create the listbox
    iListBox = CPbk2ContactInfoListBox::NewL(
        *this, R_PBK2_CONTACT_INFO_CONTROL, iContactManager);
    iListBox->SetObserver(this);
    iListBox->SetScrollEventObserver( this );
    iListBox->SetListBoxObserver( this );

    // Get flags from listbox
    iFlags = iListBox->Flags();

    // Create thumbnail loader
    iThumbnailLoader = CPbk2ThumbnailLoader::NewL( iContactManager );

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if( statusPane && statusPane->
        PaneCapabilities( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        iTitlePane = static_cast<CAknTitlePane*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoControl::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactInfoControl* CPbk2ContactInfoControl::NewL
        ( const CCoeControl* aContainer,
          CVPbkContactManager& aManager,
          MPbk2ContactNameFormatter& aNameFormatter,
          MPbk2FieldPropertyArray& aFieldProperties,
          CPbk2StorePropertyArray& aStoreProperties,
          MPbk2ContactNavigation* aNavigator )
    {
    CPbk2ContactInfoControl* self = new ( ELeave ) CPbk2ContactInfoControl
        ( aManager, aNameFormatter, aFieldProperties, aStoreProperties,
          aNavigator );
    CleanupStack::PushL( self );
    self->ConstructL( aContainer );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::AddObserverL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactInfoControl::AddObserverL
        (MPbk2ControlObserver& aObserver)
    {
    User::LeaveIfError(iObservers.Append(&aObserver));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ContactInfoControl::RemoveObserver
        (MPbk2ControlObserver& aObserver)
    {
    const TInt index = iObservers.Find(&aObserver);
    if (index >= 0)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2ContactInfoControl::ParentControl() const
    {
    // Contact info control has no parent control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::NumberOfContacts() const
    {
    return KNumberOfContacts;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2ContactInfoControl::FocusedContactL() const
    {
    return iContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2ContactInfoControl::FocusedViewContactL() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2ContactInfoControl::FocusedStoreContact() const
    {
    return iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetFocusedContactL(
        const MVPbkBaseContact& aContact)
    {
    // Load the new contact
    delete iContactLink;
    iContactLink = NULL;
    iContactLink = aContact.CreateLinkLC();
    CleanupStack::Pop(); // iContactLink

    RetrieveContactL( *iContactLink );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetFocusedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetFocusedContactL(
        const MVPbkContactLink& aContactLink )
    {
    // Load the new contact
    delete iContactLink;
    iContactLink = NULL;
    iContactLink = aContactLink.CloneLC();
    CleanupStack::Pop(); // iContactLink

    RetrieveContactL( *iContactLink );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::FocusedContactIndex() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetFocusedContactIndexL( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::NumberOfContactFields() const
    {
    return iContact->Fields().FieldCount();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CPbk2ContactInfoControl::FocusedField() const
    {
    // PreCond:
    __ASSERT_DEBUG(iListBox && iFields, Panic(EPanicPreCond_FocusedField));

    const MVPbkBaseContactField* ret = NULL;
    const TInt fieldsCount = iFields->FieldCount();
    const TInt index = iListBox->CurrentItemIndex();
    TInt numberOfVisibleFields = 0;
    if (index >= 0)
        {
        __ASSERT_DEBUG(index < iFields->FieldCount(),
            Panic(EPanicLogic_FocusedField));
        
        // Field collection may have hidden fields, so find visible field
        // corresponding to the focused field in the listbox.
        for ( TInt i = 0; i < fieldsCount && ret == NULL; i++ )
            {
            if ( iFields->At( i ).IsVisibleInDetailsView() && 
                iFields->At( i ).DynamicVisibility() )
                {
                numberOfVisibleFields++;
                }
            if ( numberOfVisibleFields - 1 == index )
                {
                ret = &iFields->FieldAt( i );
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::FocusedFieldIndex() const
    {
    // PreCond:
    __ASSERT_DEBUG(iContact, Panic(EPanicPreCond_FocusedFieldIndex));

    return iListBox->CurrentItemIndex();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetFocusedFieldIndex(
        TInt aIndex )
    {
    iFocusedFieldIndex = aIndex;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoControl::ContactsMarked() const
    {
    // This control does not support contacts marking
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2ContactInfoControl::SelectedContactsL() const
    {
    // No selection support in contact info control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2ContactInfoControl::SelectedContactsOrFocusedContactL() const
    {
    CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();
    const MVPbkBaseContact* focusedContact = FocusedContactL();
    if ( focusedContact )
        {
        MVPbkContactLink* link = focusedContact->CreateLinkLC();
        CleanupStack::Pop(); // link
        array->AppendL( link );
        }
    CleanupStack::Pop(); // array

    return array;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
    CPbk2ContactInfoControl::SelectedContactsIteratorL() const
    {
    // No selection support in contact info control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2ContactInfoControl::SelectedContactStoresL() const
    {
    // This is not a contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::ClearMarks()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetSelectedContactL(
        TInt /*aIndex*/,
        TBool /*aSelected*/)
    {
    // Not supported in Contact Info
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetSelectedContactL(
        const MVPbkContactBookmark& /*aContactBookmark*/,
        TBool /*aSelected*/ )
    {
    // Not supported
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetSelectedContactL(
        const MVPbkContactLink& /*aContactLink*/,
        TBool /*aSelected*/ )
    {
    // Not supported
    }

TInt CPbk2ContactInfoControl::CommandItemCount() const
	{
	return 0; // No command items.
	}

const MPbk2UiControlCmdItem& CPbk2ContactInfoControl::CommandItemAt(
	TInt /*aIndex*/ ) const
	{
	// There are no command items in this control
	__ASSERT_ALWAYS( EFalse, User::Panic( _L("Pbk2"), 4)); //TODO
	MPbk2UiControlCmdItem* item = NULL; // For compiler only
	return *item; // For compiler only
	}

const MPbk2UiControlCmdItem* CPbk2ContactInfoControl::FocusedCommandItem() const
	{
	return NULL;
	}

void CPbk2ContactInfoControl::DeleteCommandItemL( TInt /*aIndex*/ )
	{
	// Do nothing
	}

void CPbk2ContactInfoControl::AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
	{	
	//Do nothing, since there shouldn't be any command items in this state.
	}

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/) const
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::ProcessCommandL(
        TInt /*aCommandId*/) const
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::UpdateAfterCommandExecution()
    {
    if( iCommand )
        {
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
        }
    iListBox->DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::GetMenuFilteringFlagsL() const
    {
    TInt ret = KPbk2MenuFilteringFlagsNone;

    const TInt fieldCount = FieldCount();
    User::LeaveIfError( fieldCount );
    if ( fieldCount > KNoContacts )
        {
        ret |= KPbk2ListContainsItems;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2ContactInfoControl::ControlStateL() const
    {
    CPbk2ViewState* state = CPbk2ViewState::NewLC();
    const MVPbkBaseContact* contact = FocusedContactL();

    if ( contact )
        {
        MVPbkContactLink* link = contact->CreateLinkLC();
        state->SetFocusedContact(link);
        CleanupStack::Pop(); // link
        link = NULL; // state has the ownership

        if ( iListBox->Model()->NumberOfItems() > 0 )
            {
            state->SetFocusedFieldIndex( FocusedFieldIndex() );
            const TInt topIndex = iListBox->TopItemIndex();
            if ( topIndex >= 0 )
                {
                const MVPbkStoreContactField& topField =
                    iFields->FieldAt( topIndex );
                state->SetTopFieldIndex( iContact->PresentationFields().
                    FindFieldIndex( topField ) );
                }
            }
        }

    CleanupStack::Pop(); // state
    return state;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::RestoreControlStateL(
        CPbk2ViewState* aState )
    {
    const TInt numItems = iListBox->Model()->NumberOfItems();
    const TInt currentItemIndex = iListBox->CurrentItemIndex();

    TInt topFieldIndex( KErrNotFound );
    if ( aState )
        {
        if ( iFocusedFieldIndex == KErrNotFound )
            {
            iFocusedFieldIndex = aState->FocusedFieldIndex();
            topFieldIndex = aState->TopFieldIndex();
            }
        }

    if ( iFocusedFieldIndex >= 0 &&
         iFocusedFieldIndex < numItems )
        {
        iListBox->SetCurrentItemIndex(iFocusedFieldIndex);
        if ( topFieldIndex >= 0 )
            {
            iListBox->SetTopItemIndex( topFieldIndex );
            }
        iListBox->UpdateScrollBarsL();
        }
    else
        {
        iListBox->Reset();

         // Get the field type
        TResourceReader resReader;
        CCoeEnv::Static()->CreateResourceReaderLC
            (resReader, R_PHONEBOOK2_SIP_MSISDN_SELECTOR );

        CVPbkFieldTypeSelector* selector =
            CVPbkFieldTypeSelector::NewL(resReader, iContactManager.FieldTypes());
        CleanupStack::PopAndDestroy(); // resReader
        CleanupStack::PushL( selector );

        const TInt count( iFields->FieldCount() );
        for ( TInt i( 0 ); i < count; ++i )
            {
            const MVPbkFieldType* field = iFields->At( i ).BestMatchingFieldType();
            if ( selector->IsFieldTypeIncluded( *field ) )
                {
                iListBox->SetCurrentItemIndex( i );
                iFocusedFieldIndex = i;
                iListBox->UpdateScrollBarsL();
                break;
                }
            }
        CleanupStack::PopAndDestroy( selector );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactInfoControl::FindTextL()
    {
    // No find box in this control
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::ResetFindL()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::ShowThumbnail()
    {
    if (iThumbnailLoader && IsFocused() )
        {
        iThumbnailLoader->LoadThumbnail
            ( iStoreContact, iListBox, ELocationUpDown );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::HideThumbnail()
    {
    if (iThumbnailLoader)
        {
        iThumbnailLoader->HideThumbnail();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetBlank(TBool /*aBlank*/)
    {
    // This control does not support blanking
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::RegisterCommand(
        MPbk2Command* aCommand)
    {
    iCommand = aCommand;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SetTextL( const TDesC& aText )
    {
    iListBox->View()->SetListEmptyTextL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactInfoControl::ContactUiControlExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        { 
		return static_cast<MPbk2ContactUiControl2*>( this );
		}
		
    return NULL;
    }


// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::UpdateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::UpdateL(
        MVPbkStoreContact* aContact)
    {
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateL);

    // Update specific field properties first
    delete iSpecificFieldProperties;
    iSpecificFieldProperties = NULL;

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCreateStoreSpecificFieldProps);
    iSpecificFieldProperties = CPbk2StoreSpecificFieldPropertyArray::NewL(
        iFieldProperties,
        iStoreProperties,
        iContactManager.FieldTypes(),
        aContact->ParentStore());
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCreateStoreSpecificFieldProps);

    // Create an presentation contact from the store contact
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCreatePresentationContact);
    CPbk2PresentationContact* presentation =
        CPbk2PresentationContact::NewL(*aContact, *iSpecificFieldProperties);
    CleanupStack::PushL(presentation);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCreatePresentationContact);
    
    // Create title pane picture
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCreateTitlePanePicture);
    CEikImage* image = Pbk2TitlePanePictureFactory::
        CreateTitlePanePictureLC( aContact, iStoreProperties );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCreateTitlePanePicture);

    // Instantiate a helper object
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCreateSwapper);
    CPbk2ContactInfoDataSwapper* swapper =
        new(ELeave) CPbk2ContactInfoDataSwapper;
    CleanupStack::PushL(swapper);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCreateSwapper);

    // Store the members which are to be swapped
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLStorePanes);
    StorePanesL(*swapper, presentation);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLStorePanes);

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCreateDynamicFieldProperties);
    CPbk2ContactFieldDynamicProperties* filter =
        CreateDynamicFieldPropertiesLC( *presentation );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCreateDynamicFieldProperties);

    // Prepare listbox for state transitition
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLPrepareForUpdate);
    iListBox->PrepareForUpdateL
        ( *swapper, presentation->PresentationFields(),
            *iSpecificFieldProperties, filter );
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLPrepareForUpdate);

    CleanupStack::Pop();  // filter

    // All code that might leave has been executed succesfully, change state
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLSetTitlePane);
    if( iTitlePane )
        {
        // Set title pane text
        if( swapper->iTitlePaneText )
            {
            iTitlePane->SetText(swapper->iTitlePaneText);
            swapper->iTitlePaneText = NULL;
            }
        // Set title pane picture
        // picture ownership is transfered to title pane
        image->SetPictureOwnedExternally( ETrue );
        iTitlePane->SetSmallPicture(
            image->Bitmap(), image->Mask(), image->Bitmap() != NULL );
        }
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLSetTitlePane);

    // Update listbox
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLUpdateListbox);
    TRAPD(err, iListBox->DoUpdateL(*swapper, *aContact));
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLUpdateListbox);

    // Cleanup and change state
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLCleanupState);
    CleanupStack::PopAndDestroy(swapper);
    // bitmap and mask from image were transfered to title pane,
    // so image can be destroyed now
    CleanupStack::PopAndDestroy( image );
    CleanupStack::Pop(presentation);
    delete iContact;
    iContact = presentation;
    iFields = &presentation->PresentationFields();
    User::LeaveIfError(err);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLCleanupState);

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLSetIndex);
    if ( iFocusedFieldIndex >= 0 &&
         iFocusedFieldIndex < iListBox->Model()->NumberOfItems() )
        {
        iListBox->SetCurrentItemIndex(iFocusedFieldIndex);
        }
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLSetIndex);

    // Update navigators
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLUpdateNavigators);
    if ( iNavigator )
        {
        MVPbkContactLink* contactLink = aContact->CreateLinkLC();
        if ( contactLink )
            {
            iNavigator->UpdateNaviIndicatorsL(*contactLink);
            }
        CleanupStack::PopAndDestroy(); // contactLink
        }
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLUpdateNavigators);

    DrawDeferred();

    // To avoid double deletion, the store contact ownership
    // is transferred after everything that can leave has been
    // executed
    delete iStoreContact;
    iStoreContact = aContact;
            
    // Notify control readiness
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoControlUpdateLSendEvents);
    TRAPD( error, SendEventToObserversL( TPbk2ControlEvent::EReady ) );
    if ( error != KErrNone )
        {
        // When function leaves the aContact will be
        // deleted through CleanupStack by calling function.
        iStoreContact = NULL;
        User::Leave( error );
        }

    SetFocus( ETrue );
    
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateLSendEvents);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoControlUpdateL);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FieldCount
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::FieldCount() const
    {
    TInt ret = KErrNotReady;
    if ( iFields )
        {
        ret = iFields->FieldCount();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FieldPos
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::FieldPos(
        const MVPbkStoreContactField& aField) const
    {
    // PreCond:
    __ASSERT_DEBUG(iListBox && iFields, Panic(EPanicPreCond_FieldPos));

    TInt ret = KErrNotFound;
    const TInt count(iFields->FieldCount());
    for (TInt i = 0; i < count; ++i)
        {
        if (iFields->At(i).IsSame(aField))
            {
            ret = i;
            break;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//

void CPbk2ContactInfoControl::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact)
    {
    if (aContact && &aOperation == iRetriever)
        {
        // contact loading completed, update control data
        TRAPD(err, UpdateL(aContact));
        if (err != KErrNone)
            {
            delete aContact;
            CCoeEnv::Static()->HandleError(err);
            }

        delete iRetriever;
        iRetriever = NULL;
        delete iContactLink;
        iContactLink = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError)
    {
    CCoeEnv::Static()->HandleError(aError);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoControl::FocusedItemPointed()
    {
    return iListBox->FocusedItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoControl::FocusableItemPointed()
    {
    return iListBox->FocusableItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoControl::SearchFieldPointed()
    {
    return iListBox->SearchFieldPointed();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2ContactInfoControl::OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_OfferKeyEventL));

    if ( aType == EEventKey )
        {
        // Reset iFocusedFieldIndex
        // UpdateL method called after editor is closed and correct field
        // is focused. Focus jump, if user same time than UpdateL method is
        // called, scrolling fields. That's why this iFocusedFieldIndex
        // reseting.
        iFocusedFieldIndex = KErrNotFound;
        }

    // Forward all key events to the list box
    TKeyResponse response = iListBox->OfferKeyEventL(aKeyEvent,aType);
    
    // Ignore Send Key up and down events to prevent Dialer appearance
    // on top of Phonebook application.
    if ( response == EKeyWasNotConsumed && IsFocused() )
        {
        if ( ( aType == EEventKeyDown || aType == EEventKeyUp )
            && aKeyEvent.iScanCode == EStdKeyYes )
            {
            response = EKeyWasConsumed;
            }
        }
        
/*
HandleControlEventL does this already...no need for double update...plus the HandleControlEventL works also for touch...
    // Let the listbox first handle the key event before drawing the
    // thumbnail.
    if ( iThumbnailLoader && aType == EEventKey &&
        (aKeyEvent.iScanCode == EStdKeyUpArrow ||
        aKeyEvent.iScanCode == EStdKeyDownArrow ))
        {
        RDebug::Print( _L("### info view: OfferKeyEventL!") );
        iThumbnailLoader->Refresh();
        }
*/
    return response;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SizeChanged()
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_SizeChanged));
    // Listbox occupies whole area of this control
    iListBox->SetRect(Rect());
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoControl::CountComponentControls() const
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_CountComponentControls));
    
    // Always only one child control: the list box.
    return KNumberOfControls;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2ContactInfoControl::ComponentControl(
        TInt PBK2_DEBUG_ONLY(aIndex)) const
    {
    __ASSERT_DEBUG(iListBox && aIndex==0,
        Panic(EPanicPreCond_ComponentControl));

    // Return the one and only child control
    return iListBox;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::FocusChanged
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::FocusChanged(
        TDrawNow aDrawNow )
    {
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }

    if ( IsFocused() )
        {
        ShowThumbnail();
        }
    else
        {
        HideThumbnail();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::HandleResourceChange(
        TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    TRAP_IGNORE(DoHandleResourceChangeL(aType));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    if ( AknLayoutUtils::PenEnabled() )
        {
        switch( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Up:
                {
                if ( iListBox != NULL && iListBox->Rect().Contains(
                    aPointerEvent.iPosition ) )
                    {
                    iFocusedFieldIndex = iListBox->CurrentItemIndex();
                    }
                break;
                }

            default:
                {
                break;
                }
            };
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::HandleControlEventL(
        CCoeControl* aControl,
        TCoeEvent aEventType)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoControl::HandleControlEventL(0x%x,0x%x,%d)"),
        this, aControl, aEventType);
    if ( iThumbnailLoader && (aControl == iListBox) && IsFocused() )
        {
        iThumbnailLoader->Refresh();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::StorePanesL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoControl::StorePanesL(
        CPbk2ContactInfoDataSwapper& aSwapper,
        const CPbk2PresentationContact* aContact)
    {
    if( iFlags & KPbk2ContactInfoControlUpdateStatusPane )
        {
        if( (iFlags & KPbk2ContactInfoControlUpdateTitlePane) && iTitlePane )
            {
            // Store title pane text
            if (aContact)
                {
                aSwapper.iTitlePaneText = iNameFormatter.GetContactTitleL(
                    aContact->Fields(), KDefaultTitleFormat);
                }
            }
        if( iFlags & KPbk2ContactInfoControlUpdateContextPane )
            {
            if (aContact)
				{
	            iThumbnailLoader->LoadThumbnail(&aContact->StoreContact(),
	                                            iListBox,
	                                            ELocationUpDown);
				}
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::SendEventToObserversL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::SendEventToObserversL(
        const TPbk2ControlEvent& aEvent)
    {
    // Loop backwards in case some observer destroys itself in the
    // event handler
    for (TInt i = iObservers.Count()-1; i >= 0; --i)
        {
        iObservers[i]->HandleControlEventL(*this, aEvent);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::CreateDynamicFieldPropertiesLC
// --------------------------------------------------------------------------
//
CPbk2ContactFieldDynamicProperties*
    CPbk2ContactInfoControl::CreateDynamicFieldPropertiesLC(
        CPbk2PresentationContact& aContact )
    {
    CPbk2ContactFieldDynamicProperties* filter =
        CPbk2ContactFieldDynamicProperties::NewLC();
    // Check is there ringtone field and if there is, check that file is still
    // available. If not, hide the field.
    TInt fieldCount( aContact.Fields().FieldCount() );
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field = aContact.Fields().FieldAt( i );
        TInt resId( field.BestMatchingFieldType()->FieldTypeResId() );
        if ( resId == R_VPBK_FIELD_TYPE_RINGTONE )
            {
            TPtrC filename =
                MVPbkContactFieldTextData::Cast( field.FieldData() ).Text();
            if ( !BaflUtils::FileExists(
                CCoeEnv::Static()->FsSession(),
                filename ) )
                {
                filter->AddHiddenFieldResourceId( resId );
                aContact.PresentationFields().At( i ).
                    SetDynamicVisibility( EFalse );
                break;
                }
            }
        }

    return filter;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::DoHandleResourceChangeL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::DoHandleResourceChangeL(
        TInt aType)
    {
    if (iListBox)
        {
        iListBox->DoHandleResourceChangeL(aType);
        }
/*  This is useless...no need to show or hide thumbnail on layout/skin change...thumbnail will handle it's own resouce change event
    if ( aType == KEikDynamicLayoutVariantSwitch ||
         aType == KAknsMessageSkinChange )
        {
        if ( IsFocused() )
            {
            RDebug::Print( _L("### InfoControl::HandleResourceChangeL - show") );
            ShowThumbnail();
            }
        else
            {
            RDebug::Print( _L("### InfoControl::HandleResourceChangeL - hide") );
            HideThumbnail();
            }
        }*/
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::RetrieveContactL
// --------------------------------------------------------------------------
//    
void CPbk2ContactInfoControl::RetrieveContactL( 
        const MVPbkContactLink& aContactLink )
    {
    delete iRetriever;
    iRetriever = NULL;
    iRetriever = iContactManager.RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------
//    
void CPbk2ContactInfoControl::HandleScrollEventL(CEikScrollBar* /*aScrollBar*/, TEikScrollEvent aEventType)
    {


    switch( aEventType )
        {
        // drag events -> hide thumbnail
        // AL: this isn't very efficient way of doing this...
        // ...thumbnail loader interface should be extended to 
        // enable thumbnail window to be hidden/shown without 
        // destroying and reloading the bitmaps...
        case EEikScrollThumbDragHoriz:
        case EEikScrollThumbDragVert:
            {
            iDidMove = ETrue;
            HideThumbnail();
            break;
            }
            
        // release events
        case EEikScrollThumbReleaseHoriz:
        case EEikScrollThumbReleaseVert:
            {
            // AL: this isn't very efficient way of doing this...
            // ...thumbnail loader interface should be extended to 
            // enable thumbnail window to be hidden/shown without 
            // destroying and reloading the bitmaps...
            if( iDidMove )
                {
                ShowThumbnail();
                iDidMove = EFalse;
                }
            break;
            }

        default:
            {
            iDidMove= EFalse;
            if( iThumbnailLoader )
                {
                iThumbnailLoader->Refresh();
                }
            }
        }
    }
// --------------------------------------------------------------------------
// CPbk2ContactInfoControl::HandleListBoxEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoControl::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventItemDoubleClicked:
            SendEventToObserversL( TPbk2ControlEvent::EContactDoubleTapped );
            break;
        case EEventItemSingleClicked:
            SendEventToObserversL( TPbk2ControlEvent::EContactTapped );
            break;   
        case EEventEnterKeyPressed:
            SendEventToObserversL( TPbk2ControlEvent::EControlEnterKeyPressed );
            break;  
        default:
            ;
        }
    }


//  End of File
