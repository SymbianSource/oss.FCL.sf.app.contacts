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
*      A class to convert Phonebook items to vCard stream and vice versa
*
*/


// INCLUDE FILES
#include "BCardEng.h" // This class

// System APIs
#include <s32mem.h>
#include <barsc.h>
#include <barsread.h>
#include <cntfldst.h>
#include <vcard.h>

// Series 60 APIs
#include <StringLoader.h>
#include <featmgr.h>
#include <bldvariant.hrh>

// Phonebook APIs
#include <PbkEngUtils.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include <CPbkFieldInfo.h>
#include <CPbkAttachmentFile.h>
#include <CPbkConstants.h>

#include <bcardeng.rsg>
#include "CBCardParserParamArray.h"
#include "CBCardFieldAction.h"
#include "CBCardFieldUidMapping.h"
#include "CBCardExportProperty.h"
#include "TPbkMatchPriorityLevel.h"
#include "TBCardVersitPlugin.h"
#include "CBCardParser.h"
#include "PbkDataCaging.hrh"

/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS
_LIT(KResFileName, "BCardEng.rsc");
_LIT(KBusinessCardText, "Business Card");   // Business card signature
_LIT(KCloseParenthesis, ")");
_LIT(KTelText, "tel ");
_LIT(KFaxText, "fax ");
_LIT(KSeparatorText, ", ");

const TText KOpenParenthesis = '(';
const TInt KBufferSize = 200;
const TInt KStartPosition = 4;
const TInt KLabelStartPosition = 5;
const TInt KLabelValueDelimiterLength = 2;

} // namespace

// ================= MEMBER FUNCTIONS =======================

inline CBCardEngine::CBCardEngine()
    {
    }

EXPORT_C CBCardEngine* CBCardEngine::NewL(CPbkContactEngine *aEngine)
    {
	CBCardEngine* self = new(ELeave) CBCardEngine();
	CleanupStack::PushL(self);
	self->ConstructL(aEngine);
	CleanupStack::Pop(self);
	return self;
    }

/*
 * ConstructL() reads the import/export definitions from the resource file
 * and stores them into class member arrays.
 */
void CBCardEngine::ConstructL(CPbkContactEngine* aEngine)
    {
    // Open a file server session
    RFs fs;
	User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Initialize feature manager
    FeatureManager::InitializeLibL();

    // Open the resource file
    RResourceFile resourceFile;
    PbkEngUtils::FindAndOpenResourceFileLC(fs, KResFileName,
		KDC_RESOURCE_FILES_DIR, KPbkRomFileDrive, resourceFile);

	TResourceReader resReader;

	// Read picture filenames
	resReader.SetBuffer(resourceFile.AllocReadLC(R_QTN_BCARD_FILENAME_PICTURE));
	iPictureFileName = resReader.ReadHBufCL();
    CleanupStack::PopAndDestroy();  // R_QTN_BCARD_FILENAME_PICTURE

	resReader.SetBuffer(resourceFile.AllocReadLC(R_QTN_BCARD_FILENAME_THUMBNAIL));    
    iThumbnailFileName = resReader.ReadHBufCL();
    CleanupStack::PopAndDestroy();  // R_QTN_BCARD_FILENAME_THUMBNAIL
    
    // Read propery name (+index) <-> Uid mapping
    resReader.SetBuffer(resourceFile.AllocReadLC(R_PBK_BCARD_NAME_UID_MAPPING));
    iNameUidMap = CBCardFieldUidMapping::NewL(resReader);
    CleanupStack::PopAndDestroy(); // R_PBK_BCARD_NAME_UID_MAPPING
    
    // Read export properties
    resReader.SetBuffer(resourceFile.AllocReadLC(R_PBK_BCARD_EXPORT_VCARD));
	TInt count = resReader.ReadInt16();
    while (--count >= 0)
        {
        CBCardExportProperty* property = 
            CBCardExportProperty::NewLC(resReader, aEngine->FieldsInfo());
        // Do not export non-existing fields, this check is required for
        // western version not to export japanese reading fields
        if (property->InfoCount() > 0)
            {
            // Array takes ownership of the property
            User::LeaveIfError(iExportProperties.Append(property));
            CleanupStack::Pop(property);
            }
        else
            {
            CleanupStack::PopAndDestroy(property);
            }
        }
    CleanupStack::PopAndDestroy(3);  // resourceFile, fs
    
    iEngine = aEngine;
    iFieldAction = CBCardFieldAction::NewL(
        *aEngine,*iNameUidMap, *iPictureFileName, *iThumbnailFileName);
    }

EXPORT_C CBCardEngine::~CBCardEngine()
    {
    delete iNameUidMap;
	delete iThumbnailFileName;
	delete iPictureFileName;
    delete iFieldAction;
    iExportProperties.ResetAndDestroy();
    iExportProperties.Close();
    FeatureManager::UnInitializeLib();
    }

