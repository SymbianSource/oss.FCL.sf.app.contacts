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
*       Phonebook Memory entry Defaults dialog methods.
*
*/


// INCLUDE FILES
#include "CPbkMemoryEntryDefaultsDlg.h"
#include <aknlists.h>    // EikControlFactory
#include <StringLoader.h>
#include <featmgr.h>

#include <pbkview.rsg>
#include "MPbkClipListBoxText.h"
#include "CPbkFieldListBoxModel.h"
#include <CPbkIconArray.h>

// PbkEng classes
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkConstants.h>
#include <CPbkContactItem.h>
#include <DigCleanupResetPointer.h>
#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>

#include <PbkDebug.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

/**
 * Enum for default field types. Defines also order of items in list.
 *
 * Because the list returns the index of the currently displayed item,
 * it's been decided to treat the two versions of SMS (regular SMS versus
 * the enhanced email-over-sms version) the same when populating the list,
 * and differentiate between the two when an SMS operation is selected.
 */
enum TDefaults
    {
    EPbkCallDefault = 0,
    EPbkVideoDefault,
    EPbkVoipDefault,
    EPbkPocDefault,
    EPbkSmsDefault,
    EPbkMmsDefault,
    EPbkEmailDefault,
    KPbkNumberOfDefaults
    };

static const TInt KMaxIntLength = 15;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPostCond_ConstructL,
    EPanicPreCond_FixIndex
    };
static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkMemoryEntryDefaultsDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace


// MODULE DATA STRUCTURES

NONSHARABLE_CLASS(CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg) :
        public CBase,
        public MPbkClipListBoxText
    {
    public:  // Interface
        CPbkMemoryEntryDefaultAssignDlg();
        TInt ExecuteLD(CPbkFieldArray& aArray);
        ~CPbkMemoryEntryDefaultAssignDlg();

    public:  // from MPbkClipListBoxText
        TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex,
            TInt aSubCellNumber);

    private: // Data
        /// Own: field selection listbox
        CEikFormattedCellListBox* iListBox;
        /// Own: popup list for the listbox
        CAknPopupList* iPopupList;
        /// Ref: Referred TBool is set ETrue in destructor
        TBool* iDestroyedPtr;
    };

/**
 * Returns default field identified by aDefaultIndex from aContact
 * or NULL if the default is not set.
 */
TPbkContactItemField* DefaultField
        (const CPbkContactItem& aContact, TInt aDefaultIndex)
    {
    TPbkContactItemField* result = NULL;
    switch (aDefaultIndex)
        {
        case EPbkCallDefault:
            {
            result = aContact.DefaultPhoneNumberField();
            break;
            }
        case EPbkVideoDefault:
            {
            result = aContact.DefaultVideoNumberField();
            break;
            }
        case EPbkMmsDefault:
            {
            result = aContact.DefaultMmsField();
            break;
            }
        case EPbkSmsDefault:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                result = aContact.DefaultEmailOverSmsField();
                }
            else
                {
                result = aContact.DefaultSmsField();
                }
            break;
            }
        case EPbkEmailDefault:
            {
            result = aContact.DefaultEmailField();
            break;
            }
        case EPbkVoipDefault:
            {
            result = aContact.DefaultVoipField();
            break;
            }
        case EPbkPocDefault:
            {
            result = aContact.DefaultPocField();
            break;
            }
        default:
            {
            break;
            }
        }
    return result;
    }

/**
 * Returns the default UI text for default field identified by aDefaultIndex.
 */
HBufC* DefaultFieldTextLC(TInt aDefaultIndex)
    {
    HBufC* result = NULL;
    switch (aDefaultIndex)
        {
        case EPbkCallDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_CALL_DEFAULT);
            break;
            }
        case EPbkVideoDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_VIDEO_DEFAULT);
            break;
            }
        case EPbkMmsDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_MMS_DEFAULT);
            break;
            }
        case EPbkSmsDefault:
            {
            // this is the same text for either SMS or EmailOverSms
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_SMS_DEFAULT);
            break;
            }
        case EPbkEmailDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_EMAIL_DEFAULT);
            break;
            }
        case EPbkVoipDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_VOIP_DEFAULT);
            break;
            }
        case EPbkPocDefault:
            {
            result = StringLoader::LoadLC(R_QTN_PHOB_SETI_POC_DEFAULT);
            break;
            }
        default:
            {
            break;
            }
        }
    return result;
    }

