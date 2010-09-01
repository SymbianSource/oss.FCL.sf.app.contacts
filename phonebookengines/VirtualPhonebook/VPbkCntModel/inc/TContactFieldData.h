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


#ifndef VPBKCNTMODEL_TCONTACTFIELDDATA_H
#define VPBKCNTMODEL_TCONTACTFIELDDATA_H


// INCLUDES
#include <MVPbkContactFieldData.h>


// FORWARD DECLARATIONS
class CContactItemField;
/// Reserved name for a future extension to TContactFieldData
class TContactFieldDataExt;


// CLASS DECLARATIONS

namespace VPbkCntModel {

/**
 * Data layout class template for Contact Model field data -> 
 * MVPbkContactFieldData mapping.
 *
 * @param FieldData base class for this class. Must derive from 
 *                  MVPbkContactFieldData.
 */
template< class FieldData >
NONSHARABLE_CLASS( TContactFieldDataCommon ): public FieldData
    {
    protected:  // Constructor and destructor
        inline TContactFieldDataCommon();
        inline TContactFieldDataCommon
            (CContactItemField& aCntModelField);

    protected:  // Data
        CContactItemField* iCntModelField;
    };

/**
 * Base class for Contact Model field data -> MVPbkContactFieldData mapping.
 */
NONSHARABLE_CLASS( TContactFieldData ): 
        public TContactFieldDataCommon< MVPbkContactFieldData >
    {
    public:  // Constructor and destructor
        TContactFieldData();
        ~TContactFieldData();

    public:  // New functions
        /**
         * Sets the Contact Model field whose data to map.
         */
        void SetField(CContactItemField& aCntModelField);

        /**
         * Removes field mapping.
         */
        void ResetField();

    private:  // from MVPbkContactFieldData
        /// Private to make static call attempts fail
        TVPbkFieldStorageType DataType() const;
        /// Private to make static call attempts fail
        TBool IsEmpty() const;
        /// Private to make static call attempts fail
        void CopyL(const MVPbkContactFieldData& aFieldData);

    private: // Hidden functions
        /// Hidden copy constructor
        TContactFieldData(const TContactFieldData&);
        /// Hidden assignment operator
        TContactFieldData& operator=(const TContactFieldData&);
    };


// INLINE FUNCTIONS

template< class FieldData >
inline TContactFieldDataCommon<FieldData>::TContactFieldDataCommon() :
    iCntModelField(NULL)
    {
    }

template< class FieldData >
inline TContactFieldDataCommon<FieldData>::TContactFieldDataCommon
        (CContactItemField& aCntModelField) :
    iCntModelField(&aCntModelField)
    {
    }

}  // namespace VPbkCntModel

#endif  // VPBKCNTMODEL_TCONTACTFIELDDATA_H
//End of file


