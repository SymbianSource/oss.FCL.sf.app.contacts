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
* Description:  Phonebook 2 field list box model.
*
*/


#ifndef CPBK2FIELDLISTBOXMODEL_H
#define CPBK2FIELDLISTBOXMODEL_H

//  INCLUDES
#include <e32base.h>
#include <TPbk2AppIconId.h>
#include <bamdesca.h>
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class CPbk2IconArray;
class CDesC16Array;
class MPbk2ClipListBoxText;
class MPbk2FieldAnalyzer;
class CPbk2FieldFormatter;
class CVPbkContactManager;
class CPbk2PresentationContactField;
class CPbk2PresentationContactFieldCollection;
class MPbk2FieldPropertyArray;
class MVPbkFieldType;
class CPbk2ContactFieldDynamicProperties;
//class CPbk2PresenceIconInfo;


// CLASS DECLARATION

/**
 * Phonebook 2 list box model row.
 *
 * @internal Only Phonebook 2 internal use supported!
 */
NONSHARABLE_CLASS(CPbk2FieldListBoxRow) : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2FieldListBoxRow* NewL();

        /**
         * Destructor.
         */
        ~CPbk2FieldListBoxRow();

    public: // Interface

        /**
         * Returns the number of columns in this row.
         *
         * @return  Number of columns in this row.
         */
        TInt ColumnCount() const;

        /**
         * Returns the descriptor at the given position.
         *
         * @param aColumnIndex  Column index.
         * @return  The descriptor at the given index.
         */
        TPtrC At(
                TInt aColumnIndex ) const;

        /**
         * Appends a given column to the row.
         *
         * @param aColumnText   Column to append.
         */
        void AppendColumnL(
                const TDesC& aColumnText );

        /**
         * Returns the total descriptor length of all the columns.
         *
         * @return  Total row length.
         */
        TInt TotalLength() const;

        /**
         * Returns the maximum column length of this row.
         *
         * @return  Maximum column length.
         */
        TInt MaxColumnLength() const;

    private: // Implementation
        CPbk2FieldListBoxRow();

    private: // Data
        /// Own: Column array
        RPointerArray<HBufC> iColumns;
    };

/**
 * Phonebook 2 field list box model.
 *
 * @internal Only Phonebook 2 internal use supported!
 */
