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
* Description:  An API for sim views
*
*/



#ifndef MVPBKSIMCNTVIEW_H
#define MVPBKSIMCNTVIEW_H

//  INCLUDES
#include <e32std.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MVPbkSimViewObserver;
class MVPbkSimCntStore;
class RVPbkSimFieldTypeArray;
class MVPbkSimContact;
class MVPbkSimStoreOperation;
class MDesCArray;
class MVPbkSimViewFindObserver;

// CLASS DECLARATION

/**
*  An API for sim views
*
*/
class MVPbkSimCntView
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor. Use Close() to release resources
        */
        virtual ~MVPbkSimCntView() {}

    public: // New functions
        
        /**
        * Returns the name of the view so that the client can use
        * the identified view.
        * @return the name of the view or KNullDesC
        */
        virtual const TDesC& Name() const = 0;
        
        /**
        * Returns the parent store of this view
        * @return the parent store of this view
        */ 
        virtual MVPbkSimCntStore& ParentStore() const = 0;

        /**
        * Starts sorting the view.
        * @param aObserver the observer that is notified after view is ready
        */
        virtual void OpenL( MVPbkSimViewObserver& aObserver ) = 0;
        
        /**
        * Releases resources.
        * @param aObserver the observer to be removed. The view must not be
        *        accessed after this
        */
        virtual void Close( MVPbkSimViewObserver& aObserver ) = 0;
        
        /**
        * Returns the amount of contacts in the view
        * @return the amount of contacts in the view
        */
        virtual TInt CountL() const = 0;

        /**
        * Returns the view contact in the specified location. View contact
        * contains fields that are defined in the sort order
        * @param aIndex the view index of the contact
        * @return the contact in the specified location. The reference is
        *         valid only until next ContactAtL call!
        */
        virtual MVPbkSimContact& ContactAtL( TInt aIndex ) = 0;

        /**
        * Changes the sort order of the view. The view will become unavailable
        * while reordering contacts
        * @param aSortOrder the new sort order.
        */ 
        virtual void ChangeSortOrderL( 
            const RVPbkSimFieldTypeArray& aSortOrder ) = 0;

        /**
        * Maps the sim index to index of the view contact
        * @param aSimIndex the sim index of the contact
        * @return the view index of the contact or KErrNotFound
        */
        virtual TInt MapSimIndexToViewIndexL( TInt aSimIndex ) = 0;
        
        /**
        * Contact matching prefix
        *
        * @param aFindStrings
        * @param aObserver
        * @return MVPbkSimStoreOperation
        */
        virtual MVPbkSimStoreOperation* ContactMatchingPrefixL(
                const MDesCArray& aFindStrings, 
                MVPbkSimViewFindObserver& aObserver ) = 0;
        
        /**
        * Returns the sort order of the view
        *
        * @return the sort order of the view.
        */
        virtual const RVPbkSimFieldTypeArray& SortOrderL() const = 0;
    };

#endif      // MVPBKSIMCNTVIEW_H
            
// End of File
