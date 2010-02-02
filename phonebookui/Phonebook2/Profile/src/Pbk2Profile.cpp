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
* Description:  Phonebook 2 profiling support.
*
*/


#include <pbk2profile.h>
#include <e32debug.h>
#include <flogger.h>

namespace Pbk2Profile {

// --------------------------------------------------------------------------
// TPbk2Profiling::GlobalL
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2Profiling* TPbk2Profiling::GlobalL()
    {
    if (!Dll::Tls())
        {
        TPbk2Profiling* profiling = new (ELeave) TPbk2Profiling;
        Dll::SetTls(profiling);
        }
    return reinterpret_cast<TPbk2Profiling*>(Dll::Tls());
    }

// --------------------------------------------------------------------------
// TPbk2Profiling::~TPbk2Profiling
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2Profiling::~TPbk2Profiling()
    {
    }

// --------------------------------------------------------------------------
// TPbk2Profiling::StartProfile
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2Profiling::StartProfile(TInt aBin)
    {
    TTime startTime;
    startTime.HomeTime();
    iProfileArray[aBin] = startTime;
    }

_LIT(KPath, "cnt");
_LIT(KFile, "PbkProfile.txt");
_LIT(KFormat, "Pbk Profile;%d; Time;%d;\n");

// --------------------------------------------------------------------------
// TPbk2Profiling::EndProfile
// --------------------------------------------------------------------------
//
EXPORT_C void TPbk2Profiling::EndProfile(TInt aBin) const
    {
    TTime endTime;
    endTime.HomeTime();

    TInt duration = (TInt) endTime.MicroSecondsFrom(iProfileArray[aBin]).Int64();
    RFileLogger::WriteFormat(KPath, KFile, EFileLoggingModeAppend, KFormat, aBin, duration);

	//RDebug::Print(_L("Pbk Profile;%d; Time;%d;\n"),
    //    aBin, endTime.MicroSecondsFrom(iProfileArray[aBin]));
    }

// --------------------------------------------------------------------------
// TPbk2Profiling::TPbk2Profiling
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2Profiling::TPbk2Profiling()
    {
    }

}  // namespace Pbk2Profile

// End of File
