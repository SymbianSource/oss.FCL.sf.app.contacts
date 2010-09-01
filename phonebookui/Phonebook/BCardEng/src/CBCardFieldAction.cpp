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


// INCLUDE FILES
#include "BCardEng.h"
#include "CBCardFieldAction.h"

// System APIs
#include <vcard.h>

// Phonebook APIs
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <CPbkFieldInfo.h>
#include <CPbkAttachmentFile.h>
#include <CPbkFieldInfoGroup.h>   
#include <CPbkConstants.h>  
#include "TBCardPropertyAdapter.h"
#include "TPbkFieldInfoExportTypeAdapter.h"
#include "CBCardFieldUidMapping.h"
#include "CBCardImageUtil.h"
#include "CBCardParserParamArray.h"
#include "CBCardExportProperty.h"
#include "CBCardThumbnailHandler.h"
#include "CBCardParser.h"

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanic_InvalidNameFormat = 1,
    };

_LIT8(KPropValue, "VALUE");
_LIT8(KPropURL, "URL");
_LIT8(KPropPREF, "PREF");
_LIT8(KPropEncoding, "ENCODING");
_LIT8(KPropBase64, "BASE64");
_LIT8(KPropType, "TYPE");
_LIT8(KPropSound,"SOUND");
_LIT8(KParamIRMCN, "X-IRMC-N");
_LIT(KFormatJPEG, "JPEG");

} // namespace


// ================= MEMBER FUNCTIONS =======================

CBCardEngine::CBCardFieldAction* CBCardEngine::CBCardFieldAction::NewL(
        CPbkContactEngine& aEngine,
        CBCardFieldUidMapping& aNameUidMap,
        HBufC& aPictureFileName,
        HBufC& aThumbnailFileName)
    {
	CBCardFieldAction* self = new(ELeave) CBCardFieldAction(
                                            aEngine,
                                            aNameUidMap,
                                            aPictureFileName,
                                            aThumbnailFileName);
	return self;
    }

CBCardEngine::CBCardFieldAction::CBCardFieldAction(
        CPbkContactEngine& aEngine,
        CBCardFieldUidMapping& aNameUidMap,
        HBufC& aPictureFileName,
        HBufC& aThumbnailFileName)        
    {
    iEngine = &aEngine;
    iNameUidMap=&aNameUidMap;
    iPictureFileName = &aPictureFileName;
    iThumbnailFileName = &aThumbnailFileName;
    }

CBCardEngine::CBCardFieldAction::~CBCardFieldAction()
    {
    }

TBool CBCardEngine::CBCardFieldAction::AddCompactCardFieldL(
	    CPbkContactItem& aDestItem,
	    TPbkFieldId aFieldId,
	    const TDesC &aValue,
	    TBool aPhoneNumber,
        const TDesC* aLabel)
    {
    CPbkFieldInfo* fieldInfo =
		iEngine->FieldsInfo().Find(aFieldId, EPbkFieldLocationNone);
    if (fieldInfo)
        {
        return AddFieldWithValueL(aDestItem, *fieldInfo, aValue,
			aPhoneNumber, aLabel);
        }
    else
        {
        return EFalse;
        }
    }

TBool CBCardEngine::CBCardFieldAction::AddFieldWithValueL(
	    CPbkContactItem& aDestItem,
	    CPbkFieldInfo& aFieldInfo,
	    const TDesC &aValue,
	    TBool aPhoneNumber,
        const TDesC* aLabel)
    {
    if (aValue == KNullDesC)
        {
        return EFalse;
        }

    TPbkContactItemField* field= aDestItem.AddOrReturnUnusedFieldL(aFieldInfo);
    if(field)
	    {
		// Copy the property value
		HBufC* buf = HBufC::NewL(aValue.Length());
		CleanupStack::PushL(buf);
		TPtr buffer = buf->Des();
		buffer = aValue;

		// Clip the property value if it does not fit the field
		TInt maxlen = FindFieldMaximumLength(field->FieldInfo().FieldId());
		if((maxlen > 0) && maxlen < aValue.Length())
            {
			buffer.SetLength(maxlen);
            }

		CContactTextField* tf = field->ItemField().TextStorage();
	    tf->SetTextL(buffer);
		if(aPhoneNumber)
            {
			// If this is a preferred phone number set field as DefaultPhoneNumber
			if(!aDestItem.DefaultPhoneNumberField())
				{
				aDestItem.SetDefaultPhoneNumberFieldL(field);
				}
            }

        if(aLabel)
            {
            field->SetLabelL(*aLabel);
            }
		CleanupStack::PopAndDestroy(buf); 
		return ETrue;
    	}
	return EFalse;    
    }

