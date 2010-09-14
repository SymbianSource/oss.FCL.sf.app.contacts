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
* Description:  Phonebook 2 start-up monitor.
*
*/


#include "CPbk2StartupMonitor.h"

// Phonebook 2
#include "CPbk2AppUi.h"
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <CPbk2AppUiBase.h>
#include <Pbk2ViewId.hrh>
#include <MPbk2AppUiExtension.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2StartupObserver.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <pbk2uicontrols.rsg>
#include <pbk2commonui.rsg>
#include <CPbk2AppViewBase.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <aknview.h>

// Debugging headers
#include <Pbk2Debug.h>

const TInt KOneSecond = 1000000;
const TInt KGranularity = 4;

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicNullPointer = 1,
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2StartupMonitor" );
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} /// namespace

/**
 * Utility class that implements the wait logic and
 * process decoration if needed.
 */
class CPbk2StartupMonitor::CPbk2StartupWaiter :
        public CTimer,
        private MPbk2ProcessDecoratorObserver

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2StartupWaiter* NewL();

        /**
         * Destructor.
         */
        ~CPbk2StartupWaiter();

    public: // Interface

        /**
         * Stops the monitor.
         */
        void Stop();
        
        /**
         * Restart monitor. 
         */
        void Restart();
        
        /**
         * Disables wait note if waiter is started.
         */
        void DisableWaitNote();
        
    private: // From CTimer
        void RunL();
        TInt RunError(
                TInt aError );

    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // Implementation
        CPbk2StartupWaiter();
        void ConstructL();

    private: // Data
        /// Own: Decorator for the process
        MPbk2ProcessDecorator* iDecorator;
        /// Is decorator launched
        TBool iStarted;
        /// Own: There are exception that even if monitor is started
        /// wait note should not be displayed.
        TBool iDisplayWaitNote;
    };

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::CPbk2StartupWaiter
// EPriorityHigh because the timer must not be delayed due to other
// active objects in this thread.
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor::CPbk2StartupWaiter::CPbk2StartupWaiter() :
        CTimer( CActive::EPriorityHigh )
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::~CPbk2StartupWaiter
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor::CPbk2StartupWaiter::~CPbk2StartupWaiter()
    {
    Cancel();
    delete iDecorator;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2StartupMonitor::CPbk2StartupWaiter::ConstructL()
    {
    CTimer::ConstructL();

    // According to the Notes specification the wait note is showed if the
    // operation takes more than a second. And when it's showed it should
    // be visible at least 1,5 seconds. In our case the operation starts
    // when the first view is launched i.e. the view's DoActivateL notifies
    // the start up monitor.
    // Wait KOneSecond and then launch the wait note without delay.
    After( TTimeIntervalMicroSeconds32( KOneSecond ) );
    iDisplayWaitNote = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::NewL
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor::CPbk2StartupWaiter*
        CPbk2StartupMonitor::CPbk2StartupWaiter::NewL()
    {
    CPbk2StartupMonitor::CPbk2StartupWaiter* self =
        new ( ELeave ) CPbk2StartupMonitor::CPbk2StartupWaiter();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::Stop
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::CPbk2StartupWaiter::Stop()
    {
    Cancel();
    if ( iDecorator )
        {
        iDecorator->ProcessStopped();
        }
    iDisplayWaitNote = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::Restart
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::CPbk2StartupWaiter::Restart()
    {
    Cancel();
    After( TTimeIntervalMicroSeconds32( KOneSecond ) );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::DisableWaitNote
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::CPbk2StartupWaiter::DisableWaitNote()
    {
    iDisplayWaitNote = EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::RunL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::CPbk2StartupWaiter::RunL()
    {
    // Timer has elapsed
    if( !iDecorator )
    	{
		iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
				( R_QTN_FDN_READING_MEMORY_WAIT_NOTE, EFalse );
		iDecorator->SetObserver(*this);
    	}
    if( !iStarted && iDisplayWaitNote ) // don't start twice
    	{
		const TInt dummy = 0; // wait note doesn't care about amount
		iDecorator->ProcessStartedL(dummy);
		iStarted = ETrue;
    	}
    iDisplayWaitNote = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2StartupMonitor::CPbk2StartupWaiter::RunError( TInt aError )
    {
    CCoeEnv::Static()->HandleError(aError);
    iStarted = EFalse;
    iDisplayWaitNote = ETrue;
    if (iDecorator)
        {
        iDecorator->ProcessStopped();
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupWaiter::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::CPbk2StartupWaiter::ProcessDismissed
        ( TInt aCancelCode )
    {
    iStarted = EFalse;
    if ( aCancelCode == EAknSoftkeyCancel )
        {
        CPbk2AppUi& appUi = static_cast<CPbk2AppUi&>
            ( *CEikonEnv::Static()->EikAppUi() );

        appUi.RunAppShutter();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::CPbk2StartupMonitor
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor::CPbk2StartupMonitor
        ( MPbk2AppUiExtension& aAppUiExtension,
          CPbk2StorePropertyArray& aStoreProperties,
          CPbk2StoreConfiguration& aStoreConfiguration,
          CVPbkContactManager& aContactManager ) :
            iAppUiExtension( aAppUiExtension ),
            iStoreProperties( aStoreProperties ),
            iStoreConfiguration( aStoreConfiguration ),
            iContactManager( aContactManager ),
            iFirstViewId( TUid::Uid( EPbk2NullViewId ) )
    {
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::~CPbk2StartupMonitor
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor::~CPbk2StartupMonitor()
    {
    delete iStoreUris;
    delete iUnavailableStoreNames;
    delete iWaiter;
    delete iIdleNotifier;
    iObservers.Close();
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2StartupMonitor::ConstructL()
    {
    iIdleNotifier = CIdle::NewL( CActive::EPriorityIdle );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2StartupMonitor* CPbk2StartupMonitor::NewL
        ( MPbk2AppUiExtension& aAppUiExtension,
          CPbk2StorePropertyArray& aStoreProperties,
          CPbk2StoreConfiguration& aStoreConfiguration,
          CVPbkContactManager& aContactManager )
    {
    CPbk2StartupMonitor* self = new ( ELeave ) CPbk2StartupMonitor
        ( aAppUiExtension, aStoreProperties,
          aStoreConfiguration, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::StartupBeginsL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::StartupBeginsL()
    {
    // Well, actually start up monitor runs when first application view is
    // activated in NotifyViewActivationL but let the UI extensions start
    // their task before that.
    iIdleNotifier->Cancel();
    iAppUiExtension.ExtensionStartupL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::RestartStartupL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::RestartStartupL()
    {
    
    //Updated Functionality (EHKN-7RRBR8):
    //When PB goes to background or foreground, its not necessary to be in 
    //EPbk2NamesListViewId. It can be even in the xspview.
    //Hence only if the Active view was EPbk2NamesListViewId, we need to emulate
    //the view activation of EPbk2NamesListViewId.
    //By this way we can reduce the unnecessary note which gets shown
    //in xsp views. Leave it to xsp views what to do when this happens
    
    //Previous Functionality :
    // This function is used in "Always On" mode. When application goes
    // background it activates EPbk2NamesListViewId. When coming back to
    // foreground again we need to emulate the view activation of
    // EPbk2NamesListViewId.
    
    //Fix for TSW Err : EHKN-7RRBR8 "Checking contacts" shown when selecting search for WLAN
    CPbk2AppViewBase* activeView = Phonebook2::Pbk2AppUi()->ActiveView();
    TUid activeViewId ( TUid::Uid( EPbk2NullViewId ) );
    if ( activeView )
        {
        activeViewId = activeView->Id();
        }
    
    if ( 
            ( iFirstViewId != TUid::Uid( EPbk2NullViewId ) ) &&
            //Perform the Startup monitor only of its the PhoneBook Views
            //exclude the xsp views
            ( IsNativePhoneBookView( activeViewId ) )
        )
        {
        TBool isMonitorActive = ( iStoreUris && iStoreUris->Count() > 0 );
        if ( !isMonitorActive )
            {
            StartupBeginsL();
            // Reset the first application view id and then call
            // NotifyViewActivationL as it would be called from
            // the DoActivateL.
            iFirstViewId = TUid::Uid( EPbk2NullViewId );
            NotifyViewActivationL( TUid::Uid( EPbk2NamesListViewId ) );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleStartupComplete
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleStartupComplete()
    {
    // Core Phonebook2 is not actually interested of others because it
    // it wants that the start up is as fast as possible.
    // E.g if a UI extension is doing something heavy in the start up
    // it will not cause delay to user.
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleStartupFailed
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleStartupFailed( TInt aError )
    {
    HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::RegisterEventsL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::RegisterEventsL( MPbk2StartupObserver& aObserver )
    {
    if (iObservers.Find(&aObserver) == KErrNotFound)
        {
        iObservers.AppendL(&aObserver);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::DeregisterEvents
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::DeregisterEvents( MPbk2StartupObserver& aObserver )
    {
    TInt pos = iObservers.Find(&aObserver);
    if (pos != KErrNotFound)
        {
        iObservers.Remove(pos);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::NotifyViewActivationL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::NotifyViewActivationL( TUid aViewId )
    {
    // Default is: wait all contacts view.
    NotifyViewActivationL( aViewId,
            *Phonebook2::Pbk2AppUi()->ApplicationServices().
                ViewSupplier().AllContactsViewL() );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::NotifyViewActivationL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::NotifyViewActivationL( TUid aViewId,
        MVPbkContactViewBase& aContactView )
    {
    if ( iFirstViewId == TUid::Uid( EPbk2NullViewId ) )
        {
        // Start monitor actions only if it hasn't been notified before.
        // Only the first application view activation is important for
        // the monitor.
        iFirstViewId = aViewId;
        // Start listening to store events
        RegisterStoreEventsForViewL( aContactView );
        // Don't start anything if there are no stores to listen.
        if ( iStoreUris && iStoreUris->Count() > 0 )
            {
            // Start wait note with delay
            if( !iWaiter )
            	{
            	iWaiter = CPbk2StartupWaiter::NewL();
            	}
            else
            	{
				iWaiter->Restart();
            	}
            // Start listening to view events
            aContactView.AddObserverL( *this );
            }
        else
            {
            // There is no view/store events coming to monitor so send
            // completion event to monitor's observers.
            StartAsyncCompletionNotification();
			StopWaiter();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::StoreReady(MVPbkContactStore& aContactStore)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StartupMonitor::StoreReady(0x%x)"), this);

    TRAPD( res, HandleStoreNotificationL( aContactStore ) );
    HandleError( res );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StartupMonitor::StoreUnavailable(0x%x)"), this);

    TRAPD( res, HandleStoreUnavailableL( aContactStore ) );
    HandleError( res );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Monitor is interested only in unavailable events
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ContactViewReady( MVPbkContactViewBase& aView )
    {
    TRAPD( res, HandleContactViewReadyEventL( aView ) );
    HandleError( res );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Monitor is only interestend in  view ready event. The monitor also
    // listens to store events so it's ok to ignore view unavailable
    // because monitor will stop if non of the stores are available.
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Monitor is interested only in view ready event
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Monitor is interested only in view ready event
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Monitor is interested only in view ready event
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::AddUnavailableStoreNameL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::AddUnavailableStoreNameL(const TDesC& aName)
    {
    if (!iUnavailableStoreNames)
        {
        iUnavailableStoreNames = new(ELeave) CDesCArrayFlat(KGranularity);
        }

    TInt dummy = 0;
    if (iUnavailableStoreNames->Find(aName, dummy) != 0)
        {
        iUnavailableStoreNames->AppendL(aName);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::ShowUnavailableStoresL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::ShowUnavailableStoresL()
    {
    if (iUnavailableStoreNames)
        {
        const TInt count = iUnavailableStoreNames->MdcaCount();
        for (TInt i = 0; i < count ;++i)
            {
            // The constructor of CAknInformationNote with an argument of 
            // ETrue will create a Wait Dialog for it which finally 
            // introduces an object of CActiveSchedulerWait that may 
            // make this function reentered by another Active Object. 
            // This AO may delete iUnavailableStoreNames and make it 
            // no longer available, and if following codes isn't aware 
            // the action, any function call via the invalid pointer 
            // surely makes the application crash.
            // Always check the pointer if it's NULL before using it.
            if (iUnavailableStoreNames)
                {
                // Get the store name
                HBufC* text = StringLoader::LoadLC
                    ( R_QTN_PHOB_STORE_NOT_AVAILABLE,
                      iUnavailableStoreNames->MdcaPoint( i ) );
                CAknInformationNote* note =
                    new ( ELeave ) CAknInformationNote( ETrue );
                // Show "not available" note
                note->ExecuteLD(*text);
                CleanupStack::PopAndDestroy(text);
                }
            }
        // Destroy when used
        delete iUnavailableStoreNames;
        iUnavailableStoreNames = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleStoreUnavailableL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleStoreUnavailableL(
        MVPbkContactStore& aContactStore )
    {
    // Get the property of the failed store
    const CPbk2StoreProperty* prop = iStoreProperties.FindProperty(
            aContactStore.StoreProperties().Uri() );

    if ( prop && prop->StoreName().Length() > 0 )
        {
        // Use localised store name if found.
        AddUnavailableStoreNameL( prop->StoreName() );
        }
    else
        {
        // Use store URI if there was no name.
        AddUnavailableStoreNameL(
                aContactStore.StoreProperties().Uri().UriDes() );
        }

    HandleStoreNotificationL( aContactStore );
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleStoreNotificationL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleStoreNotificationL(
        MVPbkContactStore& aContactStore )
    {
    TVPbkContactStoreUriPtr uri = aContactStore.StoreProperties().Uri();
    DeregisterStoreEventsL( uri );
    iStoreUris->Remove( uri );
    if ( iStoreUris->Count() == 0 )
        {
        // Wait note is dismissed when all stores have send notifications.
        StopWaiter();
        // Show notes for the stores that were not available.
        ShowUnavailableStoresL();
        // Send monitor events to observers.
        StartAsyncCompletionNotification();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleContactViewReadyEventL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleContactViewReadyEventL(
        MVPbkContactViewBase& aContactView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StartupMonitor::HandleContactViewReadyEventL(0x%x)"), this);

    // We can not trust that view event ever arrives because client might
    // have deleted the view. The monitor actions are not completed because
    // of that. It's done when all stores have send notifications. This
    // kind of logic assumes that building a view is quite fast
    // after the stores are ready.
    aContactView.RemoveObserver( *this );
    // Wait note is dismissed when the view is ready. This is because the
    // composite contact view is ready when the first sub view is ready
    // and application doesn't want to wait the slowest sub view.
    StopWaiter();
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::SendMessageToObservers
// --------------------------------------------------------------------------
//
TInt CPbk2StartupMonitor::SendMessageToObservers( TAny* aSelf )
    {
    __ASSERT_DEBUG( aSelf, Panic(EPanicNullPointer));

    CPbk2StartupMonitor* self = static_cast<CPbk2StartupMonitor*>( aSelf );

    const TInt count = self->iObservers.Count();
    TRAPD( res,
        {
        for ( TInt i = 0; i < count; ++i )
            {
            self->iObservers[i]->ContactUiReadyL( *self );
            }
        });
    self->HandleError( res );

    // Don't continue
    return 0;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::HandleError
// This function is used for errors that happens in this class. If store
// or view sends an error it's not to be handle by this class.
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::HandleError( TInt aResult )
    {
    if (aResult != KErrNone)
        {
        // Stop the wait note
        StopWaiter();
        // Show default error note
        CCoeEnv::Static()->HandleError(aResult);
        // We are in the middle of start up so better to shut it down than
        // continue.
        iAvkonAppUi->RunAppShutter();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::StopWaiter
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::StopWaiter()
    {
    if (iWaiter)
        {
        iWaiter->Stop();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::RegisterStoreEventsForViewL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::RegisterStoreEventsForViewL
        ( MVPbkContactViewBase& aContactView )
    {
    // Monitor is interested only stores that are in current config
    delete iStoreUris;
    iStoreUris = NULL;
    // Get all supported URIs in Pbk2
    iStoreUris = iStoreConfiguration.SupportedStoreConfigurationL();
    // Open all stores in current config to get their state.
    const TInt count = iStoreUris->Count();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StartupMonitor::RestartStartupL(0x%x) [%d]"), this, count);

    MVPbkContactStoreList& stores = iContactManager.ContactStoresL();

    // Start listening every store that is used in aContactView and
    // is found from iContactManager. Do not load stores to contact
    // manager because it's not start-up monitor's responsibility.
    for ( TInt i = count - 1; i >= 0; --i )
        {
        TVPbkContactStoreUriPtr uriPtr( (*iStoreUris)[i] );
        MVPbkContactStore* store = stores.Find( uriPtr );
        if ( store && aContactView.MatchContactStore( uriPtr.UriDes() ) )
            {
            store->OpenL( *this );
            }
        else
            {
            iStoreUris->Remove( uriPtr );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::DeregisterStoreEventsL
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::DeregisterStoreEventsL
        ( TVPbkContactStoreUriPtr aUri )
    {
    MVPbkContactStoreList& stores = iContactManager.ContactStoresL();
    MVPbkContactStore* store = stores.Find( aUri );
    if ( store )
        {
        store->Close( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::StartAsyncCompletionNotification
// --------------------------------------------------------------------------
//
void CPbk2StartupMonitor::StartAsyncCompletionNotification()
    {
    iIdleNotifier->Cancel();
    iIdleNotifier->Start( TCallBack( &SendMessageToObservers, this ));
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::IsNativePhoneBookView
// --------------------------------------------------------------------------
//
//See Pbk2ViewId.hrh
TBool CPbk2StartupMonitor::IsNativePhoneBookView( TUid aActiveViewId )
    {
    TBool nativePhoneBookView = EFalse;
    for ( TInt index = EPbk2NullViewId; index <= EPbk2SettingsViewId; index++ )
        {
        if ( aActiveViewId == TUid::Uid( index ) )
            {
            nativePhoneBookView = ETrue;
            break;
            }
        }
    
    return nativePhoneBookView;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::Extension
// --------------------------------------------------------------------------
//
TAny* CPbk2StartupMonitor::StartupMonitorExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KPbk2StartupMonitorExtensionUid )
        {
        return static_cast<MPbk2StartupMonitorExtension*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2StartupMonitor::DisableWaitNote
// --------------------------------------------------------------------------
//
void  CPbk2StartupMonitor::DisableMonitoring()
    {
    iWaiter->DisableWaitNote();
    }

// End of File
