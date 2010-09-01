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
*     Check if a Contact Link is valid
*
*/

// INCLUDE FILES
#include "CLogsCntLinkChecker.h"
#include "MLogsObserver.h"
#include "MLogsReaderConfig.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::CLogsCntLinkChecker
// ----------------------------------------------------------------------------
//
CLogsCntLinkChecker::CLogsCntLinkChecker( 
    RFs& aFsSession,
    MLogsObserver* aObserver ) :
        CLogsBaseUpdater( aFsSession, aObserver),
        iContactLinkValid( EFalse )
    {
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::NewL
// ----------------------------------------------------------------------------
//
CLogsCntLinkChecker* CLogsCntLinkChecker::NewL( 
    RFs& aFsSession,
    MLogsObserver* aObserver )
    {
    CLogsCntLinkChecker* self = new ( ELeave ) CLogsCntLinkChecker( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
  
// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::ConstructL()
    {
    BaseConstructL(CVPbkPhoneNumberMatchStrategy::EVPbkExactMatchFlag);
    iWait = new( ELeave )CActiveSchedulerWait();
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::~CLogsEventUpdater
// ----------------------------------------------------------------------------
//
CLogsCntLinkChecker::~CLogsCntLinkChecker()
    {
    delete iContactLink;
    
    if ( iWait )
        {
        if( iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
            delete iWait;
            iWait = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::StartRunningL
//
// Called from base class 
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::StartRunningL()   
    {
    //Important that ConfigureL is called before we start here 
    if( iContactLink )
        {
        //We first try to search corresponding contact for this phonenumber from Phonebook
        SearchContactLinkL( *iContactLink );   
        }
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::ContinueRunningL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::ContinueRunningL( TInt aFieldId )
    {
    iContactLinkValid = EFalse;
    
    if( aFieldId != KErrNotFound )    
        {
        iContactLinkValid = ETrue;
        }
    
    iState = EStateCheckContactLinkFinished;
    iPhase = EDone;

    if( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
   
    if ( iObserver )
        {
        iObserver->StateChangedL( this );
        }
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::CntLinkDeletedL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::CntLinkDeletedL()
    {
    if( iContactLink )
        {
        delete iContactLink;
        iContactLink = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::ConfigureL
//
// Sets the contact link which is used to retrieve the contact.
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::ConfigureL( const MLogsReaderConfig* aConfig )   //Implemented from MLogsReader
    {
    if( iContactLink )
        {
        delete iContactLink;
        iContactLink = NULL;
        }
    iState = EStateUndefined;
    iContactLink = aConfig->ContactLink().AllocL();
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::IsSameContactLink
//
// Check if contact link is the same one, if same, no need to reconfig it.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CLogsCntLinkChecker::IsSameContactLink( const TDesC8& aContactLink ) const
    {
    TBool flag = EFalse;
    
    if ( iContactLink 
            && 0 == iContactLink->Compare( aContactLink )
            && ( iState == EStateCheckContactLinkFinished 
                    || iState == EStateInitializing ) )
        {
        flag = ETrue;
        }
    return flag;
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::IsCntLinkValidSync
//
// Check if contact link is valid.
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CLogsCntLinkChecker::IsCntLinkValidSync ( const TDesC8& aContactLink )
    {
    TBool flag = EFalse;
    
    if ( !iContactLink || 0 != iContactLink->Compare(aContactLink) )
        {
        return flag;
        }
    
    if ( iState == EStateInitializing )
        {
        if( !(iWait->IsStarted()) ) 
            {
            iWait->Start();                     
            }
        }
    if ( iState == EStateCheckContactLinkFinished ) 
        {
        if( iContactLinkValid )
            {
            flag = ETrue;
            }
            else
            {
            flag = EFalse;
            }
        }
    
    return flag;
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::RunL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::RunL()
    {
    
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::Stop
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::Stop()
    {
    if ( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        iPbkOperationsOngoing = EFalse;
        iState = EStateInterrupted;
        }
    
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    return;
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::ProcessVPbkSingleContactOperationCompleteImplL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::ProcessVPbkSingleContactOperationCompleteImplL(
    MVPbkContactOperationBase& /* aOperation */,
    MVPbkStoreContact* /*aContact)*/ )
    {
    
    }

// ----------------------------------------------------------------------------
// CLogsCntLinkChecker::HandleStoreEventL
// ----------------------------------------------------------------------------
//
void CLogsCntLinkChecker::HandleStoreEventL( MVPbkContactStore&     /*aContactStore */, 
                                          TVPbkContactStoreEvent aStoreEvent )
    {
    if(aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactDeleted)
        { 
        CntLinkDeletedL();
        }
    }
// End of File