TBool CBCardEngine::CBCardFieldAction::AddFieldL(
        CPbkContactItem& aDestItem,
        CParserProperty& aProperty,
        TPbkMatchPriorityLevel& aPriorityLevel)
    {
    TBCardPropertyAdapter adapter(aProperty,0,*iNameUidMap);    
    CPbkFieldInfo* fieldInfo = iEngine->FieldsInfo().Match(adapter, aPriorityLevel);
     
    TBool result = EFalse;
    if (fieldInfo)
        {
        if (aProperty.Value())
            {
            if ((aProperty.Name() == KPropSound) && (aProperty.Param(KParamIRMCN)))
                {
                // There was X-IRMC-N param used in SOUND field
                // --> multiple fields coming although KVersitPropertyHBufCUid used
                // (special case for Japanese pronounciation fields)
                return AddPronounciationFieldL(aDestItem, aProperty, aPriorityLevel);
                }
            if (aProperty.Value()->Uid() == TUid::Uid(KVersitPropertyHBufCUid))
                {
                result = AddTextFieldL(aDestItem,*fieldInfo,aProperty);
                }
            else if (aProperty.Value()->Uid() == TUid::Uid(KVersitPropertyCDesCArrayUid))
                {
                result = AddArrayFieldsL(aDestItem,aProperty,aPriorityLevel);
                }
            else if (aProperty.Value()->Uid() == TUid::Uid(KVersitPropertyBinaryUid))
                {
                result = AddBinaryFieldL(aDestItem,aProperty);
                } 
            else if (aProperty.Value()->Uid() == TUid::Uid(KVersitPropertyDateUid))
                {
                result = AddDateFieldL(aDestItem,*fieldInfo,aProperty);
                }                
            else
                {
                result = EFalse;
                }
            }
        }

    return result;
    }

TBool CBCardEngine::CBCardFieldAction::AddArrayFieldsL(
        CPbkContactItem& aDestItem,
        CParserProperty& aProperty,
        TPbkMatchPriorityLevel& aPriorityLevel)
    {
	CParserPropertyValueCDesCArray* arrayProperty = 
		reinterpret_cast<CParserPropertyValueCDesCArray*>(aProperty.Value());
	CDesCArray* values = arrayProperty->Value();
    
    TBool valueAdded = EFalse;
    for (TInt i = 0; i < values->Count(); i++)
        {                
        TBCardPropertyAdapter adapter(aProperty,i,*iNameUidMap);        
        CPbkFieldInfo* fieldInfo = iEngine->FieldsInfo().Match(adapter,aPriorityLevel);     
        if (fieldInfo)
            {
            TBool returnValue = EFalse; 
			if ( fieldInfo->FieldId() == EPbkFieldIdCompanyName && 
			    !iEngine->Constants()->LocallyVariatedFeatureEnabled(
                    EVPbkLVMegaOperatorFields ) )
				{
				// If department field (one of megaoperator fields) is not supported 
				// department field data is added to company field 
				HBufC* fieldTextHeap = GetCompanyNameFieldTextL(*values);
				CleanupStack::PushL(fieldTextHeap);
				returnValue = AddFieldWithValueL(
								aDestItem,
								*fieldInfo, 
								*fieldTextHeap);      
				CleanupStack::PopAndDestroy(fieldTextHeap); 
				}
		    else if ( fieldInfo->FieldId() == EPbkFieldIdFirstName )
		        {
				// Firstname field must contain all the rest
				// subfields from the N-property
				HBufC* fieldTextHeap = GetFirstNameFieldTextL(*values);
				CleanupStack::PushL(fieldTextHeap);
				returnValue = AddFieldWithValueL(
								aDestItem,
								*fieldInfo, 
								*fieldTextHeap);      
				CleanupStack::PopAndDestroy(fieldTextHeap); 
		        }
		    else
		        {
				returnValue = AddFieldWithValueL(
								aDestItem,
								*fieldInfo, 
								(*values)[i]);
		        }

            if (returnValue)
                {
                // If even one field was added set 'valueAdded' on  
                valueAdded = ETrue;
                }
            }
        }
    return valueAdded;    
    }

