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
*     Updates Log events adding remote party
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <logfilterandeventconstants.hrh>
#endif
#include <logcli.h>
#include <logview.h>
#include <vpbkeng.rsg>
#include <PbkFields.hrh>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>

#include "CLogsEventUpdater.h"
#include "MLogsObserver.h"
#include "MLogsReaderConfig.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KLogsPbkTypeIdLength = 11;
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsEventUpdater::CLogsEventUpdater
// ----------------------------------------------------------------------------
//
CLogsEventUpdater::CLogsEventUpdater( 
    RFs& aFsSession,
    MLogsObserver* aObserver ) :
        CLogsBaseUpdater( aFsSession, aObserver ),
        iNumberFieldType( EPbkFieldIdNone )
    {
    }

// ----------------------------------------------------------------------------
// CLogsEventUpdater::NewL
// ----------------------------------------------------------------------------
//
CLogsEventUpdater* CLogsEventUpdater::NewL( 
    RFs& aFsSession,
    MLogsObserver* aObserver,
    CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags )
    {
    CLogsEventUpdater* self = new ( ELeave ) CLogsEventUpdater( aFsSession, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aMatchFlags );
    CleanupStack::Pop();
    return self;
    }
  
// ----------------------------------------------------------------------------
// CLogsEventUpdater::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::ConstructL( CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags )
    {
    BaseConstructL( aMatchFlags );
    iLogViewEvent = CLogViewEvent::NewL( *iLogClient );
    iFilter->SetNullFields( ELogRemotePartyField );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CLogsEventUpdater::~CLogsEventUpdater
// ----------------------------------------------------------------------------
//
CLogsEventUpdater::~CLogsEventUpdater()
    {
    delete iNumber;
    }

// ----------------------------------------------------------------------------
// CLogsEventUpdater::StartRunningL
//
// Called from base class 
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::StartRunningL()   
    {
    Cancel();

    //Important that ConfigureL is called before we start here 
    if( iNumber )
        {
        //We first try to search corresponding contact for this phonenumber from Phonebook
        SearchRemotePartyL( *iNumber );   
        }
    }


// ----------------------------------------------------------------------------
// CLogsEventUpdater::ContinueRunningL
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::ContinueRunningL( TInt aFieldId )
    {
    TBool okToContinue( EFalse );    

    //1. Continue only if the contact was found from Phonebook as we prcess only one phone number at a time.
    if( aFieldId != KErrNotFound )    
        {
        
        // iFieldId.Num( aFieldId );
        okToContinue = iLogViewEvent->SetFilterL( *iFilter, iStatus );  //ETrue, if at least one row in db available for filter,
                                                                        //current entry will be the first in view when asynch call
                                                                        //is finished.
        }
    
    //2. Continue only if found one or more database entries that need updating the remote party.
    if( okToContinue )
        {
        iState = EStateActive;
        iPhase = ERead;
        SetActive();                //Continue
        }
    else if( iObserver )            //End this run
        {
        iState = EStateEventUpdaterFinished;
        iPhase = EDone;
        iObserver->StateChangedL( this );
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventUpdater::ConfigureL
//
// Sets the phone number that is used as search criteria this time (all events containing this 
// phone number are updated to log database)
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::ConfigureL( const MLogsReaderConfig* aConfig )   //Implemented from MLogsReader
    {
    if( iNumber )
        {
        delete iNumber;
        iNumber = NULL;
        }
    iNumber = aConfig->Number().AllocL();
    iFilter->SetNumber( *iNumber );
    }


// ----------------------------------------------------------------------------
// CLogsEventUpdater::RunL
//
// Update Log database entries corresponding to the filter
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::RunL()
    {
    if( iStatus != KErrNone && iObserver || 
        *iName == KNullDesC )  // If the contact name is empty, dont update. Otherwise the code below the else 
                               // statement loops forewer between asynch calls to iLogClient->ChangeEvent and
                               // iLogViewEvent->FirstL
        {
        iState = EStateEventUpdaterFinished;
        if ( iObserver )
            {
            iObserver->StateChangedL( this );
            }
        }
    else
        {
        switch( iPhase )
            {
            case ERead:
                {
                TBuf<KLogsPbkTypeIdLength> subject;
                subject.Zero();
                iEvent->CopyL( iLogViewEvent->Event() );   
                iEvent->SetRemoteParty( *iName );
                subject.AppendNum( iNumberFieldType );
                iEvent->SetSubject( subject );
                iEvent->SetContact( KNullContactId );    //In Pbk2 links are used instead of TContactItemId
                iPhase = EUpdate;
                iLogClient->ChangeEvent( *iEvent, iStatus );
                SetActive();
                }
                break;

            case EUpdate:
                if( iLogViewEvent->FirstL( iStatus ) )  //If there's still at least one entry in view, let's continue 
                                                        //processing it (this assumes that the event updated above has
                                                        //already disappeared from view and the "next" shows now as "first"
                                                        //in view, otherwise not all events are updated correctly).        
                    {
                    iPhase = ERead;
                    SetActive();
                    }
                else if( iObserver )
                    {
                    iState = EStateEventUpdaterFinished;
                    iPhase = EDone;
                    iObserver->StateChangedL( this );
                    }
                break;

            default:
                break;
            }
        }   
    }

// ----------------------------------------------------------------------------
// CLogsEventUpdater::ProcessVPbkSingleContactOperationCompleteImplL
//
// Detrived function by base class for get the number's pbk field type
// ----------------------------------------------------------------------------
//
void CLogsEventUpdater::ProcessVPbkSingleContactOperationCompleteImplL(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact )
    {
	//invoke the base class function firstly to get the event's title(name) 
    CLogsBaseUpdater::ProcessVPbkSingleContactOperationCompleteImplL( aOperation, aContact );
 
    iNumberFieldType = EPbkFieldIdNone ;
    TBool found( EFalse );
      
    TInt count = iContactManager->FieldTypes().FieldTypeCount();
        
    for ( TInt i = 0; i < count && !found; i++ )
        {
        const MVPbkFieldType& fieldType = iContactManager->FieldTypes().FieldTypeAt( i );
        const TInt fieldTypeResId = fieldType.FieldTypeResId();
                       
        if ( fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEGEN
             || fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEWORK
             || fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEHOME 
             || fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEHOME
             || fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEWORK
             || fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEGEN
             || fieldTypeResId == R_VPBK_FIELD_TYPE_CARPHONE
             || fieldTypeResId == R_VPBK_FIELD_TYPE_ASSTPHONE
             || fieldTypeResId == R_VPBK_FIELD_TYPE_PAGERNUMBER
             || fieldTypeResId == R_VPBK_FIELD_TYPE_FAXNUMBERGEN
             || fieldTypeResId == R_VPBK_FIELD_TYPE_FAXNUMBERHOME
             || fieldTypeResId == R_VPBK_FIELD_TYPE_FAXNUMBERWORK )
            {
            CVPbkBaseContactFieldTypeIterator* itr = 
                CVPbkBaseContactFieldTypeIterator::NewLC( fieldType, 
                    aContact->Fields() );
                               
            while ( !found && itr->HasNext() )
                {
                const MVPbkBaseContactField* field = itr->Next();
                // Check field type before casting.
                const MVPbkContactFieldData& contactFieldData = field->FieldData();
                if ( EVPbkFieldStorageTypeText == contactFieldData.DataType() )
                    {
                    // get field data
                    const MVPbkContactFieldTextData& data = 
                    MVPbkContactFieldTextData::Cast( contactFieldData );
					//Get the rightmost string pointer with the max length of match(iDigitsToMatch)
                    TPtrC clipNumber = iNumber->Right( iDigitsToMatch );
                    TPtrC fieldNumber = data.Text().Right( iDigitsToMatch );
                    
                    if ( 0 == fieldNumber.Compare( clipNumber ) )               
                        {
                        found = ETrue;
                        iNumberFieldType = NumberPbkFieldFromVPbk( fieldTypeResId );
                        }  
                    }                  
                }
            CleanupStack::PopAndDestroy( itr );
            }   
        } 
    }

TInt CLogsEventUpdater::NumberPbkFieldFromVPbk( TInt aNumberFieldType ) const
    {
    TInt pbkFieldType = EPbkFieldIdNone;
       
    switch ( aNumberFieldType ) 
        {   
        case R_VPBK_FIELD_TYPE_MOBILEPHONEGEN:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEWORK:
        case R_VPBK_FIELD_TYPE_MOBILEPHONEHOME:
            pbkFieldType = EPbkFieldIdPhoneNumberMobile;           
            break;

        case R_VPBK_FIELD_TYPE_LANDPHONEHOME:
        case R_VPBK_FIELD_TYPE_LANDPHONEWORK:
        case R_VPBK_FIELD_TYPE_LANDPHONEGEN:
            pbkFieldType = EPbkFieldIdPhoneNumberGeneral;            
            break;
                
        case R_VPBK_FIELD_TYPE_PAGERNUMBER:
            pbkFieldType = EPbkFieldIdPagerNumber;            
            break;
            
        case R_VPBK_FIELD_TYPE_FAXNUMBERGEN:
        case R_VPBK_FIELD_TYPE_FAXNUMBERHOME:
        case R_VPBK_FIELD_TYPE_FAXNUMBERWORK:
            pbkFieldType = EPbkFieldIdFaxNumber;           
            break;
             
        case R_VPBK_FIELD_TYPE_ASSTPHONE:
            pbkFieldType = EPbkFieldIdAssistantNumber; 
            break;
  
        case R_VPBK_FIELD_TYPE_CARPHONE:
            pbkFieldType = EPbkFieldIdCarNumber; 
            break;                   
                     
        default:
            break;
        }
    
    return pbkFieldType;  
    }

// End of File

