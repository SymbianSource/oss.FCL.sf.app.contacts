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
* Description:  Phonebook 2 view activation transaction.
*
*/


#ifndef CPBK2VIEWACTIVATIONTRANSACTION_H
#define CPBK2VIEWACTIVATIONTRANSACTION_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ViewActivationTransaction.h>
#include <vwsdef.h> // TVwsViewId

// FORWARD DECLARATIONS
class CPbk2ViewNode;
class CAknTitlePane;
class CAknContextPane;
class CEikImage;
class CAknNavigationDecorator;
class CAknTabGroup;
class CPbk2AppUi;
class CPbk2ViewExplorer;
class CAknNavigationControlContainer;

// CLASS DECLARATION

/**
 * Phonebook 2 view activation transaction.
 * Transaction class for updating application-wide state leave-safely
 * at a view switch.
 */
class CPbk2ViewActivationTransaction : public CBase,
                                       public MPbk2ViewActivationTransaction
    {
    public: // Constructors and destructor

        /**
         * Creates an instace of this transaction, performs all update
         * operations of this transaction and saves rollback data in case
         * the transaction is not committed.
         *
         * @param aAppUi                Phonebook 2's application UI.
         * @param aViewExplorer         Phonebook 2 view explorer.
         * @param aViewId               Id of the view to be activated.
         * @param aPrevViewId           Previously active view's id.
         * @param aTitlePaneText        Text to place into the title pane,
         *                              if NULL default text is used.
         * @param aContextPanePicture   Picture to place into context pane,
         *                              if NULL default picture is used.
         * @param aFlags                View actication flags.
         * @return  A new instance of this class.
         */
        static CPbk2ViewActivationTransaction* NewLC(
                CPbk2AppUi& aAppUi,
                CPbk2ViewExplorer& aViewExplorer,
                const TUid& aViewId,
                const TVwsViewId& aPrevViewId,
                const TDesC* aTitlePaneText,
                const CEikImage* aContextPanePicture,
                TUint aFlags );

        /**
         * Destructor. Rolls back changes done in NewLC if this transaction
         * is not committed.
         */
        ~CPbk2ViewActivationTransaction();

    public: // From MPbk2ViewActivationTransaction
        void Commit();
        void RollbackL();

    private: // Implementation
        CPbk2ViewActivationTransaction(
                CPbk2AppUi& aAppUi,
                CPbk2ViewExplorer& aViewExplorer,
                const TUid& aViewId,
                const TVwsViewId& aPrevViewId,
                TUint aFlags );
        void ConstructL(
                const TDesC* aTitlePaneText,
                const CEikImage* aContextPanePicture );
        void UpdateViewNavigationState();
        void UpdateNaviPaneL(
                TUid aViewId );
        void SetTitlePaneTextL(
                const TDesC* aTitlePaneText );
        void SetContextPanePictureL(
                const CEikImage* aContextPanePicture );
        CEikImage* CopyImageL(
                const CEikImage& aSrc );
        void Rollback();

    private: // Data
        /// Ref: Phonebook 2 Application UI
        CPbk2AppUi* iAppUi;
        /// Ref: Phonebook 2 view explorer
        CPbk2ViewExplorer& iViewExplorer;
        /// Own: Activated view's id
        TUid iViewId;
        /// Own: Deactivated view's id
        TVwsViewId iPrevViewId;
        /// Own: Flags
        TUint iFlags;
        /// Ref: Saved previous view node state
        CPbk2ViewNode* iSavePreviousViewNode;
        /// Ref: Application's title pane
        CAknTitlePane* iTitlePane;
        /// Own: Saved title pane text
        HBufC* iSaveTitlePaneText;
        /// Ref: Application's context pane
        CAknContextPane* iContextPane;
        /// Own: Saved picture from context pane
        CEikImage* iSaveContextPanePicture;
        /// Ref: Navigation decorator pushed on the navi pane,
        /// if NULL nothing was pushed
        CAknNavigationDecorator* iPushedNaviDecorator;
        /// Ref: Modified tab group
        CAknTabGroup* iTabGroup;
        /// Own: Saved index of active tab in iTabGroup
        TInt iSaveTabIndex;
        /// Ref: Navi pane
        CAknNavigationControlContainer* iNaviPane;
    };

#endif // CPBK2VIEWACTIVATIONTRANSACTION_H

// End of File
