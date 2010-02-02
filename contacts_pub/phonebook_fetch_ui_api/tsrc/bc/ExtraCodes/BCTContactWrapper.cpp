/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/








// INCLUDE FILES
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldsInfo.h>
#include "BCTContactWrapper.h"
#include "BCTBCardData.h"

// LOCAL CONSTANTS AND MACROS
_LIT(KSubfieldDelimiter, "|");
_LIT(KDefaulPhoneTag, "<DEFAULTPHONE>");
_LIT(KLabelStartTag, "<LABEL>");
_LIT(KLabelEndTag, "</LABEL>");

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------
//
CBCTContactWrapper::CBCTContactWrapper( 
        CPbkContactItem& aContact, 
        CPbkContactEngine& aContactEngine,
        const TBool aForVCard)
        :iContact(aContact),
         iContactEngine(aContactEngine),
         iForVCard(aForVCard)
    {

    }
// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CBCTContactWrapper::ConstructL()
    {
    }

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
CBCTContactWrapper* CBCTContactWrapper::NewL( 
        CPbkContactItem& aContact, 
        CPbkContactEngine& aContactEngine,
        const TBool aForVCard)
    {
    CBCTContactWrapper* self = new (ELeave) CBCTContactWrapper(aContact,
        aContactEngine,aForVCard);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CBCTContactWrapper::~CBCTContactWrapper()
    {
    
    }

/**
* Get the contact data.
*/ 
void CBCTContactWrapper::GetContactDataL(CBCTBCardData& aData)
    {
    aData.ResetData();
    aData.iLastName = GetDataFieldContentsL(EPbkFieldIdLastName);
    aData.iFirstName = GetDataFieldContentsL(EPbkFieldIdFirstName);
    aData.iPhoneNumberStandard = GetDataFieldContentsL(EPbkFieldIdPhoneNumberStandard);
    aData.iPhoneNumberHome = GetDataFieldContentsL(EPbkFieldIdPhoneNumberHome,EPbkFieldLocationHome);
    aData.iPhoneNumberWork = GetDataFieldContentsL(EPbkFieldIdPhoneNumberWork,EPbkFieldLocationWork);
    aData.iPagerNumber = GetDataFieldContentsL(EPbkFieldIdPagerNumber);
    
    aData.iPhoneNumberMobile = GetDataFieldContentsL(EPbkFieldIdPhoneNumberMobile);
    aData.iPhoneNumberMobileHome = GetDataFieldContentsL(EPbkFieldIdPhoneNumberMobile,EPbkFieldLocationHome);
    aData.iPhoneNumberMobileWork = GetDataFieldContentsL(EPbkFieldIdPhoneNumberMobile,EPbkFieldLocationWork);
        
    aData.iFaxNumber = GetDataFieldContentsL(EPbkFieldIdFaxNumber);
    aData.iFaxNumberHome = GetDataFieldContentsL(EPbkFieldIdFaxNumber,EPbkFieldLocationHome);
    aData.iFaxNumberWork = GetDataFieldContentsL(EPbkFieldIdFaxNumber,EPbkFieldLocationWork);

    aData.iEmailAddress = GetDataFieldContentsL(EPbkFieldIdEmailAddress);
    aData.iEmailAddressHome = GetDataFieldContentsL(EPbkFieldIdEmailAddress,EPbkFieldLocationHome);
    aData.iEmailAddressWork = GetDataFieldContentsL(EPbkFieldIdEmailAddress,EPbkFieldLocationWork);

    aData.iPostalAddress = GetDataFieldContentsL(EPbkFieldIdPostalAddress);
    aData.iPostalAddressHome = GetDataFieldContentsL(EPbkFieldIdPostalAddress,EPbkFieldLocationHome);
    aData.iCompanyAddress = GetDataFieldContentsL(EPbkFieldIdPostalAddress,EPbkFieldLocationWork);
    
    aData.iURL = GetDataFieldContentsL(EPbkFieldIdURL);
    aData.iURLWork = GetDataFieldContentsL(EPbkFieldIdURL,EPbkFieldLocationWork);
    aData.iURLHome = GetDataFieldContentsL(EPbkFieldIdURL,EPbkFieldLocationHome);
    
    aData.iJobTitle = GetDataFieldContentsL(EPbkFieldIdJobTitle);
    aData.iCompanyName = GetDataFieldContentsL(EPbkFieldIdCompanyName);
    aData.iDate = GetDateL(EPbkFieldIdDate);
    aData.iNote = GetDataFieldContentsL(EPbkFieldIdNote);
    aData.iPicture = GetDataFieldContentsL(EPbkFieldIdPicture);
    // this kind of thumbnail retrieving is not in use any more
    //aData.iThumbnailImage = GetDataFieldContentsL(EPbkFieldIdThumbnailImage);
    // ADR general
    aData.iPOBox = GetDataFieldContentsL(EPbkFieldIdPOBox);
    aData.iExtensonAdr = GetDataFieldContentsL(EPbkFieldIdExtendedAddress);
    aData.iStreet = GetDataFieldContentsL(EPbkFieldIdStreetAddress);
    aData.iPostalCode = GetDataFieldContentsL(EPbkFieldIdPostalCode);
    aData.iCity = GetDataFieldContentsL(EPbkFieldIdCity);
    aData.iState = GetDataFieldContentsL(EPbkFieldIdState);
    aData.iCountry = GetDataFieldContentsL(EPbkFieldIdCountry);
    // ADR home
    aData.iPOBoxHome = GetDataFieldContentsL(EPbkFieldIdPOBox,EPbkFieldLocationHome);
    aData.iExtensonAdrHome = GetDataFieldContentsL(EPbkFieldIdExtendedAddress,EPbkFieldLocationHome);
    aData.iStreetHome = GetDataFieldContentsL(EPbkFieldIdStreetAddress,EPbkFieldLocationHome);
    aData.iPostalCodeHome = GetDataFieldContentsL(EPbkFieldIdPostalCode,EPbkFieldLocationHome);
    aData.iCityHome = GetDataFieldContentsL(EPbkFieldIdCity,EPbkFieldLocationHome);
    aData.iStateHome = GetDataFieldContentsL(EPbkFieldIdState,EPbkFieldLocationHome);
    aData.iCountryHome = GetDataFieldContentsL(EPbkFieldIdCountry,EPbkFieldLocationHome);
    // ADR work
    aData.iPOBoxWork = GetDataFieldContentsL(EPbkFieldIdPOBox,EPbkFieldLocationWork);
    aData.iExtensonAdrWork = GetDataFieldContentsL(EPbkFieldIdExtendedAddress,EPbkFieldLocationWork);
    aData.iStreetWork = GetDataFieldContentsL(EPbkFieldIdStreetAddress,EPbkFieldLocationWork);
    aData.iPostalCodeWork = GetDataFieldContentsL(EPbkFieldIdPostalCode,EPbkFieldLocationWork);
    aData.iCityWork = GetDataFieldContentsL(EPbkFieldIdCity,EPbkFieldLocationWork);
    aData.iStateWork = GetDataFieldContentsL(EPbkFieldIdState,EPbkFieldLocationWork);
    aData.iCountryWork = GetDataFieldContentsL(EPbkFieldIdCountry,EPbkFieldLocationWork);
    }

/**
* Set the contact data.
*/ 
void CBCTContactWrapper::SetContactDataL(CBCTBCardData& aData)
    {
    AddDataFieldsL(EPbkFieldIdLastName,aData.iLastName,NULL);
    AddDataFieldsL(EPbkFieldIdFirstName,aData.iFirstName,NULL);    
    AddDataFieldsL(EPbkFieldIdPhoneNumberStandard,aData.iPhoneNumberStandard,NULL);
    AddDataFieldsL(EPbkFieldIdPhoneNumberHome,aData.iPhoneNumberHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdPhoneNumberWork,aData.iPhoneNumberWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdPagerNumber,aData.iPagerNumber,NULL);
    
    AddDataFieldsL(EPbkFieldIdPhoneNumberMobile,aData.iPhoneNumberMobile,NULL);
    AddDataFieldsL(EPbkFieldIdPhoneNumberMobile,aData.iPhoneNumberMobileHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdPhoneNumberMobile,aData.iPhoneNumberMobileWork,NULL,EPbkFieldLocationWork);
    
    AddDataFieldsL(EPbkFieldIdFaxNumber,aData.iFaxNumber,NULL);
    AddDataFieldsL(EPbkFieldIdFaxNumber,aData.iFaxNumberHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdFaxNumber,aData.iFaxNumberWork,NULL,EPbkFieldLocationWork);

    AddDataFieldsL(EPbkFieldIdEmailAddress,aData.iEmailAddress,NULL);
    AddDataFieldsL(EPbkFieldIdEmailAddress,aData.iEmailAddressWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdEmailAddress,aData.iEmailAddressHome,NULL,EPbkFieldLocationHome);

    AddDataFieldsL(EPbkFieldIdPostalAddress,aData.iPostalAddress,NULL);
    AddDataFieldsL(EPbkFieldIdPostalAddress,aData.iPostalAddressHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdPostalAddress,aData.iCompanyAddress,NULL,EPbkFieldLocationWork);

    AddDataFieldsL(EPbkFieldIdURL,aData.iURL,NULL);
    AddDataFieldsL(EPbkFieldIdURL,aData.iURLHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdURL,aData.iURLWork,NULL,EPbkFieldLocationWork);

    AddDataFieldsL(EPbkFieldIdJobTitle,aData.iJobTitle,NULL);
    AddDataFieldsL(EPbkFieldIdCompanyName,aData.iCompanyName,NULL);
    SetDateL(EPbkFieldIdDate,aData.iDate);    
    AddDataFieldsL(EPbkFieldIdNote,aData.iNote,NULL);

 	SetThumbnailValueL(aData.iThumbnailImage);

    // ADR general
    AddDataFieldsL(EPbkFieldIdPOBox,aData.iPOBox,NULL);
    AddDataFieldsL(EPbkFieldIdExtendedAddress,aData.iExtensonAdr,NULL);
    AddDataFieldsL(EPbkFieldIdStreetAddress,aData.iStreet,NULL);
    AddDataFieldsL(EPbkFieldIdPostalCode,aData.iPostalCode,NULL);
    AddDataFieldsL(EPbkFieldIdCity,aData.iCity,NULL);
    AddDataFieldsL(EPbkFieldIdState,aData.iState,NULL);
    AddDataFieldsL(EPbkFieldIdCountry,aData.iCountry,NULL);
    // ADR home
    AddDataFieldsL(EPbkFieldIdPOBox,aData.iPOBoxHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdExtendedAddress,aData.iExtensonAdrHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdStreetAddress,aData.iStreetHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdPostalCode,aData.iPostalCodeHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdCity,aData.iCityHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdState,aData.iStateHome,NULL,EPbkFieldLocationHome);
    AddDataFieldsL(EPbkFieldIdCountry,aData.iCountryHome,NULL,EPbkFieldLocationHome);
    // ADR work
    AddDataFieldsL(EPbkFieldIdPOBox,aData.iPOBoxWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdExtendedAddress,aData.iExtensonAdrWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdStreetAddress,aData.iStreetWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdPostalCode,aData.iPostalCodeWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdCity,aData.iCityWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdState,aData.iStateWork,NULL,EPbkFieldLocationWork);
    AddDataFieldsL(EPbkFieldIdCountry,aData.iCountryWork,NULL,EPbkFieldLocationWork);
    }

void CBCTContactWrapper::SetThumbnailValueL(const HBufC* aImageFileName)
    {
    if (aImageFileName)
        {
        SaveImageFileToContactL(iContact,*aImageFileName);
        }
    }



TBool CBCTContactWrapper::SaveImageFileToContactL(CPbkContactItem& aItem, const TDesC& aFilename)
    {
    CPbkFieldInfo* field = iContactEngine.FieldsInfo().Find(EPbkFieldIdThumbnailImage);
    if (field)
        {
        switch (field->FieldStorageType())    
            {
            case KStorageTypeText:
                {
                // store imagename as link                
                TPbkContactItemField* itemField = aItem.AddOrReturnUnusedFieldL(*field);
		        
                CContactTextField* tf = itemField->ItemField().TextStorage();
	            tf->SetTextL(aFilename);
                break;
                }
            case KStorageTypeStore:
                {
                // store image using manager
                SaveEmbeddedImageToContactL(*field, aItem, aFilename);
                break;
                }
            default:
                {
                return EFalse;
                break;
                }
            }
        }

    return ETrue;    
    }


void CBCTContactWrapper::SaveEmbeddedImageToContactL(
        CPbkFieldInfo& aFieldInfo,
        CPbkContactItem& aItem, 
        const TDesC& aFilename)
    {
    // read imagefile to buffer    
    HBufC8* buffer = ReadImageFileToDescL(aFilename);
    CleanupStack::PushL(buffer);
    
    TPbkContactItemField* field = aItem.AddOrReturnUnusedFieldL(aFieldInfo);    
    
    if (!field)
        {
        User::Leave(KErrNotFound);
        }
    
    CContactStoreField* store = field->ItemField().StoreStorage();
    store->SetThingL(*buffer);

    CleanupStack::PopAndDestroy(buffer);
    }


HBufC8* CBCTContactWrapper::ReadImageFileToDescL(const TDesC& aFilename)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);    
    
    RFile file;

    TInt err = file.Open(fs, aFilename, EFileRead);
    if (err != KErrNone)
        {
        if (err == KErrNotFound)
            {
            // 
            HBufC8* empty = HBufC8::NewL(1);
            CleanupStack::PopAndDestroy(); // fs
            return empty;
            }
        else
            {
            User::Leave(err);
            }
        }

    CleanupClosePushL(file);    

	TInt filelen;
	file.Size(filelen);

    HBufC8* imageBuffer = HBufC8::NewL(filelen);
    CleanupStack::PushL(imageBuffer);

    TPtr8 imabeBufferPtr = imageBuffer->Des();
    User::LeaveIfError(file.Read(imabeBufferPtr));
    
    CleanupStack::Pop(imageBuffer);
    CleanupStack::PopAndDestroy(2); // fs, file
    
    return imageBuffer;
    }




