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
*       Methods for field type selection dialog for Phonebook's Create 
*       New / Edit Memory entry functions.
*
*/


// INCLUDE FILES
#include    "CPbkMemoryEntryAddItemDlg.h"  // This class

// System classes
#include    <aknPopup.h>    // CAknPopupList
#include    <aknlists.h>    // EikControlFactory

// PbkView classes
#include    <pbkview.rsg>
#include    <CPbkIconArray.h>

// PbkEng classes
#include    <CPbkFieldsInfo.h>
#include    <CPbkFieldInfo.h>
#include    <DigCleanupResetPointer.h>
#include    <CPbkExtGlobals.h>
#include    <MPbkExtensionFactory.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_MdcaCount,
    EPanicPreCond_MdcaPoint,
    EPanicPreCond_SetArray,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)   
    {
    _LIT(KPanicText, "CPbkMemoryEntryAddItemDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

}  // namespace



// MODULE DATA STRUCTURES

NONSHARABLE_CLASS(CPbkMemoryEntryAddItemDlg::CListBoxModel) : 
        public CBase, 
        public MDesCArray
    {
    public: // Interface
        void SetArray(CArrayPtr<CPbkFieldInfo>* aArray, CPbkIconArray* aIconArray, TBool aSortFieldInfos);
        void Sort();
        inline CArrayPtr<CPbkFieldInfo>& Array() { return *iArray; }
    
    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;
    
    private: // implementation
        class TAddItemSortKey;

    private: // data members
        // Ref: array of CPbkFieldInfo objects to show
        CArrayPtr<CPbkFieldInfo>* iArray;
        /// Maximum list box row buffer size
        enum { KMaxRowBuf=128 };
        /// Own: Buffer for formatting a list box row text
        __MUTABLE TBuf<KMaxRowBuf> iRowBuf;
        /// Ref: icons for field types
        CPbkIconArray* iIconArray;
    };


NONSHARABLE_CLASS(CPbkMemoryEntryAddItemDlg::CListBoxModel::TAddItemSortKey) : 
        public TKeyArrayFix
    {
    public: 
        inline TAddItemSortKey(CArrayPtr<CPbkFieldInfo>* aArray) 
            : TKeyArrayFix(0, ECmpTInt32), iArray(aArray) { }

    public: // from TKey
        TInt Compare(TInt aLeft,TInt aRight) const;
    
    private: // data members
        // Ref: field info array
        CArrayPtr<CPbkFieldInfo>* iArray;
    };

// ==================== LOCAL FUNCTIONS ====================

TInt CPbkMemoryEntryAddItemDlg::CListBoxModel::TAddItemSortKey::Compare
        (TInt aLeft,
        TInt aRight) const
    {
    return (*iArray)[aLeft]->AddItemOrdering() - (*iArray)[aRight]->AddItemOrdering();
    }

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkMemoryEntryAddItemDlg::CPbkMemoryEntryAddItemDlg() :
    iSortFieldInfos(EFalse)
    {
    // CBase::operator new() will reset members
    __ASSERT_DEBUG(!iListBox && ! iPopupList,
        Panic(EPanicPostCond_Constructor));
    }

EXPORT_C void CPbkMemoryEntryAddItemDlg::ResetWhenDestroyed
        (CPbkMemoryEntryAddItemDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

EXPORT_C CPbkFieldInfo* CPbkMemoryEntryAddItemDlg::ExecuteLD
        (CArrayPtrFlat<CPbkFieldInfo>& aFieldInfos,
        TInt aCbaResource)
    {
    PrepareLC(aFieldInfos, aCbaResource, KNullDesC);
    return RunLD();
    }

EXPORT_C CPbkFieldInfo* CPbkMemoryEntryAddItemDlg::ExecuteLD
        (CArrayPtr<CPbkFieldInfo>& aFieldInfos, 
        TInt aCbaResource,
        const TDesC& aTitleText)
    {
    PrepareLC(aFieldInfos, aCbaResource, aTitleText);
    return RunLD();
    }

EXPORT_C CPbkFieldInfo* CPbkMemoryEntryAddItemDlg::ExecuteLD
        (CArrayPtr<CPbkFieldInfo>& aFieldInfos)
    {
    PrepareLC(aFieldInfos, R_AVKON_SOFTKEYS_OK_CANCEL, KNullDesC);
    return RunLD();
    }

CPbkMemoryEntryAddItemDlg::~CPbkMemoryEntryAddItemDlg()
    {
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    // Tells RunLD this object has been destroyed
    if (iDestroyedPtr) *iDestroyedPtr = ETrue;
    if (iPopupList) iPopupList->CancelPopup();
    delete iListBox;
    }

void CPbkMemoryEntryAddItemDlg::SortFieldInfoArray()
    {
    iSortFieldInfos = ETrue;
    }

void CPbkMemoryEntryAddItemDlg::PrepareLC
        (CArrayPtr<CPbkFieldInfo>& aFieldInfos, 
        TInt aCbaResource, 
        const TDesC& aTitle)
    {
    __ASSERT_DEBUG(!iListBox && ! iPopupList && !iDestroyedPtr,
        Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    CleanupResetPointerPushL(iPopupList);

    // Create a list box
    iListBox = static_cast<CEikFormattedCellListBox*>
        (EikControlFactory::CreateByTypeL(EAknCtSingleGraphicPopupMenuListBox).iControl);

    // Create a popup list
    CAknPopupList* popupList = CAknPopupList::NewL
        (iListBox, aCbaResource, AknPopupLayouts::EMenuGraphicWindow);
    CleanupStack::PushL(popupList);
    if (aTitle.Length() > 0)
        {
        popupList->SetTitleL(aTitle);
        }
    	
    // Init list box
	iListBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    // Load icons
    CPbkIconArray* iconArray = new(ELeave) CPbkIconArray(4);
    CleanupStack::PushL(iconArray);
    iconArray->ConstructFromResourceL(R_PBK_FIELDTYPE_ICONS);

    // get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();
    factory.AddPbkFieldIconsL(NULL, iconArray);
    CleanupStack::PopAndDestroy(extGlobal);

    CleanupStack::Pop(); // iconArray
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);
    
    // Create listbox model
    CListBoxModel* listBoxModel = new(ELeave) CListBoxModel;
    listBoxModel->SetArray(&aFieldInfos, iconArray, iSortFieldInfos);
    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);

    // Prepare to execute popup list
    CleanupStack::Pop();  // popupList
    iPopupList = popupList;
    }

CPbkFieldInfo* CPbkMemoryEntryAddItemDlg::RunLD()
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    TBool res = iPopupList->ExecuteLD();

    CPbkFieldInfo* result = NULL;
    if (thisDestroyed)
        {
        // this object has been destroyed
        CleanupStack::Pop(2);  // CleanupResetPointerPushL(iPopupList), this
        }
    else
        {
        if (res)
            {
            // Retrieve and return selection
            const TInt index = iListBox->CurrentItemIndex();
            if (index >= 0)
                {
                CListBoxModel* model = static_cast<CListBoxModel*>(iListBox->Model()->ItemTextArray());
                result = model->Array()[index];
                }
            }
        CleanupStack::PopAndDestroy(2);  // CleanupResetPointerPushL(iPopupList), this
        }

    return result;
    }

