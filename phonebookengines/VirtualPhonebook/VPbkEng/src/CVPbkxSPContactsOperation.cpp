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
* Description:  xSP Contact operation
 *
*/


// INCLUDES
#include "CVPbkxSPContactsOperation.h"


#include <barsc.h>
#include <barsread.h>


#include <TVPbkWordParserCallbackParam.h>
#include <vpbkeng.rsg>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeRefsList.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include "VPbkDebug.h"

#include <CVPbkEComImplementationsList.h>
#include <spsettings.h>
#include <spproperty.h>
namespace
    {

#ifdef _DEBUG	
    enum TxSPErrors
        {
        ExSPErrorWrongLogic1,
        ExSPErrorWrongState,
        EInvalidOperation,
        ExSPErrorWrongStateRun
        };

    _LIT( KPanicStrxSPManagement, "VPbk_xSPManag");

    void Panic(TxSPErrors aReason)
        {
        User::Panic(KPanicStrxSPManagement, aReason);
        }
#endif	
    }

///////////////////////////////////////////////////////////////////////////////

CVPbkxSPContactsOperation* CVPbkxSPContactsOperation::NewLC(
        const MVPbkStoreContact& aContact,
        CVPbkContactManager& aContactManager,
        MVPbkOperationErrorObserver& aErrorObserver, TxSPOperation aOperation)
    {
    CVPbkxSPContactsOperation* self = new (ELeave) CVPbkxSPContactsOperation(
            aContact,
            aContactManager,
            aErrorObserver,
            aOperation );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CVPbkxSPContactsOperation::ConstructL()
    {
    iContactLink = iContact.CreateLinkLC();
    CleanupStack::Pop();  //iContactLink
    iContactLinks = CVPbkContactLinkArray::NewL();
    CompleteOurself(); //start execution right away    
    }

CVPbkxSPContactsOperation::CVPbkxSPContactsOperation(
    const MVPbkStoreContact& aContact,
    CVPbkContactManager& aContactManager,
    MVPbkOperationErrorObserver& aErrorObserver, 
    TxSPOperation /*aOperation*/) :
    CActive(EPriorityStandard), 
        iNextState(EStateNone), iContact(aContact),
        iContactManager(aContactManager), iErrorObserver(&aErrorObserver)
    {
    CActiveScheduler::Add(this);
    }

CVPbkxSPContactsOperation::~CVPbkxSPContactsOperation()
    {
    Cancel();
    delete iContactOperation;

    if(iContactLinks) 
        {
        iContactLinks->ResetAndDestroy();
        }
    
    delete iContactLinks;
    delete iContactLink;
    delete iFieldTypeRefList;
    delete iXspIdArray;
    }

MVPbkContactOperationBase* CVPbkxSPContactsOperation::NewGetxSPLinksOperationL(
        const MVPbkStoreContact& aContact,
        CVPbkContactManager& aContactManager,
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver, TxSPOperation aOperation)
    {
    __ASSERT_DEBUG( aOperation == EGetxSPContactLinks,
            Panic(EInvalidOperation) );

    CVPbkxSPContactsOperation* self = NewLC(aContact, aContactManager,
            aErrorObserver, aOperation);
    self->iLinksObserver = &aObserver;
    self->iNextState = EStateFind;

    CleanupStack::Pop(self);
    return self;
    }

void CVPbkxSPContactsOperation::DoCancel()
    {
    delete iContactOperation;
    iContactOperation = NULL;
    }

void CVPbkxSPContactsOperation::RunL()
    {
    switch(iNextState)
        {
        case EStateFind:
            {
            GetIMPPDataL();
            }
            break;
        
        case EStateAbortWithError:
            {
            NotifyError(iAbortError);
            }
            break;
            
        default:
            {
            __ASSERT_DEBUG( EFalse,Panic( ExSPErrorWrongStateRun ) );
            }
        }
    }

TInt CVPbkxSPContactsOperation::RunError(TInt aError)
    {
    NotifyError(aError);
    return KErrNone;
    }

//---------------------------------------------------------------
// These are from MVPbkContactStoreObserver

void CVPbkxSPContactsOperation::FindFromStoreSucceededL(
        MVPbkContactStore& /*aStore*/,
        MVPbkContactLinkArray* aResultsFromStore)
    {
    ProcessContactLinksL(aResultsFromStore);
    delete aResultsFromStore;
    }

void CVPbkxSPContactsOperation::FindFromStoreFailed(
        MVPbkContactStore& /*aStore*/, TInt /*aError*/)
    {
    //do nothing
    }

void CVPbkxSPContactsOperation::FindFromStoresOperationComplete()
    {
    TRAP_IGNORE(SubsearchCompleteL());
    }

// ----------------------------------------------------------
// CVPbkxSPContactsOperation::GetIMPPDataL
// 
// ----------------------------------------------------------
//
void CVPbkxSPContactsOperation::GetIMPPDataL()
    {
    //Check is contact valid.
    __ASSERT_ALWAYS ( NULL != &iContact, User::Leave(KErrArgument));
    
    TBool imppFound = EFalse;
    iXspIdArray = new ( ELeave ) CDesCArrayFlat( 5 );    
    const MVPbkFieldType* type = 
        iContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP);
    CVPbkBaseContactFieldTypeIterator* itr =
            CVPbkBaseContactFieldTypeIterator::NewLC( *type,
                    iContact.Fields() );
    if (itr->HasNext() )
        {
        imppFound = ETrue;
        while (itr->HasNext() )
            {
            const MVPbkBaseContactField* field = itr->Next();
            const MVPbkContactFieldUriData& uri =
                    MVPbkContactFieldUriData::Cast(field->FieldData() );
            HBufC* data = uri.Uri().AllocL();
            CleanupStack::PushL(data);
            iXspIdArray->AppendL(data->Des() ); 
            CleanupStack::PopAndDestroy(data); 
            }

        iFieldTypeRefList = CVPbkFieldTypeRefsList::NewL();
        iFieldTypeRefList->AppendL( 
            *iContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP) );
        FindUsingIdL();
        }

    CleanupStack::PopAndDestroy(itr);     

    //If there isn't any impp fields, notify empty contact link array. 
    if (!imppFound)
        {
        NotifyResult();
        }
    }

