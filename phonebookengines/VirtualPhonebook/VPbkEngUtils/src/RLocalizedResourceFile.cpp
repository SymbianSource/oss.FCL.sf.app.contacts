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
* Description:  Fast digit string handler class
*
*/


#include "RLocalizedResourceFile.h"
#include <bautils.h>

#include <VPbkDebug.h>

namespace VPbkEngUtils {

_LIT(KBackslash, "\\");

void EnsureBackslashInMiddle(const TDesC& aLhs, const TDesC& aRhs, TDes& aResult)
    {
    if (aLhs[aLhs.Length() - 1] != KBackslash()[0] &&
        (aRhs.Length() == 0 || aRhs[0] != KBackslash()[0]))
        {
        aResult.Append(KBackslash);
        }
    }


EXPORT_C void RLocalizedResourceFile::OpenLC
        (RFs& aFs, const TDesC& aDrive, const TDesC& aPath, const TDesC& aFileName)
    {
    TFileName fileName(aDrive);
    EnsureBackslashInMiddle(fileName, aPath, fileName);
    fileName.Append(aPath);
    EnsureBackslashInMiddle(fileName, aFileName, fileName);
    fileName.Append(aFileName);
    BaflUtils::NearestLanguageFile(aFs, fileName);
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("RLocalizedResourceFile::OpenLC %S"), &fileName);

    RResourceFile::OpenL(aFs, fileName);

    CleanupClosePushL(*this);
    ConfirmSignatureL();
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("RLocalizedResourceFile::OpenLC end"));
    }

EXPORT_C void RLocalizedResourceFile::OpenL
        (RFs& aFs, const TDesC& aDrive, const TDesC& aPath, const TDesC& aFileName)
    {
    OpenLC(aFs, aDrive, aPath, aFileName);
    CleanupStack::Pop();
    }

} // namespace VPbkEngUtils
//End of file

