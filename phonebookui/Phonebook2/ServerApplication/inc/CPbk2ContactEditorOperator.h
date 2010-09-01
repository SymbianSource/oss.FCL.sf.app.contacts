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
* Description:  Phonebook 2 server app contact editor operator.
*
*/


#ifndef CPBK2CONTACTEDITOROPERATOR_H
#define CPBK2CONTACTEDITOROPERATOR_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ExitCallback.h>
#include <MPbk2DialogEliminator.h>
#include <coehelp.h> // TCoeHelpContext

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MPbk2EditedContactObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 server app contact editor operator.
 * Responsible for implementing all server app contact editor operations.
 */
class CPbk2ContactEditorOperator :  public CBase,
                                    public MPbk2ExitCallback,
                                    public MPbk2DialogEliminator
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExitCallback     Exit callback.
         * @param aContactObserver  Observer for the edited contact.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorOperator* NewLC(
                MPbk2ExitCallback& aExitCallback,
                MPbk2EditedContactObserver& aContactObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactEditorOperator();

    public: // Interface

        /**
         * Opens given contact in editor.
         *
         * @param aStoreContact The store contact to edit.
         * @param aIndex        Index of the newly added field,
         *                      pass KErrNotFound if nothing was added.
         *                      This must be store index.
         * @param aFlags        Assign flags.
         * @param aHelpContext  Editor help context.
         */
        void OpenEditorL(
                MVPbkStoreContact*& aStoreContact,
                TInt& aIndex,
                TUint& aFlags,
                TCoeHelpContext& aHelpContext );

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                 TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    private: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // Implementation
        CPbk2ContactEditorOperator(
                MPbk2ExitCallback& aExitCallback,
                MPbk2EditedContactObserver& aContactObserver );

    private: // Data
        /// Ref: Contact editor eliminator
        MPbk2DialogEliminator* iContactEditorEliminator;
        /// Ref: Exit callback
        MPbk2ExitCallback& iExitCallback;
        /// Ref: Observer for the edited contact.
        MPbk2EditedContactObserver& iContactObserver;
        /// Ref: Set to NULL when the execution enters to destructor
        MPbk2DialogEliminator** iSelfPtr;
    };

#endif // CPBK2CONTACTEDITOROPERATOR_H

// End of File
