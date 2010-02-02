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
*     Find matching primitives interface for CPbkContactFindView.
*
*/


#ifndef __MPbkFindPrimitives_H__
#define __MPbkFindPrimitives_H__

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Find matching primitives interface for CPbkContactFindView.
 */
class MPbkFindPrimitives
    {
    public:
        /**
         * Destructor. 
         */
        virtual ~MPbkFindPrimitives() { }

        /**
         * Sets the Contact View sort order. The sort order is needed to
         * interpret CViewContact field types in IsFindMatchL.
         * This function must be called before IsFindMatchL is called.
		 * @param aSortOrder the sort order to set
         */
        virtual void SetContactViewSortOrderL
            (const RContactViewSortOrder& aSortOrder) =0;

        /**
         * Matches a find text to a view contact.
         * @param aViewContact  view contact to search for aFindText
         * @param aFindText     text to search from aTargetText
         * @return ETrue if aFindText matches aTargetText, EFalse otherwise.
         */
        virtual TBool IsFindMatchL
            (const CViewContact& aViewContact, const TDesC& aFindText) =0;

        /**
         * Returns initial set of matching contacts from a Contact View.
         *
         * @param aView             contact view where contacts are searched 
         *                          from.
         * @param aFindText         find text.
         * @param aMatchedContacts  array filled with matching contacts.
         */
        virtual void GetInitialMatchesL
            (CContactViewBase& aView,
            const TDesC& aFindText,
            RPointerArray<CViewContact>& aMatchedContacts) =0;

        /**
         * Returns true if text matches the initial find text passed to
         * GetInitialMatchesL.
         *
         * @param aText the text to match.
         * @return ETrue if aText matches the initial find text.
         */
        virtual TBool MatchesInitialFindTextL(const TDesC& aText) =0;

        /**
         * Returns true if a contact matches the initial find text passed to
         * GetInitialMatchesL.
         *
         * @param aViewContact  the view contact to match
         * @return ETrue if aViewContact matches the initial find text.
         * @precond GetInitialMatchesL has been called succesfully.
         */
        virtual TBool MatchesInitialFindTextL
            (const CViewContact& aViewContact) =0;
    };

#endif // __MPbkFindPrimitives_H__

// End of File
