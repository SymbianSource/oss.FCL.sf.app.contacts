/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 names list control.
*
*/


// INCLUDE FILES
#include "CPbk2NamesListControl.h"

// Phonebook 2
#include "CPbk2NamesListStateFactory.h"
#include "CPbk2ContactViewListBox.h"
#include "CPbk2ThumbnailLoader.h"
#include "cpbk2filteredviewstack.h"
#include "MPbk2NamesListState.h"
#include "MPbk2ControlObserver.h"
#include "MPbk2UiControlEventSender.h"
#include <CPbk2ControlContainer.h>
#include <CPbk2ViewState.h>
#include <Pbk2UIControls.hrh>
#include <MPbk2Command.h>
#include <MPbk2ContactUiControlExtension.h>
#include <pbk2contactuicontroldoublelistboxextension.h>
#include <MPbk2UIExtensionFactory.h>
#include <CPbk2UIExtensionManager.h>
#include "MPbk2FilteredViewStack.h"
#include "cpbk2predictiveviewstack.h"
#include <Pbk2Commands.hrh>
#include <Pbk2InternalUID.h>
#include "CPbk2PredictiveSearchFilter.h"
#include "cpbk2contactviewdoublelistbox.h"
#include <CPbk2ThumbnailManager.h>
#include <pbk2commonui.rsg>
#include <pbk2uicontrols.rsg>
#include "CPbk2AppUi.h"

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>

// System includes
#include <barsread.h>
#include <aknsfld.h>
#include <AknPriv.hrh>
#include <featmgr.h>
#include <AknWaitDialog.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Handles errors.
 *
 * @param aErrorCode    Error code.
 */
void HandleError( TInt aErrorCode )
    {
    if ( aErrorCode != KErrNone )
        {
        // Display standard error message
        CEikonEnv::Static()->HandleError( aErrorCode );
        }
    }

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicLogic_ComponentControl = 1,
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2NamesListControl");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace

/**
 * A helper class for sending control events.
 */
NONSHARABLE_CLASS( CPbk2UiControlEventSender ) : public CBase,
                                                public MPbk2UiControlEventSender
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aControl      Control.
         * @param aObservers    Observers.
         * @return  A new instance of this class.
         */
        static CPbk2UiControlEventSender* NewL(
                CPbk2NamesListControl& aControl,
                RPointerArray<MPbk2ControlObserver>& aObservers );

        /**
         * Destructor.
         */
       ~CPbk2UiControlEventSender();

    public: /// From MPbk2UiControlEventSender
        TBool SendEventToObserversL(
                const TPbk2ControlEvent& aEvent );
        void Reset();

    private: // Implementation
        CPbk2UiControlEventSender(
                CPbk2NamesListControl& aControl,
                RPointerArray<MPbk2ControlObserver>& aObservers );
        void ConstructL();
        TBool SendEvent(
                const TPbk2ControlEvent& aEvent );

    private: // Data
        /// Ref: Control
        CPbk2NamesListControl& iControl;
        /// Ref: Observers
        RPointerArray<MPbk2ControlObserver>& iObservers;
        /// Own: Indicates has the ready event been sent
        TBool iReadyEventSend;
    };

/**
 * A helper class for hiding inheritance of MPbk2FilteredViewStackObserver
 * from CPbk2NamesListControl.
 */
class CPbk2FilteredViewStackObserver  : public CBase,
                                                      public MPbk2FilteredViewStackObserver
    {
    public: /// Typedefs
        typedef void( CPbk2NamesListControl::* TopViewChangedFuncPtr )
            ( MVPbkContactViewBase& aOldView );
        typedef void( CPbk2NamesListControl::* ViewStackErrorFuncPtr )
            ( TInt aError );
        typedef void( CPbk2NamesListControl::* ContactAddedToBaseViewFuncPtr )
            ( MVPbkContactViewBase& aBaseView,
              TInt aIndex, const MVPbkContactLink& aContactLink );
        typedef void( CPbk2NamesListControl::* TopViewUpdatedFuncPtr )();
        typedef void( CPbk2NamesListControl::* BaseViewChangedFuncPtr )();

    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aControl                          Control.
         * @param aViewStack                        View stack.
         * @param aTopViewChangedFuncPtr            Top view changed function pointer.
         * @param aTopViewUpdatedFuncPtr            Top view updated function pointer.
         * @param aBaseViewChangedFuncPtr           Base view changed function pointer.
         * @param aViewStackErrorFuncPtr            View stack error function pointer.
         * @param aContactAddedToBaseViewFuncPtr    Contact added to base view function pointer.
         * @return  A new instance of this class.
         */
        static CPbk2FilteredViewStackObserver* NewL(
                CPbk2NamesListControl& aControl,
                TopViewChangedFuncPtr aTopViewChangedFuncPtr,
                TopViewUpdatedFuncPtr aTopViewUpdatedFuncPtr,
                BaseViewChangedFuncPtr aBaseViewChangedFuncPtr,
                ViewStackErrorFuncPtr aViewStackErrorFuncPtr,
                ContactAddedToBaseViewFuncPtr aContactAddedToBaseViewFuncPtr );

        /**
         * Destructor.
         */
       ~CPbk2FilteredViewStackObserver();

    private: // From MPbk2FilteredViewStackObserver
        void TopViewChangedL(
                MVPbkContactViewBase& aOldView );
        void TopViewUpdatedL();
        void BaseViewChangedL();
        void ViewStackError(
                    TInt aError );
        void ContactAddedToBaseView(
                    MVPbkContactViewBase& aBaseView,
                    TInt aIndex,
                    const MVPbkContactLink& aContactLink );

    private: // Implementation
        CPbk2FilteredViewStackObserver(
                CPbk2NamesListControl& aControl,
                TopViewChangedFuncPtr aTopViewChangedFuncPtr,
                TopViewUpdatedFuncPtr aTopViewUpdatedFuncPtr,
                BaseViewChangedFuncPtr aBaseViewChangedFuncPtr,
                ViewStackErrorFuncPtr aViewStackErrorFuncPtr,
                ContactAddedToBaseViewFuncPtr aContactAddedToBaseViewFuncPtr );
        void ConstructL();

    private: // Data
        /// Ref: Control
        CPbk2NamesListControl& iControl;
        /// Ref: Top view changed function pointer
        TopViewChangedFuncPtr iTopViewChangedFuncPtr;
        /// Ref: Top view updated function pointer
        TopViewUpdatedFuncPtr iTopViewUpdatedFuncPtr;
        /// Ref: Base view changed function pointer
        BaseViewChangedFuncPtr iBaseViewChangedFuncPtr;
        /// Ref: View stack error function pointer
        ViewStackErrorFuncPtr iViewStackErrorFuncPtr;
        /// Ref: Contact added to base view function pointer
        ContactAddedToBaseViewFuncPtr iContactAddedToBaseViewFuncPtr;
    };

/**
 * An observer for list box selections. Sends control events
 * when the list box enters to or from the selection mode.
 */
NONSHARABLE_CLASS( CPbk2ListBoxSelectionObserver ) :  public CBase,
                                                        private MListBoxSelectionObserver
    {
    public: // Interface

        /**
         * Creates a new instance of this class.
         *
         * @param aListBox      The list box that is observed.
         * @param aEventSender  An event sender for sending control
         *                      events.
         * @return  A new instance of this class.
         */
        static CPbk2ListBoxSelectionObserver* NewL(
                CPbk2NamesListControl& aControl,
                CEikListBox& aListBox,
                MPbk2UiControlEventSender& aEventSender );

        /**
         * Destructor
         */
        ~CPbk2ListBoxSelectionObserver();

    private: /// From MListBoxSelectionObserver
        void SelectionModeChanged(
                CEikListBox* aListBox,
                TBool aSelectionModeEnabled );

    private: // Implementation
        CPbk2ListBoxSelectionObserver(
                CPbk2NamesListControl& aControl,
                CEikListBox& aListBox,
                MPbk2UiControlEventSender& aEventSender );
        void ConstructL();

    private: // Data
        /// Ref: Control
        CPbk2NamesListControl& iControl;
        /// Ref: List box
        CEikListBox& iListBox;
        /// Ref: Event sender
        MPbk2UiControlEventSender& iEventSender;
    };


/**
 * Background Task Handler.
 */
