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
* Description:  Phonebook 2 contact find policy.
*
*/


#ifndef CPBK2CONTACTFINDPOLICY_H
#define CPBK2CONTACTFINDPOLICY_H

// INCLUDES
#include <e32base.h>
#include <CVPbkContactFindPolicy.h>
#include <bamdesca.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CFindUtil;
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
class CVPbkFieldTypeRefsList;

// CLASS DECLARATION

/**
 * Phonebook 2 contact find policy.
 * Responsible for implementing Virtual Phonebook defined contact
 * find policy so that the find is done according to the rules specified
 * in the UI level specifications.
 */
class CPbk2ContactFindPolicy : public CVPbkContactFindPolicy
    {
    public: // Constructors and destructor
 
        /**
         * Two-phased constructor.
         * @param aParam Find policy parameter
         * @return  A new instance of this class.
         */
        static CPbk2ContactFindPolicy* NewL( TParam* aParam );

        /**
         * Destructor.
         */
        ~CPbk2ContactFindPolicy();

    public: // From CVPbkContactFindPolicy
        TBool Match( const TDesC& aText,
                     const TDesC& aSearchString );
        TBool MatchRefineL( const TDesC& aText,
                            const TDesC &aSearchString );
        TBool MatchContactNameL( const MDesCArray& aFindWords,
                                 const MVPbkBaseContact& aContact );
        TBool MatchChineseContactNameL( const MDesCArray& aFindWords,
                                 const MVPbkBaseContact& aContact );
        TBool IsWordValidForMatching( const TDesC& aWord );

    private: // Implementation
        CPbk2ContactFindPolicy();
        void ConstructL( TParam* aParam );
        void SplitContactFieldTextIntoArrayL(
        		CDesCArray* aArray,
                const TDesC& aText );
        MDesCArray* SplitContactFieldTextIntoArrayL( const TDesC& aText );
        
        /**
         * If FDN contact, no name, use phonenumber as title if exists.
         * @aContact a contact with no name
         * @aAllNamesArray an array to contain name.
         */
        void GetTitleFromFDNNumberL( const MVPbkBaseContact& aContact, CDesC16Array& aAllNamesArray );

    private: // Data
        /// Own: Find util
        CFindUtil* iFindUtil;
        /// Own: Sort order manager
        CPbk2SortOrderManager* iSortOrderManager;
        /// Own: Contact name formatter
        MPbk2ContactNameFormatter* iNameFormatter;
        /// Own: a field type list for name formatter
        CVPbkFieldTypeRefsList* iFieldTypeRefsList;
    };

#endif // CPBK2CONTACTFINDPOLICY_H

// End of File
