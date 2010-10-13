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
*        Class methods for field type selection dialog for Phonebook's Create 
         New / Edit Memory entry functions.
*
*/


// INCLUDE FILES
#include    "CPbkMemoryEntrySelectAddItemDlg.h"  // This class

// System classes
#include    <aknPopup.h>    // CAknPopupList
#include    <aknlists.h>    // EikControlFactory

#include    <CPbkExtGlobals.h>
#include    <MPbkExtensionFactory.h>


// PbkView classes
#include    <PbkView.rsg>
#include    <CPbkIconArray.h>
#include    "TPbkAddItemWrapper.h"

// PbkEng classes
#include    <DigCleanupResetPointer.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_MdcaCount,
    EPanicPreCond_MdcaPoint,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)   
    {
    _LIT(KPanicText, "CPbkMemoryEntrySelectAddItemDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// MODULE DATA STRUCTURES

NONSHARABLE_CLASS(CPbkMemoryEntrySelectAddItemDlg::CListBoxModel) : 
        public CBase, 
        public MDesCArray
    {
    public: // Interface
        /**
         * Constructor.
		 * @param aArray array
		 * @param aIconArray icon array
         */
        CListBoxModel(
            RArray<TPbkAddItemWrapper>& aArray,
            CPbkIconArray* aIconArray);
        /**
         * Returns the array.
         */
        inline RArray<TPbkAddItemWrapper>& Array()
			{
			return iArray;
			}
    
    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;
    
    public: // implementation
        static TInt Compare(const TPbkAddItemWrapper& aLhs, const TPbkAddItemWrapper& aRhs);

    private: // data members
        // Ref: array of TPbkAddItemWrapper objects to show
        RArray<TPbkAddItemWrapper>& iArray;
        /// Maximum list box row buffer size
        enum { KMaxRowBuf=128 };
        /// Own: Buffer for formatting a list box row text
        __MUTABLE TBuf<KMaxRowBuf> iRowBuf;
        /// Ref: icons for field types
        CPbkIconArray* iIconArray;
    };


// ================= MEMBER FUNCTIONS =======================

CPbkMemoryEntrySelectAddItemDlg::CPbkMemoryEntrySelectAddItemDlg()        
    {
    // CBase::operator new() will reset members
    __ASSERT_DEBUG(!iListBox && ! iPopupList,
        Panic(EPanicPostCond_Constructor));
    }

void CPbkMemoryEntrySelectAddItemDlg::ResetWhenDestroyed
        (CPbkMemoryEntrySelectAddItemDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

TPbkAddItemWrapper* CPbkMemoryEntrySelectAddItemDlg::ExecuteLD
        (RArray<TPbkAddItemWrapper>& aAddItems)
    {
    PrepareLC(aAddItems, R_AVKON_SOFTKEYS_OK_CANCEL);
    return RunLD();
    }

CPbkMemoryEntrySelectAddItemDlg::~CPbkMemoryEntrySelectAddItemDlg()
    {
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    // Tells RunLD this object has been destroyed
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

void CPbkMemoryEntrySelectAddItemDlg::PrepareLC
        (RArray<TPbkAddItemWrapper>& aAddItems, 
        TInt aCbaResource)
    {
    __ASSERT_DEBUG(!iListBox && ! iPopupList && !iDestroyedPtr,
        Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    CleanupResetPointerPushL(iPopupList);

    // Create a list box
    iListBox = static_cast<CEikFormattedCellListBox*> 
        (EikControlFactory::CreateByTypeL(
            EAknCtSingleGraphicPopupMenuListBox).iControl);

    // Create a popup list
    CAknPopupList* popupList = CAknPopupList::NewL
        (iListBox, aCbaResource, AknPopupLayouts::EMenuGraphicWindow);
    CleanupStack::PushL(popupList);
        	
    // Init list box
	iListBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

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
    CListBoxModel* listBoxModel = new(ELeave) CListBoxModel (aAddItems, iconArray);        
    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);

    // Prepare to execute popup list
    CleanupStack::Pop();  // popupList
    iPopupList = popupList;
    }

TPbkAddItemWrapper* CPbkMemoryEntrySelectAddItemDlg::RunLD()
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    TBool res = iPopupList->ExecuteLD();

    TPbkAddItemWrapper* result = NULL;
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
                result = &model->Array()[index];
                }
            }
        CleanupStack::PopAndDestroy(2);  // CleanupResetPointerPushL(iPopupList), this
        }

    return result;
    }


// CPbkMemoryEntrySelectAddItemDlg::CListBoxModel

CPbkMemoryEntrySelectAddItemDlg::CListBoxModel::CListBoxModel
        (RArray<TPbkAddItemWrapper>& aArray,
        CPbkIconArray* aIconArray) :
    iArray(aArray)    
    {
    iIconArray = aIconArray;    
    iArray.Sort(TLinearOrder<TPbkAddItemWrapper>(CPbkMemoryEntrySelectAddItemDlg::CListBoxModel::Compare));
    }

TInt CPbkMemoryEntrySelectAddItemDlg::CListBoxModel::MdcaCount() const
    {
    //PreCond:
    __ASSERT_DEBUG(iArray.Count() > 0, Panic(EPanicPreCond_MdcaCount));

    return iArray.Count();
    }

// format according to correct listbox style, show fields in predefined order
TPtrC16 CPbkMemoryEntrySelectAddItemDlg::CListBoxModel::MdcaPoint(TInt aIndex) const
    {
    //PreCond:
    __ASSERT_DEBUG(aIndex >= 0 && aIndex < iArray.Count(), Panic(EPanicPreCond_MdcaPoint));

    _LIT(KIconFormat, "%d\t");
        
    // insert icon index
    TInt iconIndex = iIconArray->FindIcon(iArray[aIndex].IconId());
    if (iconIndex < 0)
        {
        // If icon is not found by Id fall back to empty icon
        iconIndex = iIconArray->FindIcon(EPbkqgn_prop_nrtyp_empty);
        }

    MUTABLE_CAST(CListBoxModel*, this)->iRowBuf.Format(KIconFormat, iconIndex); 
    const TDesC& fieldName = iArray[aIndex].Label();
    const TInt chars = Min(iRowBuf.MaxLength()-iRowBuf.Length(), fieldName.Length());
    MUTABLE_CAST(CListBoxModel*, this)->iRowBuf.Append(fieldName.Left(chars));
    
    return iRowBuf;
    }

TInt CPbkMemoryEntrySelectAddItemDlg::CListBoxModel::Compare
        (const TPbkAddItemWrapper& aLhs, const TPbkAddItemWrapper& aRhs)
    {
    return aLhs.AddItemOrdering() - aRhs.AddItemOrdering();
    }

//  End of File  
