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

#include "SpdiaIndexDataVPbk.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetIndex
// Sets the SpeedDial list box index
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetIndex
        (TInt aIndex)
    {
    iIndex = aIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Index
// Returns the SpeedDial list box index
// ---------------------------------------------------------
//
inline TInt TSpdiaIndexDataVPbk::Index() const
    {
    return iIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetNumber
// Sets the SpeedDial list box Number
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetNumber
            (TInt aNumber)
    {
    iNumber = aNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Number
// Returns the SpeedDial list box Number
// ---------------------------------------------------------
//
inline TInt TSpdiaIndexDataVPbk::Number() const
    {
    return iNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetPhoneNumber
// Sets the link phone number
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetPhoneNumber
        (const TDesC& aPhoneNumber)
    {
    iPhoneNumber = aPhoneNumber;
    // iPhoneNumber.Trim();
    iTelNumber = iPhoneNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::PhoneNumber()
// Returns link phone number
// ---------------------------------------------------------
//
inline const TDesC& TSpdiaIndexDataVPbk::PhoneNumber() const
    {
    return iPhoneNumber;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::TelNumber()
// Returns the link phone number
// ---------------------------------------------------------
//
inline const TDesC& TSpdiaIndexDataVPbk::TelNumber() const
    {
    return iTelNumber;
    }


// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetIcon
// Sets the icon index
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetIconIndex(TInt aIndex)
    {
    iIconIndex = aIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Icon
// Returns icon index
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexDataVPbk::IconIndex() const
    {
    return iIconIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetThumb
// Sets the thumbnail index
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetThumbIndex(TInt aIndex, const CFbsBitmap* aBitmap)
    {
    iThumbIndex = aIndex;
    if (aBitmap)
        {
        iThumbSize = aBitmap->SizeInPixels();
        }
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::ResetThumbIndex
// Sets the thumbnail index
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::ResetThumbIndex()
    {
    iThumbIndex = -1;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Thumb
// Returns the Thumbnail index
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexDataVPbk::ThumbIndex() const
    {
    return iThumbIndex;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::ThumbSize
// Returns the Thumbnail size
// ---------------------------------------------------------
//
inline TSize TSpdiaIndexDataVPbk::ThumbSize() const
    {
    return iThumbSize;
    }


// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Operation
// Returns the thumbnail operation.
// ---------------------------------------------------------
//
inline MPbk2ImageOperation* TSpdiaIndexDataVPbk::ImageOperation()
    {
    return iImageOperation;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Operation
// Sets the thumbnail operation.
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetImageOperation(MPbk2ImageOperation* aOperation)
    {
    iImageOperation = aOperation;
    }
    
inline MVPbkStoreContact* TSpdiaIndexDataVPbk::Contact() const
{
	return iContact;
}

 inline void TSpdiaIndexDataVPbk::SetContact(MVPbkStoreContact* aContact)
 {
 	iContact = aContact;
 }
    
// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Operation
// Sets the thumbnail operation.
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetOperation(MVPbkContactOperationBase* aContactOperation,TOperation aOperation)
    {
    if(iOperationInfo.iContactOperation != NULL)
    {
    	delete iOperationInfo.iContactOperation;
    }
    iOperationInfo.iContactOperation = aContactOperation;
    iOperationInfo.iOperation = aOperation;
    }
    
inline TBool TSpdiaIndexDataVPbk::HasOperation(MVPbkContactOperationBase* aContactOperation)
	{
	return (iOperationInfo.iContactOperation == aContactOperation);
	}
	
inline TSpdiaIndexDataVPbk::TOperation TSpdiaIndexDataVPbk::Operation()
	{
	return iOperationInfo.iOperation;
	}
	
// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::OperatLastThumbion
// Returns the mark of last thumbnail.
// ---------------------------------------------------------
//
inline TBool TSpdiaIndexDataVPbk::LastThumb()
    {
    return iLastThumb;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::Operation
// Mark the last thumbnail data.
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetLastThumb(TBool aLast)
    {
    iLastThumb = aLast;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::FieldId
// Returns field id.
// ---------------------------------------------------------
//
inline MVPbkStoreContactField* TSpdiaIndexDataVPbk::Field() const
{
	MVPbkStoreContactField* field = NULL;
	if(iContact != NULL && iContactLink != NULL)
	{
		field = iContact->Fields().RetrieveField(*iContactLink);
	}
    return field;
}

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::SetContactField
// Sets contact field.
// ---------------------------------------------------------
//
inline void TSpdiaIndexDataVPbk::SetContactLink(const MVPbkContactLink* aContactLink)
{
	if(iContactLink != NULL)
	{
		delete iContactLink;
	}
    iContactLink = aContactLink;
}

// End of File

inline void TSpdiaIndexDataVPbk::SetIconId(TPbk2IconId& aId)
{
	iIconId = aId;
}


