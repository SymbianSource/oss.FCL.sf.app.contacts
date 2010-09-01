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
* Description:     Declares index data for SpdCtrl.
*
*/






// INCLUDE FILES

#include "SpdiaIndexData.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TSpdiaIndexData::SetIndex
// Sets the SpeedDial list box index
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetIndex
        (TInt aIndex)
    {
    iIndex = aIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Index
// Returns the SpeedDial list box index
// ---------------------------------------------------------
//
inline TInt TSpdiaIndexData::Index() const
    {
    return iIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::SetNumber
// Sets the SpeedDial list box Number
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetNumber
            (TInt aNumber)
    {
    iNumber = aNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Number
// Returns the SpeedDial list box Number
// ---------------------------------------------------------
//
inline TInt TSpdiaIndexData::Number() const
    {
    return iNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::SetPhoneNumber
// Sets the link phone number
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetPhoneNumber
        (const TDesC& aPhoneNumber)
    {
    iPhoneNumber = aPhoneNumber;
    iPhoneNumber.Trim();
    iTelNumber = iPhoneNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::PhoneNumber()
// Returns link phone number
// ---------------------------------------------------------
//
inline const TDesC& TSpdiaIndexData::PhoneNumber() const
    {
    return iPhoneNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::TelNumber()
// Returns the link phone number
// ---------------------------------------------------------
//
inline const TDesC& TSpdiaIndexData::TelNumber() const
    {
    return iTelNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::SetContactId
// Sets the Speed dial contactid
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetContactId
        (TContactItemId aContactId)
    {
    iContactId = aContactId;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::ContactId
// Returns the Speed dial contact id
// ---------------------------------------------------------
//
inline TContactItemId TSpdiaIndexData::ContactId() const
    {
    return iContactId;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::SetIcon
// Sets the icon index
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetIconIndex(TInt aIndex)
    {
    iIconIndex = aIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Icon
// Returns icon index
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexData::IconIndex() const
    {
    return iIconIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::SetThumb
// Sets the thumbnail index
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetThumbIndex(TInt aIndex, const CFbsBitmap* aBitmap)
    {
    iThumbIndex = aIndex;
    if (aBitmap)
        {
        iThumbSize = aBitmap->SizeInPixels();
        }
    }

// ---------------------------------------------------------
// TSpdiaIndexData::ResetThumbIndex
// Sets the thumbnail index
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::ResetThumbIndex()
    {
    iThumbIndex = -1;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Thumb
// Returns the Thumbnail index
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexData::ThumbIndex() const
    {
    return iThumbIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::ThumbSize
// Returns the Thumbnail size
// ---------------------------------------------------------
//
inline TSize TSpdiaIndexData::ThumbSize() const
    {
    return iThumbSize;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::ContactItem
// Returns the Contact's item
// ---------------------------------------------------------
//
inline CPbkContactItem* TSpdiaIndexData::ContactItem()
    {
    return iContactItem;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::ContactItem
// Sets the Contact's item.
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetContactItem(CPbkContactItem* aItem)
    {
    iContactItem = aItem;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Operation
// Returns the thumbnail operation.
// ---------------------------------------------------------
//
inline MPbkThumbnailOperation* TSpdiaIndexData::Operation()
    {
    return iOperation;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Operation
// Sets the thumbnail operation.
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetOperation(MPbkThumbnailOperation* aOperation)
    {
    iOperation = aOperation;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::OperatLastThumbion
// Returns the mark of last thumbnail.
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexData::LastThumb()
    {
    return iLastThumb;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::Operation
// Mark the last thumbnail data.
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetLastThumb(TBool aLast)
    {
    iLastThumb = aLast;
    }

// ---------------------------------------------------------
// TSpdiaIndexData::FieldId
// Returns field id.
// ---------------------------------------------------------
//
inline TInt TSpdiaIndexData::FieldId() const
{
    return iFieldId;
}

// ---------------------------------------------------------
// TSpdiaIndexData::SetFieldId
// Sets field id.
// ---------------------------------------------------------
//
inline void TSpdiaIndexData::SetFieldId(TInt aFieldId)
{
    iFieldId = aFieldId;
}

// End of File