NONSHARABLE_CLASS(CPbk2NamesListControlBgTask) : public CActive
    {

    public:
        CPbk2NamesListControlBgTask( CPbk2NamesListControl& aControl );
        ~CPbk2NamesListControlBgTask();

        void AddEvent( CPbk2NamesListControl::TPbk2NamesListBgEvents aEvent );
        void ClearAllEvents();
        void RemoveEvent( CPbk2NamesListControl::TPbk2NamesListBgEvents aEvent );

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private:
        //Owns
        RArray <CPbk2NamesListControl::TPbk2NamesListBgEvents> iEventQueue;

        //doesnt Own
        CPbk2NamesListControl& iControl;
    };

/**
 * A helper class for mass update cases.
 */
NONSHARABLE_CLASS( CPbk2HandleMassUpdate ) : public CBase
    {
    public: // Construction and destruction
        /**
         * Creates new instance of this class.
         * @return new instance of this class.
         */
        static CPbk2HandleMassUpdate* NewL(CEikListBox& iListBox);

        /**
         * Destructor.
         */
       ~CPbk2HandleMassUpdate();

    public:
        /**
         * Call this function after each update event.
         * @return  ETrue if this event is part of a mass update.
         */
       TBool MassUpdateCheckThis();

        /**
         * Call this function to check if mass update process is ongoing.
         * @return  ETrue if mass update process is ongoing.
         */
       TBool MassUpdateDetected();
       
       /**
        * Call this function to skip the showing of blocking progress note.
        * When done MassUpdateSkipProgressNote( EFalse ) must be called to reset.
        */
      void MassUpdateSkipProgressNote( TBool aSkip );       

    private:
        CPbk2HandleMassUpdate(CEikListBox& iListBox);
        void ConstructL();
        TBool HandleMassUpdateCheckL();
        void HandleMassUpdateBurstL(
            const TTimeIntervalMicroSeconds aFromFirst, 
            const TInt64 aDeltaMax64);        
        void HandleMassUpdateResetCounters();
        void HandleMassUpdateDone();
        static TInt HandleMassUpdateTimerCallBack(TAny* aAny);

    private:
        CEikListBox& iListBox;
        TBool iHandleMassUpdateDetected;
        TTime iHandleMassUpdateFirst;
        TTime iHandleMassUpdatePrev;
        TInt  iHandleMassUpdateCount;
        TBool iHandleMassUpdateSkipDialog;        
        CPeriodic* iHandleMassUpdateTimer;
        CAknWaitDialog*  iHandleMassUpdateDialog;
    };

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::CPbk2UiControlEventSender
// --------------------------------------------------------------------------
//
CPbk2UiControlEventSender::CPbk2UiControlEventSender
        ( CPbk2NamesListControl& aControl,
          RPointerArray<MPbk2ControlObserver>& aObservers ) :
            iControl( aControl ),
            iObservers( aObservers )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::NewL
