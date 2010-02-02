/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A stack for the base view and filtered views.
*
*/


#include "cpbk2filteredviewstack.h"

// Phonebook 2
#include "cpbk2filteredviewstackelement.h"

// Virtual Phonebook
#include <MVPbkContactViewFiltering.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactFindPolicy.h>

// Debugging headers
#include <Pbk2Debug.h>

// System includes
#include <aknsfld.h>

// CONSTANTS
const TInt KSearchLevelArrayGranularity = 2;

/**
 * A helper class for stack element ownership handling.
 */
NONSHARABLE_CLASS( CElementStack ) : public CBase
    {
    public: // Interface

        /**
         * Destructor.
         */
        ~CElementStack();

        /**
         * Returns the number of elements in the stack.
         *
         * @return  Number of elements in the stack.
         */
        TInt Count() const;

        /**
         * Returns an element in given index.
         *
         * @param aIndex    Index.
         * @return  Element at given index.
         */
        MPbk2FilteredViewStackElement& operator[](
                TInt aIndex );

        /**
         * Appends element to the stack.
         *
         * @param aElement  Element to append. Ownership is taken.
         */
        void AppendL(
                MPbk2FilteredViewStackElement* aElement );

        /**
         * Moves elements from given stack to this stack.
         *
         * @param aStack    Elements to append.
         */
        void AppendStackL(
                CElementStack& aStack );

        /**
         * Destroys all element in the stack starting from the top.
         */
        void Reset();

        /**
         * Removes an element from given position. Client must first
         * take the ownership of the element.
         *
         * @param aIndex    Index.
         */
        void Remove(
                TInt aIndex );

        /**
         * Finds the first element under destruction starting from bottom.
         */
        TInt FindFirstElementUnderDestruction();

        /**
         * Reserves memory for given number of items.
         *
         * @param aNumberOfItems    Number of items.
         */
        void ReserveL(
                TInt aNumberOfItems );

        /**
         * Used together with ReserveL. Append doesn't leave if ReserveL has
         * been called earlier.
         *
         * @param aElement  Element to append.
         */
        void Append(
                MPbk2FilteredViewStackElement* aElement );

    private: // Data
        /// Own: Stack of elements
        RPointerArray<MPbk2FilteredViewStackElement> iStack;
    };

/**
 * An internal callback class for handling asynchrnous observer
 * events.
 */
NONSHARABLE_CLASS( CPbk2FilteredViewStack::CCallback ) : public CActive
    {
    public: // Interface
        /**
         * A member function pointer definition for handling AddObserverL
         */
        typedef void(CPbk2FilteredViewStack::* AddObserverL)(
            MVPbkContactViewObserver&, CCallback& );
        /**
         * A member function pointer definition for handling error if
         * AddObserverL -function leaves.
         */
        typedef void(CPbk2FilteredViewStack::* AddObserverError)(
            TInt, CCallback& );

        /**
         * Constructor.
         *
         * @param aViewStack    The parent of this class.
         * @param aObserver     New observer that will given back to
         *                      parent class when the call back completes.
         * @param aAddObserverFuncL     Member function pointer to function
         *                              that handles adding of new observer
         * @param aAddObserverErrorFunc Member function pointer to function
         *                               that handles an error if the
         *                              aAddObserverFuncL leaves.
         */
        CCallback(
                CPbk2FilteredViewStack& aViewStack,
                MVPbkContactViewObserver& aObserver,
                AddObserverL aAddObserverFuncL,
                AddObserverError aAddObserverErrorFunc );

        /**
         * Destructor.
         */
        ~CCallback();

        /**
         * Starts the callback.
         */
        void Execute();

        /**
         * Returns the observer.
         *
         * @return  The observer that were given in construction..
         */
        MVPbkContactViewObserver& Observer();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Data
        /// Ref: View stack
        CPbk2FilteredViewStack& iViewStack;
        /// Ref: Observer
        MVPbkContactViewObserver& iObserver;
        /// Ref: Add observer function pointer
        AddObserverL iAddObserverFuncL;
        /// Ref: Add observer error function pointer
        AddObserverError iAddObserverErrorFunc;
    };

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

/**
 * Panic codes for debug build
 */
enum TPanicCode
    {
    EPreCond_ReserveL = 1,
    EPreCond_AppendStackL,
    EInvariantConstructionStackNotIntOrder,
    EInvariantViewStackNotIntOrder,
    EPostCond_Reset
    };

/**
 * A panic function for this class
 */
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2FilteredViewStack");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

