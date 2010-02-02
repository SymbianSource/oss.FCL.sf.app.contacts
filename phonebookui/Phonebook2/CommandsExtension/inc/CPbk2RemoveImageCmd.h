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
*     Remove image command event handling class.
*
*/


#ifndef CPBK2REMOVEIMAGECMD_H
#define CPBK2REMOVEIMAGECMD_H

//  INCLUDES
#include "CPbk2ImageCmdBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Image remove command event handling class.
 */
NONSHARABLE_CLASS(CPbk2RemoveImageCmd) : 
        public CPbk2ImageCmdBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aUiControl 	A related ui control
         * @return a new instance of this class.
         */
        static CPbk2RemoveImageCmd* NewL(
                MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        ~CPbk2RemoveImageCmd();
        
    private:  // from CPbk2ImageCmdBase
        TBool ExecuteCommandL();
       
    private:  // Implementation
        CPbk2RemoveImageCmd(
                MPbk2ContactUiControl& aUiControl);
        void ConstructL();
    };

#endif // CPBK2REMOVEIMAGECMD_H
            
// End of File
