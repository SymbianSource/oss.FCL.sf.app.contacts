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
* Description:  Exporting the contact field from contact to 
*                CParserProperty field
*
*/


#ifndef CVPBKVCARDPROPERTYEXPORTER_H
#define CVPBKVCARDPROPERTYEXPORTER_H

// INCLUDES
#include "TVPbkFieldVersitProperty.h"
#include "CVPbkVCardActiveIncremental.h"
#include <MVPbkSingleContactOperationObserver.h>
#include "CVPbkOwnCardHandler.h"
#include "CVPbkGroupCardHandler.h"

#include <e32base.h>
#include <badesca.h>


// FORWARD DECLARATIONS
class CParserVCard;
class MVPbkStoreContact;
class CVPbkVCardFieldTypeProperty;
class CVPbkVCardIdNameMapping;
class MVPbkFieldTypeList;
class CParserPropertyValue;
class CVPbkVCardParserParamArray;
class MVPbkBaseContactField;
class TVPbkVCardIndexedProperty;
class RWriteStream;
class MVPbkContactLink;
class CVPbkContactManager;
class MVPbkStoreContactField;
class CParserVCard;

// CLASS DECLARATIONS
/**
 * Class is exporting the contact field from contact to 
 * CParserProperty field
 */
NONSHARABLE_CLASS(CVPbkVCardPropertyExporter)
:   public CVPbkVCardActiveIncremental,
    public MVPbkVCardIncremental,
    public MVPbkSingleContactOperationObserver
    {
    public:
        /**
         * Create new instance of this class.
         */
        static CVPbkVCardPropertyExporter* NewLC( 
            CVPbkVCardData& aData, 
			const MVPbkStoreContact* aSourceItem, 
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed);

        static CVPbkVCardPropertyExporter* NewLC( 
            CVPbkVCardData& aData,
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed);

        ~CVPbkVCardPropertyExporter();

    protected: // From MVPbkVCardIncremental

        virtual TInt Next(TInt& aRemainingSteps);
        virtual void CloseL();
        virtual void HandleLeave(TInt aError);
        virtual void HandleCancel();

    protected: // From MVPbkContactOperation

        virtual void StartL();

    private:
        CVPbkVCardPropertyExporter(
            CVPbkVCardData& aData, 
			const MVPbkStoreContact* aSourceItem, 
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed);

        CVPbkVCardPropertyExporter(
            CVPbkVCardData& aData, 
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed);

        void ConstructL(CVPbkVCardData& aData);

    public: // New functions

        void InitLinkL(
            const MVPbkContactLink& aContactLink, 
            CVPbkContactManager& aContactManager );

    public: // From MVPbkSingleContactOperationObserver

        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);

        void VPbkSingleContactOperationFailed
            (MVPbkContactOperationBase& aOperation, TInt aError);

    private: 
        void ExportFieldL( CVPbkVCardFieldTypeProperty& aVCardProperty );

        void GetValuesFromIndexedPropertiesL( TBool aSingleItem );

        TBool GetSingleValueAndParamsL( 
            const TVPbkVCardIndexedProperty& aIndexedProperty, 
            CVPbkVCardParserParamArray& aParamArray,
            TInt& aFindIndex );

        MVPbkStoreContactField*
            FindFieldFromContact( 
                const TVPbkVCardIndexedProperty& aIndexedProperty, 
                TInt& aIndex );

        CParserPropertyValue* FetchDataFromFieldL( 
            MVPbkStoreContactField&, 
            CVPbkVCardParserParamArray& aParameters );

        TBool AddPropertyToParserL( RPointerArray< CParserPropertyValue >& aValues,
                                    CVPbkVCardParserParamArray* aParams);

        TBool HasValues();

        CParserPropertyValue* GetExportTextValueL(
            const MVPbkBaseContactField& aField);

        CParserPropertyValue* GetExportUriValueL(
            const MVPbkBaseContactField& aField);

        CParserPropertyValue* GetExportDateValueL(
            const MVPbkBaseContactField& aField);

        CParserPropertyValue* GetExportBinaryValueL(
            const MVPbkBaseContactField& aField,
            CVPbkVCardParserParamArray& aParameters );

        TBool ContainsValidTextualDataL(const TDesC& aDataText) const;

        void GetAdditionalParamsL( 
            MVPbkStoreContactField& aField, CVPbkVCardParserParamArray& aParams );

        CParserPropertyValue* GetValueL(
            RPointerArray< CParserPropertyValue >& aValues);

        void InitDataL();

        void DoNextL();
        void HandleOwnCardL();
        void HandleGroupCardL();
        void AddRevL();
        CParserPropertyValueDateTime* CreateDateTimePropertyL(
            const TTime& aDateTime, 
            TVersitDateTime::TRelativeTime aRelativeTime);

    private:
        CParserVCard* iParser;
        /// Own: Array of property values
        RPointerArray< CParserPropertyValue >           iValues;
        /// Ref: Current VCard property
        CVPbkVCardFieldTypeProperty*					iVCardProperty;
        /// Ref: Source contact
        const MVPbkStoreContact*                        iContact;
        /// Ref: Destination stream
        RWriteStream&                                   iDestinationStream;
        /// Own: Internal index for execution loop
        TInt                                            iCurrentPropertyIndex;
        /// Own: Maximum property count
        TInt                                            iMaxPropertyCount;
        /// Own: The source contact which is owned by exporter
        MVPbkStoreContact*                              iOwnedContact;
        /// Own: The last operation executed
        MVPbkContactOperationBase*                      iOperation;
        /// Own: Internal state of the class
        enum TVPbkVCardExporterState
            {
            ECreatingRetriever,
            EExporting
            } iState;
            
        // Ref: Array to hold those fields masked during a beamed export
         const CDesC8ArrayFlat*  iMaskedFieldsArray;
         
         // ETrue if the current orperation is a beamed export, otherwise EFalse
         const TBool iBeamed;
         CVPbkOwnCardHandler* iOwncardHandler;
         //Own: GroupCard Handler
         CVPbkGroupCardHandler* iGroupcardHandler;
    };

#endif // CVPBKVCARDPROPERTYEXPORTER_H

// End of file
