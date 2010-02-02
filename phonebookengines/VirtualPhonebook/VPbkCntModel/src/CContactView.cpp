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
* Description:  Contacts Model store contact view implementation.
*
*/


#include "CContactView.h"

// VPbkCntModel
#include "CContactStore.h"
#include "CFieldFactory.h"
#include "CContact.h"
#include "CFieldTypeList.h"
#include "CContactLink.h"
#include "VPbkCntModelRemoteViewPreferences.h"
#include "NamedRemoteViewViewDefinitionStoreUtility.h"

// VPbkEng
#include <MVPbkContactViewObserver.h>
#include <MVPbkFieldType.h>
#include <VPbkError.h>
#include <CVPbkSortOrder.h>
#include <CVPbkAsyncCallback.h>

// System includes
#include <cntitem.h>


namespace VPbkCntModel {

/// Unnamed namespace for local definitions
namespace {

} /// namespace

// --------------------------------------------------------------------------
// CContactView::CContactView
// --------------------------------------------------------------------------
//
inline CContactView::CContactView( 
        CContactStore& aParentStore ) :
    CViewBase( aParentStore )
    {
    }

// --------------------------------------------------------------------------
// CContactView::~CContactView
// --------------------------------------------------------------------------
//
CContactView::~CContactView()
    {
    }

// --------------------------------------------------------------------------
// CContactView::NewLC
// --------------------------------------------------------------------------
//
CContactView* CContactView::NewLC(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        CContactStore& aParentStore, 
        const MVPbkFieldTypeList& aSortOrder )
    {
    CContactView* self = new ( ELeave ) CContactView( aParentStore );
    CleanupStack::PushL( self );
    self->ConstructL( aViewDefinition, aObserver, aSortOrder );
    return self;
    }

// --------------------------------------------------------------------------
// CContactView::ConstructL
// --------------------------------------------------------------------------
//
void CContactView::ConstructL(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver, 
        const MVPbkFieldTypeList& aSortOrder )
    {
    CViewBase::ConstructL( aViewDefinition, aObserver, aSortOrder );
    }

// --------------------------------------------------------------------------
// CContactView::Type
// --------------------------------------------------------------------------
//
TVPbkContactViewType CContactView::Type() const
    {
    return EVPbkContactsView;
    }

// --------------------------------------------------------------------------
// CContactView::DoInitializeViewL
// --------------------------------------------------------------------------
//
void CContactView::DoInitializeViewL(
        const CVPbkContactViewDefinition& aViewDefinition,
        RContactViewSortOrder& aViewSortOrder )
    {
    if ( RemoteViewDefinition( aViewDefinition ) )
        {
        iRemoteView = CContactNamedRemoteView::NewL( 
                *this, RemoteViewName( aViewDefinition ), 
                Store().NativeDatabase(), aViewSortOrder, 
                KVPbkDefaultContactViewPrefs );
                
        iView = iRemoteView;
        }
    else
        {
        iView = CContactLocalView::NewL( *this, Store().NativeDatabase(), 
                aViewSortOrder, KVPbkDefaultContactViewPrefs );
        }
    }

// --------------------------------------------------------------------------
// CContactView::DoTeardownView
// --------------------------------------------------------------------------
//
void CContactView::DoTeardownView()
    {
    /// Do nothing
    }

// --------------------------------------------------------------------------
// CContactView::DoChangeSortOrderL
// --------------------------------------------------------------------------
//
TBool CContactView::DoChangeSortOrderL(
        const CVPbkContactViewDefinition& aViewDefinition,
        RContactViewSortOrder& aSortOrder )
    {
    TBool canBeChanged = ETrue;
    if ( iRemoteView )
        {
        if ( RemoteViewName( aViewDefinition ).Compare( 
                KVPbkAllContactsViewName ) == 0 )
            {
            // Set Contacts Model default view setting only if client
            // is using KVPbkAllContactsViewName shared view.
            NamedRemoteViewViewDefinitionStoreUtility::
                SetNamedRemoteViewViewDefinitionL(
                    KVPbkAllContactsViewName, aSortOrder, 
                    KVPbkDefaultContactViewPrefs);
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
