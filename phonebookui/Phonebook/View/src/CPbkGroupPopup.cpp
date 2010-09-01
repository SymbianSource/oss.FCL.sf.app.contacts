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
*       Provides methods for UI control for Phonebook's "Add to group".
*
*/


// INCLUDE FILES
#include "CPbkGroupPopup.h"  // This class
#include <aknlists.h>   // AknListBoxUtils
#include <aknPopup.h>   // CAknPopupList

#include "CPbkInputAbsorber.h"
#include "CPbkContactGroupListModel.h"

#include <CPbkContactEngine.h>  // Phonebook engine
#include <CPbkContactItem.h>    // CPbkContactItem

/// Unnamed namespace for local defintions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_RunPopupL,
    EPanicLogic_RunPopupL,
    EPanicPostCond_ExecuteLD,
    EPanicPreCond_ResetWhenDestroyed
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupPopup");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// MODULE DATA STRUCTURES

NONSHARABLE_CLASS(CPbkGroupPopup::CPopupList) : 
        public CAknPopupList
    {
    public:
        /**
         * Static constructor.
         * @param aListbox the listbox to use in the list
         * @param aCbaResource softkey resource
         * @param aType popup layout type
         * @return a new instance of this class
         */
	    static CPopupList* NewL(CEikListBox* aListBox, TInt aCbaResource,
            AknPopupLayouts::TAknPopupLayouts aType=AknPopupLayouts::EMenuWindow);
        /**
         * Destructor.
         */
        ~CPopupList();
        /**
         * Resets the object pointer after destruction.
         */
        void ResetWhenDestroyed(CPopupList** aSelf);

    protected:
        /**
         * Constructor.
         */
        CPopupList();

    private:  // Data
        /// Ref: self pointer
        CPopupList** iSelfPtr;
    };




// ================= MEMBER FUNCTIONS =======================

// CPbkGroupPopup::CPopupList
inline CPbkGroupPopup::CPopupList::CPopupList()
    {
    }

CPbkGroupPopup::CPopupList* CPbkGroupPopup::CPopupList::NewL
        (CEikListBox* aListBox, 
        TInt aCbaResource, 
        AknPopupLayouts::TAknPopupLayouts aType)
    {
    CPopupList* self = new(ELeave) CPopupList;
    CleanupStack::PushL(self);
    self->CAknPopupList::ConstructL(aListBox, aCbaResource, aType);
    CleanupStack::Pop(self);
    return self;
    }

CPbkGroupPopup::CPopupList::~CPopupList()
    {
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    }

void CPbkGroupPopup::CPopupList::ResetWhenDestroyed(CPopupList** aSelf)
    {
    __ASSERT_DEBUG(!aSelf || *aSelf==this, Panic(EPanicPreCond_ResetWhenDestroyed));
    iSelfPtr = aSelf;
    }


// CPbkGroupPopup::TParams
CPbkGroupPopup::TParams::TParams
        (CContactViewBase& aGroupView, 
        TInt aInitialSoftkeyResId,
        TInt aSoftKeyResId) :
    iGroupView(&aGroupView),
    iInitialSoftkeyResId(aInitialSoftkeyResId),
    iSoftkeyResId(aSoftKeyResId),
    iPrompt(NULL),
    iEmptyText(NULL),
    iListBoxFlags(0),
    iSelectedGroupId(KNullContactId)
    {
    }


