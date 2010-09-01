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
*		Interface for CPbkContactFindView states
*
*/


// INCLUDES
#include "CPbkContactFindView.h"
#include "CPbkContactFindViewImpl.h"

// MACROS

/// Define this to print find performance data to debug output
#ifdef PBK_BENCHMARK_FIND
  #pragma message("Warning: PBK_BENCHMARK_FIND is set")
#endif

// MODULE DATA STRUCTURES

#ifdef PBK_BENCHMARK_FIND

/// Benchmarking helper class.
template<class NumT>
NONSHARABLE_CLASS(TRunningAverage)
    {
    public:  // Interface
        /**
         * Constructor.
         */
        inline TRunningAverage()
			{
			Reset();
			}
        /**
         * () operator.
         */
        inline NumT operator()() const
			{
			return iAverage;
			}
        /**
         * + operator
		 * @param aSample operand
         */
        inline TRunningAverage& operator+=(NumT aSample)
            {
            const TInt nextN = iN+1;
            iAverage = (iN*iAverage + aSample) / nextN;
            iN = nextN;
            return *this;
            }
        
        /**         
		 * @param aTimeSlot observed time
         */
        inline void Inc(NumT aTimeSlot)
            {
            iTotal +=aTimeSlot;            
            }
               
        /**         
		 * Returns total time
         */
        inline NumT Total() const
            {
            return iTotal;
            }
                                               
        /**
         * Reset.
         */
        inline void Reset()
			{
			iAverage=0; iN=0; iTotal=0;
			}

    private:  // Data
		/// Own: average
        NumT iAverage;
		/// Own: count
        TInt iN;
        /// Own: total time
        NumT iTotal;
    };

#endif  // PBK_BENCHMARK_FIND

/**
 * Base class for CPbkContactFindView states.
 */
NONSHARABLE_CLASS(CPbkContactFindView::CStateBase) : 
        public CBase, 
        public CPbkContactFindView::MState
    {
    public:  // from MState
        TBool SetFindTextL
            (const TDesC& aFindText, const CContactIdArray* aIncludeAlways);
        const TDesC& FindText() const;
        TBool ResetFind();
        TInt IndexOfFirstFindMatchL() const;
	    TContactItemId AtL(TInt aIndex) const;
	    const CViewContact& ContactAtL(TInt aIndex) const;
	    TInt CountL() const;
	    TInt FindL(TContactItemId aId) const;
	    HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
        TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);

    protected:  // Derived class interface
        /**
         * Constructor.
		 * @param aParent parent view
         */
        CStateBase(CPbkContactFindView& aParent);
        /**
         * Returns parents baseview.
         */
        CContactViewBase& BaseView() const;

        /**
         * Override to handle contact a contact view event.
         * @param aView     View where the event occured.
         * @param aEvent    The event. May be modified by this function.
         * @return  ETrue if the event should be forwarded to parent view's 
         *          observers
         */
	    virtual TBool DoHandleContactViewEventL
            (const CContactViewBase& aView, TContactViewEvent& aEvent) = 0;

    protected:  // Data
		/// Ref: parent view
        CPbkContactFindView& iParent;
    };


/**
 * A state of CPbkContactFindView. 
 * CPbkContactFindView is in this state when its underlying view is not ready. 
 * This state emulates an empty view as closely as possible.
 */
NONSHARABLE_CLASS(CPbkContactFindView::CNotReadyState) : 
        public CPbkContactFindView::CStateBase
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
		 * @param aParent parent view
         */
        static CNotReadyState* NewL(CPbkContactFindView& aParent);
        /**
         * Switches to ready state.
         */
        void SwitchToReadyStateL();

    public:  // from MState
        TBool SetFindTextL
            (const TDesC& aFindText, const CContactIdArray* aIncludeAlways);

    protected:  // from CStateBase
	    TBool DoHandleContactViewEventL
            (const CContactViewBase& aView, TContactViewEvent& aEvent);

    protected:  // Implementation
        /**
         * Constructor.
		 * @param aParent parent view
         */
        CNotReadyState(CPbkContactFindView& aParent);
    };


/**
 * A state of CPbkContactFindView. 
 * CPbkContactFindView is in this state when its underlying view is ready but
 * find is not active. This state delegates all CContactViewBase API calls
 * directly to the underlying view.
 */
NONSHARABLE_CLASS(CPbkContactFindView::CReadyState) : 
        public CPbkContactFindView::CStateBase
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
		 * @param aParent parent view
		 * @param aNotReadyState not ready state object
         */
        static CReadyState* NewL
            (CPbkContactFindView& aParent, 
            CNotReadyState* aNotReadyState);

		/**
         * Destructor.
         */
        ~CReadyState();

        /**
         * Switches to not ready state. Deletes itself.
         */
        void SwitchToNotReadyStateD();

        /**
         * Switches to find state.
		 * @param aFindText text to match
		 * @param aIncludeAlways contacts to include always in this view
         *        even if they don't match aFindText
		 * @return 
         */
        TBool SwitchToFindStateL
            (const TDesC& aFindText, const CContactIdArray* aIncludeAlways);

