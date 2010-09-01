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
*       Sub view to a CContactViewBase.
*
*/


// INCLUDES
#include <CPbkContactSubView.h>
#include <MPbkContactSelector.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    ECntPanicViewNotReady = 1,
    EPanicPreCond_ConstructL,
    EPanicPostCond_FindL,
    EPanicPreCond_HandleContactViewEvent,
    EPanicPreCond_SetStateReady,
    EPanicPreCond_StartMapping,
    EPanicPostCond_AddMappingL,
    EPanicPreCond_HandleItemAdded,
    EPanicPostCond_HandleItemAdded,
    EPanicPreCond_HandleItemRemoved,
    EPanicPostCond_HandleItemRemoved,
    EPanicInvariant
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkContactSubView");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace


// MODULE DATA STRUCTURES

/**
 * Background processing active object for filling CPbkContactSubView.
 */
NONSHARABLE_CLASS(CPbkContactSubView::CMapper) : 
        public CActive
    {
    public:
        CMapper(CPbkContactSubView& aView) ;
        ~CMapper();
        void Start();

    private:  // from CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:  // Implementation
        void RequestIteration();

    private:  // Data
        /// Ref: reference to a sub view
        CPbkContactSubView& iView;
        /// Own: mapper state
        TInt iIndex;
    };


// ================= MEMBER FUNCTIONS =======================

/**
 * Searches this view for the index of aId. If not found returns KErrNotFound.
 */
inline TInt CPbkContactSubView::DoFind(TContactItemId aId) const
    {
    TIdWithMapping findMap;
    findMap.iId = aId;
    return iFilteredIdArray.Find(findMap,TIdentityRelation<TIdWithMapping>(IdsEqual));
    }

inline CPbkContactSubView::CPbkContactSubView
        (const CContactDatabase& aDb,
        MPbkContactSelector& aContactSelector)
    : CContactViewBase(aDb), iContactSelector(&aContactSelector)
    {
    // CBase::operator new will reset other members
    }

EXPORT_C CPbkContactSubView* CPbkContactSubView::NewL
        (MContactViewObserver& aObserver,
        const CContactDatabase& aDb,
        CContactViewBase& aBaseView,
        MPbkContactSelector& aContactSelector)
    {
    CPbkContactSubView* self=new(ELeave) CPbkContactSubView(aDb, 
        aContactSelector);
    CleanupStack::PushL(self);
    self->ConstructL(aObserver, aBaseView);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C MPbkContactSelector& CPbkContactSubView::ContactSelector() const
    {
    return *iContactSelector;
    }

EXPORT_C void CPbkContactSubView::SetContactSelector
        (MPbkContactSelector& aContactSelector)
    {
    iContactSelector = &aContactSelector;
    Refresh();
    }

EXPORT_C void CPbkContactSubView::Refresh()
    {
    SetStateInitializing();
    StartMapping(TContactViewEvent::EReady);
    }

EXPORT_C void CPbkContactSubView::AddContactL(TContactItemId aContactId)
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));

    if (iContactSelector->IsContactIncluded(aContactId) &&
        DoFind(aContactId) == KErrNotFound)
        {
        TIdWithMapping mapEntry;
        mapEntry.iId = aContactId;
        mapEntry.iMapping = iBaseView->FindL(aContactId);
        if (mapEntry.iMapping < 0)
            {
            User::Leave(KErrNotFound);
            }

        User::LeaveIfError(iFilteredIdArray.InsertInOrderAllowRepeats
            (mapEntry, &CPbkContactSubView::CompareIndexes));
        const TInt index = DoFind(aContactId);
        TContactViewEvent event
            (TContactViewEvent::EItemAdded,index,aContactId);
        NotifyObservers(event);
        }
    }

EXPORT_C void CPbkContactSubView::RemoveContact(TContactItemId aContactId)
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    const TInt index = DoFind(aContactId);
    if (index != KErrNotFound)
        {
        iFilteredIdArray.Remove(index);
        TContactViewEvent event
            (TContactViewEvent::EItemRemoved,index,aContactId);
        NotifyObservers(event);
        }
    }

EXPORT_C void CPbkContactSubView::AddBaseViewPreEventHandlerL
        (MContactViewObserver& aObserver)
    {
    User::LeaveIfError(iPreBaseViewObservers.Append(&aObserver));
    }

EXPORT_C void CPbkContactSubView::RemoveBaseViewPreEventHandler
        (MContactViewObserver& aObserver)
    {
    const TInt index = iPreBaseViewObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iPreBaseViewObservers.Remove(index);
        }
    }