/*
 * THE MAIN IMPORT FUNCTION
 * Properties are imported from vCard file according to the specifications
 * loaded from the resource file.
 */ 
EXPORT_C void CBCardEngine::ImportBusinessCardL(
    CPbkContactItem& aDestItem, RReadStream &aSourceStream)
    {
    // Must use CParserVCard here, not the own parser class!
    // Otherwise there will be problems, and the parsing goes wrongly
    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);

    if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        // if in Japanese mode, use shift-jis by default
        parser->SetDefaultCharSet(Versit::EShiftJISCharSet);
        parser->SetFlags(CVersitParser::EUseDefaultCharSetForAllProperties);
        }

    // This has to be trapped
    TRAPD(err,parser->InternalizeL(aSourceStream));        
    if ((err != KErrNone) && (err != KErrEof))
        {
        User::Leave(err);        
        }

    CArrayPtr<CParserProperty>* arr = parser->ArrayOfProperties(EFalse);   
    if (!arr)
        {
        // Return if there are no properties
        CleanupStack::PopAndDestroy(parser); 
        return;
        }

    // Loop through different prioritylevels
    for (TPbkMatchPriorityLevel priorityLevel(iEngine->FieldsInfo().CreateMatchPriority());
        !priorityLevel.End(); 
        priorityLevel.Next())
        {
        // Loop through all the fields
        for (TInt i=0; i<arr->Count(); i++)
            {
            CParserProperty* prop = arr->At(i);        
            if (prop)
                {
                iFieldAction->AddFieldL(aDestItem, *prop, priorityLevel);
                }    
            }
        }

    CleanupStack::PopAndDestroy(parser); // parser
    }

/**
 * The main Export function.
 */
EXPORT_C void CBCardEngine::ExportBusinessCardL(
        RWriteStream& aDestStream, 
        CPbkContactItem& aSourceItem)
    {
    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);
    
    // For Japanese: Set charset based on local variation setting
    if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        if (iEngine->Constants()->LocallyVariatedFeatureEnabled(EPbkLVUseJapaneseUTF8))
            {
            parser->SetDefaultCharSet(Versit::EUTF8CharSet);
            }
        else
            {
            parser->SetDefaultCharSet(Versit::EShiftJISCharSet);
            }
        parser->SetFlags(CVersitParser::EUseDefaultCharSetForAllProperties);
        }
    
    // use plug in for disabling encoding
    TBCardVersitPlugin* exportPlugin = new (ELeave) TBCardVersitPlugin;
    CleanupStack::PushL(exportPlugin);
    parser->SetPlugIn(exportPlugin);

    for (TInt i = 0; i < iExportProperties.Count(); i++)
        {
        ProcessExportPropertyL(*parser, aSourceItem, *iExportProperties[i]);       
        }

    // Externalize the Parser to the Stream
    parser->ExternalizeL(aDestStream);

    CleanupStack::PopAndDestroy(exportPlugin);
    CleanupStack::PopAndDestroy(parser);
    }

void CBCardEngine::ProcessExportPropertyL(
        CParserVCard& aParser, 
        CPbkContactItem& aSourceItem, 
        const CBCardExportProperty& aExportProperty)
    {
    if (aExportProperty.StorageType() == EPbkVersitPropertyCDesCArray)
        {
        iFieldAction->ExportArrayOfFieldsL(aParser, aSourceItem, aExportProperty);
        }
    else
        {
        const CPbkFieldInfo* info = aExportProperty.InfoAt(0);
        
        TBool doContinue = (info->Multiplicity() == EPbkFieldMultiplicityMany);
        
        TInt index = 0;
        do 
            {           
            TPbkContactItemField* field = aSourceItem.FindField(*info, index);            
            if (field)
                {
                ++index;
                iFieldAction->ExportContactFieldL(
                    aExportProperty, 
                    aParser, 
                    aSourceItem, 
                    *field);
                }
            else
                {
                doContinue = EFalse;
                }
        
            } while (doContinue);
        }
    }

TInt CBCardEngine::GetByteFromStream(TUint8& aByte, RReadStream& aSourceStream)
    {
    TRAPD(err, aByte = aSourceStream.ReadUint8L());
    return err;
    }

TInt CBCardEngine::GetLineFromStreamL(TDes& aBuf, RReadStream& aSourceStream)
    {
    aBuf.SetLength(0);
    TUint8 byte=0;
    while(GetByteFromStream(byte, aSourceStream) == KErrNone)
        {
        if(byte == EKeyLineFeed)
            {
            break;
            }
        if(byte != EKeyEnter)
            {
            if (aBuf.Length() < aBuf.MaxLength())
                {
                aBuf.Append(byte);
                }
            else
                {
                User::Leave(KErrCorrupt);
                }
            }
        }
    return aBuf.Length();
    }

