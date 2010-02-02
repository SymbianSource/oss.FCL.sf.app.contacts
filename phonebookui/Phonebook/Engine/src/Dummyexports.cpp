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


// Replaces IMPORT_C void ConstructL(TResourceReader& aResReader);
EXPORT_C void __PbkEngine_DummyExport1() { Panic(1); }

// public: static class CPbkSimEntryCopierBase * __cdecl PbkSimEntryCopyFactory::CreateLC(class CPbkSimEntry &,class CPbkContactEngine &)
EXPORT_C void __PbkEngine_DummyExport2() { Panic(2); }

// CDigResourceObjectArrayImpl::~CDigResourceObjectArrayImpl(void)
EXPORT_C void __PbkEngine_DummyExport3() { Panic(3); }

// CDigResourceObjectArrayImpl::FindObject(int) const
EXPORT_C void __PbkEngine_DummyExport4() { Panic(4); }

// CDigResourceObjectArrayImpl::ResetAndDestroy(void)
EXPORT_C void __PbkEngine_DummyExport5() { Panic(5); }

// PbkEng_TestAccess::PrepareAfterLoadL(CPbkContactItem &)
EXPORT_C void __PbkEngine_DummyExport6() { Panic(6); }

// FindField@CPbkFieldsInfo@@QBEPAVCContactItemField@@ABVCContactItem@@H@Z @103
EXPORT_C void __PbkEngine_DummyExport7() { Panic(7); }

// ?NewLC@CPbkContactItem@@SAPAV1@PAVCContactItem@@ABVCPbkFieldsInfo@@AAVMPbkContactNameFormat@@@Z @190
EXPORT_C void __PbkEngine_DummyExport8() { Panic(8); }

// OrderingGroup@CPbkFieldInfo@@QBEHXZ @199
EXPORT_C void __PbkEngine_DummyExport9() { Panic(9); }

// OrderingItem@CPbkFieldInfo@@QBEHXZ @200
EXPORT_C void __PbkEngine_DummyExport10() { Panic(10); }

// PbkFieldDisplayTextL@MPbkFieldData@@QBEXAAVTDes16@@ABVTDesC16@@@Z @204
EXPORT_C void __PbkEngine_DummyExport11() { Panic(11); }


// End of File
