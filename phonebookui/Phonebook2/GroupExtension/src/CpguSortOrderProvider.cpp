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
* Description:  Phonebook 2 all groups view sort order provider.
*
*/


#include "CPguSortOrderProvider.h"

// Phonebook 2
#include "CPguSortOrder.h"
#include <CPbk2StoreConfiguration.h>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_NewL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPguSortOrderProvider");
    User::Panic(KPanicText, aReason);
    };

#endif //_DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguSortOrderProvider::CPguSortOrderProvider
// --------------------------------------------------------------------------
//
CPguSortOrderProvider::CPguSortOrderProvider()
    {
    }

// --------------------------------------------------------------------------
// CPguSortOrderProvider::~CPguSortOrderProvider
// --------------------------------------------------------------------------
//
CPguSortOrderProvider::~CPguSortOrderProvider()
    {
    delete iStoreConfiguration;
    delete iAllGroupsViewSortOrder;
    }
// --------------------------------------------------------------------------
// CPguSortOrderProvider::ConstructL
// --------------------------------------------------------------------------
//
void CPguSortOrderProvider::ConstructL
        ( const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    iAllGroupsViewSortOrder = CPguSortOrder::NewL( aMasterFieldTypeList );
    }

// --------------------------------------------------------------------------
// CPguSortOrderProvider::NewLC
// --------------------------------------------------------------------------
//
CPguSortOrderProvider* CPguSortOrderProvider::NewL( TAny* aParam )
    {
    __ASSERT_DEBUG(aParam, Panic(EPanicPreCond_NewL) );
    TSortOrderAcquirerParam* param = static_cast<TSortOrderAcquirerParam*>
        ( aParam );
    CPguSortOrderProvider* self = new ( ELeave ) CPguSortOrderProvider;
    CleanupStack::PushL( self );
    self->ConstructL( param->iMasterFieldTypeList );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguSortOrderProvider::ApplySortOrderToStoreL
// --------------------------------------------------------------------------
//
TBool CPguSortOrderProvider::ApplySortOrderToStoreL
        ( const TVPbkContactStoreUriPtr& aStoreUri )
    {
    if ( !iStoreConfiguration )
        {
        iStoreConfiguration = CPbk2StoreConfiguration::NewL();
        }

    CVPbkContactStoreUriArray* uris =
        iStoreConfiguration->SupportedStoreConfigurationL();
    TBool result = uris->IsIncluded( aStoreUri );
    delete uris;
    return result;
    }

// --------------------------------------------------------------------------
// CPguSortOrderProvider::SortOrder
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CPguSortOrderProvider::SortOrder() const
    {
    return *iAllGroupsViewSortOrder;
    }

// End of File
