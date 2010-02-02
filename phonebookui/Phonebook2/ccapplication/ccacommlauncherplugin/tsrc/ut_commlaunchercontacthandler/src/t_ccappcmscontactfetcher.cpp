/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
#include "ccappcommlauncherheaders.h"

//#include "t_ccappcmscontactfetcher.h"
// dummy class


CCCAppCmsContactFetcherWrapper::CCCAppCmsContactFetcherWrapper( ) 
    {
    }
void CCCAppCmsContactFetcherWrapper::ConstructL( ) 
    {
    iContactFieldInfo = CCmsContactFieldInfo::NewL();
    CCmsContactField* field = new (ELeave) CCmsContactField( PHONE );
    iFieldArray.Append( field );
    }
    
CCCAppCmsContactFetcherWrapper::~CCCAppCmsContactFetcherWrapper( ) 
    {
    delete iContactFieldInfo;
    iObservers.Close();
    iFieldArray.ResetAndDestroy();
    }

CCCAppCmsContactFetcherWrapper* CCCAppCmsContactFetcherWrapper::InstanceL( CCCAParameter* /*aParameter*/ )
        {
        CCCAppCmsContactFetcherWrapper* self = new (ELeave) CCCAppCmsContactFetcherWrapper( );
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
        }
       
CCCAppCmsContactFetcherWrapper* CCCAppCmsContactFetcherWrapper::InstanceL()
        {
        CCCAppCmsContactFetcherWrapper* self = new (ELeave) CCCAppCmsContactFetcherWrapper(  );
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
        //return NULL;
        }

void CCCAppCmsContactFetcherWrapper::Release()
        {
        delete this;
        } 
        
const CCmsContactFieldInfo* CCCAppCmsContactFetcherWrapper::ContactInfo()
        {
        T_CTestSingleton* singleton = T_CTestSingleton::InstanceL();
        TInt command = singleton->GetValue();
        singleton->Release();
        singleton = NULL;
        if ( KTestContactInfoNull == command )
            {
            return NULL;
            }
        // first case
        return iContactFieldInfo;
        }

RPointerArray<CCmsContactField>& CCCAppCmsContactFetcherWrapper::ContactFieldDataArray()
        {
        return iFieldArray;
        }

HBufC8* CCCAppCmsContactFetcherWrapper::ContactIdentifierLC( 
        const TCmsContactIdentifierType /*aIdType = ECmsPackedContactLinkArray*/ )
        {
        TPtrC8 ptr ( KTestString );
        HBufC8* buf = ptr.AllocLC();
        return buf;
        }

void CCCAppCmsContactFetcherWrapper::AddObserverL( MCCAppContactFieldDataObserver& aObserver )
    {
    const TInt index = iObservers.Find( &aObserver );
    if ( KErrNotFound == index )
        {
        iObservers.AppendL( &aObserver );
        }
    }

void CCCAppCmsContactFetcherWrapper::RemoveObserver( MCCAppContactFieldDataObserver& aObserver )
    {
    const TInt index = iObservers.Find( &aObserver );
    if ( KErrNotFound != index )
        {
        iObservers.Remove( index );
        }
    }

TBool CCCAppCmsContactFetcherWrapper::IsServiceAvailable(
    VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector /*aContactAction */)
    {
    return ETrue;//iCmsContactDataFetcher.IsServiceAvailable( aContactAction );
    }

TCmsContactStore CCCAppCmsContactFetcherWrapper::ContactStore() const
    {
    const_cast<TBool&>(iContactStore_called) = ETrue;
    }


TInt CCCAppCmsContactFetcherWrapper::GetContactActionFieldCount(
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector /*aContactAction*/)
    {
    return ETrue; //iCmsContactDataFetcher.GetContactActionFieldCount(aContactAction);
    }

void CCCAppCmsContactFetcherWrapper::RefetchContactL()
    {
    
    }
  