HBufC* CBCardEngine::CBCardFieldAction::GetFirstNameFieldTextL(
        const CDesCArray& aValues) const
    {
    // The index where the firstname fields
    // start in value array
    const TInt firstField(1);    
    // The last field to include; 4 is the last param suffix
    TInt lastField( 4 );
    // Field to be skipped
    TInt skipField( KErrNotFound );
    
    // Check supported fields and add not supported to first name field
    TBool megaOps = iEngine->Constants()->LocallyVariatedFeatureEnabled(
                    EVPbkLVMegaOperatorFields );
    TBool mdo = iEngine->Constants()->LocallyVariatedFeatureEnabled(
                    EPbkLVAddMDOFields );

    if ( mdo )
        {
        if ( megaOps )
            {
            // all fields are supported
            lastField = 1;
            }
        else
            {
            // add middle name to first name field
            lastField = 2;
            }
        }
    
    if ( megaOps && !mdo )
        {
        // Skip middle name because it has own field
        skipField = 2;
        }
        
    return GetArrayValueCollectedL(aValues,firstField,lastField, skipField);
    }

HBufC* CBCardEngine::CBCardFieldAction::GetCompanyNameFieldTextL(
        const CDesCArray& aValues) const
    {
    // The index where the company name fields start
    // start in value array
    const TInt firstField = 0;    
    // The last field to include; 1 is the index 
    // of the company namefield to include from the card
    const TInt lastField = 1;
    const TInt skipField = KErrNotFound; // no field skipping
    return GetArrayValueCollectedL(aValues,firstField,lastField, skipField);
    }

HBufC* CBCardEngine::CBCardFieldAction::GetArrayValueCollectedL(
        const CDesCArray& aValues, 
        const TInt aStartIndex, 
        const TInt aEndIndex,
        const TInt aSkipIndex ) const
    {

    // The last field to include
    const TInt lastField = Min (aValues.Count()-1,aEndIndex);

    // Calculate buffer size
    TInt bufSize = 0;
    TInt i = 0;
    for (i = aStartIndex; i <= lastField; i++)
        {
        if ( aSkipIndex != i )
            {
            bufSize+=aValues[i].Length();
    		if((bufSize > 0) && (aValues[i].Length() > 0) && (i != lastField))
                {
                // add room for space
    			bufSize++;
                }
            }
        }
    
	// Allocate buffer
	HBufC* buffer = HBufC::NewMaxLC(bufSize);
	TPtr writeBuf = buffer->Des();
    writeBuf = KNullDesC;

    // Construct fieldtext
    for (i = aStartIndex; i <= lastField; i++)
        {
        if ( aSkipIndex != i )
            {
            if ((writeBuf.Length() > 0) && (aValues[i].Length() > 0))                
                {
                writeBuf.Append(EKeySpace);
                }
            writeBuf.Append(aValues[i]);
            }
        }
    
    // Pass the ownership to the caller
    CleanupStack::Pop(buffer);
    return buffer;    
    }

