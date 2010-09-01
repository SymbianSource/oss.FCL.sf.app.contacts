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
*       Composite view extension.
*
*/


// INCLUDE FILES
#include "CPbkMultiViewExtension.h"
#include "ForEachUtil.h"

#include <eikmenup.h>

// CONSTANTS
const TInt KExtensionGranularity = 1;

// ==================== MEMBER FUNCTIONS ====================
CPbkMultiViewExtension::CPbkMultiViewExtension() :
    iViews(KExtensionGranularity)
    {
    }

CPbkMultiViewExtension::~CPbkMultiViewExtension()
    {
    // This really cannot leave since this is a template
    // method and actually Release method is called
    ForEachL(iViews,
             &Release);
    iViews.Reset();
    }

CPbkMultiViewExtension* CPbkMultiViewExtension::NewL()
    {
    CPbkMultiViewExtension* self = new (ELeave) CPbkMultiViewExtension;
    return self;
    }

void CPbkMultiViewExtension::AppendL(MPbkViewExtension* aView)
    {
    iViews.AppendL(aView);
    }

void CPbkMultiViewExtension::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    ForEachL(iViews,
             VoidMemberFunction(&MPbkViewExtension::DynInitMenuPaneL),
             aResourceId,
             aMenuPane);
    }

TBool CPbkMultiViewExtension::HandleCommandL(TInt aCommandId)
    {
    typedef TBool (MPbkViewExtension::* HandleCommandPtr)(TInt);
    HandleCommandPtr handleFunc = &MPbkViewExtension::HandleCommandL;
    return TryEachL(iViews,
                    MemberFunction(handleFunc),
                    aCommandId);
    }

TBool CPbkMultiViewExtension::HandleCommandL
        (TInt aCommandId, MPbkMenuCommandObserver& aObserver)
    {
    typedef TBool (MPbkViewExtension::* HandleCommandPtr)
        (TInt,MPbkMenuCommandObserver&);
    HandleCommandPtr handleFunc = &MPbkViewExtension::HandleCommandL;
    return TryEachL(iViews,
                    MemberFunction(handleFunc),
                    aCommandId,
                    aObserver);
    }

void CPbkMultiViewExtension::SetContactUiControl
        (MPbkContactUiControl* aContactControl)
    {
    // This can't leave because the called method is 
    // actually MPbkViewExtension::SetContactUiControl
    ForEachL(iViews,
             VoidMemberFunction(&MPbkViewExtension::SetContactUiControl),
             aContactControl);
    }

void CPbkMultiViewExtension::DoRelease()
    {
    delete this;
    }

