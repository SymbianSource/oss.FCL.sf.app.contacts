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
*     Implements interface for Logs reader. Common functionality of updater.
*
*/


#ifndef __Logs_Engine_CLogsBaseUpdater_H__
#define __Logs_Engine_CLogsBaseUpdater_H__

//  INCLUDES
#include <e32base.h>
#include <logwrap.h>
#include <cntdef.h>

#include <MVPbkContactStoreListObserver.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactOperationBase.h>
    #include <MPbk2StoreConfigurationObserver.h>

#include "MLogsReader.h"
#include "MLogsStateHolder.h"


// FORWARD DECLARATION
class CVPbkContactManager;    
class MVPbkContactStoreList; 
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class CVPbkContactStoreUriArray;
class CVPbkPhoneNumberMatchStrategy;
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
    class CPbk2StoreConfiguration;

// FORWARD DECLARATION
class CLogFilter;
class CLogEvent;
class CLogViewEvent;
class CLogClient;
class MLogsObserver;


// CLASS DECLARATION

/**
 *  Implements interface for Logs reader. Common functionality of updater.
 */
class CLogsBaseUpdater :    public CActive, 
                            public MLogsReader,
                            public MLogsStateHolder,
                            public MVPbkContactStoreListObserver,
                            public MVPbkContactFindObserver,
                            public MVPbkSingleContactOperationObserver,
                            public MPbk2StoreConfigurationObserver
    {

    public:
        
       /**
        *   Destructor.
        */
        virtual ~CLogsBaseUpdater();

    private:

       /**
        *  Default constructor
        */
        CLogsBaseUpdater();
       
    protected:

        /**
         *  Constructor
         *
         *  @param aFsSession   ref. to file server session
         *  @param aObserver     pointer to observer
         */
        CLogsBaseUpdater( RFs& aFsSession, 
                          MLogsObserver* aObserver ); 
   
        /**
         *  Second phase constructor
         */
        void BaseConstructL( CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags );

    protected: // From CActive

        void DoCancel();
        virtual void RunL();
		TInt RunError(TInt aError);

    private:
        virtual void StartRunningL() = 0;
        virtual void ContinueRunningL( TInt aFieldId ) = 0;        

    protected:   
        virtual void ProcessVPbkSingleContactOperationCompleteImplL(
                MVPbkContactOperationBase&  aOperation ,
                MVPbkStoreContact* aContact );

    public: // From MLogsReader

        virtual void Stop();
        void Interrupt();
        TBool IsInterrupted() const;
        void SetObserver( MLogsObserver* aObserver );
        void StartL();          
        void StartUpdaterL();
        void ContinueL();
        virtual void ConfigureL( const MLogsReaderConfig* aConfig );
        TBool IsDirty() const;
        void SetDirty();
        void ActivateL();
        void DeActivate();
        
    public: // From MLogsStateHolder

        TLogsState State() const;
       
    protected:
      
        TBool SearchRemotePartyL( const TDesC& aNumber );      

        void  SearchContactLinkL (const TDesC8& aContactLink);

    public: // From MVPbkContactStoreListObserver
    
        void OpenComplete();  

        // From MVPbkContactStoreObserver        
        /**
         * Called when a contact store is ready to use.
         */
        void StoreReady(MVPbkContactStore& aContactStore);

        /**
         * Called when a contact store becomes unavailable.
         * Client may inspect the reason of the unavailability and decide whether or not
         * it will keep the store opened (ie. listen to the store events).
         * @param aContactStore The store that became unavailable.
         * @param aReason The reason why the store is unavailable.
         *                This is one of the system wide error codes.
         */
        void StoreUnavailable(MVPbkContactStore& aContactStore, TInt aReason);

        /**
         * Called when changes occur in the contact store.
         * @see TVPbkContactStoreEvent
         *
         * @param aStoreEvent Event that has occured.
         */
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent);        
        
        // From MVPbkContactFindObserver
              
        /**
         * Called when find is complete. Callee takes ownership of the results.
         * In case of an error during find, the aResults may contain only 
         * partial results of the find.
         *
         * @param aResults Array of contact links that matched the find.
         *                 Callee must take ownership of this object in
         *                 the end of the function, ie. in case the function
         *                 does not leave.
         */
        void FindCompleteL(MVPbkContactLinkArray* aResults);

        /**
         * Called in case the find fails for some reason.
         * 
         * @param aError One of the system wide error codes.
         */
        void FindFailed(TInt aError);
        
        
        // From MVPbkSingleContactOperationObserver
        /**
         * Called when operation is completed.
         *
         * @param aOperation the completed operation.
         * @param aContact  the contact returned by the operation.
         *                  Client must take the ownership immediately.
         *
         *                  !!! NOTICE !!!
         *                  If you use Cleanupstack for MVPbkStoreContact
         *                  Use MVPbkStoreContact::PushL or
         *                  CleanupDeletePushL from e32base.h.
         *                  (Do Not Use CleanupStack::PushL(TAny*) because
         *                  then the virtual destructor of the M-class
         *                  won't be called when the object is deleted).
         */
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);

        /**
         * Called if the operation fails.
         *
         * @param aOperation    the failed operation.
         * @param aError        error code of the failure.
         */
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);
                
    public: //from MPbk2StoreConfigurationObserver
        
        /**
         * Called when the store configuration changes.
         */
         void ConfigurationChanged();
      
        /**
         * Called when the configuration changed event
         * has been delivered to all observers.
         */
         void ConfigurationChangedComplete();
         
         /**
          * Configres the virtual phonebook stores and other
          * api's
          */
         void ConfigureVPbkStoresL();
         
         /**
          * Called when configuration has changed and starting updater.
          * Calls ConfigureVPbkStoresL.
          */
         void ReConfigureVPbkStoresL();
      

    private:
            
        void ProcessVPbkSingleContactOperationCompleteL(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);

                

    protected:    // data
      	CVPbkContactManager*            iContactManager;    
      	MVPbkContactStoreList*          iContactList; 
      	MVPbkStoreContact*              iContact;
        CVPbkContactStoreUriArray*      iStoreArray;
        CVPbkPhoneNumberMatchStrategy*  iPhoneNumberMatchStrategy;
        CPbk2SortOrderManager*          iSortOrderManager;
        MPbk2ContactNameFormatter*      iNameFormatter;
        MVPbkContactOperationBase* 		iOperation;                
        CPbk2StoreConfiguration*        iStoreConfiguration;
        
        CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags iMatchFlags;
        
        TBool                           iStoreConfigChanged;
        TBool                           iPbkOperationsOngoing;

    protected:

        // internal states of object
        enum TReaderPhase
            {
            EInitNotDone = 0,
            EInitNotDoneStartReq,
            EInitDone,
            EFilter,     //used in sms updater
            ERead,            
            EUpdate,
            EDone
            };

               
    protected:    // data
        
        /// ref: file session
        RFs&            iFsSession;        
        TInt            iDigitsToMatch;

        /// own: log client
        CLogClient*     iLogClient;

        /// Own: View to filtered events.
        CLogViewEvent* iLogViewEvent;

        /// ref: observer
        MLogsObserver*  iObserver;

        /// own: phase
        TReaderPhase    iPhase;                   

        /// own: state for observer
        TLogsState      iState;

        /// Own: Event
        CLogEvent* iEvent;

        /// Own: Filter
        CLogFilter* iFilter;

        /// Own: Contact name
        HBufC* iName;
    };

#endif      // __Logs_Engine_CLogsBaseUpdater_H__


// End of File



        

        
       
