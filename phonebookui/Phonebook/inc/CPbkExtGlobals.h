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
*       PbkExt.dll global variables. A singleton class.
*
*/


#ifndef __CPbkExtGlobals_H__
#define __CPbkExtGlobals_H__

// INCLUDES
#include <e32base.h>
#include "CPbkExtensionScanner.h" // CPbkExtensionArray typedef

// FORWARD DECLARATIONS
class MPbkExtensionFactory;
class CPbkMultiExtensionFactory;
class MPbkUiExtensionBase;

/**
 * Singleton class.
 */
class CPbkExtGlobals : public CBase
    {
    public:
        /**
         * Returns the instance of this class.
         */
        IMPORT_C static CPbkExtGlobals* InstanceL();

        /**
         * Returns the extensionfactory. If no instance exists yet it is 
		 * created.
         */
        IMPORT_C MPbkExtensionFactory& FactoryL();

        /**
         * Pushes this object to cleanup stack for releasing.
         */
        void PushL();

    private: // Interface for Release(CPbkExtGlobals*)
		friend void Release(CPbkExtGlobals*);
        virtual void DoRelease();
		static void CleanupRelease(TAny* aObj);

    private: // Implementation
        CPbkExtGlobals();
        ~CPbkExtGlobals();
        static CPbkExtGlobals* NewLC();
        void IncRef();
        TInt DecRef();
        void ScanExtensionsL();
    private: // Data
        /// Reference count
        TInt iRefCount;
        /// Array of extensions
        CPbkExtensionArray iExtensions;
        /// Own: Extension factory
		CPbkMultiExtensionFactory* iMultiFactory;
    };

// INLINE FUNCTIONS

inline void CPbkExtGlobals::PushL()
	{
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this)); 
	}

inline void Release(CPbkExtGlobals* aObj)
	{
	if (aObj)
		{
		aObj->DoRelease();
		}
	}

inline void CPbkExtGlobals::CleanupRelease(TAny* aObj)
    { 
    Release(static_cast<CPbkExtGlobals*>(aObj)); 
    }

#endif // __CPbkExtGlobals_H__

// End of File
