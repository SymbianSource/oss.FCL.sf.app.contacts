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
* Description:  Contact model store Voice tag attribute operation.
*
*/


#include "csindvoicetagattributelistoperation.h"
#include "ccontactstore.h"
#include "ccontactlink.h"

#include <cvpbkcontactlinkarray.h>
#include <mvpbkcontactfindobserver.h>

// System includes
#include <vuivoicerecogdefs.h>  // KVoiceDialContext
// From Contacts Model
#include <cntitem.h>

namespace VPbkCntModel {

namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_GetContextCompleted
    };

void Panic( TPanicCode aPanicCode )
    {
    _LIT(KPanicText, "CSindVoiceTagAttributeListOperation");
    User::Panic(KPanicText, aPanicCode);
    };
#endif // _DEBUG
	    
} // unnamed namespace

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::CSindVoiceTagAttributeListOperation
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeListOperation::CSindVoiceTagAttributeListOperation(
        CContactStore& aContactStore,
        MNssContextMgr& aNssContextManager,
        MNssTagMgr& aNssTagManager,
        MVPbkContactFindObserver& aObserver) :
    iContactStore( aContactStore ),
    iNssContextManager( aNssContextManager ),
    iNssTagManager( aNssTagManager ),
    iFindObserver( aObserver ),
    iContactIterator( aContactStore.NativeDatabase() )
    {
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::ConstructL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::ConstructL()
    {
    iContactLinks = CVPbkContactLinkArray::NewL();
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::NewListLC
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeListOperation* CSindVoiceTagAttributeListOperation::NewListLC(
        CContactStore& aContactStore,
        MNssContextMgr& aNssContextManager,
        MNssTagMgr& aNssTagManager,
        MVPbkContactFindObserver& aObserver)
    {
    CSindVoiceTagAttributeListOperation* self = 
        new(ELeave) CSindVoiceTagAttributeListOperation( aContactStore, 
            aNssContextManager, aNssTagManager, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::~CSindVoiceTagAttributeListOperation
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeListOperation::~CSindVoiceTagAttributeListOperation()
    {
    delete iContactLinks;
    delete iNssContext;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::StartL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::StartL()
    {
    // Get first the voice tag context asynchronously. 
    // See GetContextCompleted.
    TInt res = iNssContextManager.GetContext( this, KVoiceDialContext );
    if ( res != TNssVasCoreConstant::EVasSuccess )
        {
        // NSS error codes can positive. We must use system wide error
        // codes.
        if ( res >= 0 )
            {
            res = KErrUnknown;
            }
        User::Leave( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::Cancel
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::Cancel()
    {
    // CancelGetContext not yet available
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetContextCompleted
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::GetContextCompleted( 
        MNssContext* aContext )
    {
    __ASSERT_DEBUG( !iNssContext, Panic( EPreCond_GetContextCompleted ));
    delete iNssContext;
    iNssContext = aContext;
    
    // Context received. Voice tags can be checked now.
    TRAPD( res, GetNextTagL() );
    if ( res != KErrNone )
        {
        iFindObserver.FindFailed( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetContextListCompleted
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::GetContextListCompleted( 
        MNssContextListArray* /*aContextList*/ )
    {
    // Not used by this operation
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetContextFailed
// --------------------------------------------------------------------------
//        
void CSindVoiceTagAttributeListOperation::GetContextFailed( 
        TNssGetContextClientFailCode aFailCode )
    {
    TInt res = KErrUnknown;
    
    switch ( aFailCode )
        {
        case MNssGetContextClient::EVASDBItemNotFound:
            {
            res = KErrNotFound;
            break;
            }
        case MNssGetContextClient::EVASDBNoMemory:
            {
            res = KErrNoMemory;
            break;
            }
        case MNssGetContextClient::EVASDBDiskFull:
            {
            res = KErrDiskFull;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    
    iFindObserver.FindFailed( res );
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetTagListCompleted
// --------------------------------------------------------------------------
//    
void CSindVoiceTagAttributeListOperation::GetTagListCompleted( 
        MNssTagListArray* aTagList )
    {
    TRAPD( res, HandleGetTagListCompletedL( aTagList ) );
    if ( res != KErrNone )
        {
        iFindObserver.FindFailed( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetTagFailed
// --------------------------------------------------------------------------
//
void CSindVoiceTagAttributeListOperation::GetTagFailed( 
        TNssGetTagClientFailCode aFailCode )
    {
    TInt res = KErrNone;
    
    switch ( aFailCode )
        {
        case MNssGetTagClient::EVASDBItemNotFound:
            {
            // Tag not found for the iCurrentId -> move to next contact.
            TRAP( res, GetNextTagL() );
            break;
            }
        case MNssGetTagClient::EVASDBNoMemory:
            {
            res = KErrNoMemory;
            break;
            }
        case MNssGetTagClient::EVASDBDiskFull:
            {
            res = KErrDiskFull;
            break;
            }
        default:
            {
            res = KErrUnknown;
            break;
            }
        }
    
    if ( res != KErrNone )
        {
        iFindObserver.FindFailed( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::CompleteOperation
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::CompleteOperation()
    {
    // An empty store -> return an empty link array
    CVPbkContactLinkArray* results = iContactLinks;
    iContactLinks = NULL;
    // Client takes the ownership of results. See MVPbkContactFindObserver
    TRAPD( res, iFindObserver.FindCompleteL( results ) );
    if ( res != KErrNone )
        {
        iFindObserver.FindFailed( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::HandleGetTagListCompletedL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::HandleGetTagListCompletedL( 
        MNssTagListArray* aTagList )
    {
    if ( aTagList )
        {
        TBool hasVoiceTag = aTagList->Count() > 0;
        aTagList->ResetAndDestroy();
        delete aTagList;
        if ( hasVoiceTag )
            {
            // There is a voice tag for a contact
            CContactLink* link = CContactLink::NewLC( iContactStore, 
                iCurrentId );
            iContactLinks->AppendL( link );
            CleanupStack::Pop( link );    
            }
        }
    
    GetNextTagL();
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeListOperation::GetNextTagL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeListOperation::GetNextTagL()
    {
    // Note that iterator is used asynchronously in this opeartion which 
    // means that the iterator can become invalid if some other client
    // is modifying the data base.
    iCurrentId = iContactIterator.NextL();
    TBool asyncRequestStarted = EFalse;
    while ( iCurrentId != KNullContactId && !asyncRequestStarted )
        {
        const TInt position = 0;
        // This is asynchronous operation if it returns KErrNone.
        // Otherwise it's synchronous.
        TInt res = iNssTagManager.GetTagList( this, iNssContext,
            (TInt) iCurrentId, position );
        if ( res != KErrNone )
            {
            // The current id didn't have voice tag. Try next.
            iCurrentId = iContactIterator.NextL();
            }
        else
            {
            // The current id has a voice tag but now this is asynchrnous
            // and it can not be canceled though the tag is already known.
            // Wait callback. See GetTagListCompleted.
            asyncRequestStarted = ETrue;
            }
        }
        
    if ( iCurrentId == KNullContactId )
        {
        CompleteOperation();
        }
    }
} // namespace VPbkCntModel

// End of File
