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






#ifndef SPDIAINDEXDATAVPBK_H
#define SPDIAINDEXDATAVPBK_H

// INCLUDES
#include <e32std.h>

#include <PhCltTypes.h>
#include <TPbk2IconId.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>


// FORWARD DECLARATIONS
class CGulIcon;
class MPbk2ImageOperation;
class MVPbkStoreContact;
class MVPbkContactOperationBase;
class MVPbkStoreContactField;

// CLASS DECLARATION

/**
*  TSpdiaIndexDataVPbk application class.
*
*  @lib SpdCtrl.DLL
*  @since 
*/
class TSpdiaIndexDataVPbk
    {

    public:     // Constructors and destructor
        /**
        * C++ default constructor.
        */
        TSpdiaIndexDataVPbk();
        
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
        * Returns Operation
        */
        inline MPbk2ImageOperation* ImageOperation();
        
        

        /**
        * Sets operation
        */
        inline void SetImageOperation(MPbk2ImageOperation* aOperation);
        
        /**
        * ETrue is returned if it is the last thumbnail
        */
        inline TBool LastThumb();

        /**
        * Sets last Thumbnail flag
        */
        inline void SetLastThumb(TBool aLast);


        
        /**
        * Gets icon id.
        */
        inline TPbk2IconId IconId();
        
        inline void SetIconId(TPbk2IconId& aId);
         
        /**
        * Sets a SpeedDial Field.
        *
        * @param aContactField     SpeedDial ContactField
        */
        inline void SetContactLink(const MVPbkContactLink* aContactLink);
        /**
        * Returns field id.
        */
        inline MVPbkStoreContactField* Field() const;
        
        /**
        * Store the contact
        */
        inline void SetContact(MVPbkStoreContact* aContact);
        
        /**
        * Retrieve the contact
        */        
        inline MVPbkStoreContact* Contact() const;
        
        void Reset();
        

        enum TDataIndex
            {
            EIndex,
            ENumber,
            EContact,
            EPhoneNumber,
            ETelNumber,
            EContactField,
            EOperationInfo,
            ELastThumb
            };
            
        enum TOperation
        {
        	OPERATION_NONE = 0,
        	OPERATION_RETRIEVE,
        	OPERATION_ASSIGN,
        	OPERATION_REMOVE,
        	OPERATION_ASSIGN_CONTACT
        };  
          
        struct TOperationInfo
        {
        	MVPbkContactOperationBase* iContactOperation;
        	TOperation iOperation; 
        	TOperationInfo(); 	       	
        };
        
        // Modify these to have more than one operation info..
        /**
        * Sets operation			
        */
        inline void SetOperation(MVPbkContactOperationBase* aContactOperation,TOperation aOperation);
        
        inline TBool HasOperation(MVPbkContactOperationBase* aContactOperation);
        
        inline TOperation Operation();


        /**
        * Returns offset address of member data 
        */
        static TInt OffsetValue(TDataIndex aIndex);

    private:    // Data
        TInt iIndex;
        TInt iNumber;

        TPhCltTelephoneNumber iPhoneNumber;
        TPhCltTelephoneNumber iTelNumber;
        TInt iIconIndex;
        TInt iThumbIndex;
        TSize iThumbSize;
        
        const MVPbkContactLink* iContactLink;
        MVPbkStoreContact* iContact;
        
        MPbk2ImageOperation* iImageOperation;
        
        TOperationInfo iOperationInfo;
        
        TBool iLastThumb;
        TInt iFieldId;
        TPbk2IconId iIconId;
       };

    #include "SpdiaIndexDataVPbk.inl"

#endif // SPDIAINDEXDATA_H

// End of File

