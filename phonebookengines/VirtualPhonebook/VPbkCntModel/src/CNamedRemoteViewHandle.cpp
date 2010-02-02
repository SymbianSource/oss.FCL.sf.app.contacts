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
* Description:  The virtual phonebook named remote view .
*
*/


#include "CNamedRemoteViewHandle.h"

// VPbkCntModel
#include "NamedRemoteViewViewDefinitionStoreUtility.h"

// VPbkEng

// System includes


namespace VPbkCntModel {

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode 
    {
    EPanicPreCond_CreateRemoteViewL = 1
    };

void Panic ( TPanicCode aReason )
    {
    _LIT(KPanicText, "CNamedRemoteViewHandle");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CNamedRemoteView::CNamedRemoteViewHandle
// --------------------------------------------------------------------------
//
inline CNamedRemoteViewHandle::CNamedRemoteViewHandle()
    {
    }

// --------------------------------------------------------------------------
// CNamedRemoteView::ConstructL
// --------------------------------------------------------------------------
//
inline void CNamedRemoteViewHandle::ConstructL()
    {
    }

// --------------------------------------------------------------------------
// CNamedRemoteViewHandle::NewLC
// --------------------------------------------------------------------------
//
CNamedRemoteViewHandle* CNamedRemoteViewHandle::NewLC()
    {
    CNamedRemoteViewHandle* self = new ( ELeave ) CNamedRemoteViewHandle;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CNamedRemoteViewHandle::~CNamedRemoteViewHandle
// --------------------------------------------------------------------------
//
CNamedRemoteViewHandle::~CNamedRemoteViewHandle()
    {
    if (iRemoteViewHandle)
        {
        iRemoteViewHandle->Close(*this);
        }
    }

// --------------------------------------------------------------------------
// CNamedRemoteViewHandle::SetRemoteViewHandle
// --------------------------------------------------------------------------
//
void CNamedRemoteViewHandle::CreateRemoteViewL(
        const TDesC& aViewName,
        CContactDatabase& /*aDatabase*/,
        RContactViewSortOrder aSortOrder,
        TContactViewPreferences aViewPreferences)
    {
    __ASSERT_DEBUG(!iRemoteViewHandle, Panic(EPanicPreCond_CreateRemoteViewL));

    NamedRemoteViewViewDefinitionStoreUtility::SetNamedRemoteViewViewDefinitionL(
        aViewName, aSortOrder, aViewPreferences);
    }

// --------------------------------------------------------------------------
// CNamedRemoteViewHandle::ViewName
// --------------------------------------------------------------------------
//
void CNamedRemoteViewHandle::HandleContactViewEvent(
        const CContactViewBase& /*aView*/, 
        const TContactViewEvent& /*aEvent*/ )
    {
    // Do nothing
    }

} // namespace VPbkCntModel

// End of File
