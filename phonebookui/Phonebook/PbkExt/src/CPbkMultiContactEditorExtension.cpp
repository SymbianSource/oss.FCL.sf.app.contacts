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
*       Composite contact editor extension.
*
*/


// INCLUDE FILES
#include "CPbkMultiContactEditorExtension.h"
#include "ForEachUtil.h"

const TInt KExtensionGranularity = 1;

// ==================== MEMBER FUNCTIONS ====================
CPbkMultiContactEditorExtension::CPbkMultiContactEditorExtension() :
    iContactEditors(KExtensionGranularity)
    {
    }

CPbkMultiContactEditorExtension::~CPbkMultiContactEditorExtension()
    {
    // This really cannot leave since this is a template
    // method and actually Release method is called
    ForEachL(iContactEditors,
             &Release);
    iContactEditors.Reset();
    }

CPbkMultiContactEditorExtension* CPbkMultiContactEditorExtension::NewL()
    {
    CPbkMultiContactEditorExtension* self = 
        new (ELeave) CPbkMultiContactEditorExtension;
    return self;
    }

void CPbkMultiContactEditorExtension::AppendL
        (MPbkContactEditorExtension* aContactEditor)
    {
    iContactEditors.AppendL(aContactEditor);
    }

void CPbkMultiContactEditorExtension::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    ForEachL(iContactEditors,
             VoidMemberFunction(&MPbkContactEditorExtension::DynInitMenuPaneL),
             aResourceId,
             aMenuPane);
    }

TBool CPbkMultiContactEditorExtension::ProcessCommandL(TInt aCommandId)
    {
    return TryEachL(iContactEditors,
                    MemberFunction(&MPbkContactEditorExtension::ProcessCommandL),
                    aCommandId);
    }
        
void CPbkMultiContactEditorExtension::PreOkToExitL(TInt aButtonId)
    {
    ForEachL(iContactEditors,
                    VoidMemberFunction(&MPbkContactEditorExtension::PreOkToExitL),
                    aButtonId);
    }

void CPbkMultiContactEditorExtension::PostOkToExitL(TInt aButtonId)
    {
    ForEachL(iContactEditors,
             VoidMemberFunction(&MPbkContactEditorExtension::PostOkToExitL),
             aButtonId);
    }

void CPbkMultiContactEditorExtension::DoRelease()
    {
    delete this;
    }