// ---------------------------------------------------------------------------
// CVPbkxSPContactsOperation::FindUsingIdL
// ---------------------------------------------------------------------------
//
TBool CVPbkxSPContactsOperation::FindUsingIdL()
    {
    TBool started(EFalse);

    if( iXspIdArray->MdcaCount() > iNextXspId )
        {
        if ( iContactOperation != NULL )
            {
            delete iContactOperation;
            iContactOperation = NULL;
            }
   
        started = ETrue;
        CDesCArrayFlat* idArray = new ( ELeave ) CDesCArrayFlat( 1 );
        CleanupStack::PushL(idArray);
        idArray->AppendL( iXspIdArray->MdcaPoint(iNextXspId) );
        iNextXspId++;
        TCallBack parser(CVPbkxSPContactsOperation::WordParserL, this);        
        CDesC16ArrayFlat* aXUriArray = new ( ELeave ) CDesCArrayFlat( 52 );
		CleanupStack::PushL(aXUriArray);
		RIdArray ids;
		CleanupClosePushL( ids );
		CSPSettings* settings = CSPSettings::NewL();
		CleanupStack::PushL( settings ); 
		TInt error = settings->FindServiceIdsL( ids );  
		const TInt count = ids.Count();
       	TVPbkContactStoreUriPtr defaultUri = VPbkContactStoreUris::DefaultCntDbUri();

		for( TInt i = 0; i < count; i++ )
		    {
		    CSPProperty* property = CSPProperty::NewLC();
		    error = settings->FindPropertyL( ids[i], EPropertyContactStoreId,
		        *property );
		    if( KErrNone == error )
		        {
		        RBuf uri;
		        uri.Create( KSPMaxDesLength );
		        CleanupClosePushL( uri );  
		        property->GetValue( uri );
		        
		        // Append the uri which is not equal to default store uri
		       	if ( defaultUri.UriDes().CompareF(uri) != KErrNone )
		       		{
		       		aXUriArray->AppendL( uri );
		       		}
		        CleanupStack::PopAndDestroy();  //uri
		        }
		    CleanupStack::PopAndDestroy();  //property
		    }
		CleanupStack::PopAndDestroy( 2 ); // settings, ids
        
        iContactOperation = iContactManager.FindL( *idArray, *iFieldTypeRefList,
                *this, parser,*aXUriArray);
                
        CleanupStack::PopAndDestroy( 2 );
        }
    
    return started;
    }

// ----------------------------------------------------------
// CVPbkxSPContactsOperation::WordParserL
// 
// ----------------------------------------------------------
//
TInt CVPbkxSPContactsOperation::WordParserL(TAny* aWordParserParam)
    {
    //Get all data, no need to parse anything.. 
    TVPbkWordParserCallbackParam* param =
            static_cast<TVPbkWordParserCallbackParam*>(aWordParserParam );
    TPtrC data(param->iStringToParse->Ptr() );
    param->iWordArray->AppendL(data);
    return 0;
    }

void CVPbkxSPContactsOperation::ProcessContactLinksL(
        MVPbkContactLinkArray* aResultsFromStore)
    {
    TInt count = aResultsFromStore->Count();

    for (TInt a = 0; a < count; a++)
        {
        const MVPbkContactLink& link = aResultsFromStore->At(a);

        //If not from original storage, add to array.
        if (!link.RefersTo(iContact))
            {
            if(iContactLinks->Find(link) == KErrNotFound)
                {
                iContactLinks->AppendL(link.CloneLC());
                CleanupStack::Pop(); //Clone
                }
            }
        }
    }

// --------------------------------------------------------------------------
// Completes this active object.
// --------------------------------------------------------------------------
void CVPbkxSPContactsOperation::CompleteOurself()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic(ExSPErrorWrongLogic1) );
        }
    }

// --------------------------------------------------------------------------
// Search completed 
// --------------------------------------------------------------------------
void CVPbkxSPContactsOperation::SubsearchCompleteL()
    {
    if(!FindUsingIdL())
        {
        NotifyResult();
        }
    }

// --------------------------------------------------------------------------
// Notifies operation observer of results
// --------------------------------------------------------------------------
void CVPbkxSPContactsOperation::NotifyResult()
    {
    iLinksObserver->VPbkOperationResultCompleted( this, iContactLinks);
    }

// --------------------------------------------------------------------------
// Sends error notification to the error observer
// --------------------------------------------------------------------------
inline void CVPbkxSPContactsOperation::NotifyError(TInt aErr)
    {
    iErrorObserver->VPbkOperationFailed( this, aErr);
    }

// --------------------------------------------------------------------------
// Notify error async
// --------------------------------------------------------------------------
void CVPbkxSPContactsOperation::AbortWithError(TInt aErr)
    {
    iAbortError = aErr;
    iNextState = EStateAbortWithError;
    CompleteOurself();
    }

// end of file
