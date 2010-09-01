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
*    Chinese find primitives implementation for Phonebook.
*
*/


// INCLUDE FILES
#include "CPbkChineseFindPrimitives.h"
#include <FindUtil.h>
#include <CPbkViewContactNameFormatter.h>
#include <PbkEngUtils.h>

#include <PbkDebug.h>
#include "PbkProfiling.h"


// ==================== MEMBER FUNCTIONS ====================

inline CPbkChineseFindPrimitives::CPbkChineseFindPrimitives()
    {
    }

inline void CPbkChineseFindPrimitives::ConstructL
        (MPbkContactNameFormat& aContactNameFormatter)
    {
    __PBK_PROFILE_RESET(PbkProfiling::EContactsMatchingCriteria);
    iFindUtil = CFindUtil::NewL();
    if (!iFindUtil)
        {
        User::Leave(KErrNotSupported);
        }

    iViewContactNameFormatter = 
        CPbkViewContactNameFormatter::NewL(aContactNameFormatter);
    }

/**
 * Returns formatted contact title for aViewContact.
 */
inline const TDesC& CPbkChineseFindPrimitives::ContactTitleL
        (const CViewContact& aViewContact)
    {
    return iViewContactNameFormatter->ContactTitleL
        (aViewContact,iContactViewSortOrder);
    }

/**
 * Returns ETrue if aCh is a word separator character.
 */
inline TBool CPbkChineseFindPrimitives::IsWordSeparator(TChar aCh)
    {
	return aCh==' '||aCh=='-'||aCh=='\t';
    }

/**
 * Returns true if aTargetText matches aFindText.
 */
inline TBool CPbkChineseFindPrimitives::IsMatchL
        (const TDesC& aTargetText, const TDesC& aFindText)
    {
	return iFindUtil->Interface()->MatchRefineL(aTargetText, aFindText);
    }

CPbkChineseFindPrimitives* CPbkChineseFindPrimitives::NewL
        (MPbkContactNameFormat& aContactNameFormatter)
    {
    CPbkChineseFindPrimitives* self = new(ELeave) CPbkChineseFindPrimitives;
    CleanupStack::PushL(self);
    self->ConstructL(aContactNameFormatter);
    CleanupStack::Pop(self);
    return self;    
    }

CPbkChineseFindPrimitives::~CPbkChineseFindPrimitives()
    {
    delete iInitialFindText;
    iContactViewSortOrder.Close();
    delete iViewContactNameFormatter;
	delete iFindUtil;
    }

void CPbkChineseFindPrimitives::SetContactViewSortOrderL
        (const RContactViewSortOrder& aSortOrder)
    {
    iContactViewSortOrder.CopyL(aSortOrder);
    }

TBool CPbkChineseFindPrimitives::IsFindMatchL
        (const CViewContact& aViewContact, const TDesC& aFindText)
    {
    return IsMatchL(ContactTitleL(aViewContact), aFindText);
    }

void CPbkChineseFindPrimitives::GetInitialMatchesL
        (CContactViewBase& aView,
        const TDesC& aFindText,
        RPointerArray<CViewContact>& aMatchedContacts)
    {
    MDesCArray* findWords = PbkEngUtils::BreakInWordsLC(aFindText, IsWordSeparator);
    __PBK_PROFILE_START(PbkProfiling::EContactsMatchingCriteria);
	aView.ContactsMatchingCriteriaL(*findWords, aMatchedContacts);    
    __PBK_PROFILE_END(PbkProfiling::EContactsMatchingCriteria);
    // Please note that european Phonebook uses CContactViewBase::ContactsMatchingPrefixL() 
    CleanupStack::PopAndDestroy();  // findWords

    // Store initial find text
    iInitialFindText = PbkEngUtils::CopyL(iInitialFindText, aFindText);

    // Filter away any nonmatching contacts returned by 
    // CContactViewBase::ContactsMatchingCriteriaL
    for (TInt i = aMatchedContacts.Count()-1; i >= 0; --i)
        {
        CViewContact* contact = aMatchedContacts[i];
        if (!IsMatchL(ContactTitleL(*contact),aFindText))
            {
            aMatchedContacts.Remove(i);
            delete contact;
            }
        }

    __PBK_PROFILE_DISPLAY(PbkProfiling::EContactsMatchingCriteria);
    }

TBool CPbkChineseFindPrimitives::MatchesInitialFindTextL(const TDesC& aText)
    {
    return (iInitialFindText ? IsMatchL(aText,*iInitialFindText) : EFalse);
    }

TBool CPbkChineseFindPrimitives::MatchesInitialFindTextL
        (const CViewContact& aViewContact)
    {
    return MatchesInitialFindTextL(ContactTitleL(aViewContact));
    }


// End of File
