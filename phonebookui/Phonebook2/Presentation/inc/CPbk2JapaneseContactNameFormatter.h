/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Japanese contact name formatter.
*
*/


#ifndef CPBK2JAPANESECONTACTNAMEFORMATTER_H
#define CPBK2JAPANESECONTACTNAMEFORMATTER_H

// INCLUDE FILES
#include "CPbk2ContactNameFormatterBase.h"

/**
 * Japanese contact name formatter.
 *
 * Name creation logic:
 *  1)  Try to create name using last and first name.
 *  2)  If the contact didn't have neither of the two fields
 *      then use last/first name reading to build title.
 *      Sort order defines the order of last/first name
 *  3)  If the title is still empty then create name
 *      accoriding to the third -> last type in the sort order.
 *  3)  If the field type is defined in the sort order but not defined
 *      as a title field type then the field data is not used.
 *
 */
NONSHARABLE_CLASS(CPbk2JapaneseContactNameFormatter)
    :   public CPbk2ContactNameFormatterBase
    {
    public: // Construction

        /**        
        * @param aMasterFieldTypeList  Master field type list of
        *                              Virtual Phonebook.
        * @param aSortOrderManager     Sort order manager for fetching
        *                              current sort order.
        * @param aTitleFieldSelector   Title fields, ownership changes
        * @param aUnnamedText          Text to use for unnamed contacts.
        * @return a new instance of this class
        */
        static CPbk2JapaneseContactNameFormatter* NewL(
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                const CPbk2SortOrderManager& aSortOrderManager,
                CVPbkFieldTypeSelector* aTitleFieldSelector,
                const TDesC& aUnnamedText );

        ~CPbk2JapaneseContactNameFormatter();

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
        
    public: // From MPbk2ContactNameFormatter2
        
         HBufC* GetContactTitleWithCompanyNameL(
             const MVPbkBaseContactFieldCollection& /*aContactFields*/,
             TUint32 /*aFormattingFlags*/ )
             {
             return NULL;
             }

    private: // Implementation
        CPbk2JapaneseContactNameFormatter(
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            const CPbk2SortOrderManager& aSortOrderManager);
        void ConstructL(
            const TDesC& aUnnamedText,
            CVPbkFieldTypeSelector* aTitleFieldSelector );
    };

#endif // CPBK2JAPANESECONTACTNAMEFORMATTER_H

// End of File
