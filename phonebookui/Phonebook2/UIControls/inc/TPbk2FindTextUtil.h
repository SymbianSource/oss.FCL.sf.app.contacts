/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Find text util class
*
*/



#ifndef TPBK2FINDTEXTUTIL_H
#define TPBK2FINDTEXTUTIL_H

//  INCLUDES
#include <e32cmn.h>
#include <e32def.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CAknSearchField;

/**
*  Find text util
*
*/
class TPbk2FindTextUtil
    {
    public:  // Constructors and destructor
        
        /**
         * Esnures that given buffer is enough for find box text
         *
         * @param aFindBox      Findbox which content needs to be buffered
         * @param aFindTextBuf  Buffer which size is to be checked
         *                      
        */
        static void EnsureFindTextBufSizeL( 
            const CAknSearchField& aFindBox,
            HBufC** aFindTextBuf );

    };

#endif // TPBK2FINDTEXTUTIL_H
            
// End of File