/**
 * Returns true if aField is an applicable default field of type identified
 * by aDefaultIndex.
 */
TBool IsApplicableDefaultField
        (const TPbkContactItemField& aField, TInt aDefaultIndex)
    {
    TBool result = EFalse;

    switch (aDefaultIndex)
        {
        case EPbkCallDefault:
            {
            if ( aField.FieldInfo().IsPhoneNumberField() &&
                 aField.Text().Length() > 0 )
                {
                result = ETrue;
                }
            break;
            }

        case EPbkVideoDefault:
            {
            if ( aField.FieldInfo().IsPhoneNumberField() &&
                 aField.Text().Length() > 0 )
                {
                if ( ( aField.FieldInfo().FieldId() != EPbkFieldIdFaxNumber ) &&
                     ( aField.FieldInfo().FieldId() != EPbkFieldIdPagerNumber ) )
                    {
                    result = ETrue;
                    }
                }
            break;
            }

        case EPbkSmsDefault:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                if (aField.FieldInfo().IsEmailOverSmsField() &&
                    aField.Text().Length() > 0)
                    {
                    result = ETrue;
                    }
                }
            else
                {
                if (aField.FieldInfo().IsPhoneNumberField() &&
                    aField.Text().Length() > 0)
                    {
                    result = ETrue;
                    }
                }
            break;
            }

        case EPbkMmsDefault:
            {
            if (aField.FieldInfo().IsMmsField() &&
                aField.Text().Length() > 0)
                {
                result = ETrue;
                }
            break;
            }
        case EPbkEmailDefault:
            {
            if (aField.FieldInfo().FieldId()==EPbkFieldIdEmailAddress &&
                aField.Text().Length() > 0)
                {
                result = ETrue;
                }
            break;
            }
        case EPbkVoipDefault:
            {
            if ( aField.FieldInfo().IsVoipField() &&
                 aField.Text().Length() > 0 )
                {
                result = ETrue;

                }
            break;
            }
        case EPbkPocDefault:
            {
            if (aField.FieldInfo().IsPocField() &&
                aField.Text().Length() > 0)
                {
                result = ETrue;
                }
            break;
            }
        default:
            {
            break;
            }
        }

    // Check that the field contains something
    if (aField.IsEmptyOrAllSpaces())
        {
        result = EFalse;
        }

    return result;
    }

/**
 * Sets aField as the default field for aContact identified by aField.
 */
TBool SetDefaultL
        (CPbkContactItem& aContact,
        TInt aDefaultIndex,
        TPbkContactItemField* aField)
    {
    TBool result = EFalse;
    switch (aDefaultIndex)
        {
        case EPbkCallDefault:
            {
            aContact.SetDefaultPhoneNumberFieldL(aField);
            result = ETrue;
            break;
            }
        case EPbkVideoDefault:
            {
            aContact.SetDefaultVideoNumberFieldL(aField);
            result = ETrue;
            break;
            }
        case EPbkMmsDefault:
            {
            aContact.SetDefaultMmsFieldL(aField);
            result = ETrue;
            break;
            }
        case EPbkSmsDefault:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdEmailOverSms))
                {
                aContact.SetDefaultEmailOverSmsFieldL(aField);
                }
            else
                {
                aContact.SetDefaultSmsFieldL(aField);
                }

            result = ETrue;
            break;
            }
        case EPbkEmailDefault:
            {
            aContact.SetDefaultEmailFieldL(aField);
            result = ETrue;
            break;
            }
        case EPbkVoipDefault:
            {
            aContact.SetDefaultVoipFieldL(aField);
            result = ETrue;
            break;
            }
        case EPbkPocDefault:
            {
            aContact.SetDefaultPocFieldL(aField);
            result = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return result;
    }


// ================= MEMBER FUNCTIONS =======================

