/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides the all contacts view sort order.
*
*/


// INCLUDE FILES
#include "CPbk2SortOrderProvider.h"

// Phonebook2
#include <CPbk2SortOrderManager.h>
#include <CPbk2StoreConfiguration.h>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>

// unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_NewL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2SortOrderProvider");
    User::Panic(KPanicText, aReason);
    };

#endif //_DEBUG

} // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2SortOrderProvider::CPbk2SortOrderProvider
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2SortOrderProvider::CPbk2SortOrderProvider()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2SortOrderProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2SortOrderProvider::ConstructL(
        const MVPbkFieldTypeList& aMasterFieldTypeList)
    {
    iAllContactsViewSortOrder = CPbk2SortOrderManager::NewL(
            aMasterFieldTypeList);
    }

// -----------------------------------------------------------------------------
// CPbk2SortOrderProvider::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPbk2SortOrderProvider* CPbk2SortOrderProvider::NewL(
        TAny* aParam)
    {
    __ASSERT_DEBUG(aParam, Panic(EPanicPreCond_NewL) );
    TSortOrderAcquirerParam* param = static_cast<TSortOrderAcquirerParam*>(aParam);
    CPbk2SortOrderProvider* self = new ( ELeave ) CPbk2SortOrderProvider;
    CleanupStack::PushL( self );
    self->ConstructL(param->iMasterFieldTypeList);
    CleanupStack::Pop(self);
    return self;
    }

    
// Destructor
CPbk2SortOrderProvider::~CPbk2SortOrderProvider()
    {
    delete iStoreConfiguration;
    delete iAllContactsViewSortOrder;
    }

// -----------------------------------------------------------------------------
// CPbk2SortOrderProvider::SortOrder
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CPbk2SortOrderProvider::SortOrder() const
    {
    return iAllContactsViewSortOrder->SortOrder();
    }

// -----------------------------------------------------------------------------
// CPbk2SortOrderProvider::ApplySortOrderToStoreL
// This sort order provider is valid for all pb2 stores i.e stores that can
// be seen in names list.
// -----------------------------------------------------------------------------
//
TBool CPbk2SortOrderProvider::ApplySortOrderToStoreL( 
        const TVPbkContactStoreUriPtr& aStoreUri )
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
//  End of File  
