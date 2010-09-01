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
* Description:  A class offers an API to find copy policy for different stores
*
*/



// INCLUDE FILES
#include <CVPbkContactCopyPolicyManager.h>

#include <CVPbkContactCopyPolicy.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkPolicyUid.h>
#include <ecom/ecom.h>

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupResetAndDestroy
// -----------------------------------------------------------------------------
//
namespace 
    {
    void CleanupResetAndDestroy(TAny* aObj)
        {
        if (aObj)
            {
            static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
            }
        }
    }
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::CVPbkContactCopyPolicyManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkContactCopyPolicyManager::CVPbkContactCopyPolicyManager()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkContactCopyPolicyManager::ConstructL()
    {
    RImplInfoPtrArray implementations;
    TEComResolverParams resolverParams;
    
    resolverParams.SetWildcardMatch(ETrue);
    _LIT8(KPbkExtension, KVPbkCopyPolicyDataString);
    resolverParams.SetDataType(KPbkExtension);
        
    REComSession::ListImplementationsL(
        TUid::Uid(KVPbkCopyPolicyInterfaceUID), 
        resolverParams, KRomOnlyResolverUid, implementations);
    
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &implementations));

    const TInt count = implementations.Count();
    for (TInt i = count-1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);
        iImplementationInfoArray.AppendL(implInfo);
        CleanupStack::Pop(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkContactCopyPolicyManager* CVPbkContactCopyPolicyManager::NewL()
    {
    CVPbkContactCopyPolicyManager* self = 
        new( ELeave ) CVPbkContactCopyPolicyManager;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// Destructor
CVPbkContactCopyPolicyManager::~CVPbkContactCopyPolicyManager()
    {
    iImplementationInfoArray.ResetAndDestroy();
    iPolicyArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::GetPolicyL
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkContactCopyPolicy* CVPbkContactCopyPolicyManager::GetPolicyL(
        CVPbkContactManager& aContactManager,
        const TVPbkContactStoreUriPtr& aUri)
    {
    MVPbkContactCopyPolicy* policy = NULL;
    CImplementationInformation* info = FindInfoL(aUri);
    if (info)
        {
        TUid uid(info->ImplementationUid());
        policy = FindPolicy(uid);
        if (!policy)
            {
            policy = LoadPolicyL(uid, aContactManager);
            }
        }
    return policy;
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::FindInfoL
// -----------------------------------------------------------------------------
//
CImplementationInformation* CVPbkContactCopyPolicyManager::FindInfoL(
        const TVPbkContactStoreUriPtr& aUri)
    {
    CImplementationInformation* info = NULL;
    const TPtrC domain = aUri.Component(
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType);
    const TInt count = iImplementationInfoArray.Count();
    for (TInt i = 0; i < count && !info; ++i)
        {
        const TInt length = iImplementationInfoArray[i]->OpaqueData().Length();
        HBufC* opaqueBuf = HBufC::NewLC(length);
        TPtr opaque = opaqueBuf->Des();
        opaque.Copy(iImplementationInfoArray[i]->OpaqueData()); 
    
        if (opaque.CompareF(domain) == 0)
            {
            info = iImplementationInfoArray[i];
            }
        CleanupStack::PopAndDestroy(opaqueBuf);
        }
    return info;
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::FindPolicy
// -----------------------------------------------------------------------------
//
MVPbkContactCopyPolicy* CVPbkContactCopyPolicyManager::FindPolicy(TUid aUid)
    {
    const TInt count = iPolicyArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iPolicyArray[i]->ImplementationUid() == aUid)
            {
            return iPolicyArray[i];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CVPbkContactCopyPolicyManager::LoadPolicyL
// -----------------------------------------------------------------------------
//    
MVPbkContactCopyPolicy* CVPbkContactCopyPolicyManager::LoadPolicyL(TUid aUid,
        CVPbkContactManager& aContactManager)
    {
    CVPbkContactCopyPolicy::TParam param(aContactManager);
    CVPbkContactCopyPolicy* policy = CVPbkContactCopyPolicy::NewL(aUid, param);
    CleanupStack::PushL(policy);
    iPolicyArray.AppendL(policy);
    CleanupStack::Pop(policy);
    return policy;
    }
//  End of File  