/**
* Find the given contact item.
*/ 
TPbkContactItemField* CBCTContactWrapper::FindFieldForModificationL(
	    CPbkContactItem& aItem,
	    const TPbkFieldId aFieldId,
        const TPbkFieldLocation aLocation)
    {
    CPbkFieldInfo* info = iContactEngine.FieldsInfo().Find(aFieldId,aLocation);    
    if (info)
        {
        return aItem.AddOrReturnUnusedFieldL(*info);
        }
    else
        {
        return NULL;
        }
    }

/**
* Get the maxlength of field.
*/ 
TInt CBCTContactWrapper::FindFieldMaximumLength(TPbkFieldId aFieldId)
    {
	CPbkFieldInfo *fi = iContactEngine.FieldsInfo().Find(aFieldId);
	if(fi)
        {
		return fi->MaxLength();
        }
	else
        {
		return 0;
        }
    }

/**
* Add data fields.
*/ 
void CBCTContactWrapper::AddDataFieldsL(
        TPbkFieldId aFieldId,
	    const TDesC* aDataLine,
        const TDesC* aLabel,
        const TPbkFieldLocation aLocation)    
    {
    if (!aDataLine)
        {
        return;
        }
    
    HBufC* line = HBufC::NewLC(aDataLine->Length());
    *line = *aDataLine;
    TInt pos = 0;
    TInt length;
    TInt startPos = 0;
    do
        {
        pos = line->Find(KSubfieldDelimiter);
        if (pos > 0)
            {
            length = pos;
            }
        else
            {
            length = line->Length();
            }

        HBufC* tmpHeap = HBufC::NewLC(length); 
        *tmpHeap = line->Mid(startPos,length);
        TPtr tmp = tmpHeap->Des();
        
        // Check if defined as default phone number
        TBool defaultPhone =EFalse;
        TInt tagPos = tmp.Find(KDefaulPhoneTag);
        if (tagPos > 0)
            {
            tmp.Replace(tagPos,KDefaulPhoneTag().Length(),KNullDesC );
            defaultPhone = ETrue;
            }

        AddFieldWithValueL(aFieldId,aLocation,tmpHeap,defaultPhone,aLabel);
        CleanupStack::PopAndDestroy(tmpHeap);
        if (pos > 0)
            {
            *line = line->Mid(pos + KSubfieldDelimiter().Length());
            }
        }
    while (pos > 0);    
    CleanupStack::PopAndDestroy(line);
    }


