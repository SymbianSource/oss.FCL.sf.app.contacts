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


#ifndef T_MVPBKCONTACTSTORECOMPOSITE_H
#define T_MVPBKCONTACTSTORECOMPOSITE_H

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <f32file.h>
#include <badesca.h>

//  INTERNAL INCLUDES
#include "MVPbkContactStoreListObserver.h"
#include "MVPbkContactViewObserver.h"
#include "MVPbkBatchOperationObserver.h"
#include "MVPbkContactObserver.h"

//  FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;
class CVPbkContactManager;
class MVPbkContactStoreList;
class CVPbkSortOrder;
class MVPbkStoreContact;
class MVPbkFieldType;
class TVPbkFieldTypeMapping;
class CVPbkContactStoreUriArray;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
    class T_MVPbkContactStoreComposite
     :  public CEUnitTestSuiteClass,
        public MVPbkContactStoreListObserver,
        public MVPbkContactViewObserver,
        public MVPbkBatchOperationObserver,
        public MVPbkContactObserver
    {
    public:     // Constructors and destructors

        static T_MVPbkContactStoreComposite* NewL(
            const RArray< TVPbkContactStoreUriPtr >& aUriList );
        static T_MVPbkContactStoreComposite* NewLC(
            const RArray< TVPbkContactStoreUriPtr >& aUriList );
        ~T_MVPbkContactStoreComposite();

    private:    // Constructors and destructors

        T_MVPbkContactStoreComposite();
        void ConstructL( const RArray< TVPbkContactStoreUriPtr >& aUriList );

    private:    // From MVPbkContactStoreListObserver

        void StoreReady( MVPbkContactStore& aContactStore );
        void StoreUnavailable( MVPbkContactStore& aContactStore,
            TInt aReason );
        void HandleStoreEventL( MVPbkContactStore& aContactStore, 
            TVPbkContactStoreEvent aStoreEvent );
	    void OpenComplete();		

    private:    // From MVPbkContactViewObserver
        void ContactViewReady( MVPbkContactViewBase& aView );
        void ContactViewUnavailable( MVPbkContactViewBase& aView );
        void ContactAddedToView( MVPbkContactViewBase& aView, TInt aIndex,
            const MVPbkContactLink& aLink );
        void ContactRemovedFromView( MVPbkContactViewBase& aView,
            TInt aIndex, const MVPbkContactLink& aLink );
        void ContactViewError( MVPbkContactViewBase& aView, TInt aError,
            TBool aErrorNotified );

    private:    // From MVPbkBatchOperationObserver

        void StepComplete( MVPbkContactOperationBase& aOperation,
            TInt aStepSize );
        TBool StepFailed( MVPbkContactOperationBase& aOperation,
            TInt aStepSize, TInt aError );
        void OperationComplete( MVPbkContactOperationBase& aOperation );

    private:    // From MVPbkContactObserver

        void ContactOperationCompleted( TContactOpResult aResult );
        void ContactOperationFailed( TContactOp aOpCode, TInt aErrorCode,
            TBool aErrorNotified );

    private:    // New methods

        void SetupL();
        void SetupEmptyL();
        void SetupStoreL();
        void SetupViewL( TInt aViewDefResId, TInt aSortOrderResId );
        void SetupSingleViewL();
        void SetupOrderedFlatCompositeViewL();
        void SetupOrderedComplexCompositeViewL();
        void SetupSortedFlatCompositeViewL();
        void SetupSortedComplexCompositeViewL();
        void SetupStoreWithContactsL();
        void SetupCreateFindStringsL();
        
        void Teardown();

        void TestGetStoreListL();
        void TestLoadContactStoreL();
        void TestOpeningAllStoresL();
        void TestViewContactCountL();
        void TestCommitMultipleContactsL();
        void TestDeleteMultipleContactsL();
        void TestCreateFilteredViewL();
        void TestCreateFilteredView2L();        
        void TestDeleteContactsFromFilteredViewL();

        void ReplaceAllStoresL();
        CVPbkSortOrder* CreateSortOrderLC( TInt aResId );
        void PopulateStoresL();        
        void AddContactFieldsAndSaveL( 
            MVPbkStoreContact& aStoreContact, 
            const TDesC& aFName, 
            const TDesC& aLName, 
            const TDesC& aTelNo );
        void AddValueToContactFieldL( MVPbkStoreContact& aStoreContact, 
            const MVPbkFieldType& aFieldType, const TDesC& aValue );
        void AddFieldToContact( 
            MVPbkStoreContact& aStoreContact, TInt aResId, const TDesC& aValue );
        TVPbkFieldTypeMapping CreateFieldTypeMappingL( const TInt aResId );
        void StartSchedulerAndDecideToLeaveL();
        void StartStoreSchedulerAndDecideToLeaveL( TInt aExpectedEvent );
        void StartContactSchedulerAndDecideToLeaveL( TInt aExpectedEvent );
        void StartViewSchedulerAndDecideToLeaveL( TInt aExpectedEvent );

    private:    // Data
        enum TLastEvent
            {
            EStoreReady,
            EOpenError,
            EOpenComplete,
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
            EOperationCancelled
            } iLastEvent;
        
        TInt iLastError;
        RFs iFs;
        TBool iFlag;
        CDesCArrayFlat* iStorePaths;
        CVPbkContactStoreUriArray* iUriArray;
        CVPbkContactManager* iContactManager;
        MVPbkContactStoreList* iStoreList;
        MVPbkContactViewBase* iView;
        MVPbkStoreContact* iStoreContact;
        
        TInt iExpectedStoreEvent;
        CActiveSchedulerWait iStoreScheduler;
        TBool iExpectedStoreResponseFound;
        
        TInt iExpectedContactEvent;
        CActiveSchedulerWait iContactScheduler;
        TBool iExpectedContactResponseFound;
        
        TInt iExpectedViewEvent;
        CActiveSchedulerWait iViewScheduler;
        TBool iExpectedViewResponseFound;
        
        /// Own:
        CDesCArrayFlat* iFindStrings;

	    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      // T_MVPBKCONTACTSTORECOMPOSITE_H

// End of file
