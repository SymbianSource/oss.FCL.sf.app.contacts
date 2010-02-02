/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for contact find used by the virtual phonebook
*
*/


#ifndef MVPBKCONTACTFINDPOLICY_H
#define MVPBKCONTACTFINDPOLICY_H

// INCLUDES
#include <e32cmn.h>
#include <e32std.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MVPbkBaseContact;

/**
 * Virtual Phonebook contact find policy interface.
 */
class MVPbkContactFindPolicy
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MVPbkContactFindPolicy() {}

        /**
         * Checks whether the specified text matches with
         * the specified search string.
         *
         * @param aText          The text to check.
         * @param aSearchString  Search string.
         * @return ETrue if there is a match, EFalse otherwise.
         */
	    virtual TBool Match(
            const TDesC& aText,
            const TDesC& aSearchString ) = 0;

        /**
         * Checks whether the specified text matches partially with the
         * specified search string. In other words, this method processes
         * pattern matching with the strings.
         *
         * @param aText The word to test.
         * @param aSearchString  Search string.
         * @return ETrue if there is a match, EFalse otherwise.
         */
	    virtual TBool MatchRefineL(
            const TDesC& aText,
            const TDesC& aSearchString ) = 0;

        /**
         * Checks whether the specified text is valid for matching.
         *
         * @param aWord The text to check.
         * @return ETrue if the text is valid, EFalse otherwise.
         */
	    virtual TBool IsWordValidForMatching(
            const TDesC& aWord ) = 0;

        /**
         * This is a higer level matching service. Matches contact name to
         * the find words according to the S60 Phonebook rules.
         *
         * @param aFindWords The array of find words.
         * @param aContact The contact for matching.
         * @return ETrue if aContact matches to aFindWords, EFalse otherwise.
         * @exception KErrNotSupported if the implementation has been
         *            constructed only for low level functionality. See
         *            construction of CVPbkContactFindPolicy.h
         */
        virtual TBool MatchContactNameL( const MDesCArray& aFindWords,
            const MVPbkBaseContact& aContact ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactFindPolicyExtension( TUid /*aExtensionUid*/ )
                                                { return NULL; }
    };

#endif // MVPBKCONTACTFINDPOLICY_H

// End of File
