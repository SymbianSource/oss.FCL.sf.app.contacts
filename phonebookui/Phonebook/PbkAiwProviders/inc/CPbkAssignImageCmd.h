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
*     Implements assign image functionality.
*
*/


#ifndef __CPbkAssignImageCmd_H__
#define __CPbkAssignImageCmd_H__

//  INCLUDES
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkThumbnailOperationObservers.h>
#include <PbkFields.hrh>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class CAknWaitDialog;
class CPbkThumbnailManager;
class MPbkThumbnailOperation;
class MAiwNotifyCallback;
class CAiwGenericParamList;
class CPbkFFSCheck;

//  CLASS DECLARATION 

class CPbkAssignImageCmd :       
        public CActive,
        public MPbkCommand,
        private MPbkThumbnailSetObserver,
        private MPbkThumbnailGetObserver
    {
    public: // constructor & destructor
        /**
         * Creates a new object of this class.
         * @param aFileName file name of the image to assign
         * @param aContacts contacts to process, ownership is taken
         * @param the field id to use as a target
         * @param aEngine reference to Phonebook contact engine
         * @param aInParamList AIW in-param list related to this command
         * @param aNotifyCallback AIW notify callback object, optional
         */
        static CPbkAssignImageCmd* NewL
            (const TDesC& aFileName,
            CContactIdArray* aContacts,
            TPbkFieldId aFieldId,
            CPbkContactEngine& aEngine,
            const CAiwGenericParamList& aInParamList,
            MAiwNotifyCallback* aNotifyCallback);
        ~CPbkAssignImageCmd();

    public: // From MPbkCommand
        void ExecuteLD();
        void AddObserver(MPbkCommandObserver& aObserver);

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // from MPbkThumbnailSetObserver
        void PbkThumbnailSetComplete(MPbkThumbnailOperation& aOperation);
        void PbkThumbnailSetFailed(MPbkThumbnailOperation& aOperation,
            TInt aError);

    private: // from MPbkThumbnailGetObserver
        void PbkThumbnailGetComplete
            (MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap);
        void PbkThumbnailGetFailed
            (MPbkThumbnailOperation& aOperation, TInt aError);

    private: // implementation
        CPbkAssignImageCmd(const TDesC& aFileName,
            CContactIdArray* aContacts,
            TPbkFieldId aFieldId,
            CPbkContactEngine& aEngine,
            const CAiwGenericParamList& aInParamList,
            MAiwNotifyCallback* aNotifyCallback);
        void ConstructL();
        void ShowWaitNoteL();
        void DismissWaitNoteL();
        void ShowEndNotesL();
        void AssignImageL();
        void FetchImageL();
        void DoSendNotifyEventL(TInt aEvent);
        void IssueRequest();
        void SetImageAsyncL();
        void IssueStopRequest(TInt aError);
		TBool ConfirmAssignL(CPbkContactItem& aItem, TPbkFieldId aId);

    private: // data
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: contact currently under assign
        CPbkContactItem* iItem;
        /// Own: wait note dialog
        CAknWaitDialog* iWaitNote;
        /// Own: image operation
        MPbkThumbnailOperation* iImageOperation;
        /// Own: image manager
        CPbkThumbnailManager* iImageManager;
        /// Own: number of contacts the image was added to
        TInt iImagesAdded;
        /// Own: the image to set
        CFbsBitmap* iBitmap;
        /// Ref: command observer
        MPbkCommandObserver* iCmdObserver;
        /// Ref: contacts to process
        CContactIdArray* iContacts;
        /// Own: id of the field to be used as a target
        TPbkFieldId iFieldId;
        /// Own: filename of the image to be assigned
        TFileName iFileName;
        /// process states
        enum TAssignState { EAssigningFirst, EFetching, EAssigning, EStopping };
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

#endif // __CPbkAssignImageCmd_H__

// End of File
