/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Provides methods for UI control of phonebook "Contact Info View".
*
*/


// INCLUDE FILES
#include "CPbkContactInfoControl.h"  // Class declarations

#include <barsread.h>    // TResourceReader
#include <akntitle.h>    // CAknTitlePane
#include <aknlists.h>    // CAknDoubleGraphicStyleListBox
#include <cntdef.h>
#include <AknsConstants.h>
#include <AknDef.h>      // KEikDynamicLayoutVariantSwitch
#include <bautils.h>

#include <PbkDebug.h>
#include <pbkview.rsg>           // View resources
#include <PbkView.hrh>
#include "CPbkViewState.h"
#include "CPbkThumbnailPopup.h"
#include "CPbkFieldListBoxModel.h"
#include <CPbkIconArray.h>

#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>

// PbkEng.dll
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkContactItem.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicPreCond_CurrentField = 1,
    EPanicLogic_CurrentField,
    EPanicPostCond_Constructor,
    EPanicPreCond_ConstructFromResourceL,
    EPanicPostCond_ConstructFromResourceL,
    EPanicInvalidListBoxType,
    EPanicPreCond_ContactItem,
    EPanicPreCond_CurrentFieldIndex,
    EPanicPostCond_CurrentFieldIndex,
    EPanicPreCond_FieldPos, // 10
    EPanicPreCond_UpdateL,
    EPanicPostCond_UpdateL,
    EPanicPreCond_OfferKeyEventL,
    EPanicPreCond_SizeChanged,
    EPanicPreCond_CountComponentControls,
    EPanicPreCond_ComponentControl,
    EPanicPreCond_FieldCount,
    EPanicPreCond_ListBox
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkContactInfoControl");
    User::Panic(KPanicText, aReason);
    }

} // namespace

// MODULE DATA STRUCTURES

/**
 * listbox item drawer for CPbkContactInfoControl.
 */
NONSHARABLE_CLASS(CPbkContactInfoControl::CItemDrawer) :
        public CFormattedCellListBoxItemDrawer
    {
    public: // interface
        /**
         * Constructor.
         */
        inline CItemDrawer(MTextListBoxModel* aTextListBoxModel,
                const CFont* aFont,
                CFormattedCellListBoxData* aFormattedCellData);

        /**
         * Sets iFields to aFields.
         */
        inline void SetFields(const CPbkFieldArray& aFields);

    private: // from CListItemDrawer
        TListItemProperties Properties(TInt aItemIndex) const;

    private: // data members
        /// Ref: array of fields.
        const CPbkFieldArray* iFields;
    };

/**
 * Listbox for CPbkContactInfoControl
 */
NONSHARABLE_CLASS(CPbkContactInfoControl::CListBox) :
        public CAknFormDoubleGraphicStyleListBox
    {
    public: // constructor
        inline CListBox();
	    inline CItemDrawer* ItemDrawer() const;
        inline void SetIconArray(CPbkIconArray* aIconArray);
        inline CPbkIconArray* IconArray();
    public: // from CEikFormattedCellListBox
    	void CreateItemDrawerL();

    };

/**
 * Data holding helper class for CPbkContactInfoControl::UpdateL().
 */
NONSHARABLE_CLASS(CPbkContactInfoControl::CUpdateData) :
        public CBase
    {
    public:  // Interface
        // Compiler-generated default constructor is ok for this class
        ~CUpdateData();

        /**
         * Saves data into this object which restores aListBox's state (model,
         * top and current item index) in this objects destructor.
         */
        void StoreListBoxState(CEikTextListBox& aListBox);

        /**
         * Resets any data set with StoreListBoxState().
         */
        inline void ResetListBoxState();

    public:  // Data
        CPbkFieldArray* iFieldArray;
        HBufC* iTitlePaneText;
        CPbkThumbnailPopup* iThumbnailHandler;
        MDesCArray* iListBoxModel;

    private:  // Data
        CEikTextListBox* iListBox;
        MDesCArray* iItemTextArray;
        TInt iRestoreTopItem;
        TInt iRestoreCurrentItem;
    };


/**
 * Helper class for UpdateL module test.
 */
