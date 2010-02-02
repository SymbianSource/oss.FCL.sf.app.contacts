/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 application view.
*
*/


// INCLUDE FILES
#include "CPbk2AppView.h"

// Phonebook 2
#include "CPbk2AppUi.h"


// --------------------------------------------------------------------------
// CPbk2AppView::CPbk2AppView
// --------------------------------------------------------------------------
//
CPbk2AppView::CPbk2AppView()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AppView::~CPbk2AppView
// --------------------------------------------------------------------------
//
CPbk2AppView::~CPbk2AppView()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AppView::Pbk2AppUi
// --------------------------------------------------------------------------
//
CPbk2AppUi* CPbk2AppView::Pbk2AppUi() const
    {
    // Explicit cast: the App UI must always be of type CPbkAppUi.
    return static_cast<CPbk2AppUi*>( CAknView::AppUi() );
    }


//  End of File
