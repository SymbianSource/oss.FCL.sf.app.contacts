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
*       Resource file loader for loading localized versions of resource files.
*
*/


// INCLUDE FILES
// From Phonebook
#include <PbkDebug.h>

#include <RPbkResourceFile.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

EXPORT_C RPbkResourceFile::RPbkResourceFile
		(CCoeEnv& aEnv) : 
	iLoader(aEnv), 
	iOpen(EFalse)
    {
    }

EXPORT_C RPbkResourceFile::~RPbkResourceFile()
    {
    Close();
    }

EXPORT_C void RPbkResourceFile::OpenL(const TDesC& aFileName)
    {
    if (!iOpen)
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Opening resource file"));
        TFileName fileName;
        fileName.Copy(aFileName);
        iLoader.OpenL(fileName);
        iOpen = ETrue;
        PBK_DEBUG_PRINT
            (PBK_DEBUG_STRING("Resource file opened succesfully"));
        }
    }

EXPORT_C void RPbkResourceFile::Close()
    {
    if (iOpen)
        {
        iLoader.Close();
        iOpen = EFalse;
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("Resource file closed"));
        }
    }

//  End of File  
