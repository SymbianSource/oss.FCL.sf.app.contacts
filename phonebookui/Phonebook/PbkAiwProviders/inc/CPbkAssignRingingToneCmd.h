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
*     Implements assign ringing tone functionality.
*
*/


#ifndef __CPbkAssignRingingToneCmd_H__
#define __CPbkAssignRingingToneCmd_H__

//  INCLUDES
#include <cntdef.h>
#include <MPbkCommand.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class CAknWaitDialog;
class MAiwNotifyCallback;
class CAiwGenericParamList;
class CPbkFFSCheck;

//  CLASS DECLARATION 

class CPbkAssignRingingToneCmd :       
        public CActive,
        public MPbkCommand
    {
    public: // constructor & destructor
        /**
         * Creates a new object of this class.
         * @param aFileName file name of the ringing tone to assign
         * @param aContacts contacts to process, ownership is taken
         * @param aEngine reference to Phonebook contact engine
         * @param aInParamList AIW in-param list related to this command
         * @param aNotifyCallback AIW notify callback object, optional
         */
        static CPbkAssignRingingToneCmd* NewL
            (const TDesC& aFileName,
            CContactIdArray* aContacts,
            CPbkContactEngine& aEngine,
            const CAiwGenericParamList& aInParamList,
            MAiwNotifyCallback* aNotifyCallback);
            
        /**
         * Destructor.
         */
        ~CPbkAssignRingingToneCmd();

    public: // From MPbkCommand
        void ExecuteLD();
        void AddObserver(MPbkCommandObserver& aObserver);

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // implementation
        CPbkAssignRingingToneCmd(const TDesC& aFileName,
            CContactIdArray* aContacts,
            CPbkContactEngine& aEngine,
            const CAiwGenericParamList& aInParamList,
            MAiwNotifyCallback* aNotifyCallback);
        void ConstructL();
        void ShowWaitNoteL();
        void DismissWaitNoteL();
        void ShowEndNotesL();
        void AssignRingingToneL();
        void FetchThumbnailL();
        void DoSendNotifyEventL(TInt aEvent);        
        void IssueRequest();
        void IssueStopRequest(TInt aError);
        void SetRingingToneL();
        TBool ConfirmAssignL(CPbkContactItem& aItem);

    private: // data
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: contact currently under thumbnail assign
        CPbkContactItem* iItem;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: number of contacts the ringing tone was added to
        TInt iTonesAdded;
        /// Ref: command observer
        MPbkCommandObserver* iCmdObserver;
        /// Ref: contacts to sync
        CContactIdArray* iContacts;
        /// Own: filename of the image to be assigned
        TFileName iFileName;
        /// process states
        enum TAssignState { EAssigning, EStopping };
        /// Own: current state of process
        TAssignState iState;
        /// Ref: AIW in-param list related to this command
        const CAiwGenericParamList& iInParamList;
        /// Ref: AIW notify callback
        MAiwNotifyCallback* iNotifyCallback;
        /// Own: contact title for end note
        HBufC* iTitleForNote;
        /// Own: Flash File System check helper
        CPbkFFSCheck* iPbkFFSCheck;
    };

#endif // __CPbkAssignRingingToneCmd_H__

// End of File
