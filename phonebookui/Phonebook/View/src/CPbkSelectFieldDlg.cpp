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
*
*/


// INCLUDE FILES
#include    "CPbkSelectFieldDlg.h"
#include    <avkon.rsg>     // AVKON resources
#include    <aknPopup.h>    // CAknPopupList
#include    <aknlists.h>    // CAknSingleGraphicHeadingPopupMenuStyleListBox
#include    <eikclbd.h>     // CColumnListBoxData
#include    <calslbs.h>     // AknTextUtils

#include    <PbkView.rsg>   // View DLL resource Ids
#include    "CPbkFieldListBoxModel.h"
#include    "MPbkControlKeyObserver.h"
#include    <CPbkIconArray.h>
#include    "MPbkClipListBoxText.h"
#include    "cpbkfieldanalyzer.h"

#include    <CPbkContactItem.h> // CPbkFieldArray
#include    <CPbkExtGlobals.h>
#include    <MPbkExtensionFactory.h>
#include    <cpbkcontactengine.h>

// Unnamed namespace for local definitions
namespace {

// MODULE DATA STRUCTURES
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_AttemptExitL,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)   
    {
    _LIT(KPanicText, "CPbkSelectFieldDlg");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


/**
 * Creates and returns an icon array with phonebook 
 * default and extension icons.
 * 
 * @return array filled with icons
 */
CPbkIconArray* CreateListBoxIconArrayL()
    {
    // create icon array with default icons
    CPbkIconArray* iconArray = new (ELeave) CPbkIconArray(2);
    CleanupStack::PushL(iconArray);
    iconArray->ConstructFromResourceL(R_PBK_FIELDTYPE_ICONS);
    
    // get extension factory for setting extension icons
    CPbkExtGlobals* extGlobal = CPbkExtGlobals::InstanceL();
    extGlobal->PushL();
    MPbkExtensionFactory& factory = extGlobal->FactoryL();
    factory.AddPbkFieldIconsL(NULL, iconArray);
    CleanupStack::PopAndDestroy(extGlobal);

    CleanupStack::Pop();  // iconArray
    return iconArray;
    }

} // namespace


/**
 * Specialized CAknPopupList for CPbkSelectFieldDlg.
 */
NONSHARABLE_CLASS(CPbkSelectFieldDlg::CPopupList) : 
        public CAknPopupList
    {
    public:  // New functions
        /**
         * Like CAknPopupList::NewL.
		 * @see CAknPopupList::NewL
         */
        static CPopupList* NewL(
            CEikListBox* aListBox, 
            TInt aCbaResource, 
            AknPopupLayouts::TAknPopupLayouts aType =
				AknPopupLayouts::EMenuWindow);

        /**
         * Sets the listbox item to focus intially. By default focus is on 
         * the first item.
         * @param aFocusIndex  index of the listbox item to focus initially.
         */
        inline void SetInitialFocus(TInt aFocusIndex);

        /**
         * Like CAknPopupList::AttemptExitL.
		 * @see CAknPopupList::AttemptExitL
         */
        inline void AttemptExitL(TBool aAccept);

    private:  // from CAknPopupList
	    void SetupWindowLayout(AknPopupLayouts::TAknPopupLayouts aType);

    private:  // Implementation
        inline CPopupList();

    private:  // Data
		/// Own: focus index
        TInt iFocusIndex;
    };

/**
 * Specialized listbox for CPbkSelectFieldDlg.
 */
NONSHARABLE_CLASS(CPbkSelectFieldDlg::CListBox) : 
        public CAknDoubleGraphicPopupMenuStyleListBox,
        public MPbkClipListBoxText
    {
    public:  // New functions
        /**
         * Constructor.
         */
        inline CListBox();

        /**
         * Sets the observer aKeyObserver.
         */
        inline void SetObserver(MPbkControlKeyObserver* aKeyObserver);

        /**
         * Returns CONE environment.
         */
        inline CCoeEnv* CoeEnv();

    private:  // from CCoeControl
	    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private:  // from MPbkClipListBoxText
        TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber);

    private:  // Data
		/// Ref: observer
        MPbkControlKeyObserver* iKeyObserver;
    };


// ================= MEMBER FUNCTIONS =======================

// CPbkSelectFieldDlg::CPopupList
inline CPbkSelectFieldDlg::CPopupList::CPopupList() :
    iFocusIndex(-1)
    {
	// Initializes the focus to -1 which is surely an errornous value
    }

CPbkSelectFieldDlg::CPopupList* CPbkSelectFieldDlg::CPopupList::NewL
        (CEikListBox* aListBox, 
        TInt aCbaResource, 
        AknPopupLayouts::TAknPopupLayouts aType)
    {
    CPopupList* self = new(ELeave) CPopupList;
    CleanupStack::PushL(self);
    self->ConstructL(aListBox, aCbaResource, aType);
    CleanupStack::Pop();  // self
    return self;
    }

inline void CPbkSelectFieldDlg::CPopupList::SetInitialFocus(TInt aFocusIndex)
    {
    iFocusIndex = aFocusIndex;
    }

inline void CPbkSelectFieldDlg::CPopupList::AttemptExitL(TBool aAccept)
    {
    CAknPopupList::AttemptExitL(aAccept);
    }

void CPbkSelectFieldDlg::CPopupList::SetupWindowLayout
        (AknPopupLayouts::TAknPopupLayouts aType)
    {
    // Call base class
    CAknPopupList::SetupWindowLayout(aType);
    // Set initial focus
    if (iFocusIndex >= 0)
        {
        ListBox()->SetCurrentItemIndex(iFocusIndex);
        }
    }

