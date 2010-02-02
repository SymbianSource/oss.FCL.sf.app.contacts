/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A SIM field type filter.
*
*/


#ifndef CVPBKSIMFIELDTYPEFILTER_H
#define CVPBKSIMFIELDTYPEFILTER_H

// INCLUDES
#include <e32base.h>
#include <VPbkSimCntFieldTypes.hrh>

//  CLASS DECLARATION

/**
 * A SIM field type filter.
 */
class CVPbkSimFieldTypeFilter : public CBase
	{
    public: // Data types
        /**
         * Filtering flags.
         */
        enum TSimContactViewFilteringCriteria
            {
            /// Include all contacts
            ESimFilterCriteriaNoFiltering           = 0x0000,
            /// Include contacts with primary number
            ESimFilterCriteriaGsmNumber             = 0x0001,
            /// Include contacts with additional number
            ESimFilterCriteriaAdditionalNumber      = 0x0002,
            /// Include contacts with email address
            ESimFilterCriteriaEmailAddress          = 0x0004,
            /// Include contacts with name field
            ESimFilterCriteriaName                  = 0x0008,
            /// Include contacts with second name
            /// (nick name or Japanese reading field)
            ESimFilterCriteriaSecondName            = 0x0010,
            /// Does not match SIM field types
            ESimFilterCriteriaFilterOut             = 0x1000
            };

	public: // Construction and destruction
        /**
         * Constructor.
         */
		IMPORT_C CVPbkSimFieldTypeFilter();

        /**
         * Destructor.
         */
        ~CVPbkSimFieldTypeFilter();

    public: // Interface
        /**
         * Appends filtering criteria with a filtering flag.
         *
         * @param aFilteringFlag    Filtering flag to append the criteria.
         */
        IMPORT_C void AppendFilteringFlag(
                TUint16 aFilteringFlag );

        /**
         * Takes a SIM field type and returns corresponding
         * filtering flag.
         *
         * @param aSimFieldType     SIM field type.
         * @return  Filtering flag.
         */
        IMPORT_C TUint16 FilteringFlagForSimFieldType(
                TVPbkSimCntFieldType aSimFieldType ) const;

        /**
         * Returns current filtering criteria.
         *
         * @return  Current filtering criteria.
         */
        IMPORT_C TUint16 FilteringCriteria() const;

        /**
         * Outputs this objects state to a buffer.
         *
         * @return Buffer containg this object.
         */
        IMPORT_C HBufC8* ExternalizeLC() const;
        
        /**
         * Initializes the instance from the buffer.
         *
         * @param aBuffer Buffer containing externalized object.
         */
        IMPORT_C void InternalizeL(
                const TDesC8& aBuffer );

    private: // Data
        /// Own: Filtering criteria
        TUint16 iFilteringCriteria;
    };

#endif // CVPBKSIMFIELDTYPEFILTER_H

// End of File
