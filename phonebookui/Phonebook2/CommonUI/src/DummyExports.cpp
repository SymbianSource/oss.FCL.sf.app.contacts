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
* Description:  Phonebook 2 Common UI dummy exports.
*
*/


// INCLUDES
#include <e32std.h>

// Unnamed namespace for local definitions
namespace {

void Panic( TInt aNum )
    {
    _LIT( KPanicText, "Pbk2CommonUI_DummyExport" );
    User::Panic( KPanicText, aNum );
    }

}  /// namespace

// Replaces IMPORT_C CVPbkContactManager& ContactManager() const
EXPORT_C void __Pbk2CommonUI_DummyExport1() { Panic( 1 ); }

// Replaces IMPORT_C CPbk2StoreConfiguration& StoreConfiguration() const
EXPORT_C void __Pbk2CommonUI_DummyExport2() { Panic( 2 ); }

// Replaces IMPORT_C static CPbk2AppGlobalsBase* InstanceL()
EXPORT_C void __Pbk2CommonUI_DummyExport3() { Panic( 3 ); }

// Replaces IMPORT ~CPbk2ViewNode()
EXPORT_C void __Pbk2CommonUI_DummyExport4() { Panic( 4 ); }
EXPORT_C void __Pbk2CommonUI_DummyExport7() { Panic( 6 ); }
EXPORT_C void __Pbk2CommonUI_DummyExport6() { Panic( 7 ); }

// Replaces IMPORT ~CPbk2ViewGraph()
EXPORT_C void __Pbk2CommonUI_DummyExport5() { Panic( 5 ); }
EXPORT_C void __Pbk2CommonUI_DummyExport8() { Panic( 8 ); }
EXPORT_C void __Pbk2CommonUI_DummyExport9() { Panic( 9 ); }

// End of File
