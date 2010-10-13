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
*           Flash File System (FFS) check helper class methods.
*
*/


// INCLUDE FILES
#include <CPbkFFSCheck.h>

#include <sysutil.h>
#include <coemain.h>
#include <PbkCommandHandler.h>  // MPbkCommandHandler


// ================= MEMBER FUNCTIONS =======================

inline CPbkFFSCheck::CPbkFFSCheck(CCoeEnv* aCoeEnv) :
    iConeEnv(aCoeEnv)
    {
    if (!iConeEnv)
        {
        iConeEnv = CCoeEnv::Static();
        }
    }

inline TBool CPbkFFSCheck::DebugFailNext()
    {
#ifdef _DEBUG
    TBool result = iDebugFailNext;
    iDebugFailNext = EFalse;
    return result;
#else
    return EFalse;
#endif
    }

EXPORT_C CPbkFFSCheck* CPbkFFSCheck::NewL(CCoeEnv* aCoeEnv/*=NULL*/)
    {
    CPbkFFSCheck* self = new (ELeave) CPbkFFSCheck(aCoeEnv);
    return self;
    }
    
CPbkFFSCheck::~CPbkFFSCheck()
    {
    }

EXPORT_C TBool CPbkFFSCheck::FFSClCheckL(TInt aBytesToWrite/*=0*/)
    {
    if (SysUtil::FFSSpaceBelowCriticalLevelL(&iConeEnv->FsSession(),aBytesToWrite) ||
        DebugFailNext())
        {
        // Show not enough memory note
        User::Leave(KErrDiskFull);
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

EXPORT_C void CPbkFFSCheck::FFSClCheckL
        (const MPbkCommandHandler& aCommandHandler, TInt aBytesToWrite/*=0*/)
    {
    if (FFSClCheckL(aBytesToWrite))
        {
        aCommandHandler.ExecuteL();
        }
    }

EXPORT_C TBool CPbkFFSCheck::MMCClCheckL(TInt aBytesToWrite/*=0*/)
    {
    if (SysUtil::MMCSpaceBelowCriticalLevelL(&iConeEnv->FsSession(),aBytesToWrite) ||
        DebugFailNext())
        {
        // Show not enough memory note
        User::Leave(KErrDiskFull);
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

EXPORT_C void CPbkFFSCheck::MMCClCheckL
        (const MPbkCommandHandler& aCommandHandler, TInt aBytesToWrite/*=0*/)
    {
    if (MMCClCheckL(aBytesToWrite))
        {
        aCommandHandler.ExecuteL();
        }
    }


//  End of File  
