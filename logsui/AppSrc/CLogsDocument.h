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
*     Logs document class
*
*/


#ifndef     __Logs_App_CLogsDocument_H__
#define     __Logs_App_CLogsDocument_H__

//  INCLUDES
#include <AknDoc.h> 
#include <coemain.h>
#include <apgwgnam.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAknApplication;
class CEikAppUi;
class CLogsEngine;

// CLASS DECLARATION

/**
 *  Logs document class. An object of this class is created by the Symbian OS 
 *  framework by a call to CLogsApplication::CreateDocumentL(). The document 
 *  object creates and owns the application engine and provides access to it by
 *  the Engine() function. The application framework creates the Logs 
 *  application UI object (CLogsAppUi) by a call to the virtual function 
 *  CreateAppUiL().
 */
class   CLogsDocument :	public CAknDocument, MCoeForegroundObserver
    {
    private:  // construction
        /**
         *  Constructor.
         *
         *  @param aApp The application object.
         */
        CLogsDocument( CAknApplication& aApp );

        /**
         *  Constructor, second phase. Creates the engine object.
         */
        void ConstructL();

    public:  // interface
        /**
         *  Standard creation function. Creates and returns a new object of
         *  this class.
         *  @param application reference
         *  @return new object
         */
        static CLogsDocument* NewL( CAknApplication& aApp );

        /**
         *  Destructor. Deletes the engine object.
         */
        ~CLogsDocument();

        /**
         *  Returns the pointer of the engine.
         *  @return iEngine
         */
        CLogsEngine* Engine();


    private: // from CEikDocument
        /**
         *  Creates and returns the Logs application UI object (CLogsAppUi).
         *  Called by the application framework.
         *
         *  @return new Logs application UI object
         *
         *  @exception KErrNoMemory if out of memory.
         *  @exception anything thrown by CLogsAppUi construction.
         */
        CEikAppUi* CreateAppUiL();  // Create the Logs Application UI object
        
        
	public:	// from MCoeForegroundObserver

		/**
		 * Handles the application coming to the foreground.
		 */
		void HandleGainingForeground();

		/**
		 * Leaving version of the HandleGainingForeground.
		 * Handles the application coming to the foreground.
		 */
		void HandleGainingForegroundL();

		/**
		 * Handles the application going into the background.
		 */
		void HandleLosingForeground();

		/**
		 * Leaving version of the HandleLosingForeground
		 * Handles the application going into the background.
		 */
		void HandleLosingForegroundL();

	public: // new methods

		/**
		 * Updates the task in fast swap windows
		 *
		 * @param aWgName the window group name
		 */
		void UpdateTaskNameL( CApaWindowGroupName* aWgName );

    private:  // data
        /// Own: Logs engine.
        CLogsEngine*    iEngine;

    };


#endif  // __Logs_App_CLogsDocument_H__


// End of File

