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
* Description: 
*     Set image command event handling class.
*
*/


#ifndef CPBK2VIEWIMAGECMD_H
#define CPBK2VIEWIMAGECMD_H

//  INCLUDES
#include "CPbk2ImageCmdBase.h"
#include <MPbk2ImageOperationObservers.h>   // MPbk2ImageSetObserver
#include <AknServerApp.h>                   // MAknServerAppExitObserver

// FORWARD DECLARATIONS
class CDocumentHandler;

// CLASS DECLARATION

/**
 * View image command event handling class.
 */
NONSHARABLE_CLASS(CPbk2ViewImageCmd) : 
        public CPbk2ImageCmdBase,
        private MAknServerAppExitObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl 	A related ui control
         * @return a new instance of this class.
         */
        static CPbk2ViewImageCmd* NewL(
                MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        ~CPbk2ViewImageCmd();
        
    private: // from CPbk2ImageCmdBase
        TBool ExecuteCommandL();

    private: // from MAknServerAppExitObserver
        void HandleServerAppExit(
                TInt aReason );
        
    private:  // Implementation
        CPbk2ViewImageCmd(
                MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void ShowImageNotOpenedNoteL();
        
    private:    // Data
        /// Own: Document handler for image opening
        CDocumentHandler* iDocHandler;
        /// Own: Image file name buffer
        TFileName iFileName;
    };

#endif // CPBK2VIEWIMAGECMD_H
            
// End of File
