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
*       Methods for phonebook contact view listbox model.
*
*/


// INCLUDE FILES
#include "CPbkContactViewListModelCommon.h"  // This class
#include <CPbkIconArray.h>
#include <PbkDebug.h>

// PbkEng.dll include files
#include <CPbkContactEngine.h>
#include <TPbkContactEntry.h>
#include <PbkEngUtils.h>

// PbkExtension classes
#include <CPbkExtGlobals.h>
#include <MPbkContactUiControlExtension.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

const TText KSeparator = '\t';
const TText KSpaceCharacter = ' ';

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactViewListModelCommon::CPbkContactViewListModelCommon
    (PbkContactViewListModelFactory::TParams& aParams) : 
    CPbkContactViewListModelBase(*aParams.iView,iFormattingBuffer), 
    iGroupLabelFieldIndex(KErrNotFound),
    iIconArray(aParams.iIconArray),
    iEmptyIconId(aParams.iEmptyId),
    iDefaultIconId(aParams.iDefaultId)
    {
    // CBase::operator new(TLeave) resets other member data
    }

void CPbkContactViewListModelCommon::ConstructL()
    {
    iGroupLabelFieldIndex = GroupLabelFieldIndexL();
    iViewSortOrder.CopyL(iView.SortOrderL());
    }

CPbkContactViewListModelCommon::~CPbkContactViewListModelCommon()
    {
    iViewSortOrder.Close();
    }

TInt CPbkContactViewListModelCommon::MdcaCount() const
    {
    return CPbkContactViewListModelBase::MdcaCount();
    }

TPtrC CPbkContactViewListModelCommon::MdcaPoint(TInt aIndex) const
    {
    return CPbkContactViewListModelBase::MdcaPoint(aIndex);
    }

void CPbkContactViewListModelCommon::SetUnnamedText(const TDesC* aText)
    {
    iUnnamedText = aText;
    }

void CPbkContactViewListModelCommon::SetContactUiControlExtension
        (MPbkContactUiControlExtension& aControlExtension)
    {
    iControlExtension = &aControlExtension;
    }

void CPbkContactViewListModelCommon::RefreshSortOrderL()
    {
    iViewSortOrder.Close();
    iViewSortOrder.CopyL(iView.SortOrderL());
    }


void CPbkContactViewListModelCommon::AddDynamicIconL
        (const CViewContact& aViewContact) const
    {
	if (iControlExtension)
		{
		const TArray<TPbkIconId>& icons = iControlExtension->
									GetDynamicIconsL( aViewContact.Id() );
		for (TInt i(0); i < icons.Count(); i++)
			{
			AddIconL(icons[i]);
			}
		}
    }

void CPbkContactViewListModelCommon::FormatBufferForGroupL
        (const CViewContact& aViewContact) const
    {
    iBuffer.Zero();
    AppendIconIndex(iDefaultIconId);
    iBuffer.Append(KSeparator);
    AppendName(aViewContact.Field(iGroupLabelFieldIndex));
    }

void CPbkContactViewListModelCommon::FormatEmptyBuffer() const
    {
    TInt iconIndex = iIconArray->FindIcon(iEmptyIconId);
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon(iDefaultIconId);
        }
    iBuffer.Num(iconIndex);
    iBuffer.Append(KSeparator);
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkContactViewListModelCommon::FormatEmptyBuffer(0x%x) formatted: %S"), 
        this, &iBuffer);
    }

void CPbkContactViewListModelCommon::AppendIconIndex(TPbkIconId aIconId) const
    {
    TInt iconIndex = iIconArray->FindIcon(iDefaultIconId);
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon(aIconId);
        }
    if (iconIndex < 0)
        {
        iconIndex = iIconArray->FindIcon(iEmptyIconId);
        }
    iBuffer.AppendNum(iconIndex);
    }

void CPbkContactViewListModelCommon::AppendName(const TDesC& aName) const
    {
    TPtrC text(aName);
    if (text.Length()==0 && iUnnamedText) 
        {
        text.Set(*iUnnamedText);
        }
    PbkEngUtils::AppendGraphicCharacters
        (iBuffer, text.Left(iBuffer.MaxLength()-iBuffer.Length()), KSpaceCharacter);
    }

const RContactViewSortOrder& CPbkContactViewListModelCommon::ViewSortOrder() const
    {
    return iViewSortOrder;
    }

void CPbkContactViewListModelCommon::AddIconL(TPbkIconId aIconId) const
    {
    TInt iconIndex(iIconArray->FindIcon(aIconId));
    if (iconIndex >= 0)
        {
        iBuffer.Append(KSeparator);
        iBuffer.AppendNum(iconIndex);
        }    
    }

//  End of File  
