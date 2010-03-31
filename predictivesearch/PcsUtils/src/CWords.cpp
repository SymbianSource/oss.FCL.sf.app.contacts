/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class to hold data for predictive search.
*                Used to marshal data between the client, server and data 
*                plugins.
*
*/

// INCLUDE FILES
#include "CWords.h"


// CWords
inline CWords::CWords()
    {
    }

EXPORT_C CWords* CWords::NewLC
        (const TDesC& aText)
    {
    CWords* self = new(ELeave) CWords();
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    return self;
    }

EXPORT_C CWords::~CWords()
    {
    iWords.Close();
    }

EXPORT_C TInt CWords::MdcaCount() const
    {
    return iWords.Count();
    }

EXPORT_C TPtrC16 CWords::MdcaPoint(TInt aIndex) const
    {
    return iWords[aIndex];
    }

void CWords::ConstructL(const TDesC& aText)
    {
    const TInt textLength = aText.Length();
    for (TInt beg=0; beg < textLength; ++beg)
        {
        // Skip separators before next word
        if (!DefaultIsWordSeparator(aText[beg]))
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < textLength && !DefaultIsWordSeparator(aText[end]); ++end)
                {
                }
            const TInt len = end-beg;
            // Append found word to the array
            iWords.AppendL( aText.Mid(beg,len) );
            // Scan for next word
            beg = end;
            }
        }

    if (iWords.Count()==0 && textLength > 0)
        {
        // aText is all word separator characters -> make a "word" out of those
        iWords.AppendL(aText);
        }
    }

TBool CWords::DefaultIsWordSeparator(TChar aChar)
    {
    return (aChar.IsSpace());
    }

