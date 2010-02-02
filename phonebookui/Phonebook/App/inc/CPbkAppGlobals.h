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
*       Phonebook application level globals.
*
*/


#ifndef __CPbkAppGlobals_H__
#define __CPbkAppGlobals_H__


// INCLUDES
#include <CPbkAppGlobalsBase.h>
#include <cntdef.h>


// FORWARD DECLARATIONS
class CPbkCommandFactory;
class CPbkAppUi;
class CContactIdArray;
class CBCardEngine;
class CPbkContactEngine;
class CPbkAiwInterestItemFactory;
class CPbkAiwInterestArray;
class CSendUi;

// CLASS DECLARATION

/**
 * Phonebook application level globals. This is a singleton class.
 */
class CPbkAppGlobals : public CPbkAppGlobalsBase
    {
    public: // Constructor and destructor
        /**
         * Returns the singleton instance of this class. This function's
		 * implementation is expensive. Cache the returned reference if it is
		 * used frequently.
		 *
		 * @return	a reference to the singleton globals object.
         */
        static CPbkAppGlobals* InstanceL();

        /**
         * Creates a new CPbkAppGlobals.
         */
        static CPbkAppGlobals* NewL();

        /**
         * Destructor.
         */
        ~CPbkAppGlobals();

    public: // From CPbkAppGlobalsBase
        MPbkCommandFactory& CommandFactory();
        MPbkAiwInterestItemFactory& AiwInterestItemFactory();
        CSendUi* SendUiL();
        CBCardEngine& BCardEngL(CPbkContactEngine& aContactEngine);
        CPbkAiwInterestArray* AiwInterestArrayL
            (CPbkContactEngine& aContactEngine);

    private: // From MPbkUiReleasable
        void DoRelease();

    private: // Implementation
        CPbkAppGlobals();
        void ConstructL();        

    private: // Data
        /// Own: phonebook command factory
        CPbkCommandFactory* iCommandFactory;
        /// Own: SendUi
		CSendUi* iSendUi;
        /// Own: vCard import/export engine
        CBCardEngine* iBCardEngine;
        /// Own: phonebook aiw interest item factory
        CPbkAiwInterestItemFactory* iAiwInterestFactory;
        /// Own: phonebook aiw interest item array
        CPbkAiwInterestArray* iAiwInterestArray;
    };

#endif // __CPbkAppGlobals_H__

// End of File
