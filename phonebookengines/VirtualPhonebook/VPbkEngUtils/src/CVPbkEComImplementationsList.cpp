/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook utility for listing ECOM implementations
*
*/


#include "CVPbkEComImplementationsList.h"

void CleanupEComPtrArray(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::CEComImpl::CEComImpl
// --------------------------------------------------------------------------
//
CVPbkEComImplementationsListBase::CEComImpl::CEComImpl( 
        CImplementationInformation* aInfo, CBase* aImpl )
        :   iInfo( aInfo ),
            iImpl( aImpl )
    {
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::CEComImpl::~CEComImpl
// --------------------------------------------------------------------------
//
CVPbkEComImplementationsListBase::CEComImpl::~CEComImpl()
    {
    delete iInfo;
    delete iImpl;
    }
    
// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::CVPbkEComImplementationsListBase
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkEComImplementationsListBase::CVPbkEComImplementationsListBase()
    {
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::~CVPbkEComImplementationsListBase
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkEComImplementationsListBase::~CVPbkEComImplementationsListBase()
    {
    iImplementations.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::LoadImplementationsL
// --------------------------------------------------------------------------
//        
EXPORT_C void CVPbkEComImplementationsListBase::LoadImplementationsL( 
        TUid aInterfaceUid, TAny* aParam )
    {
    RImplInfoPtrArray implementations;
    CleanupStack::PushL(
        TCleanupItem( CleanupEComPtrArray, &implementations ) );
        
    REComSession::ListImplementationsL( aInterfaceUid, implementations );
    
    DoLoadImplementationsL( implementations, aParam );
    
    CleanupStack::PopAndDestroy(); // TCleanupItem
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::LoadImplementationsL
// --------------------------------------------------------------------------
//        
EXPORT_C void CVPbkEComImplementationsListBase::LoadImplementationsL( 
        TUid aInterfaceUid, 
        const TEComResolverParams& aResolverParams, TAny* aParam )
    {
    RImplInfoPtrArray implementations;
    CleanupStack::PushL(
        TCleanupItem( CleanupEComPtrArray, &implementations ) );
        
    REComSession::ListImplementationsL( aInterfaceUid, aResolverParams, 
            implementations );
    
    DoLoadImplementationsL( implementations, aParam );
    
    CleanupStack::PopAndDestroy(); // TCleanupItem
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::Count
// --------------------------------------------------------------------------
//        
EXPORT_C TInt CVPbkEComImplementationsListBase::Count() const
    {
    return iImplementations.Count();
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::At
// --------------------------------------------------------------------------
//        
EXPORT_C CBase& CVPbkEComImplementationsListBase::At( TInt aIndex ) const
    {
    return *iImplementations[aIndex]->iImpl;
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::FindInfo
// --------------------------------------------------------------------------
//        
EXPORT_C const CImplementationInformation* 
        CVPbkEComImplementationsListBase::FindInfo( CBase& aImpl ) const
    {
    CImplementationInformation* result = NULL;
    const TInt count = iImplementations.Count();
    for ( TInt i = 0; i < count && !result; ++i )
        {
        if ( iImplementations[i]->iImpl == &aImpl )
            {
            result = iImplementations[i]->iInfo;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsListBase::DoLoadImplementationsL
// --------------------------------------------------------------------------
//        
void CVPbkEComImplementationsListBase::DoLoadImplementationsL( 
        RImplInfoPtrArray& aImplInfoPtrArray, 
        TAny* aParam )
    {
    const TInt count = aImplInfoPtrArray.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        CImplementationInformation* implInfo = aImplInfoPtrArray[i];
        CleanupStack::PushL(implInfo);
        aImplInfoPtrArray.Remove(i);
        CBase* impl = CreateImplementationL( implInfo->ImplementationUid(), 
                aParam );
        CleanupStack::PushL( impl );
        CEComImpl* ecomImpl = new ( ELeave ) CEComImpl( implInfo, impl );
        CleanupStack::Pop( 2, implInfo );
        CleanupStack::PushL( ecomImpl );
        iImplementations.AppendL( ecomImpl );
        CleanupStack::Pop( ecomImpl );
        }
    }