#ifdef PBK_BENCHMARK_FIND
        TRunningAverage<TInt> iAvgMatchTime;
        TRunningAverage<TInt> iTotalRefineTime;
        TRunningAverage<TInt> iTotalAlwaysIncludedBranch;
        TRunningAverage<TInt> iTotalBaseView_FindL;
        TRunningAverage<TInt> iTotalSafeGetContactL_result;
        TRunningAverage<TInt> iTotalSafeGetContactL;
        TRunningAverage<TInt> iTotalFind_success;
#endif

    public:  // from MState
        TBool SetFindTextL(const TDesC& aFindText, const CContactIdArray* aIncludeAlways);
    	TContactItemId AtL(TInt aIndex) const;
	    const CViewContact& ContactAtL(TInt aIndex) const;
	    TInt CountL() const;
	    TInt FindL(TContactItemId aId) const;
	    HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;

    protected:  // from CStateBase
	    TBool DoHandleContactViewEventL
            (const CContactViewBase& aView, TContactViewEvent& aEvent);

    protected:  // Implementation
        CReadyState(CPbkContactFindView& aParent);

    private:  // Data
		/// Own: not ready state object
        CNotReadyState* iNotReadyState;
    };


/**
 * A state of CPbkContactFindView. 
 * CPbkContactFindView is in this state when its underlying view is and
 * find filtering is active.
 */
NONSHARABLE_CLASS(CPbkContactFindView::CFindState) : 
        public CPbkContactFindView::CStateBase
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
		 * @param aParent parent view
		 * @param aNotReadyState ready state object
         */
        static CFindState* NewL
            (CPbkContactFindView& aParent, CReadyState* aReadyState);
        /**
         * Destructor.
         */
        ~CFindState();

        /**
         * Switches to ready state. Deletes itself.
         */
        void SwitchToReadyStateD();

        /**
         * Switches to not ready state. Deletes itself.
         */
        void SwitchToNotReadyStateD();

    public:  // from MState
        TBool SetFindTextL
            (const TDesC& aFindText, const CContactIdArray* aIncludeAlways);
        const TDesC& FindText() const;
        TBool ResetFind();
        TInt IndexOfFirstFindMatchL() const;
	    TContactItemId AtL(TInt aIndex) const;
	    const CViewContact& ContactAtL(TInt aIndex) const;
	    TInt CountL() const;
	    TInt FindL(TContactItemId aId) const;
	    HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
        TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);

    protected:  // from CStateBase
	    TBool DoHandleContactViewEventL
            (const CContactViewBase& aView, TContactViewEvent& aEvent);

    protected:  // Implementation
        /**
         * Constructor.
		 * @param aParent parent view
         */
        CFindState(CPbkContactFindView& aParent);

        /**
         * Second phase constructor.
		 * @param aReadyState ready state object
         */
        void ConstructL(CReadyState* aReadyState);

    private:  // Implementation
        void DeleteAllContactData();
        TBool IsFindMatchL(const CViewContact& aContact, 
            const TDesC& aFindText) const;
        TBool IsFindMatchL(const CViewContact& aContact) const;
        TBool MatchesInitialFindTextL(const CViewContact& aContact) const;
        TBool IsContactIncludedL(const CViewContact& aContact) const;
        TBool IsAlwaysIncluded(TContactItemId aContactId) const;
        void UpdateLD();
        void MakeInitialMatchL();
        static void SwitchToReadyStateD(TAny* aPtr);
        TBool HandleItemAddedL(TContactViewEvent& aEvent);
        TBool HandleItemRemoved(TContactViewEvent& aEvent);
        TInt DeleteAllContactData(TContactItemId aContactId);
        void StoreFindTextL(const TDesC& aFindText);
        TBool RefineFindL();
        TInt SafeGetContactL
            (TContactItemId aContactId, const CViewContact** aContact) const;

    private:  // Data
        /// Own: contacts included in this view
	    RPointerArray<CViewContact> iContacts;
        /// Own: contacts not currently included in this view
	    RPointerArray<CViewContact> iNonMatchingContacts;
        /// Own: contacts to include always in this view
        CContactIdArray* iAlwaysIncluded;
        /// Own: current find text
        HBufC* iFindText;
        /// Own: contact view sort order
        RContactViewSortOrder iViewSortOrder;
        /// Own: previous state
        CReadyState* iReadyState;
    };