NONSHARABLE_CLASS(CPbkContactInfoControl::CUpdateTestData) :
        public CBase
    {
    public:  // Interface
        void ConstructL(CPbkContactInfoControl& aControl);
        ~CUpdateTestData();
        TBool Compare(CPbkContactInfoControl& aControl) const;

    private:  // Data
        CPbkContactItem* iContactItem;
        TInt iTopIndex;
        TInt iCurrentIndex;
        CAknTitlePane* iTitlePane;
        HBufC* iTitlePaneText;
        CPbkFieldArray* iFieldArray;
        CPbkThumbnailPopup* iThumbnailHandler;
    };

// ==================== LOCAL FUNCTIONS ====================

template<class Type>
inline void Swap(Type& aObj1, Type& aObj2)
    {
    Type tmp = aObj1;
    aObj1 = aObj2;
    aObj2 = tmp;
    }


// ================= MEMBER FUNCTIONS =======================
inline
CPbkContactInfoControl::CItemDrawer::CItemDrawer
        (MTextListBoxModel* aTextListBoxModel,
        const CFont* aFont,
        CFormattedCellListBoxData* aFormattedCellData) :
    CFormattedCellListBoxItemDrawer(
        aTextListBoxModel,
        aFont,
        aFormattedCellData)
    {
    }

inline
void CPbkContactInfoControl::CItemDrawer::SetFields
        (const CPbkFieldArray& aFields)
    {
    iFields = &aFields;
    }

TListItemProperties CPbkContactInfoControl::CItemDrawer::Properties
        (TInt aItemIndex) const
    {
    TListItemProperties result =
        CFormattedCellListBoxItemDrawer::Properties(aItemIndex);

   // TODO: aItemIndex<iFields->Count() and aItemIndex>=0 restrictions below
   // because class' internal data is corrupted,
   // probably in CPbkContactInfoControl::UpdateL
   if (iFields && aItemIndex>=0 && aItemIndex<iFields->Count() &&
        (*iFields)[aItemIndex].DefaultPhoneNumberField())
        {
        // default phonenumber field prompt is underlined
        result.SetUnderlined(ETrue);
        }
    return result;
    }

inline
CPbkContactInfoControl::CListBox::CListBox()
    {
    }

inline
CPbkContactInfoControl::CItemDrawer* CPbkContactInfoControl::CListBox::ItemDrawer
        (  ) const
    {
    return static_cast<CPbkContactInfoControl::CItemDrawer*>(CAknDoubleGraphicStyleListBox::ItemDrawer());
    }

inline
void CPbkContactInfoControl::CListBox::SetIconArray(CPbkIconArray* aIconArray)
    {
    ItemDrawer()->ColumnData()->SetIconArray(aIconArray);
    }

inline
CPbkIconArray* CPbkContactInfoControl::CListBox::IconArray()
    {
    return static_cast<CPbkIconArray*>(ItemDrawer()->ColumnData()->IconArray());
    }

void CPbkContactInfoControl::CListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* formattedData = CFormattedCellListBoxData::NewL();
    CleanupStack::PushL(formattedData);
    iItemDrawer = new(ELeave) CPbkContactInfoControl::CItemDrawer(Model(), iEikonEnv->NormalFont(), formattedData);
    CleanupStack::Pop();
    }

CPbkContactInfoControl::CUpdateData::~CUpdateData()
    {
    if (iListBox)
        {
        iListBox->Model()->SetItemTextArray(iItemTextArray);
        if (iRestoreTopItem >= 0) iListBox->SetTopItemIndex(iRestoreTopItem);
        if (iRestoreCurrentItem >= 0) iListBox->SetCurrentItemIndex(iRestoreCurrentItem);
        }
    delete iThumbnailHandler;
    delete iTitlePaneText;
    delete iListBoxModel;
    delete iFieldArray;
    }

void CPbkContactInfoControl::CUpdateData::StoreListBoxState
        (CEikTextListBox& aListBox)
    {
    iListBox = &aListBox;
    iItemTextArray = iListBox->Model()->ItemTextArray();
    iRestoreTopItem = iListBox->TopItemIndex();
    iRestoreCurrentItem = iListBox->CurrentItemIndex();
    }

inline void CPbkContactInfoControl::CUpdateData::ResetListBoxState()
    {
    iListBox = NULL;
    }

