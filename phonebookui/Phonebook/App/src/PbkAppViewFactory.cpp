/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*          Provide factory methods for Phonebook's app views.
*
*/


// INCLUDE FILES
#include    "PbkAppViewFactory.h"
#include    <Phonebook.hrh>
#include    "CPbkNamesListAppView.h"
#include    "CPbkGroupsListAppView.h"
#include    "CPbkGroupMembersListAppView.h"
#include    "CPbkContactInfoAppView.h"

// From ViewInc

// From ExtInc
#include    <CPbkExtGlobals.h>
#include    <MPbkExtensionFactory.h>


// MODULE DATA STRUCTURES

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicUnknownViewId = 1,
    EPanicViewIdMismatch
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAppViewFactory");
    User::Panic(KPanicText,aReason);
    }

#endif

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkAppViewFactory::CPbkAppViewFactory()
    {
    }

inline void CPbkAppViewFactory::ConstructL()
    {
    iExtGlobal = CPbkExtGlobals::InstanceL();
    }

CPbkAppViewFactory* CPbkAppViewFactory::NewL()
    {
    CPbkAppViewFactory* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAppViewFactory* CPbkAppViewFactory::NewLC()
    {
    CPbkAppViewFactory* self = new (ELeave) CPbkAppViewFactory;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CPbkAppViewFactory::~CPbkAppViewFactory()
    {
    Release(iExtGlobal);    
    }

CAknView* CPbkAppViewFactory::CreateAppViewL
        (TUid aId)
    {
    CAknView* view = NULL;    
    view = iExtGlobal->FactoryL().CreateViewL(aId);

    if (!view)
        {
        // Create the view object
        switch (aId.iUid) 
            {
            case EPbkNamesListViewId:
                {
                view = CPbkNamesListAppView::NewL();
                break;
                }
                
            case EPbkGroupsListViewId:
                {
                view = CPbkGroupsListAppView::NewL();
                break;
                }
                
            case EPbkGroupMembersListViewId:
                {
                view = CPbkGroupMembersListAppView::NewL();
                break;
                }
                
            case EPbkContactInfoViewId:
                {
                view = CPbkContactInfoAppView::NewL();
                break;
                }
            }
        }

    __ASSERT_DEBUG(view, Panic(EPanicUnknownViewId));
    __ASSERT_DEBUG(view->Id()==aId, Panic(EPanicViewIdMismatch));

    // Return the created view object
    return view;
    }

CAknView* CPbkAppViewFactory::CreateAppViewLC
        (TUid aId)
    {
    CAknView* view = CreateAppViewL(aId);
    CleanupStack::PushL(view);
    return view;
    }


//  End of File  
