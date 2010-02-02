/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef VPBKCNTMODEL_TCONTACTFIELDDATAIMPL_H
#define VPBKCNTMODEL_TCONTACTFIELDDATAIMPL_H


// INCLUDES
#include "TContactFieldData.h"
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldUriData.h>


// FORWARD DECLARATIONS


// CLASS DECLARATIONS

namespace VPbkCntModel {

/**
 * CContactTextField -> MVPbkContactFieldStringData mapping.
 */
NONSHARABLE_CLASS( TContactFieldTextData ): 
        public TContactFieldDataCommon< MVPbkContactFieldTextData >
    {
    public:  // Constructor and destructor
        TContactFieldTextData(CContactItemField& aCntModelField);

    public:  // from MVPbkContactFieldData
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);

    public:  // from MVPbkContactFieldTextData
        TPtrC Text() const;
        void SetTextL(const TDesC& aText);
        TInt MaxLength() const;
    };

/**
 * CContactStoreField -> MVPbkContactFieldBinaryData mapping.
 */
NONSHARABLE_CLASS( TContactFieldBinaryData ): 
        public TContactFieldDataCommon< MVPbkContactFieldBinaryData >
    {
    public:  // Constructor and destructor
        TContactFieldBinaryData(CContactItemField& aCntModelField);

    public:  // from MVPbkContactFieldData
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);

    public:  // from MVPbkContactFieldBinaryData
        TPtrC8 BinaryData() const;
        void SetBinaryDataL(const TDesC8& aBinaryData);
    };

/**
 * CContactDateField -> MVPbkContactFieldDateTimeData mapping.
 */
NONSHARABLE_CLASS( TContactFieldDateTimeData ): 
        public TContactFieldDataCommon< MVPbkContactFieldDateTimeData >
    {
    public:  // Constructor and destructor
        TContactFieldDateTimeData(CContactItemField& aCntModelField);

    public:  // from MVPbkContactFieldData
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);

    public:  // from MVPbkContactFieldDateTimeData
        TTime DateTime() const;
        void SetDateTime(const TTime& aTime);
    };

/**
 * CContactTextField -> MVPbkContactFieldUriData mapping for IMPP field data type.
 */
NONSHARABLE_CLASS( TContactFieldUriData ): 
        public TContactFieldDataCommon< MVPbkContactFieldUriData >
    {
    public:  // Constructor and destructor
        TContactFieldUriData(CContactItemField& aCntModelField);

    public:  // from MVPbkContactFieldData
        TBool IsEmpty() const;
        void CopyL(const MVPbkContactFieldData& aFieldData);

    public:  // from MVPbkContactFieldUriData
        virtual TPtrC Scheme() const;
        virtual TPtrC Text() const;
        virtual TPtrC Uri() const;
        virtual void SetUriL(const TDesC& aUri);
        virtual void SetUriL(const TDesC& aScheme, const TDesC& aText);
        virtual TInt MaxLength() const;

    private:
        TBool Validate(const TDesC& aUri, TBool aFullUri);
    };


// INLINE FUNCTIONS

// TContactFieldStringData
inline TContactFieldTextData::TContactFieldTextData
        (CContactItemField& aCntModelField) :
    TContactFieldDataCommon<MVPbkContactFieldTextData>(aCntModelField)
    {
    }

// TContactFieldBinaryData
inline TContactFieldBinaryData::TContactFieldBinaryData
        (CContactItemField& aCntModelField) :
    TContactFieldDataCommon<MVPbkContactFieldBinaryData>(aCntModelField)
    {
    }

// TContactFieldDateTimeData
inline TContactFieldDateTimeData::TContactFieldDateTimeData
        (CContactItemField& aCntModelField) :
    TContactFieldDataCommon< MVPbkContactFieldDateTimeData >(aCntModelField)
    {
    }

inline TContactFieldUriData::TContactFieldUriData
        (CContactItemField& aCntModelField) :
    TContactFieldDataCommon< MVPbkContactFieldUriData >(aCntModelField)
    {
    }


}  // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_TCONTACTFIELDDATAIMPL_H

