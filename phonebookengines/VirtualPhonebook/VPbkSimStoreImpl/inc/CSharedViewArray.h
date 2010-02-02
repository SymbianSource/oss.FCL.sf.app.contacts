/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An array that manages shared views
*
*/



#ifndef VPBKSIMSTOREIMPL_CSHAREDVIEWARRAY_H
#define VPBKSIMSTOREIMPL_CSHAREDVIEWARRAY_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCntView.h>
#include <VPbkSimStoreCommon.h>

// FORWARD DECLARATIONS
class CVPbkSimContactView;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CSharedViewOwner;
class CViewHandle;

// CLASS DECLARATION

/**
* An array that manages shared views.
*/
NONSHARABLE_CLASS(CSharedViewArray) : public CBase
    {    
    public: // Data types
        enum TViewDestructionPolicy
            {
            /// Views are destroyed when this array is destroyed
            EDestroyViewWhenArrayIsDestroyed,
            /// Views are destroyed when last handle is removed.
            EDestroyViewIfNoHandles
            };
            
    public: // Construction and Destruction

        /**
        * Two-phased constructor.
        *
        * @return A new instance of this class
        */
        static CSharedViewArray* NewL( 
                TViewDestructionPolicy aDestructionPolicy );
        
        /**
        * Destructor.
        */
        ~CSharedViewArray();
    
    public: // Interface

        /**
         * Creates a new shared view if it didn't exist and
         * a new handle to it.
         *
         * @param aSortOrder The sort order for the view. Will be ignored
         *                   if the shared view already exist.
         * @param aConstructionPolicy Will be ignored if the shared view
         *                            already exist.
         * @param aParentStore The source of contacts.
         * @param aViewName A shared view identifier
         * @return a new handle to the shared view.
         */
        CViewHandle* CreateNewHandleL(
                const RVPbkSimFieldTypeArray& aSortOrder,
                TVPbkSimViewConstructionPolicy aConstructionPolicy, 
                MVPbkSimCntStore& aParentStore, const TDesC& aViewName );
        
        /**
         * Removes the view handle.
         *
         * @param aHandle The handle to be removed.
         */
        void RemoveHandle( CViewHandle& aHandle );

    private: // Implementation

        /**
        * C++ constructor.
        */
        CSharedViewArray( TViewDestructionPolicy aDestructionPolicy );
        /// Returns the owner or NULL
        CSharedViewOwner* FindOwner( const TDesC& aViewName );
        
    private:    // Data
        /// Own: defines how views are destroyed
        TViewDestructionPolicy iDestructionPolicy;
        /// Own: view holders
        RPointerArray<CSharedViewOwner> iSharedViews;
    };

/**
 * A class for view handle. Forwards requests to real view.
 */
NONSHARABLE_CLASS(CViewHandle) : 
            public CBase, 
            public MVPbkSimCntView
    {
    public: // Construction and Destruction

        /**
        * Two-phased constructor.
        *
        * @param aSharedViewArray An array that owns shared views.
        * @param aSharedView The view this handle points to.
        * @return A new instance of this class
        */
        CViewHandle( CSharedViewArray& aSharedViewArray, 
                CVPbkSimContactView& aSharedView );
        
        /**
        * Destructor.
        */
        ~CViewHandle();
    
    public: // Interface
        /**
         * @return The view this handle points to.
         */
        CVPbkSimContactView& SharedView();
        
    public: // From MVPbkSimCntView
        const TDesC& Name() const;
        MVPbkSimCntStore& ParentStore() const;
        void OpenL(
                MVPbkSimViewObserver& aObserver );
        void Close(
                MVPbkSimViewObserver& aObserver );
        TInt CountL() const;
        MVPbkSimContact& ContactAtL(
                TInt aIndex );
        void ChangeSortOrderL(
                const RVPbkSimFieldTypeArray& aSortOrder );
        TInt MapSimIndexToViewIndexL(
                TInt aSimIndex );
        MVPbkSimStoreOperation* ContactMatchingPrefixL(
                const MDesCArray& aFindStrings, 
                MVPbkSimViewFindObserver& aObserver );
        const RVPbkSimFieldTypeArray& SortOrderL() const;
    
    private: // Data
        /// Own: An array of shared views.
        CSharedViewArray& iSharedViewArray;
        /// Own: The view
        CVPbkSimContactView& iSharedView;
    };

/**
 * A class for managing view handles to the one shared view.
 */
NONSHARABLE_CLASS(CSharedViewOwner) : public CBase
    {
    public: // Construction and Destruction

        /**
        * Two-phased constructor.
        *
        * @param aSharedViewArray An array that owns shared views.
        * @param aSharedView The view this handle points to.
        */
        CSharedViewOwner( CSharedViewArray& aSharedViewArray,
                CVPbkSimContactView* aSharedView );
        
        /**
        * Destructor.
        */
        ~CSharedViewOwner();
    
    public: // Interface
    
        /**
         * @return The shared view that is owned by this owner.
         */
        CVPbkSimContactView& View() const;
        
        /**
         * Creates a new handle to the View()
         * 
         * @return a new handle to the View()
         */
        CViewHandle* CreateNewHandleL();
        
        /**
         * Removes a handle pointing to View()
         *
         * @param aHandle The handle to be removed.
         */
        void RemoveHandle( CViewHandle& aHandle );
        
        /**
         * @return ETrue if View() has handles.
         */
        TBool HasHandles() const;
    
    private: // Data
        /// Ref: The parent array of this owner
        CSharedViewArray& iSharedViewArray;
        /// Own: The shared view
        CVPbkSimContactView* iSharedView;
        /// Ref: References to view handles
        RPointerArray<CViewHandle> iViewHandles;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSHAREDVIEWARRAY_H
            
// End of File
