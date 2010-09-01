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
*       Provides additional Phonebook Application view methods.
*
*/


// INCLUDE FILES
#include    "CPbkAppView.h"     // This class' declaration
#include    "CPbkAppUi.h"       // Phonebook application UI class
#include    "CPbkDocument.h"    // Phonebook application document class

// ================= MEMBER FUNCTIONS =======================

CPbkAppView::CPbkAppView()
    {
    }

CPbkAppView::~CPbkAppView()
    {    
    }

CPbkAppUi* CPbkAppView::PbkAppUi() const
    {
    // Explicit cast: the app ui must always be of type CPbkAppUi.
    return static_cast<CPbkAppUi*>(CAknView::AppUi());
    }

//  End of File  
