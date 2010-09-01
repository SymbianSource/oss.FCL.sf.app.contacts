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
* Description:  Virtual Phonebook group members view
*
*/
 

#include "CGroupMembersView.h"

// System includes
#include <cntitem.h>
#include <cntview.h>

// From VPbkEng
#include <MVPbkContactViewObserver.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkError.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkContactViewDefinition.h>

// From VPbkCntModel
#include "CContactStore.h"
#include "CFieldFactory.h"
#include "CContact.h"
#include "CFieldTypeList.h"
#include "CContactLink.h"
#include "CContactView.h"

namespace VPbkCntModel {

CGroupMembersView::CGroupMembersView(
        CContactStore& aParentStore,
        TContactItemId aGroupId ) :
        CViewBase( aParentStore ),
        iGroupId( aGroupId )
    {
    }

void CGroupMembersView::ConstructL(
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    // For showing a group members it needs a contact view that contains
    // all the contacts
    // First create a handle to the "All Contacts" remote view.
    CVPbkContactViewDefinition* viewDefinition = 
            CVPbkContactViewDefinition::NewLC();
    viewDefinition->SetUriL( Store().StoreProperties().Uri().UriDes() );
    viewDefinition->SetType( EVPbkContactsView );
    viewDefinition->SetSharing( EVPbkSharedView );
    
    CViewBase* tempView = CContactView::NewLC( *viewDefinition, *this, Store(), 
        aSortOrder );
    CleanupStack::Pop();
    iBaseView = tempView;
    tempView = NULL;
    
    // Group members view is a local view built on iBaseView.
    // aSortOrder will be ignored because the sort order is defined
    // by iBaseView.
    viewDefinition->SetSharing( EVPbkLocalView );
    CViewBase::ConstructL( *viewDefinition, aObserver, aSortOrder );
    CleanupStack::PopAndDestroy( viewDefinition );
    }

CGroupMembersView* CGroupMembersView::NewLC(
        CContactStore& aParentStore,
        TContactItemId aGroupId,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    CGroupMembersView* self = new ( ELeave ) CGroupMembersView(
        aParentStore, aGroupId );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aSortOrder );
    return self;
    }

CGroupMembersView::~CGroupMembersView()
    {
    delete iBaseView;
    }

TVPbkContactViewType CGroupMembersView::Type() const
    {
    return EVPbkContactsView;
    }

void CGroupMembersView::DoInitializeViewL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& /*aViewSortOrder*/ )
    {
    iView = CContactGroupView::NewL(
            Store().NativeDatabase(),
            iBaseView->NativeView(),
            *this, 
            iGroupId,
            CContactGroupView::EShowContactsInGroup );
    }

void CGroupMembersView::DoTeardownView()
    {
    }

TBool CGroupMembersView::DoChangeSortOrderL( 
        const CVPbkContactViewDefinition& /*aViewDefinition*/,
        RContactViewSortOrder& aSortOrder )
    {
    // CContactGroupView doesn't support ChangeSortOrderL, but it's
    // built on iBaseView so the order must be changed from iBaseView.
    MVPbkFieldTypeList* newSortOrder = CFieldTypeList::NewLC(
            aSortOrder, Store().FieldTypeMap() );
    iBaseView->ChangeSortOrderL( *newSortOrder );
    CleanupStack::PopAndDestroy(); // newSortOrder
    return ETrue;
    }

} // namespace VPbkCntModel

// End of File