TBool CBCardEngine::CBCardFieldAction::AddTextFieldL(
        CPbkContactItem& aDestItem,
        CPbkFieldInfo& aFieldInfo,
        CParserProperty& aProperty)
    {
	CParserPropertyValueHBufC* hbufVal = 
		static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
	TPtrC normalVal = hbufVal->Value();
    
    HBufC* tempBuf = HBufC::NewLC(normalVal.Length());
    TPtr tempBufVal = tempBuf->Des();
    tempBufVal = normalVal;
    
    TBool setDefaultPhone = EFalse;
    if (aFieldInfo.IsPhoneNumberField())
        {
	    CParserParam* param = aProperty.Param(KPropPREF);
        if (param)
            {
            setDefaultPhone = ETrue;
            }
        }

    TBool returnValue = AddFieldWithValueL(aDestItem, aFieldInfo,
                            tempBufVal, setDefaultPhone);
    CleanupStack::PopAndDestroy(tempBuf);
    return returnValue;
    }

TBool CBCardEngine::CBCardFieldAction::AddBinaryFieldL(
        CPbkContactItem& aDestItem,
        CParserProperty& aProperty)
    {
	CParserParam* param = aProperty.Param(KPropValue);
	if(param)
		{
        // Here we don't support URL for external binaries
		if(!param->Value().Compare(KPropURL))
            {
			return EFalse;
            }
		}
    CParserPropertyValueBinary* val = 
		static_cast<CParserPropertyValueBinary*>(aProperty.Value());
		
	if (val->Value()->Size() == 0)
	    {
        // Some devices send all the properties they support, even
        // if they don't have data value at all. Return and ignore
        // empty binary field.
	    return EFalse;
	    }

    CBCardThumbnailHandler* handler = CBCardThumbnailHandler::NewL(*iEngine);
    CleanupStack::PushL(handler);

    // save the thumbnail to item
    HBufC8* valueBuf = GetHBufCOfCBufSegL(MUTABLE_CAST(CBufSeg*, val->Value()));
    CleanupStack::PushL(valueBuf);
    handler->SaveThumbnailL(aDestItem, *valueBuf);
    CleanupStack::PopAndDestroy(valueBuf);
    CleanupStack::PopAndDestroy(handler);
    return ETrue;
    }

TBool CBCardEngine::CBCardFieldAction::AddDateFieldL(
        CPbkContactItem& aDestItem,
        CPbkFieldInfo& aFieldInfo,
        CParserProperty& aProperty)
    {
    CParserPropertyValueDate* dval = 
		reinterpret_cast<CParserPropertyValueDate*>(aProperty.Value());
	TVersitDateTime* dateval = dval->Value();
    
    if (!dateval)
        {
        return EFalse;
        }

    TPbkContactItemField* field = aDestItem.AddOrReturnUnusedFieldL(aFieldInfo);
	if(field) 
        {
		field->ItemField().DateTimeStorage()->SetTime(dateval->iDateTime);
		return ETrue;
		}
    else 
        {
		return EFalse;
        }    
    }

/**
 * Pronounciation fields come from the Versit parser as hbuf values although
 * containing several fields. The format is:
 * SOUND;X-IRMC-N;CHARSET=SHIFT_JIS:LASTNAME_READING;FIRSTNAME_READING;
 * This methods takes a hbufc-property, uses versit parser to make an
 * descarray of it and then creates a array property and gives
 * it to array field adder
 */