EXPORT_C CPbkContactInfoControl* CPbkContactInfoControl::NewL
        (TInt aResId,
        const CCoeControl& aParent,
        CPbkContactItem* aContact,
        CPbkContactEngine* aEngine)
    {
    CPbkContactInfoControl* self = new(ELeave) CPbkContactInfoControl;
    CleanupStack::PushL(self);

    self->iContactEngine = aEngine;
    self->SetContainerWindowL(aParent);

    TResourceReader reader;
    self->iCoeEnv->CreateResourceReaderLC(reader, aResId);
    self->ConstructFromResourceL(reader);
    CleanupStack::PopAndDestroy();  // reader

    self->UpdateL(aContact);
    CleanupStack::Pop();  // self
    return self;
    }

CPbkContactInfoControl::CPbkContactInfoControl()
    {
    iShowThumbnail = ETrue;
    // new(ELeave) will reset member data
    __ASSERT_DEBUG(
        !iListBox && iFlags==0 && !iFields && !iContactItem && !iThumbnailHandler,
        Panic(EPanicPostCond_Constructor));
    }

void CPbkContactInfoControl::ConstructFromResourceL(TResourceReader& aReader)
    {
    __ASSERT_DEBUG(
        !iListBox && !iListBoxModel && iFlags==0 && !iFields && !iContactItem && !iThumbnailHandler,
        Panic(EPanicPreCond_ConstructFromResourceL));

    TPtrC emptyText = aReader.ReadTPtrC();
    iFlags = aReader.ReadUint32();
    const TInt listBoxType = aReader.ReadInt16();
    // It is no more possible to specify the listbox type from resource file
    __ASSERT_ALWAYS(listBoxType == EAknCtDoubleGraphicListBox,
        Panic(EPanicInvalidListBoxType));

    iListBox = new(ELeave) CListBox;
    iListBox->SetContainerWindowL(*this);
    iListBox->ConstructFromResourceL(aReader);
    iListBox->View()->SetListEmptyTextL(emptyText);

    // Load list box icons
    TInt iconArrayRes = aReader.ReadInt32();
	// Use granularity of 4
    CPbkIconArray* iconArray = new(ELeave) CPbkIconArray(4);
    CleanupStack::PushL(iconArray);
    iconArray->ConstructFromResourceL(iconArrayRes);

    // get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();
    factory.AddPbkFieldIconsL(NULL, iconArray);
    CleanupStack::PopAndDestroy(extGlobal);

    CleanupStack::Pop();  // iconArray
    iListBox->SetIconArray(iconArray);

    iDefaultIconId = static_cast<TPbkIconId>(aReader.ReadInt32());

    // Set up the listbox
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    // Take ownership of default listbox model
    iListBoxModel = iListBox->Model()->ItemTextArray();
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    __ASSERT_DEBUG(
        iListBox && iListBoxModel && !iFields && !iContactItem && !iThumbnailHandler,
        Panic(EPanicPostCond_ConstructFromResourceL));
    }

CPbkContactInfoControl::~CPbkContactInfoControl()
    {
    delete iMarkedItemsArray;
    delete iThumbnailHandler;
    delete iListBox;
    delete iListBoxModel;
    delete iFields;
    }

