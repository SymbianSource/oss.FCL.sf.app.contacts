/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Phonebook 2 merge contacts command.
 *
 */

#ifndef CPBK2MERGECONTACTSCMD_H
#define CPBK2MERGECONTACTSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2FetchDlgObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactSelector.h>
#include <MVPbkBatchOperationObserver.h>
#include <MPbk2ExitCallback.h>
#include <MPbk2ProcessDecorator.h>
#include "CPbk2MergeResolver.h"
#include <f32file.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactOperationBase;
class CVPbkContactManager;
class MPbk2DialogEliminator;
class CVPbkContactStoreUriArray;
class CPbk2MergeResolver;
class CPbk2ApplicationServices;
class CAknNavigationDecorator;
class CVPbkContactLinkArray;
class MVPbkStoreContactField;

// CLASS DECLARATION
/**
 * Phonebook 2 merge contacts command.
 */
class CPbk2MergeContactsCmd : public CActive,
        public MPbk2Command,
        private MPbk2FetchDlgObserver,
        private MVPbkContactSelector,
        private MVPbkContactViewObserver,
        private MVPbkSingleContactOperationObserver,
        private MVPbkBatchOperationObserver,
        private MPbk2ExitCallback,
        private MPbk2ProcessDecoratorObserver,
        private MVPbkContactObserver
    {
public:
    // Construction and destruction

    /**
     * Creates a new instance of this class.
     *
     * @param aUiControl The UI control.
     * 
     * @return  A new instance of this class.
     */
    static CPbk2MergeContactsCmd* NewL(MPbk2ContactUiControl& aUiControl);

    /**
     * Destructor.
     */
    ~CPbk2MergeContactsCmd();

private: // From MVPbkContactSelector
    TBool IsContactIncluded(
            const MVPbkBaseContact& aContact );
    
private: // MPbk2ExitCallback
    TBool OkToExitL(TInt aCommandId);

private: //From MPbk2Command
    void ExecuteLD();
    void ResetUiControl(MPbk2ContactUiControl& aUiControl);
    void AddObserver(MPbk2CommandObserver& aObserver);

private: // From MVPbkContactViewObserver
    void ContactViewReady(MVPbkContactViewBase& aView);
    void ContactViewUnavailable(MVPbkContactViewBase& aView);
    void ContactAddedToView(MVPbkContactViewBase& aView, TInt aIndex,
            const MVPbkContactLink& aContactLink);
    void ContactRemovedFromView(MVPbkContactViewBase& aView, TInt aIndex,
            const MVPbkContactLink& aContactLink);
    void ContactViewError(MVPbkContactViewBase& aView, TInt aError,
            TBool aErrorNotified);

private: // From MVPbkSingleContactOperationObserver
    void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
    void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError );

private: //Construction
    CPbk2MergeContactsCmd(MPbk2ContactUiControl& aUiControl);
    void ConstructL();

private: //From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private: // From MPbk2FetchDlgObserver
    MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection AcceptFetchSelectionL(
            TInt aNumMarkedEntries, MVPbkContactLink& aLastSelection);
    void FetchCompletedL(MVPbkContactLinkArray* aMarkedEntries);
    void FetchCanceled();
    void FetchAborted();
    TBool FetchOkToExit();
    
private: // From MVPbkBatchOperationObserver
    void StepComplete(
            MVPbkContactOperationBase& aOperation,
            TInt aStepSize );
    TBool StepFailed(
            MVPbkContactOperationBase& aOperation,
            TInt aStepSize,
            TInt aError );
    void OperationComplete(
            MVPbkContactOperationBase& aOperation );
 
private: // From MVPbkContactObserver
    void ContactOperationCompleted( TContactOpResult aResult );
    void ContactOperationFailed
                ( TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified );
private: // From MPbk2ProcessDecoratorObserver    
    void ProcessDismissed( TInt aCancelCode );
    