inline CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultsDlg
        (CPbkContactEngine& aEngine, TContactItemId aContactId) :
        iContactId(aContactId),
        iEngine(aEngine)
    {
    __ASSERT_DEBUG(
        iContactId==aContactId && &iEngine==&aEngine && !iListBox &&
        !iDefaultAssignDlg && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

EXPORT_C CPbkMemoryEntryDefaultsDlg* CPbkMemoryEntryDefaultsDlg::NewL
        (TContactItemId aContactId,
        CPbkContactEngine& aEngine)
    {
    CPbkMemoryEntryDefaultsDlg* self = new(ELeave)
        CPbkMemoryEntryDefaultsDlg(aEngine, aContactId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

EXPORT_C void CPbkMemoryEntryDefaultsDlg::ExecuteLD()
    {
    CAknPopupList::ExecuteLD();
    }

CPbkMemoryEntryDefaultsDlg::~CPbkMemoryEntryDefaultsDlg()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkMemoryEntryDefaultsDlg destructor start"));
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    delete iDefaultAssignDlg;
    delete iListBox;

    FeatureManager::UnInitializeLib();

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkMemoryEntryDefaultsDlg destructor end"));
    }

void CPbkMemoryEntryDefaultsDlg::ProcessCommandL(TInt aCommandId)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkMemoryEntryDefaultsDlg::ProcessCommandL start"));

    switch (aCommandId)
        {
        case EAknSoftkeyOk:
            {
            if (DefaultsAssignPopupL(iListBox->CurrentItemIndex()))
                {
                CreateLinesL();
                iListBox->DrawDeferred();
                }
                //  If DefaultsAssignPopupL returns false, this object
                //  might be destroyed
            break;
            }

        default:
            {
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING
                ("About to call CAknPopupList::ProcessCommandL"));
            CAknPopupList::ProcessCommandL(aCommandId);
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING
                ("Called CAknPopupList::ProcessCommandL"));
            break;
            }
        }
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkMemoryEntryDefaultsDlg::ProcessCommandL end"));
    }

void CPbkMemoryEntryDefaultsDlg::HandleListBoxEventL
        (CEikListBox* aListBox, TListBoxEvent aEventType)
    {
    // Respond to events from listboxitem
    if (aListBox == iListBox)
        {
        if (aEventType == MEikListBoxObserver::EEventEnterKeyPressed)
            {
            // Ok/Enter key pressed, command is same as EAknSoftkeyOk
            ProcessCommandL(EAknSoftkeyOk);
            }
        }
    }

void CPbkMemoryEntryDefaultsDlg::CreateLinesL() const
    {
    CPbkContactItem* item = iEngine.ReadContactLC(iContactId);

    CDesCArray* lines = static_cast<CDesCArray*>
        (iListBox->Model()->ItemTextArray());
    lines->Reset();

    _LIT(KFormat, "%S\t%S");
    HBufC* noDefault = StringLoader::LoadLC(R_QTN_PHOB_SETI_NO_DEFAULT);

    for (TInt row=0; row < KPbkNumberOfDefaults; ++row)
        {
        // Only supported lines are created
        if (IsSupported(row))
            {
            HBufC* defaultFieldText = DefaultFieldTextLC(row);
            const TPbkContactItemField* field = DefaultField(*item, row);
            TPtrC label;
            if(field)
                {
                label.Set(field->Label());
                }
            else
                {
                label.Set(*noDefault);
                }

            // Allocate and format the listbox line
            HBufC* lineBuf = HBufC::NewLC(KFormat().Length()
                + defaultFieldText->Length() + label.Length());
            TPtr line(lineBuf->Des());
            line.Format(KFormat, defaultFieldText, &label);

            lines->AppendL(line);

            // Cleanup
            CleanupStack::PopAndDestroy(2);  // lineBuf, defaultFieldText
            }
        }

    CleanupStack::PopAndDestroy(2); // noDefault, item
    }

