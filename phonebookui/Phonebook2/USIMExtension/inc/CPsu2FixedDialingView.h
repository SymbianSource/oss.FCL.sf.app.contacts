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
* Description:  Phonebook 2 USIM UI Extension FDN names list view.
*
*/


#ifndef CPSU2FIXEDDIALINGVIEW_H
#define CPSU2FIXEDDIALINGVIEW_H

//  INCLUDES
#include "CPsu2NameListViewBase.h"
#include <MPbk2EditedContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <aknmarkingmodeobserver.h>


// FORWARD DECLARATIONS
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkFieldType;
class CPsu2FixedDialingCall;
class MVPbkContactLink;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MPbk2DialogEliminator;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension FDN names list view.
 */
class CPsu2FixedDialingView : public CPsu2NameListViewBase,
                              public MPbk2EditedContactObserver,
                              public MVPbkSingleContactOperationObserver,
                              public MVPbkContactObserver,
                              public MAknMarkingModeObserver
    {
    public:  // Constructors and destructor
       
        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionView    The view from the extension framework.
         * @param aViewManager      The view manager of the USIM extension.
         * @return  A new instance of this class.
         */
        static CPsu2FixedDialingView* NewL(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        
        /**
         * Destructor.
         */
        ~CPsu2FixedDialingView();

    public: // From MPbk2UIExtensionView
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );            
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
    public: // From MPbk2ControlObserver
        void HandleControlEventL(
            MPbk2ContactUiControl& aControl,
            const TPbk2ControlEvent& aEvent );
        
    public: // From MAknMarkingModeObserver 
            
        /**
         * This method is called when marking mode is activated or deactivated.
         * 
         * @param aActivated @c ETrue if marking mode was activate, @c EFalse
         *                   if marking mode was deactivated.
         */
        void MarkingModeStatusChanged( TBool aActivated );

        /**
         * This method is called just before marking mode is closed. Client can 
         * either accept or decline closing.
         * 
         * @return @c ETrue if marking mode should be closed, otherwise @c EFalse.
         */
        TBool ExitMarkingMode() const;


    private: // From CPsu2NameListViewBase
        void UpdateCbasL();
        TInt NameListControlResourceId() const;

    private: // From MPbk2EditedContactObserver
        void ContactEditingComplete(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingDeletedContact(
                MVPbkStoreContact* aEditedContact );
        void ContactEditingAborted();
        
    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError );                
    
    private: // From MVPbkContactObserver        
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed(
                TContactOp aOpCode, 
                TInt aErrorCode, 
                TBool aErrorNotified );       
    public: // Implementation
        static TInt WaitOtherCompleteL( TAny* object );
        
    private:
        void CheckFDNActivityL();

    private: // Implementation
        CPsu2FixedDialingView(
                CPbk2UIExtensionView& aExtensionView,
                CPsu2ViewManager& aViewManager );
        void ConstructL();
        void UpdateNaviPaneTextL();
        void DestroyNaviPaneText();
        void OpenInfoViewCmdL(
                MPbk2ContactUiControl& aUiControl ) const;
        void ActivateFDNCmdL();
        void DeactivateFDNCmdL();
        void CreateNewFdnContactL();
        void AddFindTextL(
                MVPbkStoreContact& aContact );
        TBool FdnStoreFullL();
        void CreateCallL(
                TInt aCommand );
        void EditFdnContactL();                
        
    private: // Data
        /// Ref: Navipane for setting activity text to
        CAknNavigationControlContainer* iNaviPane;
        /// Ref: Decorator for navipane
        CAknNavigationDecorator* iNaviDecorator;
        /// Ref: FDN Store
        MVPbkContactStore* iFdnStore;
        /// Own: FDN calling support
        CPsu2FixedDialingCall* iFdnCall;
        /// Own: FDN not active note is shown
        TBool iShowFdnNotActiveNote;
        /// Own: Link to contact to edit
        MVPbkContactLink* iContactLink;
        /// Own: The store contact to edit
        MVPbkStoreContact* iContact;
        /// Own: An operation to get a contact from the link
        MVPbkContactOperationBase* iContactRetriever;
        /// Own: ETrue, while CommandHandleL() is being executed
        TBool iCommandIsBeingHandled;
        //  Own: Wait for other things complete then pop up the note.
        CIdle* iWaitOtherComplete;
        
        /// Ref: Eliminator of current contact editing dialog
        MPbk2DialogEliminator* iDlgEliminator;
        // Flag to indicate whether Marking mode is active
        TBool iMarkingModeOn;

    };

#endif // CPSU2FIXEDDIALINGVIEW_H
            
// End of File