/// Unnamed namespace for local definitions
namespace {

// ==================== LOCAL FUNCTIONS ====================

/**
 * Searches aArray for the index of contact with aId. 
 * If not found returns KErrNotFound.
 */
TInt Find(TContactItemId aId, const RPointerArray<CViewContact>& aArray)
    {
    const TInt count = aArray.Count(); 
    for (TInt i=0; i<count; ++i)
        {
        if (aArray[i]->Id() == aId)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactFindViewImpl");
    User::Panic(KPanicText,aReason);
    }

}  // namespace



// ================= MEMBER FUNCTIONS =======================

CPbkContactFindView::MState* CPbkContactFindView::MState::CreateInitialStateL
        (CPbkContactFindView& aParent)
    {
    return CNotReadyState::NewL(aParent);
    }

// CStateBase
inline CPbkContactFindView::CStateBase::CStateBase
        (CPbkContactFindView& aParent) 
    : iParent(aParent) 
    { 
    }

TBool CPbkContactFindView::CStateBase::SetFindTextL
        (const TDesC& /*aFindText*/, const CContactIdArray* /*aIncludeAlways*/)
    {
    return EFalse;
    }

const TDesC& CPbkContactFindView::CStateBase::FindText() const
    {
    return KNullDesC;
    }

TBool CPbkContactFindView::CStateBase::ResetFind()
    {
    return EFalse;
    }

TInt CPbkContactFindView::CStateBase::IndexOfFirstFindMatchL() const
    {
    return -1;
    }

TContactItemId CPbkContactFindView::CStateBase::AtL
        (TInt aIndex) const
    {
    if (aIndex >= 0)
        {
        User::Leave(KErrNotFound);
        }
    else
        {
        Panic(EPanicInvalidIndex);
        }
    // Dummy return to satisfy the compiler
    return KNullContactId;
    }

const CViewContact& CPbkContactFindView::CStateBase::ContactAtL
        (TInt aIndex) const
    {
    AtL(aIndex);
    // Dummy return to satisfy the compiler
    CBase* dummy = NULL;        
    return *static_cast<CViewContact*>(dummy);
    }

TInt CPbkContactFindView::CStateBase::CountL() const
    {
    return 0;
    }

TInt CPbkContactFindView::CStateBase::FindL(TContactItemId /*aId*/) const
    {
    return KErrNotFound;
    }

HBufC* CPbkContactFindView::CStateBase::AllFieldsLC
        (TInt aIndex,const TDesC& /*aSeparator*/) const
    {
    // Reuse aIndex checking code from AtL()
    AtL(aIndex);
    // Dummy return to satisfy the compiler
    return NULL;
    }

void CPbkContactFindView::CStateBase::HandleContactViewEvent
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
    {
    // Copy the event for possible modification
    TContactViewEvent event(aEvent);

    // Store iParent into a stack variable because this object might get
    // destroyed in DoHandleContactViewEventL()
    CPbkContactFindView& parent = iParent;

    // Handle the event
    TBool forward = ETrue;
    TRAPD(err, forward = DoHandleContactViewEventL(aView,event));

    // Handle errors
    if (err != KErrNone)
        {
        event.iEventType = TContactViewEvent::ESortError;
        event.iInt = err;
        event.iContactId = KNullContactId;
        }

    // Forward event to parent's observers
    if (forward)
        {
        parent.NotifyObservers(event);
        }
    }

TAny* CPbkContactFindView::CStateBase::CContactViewBase_Reserved_1
        (TFunction aFunction,TAny* aParams)
    {
    return iParent.iBaseView.CContactViewBase_Reserved_1(aFunction, aParams);
    }

inline CContactViewBase& CPbkContactFindView::CStateBase::BaseView() const
    {
    return iParent.iBaseView;
    }


// CNotReadyState
inline CPbkContactFindView::CNotReadyState::CNotReadyState
        (CPbkContactFindView& aParent)
    : CStateBase(aParent) 
    {
    }

CPbkContactFindView::CNotReadyState* CPbkContactFindView::CNotReadyState::NewL
        (CPbkContactFindView& aParent)
    {
    return (new(ELeave) CNotReadyState(aParent));
    }

inline void CPbkContactFindView::CNotReadyState::SwitchToReadyStateL()
    {
    // CReadyState object takes ownership of this
    iParent.iCurrentState = CReadyState::NewL(iParent,this);
    iParent.iState = EReady;
    }

TBool CPbkContactFindView::CNotReadyState::SetFindTextL
        (const TDesC& aFindText, const CContactIdArray* /*aIncludeAlways*/)
    {
    if (aFindText.Length() > 0)
        {
        Panic(ECntPanicViewNotReady);
        }
    return EFalse;
    }

TBool CPbkContactFindView::CNotReadyState::DoHandleContactViewEventL
        (const CContactViewBase& /*aView*/, TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
		case TContactViewEvent::EReady: //FALLTHROUGH
		case TContactViewEvent::ESortOrderChanged:
            {
            SwitchToReadyStateL();
            // Forward event
            return ETrue;
            }

        case TContactViewEvent::EUnavailable:   //FALLTHROUGH
        case TContactViewEvent::EServerError:   //FALLTHROUGH
        case TContactViewEvent::ESortError:     //FALLTHROUGH
        case TContactViewEvent::EIndexingError:
            {
            // Forward event
            return ETrue;
            }

        default:
            {
            // Don't forward other events (actually there shouldn't be any 
            // other events, but with Contact Model, you never know;)
            return EFalse;
            }
        }
    }


// CReadyState

inline CPbkContactFindView::CReadyState::CReadyState
        (CPbkContactFindView& aParent)
    : CStateBase(aParent)
    {
    }