TBool CPbkMemoryEntryDefaultsDlg::DefaultsAssignPopupL(TInt aDefaultIndex)
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    CPbkContactItem* item = iEngine.OpenContactL(iContactId);
    CleanupStack::PushL(item);

    CPbkFieldArray* fields = new(ELeave) CPbkFieldArray(4 /*granularity*/);
    CleanupStack::PushL(fields);
    const TInt fieldCount = item->CardFields().Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        TPbkContactItemField& field = item->CardFields()[i];
        TInt theIndex = FixIndex(aDefaultIndex); // we have to fix the index
        if (IsApplicableDefaultField(field, theIndex))
            {
            fields->AppendL(field);
            }
        }

    iDefaultAssignDlg = new(ELeave) CPbkMemoryEntryDefaultAssignDlg;
    TInt fieldIndex = 0;
    TRAPD(err, fieldIndex = iDefaultAssignDlg->ExecuteLD(*fields));
    iDefaultAssignDlg = NULL;
    if (err)
        {
        User::Leave(err);
        }

    TBool result = EFalse;
    if (!thisDestroyed && fieldIndex >= 0)
        {
        TPbkContactItemField* field = NULL;
        if (fieldIndex < fields->Count())
            {
            field = &(*fields)[fieldIndex];
            }
            TInt theIndex = FixIndex(aDefaultIndex); // we have to fix the index
            result = SetDefaultL(*item, theIndex, field);
        }

    CleanupStack::PopAndDestroy(fields);

    if (result)
        {
        iEngine.CommitContactL(*item);
        }
    else
        {
        iEngine.CloseContactL(iContactId);
        }

    CleanupStack::PopAndDestroy(item);

    return result;
    }

void CPbkMemoryEntryDefaultsDlg::ConstructL()
    {
    // initialize feature manager
    FeatureManager::InitializeLibL();

    // Create a list box
    iListBox = static_cast<CEikColumnListBox*>
        (EikControlFactory::CreateByTypeL
            (EAknCtMenuDoublePopupMenuListBox).iControl);

    CAknPopupList::ConstructL
        (iListBox, R_PBK_SOFTKEYS_ASSIGN_BACK,
        AknPopupLayouts::EMenuDoubleWindow);

    // Init list box
    iListBox->ConstructL(this, CEikListBox::ELeftDownInViewRect);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // set title of popuplist
    HBufC* heading = StringLoader::LoadLC(R_QTN_PHOB_TITLE_DEFAULT_SETTINGS);
    SetTitleL(*heading);
    CleanupStack::PopAndDestroy(heading);

    CreateLinesL();

    __ASSERT_DEBUG(
        iListBox && !iDefaultAssignDlg && !iDestroyedPtr,
        Panic(EPanicPostCond_ConstructL));
    }


////////////////////////////////////////////////////////////
// CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg

inline CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg::
        CPbkMemoryEntryDefaultAssignDlg()
    {
    // CBase::operator new(TLeave) will reset members
    }

