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
*     Sub view to a CContactViewBase.
*
*/


#ifndef __CPbkContactSubView_H__
#define __CPbkContactSubView_H__

// INCLUDES
#include <cntviewbase.h>

// FORWARD DECLARATIONS
class MPbkContactSelector;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Sub view of an existing CContactViewBase. 
 * Contacts to include in the view are filtered with MPbkContactSelector
 * interface.
 */
class CPbkContactSubView 
        : public CContactViewBase, private MContactViewObserver
	{
    public:  // Constructors and destructors
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver observer for this contact view
		 * @param aDb contact database reference
         * @param aBaseView the view this view is based on
         * @param aContactSelector contact selector used to filter this
         *        view's contents
         */
	    IMPORT_C static CPbkContactSubView* NewL
            (MContactViewObserver& aObserver,
            const CContactDatabase& aDb,
            CContactViewBase& aBaseView,
            MPbkContactSelector& aContactSelector);

    public:  // New functions
        /**
         * Returns the current contact selector.
         */
        IMPORT_C MPbkContactSelector& ContactSelector() const;

        /**
         * Changes the contacts included in this sub view.
         * Sends TContactViewEvent::EUnavailable. TContactViewEvent::EReady is 
         * sent when this view is again ready for use. 
         * TContactViewEvent::ESortError is sent if there is an error.
		 * @param aContactSelector contact selector used to filter this 
         *        view's contents
         */
        IMPORT_C void SetContactSelector(MPbkContactSelector& aContactSelector);

        /**
         * Refreshes the contents of this sub view by querying each contact 
         * again from the selector.
         * Sends TContactViewEvent::EReady when this view is again ready for
         * use. TContactViewEvent::ESortError is sent if there is an error.
         */
        IMPORT_C void Refresh();

        /**
         * Adds a contact to this sub view's set.
         * Leaves with KErrNotFound if the contact id is not found from the db.
         *
         * @param aContactId    id of the contact to add.
         */
        IMPORT_C void AddContactL(TContactItemId aContactId);

        /**
         * Removes a contact from this sub view's set.
         *
         * @param aContactId    id of the contact to remove.
         */
        IMPORT_C void RemoveContact(TContactItemId aContactId);

        /**
         * Adds an event handler which sees all underlying view's events 
         * before this view handles the event. Use to update any data
         * structure used in MPbkContactSelector::IsContactIncluded().
		 * @param aObserver contact view observer
         */
        IMPORT_C void AddBaseViewPreEventHandlerL
            (MContactViewObserver& aObserver);

        /**
         * Removes a pre event handler registered with
		 * AddBaseViewPreEventHandlerL().
		 * @param aObserver contact view observer
         */
        IMPORT_C void RemoveBaseViewPreEventHandler
            (MContactViewObserver& aObserver);

    public: // From CContactViewBase.
	    TContactItemId AtL(TInt aIndex) const;
	    const CViewContact& ContactAtL(TInt aIndex) const;
	    TInt CountL() const;
	    TInt FindL(TContactItemId aId) const;
	    HBufC* AllFieldsLC(TInt aIndex,const TDesC& aSeparator) const;
	    TContactViewPreferences ContactViewPreferences();
	    const RContactViewSortOrder& SortOrderL() const;
        TAny* CContactViewBase_Reserved_1(TFunction aFunction,TAny* aParams);

    protected:  // Constructors and destructor
        /**
         * C++ constructor.
         * @param aDb               an open contact db
         * @param aContactSelector  contact selector used to filter this 
         *                          view's contents
         */
	    CPbkContactSubView
            (const CContactDatabase& aDb, 
            MPbkContactSelector& aContactSelector);

        /**
         * Second phase constructor.
         * @param aObserver mandatory observer for this view
         * @param aBaseView a contact view this view's contents is filtered 
         *                  from
         */
	    void ConstructL
            (MContactViewObserver& aObserver, CContactViewBase& aBaseView);

        /**
         * Destructor.
         */
	    ~CPbkContactSubView();

    private: // From MContactViewObserver.
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);

    private:  // Implementation
        class CMapper;
        friend class CMapper;
	    struct TIdWithMapping
		    {
		    TInt iMapping;
		    TContactItemId iId;
		    };
        TInt DoFind(TContactItemId aId) const;
        void SetStateReady(const TContactViewEvent& aEvent);
        void SetStateInitializing();
        void SetStateNotReady(const TContactViewEvent& aEvent);
        void StartMapping(const TContactViewEvent& aCompleteEvent);
        void ResetMapping();
        void AddMappingL(const TIdWithMapping& aMapping);
        void MappingComplete();
        void MappingError(TInt aError);
        TBool HandleItemAdded(TContactViewEvent& aEvent);
        TBool HandleItemAddedL(TContactViewEvent& aEvent);
        TBool HandleItemRemoved(TContactViewEvent& aEvent);
        TBool HandleItemRemovedL(TContactViewEvent& aEvent);
        TInt UpdateMappingsL();
        static TInt CompareIndexes(const TIdWithMapping& aLhs, const TIdWithMapping& aRhs);
	    static TBool IdsEqual(const TIdWithMapping& aLhs,const TIdWithMapping& aRhs);

    private:  // Data
        /// Ref: the contact view this view is based on
	    CContactViewBase* iBaseView;
        /// Ref: selector defining which contacts to include into this view
        MPbkContactSelector* iContactSelector;
        /// Own: mapping array of contact id -> index in iBaseView
	    RArray<TIdWithMapping> iFilteredIdArray;
        /// Own: active object which populates the view
        CMapper* iMapper;
        /// Own: event to send when mapping is ready
        TContactViewEvent iMappingCompleteEvent;
        /// Own: array of pre event observers
        RPointerArray<MContactViewObserver> iPreBaseViewObservers;
	};


#endif // __CPbkContactSubView_H__

// End of File