void CPbkContactInfoControl::UpdateL(CPbkContactItem* aContact)
    {
    // PreCond:
    __ASSERT_DEBUG(aContact && iListBox, Panic(EPanicPreCond_UpdateL));

    iVoiceTagField = NULL;
    iVoiceTagFetchCompleted = EFalse;

    CUpdateData* updateData = new(ELeave) CUpdateData;
    CleanupStack::PushL(updateData);
    updateData->iFieldArray = new(ELeave) CPbkFieldArray(8);  // granularity of 8

    CheckRingToneFieldL( aContact );

    const TInt fieldCount = aContact->CardFields().Count();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        TPbkContactItemField& field = aContact->CardFields()[i];
        if (!field.IsHidden() &&  // Don't show hidden fields
            !field.IsEmptyOrAllSpaces() &&  // Don't show empty fields
              // Don't show name fields unless specfied by flags
            ((iFlags & KPbkContactInfoControlShowNameFields) || !field.FieldInfo().NameField()) &&
              // Don't show image fields unless specfied by flags
            ((iFlags & KPbkContactInfoControlShowImageFields) || !field.FieldInfo().IsImageField()))
            {
            updateData->iFieldArray->AppendL(field);
            }
        }

    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* titlePane = NULL;
    if (statusPane && (iFlags & KPbkContactInfoControlUpdateStatusPane))
        {
        if ((iFlags & KPbkContactInfoControlUpdateTitlePane) &&
            statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
            {
            titlePane = static_cast<CAknTitlePane*>(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
            updateData->iTitlePaneText = aContact->GetContactTitleOrNullL();
            if (!updateData->iTitlePaneText)
                {
                updateData->iTitlePaneText = iCoeEnv->AllocReadResourceL(R_QTN_PHOB_UNNAMED);
                }
            }
        if ( (iFlags & KPbkContactInfoControlUpdateContextPane) && iShowThumbnail )
            {
            updateData->iThumbnailHandler = CPbkThumbnailPopup::NewL(*iContactEngine);
            updateData->iThumbnailHandler->Load(*aContact, iListBox );
            }
        }

    // Contact must be set before setting listbox
    iContactItem = aContact;

    // Update list box
    HBufC* timeFormat = iCoeEnv->AllocReadResourceLC(R_QTN_DATE_USUAL);

	CPbkFieldListBoxModel::TParams params(
		*updateData->iFieldArray,
		*timeFormat,
		*iListBox->IconArray());
	params.iDefaultIconId = iDefaultIconId;
	params.iFieldAnalyzer = this;

	CPbkFieldListBoxModel* listBoxModel = CPbkFieldListBoxModel::NewL(params);
    CleanupStack::PopAndDestroy();  // timeFormat
    listBoxModel->SetClipper(*this);
    updateData->iListBoxModel = listBoxModel;
    updateData->StoreListBoxState(*iListBox);
    iListBox->Model()->SetItemTextArray(updateData->iListBoxModel);
    iListBox->Reset();
    iListBox->UpdateScrollBarsL();

    // All code that might leave has been executed succesfully
    // -> Change state and redraw.
    updateData->ResetListBoxState();
    Swap(iFields, updateData->iFieldArray);
    Swap(iListBoxModel, updateData->iListBoxModel);
    iListBox->ItemDrawer()->SetFields(*iFields);
    iContactItem = aContact;
    if (titlePane && updateData->iTitlePaneText)
        {
        titlePane->SetText(updateData->iTitlePaneText);
        updateData->iTitlePaneText = NULL;
        }
    Swap(iThumbnailHandler, updateData->iThumbnailHandler);

    // Cleanup
    CleanupStack::PopAndDestroy();  // updateData

    __ASSERT_DEBUG(iListBox && iContactItem && iFields && iListBoxModel, Panic(EPanicPostCond_UpdateL));
    __ASSERT_DEBUG(iContactItem->CardFields().Count() >= iFields->Count(), Panic(EPanicPostCond_UpdateL));
    __ASSERT_DEBUG(&static_cast<CPbkFieldListBoxModel*>(iListBoxModel)->Array() == iFields, Panic(EPanicPostCond_UpdateL));
    __ASSERT_DEBUG(iListBox->Model()->ItemTextArray() == iListBoxModel, Panic(EPanicPostCond_UpdateL));
    __ASSERT_DEBUG(iListBox->Model()->NumberOfItems() == iFields->Count(), Panic(EPanicPostCond_UpdateL));
    }

CPbkContactItem& CPbkContactInfoControl::ContactItem()
    {
    __ASSERT_DEBUG(iContactItem, Panic(EPanicPreCond_ContactItem));
    return *iContactItem;
    }

TPbkContactItemField* CPbkContactInfoControl::CurrentField() const
    {
    // PreCond:
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_CurrentField));

    const TInt index = iListBox->CurrentItemIndex();
    // CPbkFieldArray will check index
    if (index >= 0)
        {
        __ASSERT_DEBUG(index < iFields->Count(), Panic(EPanicLogic_CurrentField));
        return &(*iFields)[index];
        }
    else
        {
        return NULL;
        }
    }

