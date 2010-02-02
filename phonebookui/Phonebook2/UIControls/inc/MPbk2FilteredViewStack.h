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
* Description:  Phonebook 2 Filtered view stack.
*
*/

#ifndef MPBK2FILTEREDVIEWSTACK_H_
#define MPBK2FILTEREDVIEWSTACK_H_

#include <e32base.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactViewBase.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CVPbkContactFindPolicy;
class MVPbkContactBookmarkCollection;
class MPbk2FilteredViewStackElement;
class CElementStack;
class CAknSearchField;


/**
 * An observer interface for view stack events. This can
 * be used together with MVPbkContactViewObserver.
 */
class MPbk2FilteredViewStackObserver
    {
    public: // Interface
        
        /**
         * This is called always when the top view of the stack is changed.
         * Note that the client will also get view events 
         * via MVPbkContactViewObserver but they might be called also in
         * situations when the top view is not changed. This will be called
         * before the MVPbkContactViewObserver event.
         *
         * @param aOldView  The old top view that will be unusable after
         *                  the call. Client can not save a reference to it.
         */
        virtual void TopViewChangedL(
                MVPbkContactViewBase& aOldView ) = 0;
        
        /**
         * This is called when the stack is updated but the top view
         * didn't change. Called before MVPbkContactViewObserver event. 
         */
        virtual void TopViewUpdatedL() = 0;
       
        /**
         * This is called when the stack's base view is changed.
         */ 
        virtual void BaseViewChangedL() = 0;
        
        /**
         * Called if TopViewChangedL or TopViewUpdatedL leaves.
         *
         * @param aError    System wide error code.
         */
        virtual void ViewStackError(
                TInt aError ) = 0;
        
        /**
         * Called always when a contact is added to the base view despite
         * of the filter level. If the top view is the base view then
         * this is called before the MVPbkContactViewObserver event.
         *
         * @param aBaseView     Same as CPbk2FilteredViewStack::BaseView()
         * @param aIndex        The index of the added contact
         *                       in the base view.
         * @param aContactLink  Link to the added contact.
         */
        virtual void ContactAddedToBaseView( 
                MVPbkContactViewBase& aBaseView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink ) = 0;
        
    protected: // Disabled functions
        ~MPbk2FilteredViewStackObserver()
                {}
    };



class MPbk2FilteredViewStack : public MVPbkContactViewBase
    {
    public:
    
        virtual ~MPbk2FilteredViewStack() {}

    public:  // Interface

        virtual void UpdateFilterL(
                const MDesCArray& aFindStrings, 
                const MVPbkContactBookmarkCollection* aAlwaysincluded,
                TBool aAlwaysIncludedChanged ) = 0;
        
        /**
         * Resets the stack. After this the top view will be the base view.
         *
         * Note: MPbk2FilteredViewStackObserver is called if the top view
         *       changes.
         */
        virtual void Reset() = 0;
        
        /**
         * Returns the base view.
         *
         * @return  The view that was given in the constructor.
         */
        virtual MVPbkContactViewBase& BaseView() const = 0;
        
        /**
         * Sets a new base view. Calls first Reset() and then
         * removes itself from observing the old view and
         * starts listening to new view. The client of stack is
         * notified by a view event when the stack is ready.
         *
         * @param aBaseView     The new base view.
         */ 
        virtual void SetNewBaseViewL(
                MVPbkContactViewBase& aBaseView ) = 0;
                
        /**
         * Returns the current level of the stack. Zero means
         * that there is only a base view in the stack.
         *
         * @return  Stack level.
         */
        virtual TInt Level() const = 0;

        /**
         * Adds an observer.
         *
         * @param aStackObserver    New observer for the stack.
         */
        virtual void AddStackObserverL( 
                MPbk2FilteredViewStackObserver& aStackObserver ) = 0;
        
        /** 
         * This can be always safely called event if the given
         * observer object is not an observer for the stack.
         *
         * @param aStackObserver    Observer to remove.
         */
        virtual void RemoveStackObserver( 
                MPbk2FilteredViewStackObserver& aStackObserver ) = 0;

    };



#endif /* MPBK2FILTEREDVIEWSTACK_H_ */
