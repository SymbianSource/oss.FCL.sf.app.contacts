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
* Description:  Phonebook 2 sort order manager.
*
*/


#include <CPbk2SortOrderManager.h>

// Phonebook 2
#include "CPbk2SortOrderManagerImpl.h"

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::~CPbk2SortOrderManager
// --------------------------------------------------------------------------
//
CPbk2SortOrderManager::~CPbk2SortOrderManager()
    {
    delete iImpl;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2SortOrderManager* CPbk2SortOrderManager::NewL
        ( const MVPbkFieldTypeList& aMasterFieldTypeList, RFs* aFs )
    {
    CPbk2SortOrderManager* self = new (ELeave) CPbk2SortOrderManager;
    CleanupStack::PushL(self);
    self->ConstructL( aMasterFieldTypeList, aFs );
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SortOrderManager::ConstructL( 
        const MVPbkFieldTypeList& aMasterFieldTypeList, RFs* aFs )
    {
    iImpl = CPbk2SortOrderManagerImpl::NewL( aMasterFieldTypeList, aFs );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::SetContactViewL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SortOrderManager::SetContactViewL
        (MVPbkContactViewBase& aContactView)
    {
    iImpl->SetContactViewL( aContactView );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::AddObserverL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SortOrderManager::AddObserverL
        (MPbk2SortOrderObserver& aObserver)
    {
    iImpl->AddObserverL( aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SortOrderManager::RemoveObserver
        (MPbk2SortOrderObserver& aObserver)
    {
    iImpl->RemoveObserver( aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::SetNameDisplayOrderL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2SortOrderManager::SetNameDisplayOrderL
        ( TPbk2NameDisplayOrder aNameDisplayOrder, const TDesC& aSeparator )
    {
    iImpl->SetNameDisplayOrderL( aNameDisplayOrder, aSeparator );
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::NameDisplayOrder
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2SortOrderManager::TPbk2NameDisplayOrder
        CPbk2SortOrderManager::NameDisplayOrder() const
    {
    return iImpl->NameDisplayOrder();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::SortOrder
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkFieldTypeList& CPbk2SortOrderManager::SortOrder() const
    {
    return iImpl->SortOrder();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::DefaultSeparator
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2SortOrderManager::DefaultSeparator()
    {
    return iImpl->DefaultSeparator();
    }

// --------------------------------------------------------------------------
// CPbk2SortOrderManager::CurrentSeparator
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2SortOrderManager::CurrentSeparator() const
    {
    return iImpl->CurrentSeparator();
    }

// End of File