// --------------------------------------------------------------------------
// MatchElement
// Matches aElement to an element in aViewStack.
//
// @param aViewStack    Stack of elements.
// @param aElement      The element that is searched for.
// @return  The matched element or NULL.
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement* MatchElement(
        CElementStack& aViewStack,
        MPbk2FilteredViewStackElement& aElement )
    {
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aViewStack[i].IsSame( aElement ) )
            {
            return &aViewStack[i];
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// FindElement
// Finds an element for the view.
//
// @param aViewStack    Stack of elements.
// @param aView         The view whose element is searched for.
// @return  The element or NULL.
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement* FindElement(
        CElementStack& aViewStack,
        MVPbkContactViewBase& aView )
    {
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aViewStack[i].View() == &aView )
            {
            return &aViewStack[i];
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// FindElement
// Finds an element for the given filter level.
//
// @param aViewStack    Stack of elements.
// @param aFilterLevel  The level of the element to search for.
// @return  The element or NULL.
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement* FindElement(
        CElementStack& aViewStack,
        TInt aFilterLevel )
    {
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( aViewStack[i].Level() == aFilterLevel )
            {
            return &aViewStack[i];
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// IsStackReady
// The stack is ready if all the elements in the stack are ready.
//
// @param aViewStack    Stack of elements.
// @return  ETrue if the stack is ready.
// --------------------------------------------------------------------------
//
TBool IsStackReady( CElementStack& aViewStack )
    {
    TBool result = ETrue;
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count && result; ++i )
        {
        if ( aViewStack[i].ViewState() !=
             MPbk2FilteredViewStackElement::EReady )
            {
            result = EFalse;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// IsStackReadyToUpdate
// The stack is ready to update if all the elements that are not under
// destruction are ready.
//
// @param aViewStack    Stack of elements.
// @return  ETrue if the stack is ready.
// --------------------------------------------------------------------------
//
TBool IsStackReadyToUpdate( CElementStack& aViewStack )
    {
    TBool result = ETrue;
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count && result; ++i )
        {
        if ( !aViewStack[i].UnderDestruction() &&
             aViewStack[i].ViewState() !=
             MPbk2FilteredViewStackElement::EReady )
            {
            result = EFalse;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// IsStackUndefined
// The stack is undefined if at least one of the elements in the
// stack is undefined.
//
// @param aViewStack    Stack of elements.
// @return  ETrue if the stack is undefined.
// --------------------------------------------------------------------------
//
TBool IsStackUndefined( CElementStack& aViewStack )
    {
    TBool result = EFalse;
    const TInt count = aViewStack.Count();
    for ( TInt i = 0; i < count && !result; ++i )
        {
        if ( aViewStack[i].ViewState() ==
             MPbk2FilteredViewStackElement::EUndefined )
            {
            result = ETrue;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// SendEventToObservers
// Sends events to the array of observes that take MVPbkContactViewBase.
// as a parameter.
//
// @param aView         The parameter for the NotifyFunc.
// @param aObservers    An array of MVPbkContactViewObservers.
// @param aNotifyFunc   A member function pointer of the
//                      MVPbkContactViewObserver.
// --------------------------------------------------------------------------
//
template <class Observer, class NotifyFunc>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<Observer>& aObservers,
        NotifyFunc aNotifyFunc )
    {
    const TInt count = aObservers.Count();
    for (TInt i = count-1; i >= 0 ; --i)
        {
        Observer* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView);
        }
    }

// --------------------------------------------------------------------------
// SendEventToObservers
// Sends events to the array of MVPbkContactViewObserver.
// Used for MVPbkContactViewObserver functions that have two extra parameters
// in addition to MVPbkContactViewBase.
//
// @param aView         The first parameter for the NotifyFunc.
// @param aObservers    An array of MVPbkContactViewObserver.
// @param aNotifyFunc   A member function pointer of the
//                      MVPbkContactViewObserver.
// @param aParam1       The second parameter for the aNotifyFunc.
// @param aParam2       The third parameter for the aNotifyFunc.
// --------------------------------------------------------------------------
//
template <class Observer, class FuncPtr, class ParamType1, class ParamType2>
void SendEventToObservers( MVPbkContactViewBase& aView,
        RPointerArray<Observer>& aObservers,
        FuncPtr aNotifyFunc,
        ParamType1 aParam1, ParamType2& aParam2)
    {
    const TInt count = aObservers.Count();
    for (TInt i = count-1; i >= 0 ; --i)
        {
        Observer* observer = aObservers[i];
        (observer->*aNotifyFunc)(aView, aParam1, aParam2);
        }
    }

} /// namespace

// --------------------------------------------------------------------------
// CElementStack::~CElementStack
// --------------------------------------------------------------------------
//
CElementStack::~CElementStack()
    {
    Reset();
    }

// --------------------------------------------------------------------------
// CElementStack::Count
// --------------------------------------------------------------------------
//
TInt CElementStack::Count() const
    {
    return iStack.Count();
    }

// --------------------------------------------------------------------------
// CElementStack::operator[]
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement& CElementStack::operator[](
        TInt aIndex )
    {
    return *iStack[aIndex];
    }

// --------------------------------------------------------------------------
// CElementStack::AppendL
// --------------------------------------------------------------------------
//
void CElementStack::AppendL( MPbk2FilteredViewStackElement* aElement )
    {
    iStack.AppendL( aElement );
    }

// --------------------------------------------------------------------------
// CElementStack::AppendStackL
// --------------------------------------------------------------------------
//
void CElementStack::AppendStackL( CElementStack& aStack )
    {
    // It's OK if this stack is empty OR
    // aStack is empty OR
    // the level of last element of this stack is 1 smaller than the first
    // element of the aStack
    __ASSERT_DEBUG( iStack.Count() == 0 ||
                    aStack.Count() == 0 ||
                    ( iStack[iStack.Count()-1]->Level() ==
                      aStack[0].Level() - 1 ),
                    Panic( EPreCond_AppendStackL ) );

    while ( aStack.Count() > 0 )
        {
        // Give ownership of the aStack[i] to iStack
        iStack.AppendL( &aStack[0] );
        aStack.Remove( 0 );
        }
    }

// --------------------------------------------------------------------------
// CElementStack::Reset
// --------------------------------------------------------------------------
//
void CElementStack::Reset()
    {
    // Stack's elements need to destroy vice versa, 
    // because views in elements are always linked to previous view.
    // Like BaseView<-FindView<-RefineView<-RefineView...
    const TInt count( iStack.Count() );
    for ( TInt i=count-1; i >= 0; --i )
        {
        MPbk2FilteredViewStackElement* element = iStack[i];
        if ( element )
            {
            iStack.Remove(i);
            delete element;
            element = NULL;
            }
        }
    iStack.Close();
    }

// --------------------------------------------------------------------------
// CElementStack::Remove
// --------------------------------------------------------------------------
//
void CElementStack::Remove( TInt aIndex )
    {
    iStack.Remove( aIndex );
    }

// --------------------------------------------------------------------------
// CElementStack::FindFirstElementUnderDestruction
// --------------------------------------------------------------------------
//
TInt CElementStack::FindFirstElementUnderDestruction()
    {
    TInt result = KErrNotFound;
    const TInt count = iStack.Count();
    for ( TInt i = 0; i < count && result == KErrNotFound; ++i )
        {
        if ( iStack[i]->UnderDestruction() )
            {
            result = i;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CElementStack::ReserveL
// --------------------------------------------------------------------------
//
void CElementStack::ReserveL( TInt aNumberOfItems )
    {
    // Function implemented for empty stacks for now
    __ASSERT_DEBUG( Count() == 0, Panic( EPreCond_ReserveL ) );

    for ( TInt i = 0; i < aNumberOfItems; ++i )
        {
        iStack.AppendL( NULL );
        }
    TInt count = Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iStack.Remove( i );
        }
    }

// --------------------------------------------------------------------------
// CElementStack::Append
// --------------------------------------------------------------------------
//
void CElementStack::Append( MPbk2FilteredViewStackElement* aElement )
    {
    /// This function is used together with ReserveL
    iStack.Append( aElement );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::CCallback
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStack::CCallback::CCallback(
        CPbk2FilteredViewStack& aViewStack,
        MVPbkContactViewObserver& aObserver, AddObserverL aAddObserverFuncL,
        AddObserverError aAddObserverErrorFunc ) : 
            CActive( EPriorityStandard ),
            iViewStack( aViewStack ),
            iObserver( aObserver ),
            iAddObserverFuncL( aAddObserverFuncL ),
            iAddObserverErrorFunc( aAddObserverErrorFunc )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::~CCallback
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStack::CCallback::~CCallback()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::Execute
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::CCallback::Execute()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::Observer
// --------------------------------------------------------------------------
//
MVPbkContactViewObserver& CPbk2FilteredViewStack::CCallback::Observer()
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::RunL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::CCallback::RunL()
    {
    (iViewStack.*iAddObserverFuncL)( iObserver, *this );
    // Member data must not be used after the call because this instance
    // will be deleted
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::CCallback::DoCancel()
    {
    // Nothing to cancel
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CCallback::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::CCallback::RunError( TInt aError )
    {
    (iViewStack.*iAddObserverErrorFunc)( aError, *this );
    // Member data must not be used after the call because this instance
    // will be deleted
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CPbk2FilteredViewStack
// --------------------------------------------------------------------------
//
inline CPbk2FilteredViewStack::CPbk2FilteredViewStack()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::~CPbk2FilteredViewStack
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStack::~CPbk2FilteredViewStack()
    {
    delete iConstructionStack;
    delete iViewStack;
    delete iFindPolicy;
    iCallbacks.ResetAndDestroy();
    iViewObservers.Close();
    iStackObservers.Close();
    if ( iBaseViewElement )
        {
        iBaseViewElement->View()->RemoveObserver( *this );
        }
    delete iBaseViewElement;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::NewL
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStack* CPbk2FilteredViewStack::NewL
        ( MVPbkContactViewBase& aBaseView )
    {
    CPbk2FilteredViewStack* self = new ( ELeave ) CPbk2FilteredViewStack;
    CleanupStack::PushL( self );
    self->ConstructL( aBaseView );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ConstructL( MVPbkContactViewBase& aBaseView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2FilteredViewStack::ConstructL"));

    // Create contact find policy
    iFindPolicy = CVPbkContactFindPolicy::NewL();

    iViewStack = new ( ELeave ) CElementStack;
    iConstructionStack = new ( ELeave ) CElementStack;

    // Create element for the base view
    iBaseViewElement = CPbk2FilteredViewStackBaseElement::NewL( aBaseView );
    // Start listening view events
    iBaseViewElement->View()->AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::UpdateFilterL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::UpdateFilterL( const MDesCArray& aFindStrings,
        const MVPbkContactBookmarkCollection* aAlwaysincluded,
        TBool aAlwaysIncludedChanged )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2FilteredViewStack::UpdateFilterL: topview(0x%x), always=%x"), 
        &TopView(), aAlwaysincluded );


    if ( aFindStrings.MdcaCount() == 0 )
        {
        Reset();
        return;
        }

    // Destroy all the views from construction stack. This is a simple
    // logic for the case that UpdateFilterL is called many times with
    // short interval.
    iConstructionStack->Reset();

    // Create or update elements according to given filter
    TBool updateStarted = EFalse;
    const TInt topLevel = CountLengthOfStrings( aFindStrings );

    // Array where to copy search words according to level
    MDesCArray* searchLevelArray = NULL;

    for ( TInt i = 1; i <= topLevel; ++i )
        {
        searchLevelArray = CopySearchWordsByLevelL( aFindStrings, i );
        CleanupStack::PushL( searchLevelArray );

        CPbk2FilteredViewStackElement* newElement =
            CPbk2FilteredViewStackElement::NewL( *iFindPolicy,
                searchLevelArray, aAlwaysincluded );
        CleanupStack::PushL( newElement );
        // Check first that is there already a same element in the stack.
        MPbk2FilteredViewStackElement* element =
            MatchElement( *iViewStack, *newElement );
        if ( !element || aAlwaysIncludedChanged )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
            "CPbk2FilteredViewStack::UpdateFilterL: no match for element"));
            // Then check that is there already an element for the filter level
            element = FindElement( *iViewStack, i );
            if ( element )
                {
                PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
                    "CPbk2FilteredViewStack::UpdateFilterL: update level %d"),
                    i );
                // There is already an element for the level -> Do only update.
                element->UpdateFilterL(
                        *iBaseViewElement->View(),
                        *this,
                        *searchLevelArray,
                        aAlwaysincluded );
                updateStarted = ETrue;
                }
            }

        if ( !element )
            {
            // A new view must be constructed
            iConstructionStack->AppendL( newElement );
            CleanupStack::Pop( newElement );
            }
        else
            {
            // No need to create a new view.
            CleanupStack::PopAndDestroy( newElement );
            }
        CleanupStack::Pop(); //searchLevelArray
        }

    // Set elements that are not part of the new filter
    // to under destruction. These elements are destroyed when the
    // stack is updated.
    for ( TInt i = topLevel; i < iViewStack->Count(); ++i )
        {
        (*iViewStack)[i].SetUnderDestruction();
        }

    // If any of the elements were not updated then we must start creating
    // the first view in construction stack. If views were updated
    // then it will send a view event and rest of the views can be constructed
    // after that.
    if ( !updateStarted )
        {
        if ( iConstructionStack->Count() > 0 )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
                "CPbk2FilteredViewStack::UpdateFilterL:\
                create new view(level=%d)"),
                (*iConstructionStack)[0].Level() );

            (*iConstructionStack)[0].CreateViewL(
                    *iBaseViewElement->View(),
                    TopView(),
                    *this );
            }
        else
            {
            // None of the views were updated and no new views were created.
            // -> update stack if there are views that must be destroyed.
            if ( IsElementsUnderDestruction() )
                {
                UpdateStackL();
                }            
            }
        }

    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::Reset
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::Reset()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2FilteredViewStack::Reset: topview(0x%x), current stack level = %d"),
        &TopView(), TopElement().Level() );

    // This can be reset without problems
    iConstructionStack->Reset();

    // iViewStack needs more careful handling because the top view can
    // change.
    MVPbkContactViewBase* oldTopView = &TopView();
    if ( oldTopView != iBaseViewElement->View() )
        {
        // The top view will be changed. We must set the base view to
        // top view but we cannot reset iViewStack before event has been
        // sent.
        // Set iViewStack to NULL while sending the event so if client
        // uses the stack the top view will be the base view.
        CElementStack* viewStack = iViewStack;
        iViewStack = NULL;
        SendTopViewChangedEvent( *oldTopView );
        // Event has been sent and we can restore the view stack and
        // reset it.
        iViewStack = viewStack;
        iViewStack->Reset();
        }

    __ASSERT_DEBUG( &TopView() == iBaseViewElement->View() &&
                    iConstructionStack &&
                    iConstructionStack->Count() == 0 &&
                    iViewStack &&
                    iViewStack->Count() == 0,
                    Panic( EPostCond_Reset ) );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::BaseView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2FilteredViewStack::BaseView() const
    {
    return *iBaseViewElement->View();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SetNewBaseViewL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::SetNewBaseViewL(
        MVPbkContactViewBase& aBaseView )
    {
    // Delete all filtered views
    Reset();
    // Remove observering the old base view
    BaseView().RemoveObserver( *this );
    // Create a new base element
    CPbk2FilteredViewStackBaseElement* newBaseElement =
        CPbk2FilteredViewStackBaseElement::NewL( aBaseView );
    delete iBaseViewElement;
    iBaseViewElement = newBaseElement;
    // Start listening to new base view
    BaseView().AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::AddStackObserverL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::AddStackObserverL(
        MPbk2FilteredViewStackObserver& aStackObserver )
    {
    iStackObservers.AppendL( &aStackObserver );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::RemoveStackObserver
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::RemoveStackObserver(
        MPbk2FilteredViewStackObserver& aStackObserver )
    {
    TInt index = iStackObservers.Find( &aStackObserver );
    if ( index != KErrNotFound )
        {
        iStackObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::Level
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::Level() const
    {
    return TopElement().Level();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CPbk2FilteredViewStack::Type() const
    {
    return TopView().Type();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ChangeSortOrderL(
        const MVPbkFieldTypeList& aSortOrder )
    {
    return TopView().ChangeSortOrderL( aSortOrder );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CPbk2FilteredViewStack::SortOrder() const
    {
    return TopView().SortOrder();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::RefreshL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::RefreshL()
    {
    return TopView().RefreshL();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactCountL
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::ContactCountL() const
    {
    return TopView().ContactCountL();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactAtL
// --------------------------------------------------------------------------
//
const MVPbkViewContact& CPbk2FilteredViewStack::ContactAtL
        ( TInt aIndex ) const
    {
    return TopView().ContactAtL( aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2FilteredViewStack::CreateLinkLC( TInt aIndex ) const
    {
    return TopView().CreateLinkLC( aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::IndexOfLinkL
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::IndexOfLinkL(
        const MVPbkContactLink& aContactLink ) const
    {
    return TopView().IndexOfLinkL( aContactLink );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::AddObserverL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::AddObserverL
        ( MVPbkContactViewObserver& aObserver )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2FilteredViewStack::AddObserverL"));

    CCallback* callBack =
        new ( ELeave ) CCallback( *this, aObserver,
            &CPbk2FilteredViewStack::DoAddObserverL,
            &CPbk2FilteredViewStack::DoAddObserverError );

    CleanupStack::PushL( callBack );
    iCallbacks.AppendL( callBack );
    CleanupStack::Pop( callBack );
    callBack->Execute();

    // Events are send in reverse order so insert to first position.
    iViewObservers.InsertL( &aObserver, 0 );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::RemoveObserver
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::RemoveObserver(
        MVPbkContactViewObserver& aObserver )
    {
    const TInt count = iCallbacks.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( &iCallbacks[i]->Observer() == &aObserver )
            {
            delete iCallbacks[i];
            iCallbacks.Remove( i );
            break;
            }
        }

    TInt index = iViewObservers.Find( &aObserver );
    if ( index != KErrNotFound )
        {
        iViewObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStack::MatchContactStore(
        const TDesC& aContactStoreUri ) const
    {
    return TopView().MatchContactStore( aContactStoreUri );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CPbk2FilteredViewStack::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const
    {
    return TopView().MatchContactStoreDomain( aContactStoreDomain );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CPbk2FilteredViewStack::CreateBookmarkLC(
        TInt aIndex ) const
    {
    return TopView().CreateBookmarkLC( aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::IndexOfBookmarkL
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::IndexOfBookmarkL(
        const MVPbkContactBookmark& aContactBookmark ) const
    {
    return TopView().IndexOfBookmarkL( aContactBookmark );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ViewFiltering
// --------------------------------------------------------------------------
//
MVPbkContactViewFiltering* CPbk2FilteredViewStack::ViewFiltering()
    {
    // The stack itself doesn't support filtering
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ContactViewReady( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::ContactViewReady(0x%x), topview(0x%x)"),
        &aView, &TopView() );

    TRAPD( error, DoHandleContactViewReadyL( aView ) );
    if ( error != KErrNone )
        {
        ContactViewError( aView, error, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ContactViewUnavailable(
        MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::ContactViewUnavailable(0x%x), topview(0x%x)"),
        &aView, &TopView() );

    // Check first if the aView is the base view.
    if ( iBaseViewElement->View() == &aView )
        {
        iBaseViewElement->SetViewState(
            MPbk2FilteredViewStackElement::EUnavailable );
        }
    else
        {
        // Check if the aView is one of the views in current stack
        MPbk2FilteredViewStackElement* element =
            FindElement( *iViewStack, aView );
        if ( !element )
            {
            element = FindElement( *iConstructionStack, aView );
            }

        if ( element )
            {
            element->SetViewState(
                MPbk2FilteredViewStackElement::EUnavailable );
            }
        }

    if ( &TopView() == &aView )
        {
        // Always forward top view events to clients
        SendEventToObservers( *this, iViewObservers,
            MVPbkContactViewObserver::ContactViewUnavailable );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ContactAddedToView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::ContactAddedToView(0x%x), index=%d,\
        topview(0x%x)"), &aView, aIndex, &TopView() );

    // If event comes from the base view forward the event using
    // MPbk2FilteredViewStackObserver.
    if ( &aView == iBaseViewElement->View() )
        {
        SendEventToObservers( *this, iStackObservers,
            MPbk2FilteredViewStackObserver::ContactAddedToBaseView, aIndex,
            aContactLink );
        }

    if ( &TopView() == &aView )
        {
        // Always forward only top view events to clients
        SendEventToObservers( *this, iViewObservers,
            MVPbkContactViewObserver::ContactAddedToView, aIndex,
            aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::ContactRemovedFromView(0x%x), index=%d,\
        topview(0x%x)"), &aView, aIndex, &TopView() );

    if ( &TopView() == &aView )
        {
        // Always forward top view events to clients
        SendEventToObservers( *this, iViewObservers,
            MVPbkContactViewObserver::ContactRemovedFromView, aIndex,
            aContactLink );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::ContactViewError
        ( MVPbkContactViewBase& aView, TInt aError, TBool aErrorNotified )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::ContactViewError(0x%x), error=%d,\
        topview(0x%x)"), &aView, aError, &TopView() );

    // Check first if the aView is the base view.
    if ( iBaseViewElement->View() == &aView )
        {
        iBaseViewElement->SetViewState(
            MPbk2FilteredViewStackElement::EError );
        }
    else
        {
        // Check if the aView is one of the views in current stack
        MPbk2FilteredViewStackElement* element =
            FindElement( *iViewStack, aView );
        if ( !element )
            {
            element = FindElement( *iConstructionStack, aView );
            }

        if ( element )
            {
            element->SetViewState( MPbk2FilteredViewStackElement::EError );
            }
        }

    // Always forward view erros to clients. The view stack is not in valid
    // state and client must reset the stack
    SendEventToObservers( *this, iViewObservers,
        MVPbkContactViewObserver::ContactViewError, aError,
        aErrorNotified );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::TopElement
// --------------------------------------------------------------------------
//
MPbk2FilteredViewStackElement& CPbk2FilteredViewStack::TopElement() const
    {
    if ( iViewStack && iViewStack->Count() > 0 )
        {
        return (*iViewStack)[iViewStack->Count() - 1];
        }
    else
        {
        return *iBaseViewElement;
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::TopView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2FilteredViewStack::TopView() const
    {
    return *TopElement().View();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::DoHandleContactViewReadyL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::DoHandleContactViewReadyL(
        MVPbkContactViewBase& aView )
    {
    // Check first if the aView is the base view.
    if ( iBaseViewElement->View() == &aView )
        {
        if ( iBaseViewElement->ViewState() == 
                MPbk2FilteredViewStackElement::EUndefined )
            {
            // The Base view is changed.
            SendBaseViewChangedEvent();            
            }  

        iBaseViewElement->SetViewState(
            MPbk2FilteredViewStackElement::EReady );
        }
    else
        {
        // Check if the aView is one of the views in current stack
        MPbk2FilteredViewStackElement* element =
            FindElement( *iViewStack, aView );
        if ( element )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2FilteredViewStack::DoHandleContactViewReadyL: view(0x%x)\
            in iViewStack, 1"), element->View() );

            element->SetViewState( MPbk2FilteredViewStackElement::EReady );
            if ( IsStackReadyToUpdate( *iViewStack ) )
                {
                // The views in iViewStack are ready -> try to find next
                // element from construction stack
                MPbk2FilteredViewStackElement* nextElement =
                    FindElement( *iConstructionStack, element->Level() + 1 );
                // Check if we need to create a view or not
                if ( nextElement && !nextElement->View() )
                    {
                    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                    ("CPbk2FilteredViewStack::DoHandleContactViewReadyL:\
                    create new view for text, 2") );
                    nextElement->CreateViewL(
                            *iBaseViewElement->View(),
                            *element->View(),
                            *this );
                    }
                else
                    {
                    // Not needed to create new view -> update stack.
                    UpdateStackL();
                    }
                }
            }
        // The view is not a base view and it's not in view stack. It must be
        // in the construction stack.
        else
            {
            element = FindElement( *iConstructionStack, aView );
            if ( element )
                {
                PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::DoHandleContactViewReadyL:\
                view(0x%x) in iConstructionStack, 3"), element->View() );

                element->SetViewState( MPbk2FilteredViewStackElement::EReady );
                if ( IsStackReady( *iConstructionStack ) )
                    {
                    // All construction views are ready. Stack can be updated.
                    UpdateStackL();
                    }
                else
                    {
                    // Construction stack is not ready. See if the next level
                    // view must be constructed.
                    MPbk2FilteredViewStackElement* nextElement = FindElement(
                        *iConstructionStack, element->Level() + 1 );
                    if ( nextElement && !nextElement->View() )
                        {
                        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                        ("CPbk2FilteredViewStack::DoHandleContactViewReadyL:\
                            create new view for text, 4") );
                        nextElement->CreateViewL(
                                *iBaseViewElement->View(),
                                *element->View(),
                                *this );
                        }
                    }
                }
            }
        }

    // Always forward top view events to clients if the the view stack is
    // ready.
    if ( &TopView() == &aView )
        {
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::DoHandleContactViewReadyL:forward\
        ContactViewReady"));

        SendEventToObservers( *this, iViewObservers,
            MVPbkContactViewObserver::ContactViewReady );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::UpdateStackL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::UpdateStackL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2FilteredViewStack::UpdateStackL:\
            CURRENT topview(0x%x)"), &TopView() );

    MVPbkContactViewBase* oldTopView = &TopView();

    // Create a temporary stack for elements that will be destroyed
    CElementStack* destructionStack = new ( ELeave ) CElementStack;
    CleanupStack::PushL( destructionStack );

    // Get the number of elements under destruction
    TInt count = iViewStack->Count();
    TInt destructionCount = 0;
    for ( TInt i = 0; i < count; ++i )
        {
        MPbk2FilteredViewStackElement& element = (*iViewStack)[i];
        if ( element.UnderDestruction() )
            {
            ++destructionCount;
            }
        }

    // If there are elements under destruction move them to destructionStack.
    if ( destructionCount > 0 )
        {
        // Reserve memory so that Append doesn't fail. It's important to
        // delete views in correct order.
        destructionStack->ReserveL( destructionCount );
        TInt index = iViewStack->FindFirstElementUnderDestruction();
        while ( index != KErrNotFound )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                        ("CPbk2FilteredViewStack::UpdateStackL:\
                        destroy view(0x%x) for text"),
            (*iViewStack)[index].View() );

            destructionStack->Append( &(*iViewStack)[index] );
            iViewStack->Remove( index );
            index = iViewStack->FindFirstElementUnderDestruction();
            }
        }

    // All under destruction views are now moved to destruction stack
    // Append construction views to iViewStack next if possible.
    if ( IsStackReady( *iConstructionStack ) )
        {
        iViewStack->AppendStackL( *iConstructionStack );
        }

    if ( &TopView() != oldTopView )
        {
        // The top view changed.
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::UpdateStackL: NEW topview(0x%x)"),
        &TopView() );

        SendTopViewChangedEvent( *oldTopView );
        }
    else
        {
        // Top view updated
        SendTopViewUpdatedEvent();
        }

    CleanupStack::PopAndDestroy( destructionStack );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::DoAddObserverL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::DoAddObserverL
        ( MVPbkContactViewObserver& aObserver, CCallback& aCallback )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2FilteredViewStack::DoAddObserverL"));

    if ( ( iViewStack->Count() == 0 &&
            iBaseViewElement->ViewState() ==
            MPbk2FilteredViewStackElement::EReady ) ||
           ( iViewStack->Count() > 0 &&  IsStackReady( *iViewStack ) ))
        {
        // The view stack is ready if
        // There is only base view and it is ready OR
        // there are also filtered views in iViewStack and they all are ready

        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::DoAddObserverL ready"));

        aObserver.ContactViewReady( *this );
        }
    // If the base view is still in EUndefined state it means that it hasn't
    // notified this instance yet.
    // If there is undefined elements in the stack it means that stack
    // is still under construction -> observer will be notified when
    // the stack becomes ready
    else if ( iBaseViewElement->ViewState() !=
              MPbk2FilteredViewStackElement::EUndefined
              && !IsStackUndefined( *iViewStack ) )
        {
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::DoAddObserverL unavailable"));

        aObserver.ContactViewUnavailable( *this );
        }

    DeleteCallback( aCallback );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::DoAddObserverError
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::DoAddObserverError
        ( TInt aError, CCallback& aCallback )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2FilteredViewStack::DoAddObserverError: aError %d"), aError );

    aCallback.Observer().ContactViewError( *this, aError, EFalse );
    DeleteCallback( aCallback );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendTopViewChangedEvent
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::SendTopViewChangedEvent
        ( MVPbkContactViewBase& aOldTopView )
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->TopViewChangedL( aOldTopView ) );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendTopViewChangedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendTopViewUpdatedEvent
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::SendTopViewUpdatedEvent()
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->TopViewUpdatedL() );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendTopViewUpdatedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::SendBaseViewChangedEvent
// --------------------------------------------------------------------------
//    
void CPbk2FilteredViewStack::SendBaseViewChangedEvent()
    {
    const TInt count = iStackObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TRAPD( res, iStackObservers[i]->BaseViewChangedL() );
        if ( res != KErrNone )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2FilteredViewStack::SendBaseViewChangedEvent:error %d"),
                res );
            iStackObservers[i]->ViewStackError( res );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::DeleteCallback
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStack::DeleteCallback( CCallback& aCallback )
    {
    TInt index = iCallbacks.Find( &aCallback );
    if ( index != KErrNotFound )
        {
        delete iCallbacks[index];
        iCallbacks.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CountLengthOfStrings
// Counts characters of array of strings
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::CountLengthOfStrings
        ( const MDesCArray& aStringArray ) const
    {
    TInt stringCount = aStringArray.MdcaCount();
    TInt totalLength = 0;
    for ( TInt i = 0; i < stringCount; ++i )
        {
        totalLength += aStringArray.MdcaPoint( i ).Length();
        }

    return totalLength;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::CopySearchWordsByLevelL
// Copies search words to new array by level
// --------------------------------------------------------------------------
//
MDesCArray* CPbk2FilteredViewStack::CopySearchWordsByLevelL
        ( const MDesCArray& aStringArray, TInt aLevel )
    {
    TInt stringNbr = 0;
    TInt stringLength = 0;
    TInt totalCharsFromBeg = 0;
    TInt charsToCopy = aLevel;
    TBool charactersLeft = ETrue;

    // Create new search level array where to copy search words
    CDesCArrayFlat* searchLevelArray = new ( ELeave ) CDesCArrayFlat(
        KSearchLevelArrayGranularity );

    while ( totalCharsFromBeg < aLevel && charactersLeft )
        {
        // Although charsToCopy may exceed the length of first string,
        // it's acceptable to use Left-function with too big value.
        searchLevelArray->AppendL(
            aStringArray.MdcaPoint( stringNbr ).Left( charsToCopy ) );
        stringLength = aStringArray.MdcaPoint( stringNbr ).Length();
        if ( charsToCopy > stringLength )
            {
            totalCharsFromBeg += stringLength;
            charsToCopy -= stringLength;
            ++stringNbr;
            }
        else
            {
            charactersLeft = EFalse;
            }
        }
    return searchLevelArray;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::IsElementsUnderDestruction
// --------------------------------------------------------------------------
//
TInt CPbk2FilteredViewStack::IsElementsUnderDestruction()
    {
    // Get the number of elements under destruction
    const TInt count = iViewStack->Count();
    TBool result( EFalse );
    for ( TInt i = 0; i < count; ++i )
        {
        MPbk2FilteredViewStackElement& element = (*iViewStack)[i];
        if ( element.UnderDestruction() )
            {
            result = ETrue;
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStack::__DbgTestInvariant
// Debug time checking. All stack elements must be in order
// --------------------------------------------------------------------------
//
#if defined(_DEBUG)
void CPbk2FilteredViewStack::__DbgTestInvariant() const
 {
    TInt filterLevel = 0;
    TInt count = iConstructionStack->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        __ASSERT_DEBUG( (*iConstructionStack)[i].Level() > filterLevel,
            Panic( EInvariantConstructionStackNotIntOrder ) );
        filterLevel = (*iConstructionStack)[i].Level();
        }

    filterLevel = 0;
    count = iViewStack->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        __ASSERT_DEBUG( (*iViewStack)[i].Level() > filterLevel,
            Panic( EInvariantViewStackNotIntOrder ) );
        filterLevel = (*iViewStack)[i].Level();
        }
 }

 #endif // _DEBUG

// End of File
