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
* Description:  Phonebook 2 contact editor event observer interface.
*
*/


#ifndef MPBK2CONTACTEDITOREVENTOBSERVER_H
#define MPBK2CONTACTEDITOREVENTOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <MVPbkContactObserver.h>

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor event observer interface.
 */
class MPbk2ContactEditorEventObserver
    {
    public: // Data types

        /**
         * Editor flags.
         */
        enum TPbk2ContactEditorFlag
            {
            /// Command id
            EPbk2EditorCommandId        = 0x00000001,
            /// Key code
            EPbk2EditorKeyCode          = 0x00000002
            };

        /**
         * Additional parameters for contact editor events.
         */
        struct TParams
            {
            /// A collection of TPbk2ContactEditorFlags
            TUint32 iFlags;
            /// This must be set if the flag EPbk2EditorCommandId is set
            TInt iCommandId;
            /// This must be set if the flag EPbk2EditorKeyCode is set
            TInt iKeyCode;
            };

        /**
         * Parameters for operation failed callback.
         */
        struct TFailParams
            {
            /// Indicates whether the error was notified
            TBool iErrorNotified;
            /// Indicates whether the editor should be closed
            TBool iCloseEditor;
            /// Internal error code
            TInt iErrorCode;
            /// Spare data
            TInt32 iSpare;
            };


    public: // Interface

        /**
         * Called when a contact editor operation has succesfully completed.
         *
         * @param aResult           Result of the operation.
         * @param aParams           Parameters of the contact event.
         */
        virtual void ContactEditorOperationCompleted(
                MVPbkContactObserver::TContactOpResult aResult,
                TParams aParams ) = 0;

        /**
         * Called when a contact editor operation has failed.
         *
         * @param aOpCode           The operation that failed.
         * @param aErrorCode        System error code of the failure.
         *                          KErrAccessDenied (when EContactCommit)
         *                          means that the contact has not been
         *                          locked.
         * @param aParams           Parameters of the event.
         * @param aFailParams       Extra params for error handling.
         * @return  Error code.
         */
        virtual void ContactEditorOperationFailed(
                MVPbkContactObserver::TContactOp aOpCode,
                TInt aErrorCode,
                TParams aParams,
                TFailParams& aFailParams ) = 0;

    protected: // Protected destructor
        ~MPbk2ContactEditorEventObserver()
                {}
    };

#endif // MPBK2CONTACTEDITOREVENTOBSERVER_H

// End of File
