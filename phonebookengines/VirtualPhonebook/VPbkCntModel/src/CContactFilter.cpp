/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook contact filter observer
*
*/


#include "CContactFilter.h"

#include <cntdef.h>
#include <cntfilt.h>

#include "CContactStore.h"
#include "CContact.h"
#include "MContactFilterObserver.h"

#include <CVPbkContactLinkArray.h>

namespace VPbkCntModel {

const TInt KFirstIndexOfArray( 0 );

#ifdef _DEBUG
enum TPanicCode
	{
	EPanicLogic_ContactViewReady = 1,
	EPanicLogic_ContactOperationCompleted
	};
	
void Panic( TPanicCode aPanic )
	{
	_LIT( KPanicCat, "VPbkCntModel::CContactFilter" );
	User::Panic( KPanicCat, aPanic );
	}
#endif // _DEBUG
	

CContactFilter::CContactFilter(
    	MContactFilterObserver& aObserver,
    	CContactStore& aStore ) : 
    CActive( CActive::EPriorityIdle ), 
	iObserver( aObserver ), 
    iStore( aStore )
	{
	CActiveScheduler::Add( this );
	}
	
CContactFilter* CContactFilter::NewL(
	 	MContactFilterObserver& aObserver,
	 	CContactStore& aStore )
	{
	CContactFilter* self = new ( ELeave ) CContactFilter(aObserver, aStore );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
void CContactFilter::ConstructL()
	{
	iLinkArray = CVPbkContactLinkArray::NewL();
	}
			 	
CContactFilter::~CContactFilter()
	{
	Cancel();
	delete iLinkArray;
	delete iContactIdArray;
	}

void CContactFilter::StartL()
	{
	iState = EGettingContactIds;
	IssueRequest();
	}

void CContactFilter::RunL()
	{
	switch( iState )
		{
		case EGettingContactIds:
			{
			CCntFilter* filter = CCntFilter::NewLC();
			filter->SetContactFilterTypeALL( EFalse ); // Set all EFalse
			filter->SetContactFilterTypeCard( ETrue ); // Set contact card ETrue
			iStore.NativeDatabase().FilterDatabaseL( *filter );
			if ( iContactIdArray )
			    {
			    delete iContactIdArray;
			    iContactIdArray = NULL;
			    }
			iContactIdArray = CContactIdArray::NewL( filter->iIds );
			CleanupStack::PopAndDestroy( filter );
			iState = EFilteringContact;
			IssueRequest();		
			break;
			}
			
		case EFilteringContact:
			{
			if (iContactIdArray->Count() > 0)
				{
				CContactItem* cntItem = 
					iStore.NativeDatabase().ReadContactL( 
					        (*iContactIdArray)[KFirstIndexOfArray] );
				CleanupStack::PushL( cntItem );
				CContact* storeContact = CContact::NewL( iStore, cntItem );
				CleanupStack::Pop( cntItem ); // CContact took the ownership
				CleanupStack::PushL( storeContact );
				if (iObserver.IsIncluded( *storeContact) )
					{
					iLinkArray->AppendL( storeContact->CreateLinkLC() );
					CleanupStack::Pop(); // link
					}
				CleanupStack::PopAndDestroy( storeContact );
				iContactIdArray->Remove( KFirstIndexOfArray );
				IssueRequest();
				}
			else
				{
				IssueStopRequest();
				}
			break;
			}
		
		case EStopping:
			{
			iObserver.FilteringDoneL( iLinkArray );
			iLinkArray = NULL;
			break;
			}
		
		default:;
		}
	}
	
void CContactFilter::DoCancel()
	{
	// do nothing
	}
	
TInt CContactFilter::RunError( TInt aError )
	{
	HandleError( aError );
	return KErrNone;
	}
	
void CContactFilter::HandleError( TInt aError )
	{
	if ( aError != KErrNone )
		{
		iObserver.FilteringError( aError );		
		}
	}
			
void CContactFilter::IssueRequest()
	{
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();	
	}
	
void CContactFilter::IssueStopRequest()
	{
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }	
	}	
			
} // namespace VPbkCntModel

// End of file
