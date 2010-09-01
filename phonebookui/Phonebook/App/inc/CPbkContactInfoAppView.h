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
*     Contact Info View controller (CAknView).
*
*/


#ifndef __CPbkContactInfoAppView_H__
#define __CPbkContactInfoAppView_H__

//  INCLUDES
#include "CPbkMemoryEntryAppView.h"  // Memory entry views super class
#include "MPbkContactInfoNavigationCallback.h"
#include <TPbkSendingParams.h>

//  FORWARD DECLARATIONS
class CPbkContactInfoControl;
template<class ControlType> class CPbkControlContainer;
class MPbkViewExtension;
class CPbkExtGlobals;
class MPbkContactInfoNavigationStrategy;
class CContactTextField;
class CPbkCodImageCmd;
class CPbkFFSCheck;

//  CLASS DECLARATION

/**
 * Phonebook "Contact Info View" application view class. 
 */
class   CPbkContactInfoAppView :
        public CPbkMemoryEntryAppView,
        public MPbkContactInfoNavigationCallback
    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         * @param aEngine Contact engine
         */
        static CPbkContactInfoAppView* NewL();

        /**
         * Destructor.
         */
        ~CPbkContactInfoAppView();

    private:  // Command handlers (use "Cmd" prefix)
        void CmdContextMenuL();
        void CmdEditMeL();
        void CmdDefaultSettingsL();
        void CmdAssignSpeedDialL();
        void CmdRemoveSpeedDialL();
        void CmdPersonalRingingToneL();
        void CmdSendContactL(TPbkSendingParams aParams);
        void CmdWriteToContactL(TPbkSendingParams aParams);
        void CmdCopyToSimL();
        void CmdGoToURLL();
        void CmdPrependL();
        void CmdCopyNumberL();
        void CmdRemoveCodL(TInt aFieldType);
        void CmdAddCodFieldL(TInt aCommandId);
        void CmdEditTextCodFieldL();
        void CmdViewCodImageL();
    private: // from CPbkMemoryEntryAppView
        void ContactChangedL();
        CPbkContactItem* ContactItemL();
        TContactItemId ContactItemId();

    public:  // from CPbkAppView
        CPbkViewState* GetViewStateLC() const;
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // from CAknView
        void HandleCommandL(TInt aCommand);
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        TUid Id() const;
        void HandleStatusPaneSizeChange();
        void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();
        
    private: // from MPbkContactInfoNavigationCallback
        void ChangeContactL(TContactItemId aContactId);
        CEikStatusPane* GetStatusPane() const;
        TContactItemId GetContactItemId() const;

    private: // Implementation
        CPbkContactInfoAppView();
        void ConstructL();
        void OptionsMenuFilteringL(CEikMenuPane& aMenuPane, TUint aFlags);
        void ContextMenuFilteringL(CEikMenuPane& aMenuPane, TUint aFlags);
        TPbkSendingParams CreateWriteParamsLC();
        TPbkSendingParams CreateSendParamsLC();
        void UpdateUrlFieldInfo();
        void ConstructNaviIndicatorsL();
        void NavigateContactL(TInt aDir);
        void StatusPaneUpdateL(TContactItemId aContactId);
        void AddObjectSubPaneMenuFilteringL(CEikMenuPane& aMenuPane, 
            TUint aFlags);
        void UpdateL( CPbkContactEngine& aPbkContactEngine,
            CPbkContactItem& aContactItem, 
            CPbkContactInfoControl& aPbkContactInfoControl );
        TBool HandleCodTextChangeL(TContactItemId aId) const;
        TBool HandleCodImageChangeL(TContactItemId aId);
        CPbkCodImageCmd& PbkCodImageCmdL();
        TBool CodTextFieldEnabled() const;
        TBool CodImageFieldEnabled() const;
        static TInt UpdateContact(TAny* aPbkContactInfoAppView);
        TInt DoUpdateContact();

    private:  // data
        /// This view's control container type
        typedef CPbkControlContainer<CPbkContactInfoControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Own: this view's UI state. Used for storing/restoring the state.
        CPbkViewState* iViewState;
        /// Own: Contact Item to display
        CPbkContactItem* iContactItem;
        /// Own: for extension menu filtering and command handling
        MPbkViewExtension* iViewExtension;
        /// Own: for accessing extension factory
        CPbkExtGlobals* iExtGlobal;
        /// Own: does the contact have an url field
        TBool iUrlFieldAvailable;
        /// Own: Navigation strategy
        MPbkContactInfoNavigationStrategy* iNaviStrategy;
        /// Own: Command handler for Cod image
        CPbkCodImageCmd* iPbkCodImageCmd;
        /// Own: Flash File System check helper
        CPbkFFSCheck* iPbkFFSCheck;
        /// Own: Exit in progress flag
        TBool iExitInProgress;
        /// Own: This invokes update contact
        CIdle* iUpdateContact;
        /// Own: Editor sets saved contact id here
        TContactItemId iSavedId;
    };


#endif // __CPbkContactInfoAppView_H__
      
// End of File