CPbkContactFindView::CReadyState* CPbkContactFindView::CReadyState::NewL
        (CPbkContactFindView& aParent, CNotReadyState* aNotReadyState)
    {
    CReadyState* self = new(ELeave) CReadyState(aParent);
    // Take ownership of aNotReadyState
    self->iNotReadyState = aNotReadyState;
    return self;
    }

CPbkContactFindView::CReadyState::~CReadyState()
    {
    delete iNotReadyState;
    }

inline void CPbkContactFindView::CReadyState::SwitchToNotReadyStateD()
    {
    // Set parent's state
    iParent.iCurrentState = iNotReadyState;
    iParent.iState = ENotReady;
    // Relinquish ownership of iNotReadyState
    iNotReadyState = NULL;
    delete this;
    }

inline TBool CPbkContactFindView::CReadyState::SwitchToFindStateL
        (const TDesC& aFindText, const CContactIdArray* aIncludeAlways)
    {
    // CFindState object takes ownership of this
    iParent.iCurrentState = CFindState::NewL(iParent,this);
    // SetFindTextL() will restore this to iParent.iCurrentState if it fails
    return iParent.iCurrentState->SetFindTextL(aFindText,aIncludeAlways);
    }

TBool CPbkContactFindView::CReadyState::SetFindTextL
        (const TDesC& aFindText, const CContactIdArray* aIncludeAlways)
    {
    if (aFindText.Length() > 0)
        {
        return SwitchToFindStateL(aFindText,aIncludeAlways);
        }
    else
        {
        return EFalse;
        }
    }

TContactItemId CPbkContactFindView::CReadyState::AtL(TInt aIndex) const
    {
    return BaseView().AtL(aIndex);
    }

const CViewContact& CPbkContactFindView::CReadyState::ContactAtL
        (TInt aIndex) const
    {
    return BaseView().ContactAtL(aIndex);
    }

TInt CPbkContactFindView::CReadyState::CountL() const
    {
    return BaseView().CountL();
    }

TInt CPbkContactFindView::CReadyState::FindL(TContactItemId aId) const
    {
    return BaseView().FindL(aId);
    }

HBufC* CPbkContactFindView::CReadyState::AllFieldsLC
        (TInt aIndex,const TDesC& aSeparator) const
    {
    return BaseView().AllFieldsLC(aIndex,aSeparator);
    }

TBool CPbkContactFindView::CReadyState::DoHandleContactViewEventL
        (const CContactViewBase& /*aView*/, TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EUnavailable:   // FALLTHROUGH
		case TContactViewEvent::ESortError:     // FALLTHROUGH
		case TContactViewEvent::EServerError:   // FALLTHROUGH
		case TContactViewEvent::EIndexingError:
            {
            SwitchToNotReadyStateD();
            break;
            }

        default:
            break;
        }
    // Forward all events to parent's observers
    return ETrue;
    }


// CFindState
inline CPbkContactFindView::CFindState::CFindState
        (CPbkContactFindView& aParent)
    : CStateBase(aParent)
    {
    }

inline void CPbkContactFindView::CFindState::ConstructL
        (CReadyState* aReadyState)
    {
    iViewSortOrder.CopyL(iParent.iBaseView.SortOrderL());
    // Take ownership of aReadyState
    iReadyState = aReadyState;
    }

CPbkContactFindView::CFindState* CPbkContactFindView::CFindState::NewL
        (CPbkContactFindView& aParent, 
        CReadyState* aReadyState)
    {
    CFindState* self = new(ELeave) CFindState(aParent);
    CleanupStack::PushL(self);
    self->ConstructL(aReadyState);
    CleanupStack::Pop(self);
    return self;
    }

/**
 * Returns ETrue if aContact matches aFindText.
 */ 
TBool CPbkContactFindView::CFindState::IsFindMatchL
        (const CViewContact& aContact, const TDesC& aFindText) const
    {
    // Delegate matching to find primitives object
    return iParent.iFindPrimitives->IsFindMatchL(aContact, aFindText);
    }

/**
 * Returns ETrue if aContact matches the current find text.
 */
inline TBool CPbkContactFindView::CFindState::IsFindMatchL
        (const CViewContact& aContact) const
    {
    return IsFindMatchL(aContact,*iFindText);
    }

/**
 * Returns ETrue if aContact matches the initial find text.
 */
inline TBool CPbkContactFindView::CFindState::MatchesInitialFindTextL
        (const CViewContact& aContact) const
    {
    return iParent.iFindPrimitives->MatchesInitialFindTextL(aContact);
    }

/**
 * Returns ETrue if aContactId is on the list of "always included" contacts.
 */
inline TBool CPbkContactFindView::CFindState::IsAlwaysIncluded
        (TContactItemId aContactId) const
    {
    return 
        (iAlwaysIncluded && iAlwaysIncluded->Find(aContactId) >= 0);
    }

/**
 * Returns ETrue if aContact belongs to this view in its current state
 * (== find text and "always included" list contents).
 */
