/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of detailsview plugin control container
*
*/

// INCLUDE FILES
#include <aknlists.h>
#include <bautils.h>
#include <aknlayoutscalable_avkon.cdl.h>

#include "ccappdetailsviewcontainer.h"
#include "ccappdetailsviewlistboxmodel.h"
#include "ccappdetailsviewdefs.h"

#include <ccappdetailsviewpluginrsc.rsg>
#include <ccappdetailsviewplugin.mbg>

#include <csxhelp/phob.hlp.hrh>
#include <ccacontactorservice.h>
#include <ccappdetailsviewplugin.h>

// PhoneBook2
#include <pbk2uicontrols.rsg>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <CPbk2IconArray.h>
#include <CPbk2IconInfoContainer.h>
#include <MVPbkBaseContact.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionIconSupport.h>
#include <CPbk2ApplicationServices.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <MPbk2Command.h>
#include <CPbk2UIExtensionManager.h>
#include "CPbk2ServiceManager.h"
#include "CPbk2IconInfo.h"
#include <CPbk2CommandHandler.h>
#include <pbk2mapcommands.hrh>

#include <Pbk2UID.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <VPbkFieldTypeSelectorFactory.h>
#include <MVPbkFieldType.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>

#include <mccaparameter.h>
#include <mccappengine.h>
#include <vpbkeng.rsg>

/// Unnamed namespace for local definitions
namespace {

    const TInt KNumberOfContacts = 1;
    const TInt KNoContacts = 0;

#ifdef _DEBUG
    enum TPanicCode
    {
        EPanicPreCond_FocusedField = 1,
        EPanicPreCond_FocusedFieldIndex,
    };

    void Panic(TInt aReason)
    {
        _LIT(KPanicText, "CCCAppDetailsViewContainer");
        User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::ConstructL()
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewContainer::ConstructL()"));

    iCommandHandler = CPbk2CommandHandler::NewL();

    iListBox = new(ELeave) CAknFormDoubleGraphicStyleListBox;

    iListBox->ConstructL(this, EAknListBoxLoopScrolling);
    iListBox->View()->SetListEmptyTextL(KNullDesC);
    iListBox->SetContainerWindowL(*this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto);
    iListBox->SetMopParent(this);

    // Create icon array.
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_PBK2_FIELDTYPE_ICONS);
    CPbk2IconArray* iconArray = CPbk2IconArray::NewL(reader);
    CleanupStack::PushL(iconArray);

    // Calculate preferred size for xsp service icons.
    TRect mainPane;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPane);
    TAknLayoutRect listLayoutRect;
    listLayoutRect.LayoutRect(
        mainPane,
        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine());
    TSize size(listLayoutRect.Rect().Size());

    // Add xsp service icons.
    CPbk2ServiceManager& servMan = iAppServices->ServiceManager();

    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();
    TUid uid;
    uid.iUid = KPbk2ServManId;

    for (TInt i = 0; i < services.Count(); i++)
    {
        const CPbk2ServiceManager::TService& service = services[i];
        if (service.iBitmapId && service.iBitmap)
        {
        	AknIconUtils::SetSize(
        	           service.iBitmap,
        	           size );
        	AknIconUtils::SetSize(
        	           service.iMask,
        	           size );
            TPbk2IconId id = TPbk2IconId(uid, service.iBitmapId);
            CPbk2IconInfo* info = CPbk2IconInfo::NewLC(
                id, service.iBitmap, service.iMask, size);
            iconArray->AppendIconL(info);
            CleanupStack::Pop(info);
        }
    }

    // Add icons from UI extensions (e.g. USIM extension)
    // UIExtensionManager handles also e.g. appending SIM store specific
    // properties to StorePropertyArray.
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    iExtensionManager->IconSupportL().AppendExtensionIconsL(*iconArray);

    // Add speed dial and default number icons.
    TResourceReader reader2;
    iCoeEnv->CreateResourceReaderLC(reader2, R_DETAILSVIEW_ICON_ARRAY);

    // create the icon info container from the icon info array
    CPbk2IconInfoContainer* container =
        CPbk2IconInfoContainer::NewL(R_DETAILSVIEW_ICON_INFO_ARRAY);
    CleanupStack::PushL(container);
    // append the icons from the icon info array
    iconArray->AppendIconsFromResourceL(reader2, *container);
    CleanupStack::PopAndDestroy(2); // reader2, container

    CleanupStack::Pop(iconArray);
    CleanupStack::PopAndDestroy(); // reader

    // Set icon array
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    iListBoxModel = CCCAppDetailsViewListBoxModel::NewL(
        *iCoeEnv, *iListBox, iAppServices, *iconArray, iPlugin);
    iListBox->Model()->SetItemTextArray(iListBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Set listbox observer
    iListBox->SetListBoxObserver(this);

    ActivateL();

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewContainer::ConstructL()"));
}