EXPORT_C TInt CPbkContactInfoControl::CurrentFieldIndex
        (  ) const
    {
    // PreCond:
    __ASSERT_DEBUG(iFields, Panic(EPanicPreCond_CurrentFieldIndex));

    const TPbkContactItemField* currentField = CurrentField();
	// Initialize the index to surely errornous value
    TInt index = -1;
    if (currentField)
        {
        // Search the field in the stored fields array
        index = iContactItem->FindFieldIndex(*currentField);
        // Field must be found
        __ASSERT_DEBUG(index >= 0, Panic(EPanicPostCond_CurrentFieldIndex));
        }
    return index;
    }

EXPORT_C TInt CPbkContactInfoControl::FieldPos(const TPbkContactItemField& aField)
    {
    // PreCond:
    __ASSERT_DEBUG(iFields, Panic(EPanicPreCond_FieldPos));

    for (TInt i = 0; i < iFields->Count(); ++i)
        {
        if ((*iFields)[i].IsSame(aField))
            {
            // aField found
            return i;
            }
        }

    // aField not found
    return -1;
    }

EXPORT_C TInt CPbkContactInfoControl::FieldCount() const
    {
    // PreCond:
    __ASSERT_DEBUG(iFields, Panic(EPanicPreCond_FieldCount));

    return iFields->Count();
    }

EXPORT_C CPbkViewState* CPbkContactInfoControl::GetStateL() const
    {
    CPbkViewState* state = GetStateLC();
    CleanupStack::Pop();  // state
    return state;
    }

EXPORT_C CPbkViewState* CPbkContactInfoControl::GetStateLC() const
    {
    CPbkViewState* state = CPbkViewState::NewLC();
    state->SetFocusedContactId(iContactItem->Id());
    if (iListBox->Model()->NumberOfItems() > 0)
        {
        state->SetFocusedFieldIndex(CurrentFieldIndex());
        const TInt topIndex = iListBox->TopItemIndex();
        if (topIndex >= 0)
            {
            const TPbkContactItemField& topField = (*iFields)[topIndex];
            state->SetTopFieldIndex(iContactItem->FindFieldIndex(topField));
            }
        }
    return state;
    }

EXPORT_C void CPbkContactInfoControl::RestoreStateL(const CPbkViewState* aState)
    {
    const TInt numItems = iListBox->Model()->NumberOfItems();
    if (aState && numItems > 0)
        {
        TInt focusIndex = 0;
        TInt topIndex = 0;
        const TInt height = iListBox->View()->NumberOfItemsThatFitInRect(iListBox->View()->ViewRect());

        // The index of field to-be-focused depends on whether the shown
        // contact is the latest contact that has been shown in ContactInfo.
        // For re-opened contacts the focus is same as before the ContactInfo
        // was closed. For new contact the first POC field should be focused.
        if (aState->FocusedFieldIndex() == -1)
            {
            // "New" contact. Set focus to first POC field
            for (TInt i = 0; i < iFields->Count(); i++)
                {
                if ((*iFields)[i].FieldInfo().IsPocField())
                    {
                    focusIndex = i;
                    break;
                    }
                }
            // Set top index so that focused field is bottom-most visible field
            topIndex = Max(focusIndex - height + 1, 0);
            }
        else
            {
            // "Old" contact. Set focus to previously selected field
            // Set top item
            if (aState->TopFieldIndex() >= 0 && aState->TopFieldIndex() < iContactItem->CardFields().Count())
                {
                topIndex = Max(FieldPos(iContactItem->CardFields()[aState->TopFieldIndex()]), 0);
                }

            // Set listIndex so that maximum number of items are visible
            if (topIndex + height > numItems)
                {
                topIndex += numItems - (height + topIndex);
                if (topIndex < 0) topIndex = 0;
                }

            // Set focus
            if (aState->FocusedFieldIndex() >= 0)
                {
                if (aState->FocusedFieldIndex() < iContactItem->CardFields().Count())
                    {
                    focusIndex = Max(FieldPos(iContactItem->CardFields()[aState->FocusedFieldIndex()]), 0);
                    }
                else
                    {
                    focusIndex = iContactItem->CardFields().Count() - 1;
                    }
                }
            }
        iListBox->SetTopItemIndex(topIndex);

        // TODO: focusIndex may be invalid e.g. after contact ringing tone
        // removal/addition.
        if (focusIndex<0)
            {
            focusIndex=0;
            }
        else if (focusIndex>=numItems )
            {
            focusIndex=numItems-1;
            }
        iListBox->SetCurrentItemIndex(focusIndex);
        }
    else
        {
        iListBox->Reset();
        }
    iListBox->UpdateScrollBarsL();
    }

