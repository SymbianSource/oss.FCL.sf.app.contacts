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
* Description:  The virtual phonebook field factory
*
*/



#ifndef CFIELDFACTORY_H
#define CFIELDFACTORY_H


// INCLUDES
#include <e32base.h>
#include <MVPbkFieldType.h>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class CContactItem;
class CContactItemField;
class TVPbkFieldVersitProperty;
class RFs;

namespace VPbkCntModel {


// FORWARD DECLARATIONS
class CFieldTypeMap;

// CLASS DECLARATIONS

/**
 * Virtual Phonebook field type -> Contact Model field factory.
 */
NONSHARABLE_CLASS(CFieldFactory) : public CBase,
                                   public MVPbkFieldTypeList
    {
    public:  // Constructor and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aFieldTypeMap     field type mapper.
         * @param aSystemTemplate   Contact Database system template. This
         *                          object takes ownership of the system
         *                          template unless this function leaves.
         * @param aMasterFieldTypeList  Master field type list from CVPbkContactManager.
         * @return a new instance of this class.
         */
        static CFieldFactory* NewL( const CFieldTypeMap& aFieldTypeMap, 
                                   CContactItem* aSystemTemplate, 
                                   const MVPbkFieldTypeList& aMasterFieldTypeList,
                                   RFs& aFs );

        /**
         * Destructor.
         */
        ~CFieldFactory();

    public:  // New functions
        /**
         * Creates and returns a new Contact Model field based on Virtual 
         * Phonebook generic field type. 
         *
         * A field definition that matches the field type is searched from the
         * system template that was passed as a construction parameter to this
         * object.
         *
         * @param aFieldType    type of the field to create.  
         * @return  a new Contact Model field. Caller takes ownership of the 
         *          returned object. May return NULL if the field type is not
         *          supported by Contact Model.
         */
        CContactItemField* CreateFieldLC( const MVPbkFieldType& aFieldType ) const;

        /**
         * Finds and returns matching Contact Model system template field base on 
         * Virtual Phonebook generic field type.
         *
         * @param aFieldType    type of the field to match.
         * @return Matching Contact Model field if found, NULL otherwise.
         */
        const CContactItemField* FindField( const MVPbkFieldType& aFieldType ) const;

    public: // From MVPbkFieldTypeList
        TInt FieldTypeCount() const;
        const MVPbkFieldType& FieldTypeAt( TInt aIndex ) const;
        TBool ContainsSame( const MVPbkFieldType& aFieldType ) const;
        TInt MaxMatchPriority() const;
        const MVPbkFieldType* FindMatch(
            const TVPbkFieldVersitProperty& aMatchProperty,
            TInt aMatchPriority) const;
        const MVPbkFieldType* FindMatch( 
            TVPbkNonVersitFieldType aNonVersitType ) const;
        const MVPbkFieldType* Find( TInt aFieldTypeResId ) const;

    private: // Data types
        class TFieldAndTypeTuple;
        
    private:  // Constructors
        CFieldFactory();
        void ConstructL(const CFieldTypeMap& aFieldTypeMap, 
            const CContactItem& aSystemTemplate,
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            RFs& aFs );
        void InsertInMasterFieldTypeOrderL( TFieldAndTypeTuple& aMapping,
            const MVPbkFieldTypeList& aMasterFieldTypeList );
        TInt IndexOfTypeInMasterList( const MVPbkFieldType& aType,
            const MVPbkFieldTypeList& aMasterFieldTypeList );
        void DoInsertMappingTupleL(
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            const CFieldTypeMap& aFieldTypeMap,
            const CContactItemField& field);

    private:  // Data
        //Own: field and type mapping array
        RArray<TFieldAndTypeTuple> iFieldAndTypeMapping;
        ///Own: System template instance. Owned by this object.
        CContactItem* iSystemTemplate;
        ///Own: max match priority
        TInt iMaxMatchPriority;
        ///Own: non system template fields
        RPointerArray<const CContactItemField> iNonSystemTemplateFields;
    };

}  // namespace VPbkCntModel

#endif  // CFIELDFACTORY_H

//End of file
