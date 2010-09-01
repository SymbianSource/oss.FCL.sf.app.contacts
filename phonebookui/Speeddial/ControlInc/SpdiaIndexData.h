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






#ifndef SPDIAINDEXDATA_H
#define SPDIAINDEXDATA_H

// INCLUDES
#include <e32std.h>
#include <TPbkContactItemField.h>
#include <PhCltTypes.h>

// FORWARD DECLARATIONS
class CGulIcon;
class MPbkThumbnailOperation;

// CLASS DECLARATION

/**
*  TSpdiaIndexData application class.
*
*  @lib SpdCtrl.DLL
*  @since 
*/
class TSpdiaIndexData
    {

    public:     // Constructors and destructor
        /**
        * C++ default constructor.
        */
        TSpdiaIndexData();

    public:     // New function
        /**
        * Sets a SpeedDial list box index.
        *
        * @param aIndex : SpeedDial list box index
        */
        inline void SetIndex(TInt aIndex);

        /**
        * Returns SpeedDial list box index.
        */
        inline TInt Index() const;

        /**
        * Sets a SpeedDial list box index.
        *
        * @param aIndex : SpeedDial list box index
        */
        inline void SetNumber(TInt aNumber);

        /**
        * Return SpeedDial list box index.
        */
        inline TInt Number() const;

        /**
        * Sets a link file name.
        *
        * @param aFileName      a link file name
        */
        inline void SetPhoneNumber(const TDesC& aPoneNumber);

        /**
        * Returns Link Phone number.
        */
        inline const TDesC& PhoneNumber() const;

        /**
        * Returns Link Telphone number(remove invalid chars).
        */
        inline const TDesC& TelNumber() const;

        /**
        * Sets a SpeedDial contact id.
        *
        * @param aContactId     SpeedDial contact id
        */
        inline void SetContactId(TContactItemId aContactId);

        /**
        * Returns SpeedDial contact id.
        */
        inline TContactItemId ContactId() const;

        /**
        * Sets a icon index number
        * A flag is stood, when SpeedDial data is compared
        * with a directory entry and there is an addition.
        *
        * @param aFlag      Update flag
        */

        inline void SetIconIndex(TInt aIndex);
        /**
        * Returns icon index number
        */
        inline TInt IconIndex() const;
                
        /**
        * Sets a thumb index number
        * A flag is stood, when SpeedDial data is compared
        * with a directory entry and there is an addition.
        *
        * @param aIndex     a index of control data
        * @param aBitmap
        */
        inline void SetThumbIndex(TInt aIndex, const CFbsBitmap* aBitmap);

       /**
        * Returns thumbnail index number.
        */
        inline TInt ThumbIndex() const;

       /**
        * Reset thumbnail index data
        */
        inline void ResetThumbIndex();

        /**
        * Returns thumbnail Size
        */
        inline TSize ThumbSize() const;

        /**
        * Returns ContactItem
        */
        inline CPbkContactItem* ContactItem();

        /**
        * Sets contact item id.
        *
        * @param aItem 
        */
        inline void SetContactItem(CPbkContactItem* aItem);

        /**
        * Returns Operation
        */
        inline MPbkThumbnailOperation* Operation();

        /**
        * Sets operation
        */
        inline void SetOperation(MPbkThumbnailOperation* aOperation);

        /**
        * ETrue is returned if it is the last thumbnail
        */
        inline TBool LastThumb();

        /**
        * Sets last Thumbnail flag
        */
        inline void SetLastThumb(TBool aLast);

        /**
        * Returns field id.
        */
        inline TInt FieldId() const;

        /**
        * Sets field id.
        */
        inline void SetFieldId(TInt aFieldId);

        enum TDataIndex
            {
            EIndex,
            ENumber,
            EContactId,
            EPhoneNumber,
            ETelNumber,
            EContactItem,
            EOperation,
            ELastThumb
            };

        /**
        * Returns offset address of member data 
        */
        static TInt OffsetValue(TDataIndex aIndex);

    private:    // Data
        TInt iIndex;
        TInt iNumber;
        TContactItemId iContactId;
        TPhCltTelephoneNumber iPhoneNumber;
        TPhCltTelephoneNumber iTelNumber;
        TInt iIconIndex;
        TInt iThumbIndex;
        TSize iThumbSize;
        CPbkContactItem* iContactItem;
        MPbkThumbnailOperation* iOperation;
        TBool iLastThumb;
        TInt iFieldId;
    };

    #include "SpdiaIndexData.inl"

#endif // SPDIAINDEXDATA_H

// End of File

