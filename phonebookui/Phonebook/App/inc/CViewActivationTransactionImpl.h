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
*     Phonebook transaction view implementation class methods.
*
*/


#ifndef __CViewActivationTransactionImpl_H__
#define __CViewActivationTransactionImpl_H__

//  INCLUDES
#include "CPbkAppUi.h"

// FORWARD DECLARATIONS
class CDigViewNode;
class CAknTitlePane;
class CAknContextPane;
class CEikImage;
class CAknNavigationDecorator;
class CAknTabGroup;


// CLASS DECLARATION

/**
 * Transaction class for updating application-wide state leave-safely
 * at view switch.
 * @see CPbkAppUi::HandleViewActivationLC
 */
class CPbkAppUi::CViewActivationTransactionImpl
        : public CPbkAppUiBase::CViewActivationTransaction
    {
    public:  // Interface
        /**
         * Creates an instace of this transaction, performs all update 
         * operations of this transaction and saves rollback data in case 
         * the transaction is not committed.
         *
         * @param aAppUi phonebook's application UI
         * @param aViewId activated view's id
         * @param aPrevViewId previously active view's id
         * @param aTitlePaneText text to put into title pane, if NULL
         *        default text is used
         * @param aContextPanePicture picture to put into context pane, if
         *        NULL default picture is used
         * @return instance of this class
         */
        static CViewActivationTransactionImpl* NewLC
            (CPbkAppUi& aAppUi, 
            const TUid& aViewId, const TVwsViewId& aPrevViewId,
            const TDesC* aTitlePaneText, 
            const CEikImage* aContextPanePicture,
            TUint aFlags);

        /**
         * Destructor. Rolls back changes done in NewLC if this transaction
         * is not committed.
         */
        ~CViewActivationTransactionImpl();

    public:  // From CViewActivationTransaction
        /**
         * Commits this transaction. After this call all changes done in
         * NewLC are kept.
         */
        void Commit();

    private:  // Implementation
		CViewActivationTransactionImpl(CPbkAppUi& aAppUi, const TUid& aViewId,
			const TVwsViewId& aPrevViewId, TUint aFlags);
		void ConstructL(const TDesC* aTitlePaneText,
			const CEikImage* aContextPanePicture);
        void UpdateViewNavigationState();
        void UpdateNaviPaneL(TUid aViewId);
        void SetTitlePaneTextL(const TDesC* aTitlePaneText);
        void SetContextPanePictureL(const CEikImage* aContextPanePicture);
        CEikImage* CopyL(const CEikImage& aSrc);

    private:  // Data
        /// Ref: Phonebook Application UI
        CPbkAppUi* iAppUi;
        /// Own: Activated view's id
        TUid iViewId;
        /// Own: Deactivated view's id
        TVwsViewId iPrevViewId;
        /// Own: flags
        TUint iFlags;
        /// Ref: Saved previous view node state
        CDigViewNode* iSavePreviousViewNode;
        /// Ref: Application's title pane
        CAknTitlePane* iTitlePane;
        /// Own: Saved title pane text
        HBufC* iSaveTitlePaneText;
        /// Ref: Application's context pane
        CAknContextPane* iContextPane;
        /// Own: Saved picture from context pane
        CEikImage* iSaveContextPanePicture;
        /// Ref: navigation decorator pushed on the navi pane, if NULL
        /// nothing was pushed.
        CAknNavigationDecorator* iPushedNaviDecorator;
        /// Ref: Modified tab group
        CAknTabGroup* iTabGroup;
        /// Own: Saved index of active tab in iTabGroup
        TInt iSaveTabIndex;
    };


#endif // __CViewActivationTransactionImpl_H__
            
// End of File
