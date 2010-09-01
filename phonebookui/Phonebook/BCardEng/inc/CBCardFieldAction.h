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
*      A class to contain functions to handle contact field action.
*
*/


#ifndef __CBCARDFIELDACTION_H__
#define __CBCARDFIELDACTION_H__

// INCLUDES
#include <badesca.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class CParserProperty;
class CPbkAttachmentFile;
class CPbkFieldInfo;
class CParserPropertyValue;
class CBCardParserParamArray;
class CBCardFieldUidMapping;
class TPbkContactItemField;
class TPbkMatchPriorityLevel;

// CLASS DECLARATION
NONSHARABLE_CLASS(CBCardEngine::CBCardFieldAction) : public CBase
	{
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine phonebook engine reference
		 * @param aNameUidMap name UID map reference
		 * @param aPictureFileName picture file name
		 * @param aThumbnailFileName thumbnail file name
         */
        static CBCardFieldAction* NewL(
            CPbkContactEngine& aEngine,
            CBCardFieldUidMapping& aNameUidMap,
            HBufC& aPictureFileName,
            HBufC& aThumbnailFileName);
        
        /**
         * Destructor.
         */
        ~CBCardFieldAction();

    public: // Interface
        /**
         * Adds a field.
         *
         * @param aDestItem target item
		 * @param aProperty the property
		 * @param aPriorityLevel the priority
		 * @return ETrue if success
         */
        TBool AddFieldL(
            CPbkContactItem& aDestItem,
            CParserProperty& aProperty,
            TPbkMatchPriorityLevel& aPriorityLevel);

        /**
         * Adds a compact card field.
         *
         * @param aDestItem target item
		 * @param aFieldId the field id
		 * @param aValue the value
		 * @param aPhoneNumber phonenumber field flag
		 * @param aLabel the label
		 * @return ETrue if success
         */
        TBool AddCompactCardFieldL(
	        CPbkContactItem& aDestItem,
	        TPbkFieldId aFieldId,
	        const TDesC& aValue,
	        TBool aPhoneNumber = EFalse,
            const TDesC* aLabel=NULL);

        /**
         * Gets additional parameters.
         * @param aField the field
		 * @param aParameters the parameters
		 * @return not used
         */
        TBool GetAdditionalParametersL(
            TPbkContactItemField& aField,
            CBCardParserParamArray& aParameters,
            CPbkContactItem& aItem);

        /**
         * Exports a contact field.
         * @param aExportProperty export property
		 * @param aParser vCard parser
		 * @param aSourceItem the source item
		 * @param aField the field to export
         */
        void ExportContactFieldL(
            const CBCardExportProperty& aExportProperty,
            CParserVCard& aParser, 
            CPbkContactItem& aSourceItem,
            TPbkContactItemField& aField);

        /**
         * Exports an array of contact fields.
		 * @param aParser vCard parser
		 * @param aSourceItem the source item
		 * @param aField the field to export
         * @param aExportProperty export property
         */
        void ExportArrayOfFieldsL(
            CParserVCard& aParser, 
            CPbkContactItem& aSourceItem, 
            const CBCardExportProperty& aExportProperty);

        /**
         * Maps image type to a string.
		 * @param aType image type enumeration
		 * @return image description string
         */
        static const TDesC& MapImageTypeToString(const TInt aType);

    private: // Constructors
        CBCardFieldAction(
            CPbkContactEngine& aEngine,
            CBCardFieldUidMapping& aNameUidMap,
            HBufC& aPictureFileName,
            HBufC& aThumbnailFileName);

    private: // Implementation 
		TInt FindFieldMaximumLength(TPbkFieldId aFieldId);

        TBool AddFieldWithValueL(
	            CPbkContactItem& aDestItem,
	            CPbkFieldInfo& aFieldInfo,
	            const TDesC &aValue,
	            TBool aPhoneNumber=EFalse,
                const TDesC* aLabel=NULL);

        CParserPropertyValue* GetExportPropertyValueL(
            CPbkContactItem& aSourceItem,
            TPbkContactItemField& aField,
            const TPbkVersitStorageType& aType,
            CBCardParserParamArray& aParameters);
        
        TBool AddArrayFieldsL(
            CPbkContactItem& aDestItem,
            CParserProperty& aProperty,
            TPbkMatchPriorityLevel& aPriorityLevel);
        
        TBool AddTextFieldL(
            CPbkContactItem& aDestItem,
            CPbkFieldInfo& aFieldInfo,
            CParserProperty& aProperty);

        TBool AddBinaryFieldL(
            CPbkContactItem& aDestItem,
            CParserProperty& aProperty);

        TBool AddDateFieldL(
            CPbkContactItem& aDestItem,
            CPbkFieldInfo& aFieldInfo,
            CParserProperty& aProperty);
        
        TBool AddPronounciationFieldL(
            CPbkContactItem& aDestItem,
            CParserProperty& aProperty,
            TPbkMatchPriorityLevel& aPriorityLevel);

        CParserPropertyValue* GetExportTextValueL(
                TPbkContactItemField& aField);

        CParserPropertyValue* GetExportBinaryValueL(
            CPbkContactItem& aSourceItem,
            CBCardParserParamArray& aParameters);

        CParserPropertyValue* GetExportDateValueL(
            TPbkContactItemField& aField);

        CParserPropertyValue* GetExportArrayValueL(
            CPbkContactItem& aSourceItem, 
            const CBCardExportProperty& aExportProperty,
            CBCardParserParamArray& aParameters);

        TBool InsertExportArrayTextL(
            TPbkContactItemField& aField, CDesCArrayFlat& aArray);

        HBufC* GetFirstNameFieldTextL(const CDesCArray& aValues) const;
        HBufC* GetCompanyNameFieldTextL(const CDesCArray& aValues) const;
        HBufC* GetArrayValueCollectedL(
            const CDesCArray& aValues, 
            const TInt aStartIndex, 
            const TInt aEndIndex,
            const TInt aSkipIndex ) const;
        HBufC8* GetHBufCOfCBufSegL(CBufSeg* aBufSeg) const;
        TBool ContainsValidTextualDataL(TPtrC aDataText) const;

    private: // Data
		/// Ref: phonebook engine
        CPbkContactEngine* iEngine;
		/// Ref: name UID mapping
        CBCardFieldUidMapping* iNameUidMap;
		/// Ref: file name of the picure
        HBufC* iPictureFileName;
		/// Ref: file name of the thumbnail
		HBufC* iThumbnailFileName;
	};

#endif // __CBCARDFIELDACTION_H__

// End of File
