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
* Description:  Plug-in
*
*/


// INCLUDE FILES
#include    "TBCardVersitPlugin.h"
#include    <vuid.h>

// ==================== MEMBER FUNCTIONS ====================

TBCardVersitPlugin::TBCardVersitPlugin()
    {
    }

TBool TBCardVersitPlugin::AddSpace()
    {
    return EFalse;
    }


TBool TBCardVersitPlugin::DeleteAllSpaces()
    {
    return EFalse;
    }

TBool TBCardVersitPlugin::NeedsBlankLine()
    {
    return EFalse;
    }


void TBCardVersitPlugin::RemoveEscaping(TPtr16& /*aText*/)
    {
    }


void TBCardVersitPlugin::AddEscaping(HBufC16*& /*aText*/)
    {
    }

TBool TBCardVersitPlugin::WrapLine(RWriteStream& /*aStream*/,
        TInt& /*aCurrentLineLength*/, const TPtr8& /*aText*/)
    {
    return EFalse;
    }

TBool TBCardVersitPlugin::EncodingType(
        Versit::TVersitEncoding& aEncoding,
        TBool aRequiresEncoding,
        Versit::TVersitEncoding /*aDefaultEncoding*/,
        TUid aPropertyUid,
        TUint /*aPropertyCharsetId*/)
    {
    // NOTE!!!: if binary then this must be set to EFalse!
    // In case of binary data encoding must be enabled
    if (aRequiresEncoding || aPropertyUid == TUid::Uid(KVersitPropertyBinaryUid))
        {
        return EFalse;
        }

    aEncoding = Versit::ENoEncoding;
    return ETrue;
    }

const TDesC8& TBCardVersitPlugin::EncodingName
    (Versit::TVersitEncoding /*aEncoding*/)
    {
    return KNullDesC8;
    }

//  End of File
