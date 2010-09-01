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
* Description:  An engine level attribute manager implementation that is
*                constructed in CVPbkContactManager. 
*                Loads store level attribute managers as ECOM components.
*
*/


#include "CVPbkContactAttributeManager.h"

#include <CVPbkContactAttributePlugin.h>
#include <CVPbkContactFindOperation.h>
#include <MVPbkContactAttribute.h>
#include <MVPbkStoreContact.h>
#include <VPbkStoreUid.h>

namespace {

/**
 * Null typelist.
 */
struct NullTypelist { };

/**
 * Typelist.
 */
template <typename _Head, typename _Tail = NullTypelist>
struct Typelist
    {
    typedef _Head Head;
    typedef _Tail Tail;
    };

/**
 * Typelist indexing helper.
 */
template <typename _List, int _Index>
struct NthInList
    {
    typedef typename NthInList< typename _List::Tail, _Index-1>::Ret Ret;
    };

template <typename _List>
struct NthInList<_List, 0>
    {
    typedef typename _List::Head Ret;
    };

/**
 * TFunct2
 */
template <typename _FPtrType, typename _FuncParList, 
    typename _X, typename _W>
NONSHARABLE_CLASS(TFunct2)
    {
    public:
        typedef typename NthInList<_FuncParList, 0>::Ret Par1;
        typedef typename NthInList<_FuncParList, 1>::Ret Par2;        
        TFunct2(_FPtrType aPtr,
            Par1& aObjA, Par2& aObjB);
        void operator()(_X* aCallee, _W& aValue);
    protected:
        _FPtrType iFuncPtr;
        Par1& iPar1;
        Par2& iPar2;
    };


/**
 * TFunct2::TFunct2
 */ 
template <typename _FPtrType,
    typename _FuncParList, typename _X, typename _W>
TFunct2<_FPtrType, _FuncParList, _X, _W>::TFunct2(
    _FPtrType aPtr, Par1& aObjA, Par2& aObjB) :
    iFuncPtr(aPtr), iPar1(aObjA), iPar2(aObjB)
    {
    }

/**
 * TFunct2::operator()
 */ 
template <typename _FPtrType, 
    typename _FuncParList, typename _X, typename _W>
void TFunct2<_FPtrType, _FuncParList, _X, _W>::operator()(
        _X* aCallee, _W& aValue)
    {
    aValue = (aCallee->*iFuncPtr)(iPar1, iPar2);
    }


/**
 * TFunct3
 */
template <typename _FPtrType, 
    typename _FuncParList, typename _X, typename _W>
NONSHARABLE_CLASS(TFunct3)
    {
    public:
        typedef typename NthInList<_FuncParList, 0>::Ret Par1;
        typedef typename NthInList<_FuncParList, 1>::Ret Par2;
        typedef typename NthInList<_FuncParList, 2>::Ret Par3;
        TFunct3(_FPtrType aPtr,
            Par1& aObjA, Par2& aObjB, Par3& aObjC);
        void operator()(_X* aCallee, _W& aValue);
    private:
        _FPtrType iFuncPtr;
        Par1& iPar1;
        Par2& iPar2;
        Par3& iPar3;
    };


/**
 * TFunct3::TFunct3
 */ 
template <typename _FPtrType, 
    typename _FuncParList, typename _X, typename _W>
TFunct3<_FPtrType, _FuncParList, _X, _W>::TFunct3(
    _FPtrType aPtr, Par1& aObjA, Par2& aObjB, Par3& aObjC) :
    iFuncPtr(aPtr), iPar1(aObjA), iPar2(aObjB), iPar3(aObjC)
    {
    }

/**
 * TFunct3::Call
 */ 
template <typename _FPtrType, 
    typename _FuncParList, typename _X, typename _W>
void TFunct3<_FPtrType, _FuncParList, _X, _W>::operator()(
        _X* aCallee, _W& aValue)
    {
    aValue = (aCallee->*iFuncPtr)(iPar1, iPar2, iPar3);
    }


/**
 * Initialisation for TBool.
 */
inline void initialize(TBool& aPtr)
    {
    aPtr = EFalse;
    }

/**
 * Initialisation for MVPbkContactOperationBase pointer.
 */
inline void initialize(MVPbkContactOperationBase*& aPtr)
    {
    aPtr = NULL;
    }

/**
 * Initialisation for MVPbkStoreContactFieldCollection pointer.
 */
inline void initialize(MVPbkStoreContactFieldCollection*& aPtr)
    {
    aPtr = NULL;
    }

// ---------------------------------------------------------------------------
// ForEachPluginL
// ---------------------------------------------------------------------------
//
template<typename _A, typename _B>
_A ForEachPluginL(
        TArray<CVPbkContactAttributePlugin*> aAttrPlugins, 
        _B aObj )
    {
    _A result;
    initialize(result);

    const TInt count = aAttrPlugins.Count();
    for (TInt i = 0; i < count && !result; ++i)
        {
        CVPbkContactAttributePlugin* plugin = aAttrPlugins[i];
        if (plugin)
            {
            aObj(plugin, result);
            }        
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// CleanupResetAndDestroy
// ---------------------------------------------------------------------------
//    
void CleanupResetAndDestroy( TAny* aObj )
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }
}


// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::CVPbkContactAttributeManager
// ---------------------------------------------------------------------------
//
CVPbkContactAttributeManager::CVPbkContactAttributeManager(
    CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager)
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::ConstructL
// ---------------------------------------------------------------------------
//  
inline void CVPbkContactAttributeManager::ConstructL()
    {
    LoadAttributePluginsL();
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::NewL
// ---------------------------------------------------------------------------
//
CVPbkContactAttributeManager* CVPbkContactAttributeManager::NewL(
        CVPbkContactManager& aContactManager )
    {
    CVPbkContactAttributeManager* self = 
        new(ELeave) CVPbkContactAttributeManager( aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::~CVPbkContactAttributeManager
// ---------------------------------------------------------------------------
//
CVPbkContactAttributeManager::~CVPbkContactAttributeManager()
    {
    iAttributePlugins.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::LoadAttributePluginsL
// ---------------------------------------------------------------------------
//
void CVPbkContactAttributeManager::LoadAttributePluginsL()
    {
    RImplInfoPtrArray implementations;
    TEComResolverParams resolverParams;
    
    resolverParams.SetWildcardMatch(ETrue);
    _LIT8(KVPbkAttributePlugin, KVPbkContactAttributePluginDataString);
    resolverParams.SetDataType(KVPbkAttributePlugin);
    
    REComSession::ListImplementationsL(
            TUid::Uid(KVPbkContactAttributePluginInterfaceUID), implementations);
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy, &implementations));

    CVPbkContactAttributePlugin::TParam param(iContactManager);
    const TInt count = implementations.Count();
    for (TInt i = count-1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);

        CVPbkContactAttributePlugin* plugin = 
            CVPbkContactAttributePlugin::NewL(implInfo->ImplementationUid(), param);
        CleanupStack::PushL(plugin);
        iAttributePlugins.AppendL(plugin);
        CleanupStack::Pop(plugin);
        
        CleanupStack::PopAndDestroy(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations    
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::ListContactsL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::ListContactsL(
        TUid aAttributeType, 
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkContactFindOperation* operation = CVPbkContactFindOperation::NewLC(aObserver);
    
    const TInt pluginCount(iAttributePlugins.Count());
    for (TInt i = 0; i < pluginCount; ++i)
        {
        CVPbkContactAttributePlugin* plugin = iAttributePlugins[i];
        MVPbkContactOperation* subOperation = NULL;
        if (plugin && (plugin->AttributeType() == aAttributeType))
            {
            subOperation = 
                plugin->CreateListContactsOperationL(aAttributeType, aObserver);
            }

        if (subOperation)
            {
            CleanupDeletePushL(subOperation);
            operation->AddSubOperationL(subOperation);
            CleanupStack::Pop(); // subOperation
            }
        }

    if (operation->SubOperationCount() == 0)
        {
        CleanupStack::PopAndDestroy(operation);
        operation = NULL;
        }

    if (operation)
        {
        operation->StartL();
        CleanupStack::Pop(operation);        
        }

    return operation;
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::ListContactsL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::ListContactsL(
        const MVPbkContactAttribute& aAttribute, 
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkContactFindOperation* operation = CVPbkContactFindOperation::NewLC(aObserver);
    
    const TInt pluginCount(iAttributePlugins.Count());
    for (TInt i = 0; i < pluginCount; ++i)
        {
        CVPbkContactAttributePlugin* plugin = iAttributePlugins[i];
        MVPbkContactOperation* subOperation = NULL;
        if (plugin && (plugin->AttributeType() == aAttribute.AttributeType()))
            {
            subOperation = 
                plugin->CreateListContactsOperationL(aAttribute, aObserver);
            }

        if (subOperation)
            {
            CleanupDeletePushL(subOperation);
            operation->AddSubOperationL(subOperation);
            CleanupStack::Pop(); // subOperation
            }
        }

    if (operation->SubOperationCount() == 0)
        {
        CleanupStack::PopAndDestroy(operation);
        operation = NULL;
        }

    if (operation)
        {
        operation->StartL();
        CleanupStack::Pop(operation);        
        }

    return operation;
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasContactAttributeL
// ---------------------------------------------------------------------------
//
TBool CVPbkContactAttributeManager::HasContactAttributeL(
        TUid aAttributeType, 
        const MVPbkStoreContact& aContact) const
    {
    typedef TBool (MVPbkContactAttributeManager::*FPtrType)(TUid, 
        const MVPbkStoreContact&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasContactAttributeL;    
    typedef Typelist<TUid, Typelist<const MVPbkStoreContact> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, TBool> 
        call(ptr, aAttributeType, aContact);
    return ForEachPluginL<TBool>(iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasContactAttributeL
// ---------------------------------------------------------------------------
//
TBool CVPbkContactAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute, 
        const MVPbkStoreContact& aContact) const
    {
    typedef TBool (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, 
        const MVPbkStoreContact&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasContactAttributeL;
    typedef Typelist<const MVPbkContactAttribute, 
        Typelist<const MVPbkStoreContact> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, TBool> 
        call(ptr, aAttribute, aContact);
    return ForEachPluginL<TBool>(iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasFieldAttributeL
// ---------------------------------------------------------------------------
//
TBool CVPbkContactAttributeManager::HasFieldAttributeL(
        TUid aAttributeType, 
        const MVPbkStoreContactField& aField) const
    {
    typedef TBool (MVPbkContactAttributeManager::*FPtrType)(TUid, 
        const MVPbkStoreContactField&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasFieldAttributeL;
    typedef Typelist<TUid, Typelist<const MVPbkStoreContactField> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, TBool> 
        call(ptr, aAttributeType, aField);
    return ForEachPluginL<TBool>(iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasFieldAttributeL
// ---------------------------------------------------------------------------
//
TBool CVPbkContactAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute, 
        const MVPbkStoreContactField& aField) const
    {
    typedef TBool (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, 
        const MVPbkStoreContactField& ) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasFieldAttributeL;
    typedef Typelist<const MVPbkContactAttribute, 
                Typelist<const MVPbkStoreContactField> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, TBool> 
        call(ptr, aAttribute, aField);
    return ForEachPluginL<TBool>(iAttributePlugins.Array(), call);
    }
 
// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::SetContactAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::SetContactAttributeL(
        const MVPbkContactLink& aContactLink, 
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactLink&, const MVPbkContactAttribute&, 
            MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::SetContactAttributeL;
    typedef Typelist<const MVPbkContactLink, Typelist<const MVPbkContactAttribute, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, 
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> 
            call(ptr, aContactLink, aAttribute, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::SetFieldAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::SetFieldAttributeL(
        MVPbkStoreContactField& aField, 
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        MVPbkStoreContactField&, const MVPbkContactAttribute&, 
            MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::SetFieldAttributeL;
    typedef Typelist<MVPbkStoreContactField, Typelist<const MVPbkContactAttribute, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aField, aAttribute, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::RemoveContactAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* 
    CVPbkContactAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& aContactLink, 
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactLink&, const MVPbkContactAttribute&, 
            MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::RemoveContactAttributeL;
    typedef Typelist<const MVPbkContactLink, Typelist<const MVPbkContactAttribute, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams,
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> 
            call(ptr, aContactLink, aAttribute, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::RemoveContactAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* 
    CVPbkContactAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& aContactLink, 
        TUid aAttributeType,
        MVPbkSetAttributeObserver& aObserver )
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactLink&, TUid, MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::RemoveContactAttributeL;
    typedef Typelist<const MVPbkContactLink, Typelist<TUid, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams,
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> call(
            ptr, aContactLink, aAttributeType, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::RemoveFieldAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* 
    CVPbkContactAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField, 
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        MVPbkStoreContactField&, const MVPbkContactAttribute&, 
            MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::RemoveFieldAttributeL;
    typedef Typelist<MVPbkStoreContactField, Typelist<const MVPbkContactAttribute, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aField, aAttribute, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::RemoveFieldAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* 
        CVPbkContactAttributeManager::RemoveFieldAttributeL(
            MVPbkStoreContactField& aField, 
            TUid aAttributeType,
            MVPbkSetAttributeObserver& aObserver)
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        MVPbkStoreContactField&, TUid, MVPbkSetAttributeObserver&);
    FPtrType ptr = &MVPbkContactAttributeManager::RemoveFieldAttributeL;
    typedef Typelist<MVPbkStoreContactField, Typelist<TUid, 
        Typelist<MVPbkSetAttributeObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams,
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> call(
            ptr, aField, aAttributeType, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::FindFieldsWithAttributeLC
// ---------------------------------------------------------------------------
//
MVPbkStoreContactFieldCollection* 
        CVPbkContactAttributeManager::FindFieldsWithAttributeLC(
            TUid aAttributeType,
            MVPbkStoreContact& aContact) const
    {
    typedef MVPbkStoreContactFieldCollection* (MVPbkContactAttributeManager::*FPtrType)(
        TUid, MVPbkStoreContact&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::FindFieldsWithAttributeLC;
    typedef Typelist<TUid, Typelist<MVPbkStoreContact> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkStoreContactFieldCollection*> 
            call(ptr, aAttributeType, aContact);
    return ForEachPluginL<MVPbkStoreContactFieldCollection*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::FindFieldsWithAttributeLC
// ---------------------------------------------------------------------------
//
MVPbkStoreContactFieldCollection* 
    CVPbkContactAttributeManager::FindFieldsWithAttributeLC(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact) const
    {
    typedef MVPbkStoreContactFieldCollection* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, MVPbkStoreContact&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::FindFieldsWithAttributeLC;
    typedef Typelist<const MVPbkContactAttribute, Typelist<MVPbkStoreContact> > FuncParams;
    TFunct2<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkStoreContactFieldCollection*> 
            call(ptr, aAttribute, aContact);
    return ForEachPluginL<MVPbkStoreContactFieldCollection*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::FindFieldWithAttributeL
// ---------------------------------------------------------------------------
//
MVPbkStoreContactField* CVPbkContactAttributeManager::FindFieldWithAttributeL(
        const MVPbkContactAttribute& aAttr, 
        MVPbkStoreContact& aContact ) const
    {
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    const TInt count = fields.FieldCount();
    for ( TInt i = 0; i < count; ++i )
        {
        MVPbkStoreContactField& field = fields.FieldAt( i );
        if ( HasFieldAttributeL( aAttr, field ) )
            {
            return &field;
            }
        }
    return NULL;
    }


// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasContactAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::HasContactAttributeL(
        TUid aAttributeType, 
        const MVPbkStoreContact& aContact,
        MVPbkSingleAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        TUid, const MVPbkStoreContact&, MVPbkSingleAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasContactAttributeL;
    typedef Typelist<TUid, Typelist<const MVPbkStoreContact, 
        Typelist<MVPbkSingleAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams,
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> 
            call(ptr, aAttributeType, aContact, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasContactAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute, 
        const MVPbkStoreContact& aContact,
        MVPbkSingleAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, const MVPbkStoreContact&, 
            MVPbkSingleAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasContactAttributeL;
    typedef Typelist<const MVPbkContactAttribute, Typelist<const MVPbkStoreContact, 
        Typelist<MVPbkSingleAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aAttribute, aContact, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasFieldAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::HasFieldAttributeL(
        TUid aAttributeType, 
        const MVPbkStoreContactField& aField,
        MVPbkSingleAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        TUid, const MVPbkStoreContactField&, MVPbkSingleAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasFieldAttributeL;
    typedef Typelist<TUid, Typelist<const MVPbkStoreContactField, 
        Typelist<MVPbkSingleAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aAttributeType, aField, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::HasFieldAttributeL
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute, 
        const MVPbkStoreContactField& aField,
        MVPbkSingleAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, const MVPbkStoreContactField&, 
            MVPbkSingleAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::HasFieldAttributeL;
    typedef Typelist<const MVPbkContactAttribute, Typelist<const MVPbkStoreContactField, 
        Typelist<MVPbkSingleAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams,
        CVPbkContactAttributePlugin, MVPbkContactOperationBase*> 
            call(ptr, aAttribute, aField, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::FindFieldsWithAttributeLC
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::FindFieldsWithAttributeL(
        TUid aAttributeType,
        MVPbkStoreContact& aContact,
        MVPbkMultiAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        TUid, MVPbkStoreContact&, 
            MVPbkMultiAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::FindFieldsWithAttributeL;
    typedef Typelist<TUid, Typelist<MVPbkStoreContact, 
        Typelist<MVPbkMultiAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aAttributeType, aContact, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// ---------------------------------------------------------------------------
// CVPbkContactAttributeManager::FindFieldsWithAttributeLC
// ---------------------------------------------------------------------------
//
MVPbkContactOperationBase* CVPbkContactAttributeManager::FindFieldsWithAttributeL(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact,
        MVPbkMultiAttributePresenceObserver& aObserver) const
    {
    typedef MVPbkContactOperationBase* (MVPbkContactAttributeManager::*FPtrType)(
        const MVPbkContactAttribute&, MVPbkStoreContact&, 
            MVPbkMultiAttributePresenceObserver&) const;
    FPtrType ptr = &MVPbkContactAttributeManager::FindFieldsWithAttributeL;
    typedef Typelist<const MVPbkContactAttribute, Typelist<MVPbkStoreContact, 
        Typelist<MVPbkMultiAttributePresenceObserver> > > FuncParams;
    TFunct3<FPtrType, FuncParams, CVPbkContactAttributePlugin, 
        MVPbkContactOperationBase*> 
            call(ptr, aAttribute, aContact, aObserver);
    return ForEachPluginL<MVPbkContactOperationBase*>(
        iAttributePlugins.Array(), call);
    }

// End of File
