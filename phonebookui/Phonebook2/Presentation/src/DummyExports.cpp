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
* Description:  Phonebook 2 Presentation dummy exports.
*
*/


// INCLUDES
#include <e32std.h>

// Unnamed namespace for local definitions
namespace {

void Panic( TInt aNum )
    {
    _LIT( KPanicText, "Pbk2Presentation_DummyExport" );
    User::Panic( KPanicText, aNum );
    }

}  /// namespace

// Replaces IMPORT_C const TPbk2IconId& FoldingViewItemIconId() const
EXPORT_C void __Pbk2Presentation_DummyExport1() { Panic(1); }

// Replaces  IMPORT_C void SetFoldingViewItemIcon( const TPbk2IconId& aIconId )
EXPORT_C void __Pbk2Presentation_DummyExport2() { Panic(2); }

EXPORT_C void __Pbk2Presentation_DummyExport3() { Panic(3); }

EXPORT_C void __Pbk2Presentation_DummyExport4() { Panic(4); }

EXPORT_C void __Pbk2Presentation_DummyExport5() { Panic(5); }

EXPORT_C void __Pbk2Presentation_DummyExport6() { Panic(6); }

EXPORT_C void __Pbk2Presentation_DummyExport7() { Panic(7); }

// End of File
