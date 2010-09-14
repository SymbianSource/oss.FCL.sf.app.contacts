/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:     Declares view for application.
*
*/






// INCLUDE FILES
#include <avkon.hrh>

#include <aknViewAppUi.h>

#include <speeddial.rsg>

#include "SpdiaBaseView.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSpdiaBaseView::CSpdiaBaseView
//
// ---------------------------------------------------------------------------
CSpdiaBaseView::CSpdiaBaseView()
    {
    }

// ---------------------------------------------------------------------------
// CSpdiaBaseView::ConstructL
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
void CSpdiaBaseView::ConstructL()
    {
    iStatusPane = AppUi()->StatusPane();
    }

// ---------------------------------------------------------------------------
// CSpdiaBaseView::~CSpdiaBaseView
// ?implementation_description
// ---------------------------------------------------------------------------
CSpdiaBaseView::~CSpdiaBaseView()
    {
  
    }

// end of file