private: // Implementation        
    enum TPhase
        {
        EPhaseNone,
        EPhaseGetSelection,
        EPhaseGetStoreContacts,
        EPhaseMerge,
        EPhaseResolveConflicts,
        EPhaseResolvePhotoConflict,
        EPhaseCreateMergedContact,
        EPhaseGetGroups,
        EPhaseAddGroups,
        EPhaseFinish
        };

    void GetContactsFromUiFetchL();
    void Finish(TInt aReason);
    void StartNext(TPhase aPhase);
    void StartNext();
    void CleanAfterFetching();
    TBool IsFromReadOnlyStore(
            const MVPbkContactLink& aContactLink ) const;
    void RetrieveContact( const MVPbkContactLink& aContactLink );
    void NotifyObservers();
    void SetTitlePaneL( TBool aCustom );
    void CheckPhotoConflictL();
    void ResolvePhotoConflictL();
    void ResolveAllPhotoConflicts( 
            EPbk2ConflictedNumber aConflictResolutionNumber );
    void AddFieldToMergedContactL( MVPbkStoreContactField& field );
    HBufC* ContactAsStringL( MVPbkStoreContact* aStoreContact );
    void ShowContactsMergedNoteL();
    void GetStoreContacts();
    void ResolveConflictsL();
    void FinalizeMergeL();
    void DeleteSourceContactsL();
    void DeleteMergedContact();
    void DeleteMergedContactL();
    void GetGroupsL();
    void AutomaticMergeL();
    void AddGroupsL();
    
private: // Data
    
    // Own: CPbk2MergeContactsCmd phase
    TPhase iNextPhase;
    
    /// Ref
    MPbk2CommandObserver* iCommandObserver;
    
    /// Ref
    MPbk2ContactUiControl *iUiControl;
    
    /// Not own: contact manager
    CVPbkContactManager* iContactManager;
    
    /// Own: all contacts view sans read-only stores
    //      Not constructed if current store configuration doesn't include
    //      read-only stores
    MVPbkContactViewBase* iAllContactsView;
    
    /// Not own: Used to getting view notifications
    MVPbkContactViewBase* iObservedView;
    
    /// Own: reference to the fetch dlg, to insure is deleted
    MPbk2DialogEliminator* iFetchDlgEliminator;
    
    /// Own: Store uris used for the fetch dlg
    CVPbkContactStoreUriArray* iStoreUris;
    
    //Own: Fitst contact for merging
    MVPbkContactLink* iContactFirst;
    
    //Own: Second contact for merging
    MVPbkContactLink* iContactSecond;
    
    //Own: Merged contact
    MVPbkContactLink* iMergedContactLink;
    
    /// Own: First store contact
    MVPbkStoreContact* iStoreContactFirst;
    
    /// Own: Second store contact
    MVPbkStoreContact* iStoreContactSecond;
    
    /// Own: Merged store contact
    MVPbkStoreContact* iMergedContact;
    
    // Own: Merge resolver
    CPbk2MergeResolver* iMergeResolver;
    
    /// Own: Retrieve operation
    MVPbkContactOperationBase* iRetrieveOperation;

    /// Own: Delete operation
    MVPbkContactOperationBase* iDeleteOperation;
    
    /// Own: Commit operation
    MVPbkContactOperationBase* iCommitOperation;
    
    /// Own: Delete merged contact operation
    MVPbkContactOperationBase* iDeleteMergedOperation;
    
    /// Own: Results
    CVPbkContactLinkArray* iContactsToDelete;
    
    /// Own: Results
    CArrayPtrFlat<MVPbkStoreContact>* iContactToCommit;
    
    /// Own: Application Services pointer
    CPbk2ApplicationServices* iAppServices;
    
    /// Own: Image field data from first contact
    TPtrC8 iDataFirst;
    
    /// Own: Image field data  from second contact
    TPtrC8 iDataSecond;
    
    /// Own: Index of photo conflict 
    //      KErrNotFound if conflit does not exist
    TInt iPhotoConflictIndex;
    
    /// Own: Count of image type conflicts
    TInt iImageTypeConflictsCount;
    
    /// Own: All groups for merged contact 
    CArrayPtrFlat<MVPbkStoreContact>* iGroupsToAdd;
    
    /// Own: Groups links array of first contact
    CVPbkContactLinkArray* iGroupLinksFirst;
    
    /// Own: Groups links array of second contact
    CVPbkContactLinkArray* iGroupLinksSecond;
    
    /// Own: Waiting note
    MPbk2ProcessDecorator* iWaitDecorator;
    
	/// Own: string for ending note
    HBufC* iFirstContactString;
	
	/// Own: string for ending note
    HBufC* iSecondContactString;
	
	/// Own: string for ending note
    HBufC* iMergedContactString;
    
    };

#endif // CPBK2MERGECONTACTSCMD_H
// End of File