EXPORT_C CEikListBox& CPbkContactInfoControl::ListBox()
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_ListBox));
    return *iListBox;
    }

void CPbkContactInfoControl::HideThumbnail()
    {
    iShowThumbnail = EFalse;
    if (iThumbnailHandler)
        {
        iThumbnailHandler->CancelLoading();
        }
    }

void CPbkContactInfoControl::ShowThumbnailL()
    {
    iShowThumbnail = ETrue;
    if (iFlags & KPbkContactInfoControlUpdateContextPane)
        {
        if (iContactItem)
            {
            if (!iThumbnailHandler)
                {
                iThumbnailHandler = CPbkThumbnailPopup::NewL(*iContactEngine);
                }
            iThumbnailHandler->Load(*iContactItem, iListBox);
            }
        }
    }

TKeyResponse CPbkContactInfoControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_OfferKeyEventL));

    if ( iThumbnailHandler &&
        (aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iScanCode == EStdKeyDownArrow ))
        {
        iThumbnailHandler->Refresh();
        }
    // Forward all key events to the list box
    return iListBox->OfferKeyEventL(aKeyEvent,aType);
    }

TInt CPbkContactInfoControl::NumberOfItems() const
	{
	// This control displays only one item
	return 1;
	}

TBool CPbkContactInfoControl::ItemsMarked() const
	{
	// No items can be marked in this control
	return EFalse;
	}

const CContactIdArray& CPbkContactInfoControl::MarkedItemsL() const
	{
    if (!iMarkedItemsArray)
        {
        iMarkedItemsArray = CContactIdArray::NewL();
        }

    // Remove old contents of array
    // (use Remove instead of Reset to keep the array buffer)
    iMarkedItemsArray->Remove(0,iMarkedItemsArray->Count());
    iMarkedItemsArray->AddL(FocusedContactIdL());

    return *iMarkedItemsArray;
	}

TContactItemId CPbkContactInfoControl::FocusedContactIdL() const
	{
	return iContactItem->Id();
	}

const TPbkContactItemField* CPbkContactInfoControl::FocusedField() const
	{
	return CurrentField();
	}

MObjectProvider* CPbkContactInfoControl::ObjectProvider()
    {
    return this;
    }

TInt CPbkContactInfoControl::FocusedFieldIndex() const
	{
	return CurrentFieldIndex();
	}

TBool CPbkContactInfoControl::ClipFromBeginning(TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber)
    {
    return AknTextUtils::ClipToFit(aBuffer, AknTextUtils::EClipFromBeginning,
        this->iListBox, aItemIndex, aSubCellNumber);
    }

TBool CPbkContactInfoControl::HasVoiceTagL(const TPbkContactItemField& aField) const
    {
    if (!iVoiceTagFetchCompleted)
        {
        iVoiceTagField = iContactItem->VoiceTagField();
        iVoiceTagFetchCompleted = ETrue;
        }
    if (iVoiceTagFetchCompleted && iVoiceTagField)
        {
        return (!iVoiceTagField->IsEmptyOrAllSpaces() && iVoiceTagField->IsSame(aField));
        }
    else
        {
        return EFalse;
        }
    }

TBool CPbkContactInfoControl::HasSpeedDialL(const TPbkContactItemField& aField) const
    {
    if (!iContactEngine)
        {
        return EFalse;
        }

    const TInt fieldIndex = iContactItem->FindFieldIndex(aField);

    // If there is no such a field, KErrNotFound is returned as a
    // field index.
    if ( fieldIndex != KErrNotFound )
        {
        return (iContactEngine->IsSpeedDialAssigned(*iContactItem, fieldIndex));
        }
    else
        {
        return EFalse;
        }
    }

void CPbkContactInfoControl::SizeChanged()
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_SizeChanged));
    // Listbox occupies whole area of this control
    iListBox->SetRect(Rect());
    }

TInt CPbkContactInfoControl::CountComponentControls() const
    {
    __ASSERT_DEBUG(iListBox, Panic(EPanicPreCond_CountComponentControls));
    // Always only one child control: the list box.
    return 1;
    }

