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
* Description:
*
*/


#ifndef T_ContactViewObserver_H
#define T_ContactViewObserver_H

//  EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "MVPbkContactViewObserver.h"

//  FORWARD DECLARATIONS
class MVPbkContactViewBase;
class MVPbkContactViewBookmark;

//  CLASS DEFINITION
/**
 *
 * Contact view observer.
 *
 */
class T_ContactViewObserver
     :  public MVPbkContactViewObserver
    {
    public:     // Constructors and destructors

        T_ContactViewObserver() { }

    private: // From MVPbkContactViewObserver
        void ContactViewReady(MVPbkContactViewBase& aView);
        void ContactViewUnavailable(MVPbkContactViewBase& aView);
        void ContactAddedToView(MVPbkContactViewBase& aView, TInt aIndex, const MVPbkContactLink& aBookmark);
        void ContactRemovedFromView(MVPbkContactViewBase& aView, TInt aIndex, const MVPbkContactLink& aBookmark);
        void ContactViewError(MVPbkContactViewBase& aView, TInt aError, TBool aErrorNotified);

    public:    // Data
        TInt iLastError;
        enum TLastEvent
            {
            EContactViewReady,
            EContactViewUnavailable,
            EContactAddedToView,
            EContactRemovedFromView,
            EContactViewError
            } iLastEvent;
    };

#endif      //  T_ContactViewObserver_H

// End of file
