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
* Description: 
*     Localized resource file for Phonebook 2.
*
*/


// INCLUDE FILES
#include <RPbk2LocalizedResourceFile.h>

// From System
#include <coemain.h>
#include <bautils.h>

// Debugging headers
#include <Pbk2Debug.h>

// Unnamed namespace for local definitions
namespace {
// Definition for backslash
_LIT(KBackslash, "\\");

/**
 * Ensures that path includes backslashs in correct places. Either left hand
 * side descriptor's or right hand side's last descriptor needs to be 
 * backslash. If backslash is not found, it is added to result descriptor
 *
 * @param aLhs      Left hand side operator.
 * @param aRhs      Right hand side operator.
 * @param aResult   On return contains correctly modified descriptor.
 */
void EnsureBackslashInMiddle(
        const TDesC& aLhs, 
        const TDesC& aRhs, 
        TDes& aResult)
    {
    if (aLhs[aLhs.Length() - 1] != KBackslash()[0] &&
        (aRhs.Length() == 0 || aRhs[0] != KBackslash()[0]))
        {
        aResult.Append(KBackslash);
        }
    }
} // namespace

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::RPbk2LocalizedResourceFile
// --------------------------------------------------------------------------
//
EXPORT_C RPbk2LocalizedResourceFile::RPbk2LocalizedResourceFile(
        CCoeEnv& aCoeEnv) :
    iFs( &aCoeEnv.FsSession() ), iCoeEnv(&aCoeEnv), iOffset(0)
    {
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::RPbk2LocalizedResourceFile
// --------------------------------------------------------------------------
//
EXPORT_C RPbk2LocalizedResourceFile::RPbk2LocalizedResourceFile( RFs* aFs ) :
        iFs( aFs ), iCoeEnv( NULL ), iOffset( 0 )
    {
    }
    
// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::OpenLC
// --------------------------------------------------------------------------
//
EXPORT_C void RPbk2LocalizedResourceFile::OpenLC
        (const TDesC& aDrive, const TDesC& aPath, const TDesC& aFileName)
    {
    EnsureOpenFileSystemSessionL();
    
    TFileName fileName(aDrive);
    EnsureBackslashInMiddle(fileName, aPath, fileName);
    fileName.Append(aPath);
    EnsureBackslashInMiddle(fileName, aFileName, fileName);
    fileName.Append(aFileName);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("RPbk2LocalizedResourceFile::OpenLC() resource filename: %S"),
        &fileName);

    BaflUtils::NearestLanguageFile(*iFs, fileName);
    
    if ( iCoeEnv )
        {
        iOffset = iCoeEnv->AddResourceFileL(fileName);
        CleanupClosePushL(*this);
        }
    else
        {
        RResourceFile::OpenL(*iFs, fileName);
        CleanupClosePushL(*this);
        ConfirmSignatureL();
        }
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::OpenL
// --------------------------------------------------------------------------
//
EXPORT_C void RPbk2LocalizedResourceFile::OpenL
        (const TDesC& aDrive, const TDesC& aPath, const TDesC& aFileName)
    {
    OpenLC(aDrive, aPath, aFileName);
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::OpenL
// --------------------------------------------------------------------------
//
EXPORT_C void RPbk2LocalizedResourceFile::OpenL(const TDesC& aFullPath)
    {
    EnsureOpenFileSystemSessionL();
    
    TFileName fileName(aFullPath);

    BaflUtils::NearestLanguageFile(*iFs, fileName);
    if ( iCoeEnv )
        {
        iOffset = iCoeEnv->AddResourceFileL(fileName);
        }
    else
        {
        RResourceFile::OpenL(*iFs, fileName);
        ConfirmSignatureL();
        }
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::Close
// --------------------------------------------------------------------------
//
EXPORT_C void RPbk2LocalizedResourceFile::Close()
    {
    if (iOffset && iCoeEnv)
        {
        iCoeEnv->DeleteResourceFile(iOffset);
        iOffset = 0;
        }
    else
        {
        RResourceFile::Close();
        }
    
    if ( iOwnFs.Handle() )
        {
        iOwnFs.Close();
        }
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::Offset
// --------------------------------------------------------------------------
//
EXPORT_C TInt RPbk2LocalizedResourceFile::Offset()
    {
    return iOffset;
    }

// --------------------------------------------------------------------------
// RPbk2LocalizedResourceFile::EnsureOpenFileSystemSessionL
// --------------------------------------------------------------------------
//
void RPbk2LocalizedResourceFile::EnsureOpenFileSystemSessionL()
    {
    if ( !iFs )
        {
        User::LeaveIfError( iOwnFs.Connect() );
        iFs = &iOwnFs;
        }
    }
// End of File
