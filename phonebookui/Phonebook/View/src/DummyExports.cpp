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
*       Dummy exported functions for PbkEng.dll BC maintenance. NEVER put 
*		functions in public APIs to this file.
*
*/


// INCLUDES
#include <e32std.h>

// Unnamed namespace for local definitions
namespace {

void Panic(TInt aNum)
    {
    _LIT(KPanicText, "PbkEngine_DummyExport");
    User::Panic(KPanicText, aNum);
    }

}  // namespace


// Replaces 
// ExecuteLD@CPbkAddressSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@1H@Z @75
EXPORT_C void __PbkEngine_DummyExport1() { Panic(1); }

// ExecuteLD@CPbkEmailAddressSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@H@Z @77
EXPORT_C void __PbkEngine_DummyExport2() { Panic(2); }

// ExecuteLD@CPbkMmsAddressSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@H@Z @87
EXPORT_C void __PbkEngine_DummyExport3() { Panic(3); }

// ExecuteLD@CPbkPhoneNumberSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@H@Z @90
EXPORT_C void __PbkEngine_DummyExport4() { Panic(4); }

// ExecuteLD@CPbkSmsAddressSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@H@Z @96
EXPORT_C void __PbkEngine_DummyExport5() { Panic(5); }

// ExecuteLD@CPbkEmailOverSmsAddressSelect@@QAE?AVTPtrC16@@ABVCPbkContactItem@@PBVTPbkContactItemField@@H@Z @179
EXPORT_C void __PbkEngine_DummyExport6() { Panic(6); }

// SetThumbnailAsyncL@CPbkThumbnailManager@@QAEPAVMPbkThumbnailOperation@@AAVMPbkThumbnailSetObserver@@AAVCPbkContactItem@@ABVCPAlbImageData@@@Z @245
EXPORT_C void __PbkEngine_DummyExport7() { Panic(7); }

// End of File