TBool CPbkContactFindView::CFindState::IsContactIncludedL
        (const CViewContact& aContact) const
    {
    return (IsFindMatchL(aContact) || IsAlwaysIncluded(aContact.Id()));
    }

void CPbkContactFindView::CFindState::SwitchToReadyStateD()
    {
    // Set parent's state
    iParent.iCurrentState = iReadyState;
    // Relinquish ownership of iReadyState
    iReadyState = NULL;
    delete this;
    }

inline void CPbkContactFindView::CFindState::SwitchToNotReadyStateD()
    {
    iReadyState->SwitchToNotReadyStateD();
    // iReadyState is deleted
    iReadyState = NULL;
    delete this;
    }

CPbkContactFindView::CFindState::~CFindState()
    {
    DeleteAllContactData();
    delete iAlwaysIncluded;
    delete iFindText;
    delete iReadyState;
    iViewSortOrder.Close();
    }

TBool CPbkContactFindView::CFindState::SetFindTextL
        (const TDesC& aFindText, const CContactIdArray* aIncludeAlways)
    {
    const TInt newFindTextLength = aFindText.Length();
    if (newFindTextLength == 0)
        {
        return ResetFind();
        }

    TBool result = EFalse;
    delete iAlwaysIncluded;
    iAlwaysIncluded = NULL;
    if (aIncludeAlways && aIncludeAlways->Count() > 0)
        {
        iAlwaysIncluded = CContactIdArray::NewL(aIncludeAlways);
        }

    const TInt currentFindTextLen = iFindText ? iFindText->Length() : 0;
    StoreFindTextL(aFindText);
    if (currentFindTextLen > 0 &&
        iParent.iFindPrimitives->MatchesInitialFindTextL(*iFindText))
        {
        // We can just refine the initial search
        result = RefineFindL();
        }
    else
        {
        // Find text has been changed beyond initial text
        UpdateLD();
        result = ETrue;
        }

    return result;
    }

const TDesC& CPbkContactFindView::CFindState::FindText() const
    {
    if (iFindText)
        {
        return (*iFindText);
        }
    else
        {
        return KNullDesC;
        }
    }

TBool CPbkContactFindView::CFindState::ResetFind()
    {
    SwitchToReadyStateD();
    return ETrue;
    }

TInt CPbkContactFindView::CFindState::IndexOfFirstFindMatchL() const
    {
    const TInt count = iContacts.Count();
	for (TInt i=0; i<count; ++i)
		{
		if (IsFindMatchL(*iContacts[i]))
			{
			return i;
			}
		}
    return KErrNotFound;
    }

