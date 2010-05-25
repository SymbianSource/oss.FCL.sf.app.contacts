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
* Description:  This is the client side internal file to handle
*                property used in Publish and Subscribe framework.
*
*/


#ifndef __CPSPROPERTYHANDLER_H__
#define __CPSPROPERTYHANDLER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>
#include <e32property.h>
#include <CPcsDefs.h>

// FORWARD declaration
class CPSRequestHandler;

// CLASS DECLARATION
/**
* CPsPropertyHandler
* An instance of the property handler object for the Predicitve Search application
*
* @lib PsServerClientAPI.lib
* @since S60 v3.2
*/

class CPsPropertyHandler : public CActive
{

	public:	// Constructors and destructors
	
		/**
        * NewL.
        * Two-phased constructor.
        * Creates a CPsPropertyHandler object using two phase construction,
        * and return a pointer to the created object.
        *
        * @param aRequestHandler The object to RequestHandler through
        *                  		 which observers will be called
        * @return A pointer to the created instance of CPsPropertyHandler.
        */	
		static CPsPropertyHandler* NewL( CPSRequestHandler* aRequestHandler );
		
		/**
        * ~CPsPropertyHandler.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
		~CPsPropertyHandler();
		
		/**
		* GetCachingStatusL
		* Returns the cache status for synchronous requests
		*/
		TInt GetCachingStatusL(TCachingStatus& aStatus);
			
	protected: // Functions from base classes
		
		/**
        * From CActive, RunL.
        * Callback function.
        * Invoked to handle responses from the Algorithm.
        */
		void RunL();
		
		/**
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
		void DoCancel();
				
	private: // Constructors and destructors
	
        /**
        * CPsPropertyHandler.
        * Performs the first phase of two phase construction.
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        */
        CPsPropertyHandler( CPSRequestHandler* aRequestHandler );

        /**
        * ConstructL.
        * Performs the second phase construction of a
        * CPsPropertyHandler object.
        */
        void ConstructL();
        
    private: // Data
    	
    	/**
    	* iRequestHandler, object to RequestHandler through which 
    	* observers will be accessed
    	*/
		CPSRequestHandler* iRequestHandler;
		
		/**
		* iProperty, property handle used to attach to the property defined
		*/
		RProperty iCacheStatusProperty;
		/**
		* iCacheErrorProperty, property handle used to attach to the cache error property
		*/
		RProperty iCacheErrorProperty;
};

#endif /* __CPSPROPERTYHANDLER_H__ */

// END OF FILE
