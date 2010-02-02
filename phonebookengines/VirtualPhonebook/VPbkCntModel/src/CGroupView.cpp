/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook group view
*
*/
 

#include "CGroupView.h"

// System includes
#include <cntitem.h>

// From VPbkEng
#include <MVPbkContactViewObserver.h>
#include <MVPbkFieldType.h>
#include <VPbkError.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include "NamedRemoteViewViewDefinitionStoreUtility.h"

// From VPbkCntModel
#include "VPbkCntModelRemoteViewPreferences.h"
#include "CContactStore.h"
#include "CFieldFactory.h"
#include "CContact.h"
#include "CFieldTypeList.h"
#include "CContactLink.h"

namespace VPbkCntModel {

inline CGroupView::CGroupView(
        CContactStore& aParentStore) :
    CViewBase(aParentStore)
    {
    }

void CGroupView::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver, 
        const MVPbkFieldTypeList& aSortOrder)
    {
    CViewBase::ConstructL(aViewDefinition, aObserver, aSortOrder);
    }

CGroupView* CGroupView::NewLC(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        CContactStore& aParentStore, 
        const MVPbkFieldTypeList& aSortOrder)
    {
    CGroupView* self = new(ELeave) CGroupView(aParentStore);
    CleanupStack::PushL(self);
    self->ConstructL(aViewDefinition, aObserver, aSortOrder);
    return self;
    }

CGroupView::~CGroupView()
    {
    }

TVPbkContactViewType CGroupView::Type() const
    {
    return EVPbkGroupsView;
    }

void CGroupView::DoInitializeViewL(
        const CVPbkContactViewDefinition& aViewDefinition,
        RContactViewSortOrder& aViewSortOrder)
    {
    if ( RemoteViewDefinition( aViewDefinition ) )
        {
        iRemoteView = CContactNamedRemoteView::NewL( 
                *this, RemoteViewName( aViewDefinition ), 
                Store().NativeDatabase(), aViewSortOrder, 
                KVPbkDefaultGroupsViewPrefs );
                
        iView = iRemoteView;
        }
    else
        {
        iView = CContactLocalView::NewL( *this, Store().NativeDatabase(), 
                aViewSortOrder, KVPbkDefaultGroupsViewPrefs );
        }
    }

void CGroupView::DoTeardownView()
    {
    }
    
TBool CGroupView::DoChangeSortOrderL(
        const CVPbkContactViewDefinition& aViewDefinition,
        RContactViewSortOrder& aSortOrder)
    {
    TBool canBeChanged = ETrue;
    if ( iRemoteView )
        {
        if ( RemoteViewName( aViewDefinition ).Compare( 
                KVPbkAllGroupsViewName ) == 0 )
            {
            // Set Contacts Model default view setting only if client
            // is using KVPbkAllGroupsViewName shared view.
            NamedRemoteViewViewDefinitionStoreUtility::
                SetNamedRemoteViewViewDefinitionL(
                    KVPbkAllGroupsViewName, aSortOrder, 
                    KVPbkDefaultGroupsViewPrefs);
            }
        iRemoteView->ChangeSortOrderL( aSortOrder );
        }
    else
        {
        // CContactLocalView doesn't support ChangeSortOrderL
        canBeChanged = EFalse;
        }
        
    return canBeChanged;
    }

} // namespace VPbkCntModel

// End of File
