/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef T_MVPBKCONTACTSTORESINGLE_H
#define T_MVPBKCONTACTSTORESINGLE_H

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <f32file.h>

//  VIRTUALPHONEBOOK INCLUDES
#include <MVPbkContactStoreListObserver.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <MVPbkContactFindObserver.h>
#include <MVPbkContactFindFromStoresObserver.h>
#include <T_MVPbkContactStore.rsg>
#include <badesca.h>
#include <mvpbkoperationobserver.h>
#include <barsread.h>
#include <cntdb.h>


//  FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class CVPbkContactManager;
class MVPbkContactStoreList;
class MVPbkContactViewBase;
class MVPbkContactViewBookmark;
class CVPbkSortOrder;
class MVPbkFieldType;
class TVPbkFieldTypeMapping;
class MVPbkContactView;
class MVPbkSingleContactOperation;
class CVPbkContactViewDefinition;
class CVPbkContactLinkArray;
class MVPbkFieldTypeList;
class T_ContactViewObserver;
class CContactDatabase;

//  CLASS DEFINITION
/**
 *
 * Test class for single contact store. 
 *
 */
class T_MVPbkContactStoreSingle
     :  public CEUnitTestSuiteClass,
        public MVPbkContactStoreListObserver,
        public MVPbkContactObserver,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactViewObserver,
        public MVPbkBatchOperationObserver,
        public MVPbkContactFindObserver,
        public MVPbkContactFindFromStoresObserver
    {
    public:     // Constructors and destructors

        static T_MVPbkContactStoreSingle* NewL(
            const TVPbkContactStoreUriPtr& aUri );
        static T_MVPbkContactStoreSingle* NewLC(
            const TVPbkContactStoreUriPtr& aUri );
        ~T_MVPbkContactStoreSingle();

    private:    // Constructors and destructors

        T_MVPbkContactStoreSingle();
        void ConstructL( const TVPbkContactStoreUriPtr& aUri );

	private:    // From MVPbkContactStoreListObserver

        void StoreReady(MVPbkContactStore& aContactStore);
        void StoreUnavailable( MVPbkContactStore& aContactStore,
            TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent );
		void OpenComplete();
    
    private:    // From MVPbkContactObserver

        void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode,
            TBool aErrorNotified );
    
    private:    // From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation,
            MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError );
    
    private:    // From MVPbkContactViewObserver

        void ContactViewReady( MVPbkContactViewBase& aView );
        void ContactViewUnavailable( MVPbkContactViewBase& aView );
        void ContactAddedToView( MVPbkContactViewBase& aView, TInt aIndex,
            const MVPbkContactLink& aLink );
        void ContactRemovedFromView( MVPbkContactViewBase& aView,
            TInt aIndex, const MVPbkContactLink& aLink );
        void ContactViewError( MVPbkContactViewBase& aView, TInt aError,
            TBool aErrorNotified);

    private:    // From MVPbkBatchOperationObserver

        void StepComplete( MVPbkContactOperationBase& aOperation,
            TInt aStepSize );
        TBool StepFailed( MVPbkContactOperationBase& aOperation,
            TInt aStepSize, TInt aError );
        void OperationComplete( MVPbkContactOperationBase& aOperation );

    private:    // From MVPbkContactFindObserver

        void FindCompleteL( MVPbkContactLinkArray* aResults );
        void FindFailed(TInt aError);

    private:    // from MVPbkContactFindFromStoresObserver

        void FindFromStoreSucceededL( MVPbkContactStore& aStore, 
            MVPbkContactLinkArray* aResultsFromStore );
        void FindFromStoreFailed( MVPbkContactStore& aStore, TInt aError );
        void FindFromStoresOperationComplete();

    private:    // New methods

        void BaseSetupL();
        void SetupStoreL();
        void SetupStoreWithContactsL();
        void SetupStoreWithContactsAndObserverL();
        void SetupViewL();
        void SetupDeleteContactsL();
        void SetupDeleteContactsForMatchL();
        void SetupStoreForFindL();
        void SetupStoreForFindWithIdentifierLastL();
        void SetupStoreForFindFailL();
        void SetupBigStoreL();
        void SetupStoreAndTwoViewsWithContactsL();
        void SetupCreateFindStringsL();
        void SetupCreateFindStrings2L();
                
        void Teardown();

        void TestGetStoreListL();
        void TestOpenStoreL();
        void TestReplaceStoreL();
        void TestCreateContactL();
        void TestDeleteMultipleContactsL();
        void TestCommitMultipleContactsL();
        void TestMatchFullPhoneNumberL();
        void DoTestMatchFullPhoneNumberL();
        void TestMatchPartialPhoneNumberL();
        void DoTestMatchPartialPhoneNumberL();
        void TestNoMatchPhoneNumberL();
        void DoTestNoMatchPhoneNumberL();
        void TestMatchPhoneNumber_FailL();
        void TestCreateContactViewL();
        void TestCreateLocalContactViewL();
        void TestCreateUnsortedContactViewL();
        void TestCreateSharedGroupViewL();
        void TestCreateLocalGroupViewL();
        void TestCreateContactViewWithSelectorL();
        void TestCreateContactViewWithSelector2L();
        void TestContactRetrieverL();
        void TestCreateContactLinkNotCommitedL();

        // MVPbkContactView tests
        void TestChangeContactViewSortOrderL();
        void TestRefreshContactViewL();
        void TestCreateContactViewLinkL();
        void TestLinkRefersToL();
        void TestCreateContactViewBookmarkL();
        void TestAddContactViewObserverL();
        void TestAddContactViewObserverTwiceL();
        void DoTestAddContactViewObserverL(
            T_ContactViewObserver& aObserver );
        void TestRemoveContactViewObserverL();
        void TestRemoveNonexistentContactViewObserverL();

        void TestContactOperationsL();
        void TestContactFieldOperationsL();
        /**
         * Performs actual testing of finding contacts by their last names.
         *
         * @param aString Part of last name to search for
         * @param aFieldTypes Field types to search in
         * @param aExpectedMatchCount Expected count of search matches
         */
        void DoTestFindL(
            const TDesC& aString,
            const MVPbkFieldTypeList& aFieldTypes,
            const TInt aExpectedMatchCount );
        void TestFindNoMatchEmptyFieldTypesL();
        void TestFindMatchLastNameL();
        void TestFindMatchFirstLastNameL();
        void TestFindNoMatchL();
        void TestFind_FailL();
        void TestFindWithParserEmptyFieldTypesL();
        void TestFindWithParserL();
        void TestFindWithParserNoMatchL();
        void TestCancelFindL();
        void TestCopyContactsL();
        void TestGroupsL();
        void TestStoreInfoL();

        // MVPbkViewContact and MVPbkBaseContact tests
        void TestViewContactExpansionL();
        void TestIsSameViewContactWithContactStoreL();
        void TestIsSameViewContactWithNullStoreL();
        void TestIsNotSameViewContactWithContactStoreL();
        void TestIsNotSameViewContactWithNullStoreL();
        void DoTestIsSameViewContactWithTwoContactViewsL(
            const TInt aFirstContactIndex,
            const TInt aSecondContactIndex,
            const TBool aExpectedResultFirstComparison,
            const TBool aExpectedResultSecondComparison,
            MVPbkContactStore* aContactStore );
        void TestIsSameViewContactL();
        void TestIsSameStoreContactWithContactStoreL();
        void TestIsNotSameStoreContactWithContactStoreL();
        void DoTestIsSameStoreContactWithContactStoreL(
            const TInt aViewContactIndex,
            MVPbkContactStore* aContactStore,
            TBool aExpectedResult );
        void TestIsSameStoreContactL();
        void TestIsNotSameStoreContactL();
        void DoTestIsSameStoreContactL(
            const TInt aViewContactIndex,
            TBool aExpectedResult );
        void TestMatchViewContactStoreL();
        void TestFailMatchViewContactStoreL();
        void TestMatchViewContactStoreDomainL();
        void TestFailMatchViewContactStoreDomainL();
        void DoTestMatchViewContactStoreDomainL(
            const TInt aViewContactIndex,
            const TDesC& aStoreUri,
            TBool aExpectedResult );
        void TestCreateViewContactBookmarkL();
        
        /// Filtered view tests
        void TestCreateFilteredViewL();
        void TestCreateFilteredView2L();
        void TestCreateFilteredView3L();
        void TestCreateFilteredView4L();
        void TestDeleteContactsFromFilteredViewL();
        void TestCreateContactAfterFilteringL();
        void TestFilteredViewL();
        void TestUpdateFilterL();

        void ReplaceStoreL();
        void CreateContactViewL(
            MVPbkContactView*& aContactView,
            TInt aSortOrderResId,
            TInt aViewDefResId = R_SINGLE_CONTACT_VIEW );
        CVPbkContactViewDefinition* CreateViewDefLC( TInt aResId );
        TInt AddContactFieldsAndSaveL(
            MVPbkStoreContact& aStoreContact,
            const TDesC& aFName, const TDesC& aLName );
        /**
         * Adds new contacts to the contact store.
         *
         * @param aAmount Number of contacts to add. Defaults to 1.
         * @param aBeginWithIdentifier If true, identifier is inserted at the
         *                             beginning of the first and last name
         *                             values, otherwise it is appended to
         *                             the end. Defaults to true.
         */
        void PopulateContactStoreL(
            const TInt aAmount = 1,
            const TBool aBeginWithIdentifier = ETrue );
        CVPbkSortOrder* CreateSortOrderLC( TInt aResId );
        TInt AddValueToContactFieldL(
            MVPbkStoreContact& aStoreContact, 
            const MVPbkFieldType& aFieldType,
            const TDesC& aValue );
        TVPbkFieldTypeMapping CreateFieldTypeMappingL( const TInt aResId );
        void DeleteAllContactsL();
        TBool IsFieldTypeSupported( const MVPbkFieldType& aFieldType );
        void StartSchedulerAndDecideToLeaveL();
        void StartViewSchedulerAndDecideToLeaveL();
        void StartStoreSchedulerAndDecideToLeaveL();
        void StartContactSchedulerAndDecideToLeaveL();
        void ResetTestSuiteEvents();
        void TestFindWithSelectiveStoreSearchL();
		void SetupStoresForSelectiveFindL();

    private:    // Data

        // Owned
        HBufC* iUriToTest;

        // Owned
        CVPbkContactManager* iContactManager;

        RFs iFs;
        
        enum TLastEvent
            {
            ENullEvent,
            EStoreReady,
            EOpenError ,
            EOpenCompleted,
            EContactViewReady,
            EContactViewUnavailable,
            EContactViewError,
            EContactAddedToView,
            EContactRemovedFromView,
            EContactLoaded,
            EContactLoadFailed,
            EContactOperationCompleted,
            EContactOperationFailed,
            ESingleContactOperationCompleted,
            ESingleContactOperationFailed,
            EStepComplete,
            EStepFailed,
            EOperationComplete,
            EOperationCancelled,
            EFindComplete,
            EFindWithParserComplete
            };
        
        // Last events
        TInt iLastViewEvent;
        TInt iLastContactOpEvent;
        TInt iLastStoreEvent;
        TInt iLastSingleContactOpEvent;
        TInt iLastBatchOpEvent;
        TInt iLastFindEvent;
        // Last errors
        TInt iLastViewError;
        TInt iLastContactOpError;
        TInt iLastStoreError;
        TInt iLastSingleContactOpError;
        TInt iLastBatchOpError;
        TInt iLastFindError;
        // expected events
        TInt iExpectedViewEvent;
        TInt iExpectedStoreEvent;
        TInt iExpectedContactOpEvent;
        // expected event found
        TBool iExpectedViewResponseFound;
        TBool iExpectedStoreResponseFound;
        TBool iExpectedContactOpResponseFound;
        // Num of iExpectedViewEvent that test code expects before
        // active scheduler is stopped.
        TInt iExpectedViewEventCount;
        // Num of iExpectedViewEvent currently. 
        // When this is equal to iExpectedViewEventCount the scheduler will be
        // stopped
        TInt iCurViewEventCount;

        // Not owned
        MVPbkContactStore* iContactStore;

        // Not owned
        MVPbkContactStoreList* iStoreList;

        // Owned
        MVPbkStoreContact* iStoreContact;

        // Owned
        MVPbkContactView* iContactView;

        // Owned
        MVPbkContactView* iContactView2;

        // Owned
        MVPbkContactViewBase* iBaseView;

        TBool iNoOpenCompleteFlag;
        TBool iStoreUnavailable;

        // Owned
        MVPbkContactLinkArray* iContactLinks;

        // Owned
        CVPbkContactLinkArray* iFindResults;
        
        // Owned
        T_ContactViewObserver* iViewObserver;

        CActiveSchedulerWait iViewScheduler;
        CActiveSchedulerWait iStoreScheduler;
        CActiveSchedulerWait iContactScheduler;

        TInt iStepCounter;

        // Tells whether the store to be tested supports a test
        TBool iTestSupportedByStore;
        
        /// Own:
        CDesCArrayFlat* iFindStrings;
        CContactDatabase* iDatabaseNew;

        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      // T_MVPBKCONTACTSTORESINGLE_H

// End of file
