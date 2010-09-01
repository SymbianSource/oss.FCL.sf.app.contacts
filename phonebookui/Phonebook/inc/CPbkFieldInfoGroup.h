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
* Description: 
*    Phonebook field info group.
*
*/


#ifndef __CPbkFieldInfoGroup_H__
#define __CPbkFieldInfoGroup_H__

//  INCLUDES
#include "PbkIconId.hrh"    // TPbkIconId
#include "PbkFields.hrh"    // TPbkFieldGroupId

// FORWARD DECLARATIONS
class CPbkFieldInfo;
class TResourceReader;

// CLASS DECLARATION

/**
 * Phonebook field info group.
 */
class CPbkFieldInfoGroup : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
         * @internal
         * Creates a new instance of this class and intializes it
		 * from resources.
		 * @param aResReader resource reader associated with resource file
         */
        static CPbkFieldInfoGroup* NewLC(TResourceReader& aReader);

        /**
         * @internal
         * Destructor.
         */
        ~CPbkFieldInfoGroup();
        
    public:  // Interface
        /**
         * Returns this group's ID.
         */
        IMPORT_C TPbkFieldGroupId Id() const;

        /**
         * Returns the label of this field info group.
         */
        IMPORT_C const TDesC& Label() const;

        /**
         * Returns the icon id of this field info group.
         */
        IMPORT_C TPbkIconId IconId() const;

        /**
         * Returns the number of field infos in this group.
         */
        IMPORT_C TInt Count() const;

        /**
         * Returns the aIndex:th field info in this group.
         */
        IMPORT_C const CPbkFieldInfo& At(TInt aIndex) const;

        /**
         * Returns the UI ordering for adding this group.
         */
        IMPORT_C TPbkAddItemOrdering AddItemOrdering() const;

        /**
         * @internal
         * Adds a new field info to this group. Only to be called by PbkEng
         * internally.
		 * @param aFieldInfo the field to add
         */
        void AddL(const CPbkFieldInfo& aFieldInfo);

    private:  // Implementation
        CPbkFieldInfoGroup();
        void ConstructL(TResourceReader& aReader);

    private:  // Data
		/// Own: group id
        TPbkFieldGroupId iId;
		/// Own: label
        HBufC* iLabel;
		/// Own: icon id
        TPbkIconId iIconId;
		/// Own: ordering object
        TPbkAddItemOrdering iAddItemOrdering;
		/// Own: array of field info objects
        RPointerArray<const CPbkFieldInfo> iFieldInfos;
        
    private: // Friend declarations
        friend class PbkFieldInfoGroupWrite;
    };


#endif // __CPbkFieldInfoGroup_H__

// End of File
