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
* Description: 
*     Default contact name formatter.
*
*/


#ifndef CPBK2CONTACTNAMEFORMATTER_H
#define CPBK2CONTACTNAMEFORMATTER_H

// INCLUDE FILES
#include "CPbk2ContactNameFormatterBase.h"

/**
 * Default contact name formatter.
 *
 * Name creation logic:
 *  1)  Try to create name according to first two field types in the
 *      sort order.
 *  2)  If the contact didn't have neither of the two fields
 *      then create name according to the third -> last type in the
 *      sort order.
 *  3)  If the field type is defined in the sort order but not defined
 *      as a title field type then the field data is not used.
 *
 * An example:
 *      Sort Order: Last name, First Name, Company Name
 *      Title fields: Last name, First Name, Company Name
 *
 *      Name is created using Last name and First name
 *      If the both first and last name fields are empty 
 *      then title is created from the company name.
 */
NONSHARABLE_CLASS(CPbk2ContactNameFormatter)
    :   public CPbk2ContactNameFormatterBase
    {
    public: // Construction

        /**
        * @param aUnnamedText          Text to use for unnamed contacts.
        * @param aMasterFieldTypeList  Master field type list of
        *                              Virtual Phonebook.
        * @param aSortOrderManager     Sort order manager for fetching
        *                              current sort order.
        * @param aTitleFieldSelector   Title fields, ownership changes
        * @return a new instance of this class
        */
        static CPbk2ContactNameFormatter* NewL(
                const TDesC& aUnnamedText,
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                const CPbk2SortOrderManager& aSortOrderManager,
                CVPbkFieldTypeSelector* aTitleFieldSelector );

        ~CPbk2ContactNameFormatter();

    private: // From MPbk2ContactNameFormatter
        void GetContactTitle(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TDes& aTitle,
            TUint32 aFormattingFlags);
        void GetContactTitleForFind(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TDes& aTitle,
            TUint32 aFormattingFlags);
        CVPbkBaseContactFieldTypeListIterator* ActualTitleFieldsLC(
            CVPbkFieldTypeRefsList& aFieldTypeList,
            const MVPbkBaseContactFieldCollection& aContactFields );
        TInt MaxTitleLength(
            const MVPbkBaseContactFieldCollection& aContactFields,
            const TUint32 aFormattingFlags);
        TInt MaxTitleLengthForFind(
            const MVPbkBaseContactFieldCollection& aContactFields,
            const TUint32 aFormattingFlags);
        TBool IsFindSeparatorChar( TChar aCh );
        
        TAny* ContactNameFormatterExtension( TUid aExtensionUid );
        
    public: // From MPbk2ContactNameFormatter2
       
        HBufC* GetContactTitleWithCompanyNameL(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TUint32 aFormattingFlags );
    private:
         
        void GetContactTitleWithCompanyName(
            const MVPbkBaseContactFieldCollection& aContactFields,
            TDes& aTitle, TUint32 aFormattingFlags);
        
        TInt MaxTitleLengthWithCompanyName(
                    const MVPbkBaseContactFieldCollection& aContactFields,
                    const TUint32 aFormattingFlags );
      
        TBool IsCompanyNameField();

    protected: // Implementation
        CPbk2ContactNameFormatter(
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            const CPbk2SortOrderManager& aSortOrderManager);
        void ConstructL(
            const TDesC& aUnnamedText,
            CVPbkFieldTypeSelector* aTitleFieldSelector );
    };

#endif // CPBK2CONTACTNAMEFORMATTER_H

// End of File
