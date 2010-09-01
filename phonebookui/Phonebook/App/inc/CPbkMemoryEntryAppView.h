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
*        Base class for Memory entry app views.
*
*/


#ifndef __CPbkMemoryEntryAppView_H__
#define __CPbkMemoryEntryAppView_H__

//  INCLUDES
#include "CPbkAppView.h"
#include <MPbkContactDbObserver.h>
#include <cntviewbase.h>


// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactChangeNotifier;
class CPbkFFSCheck;
class RSharedDataClient;


// CLASS DECLARATION

/**
 *  Base class for Memory entry app views.
 */
class CPbkMemoryEntryAppView :
        public CPbkAppView,
        private MPbkContactDbObserver,
        private MContactViewObserver
    {
    protected:  // Constructors and destructor
        /**
         * Default constructor.
         */
        CPbkMemoryEntryAppView();

        /**
         * Destructor.
         */
        ~CPbkMemoryEntryAppView();

    protected: // Base class interface
        /**
         * Command handler: Back softkey.
         */
        virtual void CmdBackL();

        /**
         * Delete contact command implementation.
         */
        virtual void CmdDeleteMeL();

        /**
         * Called by CmdDeleteMeL() to verify the deletion.
         *
         * @return ETrue if it is ok to delete the contact, EFalse otherwise.
         */
        virtual TBool DoAskDeleteL(CPbkContactItem& aContact);

        /**
         * Notifies derived class that the displayed contact has changed.
         * Default implementation does nothing.
         */
        virtual void ContactChangedL();

        /**
         * Returns the contact item this view is displaying currently.
         */
        virtual CPbkContactItem* ContactItemL() =0;

        /**
         * Returns the id of the contact item this view is displaying 
         * currently.
         */
        virtual TContactItemId ContactItemId() =0;

    protected:  // from CAknView
        /// This must be called from derived class implementation
	    void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        /// This must be called from derived class implementation
	    void DoDeactivate();
        
    protected:  // from CPbkAppView
        void HandleCommandL(TInt aCommandId);
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // from MPbkContactDbObserver
        void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);

    private:  // Implementation
        void HandleContactDeletedL();
        void FFSCheckL();
        void RequestFreeDiskSpaceLC();
        void DbCompactL();


    private:  // Data
        /// Ref: all contacts view
        CContactViewBase* iAllContactsView;
        /// Own: Contact database change notifier
        CPbkContactChangeNotifier* iContactChangeNotifier;
        /// Helper class
        class TDeletedContactData
            {
            public:  // Interface
				/**
				 * Constructor.
				 */
                TDeletedContactData();
				
				/**
				 * Sets deleted.
				 */
                void SetDeleted();

				/**
				 * Sets removed
				 */
                void SetRemoved();

				/**
				 * Returns ETrue if the item is deleted and removed.
				 */
                TBool IsDeletedAndRemoved() const;

				/**
				 * Sets the index to aIndex.
				 */
                void SetIndex(TInt aIndex);

				/**
				 * Returns the index.
				 */
                TInt Index() const;

				/**
				 * Performs reset.
				 */
                void Reset();

            private:  // Data
				/// Own: flags
                TUint iFlags;
				/// Own: index
                TInt iIndex;
            };
        /// Own: deleted contact data
        TDeletedContactData iDeletedContactData;        
        /// Own: Flash File System check helper
        CPbkFFSCheck* iPbkFFSCheck;
        /// Own: shared data client
        RSharedDataClient* iSharedDataClient;       

    };


// INLINE FUNCTIONS

inline CPbkMemoryEntryAppView::TDeletedContactData::TDeletedContactData()
    {
    Reset();
    }

inline CPbkMemoryEntryAppView::CPbkMemoryEntryAppView()
	{
	}


#endif // __CPbkMemoryEntryAppView_H__
            
// End of File
