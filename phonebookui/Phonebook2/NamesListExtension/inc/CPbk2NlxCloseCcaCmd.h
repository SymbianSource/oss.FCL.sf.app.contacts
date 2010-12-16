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
* Description:  Phonebook 2 CCA close command
*
*/


#ifndef CPBK2NLXCLOSECCACMD_H
#define CPBK2NLXCLOSECCACMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;

//Cca
class MCCAConnection;

// CLASS DECLARATION

/**
 * Phonebook 2 close cca command object for invoking CCA application.
 */
class CPbk2NlxCloseCcaCmd : public CBase,
                            public MPbk2Command
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param Pbk2 Contact Ui control
         * @param a reference to a pointer to CCA client connection object
         * @return  A new instance of this class.
         */
        static CPbk2NlxCloseCcaCmd* NewL(
            MPbk2ContactUiControl& aUiControl, 
            MCCAConnection*& aCCAConnection );

        /**
         * Destructor.
         */
        ~CPbk2NlxCloseCcaCmd();

    public: //From MPbk2Command
        void ExecuteLD();
        void ResetUiControl( MPbk2ContactUiControl& aUiControl );
        void AddObserver( MPbk2CommandObserver& aObserver );

    private: //Construction
        CPbk2NlxCloseCcaCmd( 
            MPbk2ContactUiControl& aUiControl, 
            MCCAConnection*& aCCAConnection );
        void ConstructL();

    private: // Data
        
        // Ref, not owned
        MPbk2CommandObserver* iCommandObserver;
        // Ref, not owned
        MPbk2ContactUiControl *iUiControl;
        // Ref to ptr: for CCA, not owned       
        MCCAConnection*& iConnectionRef;
    };

#endif // CPBK2NLXCLOSECCACMD_H

// End of File
