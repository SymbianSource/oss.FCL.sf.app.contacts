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
*      MPbkFindPrimitives implementation for european Phonebook.
*
*/


// INCLUDE FILES
#include "CPbkFindPrimitives.h"
#include <MPbkContactNameFormat.h>
#include <MPbkFieldData.h>
#include <PbkViewContactFieldDataAdapter.h>
#include <PbkEngUtils.h>
//#define PBK_ENABLE_DEBUG_PRINT
#include <PbkDebug.h>

namespace {

const TInt KMinTextBufSize = 32;

// ================= LOCAL FUNCTIONS =======================

/** 
 * Normalizes text by converting all sequences of space characters to a 
 * single space.
 *
 * @param aBuffer   buffer where the normalized text will be stored. The buffer
 *                  will be reallocated if necessary.
 * @param aText     the text to be normalized.
 * @return normalized text.
 */
const TDesC& NormalizeL(HBufC*& aBuffer, const TDesC& aText)
    {
    const TInt textLength = aText.Length();
    aBuffer = PbkEngUtils::AllocL(aBuffer, Max(textLength,KMinTextBufSize));
    TPtr bufPtr(aBuffer->Des());
    bufPtr.Zero();
    TBool uncommittedSpace = EFalse;
    for (TInt i=0; i < textLength; ++i)
        {
        TChar ch(aText[i]);
        if (ch.IsSpace())
            {
            uncommittedSpace = ETrue;
            }
        else
            {
            if (uncommittedSpace)
                {
                bufPtr.Append(' ');
                uncommittedSpace = EFalse;
                }
            bufPtr.Append(ch);
            }
        }
    // copy final trailing space
    if (uncommittedSpace)
        {
        bufPtr.Append(' ');
        }

    return (*aBuffer);
    }

}


// ================= MEMBER FUNCTIONS =======================

inline CPbkFindPrimitives::CPbkFindPrimitives(MPbkContactNameFormat& aNameFormatter) :
    iNameFormatter(aNameFormatter)
    {
    }

CPbkFindPrimitives* CPbkFindPrimitives::NewL(MPbkContactNameFormat& aNameFormatter)
    {
    CPbkFindPrimitives* self = new(ELeave) CPbkFindPrimitives(aNameFormatter);
    return self;
    }

CPbkFindPrimitives::~CPbkFindPrimitives()
    {
    delete iFindText;
    delete iFieldText;
    delete iInitialFindText;
    iViewSortOrder.Close();
    }

inline TBool CPbkFindPrimitives::IsWordSeparator(TChar aCh)
    {
    return aCh.IsSpace();
    }

void CPbkFindPrimitives::SetContactViewSortOrderL
        (const RContactViewSortOrder& aSortOrder)
    {
    iViewSortOrder.CopyL(aSortOrder);
    }

/**
 * Stores the initial find text.
 *
 * @see InitialFindText
 * @see MatchesInitialFindText
 */
inline void CPbkFindPrimitives::StoreInitialFindTextL
        (const TDesC& aInitialFindText)
    {
    iInitialFindText = PbkEngUtils::CopyL
        (iInitialFindText, aInitialFindText, aInitialFindText.Length()+1);
    TPtr ptr(iInitialFindText->Des());
    ptr.Append('*');
    }

/**
 * Returns the initial find text passed to StoreInitialFindTextL.
 *
 * @see StoreInitialFindTextL
 * @see MatchesInitialFindText
 */
inline TPtrC CPbkFindPrimitives::InitialFindText() const
    {
    TPtrC result;
    if (iInitialFindText)
        {
        // Strip trailing '*' appended for matching in StoreInitialFindTextL
        return iInitialFindText->Left(iInitialFindText->Length()-1);
        }
    return result;
    }

/**
 * Matches (Using TDesC::MatchC) the initial find text against aText. Returns
 * ETrue if aText matches the initial find text.
 *
 * @see StoreInitialFindTextL
 * @see InitialFindText
 */
inline TBool CPbkFindPrimitives::DoMatchesInitialFindText
        (const TDesC& aText) const
    {
    if (iInitialFindText)
        {
        return (aText.MatchC(*iInitialFindText) != KErrNotFound);
        }
    return EFalse;
    }


/**
 * Does a prefix matching of aFindText to aContactData. Returns ETrue if 
 * the find text matches completely to contact data.
 *
 * @param aContactData  contact data to match against. Matching is done in
 *                      field order.
 * @param aFindText     normalized (see NormalizeL) find text.
 */
