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
*     Fetch dialog page for Phonebook.
*
*/


#ifndef __MPbkFetchDlg_H__
#define __MPbkFetchDlg_H__

//  INCLUDES
#include <e32std.h>
#include <cntdef.h>

//  FORWARD DECLARATIONS
class CCoeControl;
class CContactViewBase;
class CPbkContactIdSet;
class CPbkContactEngine;
class MPbkFetchDlgPage;

// CLASS DECLARATION

/**
 * Phonebook fetch dialog interface. Used by MPbkFetchDlgPage implementation 
 * objects.
 * @see MPbkFetchDlgPage
 */
class MPbkFetchDlg
    {
    public:
        /**
         * Returns a dialog control with id aCtrlId.
         */
        virtual CCoeControl* FetchDlgControl(TInt aCtrlId) const = 0;

        /**
         * Returns the contact view to use for names list contents.
         */
        virtual CContactViewBase& FetchDlgNamesView() const = 0;

        /**
         * Returns the current fetch dialog selection set.
         */
        virtual CPbkContactIdSet& FetchDlgSelection() = 0;

        /**
         * Displays an async error aError note in the UI.
         */
        virtual void FetchDlgHandleError(TInt aError) = 0;

        /**
         * Returns the rectangle to use for control.
         */
        virtual TRect FetchDlgClientRect() const = 0;

        /**
         * Returns the Phonebook engine.
         */
        virtual CPbkContactEngine& PbkEngine() = 0;

        /**
         * Called by a dialog page aPage when its state or contents
		 * has changed.
         */
        virtual void FetchDlgPageChangedL(MPbkFetchDlgPage& aPage) = 0;        

    protected:
        /**
         * Protected destructor. Concrete fetch dialog objects are not
         * destroyed through this interface.
         */
        virtual ~MPbkFetchDlg() { }
    };

#endif // __MPbkFetchDlg_H__

// End of File
