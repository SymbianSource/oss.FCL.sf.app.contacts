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
*     Phonebook document class definition
*
*/


#ifndef __CPbkDocument_H__
#define __CPbkDocument_H__

//  INCLUDES
#include <CPbkDocumentBase.h>

// FORWARD DECLARATIONS
class CPbkApplication;
class CPbkContactEngine;
class CPbkExtGlobals;


// CLASS DECLARATION

/**
 * Phonebook application document class. 
 * An object of this class is created by the Symbian OS framework by a call to 
 * CPbkApplication::CreateDocumentL(). The document object creates and owns 
 * the application engine and provides access to it by the Engine() function. 
 * The application framework creates the Phonebook application UI object 
 * (CPbkAppUi) by a call to the virtual function CreateAppUiL().
 */
class CPbkDocument :
        public CPbkDocumentBase
    {
    public:  // Interface
        /**
         * Creates and returns a new object of this class.
		 * @param aApp reference to eik application object
         */
        static CPbkDocument* NewL(CEikApplication& aApp);

        /**
         * Returns the Phonebook application object.
         */
        CPbkApplication* PbkApplication() const;

        /**
         * Creates the Phonebook engine owned by this object.
         *
         * @param aReplace  if true, existing DB will be replaced.
		 *		  All data in the replaced DB is lost
         */
        void CreateEngineL(TBool aReplace=EFalse);

        /**
         * Creates view globals. Needed to assure that view globals
         * lives longer than AppUi.
         */
        void CreateGlobalsL();

    public:  // from CPbkDocumentBase
        CPbkContactEngine* Engine() const;

    private: // from CAknDocument
	    ~CPbkDocument();
        CEikAppUi* CreateAppUiL();

    private:  // Implementation
	    CPbkDocument(CEikApplication& aApp);  
        void ConstructL();  

    private:  // Data
        /// Own: the Phonebook engine.
        CPbkContactEngine* iEngine;
        /// Own: need to be existing whole life time of an application
        CPbkExtGlobals* iExtGlobal;
    };

#endif // __CPbkDocument_H__
            
// End of File
