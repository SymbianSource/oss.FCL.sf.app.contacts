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
*     Chinese contact name formatter.
*
*/


#ifndef CPBK2CHINESECONTACTNAMEFORMATTER_H
#define CPBK2CHINESECONTACTNAMEFORMATTER_H

// INCLUDE FILES
#include "CPbk2ContactNameFormatter.h"

/**
 * Chinese contact name formatter.
 *
 * Name creation logic:
 *  1)  Try to create name according to first two field types in the
 *      sort order.
 *  2)  If the contact didn't have neither of the two fields
 *      then create name according to the third -> last type in the
 *      sort order.
 *  3)  If the field type is defined in the sort order but not defined
 *      as a title field type then the field data is not used.
 */
NONSHARABLE_CLASS(CPbk2ChineseContactNameFormatter) 
    :   public CPbk2ContactNameFormatter
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
        static CPbk2ChineseContactNameFormatter* NewL(
                const TDesC& aUnnamedText, 
                const MVPbkFieldTypeList& aMasterFieldTypeList, 
                const CPbk2SortOrderManager& aSortOrderManager,
                CVPbkFieldTypeSelector* aTitleFieldSelector );
                
        ~CPbk2ChineseContactNameFormatter();

    private: // From MPbk2ContactNameFormatter
        TBool IsFindSeparatorChar( TChar aCh );            

    private: // Implementation
        CPbk2ChineseContactNameFormatter(
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                const CPbk2SortOrderManager& aSortOrderManager );
    };

#endif // CPBK2CHINESECONTACTNAMEFORMATTER_H

// End of File