TBool CBCardEngine::CBCardFieldAction::AddPronounciationFieldL(
        CPbkContactItem& aDestItem,
        CParserProperty& aProperty,
        TPbkMatchPriorityLevel& aPriorityLevel)
    {
    // Get the HBufC property
	CParserPropertyValueHBufC* hbufVal = 
		static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
	TPtrC normalVal = hbufVal->Value();

    // Create a modifiable descriptor
    HBufC16* bufPtr = normalVal.Alloc();
    CleanupStack::PushL(bufPtr);
    TPtr value = bufPtr->Des();

    // Instantiate the parser
    CBCardParser* parser = CBCardParser::NewL();
    CleanupStack::PushL(parser);

    // Parse the values to an array
    CDesCArray* array = parser->MakePropertyValueCDesCArrayL(value);
    // Cleanup
    CleanupStack::PopAndDestroy(parser);
    CleanupStack::PopAndDestroy(bufPtr);
    
    CleanupStack::PushL(array);
    // Create new arrayed parser property value
    CParserPropertyValueCDesCArray* arrayPropValue =
        new (ELeave) CParserPropertyValueCDesCArray(array);
    CleanupStack::Pop(array);
    CleanupStack::PushL(arrayPropValue);

    // Create new parser property
    CParserProperty* newProperty = CParserProperty::NewL(*arrayPropValue,
        KParamIRMCN, NULL);
    CleanupStack::Pop(arrayPropValue);
    CleanupStack::PushL(newProperty);
    
    // Give the new property to array field adder
    TBool returnValue = AddArrayFieldsL(aDestItem, *newProperty, aPriorityLevel);

    // Cleanup and return
    CleanupStack::PopAndDestroy(newProperty);
    return returnValue;
    }


void CBCardEngine::CBCardFieldAction::ExportContactFieldL(
        const CBCardExportProperty& aExportProperty,
        CParserVCard& aParser, 
        CPbkContactItem& aSourceItem,
        TPbkContactItemField& aField)
    {
    CBCardParserParamArray* paramArr = new (ELeave) CBCardParserParamArray(1);
    CleanupStack::PushL(paramArr);
    
	CParserPropertyValue* value = NULL;
    value = GetExportPropertyValueL(
            aSourceItem, aField, aExportProperty.StorageType(), *paramArr);

    if (value)
        {
        CleanupStack::PushL(value);
        GetAdditionalParametersL(aField, *paramArr, aSourceItem);
	    CParserProperty* property = 
            CParserProperty::NewL(*value, aExportProperty.Name(), paramArr);        
        CleanupStack::Pop(value);
        CleanupStack::Pop(paramArr);
        // NOTE: Versit pushes the CParserProperty* pointer stright away
        // to the cleanup stack, so to avoid duplicate entries in
        // the stack pointer must not be pushed there.
		aParser.AddPropertyL(property, EFalse);
        }
    else
        {
        CleanupStack::PopAndDestroy(paramArr); 
        }
    }

CParserPropertyValue* CBCardEngine::CBCardFieldAction::GetExportPropertyValueL(
        CPbkContactItem& aSourceItem,
        TPbkContactItemField& aField,
        const TPbkVersitStorageType& aType,
        CBCardParserParamArray& aParameters)
    {
    CParserPropertyValue* returnValue = NULL;
    switch (aType)
        {
        case EPbkVersitPropertyHBufC:
            {
            returnValue = GetExportTextValueL(aField);
            break;
            }

        case EPbkVersitPropertyBinary:
            {
            returnValue = GetExportBinaryValueL(aSourceItem, aParameters);
            break;
            }

        case EPbkVersitPropertyDateTime:
            {
            returnValue = GetExportDateValueL(aField);
            break;
            }

        default:
            {
            returnValue = NULL;
            }
        }
    
    return returnValue;
    }

TBool CBCardEngine::CBCardFieldAction::GetAdditionalParametersL(
        TPbkContactItemField& aField,
        CBCardParserParamArray& aParameters,
        CPbkContactItem& aItem)
    {
    TPbkFieldInfoExportTypeAdapter adapter(aField.FieldInfo());    
    const MPbkVcardParameters& parameters = adapter.PropertyParameters();  
    
    TUid uid;
    for (TInt i = 0; i < parameters.ParameterCount(); i++)
        {
        uid = parameters.ParameterAt(i);
        TPtrC8 name = iNameUidMap->FindName(uid);
        if (name != KNullDesC8)
            {
		    CParserParam* param = CParserParam::NewL(name,KNullDesC);
		    CleanupStack::PushL(param);
		    aParameters.AppendL(param);
		    CleanupStack::Pop(param);
            }
        }

    // Set phone number preference if this is a Phone number field
	if(aField.FieldInfo().IsPhoneNumberField() && 
		&aField == aItem.DefaultPhoneNumberField())
	    {
		CParserParam* param = CParserParam::NewL(KPropPREF, KNullDesC);
		CleanupStack::PushL(param);
		aParameters.AppendL(param);
		CleanupStack::Pop(param);
		}

    // Set X-IRMC-N extension if this a Japanese reading field
    if(aField.FieldInfo().IsReadingField())
	    {
		CParserParam* param = CParserParam::NewL(KParamIRMCN, KNullDesC);
		CleanupStack::PushL(param);
		aParameters.AppendL(param);
		CleanupStack::Pop(param);
		}

    return EFalse;
    }

