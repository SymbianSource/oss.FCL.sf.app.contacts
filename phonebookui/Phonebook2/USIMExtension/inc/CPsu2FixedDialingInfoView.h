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
* Description:  Phonebook 2 USIM UI Extension FDN contact info view.
*
*/


#ifndef CPSU2FIXEDDIALINGINFOVIEW_H
#define CPSU2FIXEDDIALINGINFOVIEW_H

// INCLUDES
#include "CPsu2InfoViewBase.h"
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2EditedContactObserver.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class CPsu2FixedDialingCall;
class MVPbkContactLink;
class MVPbkStoreContactField;
class MVPbkStoreContact;
class MPbk2DialogEliminator;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension FDN contact info view.
 * Responsible for implementing FDN info view.
 */
class CPsu2FixedDialingInfoView : public CPsu2InfoViewBase,
                                  public MVPbkSingleContactOperationObserver,
                                  public MVPbkContactObserver,
                                  public MPbk2EditedContactObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionView    The view from the extension framework.
         * @param aViewManager      View manager.
         * @return  A new instance of this class.
         */
        static CPsu2FixedDialingInfoView* NewL(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );

        /**
         * Destructor.
         */
        ~CPsu2FixedDialingInfoView();

    public: // From MPbk2UIExtensionView
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();

    public: // From MPbk2ControlObserver
        void HandleControlEventL(
            MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent );
    private: // From MPbk2UIExtensionView
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );

    private: // From CPsu2InfoViewBase
        TInt NameListControlResourceId() const;
        HBufC* NaviPaneLabelL() const;
        void UpdateCbasL();
        
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError );                
    
    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingDeletedContact(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingAborted();
        
    private: // From MVPbkContactObserver        
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed(
                TContactOp aOpCode, 
                TInt aErrorCode, 
                TBool aErrorNotified );
                
    private: // Implementation
        CPsu2FixedDialingInfoView(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        void ConstructL();
        void CreateCallL(
                TInt aCommand );
        void EditFdnContactL();
        MVPbkStoreContactField* FocusedFieldLC( 
            MVPbkStoreContact& aStoreContact );

    private: // Data
        /// Own: FDN calling support
        CPsu2FixedDialingCall* iFdnCall;
        /// Own: Link to contact to edit
        MVPbkContactLink* iContactLink;
        /// Own: The store contact to edit
        MVPbkStoreContact* iContact;
        /// Own: An operation to get a contact from the link
        MVPbkContactOperationBase* iContactRetriever;
        /// Ref: Eliminator of current contact editing dialog
        MPbk2DialogEliminator* iDlgEliminator;
    };

#endif // CPSU2FIXEDDIALINGINFOVIEW_H

// End of File
