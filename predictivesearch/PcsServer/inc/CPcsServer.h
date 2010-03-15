/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PCS Server main class. Co-ordinates server startup 
*                and shutdown and receives client requests.
*
*/

#ifndef PCS_SERVER_H
#define PCS_SERVER_H

// INCLUDE FILES
#include <e32base.h>
#include <CPcsDefs.h>

// FORWARD DECLARATIONS
class CPcsSession;
class CPcsHandler;
class CPcsPluginInterface;

/** 
 * PCS Server main class. Co-ordinates server startup and shutdown 
 * and receives client requests.
 */
class CPcsServer : public CServer2
{
    public:
    
        /**
         * Two phase construction
         */
		static CPcsServer* NewL();
		
		/**
		 * Destructor
		 */
		~CPcsServer();
		
		/**
		 * Creates a server side session object
		 * Implements NewSessionL of CServer2
		 */
		CSession2* NewSessionL(const TVersion& aVersion,
			                   const RMessage2& aMessage) const;
			                   
        /**
         * Return the plugin interface instance
         */			                  
	    CPcsPluginInterface* PluginInterface();
	    
    private:
    
        /**
         * Constructor
         */
		CPcsServer();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();
		
		/**
		 * Returns ETrue if chinese feature is initilized
		 */
		TBool IsChineseFeatureInitilizedL();
    private:
    
        /**
         * A PCS algorithm instance (owned)
         */
    	CPcsPluginInterface* iPcs;
};

#endif // End of file
