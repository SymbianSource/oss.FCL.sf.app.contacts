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
*		Namespace structure for Phonebook AIW Providers helper functions
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <AiwGenericParam.h>
#include "PbkProviderUtils.h"
#include "PbkAiwProviders.hrh"

/// Unnamed namespace for local definitions
namespace __RVCT_UNS_PbkProviderUtils {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_CopyAndReplaceChars = 1,
    };

_LIT(KMimeTypeImage, "IMAGE");
_LIT(KMimeTypeAudio, "AUDIO");

_LIT(KMimeTypeRingingTone, "APPLICATION/VND.NOKIA.RINGING-TONE");

#ifdef RD_VIDEO_AS_RINGING_TONE
_LIT(KMimeTypeVideo, "VIDEO");
_LIT(KMimeTypeRealMedia, "APPLICATION/VND.RN-REALMEDIA");
_LIT(KMimeTypeSDP, "APPLICATION/SDP");
_LIT(KMimeTypeFlash, "APPLICATION/X-SHOCKWAVE-FLASH");
#endif  // RD_VIDEO_AS_RINGING_TONE



// ================= LOCAL FUNCTIONS =======================
#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "PbkProviderUtils");
    User::Panic(KPanicText, aReason);
    }
#endif
}

using namespace __RVCT_UNS_PbkProviderUtils;


// ================= MEMBER FUNCTIONS =======================

TInt PbkProviderUtils::MapMimeTypeL
        (const TDesC& aMimeTypeString)
    {
    _LIT(KSeparator, "/");
    
    // Check for a type separator in the string
    TInt pos = aMimeTypeString.Find(KSeparator);
    // Leave if no separator was found.. the MIME
    // standard requires it
    User::LeaveIfError(pos);
    
    // Copy the full Mime type string (needed for uppercase)
    HBufC* fullBuf = aMimeTypeString.AllocLC();
    TPtr fullString = fullBuf->Des();
    fullString.UpperCase();

	// Construct the compare string    
    TPtrC extractString(aMimeTypeString.Left(pos));
    HBufC* compareBuf = extractString.AllocLC();
    TPtr compareString = compareBuf->Des();
    compareString.UpperCase();

	// Perform the comparison
    TInt ret = EPbkMimeTypeNotSupported;
    // Mime type case:  IMAGE/*
    if (compareString.CompareF(KMimeTypeImage) == 0)
    	{
    	ret = EPbkMimeTypeImage;
    	}
    // Mime type case:  AUDIO/*
	else if (compareString.CompareF(KMimeTypeAudio) == 0)
		{
    	ret = EPbkMimeTypeAudio;
    	}
    //	Mime type case: APPLICATION/VND.NOKIA.RINGING-TONE
    else if (fullString.CompareF(KMimeTypeRingingTone) == 0)    
        {
        ret = EPbkMimeTypeRingingTone;
        }    	
    	
#ifdef RD_VIDEO_AS_RINGING_TONE

    // Mime type case:  VIDEO/*
	else if (compareString.CompareF(KMimeTypeVideo) == 0)
		{
    	ret = EPbkMimeTypeVideo;
    	}    
    //	Mime type case: Special video formats
    else if ( (fullString.CompareF(KMimeTypeRealMedia) == 0) ||
              (fullString.CompareF(KMimeTypeSDP) == 0) ||
              (fullString.CompareF(KMimeTypeFlash) == 0) )
        {
        ret = EPbkMimeTypeVideo;
        }
        
#endif      // RD_VIDEO_AS_RINGING_TONE

	CleanupStack::PopAndDestroy(compareBuf);	    	
	CleanupStack::PopAndDestroy(fullBuf);
    
	return ret;    
    }
    
TPtrC PbkProviderUtils::GetAiwParamAsDescriptor(
		const CAiwGenericParamList& aParamList,
		TGenericParamId aParamType)
	{
	TInt index = 0;
	const TAiwGenericParam* genericParam = NULL;
    genericParam = aParamList.FindFirst( 
        index,
        aParamType,
        EVariantTypeDesC);
        
    if (index >= 0 && genericParam)
        {
        // Get the data
        return genericParam->Value().AsDes();
        }
	else
    	{
		return KNullDesC();
    	}
	}

// End of File