TContactItemId CPbkContactFindView::CFindState::AtL
        (TInt aIndex) const
    {
	if (aIndex >= iContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return iContacts[aIndex]->Id();
    }

const CViewContact& CPbkContactFindView::CFindState::ContactAtL
        (TInt aIndex) const
    {
	if (aIndex >= iContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
    return (*iContacts[aIndex]);
    }

TInt CPbkContactFindView::CFindState::CountL() const
    {
	return iContacts.Count();
    }

TInt CPbkContactFindView::CFindState::FindL(TContactItemId aId) const
    {
    return Find(aId,iContacts);
    }

HBufC* CPbkContactFindView::CFindState::AllFieldsLC
        (TInt aIndex,const TDesC& aSeparator) const
    {
	if (aIndex >= iContacts.Count())
		{
		//Out of Bounds.
		User::Leave(KErrNotFound);
		}
	return iParent.FieldsWithSeparatorLC(iContacts,aIndex,aSeparator);
    }

TAny* CPbkContactFindView::CFindState::CContactViewBase_Reserved_1
        (TFunction aFunction,TAny* aParams)
    {
    // Forward call to parent view because we want to do this locally
    return iParent.CContactViewBase::CContactViewBase_Reserved_1
        (aFunction, aParams);
    }

TBool CPbkContactFindView::CFindState::DoHandleContactViewEventL
        (const CContactViewBase& /*aView*/, TContactViewEvent& aEvent)
    {
    TBool forwardEvent = ETrue;
    switch (aEvent.iEventType)
        {
		case TContactViewEvent::ESortError:     // FALLTHROUGH
		case TContactViewEvent::EServerError:   // FALLTHROUGH
		case TContactViewEvent::EIndexingError:
            {
            SwitchToNotReadyStateD();
            break;
            }

        case TContactViewEvent::EUnavailable:
            {
            // Can't fall back to not ready state, because we would lose
            // find state (==this object's state) when the view becomes 
            // available again. Delete all cached data from the underlying
            // view and refresh when the view becomes ready again.
            DeleteAllContactData();
            iParent.iState = ENotReady;
            break;
            }

		case TContactViewEvent::EReady: //FALLTHROUGH
		case TContactViewEvent::ESortOrderChanged:
            {
            iParent.iState = EReady;
            UpdateLD();
            break;
            }

		case TContactViewEvent::EItemAdded:
            {
            forwardEvent = HandleItemAddedL(aEvent);
            break;
            }

		case TContactViewEvent::EItemRemoved:
            {
            forwardEvent = HandleItemRemoved(aEvent);
			break;
            }

        default:
            break;
        }
    return forwardEvent;
    }

/**
 * Deletes all underlying view's contact data cached into this view.
 */
void CPbkContactFindView::CFindState::DeleteAllContactData()
    {
    iContacts.ResetAndDestroy();
    iNonMatchingContacts.ResetAndDestroy();
    }

/**
 * Refreshes this view's contents completely by doing a full rematching
 * from the underlying view.
 * If this function leaves this object is destroyed and CReadyState is 
 * activated.
 */
void CPbkContactFindView::CFindState::UpdateLD()
    {
    DeleteAllContactData();
    // If full refresh fails fall back to ready state
    CleanupStack::PushL(TCleanupItem(SwitchToReadyStateD,this));
    MakeInitialMatchL();
    CleanupStack::Pop();  // SwitchToReadyStateD
    }

void CPbkContactFindView::CFindState::MakeInitialMatchL()
    {
    __ASSERT_DEBUG(
        iContacts.Count()==0 && iNonMatchingContacts.Count()==0 && iFindText,
        Panic(EPanicPreCond_MakeInitialMatchL));

    #ifdef PBK_BENCHMARK_FIND
    TTime time1, time2;
    time1.UniversalTime();
    #endif

    // Get contacts matching find text from base view. This will be the
    // "starting set" for further filtering the contents of this view.
    iParent.iFindPrimitives->GetInitialMatchesL
        (BaseView(), *iFindText, iContacts);

    #ifdef PBK_BENCHMARK_FIND
        {
        time2.UniversalTime();
        const TInt timeDiff = 
            I64LOW(time2.MicroSecondsFrom(time1).Int64()) / 1000;
        iReadyState->iAvgMatchTime += timeDiff;
        RDebug::Print(_L("MakeInitialMatchL took %d ms, avg=%d"), 
            timeDiff, iReadyState->iAvgMatchTime());
        }
    #endif

    RefineFindL();    
    }

void CPbkContactFindView::CFindState::SwitchToReadyStateD(TAny* aPtr)
    {
    static_cast<CFindState*>(aPtr)->SwitchToReadyStateD();
    }

/**
 * Handles item added event from iBaseView.
 */
TBool CPbkContactFindView::CFindState::HandleItemAddedL
        (TContactViewEvent& aEvent)
	{
    TBool forwardEvent = EFalse;
    const CViewContact* contact = NULL;
    if (SafeGetContactL(aEvent.iContactId,&contact) != KErrNotFound)
        {
        DeleteAllContactData(contact->Id());
        if (IsContactIncludedL(*contact))
            {
            // Add a copy of contact to this view
            TInt newIndex;
            iContacts.FindInOrder
                (contact, newIndex, 
                TLinearOrder<CViewContact>(
                    CPbkContactFindView::CompareFieldsL));
            CViewContact* newContact = CViewContact::NewL(*contact);
            CleanupStack::PushL(newContact);
            User::LeaveIfError(iContacts.Insert(newContact, newIndex));
            CleanupStack::Pop(newContact);
            aEvent.iInt = newIndex;
            forwardEvent = ETrue;
            }
        else if (MatchesInitialFindTextL(*contact))
            {
            // Append to nonmatching contacts if contact mathces the initial 
            //find string
            CViewContact* newContact = CViewContact::NewL(*contact);
            CleanupStack::PushL(newContact);
            User::LeaveIfError(iNonMatchingContacts.Append(newContact));
            CleanupStack::Pop(newContact);
            }
        }

    __ASSERT_DEBUG
        ((!contact || !IsContactIncludedL(*contact)) ||
        Find(aEvent.iContactId,iContacts)==aEvent.iInt,
        Panic(EPanicPostCond_HandleItemAddedL));
    return forwardEvent;
	}

/**
 * Handles item removed event from iBaseView.
 */
TBool CPbkContactFindView::CFindState::HandleItemRemoved
        (TContactViewEvent& aEvent)
	{
    __ASSERT_DEBUG(
        aEvent.iEventType==TContactViewEvent::EItemRemoved, 
        Panic(EPanicPreCond_HandleItemRemoved));

    TBool forwardEvent = EFalse;
    const TInt index = DeleteAllContactData(aEvent.iContactId);
    if (index != KErrNotFound)
        {
        aEvent.iInt = index;
        forwardEvent = ETrue;
        }

    if (iAlwaysIncluded)
        {
        const TInt index = iAlwaysIncluded->Find(aEvent.iContactId);
        if (index >= 0)
            {
            iAlwaysIncluded->Remove(index);
            }
        }

    __ASSERT_DEBUG(Find(aEvent.iContactId,iContacts)==KErrNotFound, 
        Panic(EPanicPostCond_HandleItemRemoved));
    __ASSERT_DEBUG(Find(aEvent.iContactId,iNonMatchingContacts)==KErrNotFound, 
        Panic(EPanicPostCond_HandleItemRemoved));
    __ASSERT_DEBUG(!IsAlwaysIncluded(aEvent.iContactId), 
        Panic(EPanicPostCond_HandleItemRemoved));
    return forwardEvent;
	}

/**
 * Deletes all cached contact data related to aContactId.
 * @return contact's index in iContacts array if present.
 */
TInt CPbkContactFindView::CFindState::DeleteAllContactData
        (TContactItemId aContactId)
    {
    const TInt matchingIndex = Find(aContactId,iContacts);
    if (matchingIndex != KErrNotFound)
        {
        delete iContacts[matchingIndex];
        iContacts.Remove(matchingIndex);
        }

    const TInt nonMatchingIndex = Find(aContactId,iNonMatchingContacts);
    if (nonMatchingIndex != KErrNotFound)
        {
        delete iNonMatchingContacts[nonMatchingIndex];
        iNonMatchingContacts.Remove(nonMatchingIndex);
        }

    return matchingIndex;
    }

/**
 * Stores aFindText into iFindText member.
 * @precond aFindText.Length()>0
 */
void CPbkContactFindView::CFindState::StoreFindTextL(const TDesC& aFindText)
    {
    const TInt KMinFindTextBufSize = 8;
    TInt findTextBufSize = 0;
    if (iFindText)
        {
        findTextBufSize = iFindText->Des().MaxLength();
        }
    if (findTextBufSize < aFindText.Length())
        {
        findTextBufSize = Max(Max(2*findTextBufSize,KMinFindTextBufSize),
            aFindText.Length());
        HBufC* newFindTextBuf = HBufC::NewL(findTextBufSize);
        delete iFindText;
        iFindText = newFindTextBuf;
        }
    *iFindText = aFindText;
    }

TBool CPbkContactFindView::CFindState::RefineFindL()
    {
    #ifdef PBK_BENCHMARK_FIND
    TTime time1, time2;
    time1.UniversalTime();
    #endif

    TBool changed = EFalse;

    // Remove contacts not matching this view's filtering criteria
    TInt i;
    for (i=iContacts.Count()-1; i>=0; --i)
        {
        CViewContact* contact = iContacts[i];
        if (!IsContactIncludedL(*contact))
            {
            if (MatchesInitialFindTextL(*contact))
                {
                // Keep contacts which match the initial find text in 
                // iNonMatchingContacts
                __ASSERT_DEBUG(Find(contact->Id(),
                    iNonMatchingContacts)==KErrNotFound,
                    Panic(EPanicLogic_RefineFindL));
                User::LeaveIfError(iNonMatchingContacts.Append(contact));
                }
            else
                {
                delete contact;
                }
            iContacts.Remove(i);
            changed = ETrue;
            }
        }

    // Go through nonmatching contacts and merge those that match now into 
    // iContacts
    for (i=iNonMatchingContacts.Count()-1; i>=0; --i)
        {
        CViewContact* contact = iNonMatchingContacts[i];
        if (IsContactIncludedL(*contact))
            {
            __ASSERT_DEBUG(Find(contact->Id(),iContacts)==KErrNotFound,
                Panic(EPanicLogic_RefineFindL));
            User::LeaveIfError(iContacts.InsertInOrderAllowRepeats
                (contact, TLinearOrder<CViewContact>(
                    CPbkContactFindView::CompareFieldsL)));
            iNonMatchingContacts.Remove(i);
            changed = ETrue;
            }
        }

    // Make sure all "always included" contacts are included. Get the contact 
    // from the underlying view if necessary.
    if (iAlwaysIncluded)
        {
        #ifdef PBK_BENCHMARK_FIND
        TTime timeAlways, timeAlways2;
        timeAlways.UniversalTime();
        TTime timeSafe, timeSafe2;
        
        TTime timeSafe3, timeSafe4;
        #endif
        const TInt count = iAlwaysIncluded->Count();
        for (TInt i=0; i<count; ++i)
            {                        
            const TContactItemId id = (*iAlwaysIncluded)[i];
            if (Find(id,iContacts) == KErrNotFound)
                {                    
                const CViewContact* contact = NULL;
                const TInt index = Find(id,iNonMatchingContacts);
                if (index != KErrNotFound)
                    {
                    #ifdef PBK_BENCHMARK_FIND                    
                    timeSafe3.UniversalTime();
                    #endif
                    contact = iNonMatchingContacts[index];
                    User::LeaveIfError(iContacts.InsertInOrderAllowRepeats
                        (contact, TLinearOrder<CViewContact>(
                            CPbkContactFindView::CompareFieldsL)));
                    iNonMatchingContacts.Remove(index);                    
                    changed = ETrue;
                    #ifdef PBK_BENCHMARK_FIND
                        {
                        timeSafe4.UniversalTime();
                        const TInt timeDiff = 
                            I64LOW(timeSafe4.MicroSecondsFrom(
                                timeSafe3).Int64()) / 1000;                                
                        iReadyState->iTotalFind_success.Inc(timeDiff);
                        }
                    #endif
            
                    }
                else if (SafeGetContactL(id,&contact) != KErrNotFound)
                    {
                    
                    #ifdef PBK_BENCHMARK_FIND                    
                    timeSafe.UniversalTime();
                    #endif
                    
                    CViewContact* copy = CViewContact::NewL(*contact);
                    CleanupStack::PushL(copy);
                    User::LeaveIfError(iContacts.InsertInOrderAllowRepeats
                        (copy, TLinearOrder<CViewContact>(
                            CPbkContactFindView::CompareFieldsL)));
                    CleanupStack::Pop(copy);
                    changed = ETrue;
                    
                    #ifdef PBK_BENCHMARK_FIND
                        {
                        timeSafe2.UniversalTime();
                        const TInt timeDiff = 
                            I64LOW(timeSafe2.MicroSecondsFrom(
                                timeSafe).Int64()) / 1000;
                        iReadyState->iTotalSafeGetContactL_result.Inc(timeDiff);
                        }
                    #endif
            
                    }
                }
            }
        #ifdef PBK_BENCHMARK_FIND
        RDebug::Print(_L(
            "CPbkContactFindView::CFindState::SafeGetContactL() FindL tot %d ms"),
            iReadyState->iTotalBaseView_FindL.Total());
        RDebug::Print(
            _L("CPbkContactFindView::CFindState::SafeGetContactL() tot %d ms"),
            iReadyState->iTotalSafeGetContactL.Total());

        RDebug::Print(_L(
            "CPbkContactFindView::CFindState::RefineFindL() Find success tot %d ms"),
            iReadyState->iTotalFind_success.Total());

        RDebug::Print(_L(
            "CPbkContactFindView::CFindState::RefineFindL() SafeGetContactL result tot %d ms"),
            iReadyState->iTotalSafeGetContactL_result.Total());
            {
            timeAlways2.UniversalTime();
            const TInt timeDiff = 
                I64LOW(timeAlways2.MicroSecondsFrom(timeAlways).Int64()) / 1000;
            iReadyState->iTotalAlwaysIncludedBranch.Inc(timeDiff);
            }
        RDebug::Print(_L(
            "CPbkContactFindView::CFindState::RefineFindL() AlwaysIncludedBranch tot %d ms"),
            iReadyState->iTotalAlwaysIncludedBranch.Total());    
        #endif
            
        }

        #ifdef PBK_BENCHMARK_FIND
            {
            time2.UniversalTime();
            const TInt timeDiff = 
                I64LOW(time2.MicroSecondsFrom(time1).Int64()) / 1000;
            iReadyState->iTotalRefineTime.Inc(timeDiff);
            RDebug::Print(
                _L("CPbkContactFindView::CFindState::RefineFindL() %d ms"),
                iReadyState->iTotalRefineTime.Total());
            }
        #endif

    return changed;
    }

/**
 * Safely return a CViewContact from underlying view using aContactId as the
 * key.
 *
 * @param aContactId    id of the contact to get.
 * @param aContact      contact details. Unmodified if this funtion returns
 *                      KErrNotFound.
 * @return index of the contact if found, KErrNotFound otherwise.
 */
TInt CPbkContactFindView::CFindState::SafeGetContactL
        (TContactItemId aContactId, const CViewContact** aContact) const
    {
    
    #ifdef PBK_BENCHMARK_FIND
    TTime timeAlwaysCase, timeAlwaysCase2;
    timeAlwaysCase.UniversalTime();
    TTime timeFind, timeFind2;
    timeFind.UniversalTime();
    #endif
        
    TInt index = BaseView().FindL(aContactId);

    #ifdef PBK_BENCHMARK_FIND
        {
        timeFind2.UniversalTime();
        const TInt timeDiff = 
            I64LOW(timeFind2.MicroSecondsFrom(timeFind).Int64()) / 1000;
        iReadyState->iTotalBaseView_FindL.Inc(timeDiff);
        }
    #endif

    while (index != KErrNotFound)
        {
        const CViewContact* contact = NULL;
        TRAPD(err, contact = &BaseView().ContactAtL(index));
        if (err == KErrNone)
            {
            if (contact->Id() == aContactId)
                {
                // Contact found
                *aContact = contact;
                break;
                }
            }
        else if (err != KErrNotFound)
            {
            User::Leave(err);
            }

        // Contact's index may have changed in the underlying view.
        // Try to get the index again
        #ifdef PBK_BENCHMARK_FIND
        timeFind.UniversalTime();
        #endif
        index = BaseView().FindL(aContactId);        
        
        #ifdef PBK_BENCHMARK_FIND
            {
            timeFind2.UniversalTime();
            const TInt timeDiff = 
                I64LOW(timeFind2.MicroSecondsFrom(timeFind).Int64()) / 1000;
            iReadyState->iTotalBaseView_FindL.Inc(timeDiff);
            }
        #endif
        
        }

    #ifdef PBK_BENCHMARK_FIND
        {    
        timeAlwaysCase2.UniversalTime();
        const TInt timeDiff = I64LOW(timeAlwaysCase2.MicroSecondsFrom(
                timeAlwaysCase).Int64()) / 1000;
        iReadyState->iTotalSafeGetContactL.Inc(timeDiff);
        }
    #endif
    
    return index;
    }


// End of File
