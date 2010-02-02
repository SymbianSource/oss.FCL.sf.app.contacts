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
* Description: Declares view class for application.
*
*/

#ifndef __NEWCONTACTLAUNCHERAPPVIEW_h__
#define __NEWCONTACTLAUNCHERAPPVIEW_h__

// INCLUDES
#include <coecntrl.h>

// CLASS DECLARATION
class CNewContactLauncherAppView : public CCoeControl
    {
public:
    // New methods

    /**
     * NewL.
     * Two-phased constructor.
     * Create a CNewContactLauncherAppView object, which will draw itself to aRect.
     * @param aRect The rectangle this view will be drawn to.
     * @return a pointer to the created instance of CNewContactLauncherAppView.
     */
    static CNewContactLauncherAppView* NewL(const TRect& aRect);

    /**
     * NewLC.
     * Two-phased constructor.
     * Create a CNewContactLauncherAppView object, which will draw itself
     * to aRect.
     * @param aRect Rectangle this view will be drawn to.
     * @return A pointer to the created instance of CNewContactLauncherAppView.
     */
    static CNewContactLauncherAppView* NewLC(const TRect& aRect);

    /**
     * ~CNewContactLauncherAppView
     * Virtual Destructor.
     */
    virtual ~CNewContactLauncherAppView();

public:
    // Functions from base classes

    /**
     * From CCoeControl, Draw
     * Draw this CNewContactLauncherAppView to the screen.
     * @param aRect the rectangle of this view that needs updating
     */
    void Draw(const TRect& aRect) const;

    /**
     * From CoeControl, SizeChanged.
     * Called by framework when the view size is changed.
     */
    virtual void SizeChanged();

    /**
     * From CoeControl, HandlePointerEventL.
     * Called by framework when a pointer touch event occurs.
     * Note: although this method is compatible with earlier SDKs, 
     * it will not be called in SDKs without Touch support.
     * @param aPointerEvent the information about this event
     */
    virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

private:
    // Constructors

    /**
     * ConstructL
     * 2nd phase constructor.
     * Perform the second phase construction of a
     * CNewContactLauncherAppView object.
     * @param aRect The rectangle this view will be drawn to.
     */
    void ConstructL(const TRect& aRect);

    /**
     * CNewContactLauncherAppView.
     * C++ default constructor.
     */
    CNewContactLauncherAppView();

    };

#endif // __NEWCONTACTLAUNCHERAPPVIEW_h__
// End of File
