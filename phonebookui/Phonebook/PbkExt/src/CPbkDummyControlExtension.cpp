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
*       Dummy view extension, which does nothing.
*
*/


// INCLUDE FILES
#include "CPbkDummyControlExtension.h"


// ================= MEMBER FUNCTIONS =======================

CPbkDummyControlExtension::CPbkDummyControlExtension()
    {
    }

CPbkDummyControlExtension::~CPbkDummyControlExtension()
    {
    iIcons.Close();
    }

void CPbkDummyControlExtension::DoRelease()
    {
    }
        
const TArray<TPbkIconId> CPbkDummyControlExtension::
        GetDynamicIconsL(TContactItemId /*aContactId*/)
    {
    return iIcons.Array();
    }


const TArray<TPbkIconId> CPbkDummyControlExtension::GetDynamicIconsL
        (const CPbkContactItem& /*aContact*/)
    {
    return iIcons.Array();
    }

void CPbkDummyControlExtension::IconArrayResourceId
        (TInt& aArrayIconInfoId, TInt& aArrayIconId)
    {
    aArrayIconInfoId = 0;
    aArrayIconId = 0;
    }

void CPbkDummyControlExtension::SetContactUiControlUpdate
        (MPbkContactUiControlUpdate* /*aContactUpdator*/)
    {
    }

// End of File
