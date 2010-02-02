/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility classes for VPbkAPITester
*
*/


#ifndef C_T_TESTSUITEUTILITIES_H
#define C_T_TESTSUITEUTILITIES_H

#include <e32base.h>
#include <MVPbkContactBookmarkCollection.h>

/**
 *  A class that implements MVPbkContactBookmarkCollection interface
 *
 */
class RContactBookmarkCollection : public MVPbkContactBookmarkCollection
    {
public:
    /** 
     * Constructor
     */
    RContactBookmarkCollection() {}
    
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
    
// from base class MVPbkContactBookmarkCollection
    inline TInt Count() const;
    inline const MVPbkContactBookmark& At( TInt aIndex ) const;
    
private: // data
    /// Own: the bookmarks
    RPointerArray<MVPbkContactBookmark> iBookmarks;
    };

// INLINE FUNCTIONS
inline void RContactBookmarkCollection::AppendL( 
        MVPbkContactBookmark* aBookmark )
    {
    iBookmarks.AppendL( aBookmark );
    }

inline void RContactBookmarkCollection::Close()
    {
    iBookmarks.ResetAndDestroy();
    }

inline TInt RContactBookmarkCollection::Count() const
    {
    return iBookmarks.Count();
    }
    
inline const MVPbkContactBookmark& RContactBookmarkCollection::At( 
        TInt aIndex ) const
    {
    return *iBookmarks[aIndex];
    }
#endif // C_T_TESTSUITEUTILITIES_H