EXPORT_C TBool CBCardEngine::ImportCompactBusinessCardL(
    CPbkContactItem& aDestItem, RReadStream &aSourceStream)
    {
    TBuf<KBufferSize> buf;

    // "Business Card" signature
    if(GetLineFromStreamL(buf, aSourceStream))
        {
        if(buf.Match(KBusinessCardText)==KErrNotFound)
            {
            return EFalse;
            }
        }        

    // Name
    if(GetLineFromStreamL(buf, aSourceStream))
        {
        iFieldAction->AddCompactCardFieldL(aDestItem, EPbkFieldIdLastName, buf);
        }

    // Company
    if(GetLineFromStreamL(buf, aSourceStream))
        {
        iFieldAction->AddCompactCardFieldL(aDestItem, EPbkFieldIdCompanyName, buf);
        }

    // Title
    if(GetLineFromStreamL(buf, aSourceStream))
        {
        iFieldAction->AddCompactCardFieldL(aDestItem, EPbkFieldIdJobTitle, buf);
        }

    // Telephone numbers
    TInt len=0;
    while((len = GetLineFromStreamL(buf, aSourceStream))>0)
        {
        TPbkFieldId fieldid = EPbkFieldIdNone;
        if(buf.Find(KTelText)>=0)
            {
            fieldid = EPbkFieldIdPhoneNumberStandard;
            } 
        else if(buf.Find(KFaxText)>=0) 
            {
            fieldid = EPbkFieldIdFaxNumber;
            }

        if(fieldid == EPbkFieldIdNone)
            {
            break;
            }
        else if(len <= KStartPosition)
            {
            // no data on this field, try next
            continue;
            }
        
		// Read tel num label
		TInt position = KStartPosition;
        HBufC* label = NULL;
        if(buf[KStartPosition] == KOpenParenthesis)
            {
            position = buf.Find(KCloseParenthesis);
            if(position == KErrNotFound)
                {
                continue;
                }
            label = HBufC::NewLC(position-KStartPosition);
    		TPtr writebuf = label->Des();
	    	writebuf = KNullDesC;
            for(TInt i=KLabelStartPosition; i<position; i++)
                {
                writebuf.Append(buf[i]);
                }
			// Jump few characters
            position += KLabelValueDelimiterLength;
            }
            
        if(position < buf.Length())
            {
    		// Read value
            HBufC* value = HBufC::NewLC(buf.Length()-position);
       		TPtr writebuf = value->Des();
        	writebuf = KNullDesC;
            for(TInt i=position; i<buf.Length(); i++)
                {
                writebuf.Append(buf[i]);
                }

    		iFieldAction->AddCompactCardFieldL(aDestItem, fieldid,
    			*value, EFalse, label);

            CleanupStack::PopAndDestroy(value); 
            }
            
        if(label)
            {
            CleanupStack::PopAndDestroy(label); 
            }
        }

    // Email address
    if(len)
        {
        iFieldAction->AddCompactCardFieldL(aDestItem,
			EPbkFieldIdEmailAddress, buf);
        }

    // Postal address (one or more lines)
    // First read all the address lines and store them
    // in a list
    CDesCArray* arr = new(ELeave) CDesCArrayFlat(2); // typically 2 lines
    CleanupStack::PushL(arr);
    len=0;
    while(GetLineFromStreamL(buf, aSourceStream))
        {
        arr->AppendL(buf);
        if(len)
            {
            len+=KSeparatorText().Length();
            }
        len+=buf.Length();
        }

    // Then concatenate them with commas into one string
    // and use that as field value.
    HBufC* fielddata = HBufC::NewLC(len);
	TPtr writebuf = fielddata->Des();
	writebuf = KNullDesC;
    for(TInt i=0;i<arr->Count();i++)
        {
        if(i)
            {
            writebuf.Append(KSeparatorText);
            }
        writebuf.Append(arr->MdcaPoint(i));
        }

    iFieldAction->AddCompactCardFieldL(aDestItem, EPbkFieldIdPostalAddress,
		*fielddata);
    
    CleanupStack::PopAndDestroy(2); // fielddata, arr

    return ETrue;
    }

EXPORT_C TBool CBCardEngine::SupportsFieldType(TPbkFieldId aFieldId)
    {
    TBool isSupported = EFalse;
    CBCardExportProperty* property;
    
    // Go trough the array of supported fields and try to find the fieldId
    for (TInt i=0; i < iExportProperties.Count(); i++)
        {
        property = iExportProperties[i];
        for (TInt j=0; j < property->InfoCount(); j++)
            {
            if (property->InfoAt(j)->FieldId() == aFieldId)
                {
                isSupported = ETrue;
                break;
                }
            }
        }
    return isSupported;
    }

// End of File
