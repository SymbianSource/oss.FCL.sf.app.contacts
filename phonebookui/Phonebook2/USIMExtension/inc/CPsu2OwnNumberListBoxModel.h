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
* Description:  Own number listbox model
*
*/



#ifndef CPSU2OWNNUMBERLISTBOXMODEL_H
#define CPSU2OWNNUMBERLISTBOXMODEL_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactViewObserver.h>
#include <badesca.h> // Array

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;
class MVPbkViewContact;
class CCoeControl;

// CLASS DECLARATION

/**
 *  Own number listbox model.
 *
 *  @lib Pbk2USimUI.dll
 */
class CPsu2OwnNumberListBoxModel : 
        public CActive, 
        public MDesCArray,
        private MVPbkContactObserver,
        private MVPbkContactViewObserver
    {
    public:  // Constructors and destructor
        /**
         * @param aView the view from the extension framework
         * @param aContainer Window is owned here
         * Two-phased constructor.
         */
        static CPsu2OwnNumberListBoxModel* NewL( MVPbkContactViewBase& aView,
            CCoeControl& aObserver );
        
        /**
         * Destructor.
         */
        virtual ~CPsu2OwnNumberListBoxModel();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError);
                
    private: // from MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;  
        
    private: // from MVPbkContactObserver
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed(
                TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);                      
                
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

    private: // New functions
        
        /**
         * Formats given contact information to listbox string and
         * puts it in array.         
         * @param aContact Own number contact 
         */
        void AddToArrayL( MVPbkStoreContact* aContact );

        /**
         * Starts async contact reading
         */
        void ReadContactsL();
       
        /**
         * Refresh contacts. Async operation
         */
        void RefreshL();
        
    private: // implementation
        CPsu2OwnNumberListBoxModel( MVPbkContactViewBase& aView,
            CCoeControl& aObserver );
        void ConstructL();
        void IssueRequest();

    private:    // Data
        /// Ref: View which contains the own number 'contacts'
        MVPbkContactViewBase& iView;
        /// Own: Stores formatted strings for listbox 
        CDesCArraySeg* iArray;
        /// Own: Currently loading contact index
        TInt iCurrentContact;
        /// Ref: This is called after 'contacts' are loaded        
        CCoeControl& iObserver;
    };

#endif      // CPSU2OWNNUMBERLISTBOXMODEL_H 
            
// End of File