// -----------------------------------------------------------------------------
// CCCAppDetailsViewContainer::CCCAppDetailsViewContainer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCCAppDetailsViewContainer::CCCAppDetailsViewContainer(const TUid& aPluginUid,
    CCCAppDetailsViewPlugin& aPlugin)
:
    iPluginUid(aPluginUid),
    iPlugin(aPlugin)
{
    // No implementation needed.
}

// -----------------------------------------------------------------------------
// CCCAppDetailsViewContainer::~CCCAppDetailsViewContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CCCAppDetailsViewContainer::~CCCAppDetailsViewContainer()
{
    delete iCommandHandler;
    if (iCommand)
    {
        iCommand->ResetUiControl(*this);
    }
    delete iListBoxModel;
    delete iListBox;
    Release(iExtensionManager);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::CountComponentControls() const
{
    return iListBox ? 1 : 0;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CCCAppDetailsViewContainer::ComponentControl(TInt aIndex) const
{
    return ((aIndex == 0) ? iListBox : NULL);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetApplicationServices(
    CPbk2ApplicationServices* aApplicationServices)
{
    iAppServices = aApplicationServices;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::
// --------------------------------------------------------------------------
//
CVPbkContactManager& CCCAppDetailsViewContainer::ContactManager() const
{
    return iAppServices->ContactManager();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ListBoxModel
// --------------------------------------------------------------------------
//
CCCAppDetailsViewListBoxModel& CCCAppDetailsViewContainer::ListBoxModel() const
{
    return *iListBoxModel;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CCCAppDetailsViewContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    // Temporary solution for handling key events.
    TKeyResponse result = EKeyWasNotConsumed;

    /* Uncomment this code if it is necessary to enable navigation hardware keys
     * on devices which have these keys (its Map Extension feature) 
    if( aKeyEvent.iScanCode == EStdKeyApplicationD && aType == EEventKeyUp )
        {
        DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionAssignFromMap );
        returnValue = EKeyWasConsumed;
        }
            
    if( aKeyEvent.iScanCode == EStdKeyApplicationE && aType == EEventKeyUp )
        {
        DoShowMapCmdL( (TPbk2CommandId)EPbk2ExtensionShowOnMap );
        returnValue = EKeyWasConsumed;
        }*/

    switch (aKeyEvent.iCode)
    {
    case EKeyLeftArrow:
    case EKeyRightArrow:
        return result;
    }

    /* On detection of EKeyBackspace or EKeyDelete events we trigger
     * the contact deletion.
     */
    if (( aType == EEventKey) &&
         ( aKeyEvent.iCode == EKeyBackspace || aKeyEvent.iCode == EKeyDelete ))
    {
        iListBoxModel->HandleDeleteRequestL();
        result = EKeyWasConsumed;
        return result;
    }
    
    // Send key event, to launch call
    if ( ( aType == EEventKey ) &&
    		( aKeyEvent.iCode == EKeyYes && IsFocused() ) )
    {
        HandleLaunchCallRequestL();       
        result = EKeyWasConsumed;
        return result;      
    }
    
    if ( result == EKeyWasNotConsumed )
    {
        // Ignore Send Key up and down events to prevent Dialer appearance
        // on top of cca application.
        if (( aType == EEventKeyDown || aType == EEventKeyUp )
            && aKeyEvent.iScanCode == EStdKeyYes )
        {
            result = EKeyWasConsumed;
        }
    }
    
    if ( result == EKeyWasNotConsumed )
    {
        // Pass all not consumed keys to the list box cntrl
        result = iListBox->OfferKeyEventL( aKeyEvent, aType );
    }
    return result;
}

// -----------------------------------------------------------------------------
// CCCAppDetailsViewContainer::HandleListBoxEventL()
// -----------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::HandleListBoxEventL(
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
{
	if ( ( aEventType == EEventItemSingleClicked ) || ( aEventType == EEventEnterKeyPressed ) )
    {
        iListBoxModel->HandleEditRequestL();
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SizeChanged
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SizeChanged()
{
    if (iListBox)
    {
        TRect rect(Rect());
        iListBox->SetRect(rect);
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusChanged
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::FocusChanged(TDrawNow aDrawNow)
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewContainer::FocusChanged()"));

    CCoeControl::FocusChanged(aDrawNow);

    if (iListBox)
    {
        iListBox->SetFocus(IsFocused(), aDrawNow);
    }

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewContainer::FocusChanged()"));
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::HandleResourceChange
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::HandleResourceChange(TInt aType)
{
    //CCA_DP(KDetailsViewLogFile, CCA_L(">>> CCCAppDetailsViewContainer::HandleResourceChange()"));

    // TODO: DoHandleResourceChangeL(aType)
    CCCAppViewPluginAknContainer::HandleResourceChange(aType);

    // TODO: this crashes on orientation switch.
    //       don't know if it works on skin change because there has
    //       been no skins with different field type icons available
    //       for testing.
    /*
     if (aType == KAknsMessageSkinChange ||
     aType == KEikDynamicLayoutVariantSwitch)
     {
     CPbk2IconArray* iconArray =
     static_cast<CPbk2IconArray*>(iListBox->ItemDrawer()->ColumnData()->IconArray());

     if (iconArray)
     {
     // Refresh core Phonebook2 icons
     iconArray->RefreshL();

     CPbk2UIExtensionManager* extManager =
     CPbk2UIExtensionManager::InstanceL();
     extManager->PushL();
     // Refresh extension icons
     extManager->IconSupportL().RefreshL(*iconArray);
     CleanupStack::PopAndDestroy(); // extManager
     }
     }
     */

    //CCA_DP(KDetailsViewLogFile, CCA_L("<<< CCCAppDetailsViewContainer::HandleResourceChange()"));
}

// -----------------------------------------------------------------------------
// CCCAppDetailsViewContainer::GetHelpContext
// -----------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::GetHelpContext(TCoeHelpContext& aContext) const
{
    aContext.iMajor.iUid = KPbk2UID3;
    aContext.iContext = KHLP_CCA_DETAILS;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CCCAppDetailsViewContainer::ParentControl() const
{
    // Contact info control has no parent control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::NumberOfContacts() const
{
    return KNumberOfContacts;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CCCAppDetailsViewContainer::FocusedContactL() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedField));

    return static_cast<MVPbkBaseContact*>(iListBoxModel->StoreContact());
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CCCAppDetailsViewContainer::FocusedViewContactL() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CCCAppDetailsViewContainer::FocusedStoreContact() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedField));

    return iListBoxModel->StoreContact();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetFocusedContactL(
    const MVPbkBaseContact& aContact)
{
    if (!iListBoxModel->StoreContact()->IsSame(aContact))
    {
        MVPbkContactLink* link = aContact.CreateLinkLC();
        HBufC8* packed = link->PackLC();

        CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(
            *packed, ContactManager().ContactStoresL() );
        CleanupStack::Pop(links); 

        iListBoxModel->SetLinks(links);
        DrawDeferred();

        // Update the CCA parameter contact data
        HBufC16* link16 = HBufC16::NewLC(packed->Length());
        link16->Des().Copy( *packed );
        iAppEngine->Parameter().SetContactDataL(*link16);
        iAppEngine->CCAppContactEventL();
        CleanupStack::PopAndDestroy(3); // link16, packed, link
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetFocusedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetFocusedContactL(
    const MVPbkContactLink& /*aContactLink*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::FocusedContactIndex() const
{
    return KErrNotSupported;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetFocusedContactIndexL(TInt /*aIndex*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::NumberOfContactFields() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedField));

    TInt cnt = KErrNotSupported;
    cnt = iListBoxModel->StoreContact()->Fields().FieldCount();
    return cnt;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CCCAppDetailsViewContainer::FocusedField() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedField));

    TInt index = iListBoxModel->FocusedFieldIndex();
    if (index != KErrNotFound)
        {
        return &iListBoxModel->StoreContact()->Fields().FieldAt(index);
        }
    
    return NULL;

}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::FocusedFieldIndex() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedFieldIndex));

    return iListBoxModel->FocusedFieldIndex();
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetFocusedFieldIndex(TInt aIndex)
{
    TInt focusedItemIndex = iListBoxModel->FindFocusListIndexByStoreIndex( aIndex );
    iListBoxModel->SetInitialFocusIndex(focusedItemIndex);
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CCCAppDetailsViewContainer::ContactsMarked() const
{
    // This control does not support contacts marking
    return EFalse;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CCCAppDetailsViewContainer::SelectedContactsL() const
{
    // No selection support in contact info control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
CCCAppDetailsViewContainer::SelectedContactsOrFocusedContactL() const
{
    CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC();
    const MVPbkBaseContact* focusedContact = FocusedContactL();

    if (focusedContact)
    {
        MVPbkContactLink* link = focusedContact->CreateLinkLC();
        CleanupStack::Pop(); // link
        array->AppendL(link);
    }
    CleanupStack::Pop(array);

    return array;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
CCCAppDetailsViewContainer::SelectedContactsIteratorL() const
{
    // No selection support in contact info control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
CCCAppDetailsViewContainer::SelectedContactStoresL() const
{
    // This is not a contact store control
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ClearMarks
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::ClearMarks()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetSelectedContactL(
    TInt /*aIndex*/,
    TBool /*aSelected*/)
{
    // Not supported in Contact Info
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetSelectedContactL(
    const MVPbkContactBookmark& /*aContactBookmark*/,
    TBool /*aSelected*/)
{
    // Not supported
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetSelectedContactL(
    const MVPbkContactLink& /*aContactLink*/,
    TBool /*aSelected*/)
{
    // Not supported
}



// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::DynInitMenuPaneL(
    TInt /*aResourceId*/,
    CEikMenuPane* /*aMenuPane*/) const
{
    // Nothing to do
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ProcessCommandL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::ProcessCommandL(TInt /*aCommandId*/) const
{
    // Nothing to do
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::UpdateAfterCommandExecution()
{
    if (iCommand)
    {
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
    }
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::GetMenuFilteringFlagsL() const
{
    // PreCond:
    __ASSERT_DEBUG(iListBoxModel, Panic(EPanicPreCond_FocusedField));

    const TInt fieldCount =
        iListBoxModel->StoreContact()->Fields().FieldCount();
    // If negative will leave
    User::LeaveIfError(fieldCount);

    TInt ret = KPbk2MenuFilteringFlagsNone;
    if (fieldCount > KNoContacts)
    {
        ret |= KPbk2ListContainsItems;
    }

    return ret;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CCCAppDetailsViewContainer::ControlStateL() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::RestoreControlStateL(
    CPbk2ViewState* /*aState*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CCCAppDetailsViewContainer::FindTextL()
{
    // No find box in this control
    return KNullDesC;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ResetFindL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::ResetFindL()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ShowThumbnail
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::ShowThumbnail()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::HideThumbnail
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::HideThumbnail()
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetBlank
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetBlank(TBool /*aBlank*/)
{
    // This control does not support blanking
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::RegisterCommand
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::RegisterCommand(MPbk2Command* aCommand)
{
    iCommand = aCommand;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::SetTextL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::SetTextL(const TDesC& /*aText*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CCCAppDetailsViewContainer::ContactUiControlExtension(TUid aExtensionUid)
{
    if (aExtensionUid == KMPbk2ContactUiControlExtension2Uid)
    {
        return static_cast<MPbk2ContactUiControl2*>(this);
    }

    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::CommandItemCount
// --------------------------------------------------------------------------
//
TInt CCCAppDetailsViewContainer::CommandItemCount() const
{
    return 0; // No command items.
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::CommandItemAt
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem& CCCAppDetailsViewContainer::CommandItemAt(
    TInt /*aIndex*/) const
{
    // There are no command items in this control
    __ASSERT_ALWAYS(EFalse, User::Panic(_L("Pbk2"), 4)); //TODO
    MPbk2UiControlCmdItem* item = NULL; // For compiler only
    return *item; // For compiler only
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::FocusedCommandItem
// --------------------------------------------------------------------------
//
const MPbk2UiControlCmdItem* CCCAppDetailsViewContainer::FocusedCommandItem() const
{
    return NULL;
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::DeleteCommandItemL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::DeleteCommandItemL(TInt /*aIndex*/)
{
    // Do nothing
}

// --------------------------------------------------------------------------
// CCCAppDetailsViewContainer::AddCommandItemL
// --------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::AddCommandItemL(
    MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/)
{
    //Do nothing, since there shouldn't be any command items in this state.
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewContainer::DoShowMapCmdL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::DoShowMapCmdL(TPbk2CommandId aCommandId)
{
    iCommandHandler->HandleCommandL(aCommandId, *this, NULL);
}

// ---------------------------------------------------------------------------
// CCCAppDetailsViewContainer::HandleLaunchCallRequestL
// ---------------------------------------------------------------------------
//
void CCCAppDetailsViewContainer::HandleLaunchCallRequestL()
    {
    const MVPbkBaseContactField* field = iListBoxModel->FocusedFieldLC();
    TInt fieldTypeId = field->MatchFieldType(0)->FieldTypeResId();
    // Video call selector is used for video call, voice call selector for others
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector commMethod;
    if ( fieldTypeId == R_VPBK_FIELD_TYPE_VIDEONUMBERGEN  || 
         fieldTypeId == R_VPBK_FIELD_TYPE_VIDEONUMBERHOME ||
         fieldTypeId == R_VPBK_FIELD_TYPE_VIDEONUMBERWORK )
        {
        commMethod = VPbkFieldTypeSelectorFactory::EVideoCallSelector;
        }
    else if ( fieldTypeId == R_VPBK_FIELD_TYPE_VOIPHOME || 
              fieldTypeId == R_VPBK_FIELD_TYPE_VOIPWORK ||
              fieldTypeId == R_VPBK_FIELD_TYPE_VOIPGEN  ||
              fieldTypeId == R_VPBK_FIELD_TYPE_IMPP )
        {
        commMethod = VPbkFieldTypeSelectorFactory::EVOIPCallSelector;
        }
    else
        {
        commMethod = VPbkFieldTypeSelectorFactory::EVoiceCallSelector;
        }
	CVPbkFieldTypeSelector* fieldTypeSelctor = 
	                 VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
	                 commMethod, ContactManager().FieldTypes() );
	CleanupStack::PushL( fieldTypeSelctor );
	        
	CCAContactorService* contactorService = iPlugin.ContactorService();
	        
	if ( fieldTypeSelctor->IsFieldIncluded(*field) && contactorService )
	    {
	    // Launch call
	    TPtrC fullName( KNullDesC() );  // Full name not required
	    TUint paramFlag = CCAContactorService::TCSParameter::EUseFieldParam;
	    TPtrC8 contactlinkarray( KNullDesC8() ); // Not required
	    TVPbkFieldStorageType dataType = field->FieldData().DataType();
	    HBufC* selectedFeild = NULL;

	    if ( dataType == EVPbkFieldStorageTypeText )
	         {
	         const MVPbkContactFieldTextData& textData =
	               MVPbkContactFieldTextData::Cast( field->FieldData() );
	         selectedFeild = textData.Text().AllocLC();
	         }      
	    else if ( dataType == EVPbkFieldStorageTypeUri )
	         {
	         const MVPbkContactFieldUriData& textData =
	                    MVPbkContactFieldUriData::Cast( field->FieldData() );
	         selectedFeild = textData.Text().AllocLC();
	         }
	            
	         CCAContactorService::TCSParameter param(
	                commMethod,
	                contactlinkarray,//contactlinkarray
	                paramFlag,
	                fullName,
	                *selectedFeild);
	    
	         contactorService->ExecuteServiceL(param);
	            
	         CleanupStack::PopAndDestroy(); // selectedFeild
	    }
	    CleanupStack::PopAndDestroy( 2 ); // field, fieldTypeSelctor   
	}
// End of File
