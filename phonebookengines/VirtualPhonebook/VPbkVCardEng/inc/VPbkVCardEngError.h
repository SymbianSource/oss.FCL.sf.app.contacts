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
*
*/


#ifndef VPBKVCARDENGERROR_H
#define VPBKVCARDENGERROR_H

#ifdef _DEBUG

//  INCLUDES
#include <e32std.h>

/**
* VPbkVCardEngError internal error codes and functions for
* debug code
*/
namespace VPbkVCardEngError
    {
    /// Panic category for VCard engine
    _LIT( KPanicCategory, "VPbkVCardEng" );

    /**
    * Panic codes for the VPbkVCardEng
    */
    enum TPanicCode
        {
        ENullParserProperty = 1,
        EInvalidLengthOfPropertyArray = 2,
        EInvalidStateOfExporter = 3,
        EStrayOperation = 4,
        ENullFieldType = 5,
        EInvalidDescriptor = 6,
        EAddToFieldOperationAlreadyExists = 7,
        EPostCond_CVPbkVCardCompactBCardImporter_ConstructL = 8,
        EPreCond_CVPbkImportToStoreOperation_StartL = 9,
        EPreCond_CommitNextContactL = 10,
        EPreCond_MergeContactsL = 11
        };

    /**
     * Called if an unrecoverable error is detected.
     * Stops the thread with User::Panic.
     */
    inline void Panic( TPanicCode aPanicCode );
    }

inline void VPbkVCardEngError::Panic( TPanicCode aPanicCode )
    {
    User::Panic( KPanicCategory, aPanicCode );
    }

#endif // _DEBUG

#endif      // VPBKVCARDENGERROR_H
            
// End of File
