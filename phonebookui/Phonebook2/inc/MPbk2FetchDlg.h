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
* Description:  Phonebook 2 fetch dialog interface.
*
*/


#ifndef MPBK2FETCHDLG_H
#define MPBK2FETCHDLG_H

//  INCLUDES
#include <e32std.h>
#include <MPbk2ControlObserver.h>

//  FORWARD DECLARATIONS
class CCoeControl;
class MVPbkContactViewBase;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MPbk2FetchDlgPage;
class MPbk2FetchDlgObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog interface.
 */
class MPbk2FetchDlg : public MPbk2ControlObserver
    {
    public: // Interface

        /**
         * Returns a dialog control with id aCtrlId.
         *
         * @param aCtrlId   Control id.
         * @return  Dialog control.
         */
        virtual CCoeControl* FetchDlgControl(
                TInt aCtrlId ) const = 0;

        /**
         * Returns the contact view used.
         *
         * @param aControlId    Control id, used to determine which
         *                      view to return.
         * @return  The contact view.
         */
        virtual MVPbkContactViewBase& FetchDlgViewL(
                TInt aControlId ) const = 0;

        /**
         * Returns the contact view used.
         *
         * @param aControlId    Control id, used to determine which
         *                      view to set.
         * @param aView         The view to set.
         */
        virtual void SetFetchDlgViewL(
                TInt aControlId,
                MVPbkContactViewBase& aView ) = 0;

        /**
         * Returns the current fetch dialog selection set.
         *
         * @return  Selected contacts.
         */
        virtual MVPbkContactLinkArray& FetchDlgSelection() = 0;

        /**
         * Displays an async error aError note in the UI.
         *
         * @param aError    The error code.
         */
        virtual void FetchDlgHandleError(
                TInt aError ) = 0;

        /**
         * Returns the rectangle to use for the control.
         *
         * @return  Rectangle area.
         */
        virtual TRect FetchDlgClientRect() const = 0;

        /**
         * Called by a dialog page aPage when its state or contents
         * has changed.
         *
         * @param aPage     The page whose contents have changed.
         */
        virtual void FetchDlgPageChangedL(
                MPbk2FetchDlgPage& aPage ) = 0;

        /**
         * Returns the fetch dialog observer
         *
         * @return  Observer.
         */
        virtual MPbk2FetchDlgObserver& FetchDlgObserver() const = 0;

        /**
         * Causes the selection or deselection of given contact.
         *
         * @param aLink         The contact to select/deselect.
         * @param aSelected     ETrue to select contact,
         *                      EFalse to deselect contact.
         */
        virtual void SelectContactL(
                const MVPbkContactLink& aLink,
                TBool aSelected ) = 0;

    protected: // Protected functions

        /**
         * Protected destructor. Concrete fetch dialog objects are not
         * destroyed through this interface.
         */
        virtual ~MPbk2FetchDlg()
                {}
    };

#endif // MPBK2FETCHDLG_H

// End of File