TInt CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg::ExecuteLD
        (CPbkFieldArray& aArray)
    {
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    // Create a list box
    iListBox = static_cast<CEikFormattedCellListBox*>
        (EikControlFactory::CreateByTypeL
            (EAknCtDoubleGraphicPopupMenuListBox).iControl);

    // Create a popup list
    CAknPopupList* popupList = CAknPopupList::NewL
        (iListBox, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
            AknPopupLayouts::EMenuDoubleLargeGraphicWindow);

    CleanupStack::PushL(popupList);

    // Init list box
    iListBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // set title of popuplist
    // Select number §qtn.phob.title.select.default§
    HBufC* heading = StringLoader::LoadLC(R_QTN_PHOB_TITLE_SELECT_DEFAULT);
    popupList->SetTitleL(*heading);
    CleanupStack::PopAndDestroy(heading);

    CPbkIconArray* iconArray = new(ELeave) CPbkIconArray(4 /*granularity*/);
    CleanupStack::PushL(iconArray);
    iconArray->ConstructFromResourceL(R_PBK_FIELDTYPE_ICONS);

    // get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();
    factory.AddPbkFieldIconsL(NULL, iconArray);
    CleanupStack::PopAndDestroy(extGlobal);

    CleanupStack::Pop();  // iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    // create model
    HBufC* timeFormat = CCoeEnv::Static()->AllocReadResourceLC
        (R_QTN_DATE_USUAL);

    CPbkFieldListBoxModel::TParams params(aArray, *timeFormat, *iconArray);
    CPbkFieldListBoxModel* listBoxModel = CPbkFieldListBoxModel::NewL(params);
    CleanupStack::PopAndDestroy(timeFormat);
    listBoxModel->SetClipper(*this);

    // insert TEXT_NO_DEFAULT as the last item to the array
    CPbkFieldListBoxRow* row = CPbkFieldListBoxRow::NewL();
    CleanupStack::PushL(row);
    TBuf<KMaxIntLength> iconBuffer;
    _LIT(KIcon, "%d");
    iconBuffer.Format(KIcon, iconArray->FindIcon(EPbkqgn_prop_nrtyp_empty));
    row->AppendColumnL(iconBuffer);
    HBufC* text = StringLoader::LoadLC(R_QTN_PHOB_SETI_NO_DEFAULT);  // LC
    row->AppendColumnL(*text);
    CleanupStack::PopAndDestroy(text);
    row->AppendColumnL(KNullDesC); // empty data field
    listBoxModel->AppendRowL(row);
    CleanupStack::Pop(row);

    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->Reset();

    // show popuplist dialog
    TInt index = -1;
    TBool res = EFalse;
    CleanupStack::Pop();  // popupList
    iPopupList = popupList;
    TRAPD(err, res = iPopupList->ExecuteLD());

    // Reset iPopupList member
    if (!thisDestroyed)
        {
        iPopupList = NULL;
        }
    // Rethrow any error
    if (err != KErrNone)
        {
        User::Leave(err);
        }

    if (thisDestroyed)
        {
        // this object has been destroyed
        index = KErrNotFound;
        CleanupStack::Pop();  // this
        }
    else
        {
        if (res)
            {
            index = iListBox->CurrentItemIndex();
            }
        CleanupStack::PopAndDestroy(); // this
        }

    return index;
    }

CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg::
        ~CPbkMemoryEntryDefaultAssignDlg()
    {
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    if (iPopupList)
        {
        iPopupList->CancelPopup();
        }
    delete iListBox;
    }

TBool CPbkMemoryEntryDefaultsDlg::CPbkMemoryEntryDefaultAssignDlg::
        ClipFromBeginning (TDes& aBuffer, TInt aItemIndex,
            TInt aSubCellNumber)
    {
    return AknTextUtils::ClipToFit
        (aBuffer, AknTextUtils::EClipFromBeginning, iListBox,
        aItemIndex, aSubCellNumber);
    }


TBool CPbkMemoryEntryDefaultsDlg::IsSupported(TInt aDefaults) const
    {
    TBool ret(ETrue);

    // Skip the line if MMS field not supported
    if ( aDefaults == EPbkMmsDefault &&
        !FeatureManager::FeatureSupported(KFeatureIdMMS))
        {
        ret = EFalse;
        }
    // Skip the line if Email field not supported
    else if ( aDefaults == EPbkEmailDefault &&
        !FeatureManager::FeatureSupported(KFeatureIdEmailUi))
        {
        ret = EFalse;
        }
    // Do not add video fields if they are not enabled
    else if ( aDefaults == EPbkVideoDefault &&
            !iEngine.Constants()->LocallyVariatedFeatureEnabled
                (EPbkLVAddVideoTelephonyFields))
        {
        ret = EFalse;
        }
    // Do not add Voip fields if they are not enabled
    else if ( aDefaults == EPbkVoipDefault &&
              !FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
        {
        ret = EFalse;
        }
    // Do not add POC fields if they are not enabled
    else if ( aDefaults == EPbkPocDefault &&
            !iEngine.Constants()->LocallyVariatedFeatureEnabled(EPbkLVPOC) )
        {
        ret = EFalse;
        }

    return ret;
    }

/**
 * This method corrects the given focus index to map with TDefaults enum.
 */
inline TInt CPbkMemoryEntryDefaultsDlg::FixIndex(TInt aIndex) const
    {
    __ASSERT_DEBUG(aIndex < KPbkNumberOfDefaults,
        Panic(EPanicPreCond_FixIndex));

    TInt ret = aIndex;

    for (TInt i=0; i <= ret; ++i)
        {
        // If the default in question is not supported,
        // add one to the index, since there was a missing row
        if (!IsSupported(i))
            {
            ++ret;
            }
        }
    return ret;
    }

//  End of File