// CPbkGroupPopup
inline CPbkGroupPopup::CPbkGroupPopup(TParams& aParams) :
    iParams(aParams)
    {
    __ASSERT_DEBUG
        (!iInputAbsorber && !iListBox && !iPopupList && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

inline void CPbkGroupPopup::ConstructL()
    {
    }

CPbkGroupPopup* CPbkGroupPopup::NewL(TParams& aParams)
    {
    CPbkGroupPopup* self = new(ELeave) CPbkGroupPopup(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkGroupPopup::~CPbkGroupPopup()
    {
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    if (iPopupList)
        {
        iPopupList->CancelPopup();
        }
    iParams.iGroupView->Close(*this);
    delete iListBox;
    delete iInputAbsorber;
    }

TInt CPbkGroupPopup::ExecuteLD()
    {
    __ASSERT_DEBUG(!iListBox && !iPopupList, Panic(EPanicPreCond_ExecuteLD));

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    CleanupStack::PushL(this);

    iInputAbsorber = CPbkInputAbsorber::NewL(iParams.iInitialSoftkeyResId);
    iInputAbsorber->SetCommandObserver(this);

    // Register to receive view events
    iParams.iGroupView->OpenL(*this);

    // Use CPbkInputAbsorber to provide modal behaviour until we get the popup list up
    iInputAbsorber->Wait();

    TInt result = 0;
    if (thisDestroyed)
        {
        CleanupStack::Pop(this);
        }
    else
        {
        result = iResult;
        if (result)
            {
            const TInt index = iListBox->CurrentItemIndex();
            iParams.iSelectedGroupId = iParams.iGroupView->AtL(index);
            }
        __ASSERT_DEBUG(!result || iParams.iSelectedGroupId!=KNullContactId,
            Panic(EPanicPostCond_ExecuteLD));
        CleanupStack::PopAndDestroy(this);
        }
    return result;
    }

void CPbkGroupPopup::ResetWhenDestroyed(CPbkGroupPopup** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
        Panic(EPanicPreCond_ResetWhenDestroyed));
    iSelfPtr = aSelfPtr;
    }

void CPbkGroupPopup::HandleContactViewEvent
        (const CContactViewBase& /*aView*/,
        const TContactViewEvent& aEvent)
    {
    TRAPD(err, DoHandleContactViewEventL(aEvent));
    if (err != KErrNone)
        {
        CCoeEnv::Static()->HandleError(err);
        Close();
        }
    }

void CPbkGroupPopup::ProcessCommandL(TInt aCommandId)
    {
    switch (aCommandId)
        {
        case EAknSoftkeyCancel: // FALLTHROUGH
        case EAknSoftkeyBack:   // FALLTHROUGH
        case EAknSoftkeyNo:     // FALLTHROUGH
        case EAknSoftkeyClose:  // FALLTHROUGH
        case EAknSoftkeyExit:   
            {
            Close();
            break;
            }

        default:
            break;
        }
    }

void CPbkGroupPopup::RunPopupL()
    {
    __ASSERT_DEBUG(iInputAbsorber->IsWaiting(), Panic(EPanicPreCond_RunPopupL));

    // Create a list box
    iListBox = static_cast<CEikColumnListBox*>
        (EikControlFactory::CreateByTypeL(EAknCtSinglePopupMenuListBox).iControl);

    // Create a popup list
    iPopupList = CPopupList::NewL(iListBox, iParams.iSoftkeyResId);
    iPopupList->ResetWhenDestroyed(&iPopupList);

    // Init list box
    iListBox->ConstructL(iPopupList, iParams.iListBoxFlags | CEikListBox::ELeftDownInViewRect);
    CPbkContactGroupListModel* listBoxModel = CPbkContactGroupListModel::NewL
        (*iParams.iGroupView);
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    iListBox->Model()->SetItemTextArray(listBoxModel);
	iListBox->CreateScrollBarFrameL(ETrue);
	iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
    
    // Set empty text for the listbox
    if (iParams.iEmptyText)
        {
        iListBox->View()->SetListEmptyTextL(*iParams.iEmptyText);
        }

    // set title of popuplist
    if (iParams.iPrompt && iParams.iPrompt->Length() > 0)
        {
        iPopupList->SetTitleL(*iParams.iPrompt);
        }

    iResult = iPopupList->ExecuteLD();
    __ASSERT_DEBUG(!iPopupList, Panic(EPanicLogic_RunPopupL));

    // Wait is started in ExecuteLD()
    iInputAbsorber->StopWait();
    }

void CPbkGroupPopup::Close()
    {
    // Wait is started in ExecuteLD()
    iInputAbsorber->StopWait();
    delete this;
    }

void CPbkGroupPopup::DoHandleContactViewEventL
        (const TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EReady:
            {
            // Run the popup list query
            RunPopupL();
            break;
            }

        case TContactViewEvent::ESortOrderChanged:
            {
            if (iListBox)
                {
                iListBox->Reset();
                iListBox->DrawNow();
                }
            break;
            }

        case TContactViewEvent::EItemAdded:
            {
            if (iListBox)
                {
                const TInt focusIndex = iListBox->CurrentItemIndex();
                if (focusIndex >= aEvent.iInt)
                    {
                    iListBox->SetCurrentItemIndex(focusIndex + 1);
                    }
                iListBox->HandleItemAdditionL();
                }
            break;
            }

        case TContactViewEvent::EItemRemoved:
            {
            if (iListBox)
                {
                const TInt focusIndex = iListBox->CurrentItemIndex();
                AknListBoxUtils::HandleItemRemovalAndPositionHighlightL
                    (iListBox, focusIndex, focusIndex==aEvent.iInt);
                }
            break;
            }

        case TContactViewEvent::EUnavailable:
            {
            Close();
            break;
            }

        case TContactViewEvent::ESortError:     // FALLTHROUGH
        case TContactViewEvent::EServerError:   // FALLTHROUGH
        case TContactViewEvent::EIndexingError:
            {
            CCoeEnv::Static()->HandleError(aEvent.iInt);
            Close();
            break;
            }

        default:
            {
            Close();
            break;
            }
        }
    }


//  End of File  
