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


// INCLUDE FILES
#include <e32base.h>
#include <logview.h>
#include <logcli.h>

#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <centralrepository.h>
#include <telconfigcrkeys.h>  //For retrieving number of digits to match 
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkPhoneNumberMatchStrategy.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreListObserver.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>

#include "CLogsBaseUpdater.h"
#include "MLogsObserver.h"



// CONSTANTS
const TInt KContactMaxLength = 70;


// ----------------------------------------------------------------------------
// CLogsBaseUpdater::CLogsBaseUpdater
// ----------------------------------------------------------------------------
//
CLogsBaseUpdater::CLogsBaseUpdater( 
    RFs& aFsSession, 
    MLogsObserver* aObserver) : 
        CActive( EPriorityStandard ),
        iStoreConfigChanged(EFalse),
        iPbkOperationsOngoing(EFalse),
        iFsSession( aFsSession ),
        iObserver( aObserver ),
        iPhase( EInitNotDone ),
        iState( EStateUndefined )
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::~CLogsBaseUpdater
// ----------------------------------------------------------------------------
//
CLogsBaseUpdater::~CLogsBaseUpdater()
    {
    Cancel();   //Active scheduler eventually calls DoCancel here

    if( iLogViewEvent )
        {
        delete iLogViewEvent;
        }

    if( iLogClient )
        {
        delete iLogClient;
        }  

    delete iEvent;
    delete iFilter;
    delete iName;
    delete iSortOrderManager;
    delete iNameFormatter;                
    delete iPhoneNumberMatchStrategy;
	delete iContactManager;        
    delete iStoreArray;    	
    delete iOperation;  
    delete iStoreConfiguration;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::BaseConstructL
// ----------------------------------------------------------------------------
//          //------------------------
void CLogsBaseUpdater::BaseConstructL( CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags )
    {
        
	//Get number of digits used to match
    CRepository* repository = NULL;
    TInt error = KErrNone;
  
    TRAP( error, repository = CRepository::NewL( KCRUidTelConfiguration ));
    
    if ( error == KErrNone)
        {
        error = repository->Get( KTelMatchDigits , iDigitsToMatch );
        delete repository;
        
        if( error )
            {
            iDigitsToMatch =  7; //KMmsNumberOfDigitsToMatch;
            }
    	} 
    	
    User::LeaveIfError( error );
    
    iMatchFlags = aMatchFlags;

    iStoreConfiguration = CPbk2StoreConfiguration::NewL();
    iStoreConfiguration->AddObserverL(*this);
    ConfigureVPbkStoresL();
   
    //Open log db interface
    iLogClient = CLogClient::NewL( iFsSession );    
    iEvent = CLogEvent::NewL();
    iFilter = CLogFilter::NewL();
      
    iName = HBufC::NewL( KContactMaxLength );
    }
            
    //Set preferred pbk stores, create Contact Manager and open stores
    //iStoreArray = CVPbkContactStoreUriArray::NewL();    

//    if( aStore == EAllStores || aStore == EPbkAndSim || aStore == ESimOnly )
// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ConfigurationChanged
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ConfigurationChanged()
    {
    // wait until ConfigurationChangedComplete
    }
    
// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ConfigurationChangedComplete
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ConfigurationChangedComplete()
    {
    iStoreConfigChanged = ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ReConfigureVPbkStoresL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ReConfigureVPbkStoresL() 
    {
    if (!iPbkOperationsOngoing)
        {
        iContactManager->ContactStoresL().CloseAll( *this ); 
    
        delete iSortOrderManager;
        iSortOrderManager = NULL;
        delete iNameFormatter;       
        iNameFormatter = NULL;
        delete iPhoneNumberMatchStrategy;
        iPhoneNumberMatchStrategy = NULL;
        delete iContactManager;  
        iContactManager = NULL;      
        delete iStoreArray;  
        iStoreArray = NULL;

        ConfigureVPbkStoresL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ConfigureVPbkStoresL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ConfigureVPbkStoresL() 
    {
    iPhase = EInitNotDone;
    iPbkOperationsOngoing = ETrue;
    iStoreConfigChanged = EFalse;
    
    iStoreArray = iStoreConfiguration->CurrentConfigurationL(); 
 
    iContactManager = CVPbkContactManager::NewL( *iStoreArray, &iFsSession );
    iContactManager->ContactStoresL().OpenAllL( *this );  //Wait until OpenComplete() is called before executing searches.

    //Create default search strategy. We use same search order that is already set in the iStoreArray
    CVPbkPhoneNumberMatchStrategy::TConfig config = 
        CVPbkPhoneNumberMatchStrategy::TConfig(
                iDigitsToMatch,                                        //MaxMatchDigits, 
                *iStoreArray,                                         //Store priority order
                CVPbkPhoneNumberMatchStrategy::EVPbkSequentialMatch,  //EVPbkSequentialMatch / EVPbkParallelMatch
                iMatchFlags ); 
                    //EVPbkExactMatchFlag: Quarantees that only contacts with an exact match are included in the result set
                    //EVPbkStopOnFirstMatchFlag: Stops the search once at least one contact is found

    iPhoneNumberMatchStrategy = CVPbkPhoneNumberMatchStrategy::NewL( config, *iContactManager, *this );                                               
                               
    //Create name formatter for contact name
    iSortOrderManager = CPbk2SortOrderManager::NewL( iContactManager->FieldTypes() );
    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
            iContactManager->FieldTypes(),  
            *iSortOrderManager );   //Ref, ownership here

    iOperation = NULL; 
    }


// ----------------------------------------------------------------------------
// CLogsBaseUpdater::OpenComplete
//
// From MVPbkContactStoreListObserver. Called when the opening process is complete, ie. all 
// stores have been reported either failed or successfully opened.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::OpenComplete()
    {
    //Note1 We will start running the search only when open attempts of all requested stores are processed. 
    //In case it takes too long, we may need to consider creating a search strategy already in 
    //CLogsBaseUpdater::StoreReady and starting the search there and upgrading strategy when the slowly opening
    //stores are available too.
    //Note2 Here we assume that at least one store (pbk itself) is always successfully opened, so we don't check 
    //availability of stores.
    
    iPbkOperationsOngoing = EFalse;
       
    if( iPhase == EInitNotDoneStartReq )    
        {
        iPhase = EInitDone; 
        TInt err;
        TRAP( err, StartRunningL()); //We have a pending start request, so start immediately               
        }
    else 
        {
        iPhase = EInitDone;  
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::StoreReady
//
// From MVPbkContactStoreListObserver. Called when a contact store is ready to use.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::StoreReady(MVPbkContactStore& aContactStore)
    {
    //Note. We start a pending search only when open attempts of all stores are 
    // processed (see CLogsBaseUpdater::OpenComplete)    
    const TDesC& uri = aContactStore.StoreProperties().Name().UriDes();    
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::StoreUnavailable
//
// Called when a contact store becomes unavailable.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::StoreUnavailable(MVPbkContactStore& aContactStore, TInt /* aReason */ )
    {
    const TDesC& uri = aContactStore.StoreProperties().Name().UriDes();
    
    if ( uri == VPbkContactStoreUris::SimGlobalOwnNumberUri() )
        {
                    //Sim pbk not ok
        }
    else if ( uri == VPbkContactStoreUris::DefaultCntDbUri() )        
        {
                    //Normal phonebook not ok        
        }
    else
        {
        }        
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::HandleStoreEventL
//
// Called when changes occur in the contact store.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::HandleStoreEventL( MVPbkContactStore&     /* aContactStore */, 
                                          TVPbkContactStoreEvent /* aStoreEvent */)
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::FindCompleteL
//
// from MVPbkContactFindObserver
// Called when find is complete. We must take ownership of the results at the end
// of this function in case we would use the the results after this function has 
// exited.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::FindCompleteL(MVPbkContactLinkArray* aResults)
    {
    //In case of an error during find, the aResults may contain only partial results of the find.
    if( aResults->Count() == 1 )   
        {
        // The operation needs to be deleted later to avoid memory leak, so we need to store it                             
        iOperation = iContactManager->RetrieveContactL( aResults->At( 0 ), *this );
        }    
    else    // Fix to error EKCN-73N7VN. In case there was no contact found, we still need to 
        {   // set the KLogEventContactSearched flag, so the sms is shown in the Log view
        iPbkOperationsOngoing = EFalse;
        ContinueRunningL( KErrNotFound );    
        }
      
    delete aResults;    //We don't take ownersip, so delete results (caller will take care
    aResults = NULL;    // of it's cleanupstack)
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::FindFailed
//
// Called in case the find fails for some reason.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::FindFailed(TInt /* aError */)
    {
    iPbkOperationsOngoing = EFalse;
    //If find not successful,we can stop processing here
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::VPbkSingleContactOperationComplete
//
// from MVPbkSingleContactOperationObserver. Called when operation is completed.
//
// @param aOperation the completed operation.
// @param aContact  the contact returned by the operation.
//                  Client must take the ownership immediately.
//
//                  !!! NOTICE !!!
//                  If you use Cleanupstack for MVPbkStoreContact
//                  Use MVPbkStoreContact::PushL or
//                  CleanupDeletePushL from e32base.h.
//                  (Do Not Use CleanupStack::PushL(TAny*) because
//                  then the virtual destructor of the M-class
//                  won't be called when the object is deleted).
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase&  aOperation,
    MVPbkStoreContact*          aContact) //must take the ownership immediately
    {
    TInt err;
    TRAP( err, ProcessVPbkSingleContactOperationCompleteL( 
                    aOperation, aContact)); 
                    
    // At this point, the operation can be deleted
    delete iOperation;
    iOperation = NULL;
    iPbkOperationsOngoing = EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ProcessVPbkSingleContactOperationCompleteL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ProcessVPbkSingleContactOperationCompleteL(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact ) //must take the ownership immediately
    {
    CleanupDeletePushL( aContact ); //take ownership 
    ProcessVPbkSingleContactOperationCompleteImplL( aOperation, aContact );
    CleanupStack::PopAndDestroy( aContact ); 
    ContinueRunningL( 0 );   //FieldId   <<<<<FIXME      
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ProcessVPbkSingleContactOperationCompleteImplL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ProcessVPbkSingleContactOperationCompleteImplL(
    MVPbkContactOperationBase& /* aOperation */,
    MVPbkStoreContact* aContact )
    {
    TPtr name( iName->Des() );
    name.Zero();  
    MVPbkStoreContactFieldCollection& fields = aContact->Fields();

    HBufC* formattedName = NULL;
    formattedName = iNameFormatter->GetContactTitleOrNullL( fields, 0 );
        
    CleanupStack::PushL( formattedName );
    
    if (formattedName != NULL) 
    	{
    
	    TPtr formatted( formattedName->Des() );
	    name.Copy(formatted.Left( name.MaxLength() )); //If MaxLength greater than the length, Left()
	   
	    /*  FIXME: WE need to retrieve field id 
	    TInt count = fields.FieldCount();

	    for(TInt i = 0; i < count; i++ )
	        {
	        MVPbkStoreContactField& field = fields.FieldAt( i ); 
	            // const MVPbkFieldType* fieldType = field.BestMatchingFieldType(); 
	            //fldType->Matches(TVPbkFieldVersitProperty , 0)
	        const MVPbkContactFieldData& fieldData = field.FieldData();  
	        
	        if ( fieldData.DataType() == EVPbkFieldStorageTypeText )
	            {
	            const MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast(fieldData);
	            TPtrC text = textData.Text();
	            name.Copy(text.Left( name.MaxLength() )); //If MaxLength greater than the length, Left()
	                                                      //just extracts the whole of the descriptor  
	            //no contact id anymore available
	            }
	        }        
	    */        
    	
   		}
   		
    CleanupStack::PopAndDestroy( formattedName );  //  just extracts the whole of the descriptor    
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::VPbkSingleContactOperationFailed
//
// Called if the operation fails.
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::VPbkSingleContactOperationFailed( 
    MVPbkContactOperationBase& /* aOperation */,
    TInt                       /* aError */)
    {
    TInt err;
    TRAP( err,  ContinueRunningL( KErrNotFound )); //If not successful,end processing
    
    // At this point, the operation can be deleted
    delete iOperation;
    iOperation = NULL;
    iPbkOperationsOngoing = EFalse;
    }         

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::SearchRemotePartyL
//
// Perform search from Virtual phonbook
// ----------------------------------------------------------------------------
//
TBool CLogsBaseUpdater::SearchRemotePartyL( const TDesC& aNumber )
    {
    iPbkOperationsOngoing = ETrue;
    iPhoneNumberMatchStrategy->MatchL( aNumber );    
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::SearchContactLinkL
// ----------------------------------------------------------------------------
//
void  CLogsBaseUpdater::SearchContactLinkL (const TDesC8& aContactLink)
    {
    iPbkOperationsOngoing = ETrue;
    MVPbkContactLinkArray* contactLinkArray = iContactManager->CreateLinksLC( aContactLink );
    
    const MVPbkContactLink& contactLink = contactLinkArray->At( 0 );
    
    // Retrieve the contact asynchronously
    iOperation = iContactManager->RetrieveContactL( contactLink, *this );
    
    CleanupStack::Pop();
    delete contactLinkArray;
    contactLinkArray = NULL;
    
    return;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::StartUpdaterL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::StartUpdaterL()   //Called from CLogsBaseView
    {
    if( iPhase == EInitNotDone || iPhase == EInitNotDoneStartReq )    
        {
        iPhase = EInitNotDoneStartReq;  //Cannot yet start, so set pending search request
        return;
        }
    else 
        {
        StartRunningL();            
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::StartL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::StartL()     //Default retrieval functionality in case StartUpdaterL
    {//was not directly called.
    if (  !IsActive() && iState != EStateActive  )
        {
        iState = EStateInitializing;
        
        if (iStoreConfigChanged)
            {
            ReConfigureVPbkStoresL();
            }
        
        StartUpdaterL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::IsInterrupted
// ----------------------------------------------------------------------------
//
TBool CLogsBaseUpdater::IsInterrupted() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::Interrupt
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::Interrupt()
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::State
// ----------------------------------------------------------------------------
//
TLogsState CLogsBaseUpdater::State() const
    {
    return iState;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::SetObserver( MLogsObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ContinueL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ContinueL()
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::Stop
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::Stop()
    {
    iState = EStateInterrupted;
    Cancel();
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::DoCancel
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::DoCancel()
    {
    iPhase = EInitDone;   //We are always already initialised when DoCancel is called.
    iState = EStateInterrupted;
    
    if( iLogViewEvent )
        {
        iLogViewEvent->Cancel();
        }
    
    if( iLogClient )
        {
        iLogClient->Cancel();
        }
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::RunL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::RunL()
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::RunError
// ----------------------------------------------------------------------------
//
TInt CLogsBaseUpdater::RunError(TInt aError)
	{
	if( aError == KErrAccessDenied )    //E.g. if DB is currently being backed up there's no access to it.
		{
		return KErrNone;
		}
	else
		{
		return aError;
		}
	}

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ConfigureL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ConfigureL( const MLogsReaderConfig* /*aConfig*/ )
    {
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::ActivateL
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::ActivateL() //Overridden method in derived classes may leave, that's why
    {                              // name of this function indicates that this may leave.
    //no op
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::DeActivate
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::DeActivate()
    {
    //no op
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::IsDirty
// ----------------------------------------------------------------------------
//
TBool CLogsBaseUpdater::IsDirty() const
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CLogsBaseUpdater::SetDirty
// ----------------------------------------------------------------------------
//
void CLogsBaseUpdater::SetDirty()
    {
    //no op
    }