NONSHARABLE_CLASS(CPbk2FieldListBoxModel) : public CBase,
                                            public MDesCArray
    {
    public: // Types

        /**
         * Column indexes.
         */
        enum TColumnIndex
            {
            /// Icon column
            EIconColumn = 0,
            /// Label column
            ELabelColumn,
            /// Content column
            EContentColumn
            };

    public: // Data structures
        
        // structure to keep presence icon position in the common icon array 
        struct TPresenceIconPosInIconArray
            {
            // position of the icon in the common icon array
            TPbk2IconId iIconId;
            // service name
            const TDesC16& iServiceName; 
            TPresenceIconPosInIconArray( const TDesC16& aServiceName ):
                iServiceName ( aServiceName )
                {
                }
            };
        
        /**
         * Construction parameters for CPbk2FieldListBoxModel.
         */
        class TParams
            {
            public: // Construction
                /**
                 * Constructor.
                 *
                 * @param aFieldCollection      Array of fields to display.
                 * @param aContactManager       Virtual Phonebook
                 *                              contact manager.
                 * @param aFieldProperties      Field properties.
                 * @param aTimeFormat           Time formatting string
                 *                              to use.
                 * @param aIconArray            Icon array to use.
                 * @param aFieldAnalyzer        Interface for
                 *                              analyzing fields.
                 * @param aDynamicProperties    Dynamic field properties.
	             * @param aCommMethod		Communication method type selected
                 */
                TParams (
                        const CPbk2PresentationContactFieldCollection&
                            aFieldCollection,
                        const CVPbkContactManager& aContactManager,
                        const MPbk2FieldPropertyArray& aFieldProperties,
                        const TDesC& aTimeFormat,
                        const CPbk2IconArray& aIconArray,
                        const MPbk2FieldAnalyzer* aFieldAnalyzer,
                        const CPbk2ContactFieldDynamicProperties* aDynamicProperties,
			            const TBool aFieldContentNeedToBeDuplicated,
						VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod = VPbkFieldTypeSelectorFactory::EEmptySelector) :
                    	    iFields( aFieldCollection ),
                    	    iContactManager( aContactManager ),
                    	    iFieldProperties( aFieldProperties ),
                    	    iTimeFormat( aTimeFormat ),
                    	    iIconArray( aIconArray ),
                    	    iDefaultIconId( EPbk2qgn_prop_nrtyp_empty ),
                    	    iFieldAnalyzer( aFieldAnalyzer ),
                    	    iDynamicProperties( aDynamicProperties ),
			                iFieldContentNeedToBeDuplicated( aFieldContentNeedToBeDuplicated ),
							iCommMethod( aCommMethod )
	            {
	            }

            public: // Data
                /// Ref: Array of fields to display
                const CPbk2PresentationContactFieldCollection& iFields;
                /// Ref: Virtual Phonebook contact manager
                const CVPbkContactManager& iContactManager;
                /// Ref: Field properties
                const MPbk2FieldPropertyArray& iFieldProperties;
                /// Ref: Time formatting string
                const TDesC& iTimeFormat;
                /// Ref: Icon array
                const CPbk2IconArray& iIconArray;
                /// Own: Default icon id
                TPbk2AppIconId iDefaultIconId;
                /// Ref: Interface for analyzing field
                const MPbk2FieldAnalyzer* iFieldAnalyzer;
                /// Own: Dynamic properties for contact fields
                const CPbk2ContactFieldDynamicProperties* iDynamicProperties;
	            /// Own: The flag indicates the field content need to be duplicated
                const TBool iFieldContentNeedToBeDuplicated;
				/// Own: Communication method type selected
				VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iCommMethod;
            };

    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   List box model parameters.
         * @param aFieldCollectionArray  Array of store contact fields arrays.
         * @return  A new instance of this class.
         */
        static CPbk2FieldListBoxModel* NewL(
            TParams& aParams,
            const TArray<CPbk2PresentationContactFieldCollection*>*
                aFieldCollectionArray = NULL,
            const TArray<TPresenceIconPosInIconArray>* aPresenceIconsPos = NULL );

        /**
         * Destructor.
         */
        ~CPbk2FieldListBoxModel();

    public: // Interface

        /**
         * Appends a given row to the end of the model.
         *
         * @param aLine     The row to append.
         */
        void AppendRowL(
                CPbk2FieldListBoxRow* aLine );

        /**
         * Sets the text clipper to given clipper.
         *
         * @param aTextClipper  The clipper to set.
         */
        void SetClipper(
                MPbk2ClipListBoxText& aTextClipper );
                
        /**
         * Formats fields. Needs to be call after construction.
         */                
        void FormatFieldsL();
        
        /**
         * Returns the index of the listbox selected item from the model
         * point of view, i.e. taking into account duplicates.
         *
         * @param aListboxIndex Listbox selected item index
         * @return Model index of the selected item
         */    
        TInt GetModelIndex( TInt aListboxIndex );
        
    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(
                TInt aIndex ) const;

    private: // Implementation
        CPbk2FieldListBoxModel(
                TParams& aParams,
                const TArray<CPbk2PresentationContactFieldCollection*>*
                    aFieldCollectionArray,
                const TArray<TPresenceIconPosInIconArray>* aPresenceIconsPos );
        void ConstructL(
                TParams& aParams );
        void FormatFieldL(
                const CPbk2PresentationContactField& aField,
                TParams& aParams );
        void FormatAddressFieldL(
                const CPbk2PresentationContactField& aField,
                TParams& aParams );
        void ExpandBuffersL(
                const CPbk2FieldListBoxRow& aRow );
        TPtr ExpandColumnBufferL(
                TInt aRequiredLength );
        void ClipText(
                const CPbk2PresentationContactField& aField,
                TInt aIndex,
                TPtr& aColumnText,
                TInt aColumnIndex ) const;
        void AppendFieldLabelL(
                const CPbk2PresentationContactField& aField,
                CPbk2FieldListBoxRow& aRow,
                TPtr& aColumnBuf );
        void AppendAddressFieldLabelL(
                const CPbk2PresentationContactField& aField,
                CPbk2FieldListBoxRow& aRow,
                TPtr& aColumnBuf );
        void AppendFieldContentL(
                const CPbk2PresentationContactField& aField,
                const MVPbkFieldType& aFieldType,
                CPbk2FieldListBoxRow& aRow,
                TPtr& aColumnBuf );
        void AppendAddressFieldContentL(
                const CPbk2PresentationContactField& aField,
                CPbk2FieldListBoxRow& aRow,
                TPtr& aColumnBuf );
        void AppendIconsToBeginningL(
                const CPbk2PresentationContactField& aField,
                const MVPbkFieldType& aFieldType,
                CPbk2FieldListBoxRow& aRow,
                TParams& aParams );
        void AppendAddressIconsToBeginningL(
        		const CPbk2PresentationContactField& aField,
                CPbk2FieldListBoxRow& aRow,
                TParams& aParams );
        void AppendIconsToEndL(
                const CPbk2PresentationContactField& aField,
                CPbk2FieldListBoxRow& aRow,
                TParams& aParams );
        TBool DuplicatesExist( TPtrC aContent );
        TBool DuplicatesAddressExist( TPtrC aContent, TInt aIndex );
        TBool IsGeoFieldForAddressL(
                const CPbk2PresentationContactField& aField );

    private: // Data
        /// Own: Text row array
        CArrayPtrFlat<CPbk2FieldListBoxRow> iRows;
        /// Own: Buffer storing one formatted listbox row
        HBufC* iLineBuf;
        /// Own: Buffer for storing one formatted listbox column
        HBufC* iColumnBuf;
        /// Own: Field content formatter
        CPbk2FieldFormatter* iFieldFormatter;
        /// Ref: Object for clipping text
        MPbk2ClipListBoxText* iTextClipper;
        /// Own: Dynamic field properties
        const CPbk2ContactFieldDynamicProperties* iDynamicProperties;
        /// Own: Parameters
        TParams iParams;
        /// Ref: Field collection array for additional contacts
        const TArray<CPbk2PresentationContactFieldCollection*>*
                iFieldCollectionArray;
        /// Own: array of duplicates in the rows
        RArray<TInt> iDuplicatesArray;
        /// Ref: Presence icons pos in common icons array
        const TArray<TPresenceIconPosInIconArray>* iPresenceIconsPos;
    };

#endif // CPBK2FIELDLISTBOXMODEL_H

// End of File
