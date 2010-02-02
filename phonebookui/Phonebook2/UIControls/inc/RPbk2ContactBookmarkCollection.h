/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  an implementation of MVPbkContactBookmarkCollection
*
*/


#ifndef R_RPBK2CONTACTBOOKMARKCOLLECTION_H
#define R_RPBK2CONTACTBOOKMARKCOLLECTION_H

#include <e32base.h>
#include <MVPbkContactBookmarkCollection.h>
#include <MVPbkContactBookmark.h>

/**
 *  A class that implements MVPbkContactBookmarkCollection interface
 *
 */
class RPbk2ContactBookmarkCollection : public MVPbkContactBookmarkCollection
    {
public:
    /**
     * Constructor
     */
    RPbk2ContactBookmarkCollection() {}

    /**
     * Appends a new bookmark to the collection
     *
     * @param aBookmark the new bookmark whose ownership is taken.
     */
    inline void AppendL( MVPbkContactBookmark* aBookmark );

    /**
     * Frees all resources
     */
    inline void Close();
    
    /**
     * Resets the array and deletes all the bookmarks
     */
    inline void ResetAndDestroy();
    
    /**
     * This removes all bookmarks from the array and deletes them.
     * It doesn' free the array memory.
     */
    inline void RemoveAndDeleteAll();

// from base class MVPbkContactBookmarkCollection
    inline TInt Count() const;
    inline const MVPbkContactBookmark& At( TInt aIndex ) const;

private: // data
    /// Own: the bookmarks
    RPointerArray<MVPbkContactBookmark> iBookmarks;
    };

// INLINE FUNCTIONS
inline void RPbk2ContactBookmarkCollection::AppendL(
        MVPbkContactBookmark* aBookmark )
    {
    iBookmarks.AppendL( aBookmark );
    }

inline void RPbk2ContactBookmarkCollection::Close()
    {
    iBookmarks.ResetAndDestroy();
    }

inline void RPbk2ContactBookmarkCollection::ResetAndDestroy()
    {
    iBookmarks.ResetAndDestroy();
    }

inline void RPbk2ContactBookmarkCollection::RemoveAndDeleteAll()
    {
    const TInt count = iBookmarks.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        delete iBookmarks[i];
        iBookmarks.Remove( i );
        }
    }

inline TInt RPbk2ContactBookmarkCollection::Count() const
    {
    return iBookmarks.Count();
    }

inline const MVPbkContactBookmark& RPbk2ContactBookmarkCollection::At(
        TInt aIndex ) const
    {
    return *iBookmarks[aIndex];
    }
#endif // R_RPBK2CONTACTBOOKMARKCOLLECTION_H
