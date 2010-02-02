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
*     Remove thumbnail command event handling class.
*
*/


#ifndef CPBK2REMOVETHUMBNAILCMD_H
#define CPBK2REMOVETHUMBNAILCMD_H

//  INCLUDES
#include "CPbk2ThumbnailCmdBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Thumbnail command event handling class.
 */
NONSHARABLE_CLASS(CPbk2RemoveThumbnailCmd) : 
        public CPbk2ThumbnailCmdBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl 	A related ui control
         * @return a new instance of this class.
         */
        static CPbk2RemoveThumbnailCmd* NewL
            (MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        ~CPbk2RemoveThumbnailCmd();
        
    private: 
        TBool ExecuteCommandL();
       
    private:  // Implementation
        CPbk2RemoveThumbnailCmd
            (MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        
    private:    // Data
    };

#endif // CPBK2REMOVETHUMBNAILCMD_H
            
// End of File