// CPbkSelectFieldDlg::CListBox
inline CPbkSelectFieldDlg::CListBox::CListBox()
    {
    }

inline void CPbkSelectFieldDlg::CListBox::SetObserver(MPbkControlKeyObserver* aKeyObserver)
    {
    iKeyObserver = aKeyObserver;
    }

inline CCoeEnv* CPbkSelectFieldDlg::CListBox::CoeEnv()
    {
    return iCoeEnv;
    }

TKeyResponse CPbkSelectFieldDlg::CListBox::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (iKeyObserver &&
        iKeyObserver->PbkControlKeyEventL(aKeyEvent,aType) == EKeyWasConsumed)
        {
        return EKeyWasConsumed;
        }
    return CAknDoubleGraphicPopupMenuStyleListBox::OfferKeyEventL(aKeyEvent, aType);
    }

TBool CPbkSelectFieldDlg::CListBox::ClipFromBeginning
        (TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber)
    {
    return AknTextUtils::ClipToFit
        (aBuffer, 
        AknTextUtils::EClipFromBeginning, 
        this, 
        aItemIndex, 
        aSubCellNumber);
    }

EXPORT_C CPbkSelectFieldDlg::CPbkSelectFieldDlg()
    {
    __ASSERT_DEBUG(
        !iListBox && !iPopupList && !iKeyObserver && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

EXPORT_C TPbkContactItemField* CPbkSelectFieldDlg::ExecuteLD
        (CPbkFieldArray& aArray, 
        TInt aCbaResourceId,
        const TDesC& aHeading /*=KNullDesC*/,
        TInt aFocusIndex /*=-1*/)
    {
    __ASSERT_DEBUG(!iListBox && !iPopupList, Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    // Destructor will set thisDestroyed to ETrue if this object is destroyed
    iDestroyedPtr = &thisDestroyed;

    // Create a list box
    iListBox = new(ELeave) CListBox;

    // Create a popup list
    CPopupList* popupList = CPopupList::NewL
        (iListBox, aCbaResourceId, AknPopupLayouts::EMenuDoubleLargeGraphicWindow);
    CleanupStack::PushL(popupList);
    	
    // Init list box
    iListBox->ConstructL(popupList, CEikListBox::ELeftDownInViewRect);
    iListBox->SetObserver(iKeyObserver);

    // set title of popuplist
    if(aHeading.Length() > 0)
        {
        popupList->SetTitleL(aHeading);
        }
    
    CPbkIconArray* iconArray = CreateListBoxIconArrayL();
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(iconArray);

    HBufC* timeFormat = iListBox->CoeEnv()->AllocReadResourceLC(R_QTN_DATE_USUAL);

    CPbkFieldListBoxModel::TParams params( aArray, *timeFormat, *iconArray);

    CPbkFieldAnalyzer* fieldAnalyzer = NULL;
    // IF the contact item is set, initialize and add the field analyzer into
    // the param for list box model.
    fieldAnalyzer = CPbkFieldAnalyzer::NewL( aArray );
    params.iFieldAnalyzer = fieldAnalyzer;
    CleanupStack::PushL( fieldAnalyzer );

    CPbkFieldListBoxModel* listBoxModel = CPbkFieldListBoxModel::NewL(params);
    
    CleanupStack::PopAndDestroy( fieldAnalyzer ); // fieldAnalyzer
    params.iFieldAnalyzer = NULL;

    CleanupStack::PopAndDestroy();  // timeFormat
    listBoxModel->SetClipper(*iListBox); 
    iListBox->Model()->SetItemTextArray(listBoxModel);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->Reset();

    if (aFocusIndex >= 0 && aFocusIndex < iListBox->Model()->NumberOfItems())
        {
        popupList->SetInitialFocus(aFocusIndex);
        }

	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
		(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);

    TPbkContactItemField* result = NULL;
    CleanupStack::Pop();  // listBox
    iPopupList = popupList;
    TInt dlgResult = 0;
    TRAPD(err, dlgResult = iPopupList->ExecuteLD());
    if (!thisDestroyed)
		{
		iPopupList = NULL;
		}
    if (err != KErrNone)
		{
		User::Leave(err);
		}

    if (thisDestroyed)
        {
        // this object has been destroyed
        CleanupStack::Pop();  // this
        }
    else
        {
        if (dlgResult)
            {
            TInt index = iListBox->CurrentItemIndex();
            result = &aArray[index];
            }
        CleanupStack::PopAndDestroy(); // this
        }

    return result;
    }

EXPORT_C CPbkSelectFieldDlg::~CPbkSelectFieldDlg()
    {
    if (iSelfPtr)
		{
		*iSelfPtr = NULL;
		}
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

EXPORT_C void CPbkSelectFieldDlg::SetObserver(MPbkControlKeyObserver* aKeyObserver)
    {
    iKeyObserver = aKeyObserver;
    }

EXPORT_C void CPbkSelectFieldDlg::AttemptExitL(TBool aAccept)
    {
    __ASSERT_DEBUG(iPopupList, Panic(EPanicPreCond_AttemptExitL));

    iPopupList->AttemptExitL(aAccept);
    }

EXPORT_C void CPbkSelectFieldDlg::ResetWhenDestroyed(CPbkSelectFieldDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }
//  End of File
