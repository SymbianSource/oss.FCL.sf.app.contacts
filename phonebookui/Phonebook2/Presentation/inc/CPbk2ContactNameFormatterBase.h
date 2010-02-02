/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A base class for contact name formatters
*
*/



#ifndef CPBK2CONTACTNAMEFORMATTERBASE_H
#define CPBK2CONTACTNAMEFORMATTERBASE_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>

#include "TPbk2SortOrderFieldMapper.h"

// FORWARD DECLARATIONS
class CPbk2SortOrderManager;
class MVPbkFieldTypeList;
class CVPbkFieldTypeRefsList;
class CVPbkFieldTypeSelector;
class TResourceReader;


// CLASS DECLARATION

/**
*  A base class for contact name formatters
*
*/
NONSHARABLE_CLASS( CPbk2ContactNameFormatterBase )
        :   public CBase,
            public MPbk2ContactNameFormatter,
            public MPbk2ContactNameFormatter2
    {
    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CPbk2ContactNameFormatterBase();

    public: // Functions from MPbk2ContactNameFormatter

        HBufC* GetContactTitleOrNullL(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TUint32 aFormattingFlags);
        HBufC* GetContactTitleL(
           const MVPbkBaseContactFieldCollection& aContactFields,
           TUint32 aFormattingFlags);
        virtual void GetContactTitle(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TDes& aTitle,
            TUint32 aFormattingFlags) = 0;
        virtual void GetContactTitleForFind(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TDes& aTitle,
            TUint32 aFormattingFlags) = 0;

        const TDesC& UnnamedText() const;
        TBool IsTitleField(
            const MVPbkBaseContactField& aContactField) const;
        TBool IsTitleFieldType(
            const MVPbkFieldType& aFieldType) const;
        virtual CVPbkBaseContactFieldTypeListIterator* ActualTitleFieldsLC(
            CVPbkFieldTypeRefsList& aFieldTypeList,
            const MVPbkBaseContactFieldCollection& aContactFields ) = 0;
        virtual TInt MaxTitleLength(
            const MVPbkBaseContactFieldCollection& aContactFields,
            const TUint32 aFormattingFlags) = 0;
        virtual TInt MaxTitleLengthForFind(
            const MVPbkBaseContactFieldCollection& aContactFields,
            const TUint32 aFormattingFlags) = 0;
        virtual TBool IsFindSeparatorChar( TChar aCh ) = 0;
        
    public: // from MPbk2ContactNameFormatter2
        virtual HBufC* GetContactTitleWithCompanyNameL(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TUint32 aFormattingFlags ) = 0;
        


    protected:  // New functions

        /**
        * Field mapper returns fields from the given contact fields
        * in the order defined by iSortOrderManager. Must be called
        * from sub class functions
        * @param aContactFields current fields
        */
        void SetFieldMapper(
            const MVPbkBaseContactFieldCollection& aContactFields );

        /**
        * Get contact title according to given sort order positions
        *
        * @param aTitle a descriptor for adding data
        * @param aFormattingFlags flags from the client
        * @param aStartingPosition the index of the first type
        *           in the sort order that is used for title
        * @param aEndingPosition the index of the last type
        *           in the sort order that is used for title
        */
        void DoGetContactTitle(
            TDes& aTitle,
            TUint32 aFormattingFlags,
            const TInt aStartingPosition,
            const TInt aEndingPosition);

        /**
        * Returns the maximum length of the title
        *
        * @param aFormattingFlags flags from the client
        * @param aStartingPosition the index of the first type
        *           in the sort order that is used for title
        * @param aEndingPosition the index of the last type
        *           in the sort order that is used for title
        */
        TInt DoCalculateMaxTitleLength(
            const TUint32 aFormattingFlags,
            const TInt aStartingPosition,
            const TInt aEndingPosition);

        /**
        * Return non empty title field types
        *
        * @param aFieldTypeList target list where field types are appended
        * @param aStartingPosition the index of the first type
        *           in the sort order that is used for title
        * @param aEndingPosition the index of the last type
        *           in the sort order that is used for title
        */
        void DoAppendNonEmptyTitleFieldTypesL(
            CVPbkFieldTypeRefsList& aFieldTypeList,
            const TInt aStartingPosition,
            const TInt aEndingPosition);
        
        /**
        * Check if title fields are empty
        *
        * @param aStartingPosition the index of the first type
        *           in the sort order that is used for title
        * @param aEndingPosition the index of the last type
        *           in the sort order that is used for title
        * @return ETrue if title fields are empty else EFalse
        */    
        TBool AreTitleFieldsEmpty(
            const TInt aStartingPosition,
            const TInt aEndingPosition ) const;            

    protected:

        /**
        * C++ default constructor.
        */
        CPbk2ContactNameFormatterBase(
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            const CPbk2SortOrderManager& aSortOrderManager);

        /**
        * By default Symbian 2nd phase constructor is protected.
        */
        void BaseConstructL( const TDesC& aUnnamedText,
            CVPbkFieldTypeSelector* aTitleFieldSelector );


    private:    // Implementation
        TBool RequiresSeparatorBetweenNames(
            const TDesC& aLhs,
            const TDesC& aRhs) const;
        void TakePartOfName(
            const MVPbkBaseContactField* aField,
            TDes& aTitle,
            const TUint32 aFormattingFlags,
            const TBool aUseSeparator) const;
        void CalculatePartOfName(
            const MVPbkBaseContactField* aField,
            TInt& aLength,
            const TUint32 aFormattingFlags,
            const TBool aUseSeparator) const;

    protected: // Data
        /// Own: Field mapper
        TPbk2SortOrderFieldMapper iFieldMapper;

    private:    // Data
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Ref: Sort order manager
        const CPbk2SortOrderManager& iSortOrderManager;
        /// Own: Text for unnamed contacts
        HBufC* iUnnamedText;
        /// Own: Title field selector
        CVPbkFieldTypeSelector* iTitleFieldSelector;
    };

#endif      // CPBK2CONTACTNAMEFORMATTERBASE_H

// End of File
