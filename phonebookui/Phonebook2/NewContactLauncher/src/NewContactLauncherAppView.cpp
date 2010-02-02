/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Application view implementation
*
*/

// INCLUDE FILES
#include <coemain.h>
#include "NewContactLauncherAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppView* CNewContactLauncherAppView::NewL(
        const TRect& aRect)
    {
    CNewContactLauncherAppView* self = CNewContactLauncherAppView::NewLC(
            aRect);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppView* CNewContactLauncherAppView::NewLC(
        const TRect& aRect)
    {
    CNewContactLauncherAppView* self =
            new (ELeave) CNewContactLauncherAppView;
    CleanupStack::PushL(self);
    self->ConstructL(aRect);
    return self;
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::CNewContactLauncherAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppView::CNewContactLauncherAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::~CNewContactLauncherAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CNewContactLauncherAppView::~CNewContactLauncherAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect drawRect(Rect());

    // Clears the screen
    gc.Clear(drawRect);

    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppView::SizeChanged()
    {
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CNewContactLauncherAppView::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {

    // Call base class HandlePointerEventL()
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }

// End of File