inline TBool CPbkFindPrimitives::IsMatchL
        (const MPbkFieldDataArray& aContactData, const TDesC& aFindText)
    {
    /* Consuming of findtext is done in the sort order */
    TPtrC findText(aFindText);
    const TInt fieldCount = aContactData.PbkFieldCount();
    for (TInt i=0; i < fieldCount; ++i)
        {
        const TDesC& fieldText = NormalizeL(iFieldText, aContactData.PbkFieldAt(i).PbkFieldText());
        const TInt fieldTextLength = fieldText.Length();
        // find word starts from fieldText
        TInt cursor = 0;
        TInt wordStart = 0;
        while (cursor < fieldTextLength)
            {
            TBool lastCharWasSpace = EFalse;
            TChar cursorChar(fieldText[cursor]);
            if (IsWordSeparator(cursorChar))
                {
                TPtrC word(&fieldText[wordStart], fieldTextLength-wordStart);
                TInt matchLen = Min(word.Length(), findText.Length());
                if (word.Left(matchLen).MatchC(findText.Left(matchLen)) != KErrNotFound)
                    {
                    // Require space in find text between fields
                    if (matchLen < findText.Length())
                        {
                        if (TChar(findText[matchLen]).IsSpace() && 
                            i < fieldCount-1)
                            {
                            // Skip space
                            ++matchLen;
                            }
                        else
                            {
                            return EFalse;
                            }
                        }
                    // Consume findText
                    findText.Set(findText.Mid(matchLen));
                    }
                lastCharWasSpace = ETrue;
                }
            ++cursor;
            if (lastCharWasSpace)
                {
                wordStart = cursor;
                }
            }

        if (cursor == fieldTextLength && wordStart != cursor)
            {
            TPtrC word(&fieldText[wordStart], fieldTextLength-wordStart);
            TInt matchLen = Min(word.Length(), findText.Length());
            if (word.Left(matchLen).MatchC(findText.Left(matchLen)) != KErrNotFound)
                {
                // Require space in find text between fields
                if (matchLen < findText.Length())
                    {
                    if (TChar(findText[matchLen]).IsSpace() && 
                        i < fieldCount-1)
                        {
                        // Skip space
                        ++matchLen;
                        }
                    else
                        {
                        return EFalse;
                        }
                    }
                // Consume findText
                findText.Set(findText.Mid(matchLen));
                }
            }
        }
    // Find text must be fully consumed for a match
    return (findText.Length() == 0);
    }


/**
 * Converts aViewContact to MPbkFieldDataArray and calls 
 * IsMatchL(const MPbkFieldDataArray& aContactData, const TDesC& aFindText).
 */
TBool CPbkFindPrimitives::IsMatchL
        (const CViewContact& aViewContact, const TDesC& aFindText)
    {
    // Convert aViewContact to MPbkFieldDataArray
    const TPbkViewContactFieldDataArray contactData
        (aViewContact, iViewSortOrder);
    
    // Filter contactData to contain only fields used in find
    // (same fields than in used in title)
    const MPbkFieldDataArray& titleData = 
        iNameFormatter.FilterContactTitleFields(contactData);
    
    // Do matching
    return IsMatchL(titleData, aFindText);
    }

TBool CPbkFindPrimitives::IsFindMatchL
        (const CViewContact& aViewContact, const TDesC& aFindText)
    {
    // Normalize aFindText by removing all extra spaces
    const TDesC& findText = NormalizeL(iFindText,aFindText);

    // Do matching
    return IsMatchL(aViewContact, findText);
    }

void CPbkFindPrimitives::GetInitialMatchesL
        (CContactViewBase& aView,
        const TDesC& aFindText,
        RPointerArray<CViewContact>& aMatchedContacts)
    {
    // Make first match with CContactViewBase::ContactsMatchingPrefixL
    MDesCArray* findWords = PbkEngUtils::BreakInWordsLC
        (aFindText, IsWordSeparator);
	aView.ContactsMatchingPrefixL(*findWords, aMatchedContacts);
    CleanupStack::PopAndDestroy();  // findWords
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkFindPrimitives::GetInitialMatchesL(%S): contact model returned %d matches"), 
        &aFindText, aMatchedContacts.Count());

    StoreInitialFindTextL(aFindText);

    // Filter away contacts that do not fullfill this implementation's criteria
    // like matching company name entries.
    const TDesC& normFindText = NormalizeL(iFindText, aFindText);
    for (TInt i=aMatchedContacts.Count()-1; i >= 0; --i)
        {
        CViewContact* contact = aMatchedContacts[i];
        if (!IsMatchL(*contact, normFindText))
            {
            aMatchedContacts.Remove(i);
            delete contact;
            }
        }    
    }

TBool CPbkFindPrimitives::MatchesInitialFindTextL(const TDesC& aText)
    {
    // NOTE: This must work the same way as cntmodel's matching! Otherwise
    // we get problems in hungarian or czech matching with double-characters.
    // The main problem case is in CFindState::SetFindTextL(). If you
    // make this work differently from cntmodel's match, make sure
    // it still works with hungarian "gy" or czech "ch" and "agy" etc...
    return DoMatchesInitialFindText(aText);
    }

TBool CPbkFindPrimitives::MatchesInitialFindTextL(const CViewContact& aViewContact)
    {
    TPtrC findText(InitialFindText());
    if (findText.Length() > 0)
        {
        return IsMatchL(aViewContact,findText);
        }
    return EFalse;
    }


// End of File