/**
* Add datafield with value.
*/ 
TBool CBCTContactWrapper::AddFieldWithValueL(
	TPbkFieldId aFieldId,
    const TPbkFieldLocation aLocation,
	HBufC* aValue,
	TBool aPhoneNumber,
    const TDesC* aLabel)
    {
	TPbkContactItemField* f;
    
    if (!aValue)
    {
        return EFalse;
    }

	f = FindFieldForModificationL(iContact, aFieldId,aLocation);
	if(f)
	    {
		// Copy the property value
		HBufC *buf = HBufC::NewL(aValue->Length());
		CleanupStack::PushL(buf);
		TPtr buffer = buf->Des();
        buffer = *aValue;

		// Clip the property value if it does not fit the field
		TInt maxlen = FindFieldMaximumLength(f->FieldInfo().FieldId());
		if(maxlen)
            {
			if(maxlen < aValue->Length())
				buffer.SetLength(maxlen);
            }

		CContactTextField* tf = f->ItemField().TextStorage();
	    tf->SetTextL(buffer);
		if(aPhoneNumber)
            {
			// If this is a preferred phone number set field as DefaultPhoneNumber
			if(!iContact.DefaultPhoneNumberField())
				iContact.SetDefaultPhoneNumberFieldL(f);
            }

        if(aLabel)
            {
            f->SetLabelL(*aLabel);
            }
		CleanupStack::PopAndDestroy(); // buf
		return ETrue;
    	}
	return EFalse;
    }


