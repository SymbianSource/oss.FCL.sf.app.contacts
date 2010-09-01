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
*       Provides methods for Phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "CPbkContactViewListModelBase.h"  // This class
#include <cntviewbase.h>    // CContactViewBase
#include <eikenv.h>         // CEikonEnv

#include <PbkDebug.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicInvalidContactType = 1
    };

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactViewListModelBase");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactViewListModelBase::CPbkContactViewListModelBase
        (CContactViewBase& aView, TDes& aBuffer)
    : iView(aView), iBuffer(aBuffer), iPreviousCount(0)
    {
    }

CPbkContactViewListModelBase::~CPbkContactViewListModelBase()
    {
    }

TInt CPbkContactViewListModelBase::GroupLabelFieldIndexL() const
    {
    TInt result = 0;
    const RContactViewSortOrder& sortOrder = iView.SortOrderL();
    for (TInt i=0; i < sortOrder.Count(); ++i)
        {
        if (sortOrder[i]==KUidContactFieldTemplateLabel)
            {
            result = i;
            }
        }
    return result;
    }

TInt CPbkContactViewListModelBase::MdcaCount() const
    {
    TInt result = iPreviousCount;
    TRAPD(err, result = iView.CountL());
    if (err != KErrNone)
        {
        HandleError(err);
        }
    iPreviousCount = result;
    return result;
    }

TPtrC CPbkContactViewListModelBase::MdcaPoint(TInt aIndex) const
    {
    TRAPD(err, FormatBufferL(aIndex));
    switch (err)
        {
        case KErrNone:
            {
            // OK
            break;
            }

        case KErrNotFound:      // FALLTHROUGH
        case KErrAccessDenied:
        case KErrDied:
            {
            PBK_DEBUG_PRINT(
                PBK_DEBUG_STRING("CPbkContactViewListModelBase::MdcaPoint(0x%x,%d), ignoring error %d"),
                this, aIndex, err);
            // Do not report these errors as they occur sometimes when
            // the view is under a massive update
            FormatEmptyBuffer();
            break;
            }

        default:
            {
            PBK_DEBUG_PRINT(
                PBK_DEBUG_STRING("CPbkContactViewListModelBase::MdcaPoint(0x%x,%d), reporting error %d"),
                this, aIndex, err);
            FormatEmptyBuffer();
            // Report error
            HandleError(err);
            break;
            }
        }
    return iBuffer;
    }

void CPbkContactViewListModelBase::AddDynamicIconL
        (const CViewContact& /*aViewContact*/) const
    {
    }

void CPbkContactViewListModelBase::FormatEmptyBuffer() const
    {
    iBuffer.Zero();
    }

void CPbkContactViewListModelBase::HandleError(TInt aError) const
    {
    CEikonEnv::Static()->NotifyIdleErrorWhileRedrawing(aError);
    }

void CPbkContactViewListModelBase::FormatBufferL(TInt aIndex) const
    {
    const CViewContact& contact = iView.ContactAtL(aIndex);
    switch (contact.ContactType())
        {
        case CViewContact::EContactItem:
            {
            FormatBufferForContactL(contact);
            TRAPD(err, AddDynamicIconL(contact));
            if (err)
                {
                HandleError(err);
                }
            break;
            }
        case CViewContact::EGroup:
            {
            FormatBufferForGroupL(contact);
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicInvalidContactType));
            break;
            }
        }
    }

//  End of File
