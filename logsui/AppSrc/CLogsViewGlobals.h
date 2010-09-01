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
*       Logs.app global variables. A singleton class.
*
*/


#ifndef CLogsViewGlobals_H
#define CLogsViewGlobals_H


//  INCLUDES
#include <e32base.h>

#include "MLogsExtensionFactory.h"

// FORWARD DECLARATIONS
class CLogsExtensionLoader;

// CLASS DECLARATION

/**
 * Logs.app global variables. A singleton class.
 */
class CLogsViewGlobals : private CBase
    {
    public: // Constructors and destructor
        /**
         * Returns the singleton CLogsViewGlobals instance. If no instance
		 * exists yet it is created.
         */
        static CLogsViewGlobals* InstanceL();

		/**
		 * Pushes this object on to the cleanup stack.
		 */
		void PushL();

    public: // New functions
		/**
		 * Returns view extension factory. If no instance exists yet it is 
		 * created.
		 */
		MLogsExtensionFactory& ExtensionFactoryL();

    private: // Interface for Release(CLogsViewGlobals*)
		friend void Release(CLogsViewGlobals*);
        virtual void DoRelease();
		static void CleanupRelease(TAny* aObj);

    private:  // Implementation
        CLogsViewGlobals();
        void ConstructL();
        static CLogsViewGlobals* NewLC();
        ~CLogsViewGlobals();
        void IncRef();
        TInt DecRef();
        TInt RefCount();

    private:  // Data
        /// Own: reference count for InstanceL calls
        TInt iRefCount;
        /// Own: for loading dll and getting extension factory
        CLogsExtensionLoader* iLoader;

        //
        //Static pointer to singleton instance of this class
    public:        
        static CLogsViewGlobals* singleThis;
    };


// INLINE FUNCTIONS

inline void CLogsViewGlobals::PushL()
	{
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this)); 
	}

inline void Release(CLogsViewGlobals* aObj)
	{
	if (aObj)
		{
		aObj->DoRelease();
		}
	}

inline void CLogsViewGlobals::CleanupRelease(TAny* aObj)
    { 
    Release(static_cast<CLogsViewGlobals*>(aObj)); 
    }


#endif
            

// End of File
