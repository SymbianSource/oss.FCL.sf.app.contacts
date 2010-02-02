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
* Description:  Phonebook 2 CCA launcher command
*
*/


#ifndef CPbk2NlxAddToTopContactsCmd_H
#define CPbk2NlxAddToTopContactsCmd_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkOperationObserver.h>
#include "MPbk2ContactRelocatorObserver.h"
#include <MPbk2FetchDlgObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactSelector.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactOperationBase;
class CVPbkTopContactManager;
class CPbk2ContactRelocator;
class MVPbkContactOperationBase;
class MPbk2ContactLinkIterator;
class CVPbkContactLinkArray;
class CVPbkContactManager;
class MPbk2DialogEliminator;
class CVPbkContactStoreUriArray;

// CLASS DECLARATION
/**
 * Phonebook 2 add to top contacts command object.
 */
class CPbk2NlxAddToTopContactsCmd : 
    public CActive,
    public MPbk2Command,
    private MVPbkOperationObserver,
    private MVPbkOperationErrorObserver,
    private MPbk2ContactRelocatorObserver,
    private MPbk2FetchDlgObserver,
    private MVPbkContactViewObserver,
    private MVPbkContactSelector,
    private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl The UI control.
         * @param aAddFavourites ETrue if the "Add favourites" feature
         *      is to be executed.
         * 
         * @return  A new instance of this class.
         */
        static CPbk2NlxAddToTopContactsCmd* NewL( 
            MPbk2ContactUiControl& aUiControl,
            TBool aAddFavourites );

        /**
         * Destructor.
         */
        ~CPbk2NlxAddToTopContactsCmd();

    private: //From MPbk2ContactRelocatorObserver
    	void ContactRelocatedL(	MVPbkStoreContact* aRelocatedContact );
    	void ContactRelocationFailed(
	        TInt aReason,
	        MVPbkStoreContact* aContact );
    	void ContactsRelocationFailed(
	        TInt aReason,
	        CVPbkContactLinkArray* aContacts );
    	void RelocationProcessComplete();
        
    private: //From MVPbkOperationObserver
	    void VPbkOperationCompleted( MVPbkContactOperationBase* aOperation );
    private: //From MVPbkOperationErrorObserver	    
	    void VPbkOperationFailed(
	            MVPbkContactOperationBase* aOperation,
	            TInt aError );

    private: //From MPbk2Command
        void ExecuteLD();
        void ResetUiControl( MPbk2ContactUiControl& aUiControl );
        void AddObserver( MPbk2CommandObserver& aObserver );

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView, 
                TInt aIndex, 
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView, 
                TInt aError, 
                TBool aErrorNotified );

    private: // From MVPbkContactSelector
        TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );
        
    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );        
        
    private: //Construction
        CPbk2NlxAddToTopContactsCmd( 
            MPbk2ContactUiControl& aUiControl,
            TBool aAddFavourites );
        void ConstructL();
        
    private://From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private: // From MPbk2FetchDlgObserver
        MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection AcceptFetchSelectionL(
            TInt aNumMarkedEntries,
            MVPbkContactLink& aLastSelection );
        void FetchCompletedL(
            MVPbkContactLinkArray* aMarkedEntries );
        void FetchCanceled();
        void FetchAborted();
        TBool FetchOkToExit();

    private: // Implementation        
		enum TPhase 
		    {
		    EPhaseNone,
		    EGetSelectionWithFetchUi,
		    ERelocate,
		    ESetAsTopContact,
		    EFinish
			};
			
		void GetSelectionWithFetchUiL();
		void GetUiControlSelectionL();
    	void StartSimContactsRelocationL();
        void Finish( TInt aReason );
        void StartNext( TPhase aPhase );
        void StartNext();
        void CleanAfterFetching();
        TInt CountSimContacts() const;
        CVPbkContactLinkArray* CreateSimContactsArrayLC() const;
        void ReplaceSimContactsL();
        void ShowWaitNoteL();

    private: // Data
        
    	TPhase iNextPhase;
        /// Ref
        MPbk2CommandObserver* iCommandObserver;
        /// Ref
        MPbk2ContactUiControl *iUiControl;
        /// Whether in "Add favourites" mode.
        TBool iAddFavourites;
        ///Own:         
        CVPbkTopContactManager* iVPbkTopContactManager;
        ///Own
        CPbk2ContactRelocator* iContactRelocator;
        ///Own: Top contact operation
        MVPbkContactOperationBase* iContactOperation;
        /// Own
        MPbk2ContactLinkIterator* iContactIterator;
        /// Own: contacts from fetch or ui controls
        CVPbkContactLinkArray* iMarkedEntries;
        /// Own: relocated sim contacts
        CVPbkContactLinkArray* iRelocatedContacts;
        /// Ref
        CVPbkContactManager* iContactManager;
        /// Own: group view without empty groups
        MVPbkContactViewBase* iNonEmptyGroupsView;
        /// Own: all contacts view sans read-only stores
        //      Not constructed if current store configuration doesn't include
        //      read-only stores
        MVPbkContactViewBase* iAllContactsView;
        /// Own: reference to the fetch dlg, to insure is deleted
        MPbk2DialogEliminator* iFetchDlgEliminator;
        /// Own: Store uris used for the fetch dlg
        CVPbkContactStoreUriArray* iStoreUris;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
    };

#endif // CPbk2NlxAddToTopContactsCmd_H

// End of File
