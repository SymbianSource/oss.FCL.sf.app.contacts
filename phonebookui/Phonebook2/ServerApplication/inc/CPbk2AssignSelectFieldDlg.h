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
* Description:  Phonebook 2 select field dialog for assign service.
*
*/


#ifndef CPBK2ASSIGNSELECTFIELDDLG_H
#define CPBK2ASSIGNSELECTFIELDDLG_H

// INCLUDES
#include <e32base.h>
#include <MPbk2KeyEventHandler.h>
#include <MPbk2DialogEliminator.h>
#include <babitflags.h>

// FORWARD DECLARATIONS
class MPbk2ExitCallback;
class MPbk2SelectFieldProperty;
class CPbk2KeyEventDealer;

// CLASS DECLARATION

/**
 * Phonebook 2 select field dialog for assign service.
 * Responsible for driving the given field property selector.
 */
class CPbk2AssignSelectFieldDlg : public CBase,
                                  public MPbk2DialogEliminator,
                                  private MPbk2KeyEventHandler

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExitCallback     Exit callback.
         * @return  A new instance of this class.
         */
        static CPbk2AssignSelectFieldDlg* NewL(
                MPbk2ExitCallback& aExitCallback );

        /**
         * Destructor.
         */
        ~CPbk2AssignSelectFieldDlg();

   public: // Interface

        /**
         * Executes the dialog.
         *
         * @param aProperty     Field property selector.
         * @return  Error code.
         * @see MPbk2SelectFieldProperty::ExecuteL
         */
        TInt ExecuteLD(
                MPbk2SelectFieldProperty& aProperty );

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    public: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2AssignSelectFieldDlg(
                MPbk2ExitCallback& aExitCallback );
        void ConstructL();
        void ExitApplication(
                TInt aCommandId );

    private: // Data types
        /**
         * Exit parameter flags
         */
       enum TExitState
            {
            EAbortOrdered,
            EAbortApproved
            };

    private: // Data
        /// Own: For checking is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Exit handling
        TBitFlags8 iExitRecord;
        /// Ref: Exit callback
        MPbk2ExitCallback& iExitCallback;
        /// Ref: Set to NULL when this dialog is destroyed
        MPbk2DialogEliminator** iSelfPtr;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Ref: Field property selector
        MPbk2SelectFieldProperty* iProperty;
    };

#endif // CPBK2ASSIGNSELECTFIELDDLG_H

// End of File