EXPORT_C void CPbkMemoryEntryAddItemDlg::Reserved_1()
    {
    }

EXPORT_C void CPbkMemoryEntryAddItemDlg::Reserved_2()
    {
    }


// CPbkMemoryEntryAddItemDlg::CListBoxModel

void CPbkMemoryEntryAddItemDlg::CListBoxModel::SetArray
        (CArrayPtr<CPbkFieldInfo>* aArray,
        CPbkIconArray* aIconArray,
        TBool aSortFieldInfos)
    {
    //PreCond:
    __ASSERT_DEBUG(aArray, Panic(EPanicPreCond_SetArray));

    iArray = aArray;
    iIconArray = aIconArray;

    if (aSortFieldInfos)
        {
        // sort the field infos according to the add item ordering
        TAddItemSortKey key(iArray);
        iArray->Sort(key);
        }
    }

TInt CPbkMemoryEntryAddItemDlg::CListBoxModel::MdcaCount() const
    {
    //PreCond:
    __ASSERT_DEBUG(iArray->Count() > 0, Panic(EPanicPreCond_MdcaCount));

    return iArray->Count();
    }

// format according to correct listbox style, show fields in predefined order
TPtrC16 CPbkMemoryEntryAddItemDlg::CListBoxModel::MdcaPoint(TInt aIndex) const
    {
    //PreCond:
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < iArray->Count(), Panic(EPanicPreCond_MdcaPoint));

    _LIT(KIconFormat, "%d\t");
        
    // insert icon index
    TInt iconIndex = iIconArray->FindIcon((*iArray)[aIndex]->IconId());
    if (iconIndex < 0)
        {
        // If icon is not found by Id fall back to empty icon
        iconIndex = iIconArray->FindIcon(EPbkqgn_prop_nrtyp_empty);
        }

    MUTABLE_CAST(CListBoxModel*, this)->iRowBuf.Format(KIconFormat, iconIndex); 
    const TDesC& fieldName = (*iArray)[aIndex]->AddItemText();
    const TInt chars = Min(iRowBuf.MaxLength()-iRowBuf.Length(), fieldName.Length());
    MUTABLE_CAST(CListBoxModel*, this)->iRowBuf.Append(fieldName.Left(chars));
    
    return iRowBuf;
    }

EXPORT_C CPbkItemTypeSelectCreateNew::CPbkItemTypeSelectCreateNew()
    {
    }

EXPORT_C CPbkItemTypeSelectCreateNew::~CPbkItemTypeSelectCreateNew()
    {
    delete iTitleBuf;
    }

EXPORT_C CPbkFieldInfo* CPbkItemTypeSelectCreateNew::ExecuteLD
        (CArrayPtr<CPbkFieldInfo>& aFieldInfos)
    {
    iTitleBuf = CCoeEnv::Static()->AllocReadResourceL(R_QTN_PHOB_QTL_ENTRY_CREATE);
    CPbkFieldInfo* result = CPbkMemoryEntryAddItemDlg::ExecuteLD
        (aFieldInfos, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT, *iTitleBuf);
    return result;
    }

EXPORT_C CPbkItemTypeSelectAddToExisting::CPbkItemTypeSelectAddToExisting()
    {
    }

EXPORT_C CPbkItemTypeSelectAddToExisting::~CPbkItemTypeSelectAddToExisting()
    {
    delete iTitleBuf;
    }

EXPORT_C CPbkFieldInfo* CPbkItemTypeSelectAddToExisting::ExecuteLD
        (CArrayPtr<CPbkFieldInfo>& aFieldInfos)
    {
    iTitleBuf = CCoeEnv::Static()->AllocReadResourceL(R_QTN_PHOB_QTL_ENTRY_ADD_TO);
    CPbkFieldInfo* result = CPbkMemoryEntryAddItemDlg::ExecuteLD
        (aFieldInfos, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT, *iTitleBuf);
    return result;
    }


//  End of File  
