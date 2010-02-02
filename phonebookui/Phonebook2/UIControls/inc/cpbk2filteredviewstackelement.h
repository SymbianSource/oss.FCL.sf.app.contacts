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
* Description:  Phonebook 2 view stack element.
*
*/


#ifndef CPBK2FILTEREDVIEWSTACKELEMENT_H
#define CPBK2FILTEREDVIEWSTACKELEMENT_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class MVPbkContactViewObserver;
class MVPbkContactFindPolicy;
class MVPbkContactBookmarkCollection;

// CLASS DECLARATION

/**
 * An interface for an element in CPbk2FilteredViewStack.
 * Responsible for:
 * -representing one element of filtered view stack
 * -owning the appropriate contact view if not the base element
 * -owning the find strings with which the view is filtered
 */
NONSHARABLE_CLASS(MPbk2FilteredViewStackElement)
    {
    public: // Types
        /**
         * Virtual Phonebook View States
         */
        enum TViewState
            {
            // View hasn't responded anything yet.
            EUndefined,
            // UpdateFilterL has been called for the view
            EUpdating,
            // View has send an unavailable event
            EUnavailable,
            // View has send a ready event
            EReady,
            // View has send an error event
            EError
            };
            
    public:
        virtual ~MPbk2FilteredViewStackElement() {}
        
        /**
         * Creates a new filtered view from aParentView using the find text
         * and always included contact information that has been set to this
         * element. Note that if we are making a find view with diraphic
         * characters we need to use the base view as a parent to prevent
         * accidentally filtering out contacts that include digprahic
         * characters.
         *
         * @param aBaseView The base view.
         * @param aParentView The parent view whose 
         *                    CreateFilteredViewLC is called.
         * @param aObserver The observer that will be notified when the view
         *                  is ready.
         */
        virtual void CreateViewL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewBase& aParentView,
                MVPbkContactViewObserver& aObserver ) = 0;
                
        /**
         * Returns the view of this element. Can be NULL if CreateViewL 
         * has not been called to CPbk2FilteredViewStackElement yet.
         * 
         * @return the view of this element or NULL.
         */
        virtual MVPbkContactViewBase* View() const = 0;
        
        /**
         * @return the find text for the filtered view.
         */
        virtual const MDesCArray& FindArray() const = 0;
        
        /** 
         * @return the filter level of this view. The level is 0
         * for the base view, 1 for the first filtered view etc.
         */
        virtual TInt Level() const = 0;
        
        /**
         * @return a reference to the always included contact collection
         *         that has been given to this element.
         */
        virtual const MVPbkContactBookmarkCollection* 
                AlwaysIncluded() const = 0;
        
        /** 
         * @return ETrue if levels are same, always included contacts
         *               are NULL and find texts match.
         */
        virtual TBool IsSame( 
                const MPbk2FilteredViewStackElement& aOther ) const = 0;
        
        /**
         * Updates the data in this element and calls UpdateFilterL 
         * for the view. If the find string is digraphic, the current view is
         * re-created instead of updating.
         */
        virtual void UpdateFilterL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aSearchLevelArray,
                const MVPbkContactBookmarkCollection* aAlwaysIncluded ) = 0;
        
        /**
         * @return the state of this element
         */
        virtual TViewState ViewState() const = 0;
        
        /**
         * @param aState the new state of this element
         */
        virtual void SetViewState( TViewState aState ) = 0;
        
        /**
         * @return ETrue if this element is under destruction and unusable.
         */
        virtual TBool UnderDestruction() const = 0;
        
        /**
         * Sets this element under destruction.
         */
        virtual void SetUnderDestruction() = 0;
    };

/**
 * A filtered view stack element for the base view. CreateViewL is
 * not suitable to call for this element because it already has a view.
 * UpdateFilterL is not usable because the base element is not itself
 * a filtered view.
 */