TContactItemId CPbkContactSubView::AtL(TInt aIndex) const
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    if (aIndex >= iFilteredIdArray.Count())
        {
        //Out of Bounds.
        User::Leave(KErrNotFound);
        }
    return iFilteredIdArray[aIndex].iId;
    }

const CViewContact& CPbkContactSubView::ContactAtL(TInt aIndex) const
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    if (aIndex >= iFilteredIdArray.Count())
        {
        //Out of Bounds.
        User::Leave(KErrNotFound);
        }

    const TInt baseViewIndex = iFilteredIdArray[aIndex].iMapping;
    return iBaseView->ContactAtL(baseViewIndex);
    }

TInt CPbkContactSubView::CountL() const
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    return iFilteredIdArray.Count();
    }

TInt CPbkContactSubView::FindL(TContactItemId aId) const
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    const TInt index = DoFind(aId);
    __ASSERT_DEBUG(index==KErrNotFound || 
        iFilteredIdArray[index].iId==aId, Panic(EPanicPostCond_FindL));
    return index;
    }

HBufC* CPbkContactSubView::AllFieldsLC
        (TInt aIndex,const TDesC& aSeparator) const
    {
    __ASSERT_DEBUG(iState==EReady,Panic(ECntPanicViewNotReady));
    if (aIndex >= iFilteredIdArray.Count())
        {
        //Out of Bounds.
        User::Leave(KErrNotFound);
        }

    const TInt baseViewIndex = iFilteredIdArray[aIndex].iMapping;
    return iBaseView->AllFieldsLC(baseViewIndex, aSeparator);
    }

TContactViewPreferences CPbkContactSubView::ContactViewPreferences()
    {
    return iBaseView->ContactViewPreferences();
    }

const RContactViewSortOrder& CPbkContactSubView::SortOrderL() const
    {
    return iBaseView->SortOrderL();
    }

TAny* CPbkContactSubView::CContactViewBase_Reserved_1
        (TFunction aFunction,TAny* aParams)
    {
    return CContactViewBase::CContactViewBase_Reserved_1(aFunction, aParams);
    }

/**
 * Handles an event from iBaseView and synchronizes this view's state
 * accordingly.
 */
void CPbkContactSubView::HandleContactViewEvent
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
    {
    __ASSERT_DEBUG(&aView == iBaseView,
        Panic(EPanicPreCond_HandleContactViewEvent));

    // Forward event to base view pre-observers first
    const TInt viewObserverCount = iPreBaseViewObservers.Count();
    for (TInt i=0; i < viewObserverCount; ++i)
        {
        iPreBaseViewObservers[i]->HandleContactViewEvent(aView,aEvent);
        }

    // Handle event
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::ESortError:     //FALLTHROUGH
        case TContactViewEvent::EServerError:   //FALLTHROUGH
        case TContactViewEvent::EIndexingError: //FALLTHROUGH
        case TContactViewEvent::EUnavailable:
            {
            SetStateNotReady(aEvent);
            break;
            }

        case TContactViewEvent::EReady:
            {
            SetStateInitializing();
            StartMapping(TContactViewEvent::EReady);
            break;
            }

        case TContactViewEvent::ESortOrderChanged:
            {
            SetStateInitializing();
            StartMapping(aEvent);
            break;
            }

        case TContactViewEvent::EItemAdded:
            {
            TContactViewEvent event = aEvent;
            if (HandleItemAdded(event))
                {
                NotifyObservers(event);
                }
            break;
            }

        case TContactViewEvent::EItemRemoved:
            {
            TContactViewEvent event = aEvent;
            if (HandleItemRemoved(event))
                {
                NotifyObservers(event);
                }
            break;
            }

        default:
            break;
        }
    }

void CPbkContactSubView::ConstructL
        (MContactViewObserver& aObserver, CContactViewBase& aView)
    {
    CContactViewBase::ConstructL();
    __ASSERT_DEBUG(iState==EInitializing, Panic(EPanicPreCond_ConstructL));
    iMapper = new(ELeave) CMapper(*this);
    OpenL(aObserver);
    aView.OpenL(*this);
    iBaseView = &aView;
    }

CPbkContactSubView::~CPbkContactSubView()
    {
    delete iMapper;
    if (iBaseView)
        {
        iBaseView->Close(*this);
        }
    iPreBaseViewObservers.Close();
    iFilteredIdArray.Close();
    }

/**
 * Switches this view to EReady state and notifies observers with aEvent.
 */
void CPbkContactSubView::SetStateReady(const TContactViewEvent& aEvent)
    {
    __ASSERT_DEBUG(iState != EReady, Panic(EPanicPreCond_SetStateReady));
    iState = EReady;
    NotifyObservers(aEvent);
    }

