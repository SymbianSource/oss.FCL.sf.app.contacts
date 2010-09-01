/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/








#ifndef __BCTContactWrapper_H__
#define __BCTContactWrapper_H__

//  INCLUDES
#include <e32base.h>
#include <PbkFields.hrh>
#include <CPbkFieldInfo.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class CBCTBCardData;
class TPbkContactItemField;
class CPbkContactEngine;
class TVersitDateTime;

// CLASS DECLARATION
class CBCTContactWrapper :public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CBCTContactWrapper* NewL( 
            CPbkContactItem& aContact, 
            CPbkContactEngine& aContactEngine,
            const TBool aForVCard);
        
        /**
        * Destructor.
        */
        virtual ~CBCTContactWrapper();

    public: // New functions
        void GetContactDataL(CBCTBCardData& aData);
        void SetContactDataL(CBCTBCardData& aData);


        
    private:
        
        CBCTContactWrapper(
            CPbkContactItem& aContact, 
            CPbkContactEngine& aContactEngine,
            const TBool aForVCard);
        void ConstructL();        
        HBufC* GetDefaultPhoneL();
        HBufC* GetFieldTextL(const TPbkContactItemField* aField);

        HBufC* GetDataFieldContentsL(
            const TPbkFieldId aId,
            const TPbkFieldLocation aLocation = EPbkFieldLocationNone);

        HBufC* GetDateL(const TPbkFieldId aId);
        TInt MonthToNumber(const TMonth aMonth) const;
        void SetDateL(const TPbkFieldId aId, const TDesC* aDataLine);
        TVersitDateTime* GetVersitDateTimeL(const TDesC* aDataText);
        TMonth NumberToMonth(const TInt aNumber) const;
        TBool IsDefaultPhone(const TPbkContactItemField* aField) const;
        
        TPbkContactItemField* FindFieldForModificationL( 
	        CPbkContactItem& aItem,
	        const TPbkFieldId aFieldId,
            const TPbkFieldLocation aLocation);
        
        TBool AddFieldWithValueL(
	        TPbkFieldId aFieldId,
            const TPbkFieldLocation aLocation,
	        HBufC* aValue,
	        TBool aPhoneNumber,
            const TDesC* aLabel);
        
        void CBCTContactWrapper::AddDataFieldsL(
            TPbkFieldId aFieldId,
	        const TDesC*,
            const TDesC* aLabel,
            const TPbkFieldLocation aLocation = EPbkFieldLocationNone);
        
            TInt FindFieldMaximumLength(TPbkFieldId aFieldId);
            void SetThumbnailValueL(const HBufC* aImageFileName);

		//From CBCTThumbnailHandler
		TBool SaveImageFileToContactL(CPbkContactItem& aItem, const TDesC& aFilename);
		void CBCTContactWrapper::SaveEmbeddedImageToContactL(
	        CPbkFieldInfo& aFieldInfo,
	        CPbkContactItem& aItem, 
	        const TDesC& aFilename);
		HBufC8* ReadImageFileToDescL(const TDesC& aFilename);

    private:    // Data
        CPbkContactItem& iContact;
        CPbkContactEngine& iContactEngine;
        const TInt iForVCard;
    };

#endif // __BCTContactWrapper_H__
            
// End of File