/**
* Check if given field is defaultphonefield.
*/ 
TBool CBCTContactWrapper::IsDefaultPhone(const TPbkContactItemField* aField) const 
    {
    if (!aField)
        {
        return EFalse;        
        }

    TPbkContactItemField* defPhone = iContact.DefaultPhoneNumberField();
    if (defPhone)
        {
        if (defPhone == aField)
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        return EFalse;
        }
    }
/**
* Get the value of the defaultphone.
*/ 
HBufC* CBCTContactWrapper::GetDefaultPhoneL()
    {

    TPbkContactItemField* defPhone = iContact.DefaultPhoneNumberField();
    if (defPhone)
        {
        return GetFieldTextL(defPhone);
        }
    else
        {
        return NULL;
        }
    }

/**
* Get date value.
*/ 
HBufC* CBCTContactWrapper::GetDateL(const TPbkFieldId aId)
    {
    TPbkContactItemField* field;	
    field= iContact.FindField(aId);
    
    if (!field)
        {
        return NULL;
        }

	TVersitDateTime* vdt = 
		new (ELeave) TVersitDateTime(field->ItemField().DateTimeStorage()->Time().DateTime(), 
		TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(vdt);

    TDateTime dateval = vdt->iDateTime;
	CleanupStack::PopAndDestroy(vdt);

	TInt year = dateval.Year();
	TInt month = MonthToNumber(dateval.Month());	
    // Note: 'day' is zero based, must add 1
    TInt day = dateval.Day() + 1;
    
    TInt KMaxLen = 8;
    HBufC* textHeap = HBufC::NewLC(KMaxLen);
    TPtr text = textHeap->Des();
    
    text.Num(year);
    if (month < 10)
        {
        text.AppendNum(0);    
        }
    text.AppendNum(month);
    
    if (day < 10)
        {
        text.AppendNum(0);    
        }    
    text.AppendNum(day);
    
	CleanupStack::Pop(textHeap);
    return textHeap;
    }

/**
* Set datefield.
*/ 
void CBCTContactWrapper::SetDateL(const TPbkFieldId aId, const TDesC* aDataLine)
    {
    if (!aDataLine)
        {
        return;
        }

	TPbkContactItemField* f;
	f = FindFieldForModificationL(iContact, aId, EPbkFieldLocationNone);
	if(f) 
        {
        TVersitDateTime* date = GetVersitDateTimeL(aDataLine);
        if (date)
            {
            f->ItemField().DateTimeStorage()->SetTime(date->iDateTime);
		    delete date;
            }
        }
    }

/**
* Get versit datetime from the given text.
*/ 
TVersitDateTime* CBCTContactWrapper::GetVersitDateTimeL(const TDesC* aDataText)
    {
    if (!aDataText)
        {
        return NULL;
        }

    TBuf<4> year;
    TBuf<2> month;
    TBuf<2> day;

    year = aDataText->Left(4);
    month = aDataText->Mid(4,2);
    day = aDataText->Mid(6,2);

    TLex yearlex(year);
    TInt yearint;
    if (yearlex.Val(yearint) != KErrNone )
        {
        return NULL;
        }

    TLex monthlex(month);
    TInt monthint;
    if (monthlex.Val(monthint) != KErrNone )
        {
        return NULL;
        }

    TLex daylex(day);
    TInt dayint;
    if (daylex.Val(dayint) != KErrNone )
        {
        return NULL;
        }
    // Note: 'day' is zero based, must decrease 1
    dayint = dayint - 1;

    TDateTime dateTime(yearint,NumberToMonth(monthint),dayint,00,00,00,000000);
	TVersitDateTime* vdt = 
		new (ELeave) TVersitDateTime(dateTime,TVersitDateTime::EIsMachineLocal);
    
    return vdt;
    }

/**
* Confert number to month.
*/ 
TMonth CBCTContactWrapper::NumberToMonth(const TInt aNumber) const
    {
    switch (aNumber)
        {        
        case 1:
            {
            return EJanuary;
            }
        case 2:
            {
            return EFebruary;
            }
        case 3:
            {
            return EMarch;
            }
        case 4:
            {
            return EApril;
            }
        case 5:
            {
            return EMay;
            }
        case 6:
            {
            return EJune;
            }
        case 7:
            {
            return EJuly;
            }
        case 8:
            {
            return EAugust;
            }
        case 9:
            {
            return ESeptember;
            }
        case 10:
            {
            return EOctober;
            }
        case 11:
            {
            return ENovember;
            }
        case 12:
            {
            return EDecember;
            }
        default:
            {            
            _LIT(KPanicText, "CBCTContactWrapper::MonthToNumber");
            const TInt KPanicCode = 1;
            User::Panic(KPanicText, KPanicCode);
            return EDecember;
            }
        }
    }

/**
* Convert month to number.
*/ 
TInt CBCTContactWrapper::MonthToNumber(const TMonth aMonth) const
    {
    switch (aMonth)
        {        
        case EJanuary:
            {
            return 1;
            }
        case EFebruary:
            {
            return 2;
            }
        case EMarch:
            {
            return 3;
            }

        case EApril:
            {
            return 4;
            }
        case EMay:
            {
            return 5;
            }
        case EJune:
            {
            return 6;
            }
        case EJuly:
            {
            return 7;
            }
        case EAugust:
            {
            return 8;
            }
        case ESeptember:
            {
            return 9;
            }
        case EOctober:
            {
            return 10;
            }
        case ENovember:
            {
            return 11;
            }
        case EDecember:
            {
            return 12;
            }
        default:
            {
            return 0;
            }
        }
    }

/**
* Get the fieldtext of given field.
*/ 
HBufC* CBCTContactWrapper::GetFieldTextL(const TPbkContactItemField* aField)
    {
	if(!aField)
        {
		return NULL;
        }
    
    TBool addDefaultPhone = EFalse;
    TBool addLabel = EFalse;

    TInt textLength = aField->Text().Length();
        
    if (textLength > 0)
        {
        // check if default phone
        if (IsDefaultPhone(aField))
            {
            addDefaultPhone = ETrue;
            textLength += KDefaulPhoneTag().Length();
            }
    
        // Add labels only if dealing with Compact business cards
        if (!iForVCard)
            {
            TInt labelLength = aField->Label().Length();
            if (labelLength > 0 )
                {
                addLabel = ETrue;
                textLength += labelLength + KLabelStartTag().Length()
                     + KLabelEndTag().Length();
                }
            }
        }
        
    HBufC* textHeap = HBufC::NewL(textLength);
    CleanupStack::PushL(textHeap);
    TPtr text = textHeap->Des();
    text.Copy(aField->Text());
    
    // add labeltext
    if (addLabel)
        {
        text.Append(KLabelStartTag);
        text.Append(aField->Label());
        text.Append(KLabelEndTag);
        }

    // process possible default phone number
    if (addDefaultPhone)
        {
        text.Append(KDefaulPhoneTag);
        }

    CleanupStack::Pop(textHeap);
    return textHeap;
    }

/**
* Get datafield contents.
*/ 
HBufC* CBCTContactWrapper::GetDataFieldContentsL(
        const TPbkFieldId aId, const TPbkFieldLocation aLocation)
    {
    TInt index = 0;
    const TInt KMaxLen = 1000;

    CPbkFieldInfo* fieldInfo = iContactEngine.FieldsInfo().Find(aId, aLocation);        
    if (!fieldInfo)
        {
        return NULL;
        }

    TPbkContactItemField* field;	
    
    field= iContact.FindField(*fieldInfo,index);
    HBufC* mainfield = GetFieldTextL(field); 
    
    if (mainfield)
        {
        CleanupStack::PushL(mainfield);    
        HBufC* tmpHeap = HBufC::NewLC(KMaxLen);
        TPtr tmp = tmpHeap->Des();
        tmp = *mainfield;

        HBufC* addText;
        while (index >= 0)
            {
            index++;
            field = iContact.FindField(*fieldInfo,index);
            addText = GetFieldTextL(field);
            if (addText)
                {
                tmp.Append(KSubfieldDelimiter);
                tmp.Append(*addText);
                delete addText;
                addText = NULL;
                }
            }
        HBufC* data = HBufC::NewL(tmpHeap->Length()); 
        *data = *tmpHeap;
        CleanupStack::PopAndDestroy(tmpHeap);    
        CleanupStack::PopAndDestroy(mainfield);    
        return data;
        }
    else
        {
        return mainfield;        
        }
  }