void CBCardEngine::CBCardFieldAction::ExportArrayOfFieldsL(
        CParserVCard& aParser, 
        CPbkContactItem& aSourceItem, 
        const CBCardExportProperty& aExportProperty)
    {
    CBCardParserParamArray* paramArr = new (ELeave) CBCardParserParamArray(1);
    CleanupStack::PushL(paramArr);
    
	CParserPropertyValue* value = NULL;
    value = GetExportArrayValueL(aSourceItem, aExportProperty, *paramArr);

    if (value)
        {
        CleanupStack::PushL(value);
	    CParserProperty* property = 
            CParserProperty::NewL(*value, aExportProperty.Name(), paramArr);
        CleanupStack::Pop(value);        
        CleanupStack::Pop(paramArr);            
        // NOTE: Versit pushes the CParserProperty* p stright away
        // to the cleanup stack, so to avoid duplicate entries in
        // the stack p must not be pushed there.
		aParser.AddPropertyL(property, EFalse);
        }
    else
        {
        CleanupStack::PopAndDestroy(paramArr);
        }
    }

CParserPropertyValue* CBCardEngine::CBCardFieldAction::GetExportArrayValueL(
        CPbkContactItem& aSourceItem, 
        const CBCardExportProperty& aExportProperty,
        CBCardParserParamArray& aParameters)
    {
	CDesCArrayFlat* desarr = new(ELeave) CDesCArrayFlat(aExportProperty.InfoCount());
	CleanupStack::PushL(desarr);
    TBool dataInserted = EFalse;
    
    for (TInt i = 0; i < aExportProperty.InfoCount(); i++ )
        {
        const CPbkFieldInfo* info = aExportProperty.InfoAt(i);
        if (info)
            {
            TPbkContactItemField* field = aSourceItem.FindField(*info);
            if (field)
                {
                if (InsertExportArrayTextL(*field, *desarr))
                    {
                    // If even one of the fields contains real data
                    // mark the property to be created
                    if (!dataInserted)
                        {
                        // get the additional parameters from the 
                        // first field which contains data
                        GetAdditionalParametersL(*field, aParameters, aSourceItem);
                        }

                    dataInserted = ETrue;
                    }
                }
            }        
        }

    if (dataInserted)
        {
        CParserPropertyValue* val = new (ELeave)CParserPropertyValueCDesCArray(desarr);
   	    CleanupStack::Pop(desarr);
        return val;
        }
    else
        {
   	    CleanupStack::PopAndDestroy(desarr);
        return NULL;
        }
    }

// Inline because only one call site
inline TBool CBCardEngine::CBCardFieldAction::InsertExportArrayTextL(
        TPbkContactItemField& aField, CDesCArrayFlat& aArray)
    {
    TPbkFieldInfoExportTypeAdapter adapter(aField.FieldInfo());    
    TUid name = adapter.PropertyName();
    
    TInt index = iNameUidMap->FindIndex(name);
    
    TBool containsData = ContainsValidTextualDataL(aField.Text());

    if (index != KErrNotFound)
        {
        while (aArray.Count() < index)
            {
            aArray.AppendL(KNullDesC);    
            }

        // If actual data is in the field add it to array - else add null string
        if (containsData)
            {
            aArray.InsertL(index, aField.Text());
            }
        else
            {
            aArray.InsertL(index, KNullDesC);            
            }

        if (index != (aArray.Count() - 1))
            {
            // delete the item which was replaced and shifted one step ahead
            aArray.Delete(index+1);
            }
        }

    // If there was added some actual value return true
    // If no actual value was added return false
    return containsData;
    }

