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
*      Phonebook Group members Fetch API.
*
*/


#ifndef __CPbkGroupMembersFetchDlg_H__
#define __CPbkGroupMembersFetchDlg_H__

//  INCLUDES
#include <e32base.h>    // CBase
#include <cntviewbase.h> // MContactViewObserver

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class CPbkFetchDlg;
class CContactGroupView;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook Group Members Fetch API.
 */
class CPbkGroupMembersFetchDlg : public CBase, private MContactViewObserver
    {
    public:  // Types
        /**
         * Fetch parameters class.
         */
        class TParams
            {
            public:  // Input parameters
                /**
                 * MANDATORY: Contact group to get label and show query.
                 */
                TContactItemId iGroupId;

            public:  // Output parameters
                /**
                 * Returns the marked entries.
                 * Caller must delete iMarkedEntries when no longer needed.
                 */
                CContactIdArray* iMarkedEntries;

            public:  // Functions
                /**
                 * C++ Constructor.
                 */
                IMPORT_C TParams();

                /**
                 * Returns a cleanup item which will release all resources
                 * held by this object in case a leave occurs.
                 */
                IMPORT_C operator TCleanupItem();

                // Compiler-generated destructor is ok for this class

            private:  // Implementation
                static void Cleanup(TAny* aPtr);

            private:  // Unimplemented functions
                /// Unimplemented copy constructor
                TParams(const TParams&);
                /// Unimplemented assignment operator
                TParams& operator=(const TParams&);
            };

    public:  // Interface
        /**
         * Creates a new group members fetch dialog.
         *
         * @param aParams @see TParams
         * @param aPbkEngine reference to phonebook engine
         */
        IMPORT_C static CPbkGroupMembersFetchDlg* NewL(TParams& aParams,
            CPbkContactEngine& aPbkEngine);

        /**
         * Executes this dialog.
         * NOTE: this object is deleted when ExecuteLD returns or leaves!
         *
         * @return !=0 if not canceled.
         */
        IMPORT_C TInt ExecuteLD();

        /**
         * Destructor. Also makes ExecuteLD() return like the dialog was
         * cancelled.
         */
        ~CPbkGroupMembersFetchDlg();

    private: // from MContactViewObserver
        void HandleContactViewEvent(const CContactViewBase& aView,
            const TContactViewEvent& aEvent);

    private:  // Implementation
        CPbkGroupMembersFetchDlg(TParams& aParams,
            CPbkContactEngine& aPbkEngine);
        void ConstructL();

    private:  // Data
        /// Own: parameters
        TParams& iParams;
        /// Ref: Phonebook Engine
        CPbkContactEngine& iPbkEngine;
        /// Own: fetch dialog
        CPbkFetchDlg* iFetchDlg;
        /// Ref: Referred TBool is set ETrue in destructor
        TBool* iDestroyedPtr;
        /// Own: Group non members sub view
        CContactGroupView* iGroupNonMembersSubView;
    };

#endif // __CPbkGroupMembersFetchDlg_H__

// End of File
