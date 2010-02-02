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
*
*/


// INCLUDE FILES
#include    <CPbkAppUiBase.h>  // This class' declaration
#include    "CPbkDocumentBase.h"    // Phonebook document class
#include    <CPbkAppViewBase.h>     // Phonebook application view
#include    <CPbkCommandStore.h>
#include    <MPbkCommandList.h>

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkAppUiBase::CPbkAppUiBase()
    {
    }

EXPORT_C CPbkAppUiBase::~CPbkAppUiBase()
    {
    delete iCommandStore;
    }

EXPORT_C CPbkDocumentBase* CPbkAppUiBase::PbkDocument() const
    {
    // Explicit cast: the document must always be of type CPbkDocumentBase
    return static_cast<CPbkDocumentBase*>(Document());
    }

EXPORT_C CPbkAppViewBase* CPbkAppUiBase::ActiveView() const
    {
    // Explicit cast: iView must always be of type CPbkAppViewBase.
    return static_cast<CPbkAppViewBase*>(iView);
    }

EXPORT_C void CPbkAppUiBase::AddAndExecuteCommandL(MPbkCommand* aCommand)
    {
    if (!iCommandStore)
        {
        iCommandStore = CPbkCommandStore::NewL();
        }
    iCommandStore->AddAndExecuteCommandL(aCommand);
    }

//  End of File  
