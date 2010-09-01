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
* Description:  Phonebook 2 change to contact info view command.
*
*/


#ifndef CPBK2ACTIVATECNTINFOVIEWCMD_H
#define CPBK2ACTIVATECNTINFOVIEWCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATION
class MPbk2ViewExplorer;
class MPbk2ContactUiControl;

// CLASS DECLARATION

/**
 * Phonebook 2 change to contact info view command.
 */
NONSHARABLE_CLASS(CPbk2ActivateCntInfoViewCmd) :
        public CBase,
        public MPbk2Command
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aViewExplorer Pbk2 View explorer
         * @param aUiControl    UI control.
         * @param aAppView      Pbk2 application view
         * @param aViewId       Pbk2 view id
         * @return  A new instance of this class.
         */
        static CPbk2ActivateCntInfoViewCmd* NewL(
                            MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2ActivateCntInfoViewCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // Implementation
        CPbk2ActivateCntInfoViewCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
                
    private: // Data
        /// Ref: Phonebook 2 view explorer for view switching
        MPbk2ViewExplorer* iViewExplorer;        
        /// Ref: UI control
        MPbk2ContactUiControl* iControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
    };

#endif // CPBK2ACTIVATECNTINFOVIEWCMD_H

// End of File
