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
*       Provides methods for Helper class for loading PbkView resources..
*
*/


// INCLUDE FILES
#include "RPbkViewResourceFile.h"
#include "PbkDataCaging.hrh"

/// Unnamed namespace for local definitons
namespace 
{

// LOCAL CONSTANTS AND MACROS
_LIT(KDllResFileName, "PbkView.rsc");

}

// ================= MEMBER FUNCTIONS =======================

EXPORT_C RPbkViewResourceFile::RPbkViewResourceFile(CCoeEnv& aEnv)
    : iLoader(aEnv), iOpen(EFalse)
    {
    }

EXPORT_C void RPbkViewResourceFile::OpenL()
    {
    _LIT(KSeparator, "\\");
    TFileName fileName;
    fileName.Copy(KPbkRomFileDrive);
    fileName.Append(KDC_RESOURCE_FILES_DIR);
    fileName.Append(KSeparator);
    fileName.Append(KDllResFileName);
    iLoader.OpenL(fileName);
    iOpen = ETrue;
    }

EXPORT_C TBool RPbkViewResourceFile::IsOpen() const
    {
    return iOpen;
    }

EXPORT_C void RPbkViewResourceFile::Close()
    {
    iLoader.Close();
    iOpen = EFalse;
    }


//  End of File  