CParserPropertyValue* CBCardEngine::CBCardFieldAction::GetExportTextValueL(
        TPbkContactItemField& aField)
    {    
	if(!ContainsValidTextualDataL(aField.Text()))
        {
        return NULL;
        }

    CParserPropertyValueHBufC* value = CParserPropertyValueHBufC::NewL(aField.Text());

    return value;
    }

CParserPropertyValue* CBCardEngine::CBCardFieldAction::GetExportDateValueL(
        TPbkContactItemField& aField)
    {
	TVersitDateTime* vdt = 
		new (ELeave) TVersitDateTime(aField.ItemField().DateTimeStorage()->Time().DateTime(), 
		TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(vdt);
	CParserPropertyValue* value = new (ELeave) CParserPropertyValueDate(vdt);
	CleanupStack::Pop(vdt); 
    return value;
    }

CParserPropertyValue* CBCardEngine::CBCardFieldAction::GetExportBinaryValueL(
        CPbkContactItem& aSourceItem,
        CBCardParserParamArray& aParameters)
    {    
    CParserPropertyValue* value = NULL;

    CBCardThumbnailHandler* handler = CBCardThumbnailHandler::NewL(*iEngine);
    CleanupStack::PushL(handler);

    if (handler->HasThumbnail(aSourceItem))
        {
	    CParserParam* param = CParserParam::NewL(KPropEncoding, KPropBase64);
	    CleanupStack::PushL(param);
	    aParameters.AppendL(param);
	    CleanupStack::Pop(param);
        
        // format is always jpeg when retrieved from db
	    param = CParserParam::NewL(KPropType, KFormatJPEG);
	    CleanupStack::PushL(param);
	    aParameters.AppendL(param);
	    CleanupStack::Pop(param);

        value = CParserPropertyValueBinary::NewL(handler->GetThumbnailL(aSourceItem));
        }

    CleanupStack::PopAndDestroy(handler);        
    return value;
    }

/**
 * Returns a maximum length of field using Phonebook Engine's services.
 * @param aFieldId Phonebook FieldId of which length should be determined
 */
TInt CBCardEngine::CBCardFieldAction::FindFieldMaximumLength
		(TPbkFieldId aFieldId)
    {
	CPbkFieldInfo *fi = iEngine->FieldsInfo().Find(aFieldId);
	if(fi)
        {
		return fi->MaxLength();
        }
	else
        {
		return 0;
        }
    }

HBufC8* CBCardEngine::CBCardFieldAction::GetHBufCOfCBufSegL(CBufSeg* aBufSeg) const
    {
    TInt size = aBufSeg->Size();
    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 bufferPtr = buffer->Des();
    
    TInt i = 0;
    TPtrC8 tmpPtr = aBufSeg->Ptr(i);
    while (tmpPtr.Length() > 0)
        {
        bufferPtr.Append(tmpPtr);
        i += tmpPtr.Length();
        tmpPtr.Set(aBufSeg->Ptr(i));
        }
    
    CleanupStack::Pop(buffer); 
    return buffer;
    }

TBool CBCardEngine::CBCardFieldAction::ContainsValidTextualDataL(TPtrC aDataText) const
    {    
    if (!aDataText.Length())
        {
        return EFalse;
        }

	HBufC* tmpText = aDataText.AllocLC();
    TPtr tmpPtr = tmpText->Des();
    tmpPtr.Trim();
    
	if(tmpPtr.Length() == 0)
        {
	    CleanupStack::PopAndDestroy(tmpText);
        return EFalse;
        }

    CleanupStack::PopAndDestroy(tmpText);
    return ETrue;
    }

// End of File
