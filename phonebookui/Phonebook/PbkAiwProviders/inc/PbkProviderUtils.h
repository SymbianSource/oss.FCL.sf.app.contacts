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
*    Namespace structure for Phonebook AIW Providers helper functions
*
*/


#ifndef __PbkProviderUtils_H__
#define __PbkProviderUtils_H__

//  INCLUDES
#include <e32std.h>
#include <AiwGenericParam.hrh>

// FORWARD DECLARATIONS
class CAiwGenericParamList;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Namespace structure for Phonebook AIW Providers helper functions.
 */
struct PbkProviderUtils
    {
    /**
     * Maps the given MIME type string to the supported types enumeration.
     *
     * @param aMimeTypeString the MIME type to map, for example "image/jpeg"
     * @ret MIME type mapped to TPbkSupportedMIMETypes structure
     * @see TPbkSupportedMIMETypes
     */
    static TInt MapMimeTypeL
		(const TDesC& aMimeTypeString);
		
    /**
     * Fetches a parameter of type aParamType from AIW parameter list and
     * returns it as a descriptor (uses TAiwVariant::AsDes, @see TAiwVariant)
     *
     * @param aParamList AIW parameter list
     * @param aParamType type
     * @ret parameter value as a descriptor
     */
	static TPtrC GetAiwParamAsDescriptor(
		const CAiwGenericParamList& aParamList,
		TGenericParamId aParamType);
    };

#endif // __PbkProviderUtils_H__

// End of File