// --------------------------------------------------------------------------
//
CPbk2UiControlEventSender* CPbk2UiControlEventSender::NewL(
        CPbk2NamesListControl& aControl,
        RPointerArray<MPbk2ControlObserver>& aObservers )
    {
    CPbk2UiControlEventSender* self =
        new ( ELeave ) CPbk2UiControlEventSender( aControl, aObservers );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::~CPbk2UiControlEventSender
// --------------------------------------------------------------------------
//
CPbk2UiControlEventSender::~CPbk2UiControlEventSender()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::SendEventToObserversL
// Sends control observer events in reverse order to disable the possibility
// of crash if aObserver is changed during the loop.
// --------------------------------------------------------------------------
//
TBool CPbk2UiControlEventSender::SendEventToObserversL(
        const TPbk2ControlEvent& aEvent )
    {
    // Loop backwards in case some observer destroys itself in the
    // event handler
    TBool okToSendEvent( SendEvent( aEvent ) );
    for ( TInt i = iObservers.Count() - 1; i >= 0 && okToSendEvent; --i )
        {
        iObservers[i]->HandleControlEventL( iControl, aEvent );
        }

    return okToSendEvent;
    }

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::Reset
// Resets event sender
// --------------------------------------------------------------------------
//
void CPbk2UiControlEventSender::Reset()
    {
    iReadyEventSend = EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2UiControlEventSender::SendEvent
// Makes sure that ready event is send only ones and reset the status
// if control goes unavailable
// --------------------------------------------------------------------------
//
TBool CPbk2UiControlEventSender::SendEvent
        ( const TPbk2ControlEvent& aEvent )
    {
    TBool ret( ETrue );
    if ( aEvent.iEventType == TPbk2ControlEvent::EUnavailable )
        {
        iReadyEventSend = EFalse;
        }
    else if ( aEvent.iEventType == TPbk2ControlEvent::EReady )
        {
        if ( iReadyEventSend )
            {
            ret = EFalse;
            }
        else
            {
            iReadyEventSend = ETrue;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::CPbk2FilteredViewStackObserver
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackObserver::CPbk2FilteredViewStackObserver(
        CPbk2NamesListControl& aControl,
        TopViewChangedFuncPtr aTopViewChangedFuncPtr,
        TopViewUpdatedFuncPtr aTopViewUpdatedFuncPtr,
        BaseViewChangedFuncPtr aBaseViewChangedFuncPtr,
        ViewStackErrorFuncPtr aViewStackErrorFuncPtr,
        ContactAddedToBaseViewFuncPtr aContactAddedToBaseViewFuncPtr ) :
            iControl( aControl ),
            iTopViewChangedFuncPtr( aTopViewChangedFuncPtr ),
            iTopViewUpdatedFuncPtr( aTopViewUpdatedFuncPtr ),
            iBaseViewChangedFuncPtr( aBaseViewChangedFuncPtr ),
            iViewStackErrorFuncPtr( aViewStackErrorFuncPtr ),
            iContactAddedToBaseViewFuncPtr( aContactAddedToBaseViewFuncPtr )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::NewL
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackObserver* CPbk2FilteredViewStackObserver::NewL(
        CPbk2NamesListControl& aControl,
        TopViewChangedFuncPtr aTopViewChangedFuncPtr,
        TopViewUpdatedFuncPtr aTopViewUpdatedFuncPtr,
        BaseViewChangedFuncPtr aBaseViewChangedFuncPtr,
        ViewStackErrorFuncPtr aViewStackErrorFuncPtr,
        ContactAddedToBaseViewFuncPtr aContactAddedToBaseViewFuncPtr )
    {
    CPbk2FilteredViewStackObserver* self =
        new ( ELeave ) CPbk2FilteredViewStackObserver( aControl,
            aTopViewChangedFuncPtr,
            aTopViewUpdatedFuncPtr,
            aBaseViewChangedFuncPtr,
            aViewStackErrorFuncPtr,
            aContactAddedToBaseViewFuncPtr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::~CPbk2FilteredViewStackObserver
// --------------------------------------------------------------------------
//
CPbk2FilteredViewStackObserver::~CPbk2FilteredViewStackObserver()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::TopViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::TopViewChangedL(
        MVPbkContactViewBase& aOldView )
    {
    ( iControl.*iTopViewChangedFuncPtr )( aOldView );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::TopViewUpdatedL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::TopViewUpdatedL()
    {
    ( iControl.*iTopViewUpdatedFuncPtr )();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::BaseViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::BaseViewChangedL()
    {
    ( iControl.*iBaseViewChangedFuncPtr )();
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::ViewStackError
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::ViewStackError( TInt aError )
    {
    ( iControl.*iViewStackErrorFuncPtr )( aError );
    }

// --------------------------------------------------------------------------
// CPbk2FilteredViewStackObserver::ContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CPbk2FilteredViewStackObserver::ContactAddedToBaseView(
        MVPbkContactViewBase& aBaseView, TInt aIndex,
        const MVPbkContactLink& aContactLink )
    {
    ( iControl.*iContactAddedToBaseViewFuncPtr )( aBaseView,
         aIndex, aContactLink );
    }

// --------------------------------------------------------------------------
// CPbk2ListBoxSelectionObserver::CPbk2ListBoxSelectionObserver
// --------------------------------------------------------------------------
//
CPbk2ListBoxSelectionObserver::CPbk2ListBoxSelectionObserver(
        CPbk2NamesListControl& aControl, CEikListBox& aListBox,
        MPbk2UiControlEventSender& aEventSender )
        :   iControl( aControl ),
            iListBox( aListBox ),
            iEventSender( aEventSender )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ListBoxSelectionObserver::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ListBoxSelectionObserver::ConstructL()
    {
    iListBox.AddSelectionObserverL( this );
    }

// --------------------------------------------------------------------------
// CPbk2ListBoxSelectionObserver::NewL
// --------------------------------------------------------------------------
//
CPbk2ListBoxSelectionObserver* CPbk2ListBoxSelectionObserver::NewL(
        CPbk2NamesListControl& aControl,
        CEikListBox& aListBox,
        MPbk2UiControlEventSender& aEventSender )
    {
    CPbk2ListBoxSelectionObserver* self =
        new( ELeave ) CPbk2ListBoxSelectionObserver( aControl, aListBox,
            aEventSender );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ListBoxSelectionObserver::~CPbk2ListBoxSelectionObserver
// --------------------------------------------------------------------------
//
CPbk2ListBoxSelectionObserver::~CPbk2ListBoxSelectionObserver()
    {
    iListBox.RemoveSelectionObserver( this );
    }

// --------------------------------------------------------------------------
// CPbk2ListBoxSelectionObserver::SelectionModeChanged
// --------------------------------------------------------------------------
//
void CPbk2ListBoxSelectionObserver::SelectionModeChanged(
        CEikListBox* aListBox, TBool aSelectionModeEnabled )
    {
    if ( &iListBox == aListBox )
        {
        TPbk2ControlEvent event(
            TPbk2ControlEvent::EControlEntersSelectionMode );
        if ( !aSelectionModeEnabled )
            {
            event.iEventType =
                TPbk2ControlEvent::EControlLeavesSelectionMode;
            }
        TRAPD( res, iEventSender.SendEventToObserversL( event ) );
        HandleError( res );
        if ( res != KErrNone )
            {
            iControl.Reset();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::NewL
// --------------------------------------------------------------------------
//
CPbk2HandleMassUpdate* CPbk2HandleMassUpdate::NewL(CEikListBox& aListBox)
    {
    CPbk2HandleMassUpdate* self =
        new( ELeave ) CPbk2HandleMassUpdate(aListBox);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::~CPbk2HandleMassUpdate
// --------------------------------------------------------------------------
//
CPbk2HandleMassUpdate::~CPbk2HandleMassUpdate()
    {
    delete iHandleMassUpdateDialog;
    delete iHandleMassUpdateTimer;
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::CPbk2HandleMassUpdate
// --------------------------------------------------------------------------
//
CPbk2HandleMassUpdate::CPbk2HandleMassUpdate(CEikListBox& aListBox) :
    iListBox(aListBox)
    {
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2HandleMassUpdate::ConstructL()
    {
    iHandleMassUpdateTimer = CPeriodic::NewL( CActive::EPriorityIdle );
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateCheckThis
//
// Functionality to detect mass updates done to contact database and to prevent
// e.g nameslist flickering when constant updates are happening in background
// when pc sync etc is adding hundreds of contacts in line
// --------------------------------------------------------------------------
//
TBool CPbk2HandleMassUpdate::MassUpdateCheckThis()
    {
    TBool ret(EFalse);
    const TInt KNbrUpdBeforeMassCheck(5);
    iHandleMassUpdateTimer->Cancel();

    if( iHandleMassUpdateCount > KNbrUpdBeforeMassCheck )
        {
        //candidates to be checked are they continual of a mass update
        TRAP_IGNORE(ret = HandleMassUpdateCheckL());
        }
    else if( iHandleMassUpdateCount > 0 )
        {
        //Subsequent updates that are handled normally even if
        // they would be first ones in a mass update burst
        iHandleMassUpdateCount++;
        iHandleMassUpdatePrev.UniversalTime();
        }
    else
        {
        //very first update, reset time & counter
        HandleMassUpdateResetCounters();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::MassUpdateDetected
// --------------------------------------------------------------------------
//
TBool CPbk2HandleMassUpdate::MassUpdateDetected()
    {
    return iHandleMassUpdateDetected;
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::MassUpdateSkipProgressNote
// --------------------------------------------------------------------------
//
void CPbk2HandleMassUpdate::MassUpdateSkipProgressNote( TBool aSkip )
    {
    iHandleMassUpdateSkipDialog = aSkip;
    HandleMassUpdateResetCounters();        
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateCheckL
// --------------------------------------------------------------------------
//
TBool CPbk2HandleMassUpdate::HandleMassUpdateCheckL()
    {
    //KDeltaAverage time per update to be considered as mass
    //update. One occasional update can take KDeltaMax time as long as
    //average time not exeeded (therefore timeout for the very first updates in
    //practice is also KDeltaAverage).
    const TInt64 KDeltaAverage(2000000);
    const TInt64 KDeltaMax(KDeltaAverage * 2);
    const TTimeIntervalMicroSeconds KMaxPrev(KDeltaMax);
    TBool ret(EFalse);

    TTime now;
    now.UniversalTime();
    TTimeIntervalMicroSeconds fromFirst = 
            now.MicroSecondsFrom(iHandleMassUpdateFirst);
    TTimeIntervalMicroSeconds fromPrev = 
            now.MicroSecondsFrom(iHandleMassUpdatePrev);
    TTimeIntervalMicroSeconds maxCumu(KDeltaAverage * iHandleMassUpdateCount);

    if( fromFirst < maxCumu && fromPrev < KMaxPrev )
        {
        //mass update burst ongoing
        HandleMassUpdateBurstL(fromFirst, KDeltaMax);    
        ret = ETrue;    
        }
    else if(iHandleMassUpdateDetected)
        {
        //mass update burst ended
        HandleMassUpdateDone();
        ret = ETrue;
        }
    else
        {
        //just normal update so reset counters
        HandleMassUpdateResetCounters();
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateBurstL
// --------------------------------------------------------------------------
//
void CPbk2HandleMassUpdate::HandleMassUpdateBurstL(
    const TTimeIntervalMicroSeconds aFromFirst,
    const TInt64 aDeltaMax64)
    {
    const TInt64 KMinWaitBeforeBlockUi(1500000);    
    const TTimeIntervalMicroSeconds KWait(KMinWaitBeforeBlockUi);    
    
    //mass update burst ongoing, nameslist behaviour can be altered
    iHandleMassUpdateDetected=ETrue;
    iHandleMassUpdateCount++;
    iHandleMassUpdatePrev.UniversalTime();
    iListBox.UpdateScrollBarsL();

    if( !iHandleMassUpdateDialog && 
        !iHandleMassUpdateSkipDialog && 
        aFromFirst > KWait)        
        {
        //mass update burst ongoing, ok also to block ui
        iHandleMassUpdateDialog = new(ELeave) CAknWaitDialog
            (reinterpret_cast<CEikDialog**>(&iHandleMassUpdateDialog), EFalse);
        iHandleMassUpdateDialog->SetTone(CAknNoteDialog::ENoTone);
        iHandleMassUpdateDialog->ExecuteLD(R_QTN_GEN_NOTE_SYNCHRONIZING_PROGRESS);
        //ExecuteLD above handles validity of pointer iHandleMassUpdateDialog plus
        //cleanupstack
        }
    else if( iHandleMassUpdateDialog && iHandleMassUpdateSkipDialog)
        {
        //burst ongoing but do not not block ui
        TRAP_IGNORE(iHandleMassUpdateDialog->ProcessFinishedL());
        delete iHandleMassUpdateDialog;
        iHandleMassUpdateDialog = NULL;        
        }

    TCallBack callback(HandleMassUpdateTimerCallBack, this);
    TTimeIntervalMicroSeconds32 delta32(aDeltaMax64);
    iHandleMassUpdateTimer->Start( delta32, delta32, callback );
    }

// ----------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateResetCounters
// ----------------------------------------------------------------------------
//
void CPbk2HandleMassUpdate::HandleMassUpdateResetCounters()
    {
    iHandleMassUpdateCount = 1;  //set as first candidate for next burst
    iHandleMassUpdateFirst.UniversalTime();
    iHandleMassUpdatePrev=iHandleMassUpdateFirst;
    }

// ----------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateTimerCallBack
// ----------------------------------------------------------------------------
//
TInt CPbk2HandleMassUpdate::HandleMassUpdateTimerCallBack(TAny* aAny)
    {
    CPbk2HandleMassUpdate* self = static_cast<CPbk2HandleMassUpdate*>( aAny );
    self->iHandleMassUpdateTimer->Cancel();
    self->HandleMassUpdateDone();
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPbk2HandleMassUpdate::HandleMassUpdateDone
// ----------------------------------------------------------------------------
//
void CPbk2HandleMassUpdate::HandleMassUpdateDone()
    {
    if( iHandleMassUpdateDialog )
        {
        TRAP_IGNORE(iHandleMassUpdateDialog->ProcessFinishedL());
        //The below 2 lines just in case... ProcessFinishedL already took care of these
        delete iHandleMassUpdateDialog;
        iHandleMassUpdateDialog = NULL;
        }

    iHandleMassUpdateDetected = EFalse;
    HandleMassUpdateResetCounters();
    iListBox.SetCurrentItemIndex(0);
    iListBox.SetTopItemIndex(0);

    //Update the items of listbox in NameListView
    iListBox.DrawDeferred();
    }

///////////////////////// End of helper classes /////////////////////////////


// --------------------------------------------------------------------------
// CPbk2NamesListControl::CPbk2NamesListControl
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2NamesListControl::CPbk2NamesListControl
            (const CCoeControl* aContainer,
            CVPbkContactManager& aManager,
            MVPbkContactViewBase& aView,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPbk2StorePropertyArray& aStoreProperties) :
                iContactManager( aManager ),
                iBaseView( &aView ),
                iContainer( aContainer ),
                iNameFormatter( aNameFormatter ),
                iStoreProperties( aStoreProperties ),
                iAllowPointerEvents( ETrue ),
                iOpeningCca( EFalse ),
                iMarkingModeOn( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::CPbk2NamesListControl
// --------------------------------------------------------------------------
//
CPbk2NamesListControl::CPbk2NamesListControl
            (const CCoeControl* aContainer,
            CVPbkContactManager& aManager,
            MVPbkContactViewBase& aView,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPbk2StorePropertyArray& aStoreProperties,
            CPbk2ThumbnailManager* aThumbManager) :
                iContactManager( aManager ),
                iBaseView( &aView ),
                iContainer( aContainer ),
                iNameFormatter( aNameFormatter ),
                iStoreProperties( aStoreProperties ),
                iAllowPointerEvents( ETrue ),                
                iThumbManager( aThumbManager ),
                iOpeningCca( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::~CPbk2NamesListControl
// --------------------------------------------------------------------------
//
CPbk2NamesListControl::~CPbk2NamesListControl()
    {
    ClearMarkedContactsInfo();

    if (iBgTask)
        {
        delete iBgTask;
        iBgTask = NULL;
        }

    if ( iViewStack && iStackObserver )
        {
        iViewStack->RemoveStackObserver( *iStackObserver );
        }

    if( iThumbManager )
        {
        TRAP_IGNORE( iThumbManager->SetContactViewL( NULL ) );
        iThumbManager->RemoveObserver();
        }

    if (iCommand)
        {
        // inform the command that the control is deleted
        iCommand->ResetUiControl(*this);
        }
    iObservers.Reset();
    iCommandItems.ResetAndDestroy();
    delete iCheckMassUpdate;
    delete iListBoxSelectionObserver;
    delete iStateFactory;
    delete iListBox;
    delete iFindBox;
    delete iThumbnailLoader;
    delete iEventSender;
    delete iStackObserver;
    delete iUiExtension;
    delete iViewStack;
    delete iSearchFilter;
    if( iOwnThumbManager )
        {
        delete iThumbManager;
        }
    if (iOwnBaseView)
        {
        delete iBaseView;
        }
    if(iFeatureManagerInitilized)
        {
        FeatureManager::UnInitializeLib();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2NamesListControl* CPbk2NamesListControl::NewL(
            TInt aResourceId,
            const CCoeControl* aContainer,
            CVPbkContactManager& aManager,
            MVPbkContactViewBase& aView,
            MPbk2ContactNameFormatter& aNameFormatter,
            CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2NamesListControl* self = new (ELeave) CPbk2NamesListControl(
            aContainer, aManager, aView, aNameFormatter, aStoreProperties );
    CleanupStack::PushL(self);
    self->ConstructL( aResourceId);
    CleanupStack::Pop(self);
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2NamesListControl::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2NamesListControl* CPbk2NamesListControl::NewL(
               TInt aResourceId,
               const CCoeControl* aContainer,
               CVPbkContactManager& aManager,
               MVPbkContactViewBase& aView,
               MPbk2ContactNameFormatter& aNameFormatter,
               CPbk2StorePropertyArray& aStoreProperties,
               CPbk2ThumbnailManager* aThumbManager )
    {
    CPbk2NamesListControl* self = new (ELeave) CPbk2NamesListControl(
            aContainer, aManager, aView, aNameFormatter, aStoreProperties, aThumbManager );
    CleanupStack::PushL(self);
    self->ConstructL( aResourceId);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ConstructL
//
// Note that NewL (and hence ConstructL) will not be called when custom control  
// constructed. Instead public constructor and ConstructFromResourceL are 
// called (happens when ServerApp launches custom fetch dialog)
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListControl::ConstructL(
    TInt aResourceId)
    {
    if (!iContainer)
        {
        User::Leave(KErrGeneral);
        }

    // First set container
    SetContainerWindowL(*iContainer);

    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC(resReader, aResourceId);
    ConstructFromResourceL(resReader);
    CleanupStack::PopAndDestroy(); // resReader
    FeatureManager::InitializeLibL();
    iFeatureManagerInitilized = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ConstructFromResourceL
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListControl::ConstructFromResourceL
        ( TResourceReader& aReader )
    {
    iSearchFilter = CPbk2PredictiveSearchFilter::NewL();

    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))

        {
        iViewStack = CPbk2PredictiveViewStack::NewL( *iBaseView, *iSearchFilter,
                                iNameFormatter  );
        }
    else
        {
        iViewStack = CPbk2FilteredViewStack::NewL( *iBaseView );
        }

    iViewStack->AddObserverL( *this );

    iStackObserver = CPbk2FilteredViewStackObserver::NewL
        ( *this,
          &CPbk2NamesListControl::HandleTopViewChangedL,
          &CPbk2NamesListControl::HandleTopViewUpdatedL,
          &CPbk2NamesListControl::HandleBaseViewChangedL,
          &CPbk2NamesListControl::HandleViewStackError,
          &CPbk2NamesListControl::HandleContactAddedToBaseView );
    iEventSender = CPbk2UiControlEventSender::NewL( *this, iObservers );
    iViewStack->AddStackObserverL( *iStackObserver );


    CPbk2UIExtensionManager* extManager =
        CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();

    iUiExtension = extManager->FactoryL()->CreatePbk2UiControlExtensionL
        ( iContactManager );

    CleanupStack::PopAndDestroy(); // extManager

    // peek listbox type from resource
    TInt flags = aReader.ReadInt32();
    aReader.Rewind( sizeof( TInt32 ) );

    // Create the listbox and its model
    if( flags & KPbk2ContactViewListControlDoubleRow )
        {
        if( !iThumbManager )
            {
            iThumbManager = CPbk2ThumbnailManager::NewL( iContactManager );
            iOwnThumbManager = ETrue;
            }

        iDoubleListBox = CPbk2ContactViewDoubleListBox::NewL
            ( *this, aReader, iContactManager,
            *iViewStack, iNameFormatter, iStoreProperties,
            iUiExtension, *iSearchFilter, *iThumbManager );

        iThumbManager->SetObserver( *iDoubleListBox );
        iThumbManager->SetContactViewL( iViewStack );

        iListBox = iDoubleListBox;
        }
    else
        {
        iListBox = CPbk2ContactViewListBox::NewL
            ( *this, aReader, iContactManager,
            *iViewStack, iNameFormatter, iStoreProperties,
            iUiExtension, *iSearchFilter );

        if( iThumbManager )
            {
            iThumbManager->RemoveObserver();
            }
        iDoubleListBox = NULL;
        }

    iListBox->SetObserver( this );
    iListBox->SetScrollEventObserver( this );
    iListBox->SetListBoxObserver(this);
    
    iListBox->SetMarkingModeObserver( this );
    
    iUiExtension->SetContactUiControlUpdate( this );
    // set command item updater
    TAny* ext = iUiExtension->ContactUiControlExtensionExtension(
        TUid::Uid( KPbk2ContactUiControlExtensionExtensionUID ) );
    if( ext )
        {
        static_cast<MPbk2ContactUiControlDoubleListboxExtension*>(ext)->
            SetCommandItemUpdater( this );
        }

    if ( iListBox->Flags() & KPbk2ContactViewListControlFindBox )
        {
        // Create a find box
        if ( iSearchFilter->IsPredictiveActivated() )
            {
            iFindBox = CAknSearchField::NewL
            (*this, CAknSearchField::ESearch, NULL, KSearchFieldLength);
            }
        else
            {
            iFindBox = CAknSearchField::NewL
                        (*this, CAknSearchField::EAdaptiveSearch, NULL, KSearchFieldLength);
            }

        iSearchFilter->SetAknSearchFieldL( iFindBox );

        iFindBox->MakeVisible( EFalse );
        iFindBox->SetObserver( this );
        iFindBox->AddAdaptiveSearchTextObserverL( this );
        }

    // If list box supports selection then construct an observer that
    // handles the event sending when the selection mode changes.
    if ( iListBox->IsSelectionListBox() )
        {
        iListBoxSelectionObserver = CPbk2ListBoxSelectionObserver::NewL(
            *this, *iListBox, *iEventSender );
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ConstructL find box created"));

    // Instantiate the state factory
    iStateFactory = CPbk2NamesListStateFactory::NewL( *this, *iListBox,
        *iViewStack, iFindBox, *iThumbnailLoader, *iEventSender,
        iNameFormatter, iCommandItems, *iSearchFilter );

    // Create the initial state
    iCurrentState = &iStateFactory->ActivateStartupStateL( iCurrentState );

    iBgTask = new (ELeave) CPbk2NamesListControlBgTask( *this );
    iCheckMassUpdate = CPbk2HandleMassUpdate::NewL(*iListBox); //iListbox created above    
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ConstructFromResourceL end"));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::AddObserverL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::AddObserverL
        ( MPbk2ControlObserver& aObserver )
    {
    User::LeaveIfError(iObservers.Append(&aObserver));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::RemoveObserver
        ( MPbk2ControlObserver& aObserver )
    {
    const TInt index = iObservers.Find(&aObserver);
    if (index >= 0)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::Reset
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::Reset()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2NamesListControl::Reset(0x%x) - IN"),
        this );

    // Reset find box
    TRAP_IGNORE( ResetFindL() );
    // Destroy all the filtered views from the stack
    iViewStack->Reset();
    // Refresh observer to get the view state
    iViewStack->RemoveObserver( *this );
    // Shouldn't leave because the observer was first removed.
    // The control updates its state when the asynchronous Virtual Phonebook
    // view event arrives.
    TRAP_IGNORE( iViewStack->AddObserverL( *this ) );

    const TInt firstContactIndex = 0;
    // Best effort: try to reset the focus.
    TRAP_IGNORE( iCurrentState->SetFocusedContactIndexL( firstContactIndex ) );
    //scroll listbox into beginning (ignore focus that may be below promotion items)
    iListBox->SetTopItemIndex(firstContactIndex);
    
    //cancel the marking mode
    iListBox->SetMarkingMode( EFalse );

    // Switch to the background state. This enables the next state to avtivate fully (ActivateStateL()) once phonebook
    // comes from the background.
    TRAP_IGNORE( iCurrentState = &iStateFactory->ActivateHiddenStateL( iCurrentState ); );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2NamesListControl::Reset() - OUT"));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetViewL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::SetViewL( MVPbkContactViewBase& aView )
    {
    // Control will receive a view event asynchronously from the stack
    iViewStack->SetNewBaseViewL( aView );

    if (iOwnBaseView)
        {
        delete iBaseView;
        iBaseView = NULL;
        }

    iBaseView = &aView;
    iOwnBaseView = EFalse;

    iEventSender->Reset();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::GiveViewL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::GiveViewL( MVPbkContactViewBase* aView )
    {
    SetViewL( *aView );
    iOwnBaseView = ETrue;
    }

EXPORT_C void CPbk2NamesListControl::AllowPointerEvents( TBool aAllow )
    {
    iAllowPointerEvents = aAllow;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetCurrentGroupLinkL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::SetCurrentGroupLinkL( MVPbkContactLink* aGroupLinktoSet)
    {
    // This is used only when predictive search is enabled
    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
        {
        static_cast<CPbk2PredictiveViewStack*> (iViewStack)->SetCurrentGroupLinkL(aGroupLinktoSet);
        }
    return;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::MassUpdateSkipProgressNote
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::MassUpdateSkipProgressNote( TBool aSkip )
    {
    iCheckMassUpdate->MassUpdateSkipProgressNote( aSkip ); 
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::View
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2NamesListControl::View()
    {
    return *iViewStack;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::MakeVisible
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::MakeVisible(TBool aVisible)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListControl::MakeVisible(0x%x,%d)"),
        this, aVisible);

    CCoeControl::MakeVisible(aVisible);
    iCurrentState->MakeComponentsVisible(aVisible);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ActivateL()
    {
    // First, call base class implementation
    CCoeControl::ActivateL();
    // Then do own initialization
    iCurrentState->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2NamesListControl::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
        {
        if( iCurrentState->NamesListState() == EStateReady ||
            iCurrentState->NamesListState() == EStateFiltered )
            {
            iSearchFilter->FilterL( aKeyEvent, aType, iFindBox );
            }
        }

    TKeyResponse result = iSearchFilter->OfferKeyEventL( aKeyEvent, aType );

    if ( result == EKeyWasNotConsumed )
        {
        result = iCurrentState->OfferKeyEventL( aKeyEvent, aType );
        }

    // Ignore Send Key up and down events to prevent Dialer appearance
    // on top of Phonebook application.
    if ( result == EKeyWasNotConsumed && iListBox->NumberOfItems() != 0 )
        {
        if ( ( aType == EEventKeyDown || aType == EEventKeyUp )
            && aKeyEvent.iScanCode == EStdKeyYes )
            {
            result = EKeyWasConsumed;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::CountComponentControls
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::CountComponentControls() const
    {
    return iCurrentState->CountComponentControls();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ComponentControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2NamesListControl::ComponentControl(TInt aIndex) const
    {
    // This is not forwarded to the states because these controls are
    // always existing. It does not matter what is the state.
    switch (aIndex)
        {
        case 0:
            {
            return iListBox;
            }
        case 1:
            {
            return iFindBox;
            }
        default:
            {
            // Illegal state
            __ASSERT_DEBUG( EFalse, Panic( EPanicLogic_ComponentControl ) );
            return NULL;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::FocusChanged( TDrawNow aDrawNow )
    {
    if ( iListBox )
        {
        iListBox->SetFocus( IsFocused(), aDrawNow );
        }

    if ( iFindBox && iFindBox->IsVisible() )
        {
        iFindBox->SetFocus( IsFocused(), aDrawNow );
        if ( aDrawNow == EDrawNow && IsFocused() )
            {
            iFindBox->DrawDeferred();
            }
        }

    if (iCurrentState && IsNonFocusing())
        {
        iCurrentState->CoeControl().SetFocus(IsFocused(), aDrawNow);
        }
    if ( IsFocused() )
        {
        ShowThumbnail();
        }
    else
        {
        HideThumbnail();
        }
    TRAP_IGNORE(ReportEventL( MCoeControlObserver::EEventStateChanged ));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SizeChanged()
    {
    iCurrentState->SizeChanged();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::Draw
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::Draw(const TRect& aRect) const
    {
    CWindowGc& gc = SystemGc();
    iCurrentState->Draw(aRect, gc);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if ( iFindBox != NULL && aType == KAknMessageFocusLost )
        {
        iFindBox->DrawDeferred();
        }

/*  This is useless...no need to show or hide thumbnail on layout/skin change...thumbnail will handle it's own resouce change event
    if ( aType == KEikDynamicLayoutVariantSwitch ||
         aType == KAknsMessageSkinChange )
        {
        if ( IsFocused() )
            {
            RDebug::Print( _L("### NamesList::HandleResourceChangeL - show") );
            ShowThumbnail();
            }
        else
            {
            RDebug::Print( _L("### NamesList::HandleResourceChangeL - hide") );
            HideThumbnail();
            }
        }*/
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandlePointerEventL(
    const TPointerEvent& aPointerEvent )
    {
    if( !iAllowPointerEvents )
        {
        //Passing event to listbox needed even if iCurrentState would be skipped (fix for ou1cimx1#316139)
        iListBox->HandlePointerEventL( aPointerEvent ); 
        return;
        }
    if ( AknLayoutUtils::PenEnabled() )
        {
        iCurrentState->HandlePointerEventL( aPointerEvent );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ParentControl
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPbk2NamesListControl::ParentControl() const
    {
    // Name list controls has no parent
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::NumberOfContacts
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::NumberOfContacts() const
    {
    TInt ret( iCurrentState->NumberOfContacts() );
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedContactL
// --------------------------------------------------------------------------
//
const MVPbkBaseContact* CPbk2NamesListControl::FocusedContactL() const
    {
    return iCurrentState->FocusedContactL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedViewContactL
// --------------------------------------------------------------------------
//
const MVPbkViewContact* CPbk2NamesListControl::FocusedViewContactL() const
    {
    return iCurrentState->FocusedViewContactL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedStoreContact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2NamesListControl::FocusedStoreContact() const
    {
    return iCurrentState->FocusedStoreContact();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetFocusedContactL
        ( const MVPbkBaseContact& aContact )
    {
    iCurrentState->SetFocusedContactL(aContact);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetFocusedContactL(
        const MVPbkContactBookmark& aContactBookmark )
    {
    iCurrentState->SetFocusedContactL( aContactBookmark );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetFocusedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetFocusedContactL(
        const MVPbkContactLink& aContactLink )
    {
    iCurrentState->SetFocusedContactL( aContactLink );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedContactIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::FocusedContactIndex() const
    {
    return iCurrentState->FocusedContactIndex();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetFocusedContactIndexL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetFocusedContactIndexL( TInt aIndex )
    {
    iCurrentState->SetFocusedContactIndexL( aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::NumberOfContactFields
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::NumberOfContactFields() const
    {
    return iCurrentState->NumberOfContactFields();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedField
// --------------------------------------------------------------------------
//
const MVPbkBaseContactField* CPbk2NamesListControl::FocusedField() const
    {
    return iCurrentState->FocusedField();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::FocusedFieldIndex() const
    {
    return iCurrentState->FocusedFieldIndex();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetFocusedFieldIndex
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetFocusedFieldIndex( TInt /*aIndex*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactsMarked
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListControl::ContactsMarked() const
    {
    return iCurrentState->ContactsMarked();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SelectedContactsL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListControl::SelectedContactsL() const
    {
    return iCurrentState->SelectedContactsL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SelectedContactsOrFocusedContactL
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray*
        CPbk2NamesListControl::SelectedContactsOrFocusedContactL() const
    {
    return iCurrentState->SelectedContactsOrFocusedContactL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SelectedContactsIteratorL
// --------------------------------------------------------------------------
//
MPbk2ContactLinkIterator*
        CPbk2NamesListControl::SelectedContactsIteratorL() const
    {
    return iCurrentState->SelectedContactsIteratorL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SelectedContactStoresL
// --------------------------------------------------------------------------
//
CArrayPtr<MVPbkContactStore>*
        CPbk2NamesListControl::SelectedContactStoresL() const
    {
    // This is not a contact store control
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ClearMarks
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ClearMarks()
    {
    iCurrentState->ClearMarks();
    ClearMarkedContactsInfo();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetSelectedContactL
        ( TInt aIndex, TBool aSelected )
    {
    iCurrentState->SetSelectedContactL( aIndex, aSelected );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetSelectedContactL(
        const MVPbkContactBookmark& aContactBookmark,
        TBool aSelected )
    {
    iCurrentState->SetSelectedContactL( aContactBookmark, aSelected );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetSelectedContactL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetSelectedContactL(
        const MVPbkContactLink& aContactLink,
        TBool aSelected )
    {
    iCurrentState->SetSelectedContactL( aContactLink, aSelected );
    }
TInt CPbk2NamesListControl::CommandItemCount() const
    {
    MPbk2ContactUiControl2* tempCurrentState =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iCurrentState->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    return tempCurrentState->CommandItemCount();
    }

const MPbk2UiControlCmdItem&
CPbk2NamesListControl::CommandItemAt( TInt aIndex ) const
    {
    MPbk2ContactUiControl2* tempCurrentState =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iCurrentState->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    return tempCurrentState->CommandItemAt( aIndex );
    }

const MPbk2UiControlCmdItem* CPbk2NamesListControl::FocusedCommandItem() const
    {
    MPbk2ContactUiControl2* tempCurrentState =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iCurrentState->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));
    return tempCurrentState->FocusedCommandItem();
    }

void CPbk2NamesListControl::DeleteCommandItemL( TInt aIndex )
    {
    iCommandItems.Remove(aIndex);
    iCurrentState->HandleCommandEventL(MPbk2NamesListState::EItemRemoved, aIndex);
    }

void CPbk2NamesListControl::AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex)
    {
    // Fix ETKI-7NADZC
    DeleteIfAlreadyAdded(aCommand);

    // add aCommand to iCommandItems
    if (aIndex > iCommandItems.Count())
        {
        aIndex = iCommandItems.Count();
        }
    iCommandItems.Insert(aCommand, aIndex);
    iCurrentState->HandleCommandEventL(MPbk2NamesListState::EItemAdded, aIndex);
    }

void CPbk2NamesListControl::DeleteIfAlreadyAdded(const MPbk2UiControlCmdItem* aCommand)
    {
    for ( TInt n = 0; n < iCommandItems.Count(); ++n)
        {
        MPbk2UiControlCmdItem* cmd = iCommandItems[n];
        if(cmd->CommandId() == aCommand->CommandId())
            {
            iCommandItems.Remove(n);
            delete cmd;
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane) const
    {
    iCurrentState->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ProcessCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ProcessCommandL( TInt aCommandId ) const
    {
    iCurrentState->ProcessCommandL(aCommandId);
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::UpdateAfterCommandExecution
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::UpdateAfterCommandExecution()
    {
    if( iCommand )
        {
        /// Reset command pointer, command has completed
        iCommand->ResetUiControl(*this);
        iCommand = NULL;
        }
    TRAP_IGNORE( ProcessCommandL( EAknUnmarkAll); );
    if( iSearchFilter )
        {
        TRAP_IGNORE( iSearchFilter->ReDrawL(ETrue); );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListControl::GetMenuFilteringFlagsL() const
    {
    return iCurrentState->GetMenuFilteringFlagsL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ControlStateL
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListControl::ControlStateL() const
    {
    CPbk2ViewState* state = CPbk2ViewState::NewLC();
    const MVPbkBaseContact* focusedContact = FocusedContactL();
    if (focusedContact)
        {
        MVPbkContactLink* link = focusedContact->CreateLinkLC();
        if (link)
            {
            CleanupStack::Pop(); // link
            state->SetFocusedContact(link);
            }
        }

    const MVPbkViewContact* topContact = iCurrentState->TopContactL();
    if ( topContact )
        {
        MVPbkContactLink* link = topContact->CreateLinkLC();
        if (link)
            {
            CleanupStack::Pop(); // link
            state->SetTopContact( link );
            }
        }

    if (ContactsMarked())
        {
        state->SetMarkedContacts(SelectedContactsL());
        }
    CleanupStack::Pop(); // state
    return state;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::RestoreControlStateL(CPbk2ViewState* aState)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::RestoreControlStateL(0x%x,0x%x)"),
        this, aState);

    if (aState)
        {
        iCurrentState->RestoreControlStateL(aState);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FindTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2NamesListControl::FindTextL()
    {
    return iCurrentState->FindTextL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ResetFindL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ResetFindL()
    {
    iCurrentState->ResetFindL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ShowThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ShowThumbnail()
    {
    iCurrentState->ShowThumbnail();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HideThumbnail
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HideThumbnail()
    {
    iCurrentState->HideThumbnail();
    }


// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetBlank
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetBlank(TBool aBlank)
    {
    MakeVisible( !aBlank );

    // The focus status of CAknSearchField* iFindBox is expected to be
    // consistent with CPbk2NamesListControl when setting blank.

    if ( iFindBox )
        {
        if ( iFindBox->IsVisible() )
            {
            iFindBox->SetFocus( IsFocused() );
            }
        }

    iCurrentState->SetBlank( aBlank );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::RegisterCommand
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::RegisterCommand(
        MPbk2Command* aCommand)
    {
    iCommand = aCommand;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SetTextL( const TDesC& aText )
    {
    iCurrentState->SetTextL( aText );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactUiControlExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListControl::ContactUiControlExtension(TUid aExtensionUid )
    {
     if( aExtensionUid == KMPbk2ContactUiControlExtension2Uid )
        {
        return static_cast<MPbk2ContactUiControl2*>( this );
        }

    return NULL;

    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ContactViewReady
        ( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ContactViewReady(0x%x)"), this );
    TRAPD( error, DoHandleContactViewReadyL( aView ) );
    HandleError( error );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ContactViewUnavailable
        ( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ContactViewUnavailable(0x%x)"), this);

    if ( &aView == iViewStack )
        {
        TRAPD( err, iCurrentState =
            &iStateFactory->ActivateNotReadyStateL( iCurrentState ) );
        HandleError( err );
        RelayoutControls();
        TRAP( err, iEventSender->SendEventToObserversL(
                TPbk2ControlEvent::EUnavailable ) );
        HandleError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ContactAddedToView(
        MVPbkContactViewBase& aView,
        TInt aIndex,
        const MVPbkContactLink& /*aContactLink*/)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ContactAddedToView()"));

    if (&aView == iViewStack)
        {
        if( !iCheckMassUpdate->MassUpdateDetected() )
            {
            TRAPD(err, DoHandleContactAdditionL(aIndex));
            HandleError(err);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ContactRemovedFromView(
        MVPbkContactViewBase& aView,
        TInt aIndex,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ContactRemovedFromView()"));

    if (&aView == iViewStack)
        {
        TRAPD(err, DoHandleContactRemovalL(aIndex));
        HandleError(err);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::ContactViewError(
        MVPbkContactViewBase& /*aView*/,
        TInt aError,
        TBool /*aErrorNotified*/)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::ContactViewError(%d)"), aError);

    // Error comes from the active view or from find view stack.
    // Actions are same in both cases. Show error note by forwarding
    // the error to CCoeEnv and try to reset control. Do reset asychronously
    // because the ContactViewError can be coming from view that is
    // destroyed when the control is reset.
    HandleError(aError);
    Reset();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::ContactViewObserverExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListControl::ContactViewObserverExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KVPbkContactViewObserverExtension2Uid )
        {
        return static_cast<MVPbkContactViewObserverExtension*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FilteredContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::FilteredContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/ )
    {    
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusedItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListControl::FocusedItemPointed()
    {
    return iCurrentState->FocusedItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::FocusableItemPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListControl::FocusableItemPointed()
    {
    return iCurrentState->FocusableItemPointed();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SearchFieldPointed
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListControl::SearchFieldPointed()
    {
    return iCurrentState->SearchFieldPointed();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleControlEventL
        ( CCoeControl* aControl, TCoeEvent aEventType )
    {
    iCurrentState->HandleControlEventL( aControl, aEventType, 0 );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::UpdateContact
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::UpdateContact
        ( const MVPbkContactLink& aContactLink )
    {
    TInt listBoxRow = KErrNotFound;

    TRAPD( err, listBoxRow = iViewStack->IndexOfLinkL( aContactLink ) );
    if ( err )
        {
        iEikonEnv->NotifyIdleErrorWhileRedrawing( err );
        }

    MPbk2ContactUiControl2* tempCurrentState =
        reinterpret_cast<MPbk2ContactUiControl2*>
            (iCurrentState->ContactUiControlExtension
                (KMPbk2ContactUiControlExtension2Uid ));

      if(tempCurrentState)
          {
        listBoxRow += tempCurrentState->CommandItemCount();
        }
    if ( listBoxRow >= 0 )
        {
        // If visible
        if ( listBoxRow >= iListBox->TopItemIndex() &&
             listBoxRow <= iListBox->BottomItemIndex() )
            {
            iListBox->RedrawItem( listBoxRow );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::UpdateCommandItem
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::UpdateCommandItem( TInt aCommandId )
    {
    const TInt count = CommandItemCount();
    const TInt top = iListBox->TopItemIndex();
    const TInt bottom = iListBox->BottomItemIndex();

    for( TInt i = 0; i < count; ++i )
        {
        if( Rng( top, i, bottom ) ) // is in visible range
            {
            if( CommandItemAt( i ).CommandId() == aCommandId )
                {
                iListBox->RedrawItem( i );
                break; // only one command with same id
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::Redraw
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::Redraw()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING(
        "CPbk2NamesListControl(0x%x)::Redraw"), this );

    // Do not use DrawNow because:
    // 1) DrawNow will draw the control everytime it's called. This can
    //    cause flickering in list box focus e.g. when the application view
    //    is restoring the control state.
    // 2) Avoid unnecessary drawing because DrawDeferred can be called many
    //    times in short interval but it will draw the control only once.
    DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::DoHandleContactViewReadyL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::DoHandleContactViewReadyL
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Change state
    SelectAndChangeReadyStateL();

    //When contact view is ready, state is activated. If there are multiple stores
    //state doesn't get event when other stores are ready. EContactViewUpdated event is
    //sent when contact view is ready, so after state activation, state can get multiple
    //contact view updated events.
    iCurrentState->HandleContactViewEventL( MPbk2NamesListState::EContactViewUpdated, 0 );

    // Check that there is still focus in list
    // Focus may be lost if for example store goes unavailable
    iListBox->CheckFocusIndex();
    // Control is ready and visible, inform observers
    TBool eventSent = iEventSender->SendEventToObserversL(
        TPbk2ControlEvent::EReady );
    // If Ready-event has been already sent, send
    // ContactSetChanged-event instead.
    if ( !eventSent )
        {
        iEventSender->SendEventToObserversL( TPbk2ControlEvent::
            EContactSetChanged );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::DoHandleContactAdditionL
//
// NOTE: See also CPbk2FilteredViewStackObserver::ContactAddedToBaseView and
//       CPbk2NamesListControl::DoContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::DoHandleContactAdditionL( TInt aIndex )
    {
    //Prevent messing with focus with always-on when Phonebook hidden in
    //reseted state. Otherwise background-added contacts will change focus.
    //This would cause Phonebook not to look like freshly started when opened again.
    //And also make sure foreground-added contact will show the new contact
    //even though the focus is not seen.
    if( (static_cast<CPbk2AppUi*>(
            CEikonEnv::Static()->AppUi()))->IsForeground() )
        {
        SelectAndChangeReadyStateL();
        }

    if( !iCheckMassUpdate->MassUpdateDetected() )
        {
        iCurrentState->HandleContactViewEventL
            ( MPbk2NamesListState::EItemAdded, aIndex );
        }

    // At least names list view needs control event about contact addition,
    // so it knows to update CBAs
    iEventSender->SendEventToObserversL( TPbk2ControlEvent::EItemAdded );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::DoHandleContactRemovalL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::DoHandleContactRemovalL( TInt aIndex )
    {
    iCurrentState->HandleContactViewEventL
        ( MPbk2NamesListState::EItemRemoved, aIndex );
    SelectAndChangeReadyStateL();
    iEventSender->SendEventToObserversL( TPbk2ControlEvent::EItemRemoved );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::SelectAndChangeReadyStateL
// Changes between ready states (ready, empty, filtered).
// Can't handle a change to unavailable state
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::SelectAndChangeReadyStateL()
    {
    if ( iViewStack->ContactCountL() == 0 )
        {
        if ( iViewStack->Level() > 0 )
            {
            // If view was filtered beyond matches, activate filtered state
            iCurrentState = &iStateFactory->ActivateFilteredStateL
                ( iCurrentState );
            }
        else
            {
            // Empty state used only if there are no contacts
            // in the base view
            iCurrentState = &iStateFactory->ActivateEmptyStateL
                ( iCurrentState );
            }
        }
    else
        {
        iCurrentState =
            &iStateFactory->ActivateReadyStateL( iCurrentState );
        }

    // Controls have to be relayouted always. Relayouting only when
    // state was changed is not enough, it results to drawing problems.
    // Calling only Redraw() is not enough also.
    RelayoutControls();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::RelayoutControls
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::RelayoutControls()
    {
    SizeChanged();
    MakeVisible( IsVisible() );
    Redraw();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleTopViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleTopViewChangedL
        ( MVPbkContactViewBase& aOldView )
    {
    SelectAndChangeReadyStateL();

    iCurrentState->TopViewChangedL( aOldView );

    // Inform observers
    iEventSender->SendEventToObserversL(
            TPbk2ControlEvent::EContactSetChanged );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleTopViewUpdatedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleTopViewUpdatedL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::HandleTopViewUpdatedL(0x%x)"), this );

    SelectAndChangeReadyStateL();

    iCurrentState->TopViewUpdatedL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleBaseViewChangedL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleBaseViewChangedL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::HandleBaseViewChangedL(0x%x)"), this );

    SelectAndChangeReadyStateL();

    iCurrentState->BaseViewChangedL();

    iEventSender->SendEventToObserversL(
            TPbk2ControlEvent::EContactSetChanged );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleViewStackError
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleViewStackError( TInt aError )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListControl::HandleBaseViewChangedL(0x%x, %d)"),
            this, aError );

    HandleError( aError );
    Reset();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleContactAddedToBaseView
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleContactAddedToBaseView
        ( MVPbkContactViewBase& /*aBaseView*/,
          TInt /*aIndex*/, const MVPbkContactLink& /*aContactLink*/ )
    {
    // The find box must be reset when new items are added to
    // the list box. When find text is reset, the possible filter
    // result (subset of contacts) of the previous find criteria must be
    // reset too. So, we reset both the find box and the view stack.
    TRAPD( res,
        {
        // This will reset back to base view, however retain original view
        //when opening CCA
        if( !iOpeningCca ) 
            {
            iCurrentState->ResetFindL(); 
            }
        });
    HandleError( res );

    if( !iCheckMassUpdate->MassUpdateCheckThis() )
        {
        // If it is the first time to goto a contat's cca card, then in cca we 
        // will set a defualt number to that contact by CmsSetVoiceCallDefault.
        // Then db item changed event is notified to ui layer and names list control 
        // updated accordingly. But to end user that is not allowed, so use this 
        // flag to avoid this reset of ui control.
        if( iOpeningCca )
            {
            if( iCurrentState->NamesListState() == EStateEmpty )
                {
                SelectAndChangeReadyStateL();
                }
            }
        else
            {              
            if( !iMarkingModeOn )
                {
                Reset();
                }
            }
        }

    // Do not handle contact addition here (DoHandleContactAdditionL),
    // ContactAddedToView to notification is sent separately
    }

// --------------------------------------------------------------------------
//
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
    {
//    iDidMove= EFalse ;

    switch( aEventType )
        {
        // drag events -> hide thumbnail
        // AL: this isn't very efficient way of doing this...
        // ...thumbnail loader interface should be extended to
        // enable thumbnail window to be hidden/shown without
        // destroying and reloading the bitmaps...
        case EEikScrollThumbDragHoriz:
        case EEikScrollThumbDragVert:
            {
            iDidMove = ETrue;
            HideThumbnail();
            break;
            }

        // release events
        case EEikScrollThumbReleaseHoriz:
        case EEikScrollThumbReleaseVert:
            {
            // AL: this isn't very efficient way of doing this...
            // ...thumbnail loader interface should be extended to
            // enable thumbnail window to be hidden/shown without
            // destroying and reloading the bitmaps...
            if( iDidMove )
                {
                iDidMove = EFalse;
                ShowThumbnail();
                }
            break;
            }

        default:
            {
            iDidMove= EFalse ;

            if( iThumbnailLoader )
                {
                iThumbnailLoader->Refresh();
                }


            }
        }

    // To avoid slow redraw of the scrollbar, make sure it gets drawn. Fixes MJAA-7ZGHRH.
    aScrollBar->DrawNow();
    }
// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleListBoxEventL
// Deal with the Panning/Flick event from listbox
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventPanningStarted:
        case EEventFlickStarted:
            {
            // Hide Thumbnial as the listbox is
            // Panning/Flicking
            HideThumbnail();
            break;
            }

        case EEventPanningStopped:
        case EEventFlickStopped:
            {
            // Show Thumbnial after the listbox
            // Panned/Flicked
            ShowThumbnail();
            break;
            }
        case EEventItemSingleClicked:
        case EEventItemDoubleClicked:
            {
            iEventSender->SendEventToObserversL(
                TPbk2ControlEvent::EContactDoubleTapped );
            break;
            }

        default:
            break;
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListControl::AdaptiveSearchTextChanged
//
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::AdaptiveSearchTextChanged( CAknSearchField* aSearchField )
    {
    //send event to current state
    //ETrue means that the event came from adaptvie search grid.
    TRAP_IGNORE(iCurrentState->HandleControlEventL( aSearchField, MCoeControlObserver::EEventStateChanged, ETrue));
    }


// -----------------------------------------------------------------------------
// CPbk2NamesListControl::MarkingModeStatusChanged
// -----------------------------------------------------------------------------
//
void CPbk2NamesListControl::MarkingModeStatusChanged( TBool /*aActivated*/ )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2NamesListControl::ExitMarkingMode
// Called by avkon, if the return value is ETrue, 
// the Marking mode will be canceled after any operation, 
// otherwise the Marking mode keep active.
// -----------------------------------------------------------------------------
//
TBool CPbk2NamesListControl::ExitMarkingMode() const
    {
    return ETrue;
    }
    
// --------------------------------------------------------------------------
// CPbk2NamesListControl::HandleForegroundEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListControl::HandleForegroundEventL(TBool aForeground)
    {
    if ( iSearchFilter )
        {
        iSearchFilter->HandleForegroundEventL( aForeground );
        }
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControl::StoreMarkedContactsAndResetViewL
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControl::StoreMarkedContactsAndResetViewL()
    {
    delete iSelectedLinkArray;
    iSelectedLinkArray = NULL;

    iSelectedLinkArray = iCurrentState->SelectedContactsL();
    }


// ---------------------------------------------------------------------------
// CPbk2NamesListControl::RestoreMarkedContactsL
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControl::RestoreMarkedContactsL()
    {
    //Set the Marked Contacts
    if ( iSelectedLinkArray )
        {
        for ( TInt index = 0; index<iSelectedLinkArray->Count(); index++ )
            {
            iCurrentState->SetSelectedContactL( iSelectedLinkArray->At(index), ETrue );
            }
        }

    delete iSelectedLinkArray;
    iSelectedLinkArray = NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControl::ClearMarkedContactsInfo
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControl::ClearMarkedContactsInfo()
    {
    if ( iBgTask )
        {
        iBgTask->ClearAllEvents();
        }
    delete iSelectedLinkArray;
    iSelectedLinkArray = NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControl::HandleViewForegroundEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::HandleViewForegroundEventL( TBool aForeground )
    {
    if ( aForeground )
        {
        if ( iBgTask )
            {
            iBgTask->AddEvent( CPbk2NamesListControl::EStateRestoreMarkedContacts );
            }
        }
    else
        {
        if ( iBgTask )
            {
            iBgTask->AddEvent( CPbk2NamesListControl::EStateSaveMarkedContacts );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControl::SetOpeningCca
// ---------------------------------------------------------------------------
//
EXPORT_C void CPbk2NamesListControl::SetOpeningCca( TBool aIsOpening )
    {
    iOpeningCca = aIsOpening;
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::CPbk2NamesListControlBgTask
// ---------------------------------------------------------------------------
//
CPbk2NamesListControlBgTask::CPbk2NamesListControlBgTask( CPbk2NamesListControl& aControl ) :
    CActive(CActive::EPriorityStandard),
    iControl( aControl )
    {
    CActiveScheduler::Add(this);
    iEventQueue.Append(CPbk2NamesListControl::EStateBgTaskEmpty);
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::~CPbk2NamesListControlBgTask
// ---------------------------------------------------------------------------
//
CPbk2NamesListControlBgTask::~CPbk2NamesListControlBgTask()
    {
    Cancel();
    iEventQueue.Reset();
    }


// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::DoCancel
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControlBgTask::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::RunL
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControlBgTask::RunL()
    {

    if ( iEventQueue.Count() )
        {
        switch ( iEventQueue[0] )
            {
            case CPbk2NamesListControl::EStateSaveMarkedContacts:
                iControl.StoreMarkedContactsAndResetViewL();
                iEventQueue.Remove( 0 );
                break;

            case CPbk2NamesListControl::EStateRestoreMarkedContacts:
                iControl.RestoreMarkedContactsL();
                iEventQueue.Remove( 0 );
                break;

            case CPbk2NamesListControl::EStateBgTaskEmpty :
            default:
                iControl.ClearMarkedContactsInfo();
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::RunError
// ---------------------------------------------------------------------------
//
TInt CPbk2NamesListControlBgTask::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::AddEvent
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControlBgTask::AddEvent( CPbk2NamesListControl::TPbk2NamesListBgEvents aEvent )
    {
    iEventQueue.Insert( aEvent, iEventQueue.Count() - 1 );
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }


// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::RemoveEvent
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControlBgTask::RemoveEvent( CPbk2NamesListControl::TPbk2NamesListBgEvents aEvent )
    {
    for ( TInt index = 0; index < iEventQueue.Count(); index++ )
        {
        if ( aEvent == iEventQueue[index] )
            {
            iEventQueue.Remove( index );
            if ( 0 == index )
                {
                Cancel();
                }
            break;
            }
        }
    }


// ---------------------------------------------------------------------------
// CPbk2NamesListControlBgTask::ClearAllEvents
// ---------------------------------------------------------------------------
//
void CPbk2NamesListControlBgTask::ClearAllEvents()
    {
    iEventQueue.Reset();
    iEventQueue.Append( CPbk2NamesListControl::EStateBgTaskEmpty );
    Cancel();
    }

//  End of File