/**
 * Switches this view to EInitializing state and notifies observers with
 * TContactViewEvent::EUnavailable if the current state is EReady.
 */
void CPbkContactSubView::SetStateInitializing()
    {
    // Observers need to be notified only if state transition is from
    // EReady -> EInitializing.
    const TBool sendEvent(iState==EReady);
    iState = EInitializing;
    ResetMapping();
    if (sendEvent)
        {
        NotifyObservers(TContactViewEvent::EUnavailable);
        }
    }

/**
 * Switches this view to ENotReady state and notifies observers with aEvent.
 */
void CPbkContactSubView::SetStateNotReady(const TContactViewEvent& aEvent)
    {
    iState = ENotReady;
    ResetMapping();
    NotifyObservers(aEvent);
    }

/**
 * Starts mapping contacts from iBaseView to this view.
 */
void CPbkContactSubView::StartMapping
        (const TContactViewEvent& aCompleteEvent)
    {
    __ASSERT_DEBUG(iState!=EReady,
        Panic(EPanicPreCond_StartMapping));

    iMappingCompleteEvent = aCompleteEvent;
    ResetMapping();
    iMapper->Start();
    }

/**
 * Resets any mapping currently in progress.
 */
void CPbkContactSubView::ResetMapping()
    {
    iMapper->Cancel();
    iFilteredIdArray.Reset();
    }

/**
 * Called by CMapper.
 */
void CPbkContactSubView::AddMappingL(const TIdWithMapping& aMapping)
    {
    if (iContactSelector->IsContactIncluded(aMapping.iId))
        {
        TInt myIndex;
        if (iFilteredIdArray.FindInOrder
                (aMapping, myIndex, &CPbkContactSubView::CompareIndexes)
                == KErrNotFound)
            {
            User::LeaveIfError(iFilteredIdArray.Insert(aMapping, myIndex));
            }

        __ASSERT_DEBUG(
            iFilteredIdArray[myIndex].iId==aMapping.iId,
            Panic(EPanicPostCond_AddMappingL));
        }
    else
        {
        __ASSERT_DEBUG(DoFind(aMapping.iId)==KErrNotFound,
            Panic(EPanicPostCond_AddMappingL));
        }
    }

/**
 * Called by CMapper.
 */
inline void CPbkContactSubView::MappingComplete()
    {
    SetStateReady(iMappingCompleteEvent);
    }

/**
 * Called by CMapper.
 */
inline void CPbkContactSubView::MappingError(TInt aError)
    {
    SetStateNotReady(TContactViewEvent(TContactViewEvent::ESortError,aError));
    }

TBool CPbkContactSubView::HandleItemAdded(TContactViewEvent& aEvent)
    {
    TBool result = EFalse;
    TRAPD(err, result = HandleItemAddedL(aEvent));
    if (err != KErrNone)
        {
        TContactViewEvent event;
        event.iEventType = TContactViewEvent::ESortError;
        event.iInt = err;
        SetStateNotReady(event);
        }
    return result;
    }

/**
 * Handles item added event from iBaseView.
 */
TBool CPbkContactSubView::HandleItemAddedL(TContactViewEvent& aEvent)
    {
    TBool forwardEvent = EFalse;
    __ASSERT_DEBUG(aEvent.iInt >= 0, Panic(EPanicPreCond_HandleItemAdded));
    if (iState == ENotReady)
        {
        return forwardEvent;
        }

    if (iContactSelector->IsContactIncluded(aEvent.iContactId))
        {
        TIdWithMapping mapEntry;
        mapEntry.iId = aEvent.iContactId;
        mapEntry.iMapping = aEvent.iInt;
        // contact id is included in this view -> insert it to the mapping array
        User::LeaveIfError(iFilteredIdArray.InsertInOrderAllowRepeats
            (mapEntry, &CPbkContactSubView::CompareIndexes));
        forwardEvent = (iState==EReady);
        }
    else
        {
        __ASSERT_DEBUG(DoFind(aEvent.iContactId)==KErrNotFound,
            Panic(EPanicPostCond_HandleItemAdded));
        }

    // Always update the mapping indexes
    UpdateMappingsL();

    if (forwardEvent)
        {
        // Notify this view's observers
        aEvent.iInt = DoFind(aEvent.iContactId);
        if (aEvent.iInt >= 0)
            {
            __ASSERT_DEBUG(
                iFilteredIdArray[aEvent.iInt].iId==aEvent.iContactId,
                Panic(EPanicPostCond_HandleItemAdded));
            }
        }
    return forwardEvent;
    }

