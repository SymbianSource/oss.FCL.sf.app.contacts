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
*       Provides methods for phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "CPbkContactGroupListModel.h"  // This class
#include <cntviewbase.h>    // CContactViewBase
#include <PbkEngUtils.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
_LIT(KCharsToReplace, "\t");
_LIT(KReplacementChars, " ");

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicLogic_FormatBufferForContactL
    };

static void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactGroupListModel");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactGroupListModel::CPbkContactGroupListModel
        (CContactViewBase& aView) : 
    CPbkContactViewListModelBase(aView,iFormattingBuffer), 
    iGroupLabelFieldIndex(-1)
    {
    }

inline void CPbkContactGroupListModel::ConstructL()
    {
    __ASSERT_DEBUG(iView.ContactViewPreferences() & EGroupsOnly, 
        Panic(EPanicPreCond_ConstructL));
    iGroupLabelFieldIndex = GroupLabelFieldIndexL();
    }

CPbkContactGroupListModel* CPbkContactGroupListModel::NewL
        (CContactViewBase& aGroupView)
    {
    CPbkContactGroupListModel* self =
        new(ELeave) CPbkContactGroupListModel(aGroupView);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactGroupListModel::~CPbkContactGroupListModel()
    {
    }

void CPbkContactGroupListModel::FormatBufferForContactL
        (const CViewContact& /*aViewContact*/) const
    {
    __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_FormatBufferForContactL));
    }

void CPbkContactGroupListModel::FormatBufferForGroupL
        (const CViewContact& aViewContact) const
    {
    iBuffer.Zero();
    AppendName(aViewContact.Field(iGroupLabelFieldIndex));
    }

void CPbkContactGroupListModel::AppendName(const TDesC& aName) const
    {
    const TInt spaceLeft = iBuffer.MaxLength()-iBuffer.Length();
    PbkEngUtils::AppendAndReplaceChars
        (iBuffer, aName.Left(spaceLeft), KCharsToReplace, KReplacementChars);
    }

void CPbkContactGroupListModel::FormatEmptyBuffer() const
    {
    iBuffer.Zero();
    }


//  End of File  
