/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An interface for the sort policy used by the virtual phonebook
*                stores.
*
*/


#ifndef MVPBKCONTACTVIEWSORTPOLICY_H
#define MVPBKCONTACTVIEWSORTPOLICY_H

// INCLUDES
#include <e32cmn.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeList;
class MVPbkViewContact;

// DATA TYPES

/**
 * Type of a sort key.
 */
enum TVPbkSortKeyType
    {
    /// Basic sort key
    EVPbkSortKeyBasic,
    /// Pronounciation sort key
    EVPbkSortKeyPronounciation
    };

// CLASS DECLARATION

/**
 * Sort key. A pair of text and TVPbkSortKeyType.
 */
class TVPbkSortKey
    {
    public:
        inline TVPbkSortKey(const TDesC& aText, TVPbkSortKeyType aType) :
            iText(aText), iType(aType) { }

        inline const TDesC& Text() const { return iText; }
        inline TVPbkSortKeyType Type() const { return iType; }

    private:
        ///Ref: sort key text
        TPtrC iText;
        ///Own: Sort key type
        TVPbkSortKeyType iType;
        ///Own: extension point
        TAny* iExtension;
    };

/**
 * An abstract array of TVPbkSortKey objects.
 */
class MVPbkSortKeyArray
    {
    public:
        /**
         * Destructor.
         */
        virtual ~MVPbkSortKeyArray() { }

        /**
         * Returns the array count.
         * @return Array count
         */
        virtual TInt Count() const = 0;

        /**
         * Return the sort key at aIndex.
         * @param aIndex Index of sorted key
         * @return Sort key
         */
        virtual TVPbkSortKey SortKeyAt( TInt aIndex ) const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* SortKeyArrayExtension( TUid /*aExtensionUid*/ ) 
                                           { return NULL; }
        };

/**
 * Virtual Phonebook Contact view sort policy.
 */
class MVPbkContactViewSortPolicy
    {
    public:
        virtual ~MVPbkContactViewSortPolicy() {}

        /**
         * Sets new sort order for the policy.
         * @param aSortOrder New sort order.
         */
        virtual void SetSortOrderL( const MVPbkFieldTypeList& aSortOrder ) = 0;

        /**
         * Indicates that the sort is about to start.
         * @precond SetSortOrderL has been called.
         */
        virtual void SortStartL() = 0;

        /**
         * Indicates that the sort has completed.
         */
        virtual void SortCompleted() = 0;

        /**
         * Compares two view contacts.
         * @param aLhs Left hand side of the comparison.
         * @param aRhs Right hand side of the comparison.
         * @return <0 if aLhs comes first,
         *         >0 if aRhs comes first,
         *         0 if contacts are equal in this sort policy
         */
        virtual TInt CompareContacts( const MVPbkViewContact& aLhs, 
                                     const MVPbkViewContact& aRhs ) = 0;
        /**
         * Compares two MVPbkSortKeyArray.
         * @see MVPbkSortKeyArray
         *
         * @param aLhs  the left-hand-side sort key array.
         * @param aRhs  the right-hand-side sort key array.
         * @return  0  if aLhs and aRhs have equal sorting order.
         *         >0  if aLhs is sorted after aRhs
         *         <0  if aLhs is sorted before aRhs.
         */
        virtual TInt CompareItems( const MVPbkSortKeyArray& aLhs, 
                                  const MVPbkSortKeyArray& aRhs ) const = 0;

        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension
         * @return Extension point or NULL
         */
        virtual TAny* ContactViewSortPolicyExtension( TUid /*aExtensionUid*/ ) 
                                                    { return NULL; }
    };

#endif // MVPBKCONTACTVIEWSORTPOLICY_H

//End of file
