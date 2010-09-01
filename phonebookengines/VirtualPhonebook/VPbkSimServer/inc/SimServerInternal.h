/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal defintions and utilites
*
*/



#ifndef VPBKSIMSERVER_SIMSERVERINTERNAL_H
#define VPBKSIMSERVER_SIMSERVERINTERNAL_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class RVPbkSimFieldTypeArray;

// CLASS DECLARATION

/**
 * static utility functions
 *
 */
NONSHARABLE_CLASS( VPbkSimSrvUtility )
    {
    public: // New functions
        
        /**
        * Completes the message if not completed.
        * @param aMessage the message to complete
        * @param aResult the result to complete with
        */
        static void CompleteRequest( const RMessage2& aMessage, TInt aResult );
        
        /**
        * Reads field types from the client buffer to the field type array
        * @param aTypeArray the array into which types are appended
        * @param aMessage the client message to be used to access client buffer
        * @param aSlot the slot number 0-3 in which the buffer is.
        */
        static void ReadFieldTypesL( RVPbkSimFieldTypeArray& aTypeArray,
            const RMessage2& aMessage, TInt aSlot );
    };

#endif      // VPBKSIMSERVER_SIMSERVERINTERNAL_H
            
// End of File