NONSHARABLE_CLASS(CPbk2FilteredViewStackBaseElement) 
    :   public CBase,
        public MPbk2FilteredViewStackElement
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this clas.
         *
         * @param aBaseView the base view that is not a filtered view itself
         * @return  A new instance of this class.
         */
        static CPbk2FilteredViewStackBaseElement* NewL( 
                MVPbkContactViewBase& aBaseView );

    public: // From MPbk2ViewStackElement
        
        void CreateViewL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewBase& aParentView,
                MVPbkContactViewObserver& aObserver );
        MVPbkContactViewBase* View() const;
        const MDesCArray& FindArray() const;
        TInt Level() const;
        const MVPbkContactBookmarkCollection* AlwaysIncluded() const;
        TBool IsSame( const MPbk2FilteredViewStackElement& aOther ) const;
        void UpdateFilterL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aSearchLevelArray,
                const MVPbkContactBookmarkCollection* aAlwaysIncluded );
        TViewState ViewState() const;
        void SetViewState( TViewState aState );
        TBool UnderDestruction() const;
        void SetUnderDestruction();
        
    private: // Implementation
        CPbk2FilteredViewStackBaseElement( MVPbkContactViewBase& aBaseView );

    private: // Data
        /// Own: All Contacts view
        MVPbkContactViewBase& iBaseView;
        TViewState iState;
    };
    
/**
 * A filtered view stack element for the filtered views.
 */
NONSHARABLE_CLASS(CPbk2FilteredViewStackElement) 
    :   public CBase,
        public MPbk2FilteredViewStackElement
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this clas.
         *
         * @param aFindPolicy a reference to find policy that can be use
         *                    to compare find texts.
         * @param aSearchLevelArray the array that contains search strings.
         * @param aAlwaysIncluded a reference to the always included contact
         *                        collection that must be alive while
         *                        this element is alive.
         * @return  A new instance of this class.
         */
        static CPbk2FilteredViewStackElement* NewL(
                MVPbkContactFindPolicy& aFindPolicy,
                const MDesCArray* aSearchLevelArray,
                const MVPbkContactBookmarkCollection* aAlwaysIncluded );

        /**
         * Destructor.
         */
        ~CPbk2FilteredViewStackElement();

    public: // From MPbk2ViewStackElement
        
        void CreateViewL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewBase& aParentView,
                MVPbkContactViewObserver& aObserver );
        MVPbkContactViewBase* View() const;
        const MDesCArray& FindArray() const;
        TInt Level() const;
        const MVPbkContactBookmarkCollection* AlwaysIncluded() const;
        TBool IsSame( const MPbk2FilteredViewStackElement& aOther ) const;
        void UpdateFilterL(
                MVPbkContactViewBase& aBaseView,
                MVPbkContactViewObserver& aObserver,
                const MDesCArray& aSearchLevelArray,
                const MVPbkContactBookmarkCollection* aAlwaysIncluded );
        TViewState ViewState() const;
        void SetViewState( TViewState aState );
        TBool UnderDestruction() const;
        void SetUnderDestruction();
                
    private: // Implementation
        CPbk2FilteredViewStackElement(
                MVPbkContactFindPolicy& aFindPolicy,
                const MVPbkContactBookmarkCollection* aAlwaysIncluded,
                const MDesCArray* aSearchLevelArray );
        void ConstructL();
        TBool Match( const MDesCArray& aArray1, 
                     const MDesCArray& aArray2 ) const;
        TInt CountLengthOfStrings( const MDesCArray& 
            aStringArray ) const;
        /**
         * @return ETrue if the find filtering must take digraphic characters
         * into account (digraph is a single character consisting of two
         * letters run together and representing a single sound). This affects
         * to the filtered find views optimization in Pbk2.
         */
        TBool FindMustBeDigraphicL() const;

    private: // Data
        /// Ref: Find policy
        MVPbkContactFindPolicy& iFindPolicy;
        /// Ref: the optional always included contacts
        const MVPbkContactBookmarkCollection* iAlwaysIncluded;
        /// Own: Contact view
        MVPbkContactViewBase* iView;
        TViewState iState;
        TBool iUnderDestruction;
        const MDesCArray* iSearchLevelArray;
        TInt iLengthOfSearchArray;
    };

#endif // CPBK2FILTEREDVIEWSTACKELEMENT_H

// End of File