CCoeControl* CPbkContactInfoControl::ComponentControl(TInt PBK_DEBUG_ONLY(aIndex)) const
    {
    __ASSERT_DEBUG(iListBox && aIndex==0, Panic(EPanicPreCond_ComponentControl));
    // Return the one and only child control
    return iListBox;
    }

void CPbkContactInfoControl::HandleResourceChange(TInt aType)
    {
    CPbkContactDetailsBaseControl::HandleResourceChange(aType);
    TRAP_IGNORE(DoHandleResourceChangeL(aType));
    }

void CPbkContactInfoControl::DoHandleResourceChangeL(TInt aType)
    {
    if (aType == KAknsMessageSkinChange || aType == KEikDynamicLayoutVariantSwitch)
        {
        CPbkIconArray* iconArray = iListBox->IconArray();
        if (iconArray)
            {
            iconArray->RefreshL(R_PBK_ICON_INFO_ARRAY);
            }
        const TBool isNonFocusing( IsNonFocusing() );
        if ( iContactItem )
            {
            if ( !isNonFocusing )
                {
				if (IsFocused())
					{
					iThumbnailHandler->Load(*iContactItem, iListBox);
					}
                }
            }
        if (aType == KEikDynamicLayoutVariantSwitch)
            {
            SizeChanged();
            if ( !isNonFocusing )
                {
				if (IsFocused())
					{
					// cause also refreshing of thumbnail
					ShowThumbnailL();
					}
                }
            }
        }
    }


void CPbkContactInfoControl::FocusChanged( TDrawNow aDrawNow )
    {
    CCoeControl::FocusChanged( aDrawNow );
    const TBool focused( IsFocused() );

    if (focused)
        {
        // No big deal if thumbnail is not shown
        TRAP_IGNORE( ShowThumbnailL() );
        }
    else
        {
        HideThumbnail();
        }

    if( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }
    }

void CPbkContactInfoControl::CheckRingToneFieldL( CPbkContactItem* aContact )
    {
    // Check is there ringtone field and if there is, check that file is still
    // available. If not, hide the field.
    TPbkContactItemField* ringtoneField =
        aContact->FindField( EPbkFieldIdPersonalRingingToneIndication );
    if ( ringtoneField )
        {
        if ( ringtoneField->StorageType()==KStorageTypeText )
            {
            if ( !BaflUtils::FileExists(
                iContactEngine->FsSession(),
                ringtoneField->Text()) )
                {
                ringtoneField->SetHidden( ETrue );
                }
            }
        }
    }

// CPbkContactInfoControl::CUpdateTestData implementation
#ifdef __TEST_UpdateL__
void CPbkContactInfoControl::CUpdateTestData::ConstructL(CPbkContactInfoControl& aControl)
    {
    iContactItem = aControl.iContactItem;
    iTopIndex = aControl.iListBox->TopItemIndex();
    iCurrentIndex = aControl.iListBox->CurrentItemIndex();

    CEikStatusPane* statusPane = static_cast<CEikonEnv*>(aControl.iCoeEnv)->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
        {
        iTitlePane = static_cast<CAknTitlePane*>(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
        iTitlePaneText = iTitlePane->Text()->AllocL();
        }

    iFieldArray = aControl.iFields;
    iThumbnailHandler = aControl.iThumbnailHandler;
    }

CPbkContactInfoControl::CUpdateTestData::~CUpdateTestData()
    {
    delete iTitlePaneText;
    }

TBool CPbkContactInfoControl::CUpdateTestData::Compare(CPbkContactInfoControl& aControl) const
    {
    return
        iContactItem == aControl.iContactItem &&
        (iTopIndex < 0 || iTopIndex == aControl.iListBox->TopItemIndex()) &&
        (iCurrentIndex < 0 || iCurrentIndex == aControl.iListBox->CurrentItemIndex()) &&
        iTitlePaneText->Compare(*iTitlePane->Text())==0 &&
        iFieldArray == aControl.iFields &&
        (!iFieldArray || iFieldArray == &static_cast<CPbkFieldListBoxModel*>(aControl.iListBoxModel)->Array()) &&
        iThumbnailHandler == aControl.iThumbnailHandler;
    }
#endif

//  End of File
