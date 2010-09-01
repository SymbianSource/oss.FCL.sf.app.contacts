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
* Description:  Phonebook 2 create new group command object.
*
*/


#ifndef CPGUCREATENEWGROUPCMD_H
#define CPGUCREATENEWGROUPCMD_H

// INCLUDES
#include <e32base.h>
#include "MPbk2Command.h"
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactGroup;
class MVPbkContactStore;
class MVPbkContactViewBase;
class CPbk2InputAbsorber;

// CLASS DECLARATION

/**
 * Phonebook 2 create new group command object.
 */
class CPguCreateNewGroupCmd :  public CBase,
                               public MPbk2Command,
                               private MVPbkContactObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPguCreateNewGroupCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguCreateNewGroupCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    public: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Implementation
        CPguCreateNewGroupCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        void AddNewGroupL();
        void CheckDefaultSavingStorageL();
        void FinishCommandL();

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Target store
        MVPbkContactStore* iTargetStore;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: The newly added contact group
        MVPbkContactGroup* iNewGroup;
        /// Ref: Groups view
        MVPbkContactViewBase* iGroupsListView;
        /// Own: Input absorber
        CPbk2InputAbsorber* iInputAbsorber;
    };

#endif // CPGUCREATENEWGROUPCMD_H

// End of File
