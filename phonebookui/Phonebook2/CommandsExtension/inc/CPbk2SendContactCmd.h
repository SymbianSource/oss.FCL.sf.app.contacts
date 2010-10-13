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
* Description:  Phonebook 2 send contact command.
*
*/


#ifndef CPBK2SENDCONTACTCMD_H
#define CPBK2SENDCONTACTCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>
#include "MPbk2vCardConverterObserver.h"
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class CIdle;
class MVPbkStoreContactField;
class CPbk2vCardConverter;
class CEikonEnv;
class MVPbkBaseContactField;
class MVPbkContactLinkArray;
class MPbk2CommandObserver;
class CVPbkVCardEng;
class MVPbkStoreContact;
class CPbk2vCardConverter;
class CAknWaitDialog;
class MVPbkFieldType;
class MPbk2ContactUiControl;
class MVPbkContactOperationBase;
class MVPbkFieldTypeList;
class TPbk2SendingParams;
class MVPbkContactLink;
class CMessageData;
class MPbk2ProcessDecorator;
class MPbk2ContactLinkIterator;
class CPbk2ApplicationServices;
class CSendUi;

// CLASS DECLARATION

/**
 * Phonebook 2 send contact command.
 */
NONSHARABLE_CLASS(CPbk2SendContactCmd) :
        public CActive,
        public MPbk2Command,
        public MPbk2vCardConverterObserver,
        public MVPbkSingleContactOperationObserver,
        private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2SendContactCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        virtual ~CPbk2SendContactCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // From MPbk2vCardConverterObserver
        void ConversionDone(
                TInt aCount );
        void ConversionError(
                TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    public: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // Implementation
        CPbk2SendContactCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void SendvCardsLD();
        static TInt SendvCardsLD(
                TAny* aThis );
        TBool AnyThumbnailsL() const;
        TInt SelectionListL() const;
        void MapSelection(
                TInt& aSelection,
                TInt aShownMenu );
        void ProcessDone(
                TInt aCancelCode );
        TInt SelectSentDataL();
        TUid ShowSendQueryL();
        TPbk2SendingParams CreateParamsLC();
        void ConvertContactL(
                TInt aSelectionIndex );
        void RetrieveContactL(
                const MVPbkContactLink& aLink );
        TInt FilterErrors(
                TInt aErrorCode);
        void IssueRequest();
        void IssueStopRequest();
        void SendMsgUsingSendUI(
                CMessageData* aMsgData );
        TBool IsMoreThanOneContact();

    private: // Data structures
        /// Process states
        enum TState
            {
            EShowingSendQuery = 0,
            ERetrievingContact,
            ESelectingSentData,
            EStopping,
            ECancelling
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Selected contacts
        MVPbkContactLinkArray* iSelectedContacts;
        /// Ref: Focused field
        const MVPbkBaseContactField* iFocusedField;
        /// Own: Send command's service uid
        TUid iMtmUid;
        /// Own: vCard engine
        CVPbkVCardEng* iVCardEngine;
        /// Own: vCard converter
        CPbk2vCardConverter* iConverter;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
        /// Own: vCard sender
        CIdle* iVCardSender;
        /// Own: Indicates whether this object is under destruction
        TBool iUnderDestruction;
        /// Own: Store contact array
        RPointerArray<MVPbkStoreContact> iStoreContacts;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: Thumbnail field type
        const MVPbkFieldType* iThumbnailFieldType;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Current index of the contact in an array
        TInt iCurrentContactLinkIndex;
        /// Own: State of the process
        TState iState;
        /// Own: contact iterator
        MPbk2ContactLinkIterator* iCntIterator;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
        /// Own: Send UI
        CSendUi* iSendUi;
    };

#endif // CPBK2SENDCONTACTCMD_H

// End of File
