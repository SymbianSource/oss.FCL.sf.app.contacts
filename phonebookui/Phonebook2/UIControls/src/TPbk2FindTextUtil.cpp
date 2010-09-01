/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Find text box util
*
*/



// INCLUDE FILES
#include "TPbk2FindTextUtil.h"
#include <e32std.h>
#include <aknsfld.h>    // CAknSearchField

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicLogic_FindTextBufferL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "TPbk2FindTextUtil");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// ============================ MEMBER FUNCTIONS ===============================

void TPbk2FindTextUtil::EnsureFindTextBufSizeL( 
        const CAknSearchField& aFindBox,
        HBufC** aFindTextBuf )
    {
    // Initial minimum size for the find text buffer
    const TInt KInitialBufSize = 8;

    const TInt findBoxTextLength = aFindBox.TextLength();
    if (findBoxTextLength > 0)
        {
        TInt bufCapacity = 0;
        if ( *aFindTextBuf )
            {
            bufCapacity = (*aFindTextBuf)->Des().MaxLength();
            }
        if (bufCapacity < findBoxTextLength)
            {
            // Allocate a new buffer of at least KInitialBufSize
            // characters or twice as large as the previous one
            const TInt KDoubleSize = 2;
            const TInt newBufSize = Max(Max(KInitialBufSize,
                KDoubleSize*bufCapacity), findBoxTextLength);
            HBufC* newBuf = HBufC::NewL(newBufSize);
            delete *aFindTextBuf;
            *aFindTextBuf = newBuf;
            }
        TPtr bufPtr = (*aFindTextBuf)->Des();
        __ASSERT_DEBUG
            (bufPtr.MaxLength()>=KInitialBufSize &&
            bufPtr.MaxLength()>=findBoxTextLength,
            Panic(EPanicLogic_FindTextBufferL));
        }
        
    // caller expect buffer to be found
    if ( !(*aFindTextBuf ) )
        {
        *aFindTextBuf = HBufC::NewL( KInitialBufSize );
        }
    
    }


//  End of File  