TBool CPbkContactSubView::HandleItemRemoved(TContactViewEvent& aEvent)
    {
    TBool result = EFalse;
    TRAPD(err, result = HandleItemRemovedL(aEvent));
    if (err != KErrNone)
        {
        TContactViewEvent event;
        event.iEventType = TContactViewEvent::ESortError;
        event.iInt = err;
        SetStateNotReady(event);
        }
    return result;
    }

/**
 * Handles item removed event from iBaseView.
 */
TBool CPbkContactSubView::HandleItemRemovedL(TContactViewEvent& aEvent)
    {
    TBool forwardEvent = EFalse;
    __ASSERT_DEBUG(aEvent.iInt >= 0, Panic(EPanicPreCond_HandleItemRemoved));
    if (iState == ENotReady)
        {
        return forwardEvent;
        }

    const TInt index = DoFind(aEvent.iContactId);
    if (index >= 0)
        {
        iFilteredIdArray.Remove(index);
        aEvent.iInt = index;
        forwardEvent = (iState==EReady);
        }

    // Always update the mapping indexes
    UpdateMappingsL();

    __ASSERT_DEBUG(DoFind(aEvent.iContactId)==KErrNotFound,
        Panic(EPanicPostCond_HandleItemRemoved));
    return forwardEvent;
    }

/**
 * Updates all this view's index mappings.
 */
TInt CPbkContactSubView::UpdateMappingsL()
    {
    TInt indexesUpdated = 0;
    for (TInt i = iFilteredIdArray.Count()-1; i >= 0; --i)
        {
        const TContactItemId contactId = iFilteredIdArray[i].iId;
        const TInt underlyingViewIndex = iBaseView->FindL(contactId);
        if (underlyingViewIndex >= 0)
            {
            // Check and update mapping index
            if (iFilteredIdArray[i].iMapping != underlyingViewIndex)
                {
                iFilteredIdArray[i].iMapping = underlyingViewIndex;
                ++indexesUpdated;
                }
            }
        else
            {
            // the contact is no longer present in the underlying view
            iFilteredIdArray.Remove(i);
            }
        }

    if (indexesUpdated > 0)
        {
        // Do a re-sort in case the indexes changed so that we need to rearrange
        // iFilteredIdArray.
        iFilteredIdArray.Sort(&CPbkContactSubView::CompareIndexes);
        }
    return KErrNone;
    }

TInt CPbkContactSubView::CompareIndexes
        (const TIdWithMapping& aLhs, const TIdWithMapping& aRhs)
    {
    return (aLhs.iMapping - aRhs.iMapping);
    }

TBool CPbkContactSubView::IdsEqual
        (const TIdWithMapping& aLhs, const TIdWithMapping& aRhs)
    {
    return (aLhs.iId == aRhs.iId);
    }

// CPbkContactSubView::CMapper

CPbkContactSubView::CMapper::CMapper(CPbkContactSubView& aView)
    : CActive(CActive::EPriorityLow),
    iView(aView),
    iIndex(KErrNotFound)
    {
    CActiveScheduler::Add(this);
    }

CPbkContactSubView::CMapper::~CMapper()
    {
    Cancel();
    }

void CPbkContactSubView::CMapper::Start()
    {
    iIndex = 0;
    RequestIteration();
    }

void CPbkContactSubView::CMapper::DoCancel()
    {
    iIndex = KErrNotFound;
    }

void CPbkContactSubView::CMapper::RunL()
    {
    // Cancelled
    if (iIndex < 0)
        {
        iStatus = KErrCancel;
        return;
        }

    if (iIndex < iView.iBaseView->CountL())
        {
        // Add a mapping for a contact in base view if included in sub view
        // but not yet present
        CPbkContactSubView::TIdWithMapping mapEntry;
        TRAPD(err, mapEntry.iId = iView.iBaseView->AtL(iIndex));
        if (err == KErrNone)
            {
            mapEntry.iMapping = iIndex;
            iView.AddMappingL(mapEntry);
            ++iIndex;
            RequestIteration();
            return;
            }
        else if (err != KErrNotFound)
            {
            User::Leave(err);
            }
        }

    iView.MappingComplete();
    }

TInt CPbkContactSubView::CMapper::RunError(TInt aError)
    {
    iIndex = KErrNotFound;
    iView.MappingError(aError);
    return KErrNone;
    }

void CPbkContactSubView::CMapper::RequestIteration()
    {
    TRequestStatus* pS=(&iStatus);
    User::RequestComplete(pS, KErrNone);
    SetActive();
    }


// End of File
