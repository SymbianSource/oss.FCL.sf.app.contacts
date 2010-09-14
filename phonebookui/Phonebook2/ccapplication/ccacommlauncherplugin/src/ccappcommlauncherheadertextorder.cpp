/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the class which takes care of placing the correct texts in the header
*
*/

#include "ccappcommlauncherheaders.h"
#include "cmscontact.h"
#include <Pbk2PresentationUtils.h>
#include <pbk2presentation.rsg>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>

const TInt KContactTypeNotSet = -1;

namespace {

/// These enumerations must have same values as in the specification
/// and in the qtn_phob_name_order. These are different compared to
/// TPbk2NameOrderInCenRep because TPbk2NameOrderInCenRep values
/// must be same as in old PbkEng TPbkNameOrder to keep data compatibility
/// of the CenRep.
enum TPbk2NameOrderInUiSpecification
    {
    EPbk2UiSpecFirstNameLastName,
    EPbk2UiSpecLastNameFirstName
    };

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicInvalidNameConfiguration
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2SortOrderManagerImpl");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

/**
 * Gets a digit from a descriptor.
 *
 * @param aReaderToBuf  Resource reader pointed to a descriptor resource.
 * @return  The digit.
 */
TInt GetDigitFromDescriptorL( TResourceReader& aReaderToBuf )
    {
    HBufC* orderBuf = aReaderToBuf.ReadHBufCL();
    CleanupStack::PushL( orderBuf );

    // The setting should be 1 digit
    __ASSERT_DEBUG( orderBuf->Length() == 1,
        Panic( EPanicInvalidNameConfiguration ) );

    TInt result = KErrNotFound;
    TPtrC ptr( orderBuf->Des() );
    if ( ptr.Length() > 0 && TChar(ptr[0]).IsDigit() )
        {
        result = TChar(ptr[0]).GetNumericValue();
        }

    CleanupStack::PopAndDestroy( orderBuf );
    return result;
    }
} // namespace

// ---------------------------------------------------------------------------
// NewL, two-phase construction
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderTextOrder* CCCAppCommLauncherHeaderTextOrder::NewL()
    {
    CCCAppCommLauncherHeaderTextOrder* self= new (ELeave) CCCAppCommLauncherHeaderTextOrder();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// C++ (first phase) constructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderTextOrder::CCCAppCommLauncherHeaderTextOrder()
    {
    for (TInt i=0; i < KCCAppCommLauncherHeaderTextOrderLastField - KCCAppCommLauncherHeaderTextOrderFirstField + 1; i++)
        {
        iGroupPriorities[i] = NULL;
        iIndividualPriorities[i] = NULL;
        }
    
    // Group 8
    SetPriority(CCmsContactFieldItem::ECmsImpp, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsUrlGeneric, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsUrlHome, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsUrlWork, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsSipAddress, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsEmailGeneric, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsEmailHome, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsEmailWork, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneGeneric, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneHome, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneWork, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneGeneric, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneHome, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneWork, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberGeneric, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberHome, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberWork, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsAssistantNumber, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsCarPhone, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsPagerNumber, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsPushToTalk, ETrue, 1);
    SetPriority(CCmsContactFieldItem::ECmsShareView, ETrue, 1);
    
    SetPriority(CCmsContactFieldItem::ECmsVoipNumberGeneric, ETrue, 1);
		
    SetPriority(CCmsContactFieldItem::ECmsImpp, EFalse, 1);

    SetPriority(CCmsContactFieldItem::ECmsUrlWork, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsUrlHome, EFalse, 3);
    SetPriority(CCmsContactFieldItem::ECmsUrlGeneric, EFalse, 4);
    
    SetPriority(CCmsContactFieldItem::ECmsSipAddress, EFalse, 5);
    SetPriority(CCmsContactFieldItem::ECmsShareView, EFalse, 6);
    SetPriority(CCmsContactFieldItem::ECmsPushToTalk, EFalse, 7);
        
    SetPriority(CCmsContactFieldItem::ECmsEmailWork, EFalse, 8);
    SetPriority(CCmsContactFieldItem::ECmsEmailHome, EFalse, 9);
    SetPriority(CCmsContactFieldItem::ECmsEmailGeneric, EFalse, 10);
    
    SetPriority(CCmsContactFieldItem::ECmsAssistantNumber, EFalse, 11);
    SetPriority(CCmsContactFieldItem::ECmsPagerNumber, EFalse, 12);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberWork, EFalse, 13);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberHome, EFalse, 14);
    SetPriority(CCmsContactFieldItem::ECmsVideoNumberGeneric, EFalse, 15);
    SetPriority(CCmsContactFieldItem::ECmsCarPhone, EFalse, 16);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneWork, EFalse, 17);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneHome, EFalse, 18);
    SetPriority(CCmsContactFieldItem::ECmsLandPhoneGeneric, EFalse, 19);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneWork, EFalse, 20);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneHome, EFalse, 21);
    SetPriority(CCmsContactFieldItem::ECmsMobilePhoneGeneric, EFalse, 22);
    
    
    // Group 7
    SetPriority(CCmsContactFieldItem::ECmsLastName, ETrue, 2);
    SetPriority(CCmsContactFieldItem::ECmsMiddleName, ETrue, 2);
    SetPriority(CCmsContactFieldItem::ECmsFirstName, ETrue, 2);
    
    SetPriority(CCmsContactFieldItem::ECmsLastName, EFalse, 1);
    SetPriority(CCmsContactFieldItem::ECmsMiddleName, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsFirstName, EFalse, 3);
    
    
    // Group 6
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryGeneric, ETrue, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionGeneric, ETrue, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalGeneric, ETrue, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetGeneric, ETrue, 3);
    
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryGeneric, EFalse, 1);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionGeneric, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalGeneric, EFalse, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetGeneric, EFalse, 4);
    
    
    // Group 5
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryWork, ETrue, 4);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionWork, ETrue, 4);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalWork, ETrue, 4);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetWork, ETrue, 4);
       
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryWork, EFalse, 1);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionWork, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalWork, EFalse, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetWork, EFalse, 4);

    
    // Group 4
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryHome, ETrue, 5);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionHome, ETrue, 5);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalHome, ETrue, 5);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetHome, ETrue, 5);
    
    SetPriority(CCmsContactFieldItem::ECmsAddrCountryHome, EFalse, 1);
    SetPriority(CCmsContactFieldItem::ECmsAddrRegionHome, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsAddrLocalHome, EFalse, 3);
    SetPriority(CCmsContactFieldItem::ECmsAddrStreetHome, EFalse, 4);
    
    
    // Group 3 (prio 6)
    SetPriority(CCmsContactFieldItem::ECmsDepartment, ETrue, 6);
    SetPriority(CCmsContactFieldItem::ECmsCompanyName, ETrue, 6);
    SetPriority(CCmsContactFieldItem::ECmsJobTitle, ETrue, 6);
    SetPriority(CCmsContactFieldItem::ECmsNickname, ETrue, 6);
 
    SetPriority(CCmsContactFieldItem::ECmsDepartment, EFalse, 1);
    SetPriority(CCmsContactFieldItem::ECmsCompanyName, EFalse, 2);
    SetPriority(CCmsContactFieldItem::ECmsJobTitle, EFalse, 3);
    SetPriority(CCmsContactFieldItem::ECmsNickname, EFalse, 4);
    
    
    // Group 1 (prio 7)
    SetPriority(CCmsContactFieldItem::ECmsNote, ETrue, 7);
    
    SetPriority(CCmsContactFieldItem::ECmsNote, EFalse, 1);
    
    }

// ---------------------------------------------------------------------------
// Helper function for reading a descriptor from repository
// ---------------------------------------------------------------------------
//
HBufC* GetStringFromRepoLC(CRepository* aRepository, TUint32 aKey)
    {
    HBufC* prioStr = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TInt trueLength = 0;
    TPtr ptr = prioStr->Des();
    TInt error = aRepository->Get(aKey, ptr, trueLength);
    if (error == KErrOverflow)
        {
        CleanupStack::PopAndDestroy(prioStr);
        prioStr =  HBufC::NewLC(trueLength);
        User::LeaveIfError(aRepository->Get(aKey, ptr));
        }
    else if (error != KErrNone)
        {
#ifdef _DEBUG 
        CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::getStringFromRepoLC - Header text priority variation error (repository field)"));   
        CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::getStringFromRepoLC - err: %d"), error );
        CEikonEnv::Static()->InfoMsg(_L("Header text priority variation error. See logs."));
#endif//_DEBUG
        CleanupStack::PopAndDestroy(prioStr);
        return 0;
        }
    return prioStr;
    }

// ---------------------------------------------------------------------------
// ConstructL, second phase constructor
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::ConstructL()
    {
    for (TInt i = 0; i < KCCAppCommLauncherHeaderTextOrderLineCount; i++)
        {
        iHeaderRows.AppendL(0);
        iHeaderRowsClipped.AppendL(0);
        iHeaderTypes[i] = KContactTypeNotSet;
        }
    
    CRepository* prioOrderRepo = CRepository::NewLC(KCRUidCCACommLauncher);
    HBufC* prioStr = 0;
    
    HBufC* prioStr1 = GetStringFromRepoLC(prioOrderRepo, KHeaderGroupPriorities);
    User::LeaveIfNull(prioStr1);
        
    HBufC* prioStr2 = GetStringFromRepoLC(prioOrderRepo, KHeaderGroupPriorities2);
    if (prioStr2)
        {
        prioStr = HBufC::NewL(prioStr1->Length() + prioStr2->Length());
        prioStr->Des().Append(*prioStr1);
        prioStr->Des().Append(*prioStr2);
        CleanupStack::PopAndDestroy(prioStr2);
        CleanupStack::PopAndDestroy(prioStr1);
        CleanupStack::PushL(prioStr);
        }
    else
        {
        prioStr = prioStr1;
        }
    
    
    TLex groupPrioLex(*prioStr);
    while (groupPrioLex.Offset() < prioStr->Length() - 1)
        {
        TPtrC nameToken = groupPrioLex.NextToken();
        CCmsContactFieldItem::TCmsContactField type;
        if (MapStringToContactField(nameToken, type) == KErrNone)
            {
            TPtrC numToken = groupPrioLex.NextToken();
            TPtrC numToken2 = groupPrioLex.NextToken();
            TLex groupPrioNumLex(numToken);
            TLex individualPrioNumLex(numToken2);
            TInt priority = 0;
            if (groupPrioNumLex.Val(priority) == KErrNone)
                {
                SetPriority(type, ETrue, priority);
                }
#ifdef _DEBUG_
            else
                {
                CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::ConstructL - Priority value is not a number: %S"), &numToken );
                CEikonEnv::Static()->InfoMsg(_L("Header text priority variation error. See logs."));
                }
#endif //_DEBUG_
            if (individualPrioNumLex.Val(priority) == KErrNone)
                {
                SetPriority(type, EFalse, priority);
                }
#ifdef _DEBUG_
            else
                {
                CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::ConstructL - Priority value is not a number: %S"), &numToken );
                CEikonEnv::Static()->InfoMsg(_L("Header text priority variation error. See logs."));
                }
#endif //_DEBUG_
            }
#ifdef _DEBUG 
        else
            {
            CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::ConstructL - Header text priority variation, field name not recognized)"));   
            CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherHeaderTextOrder::ConstructL - field name: %S"), &nameToken );
            }
#endif //_DEBUG
        }
    CleanupStack::PopAndDestroy(prioStr);
    
    CleanupStack::PopAndDestroy(prioOrderRepo);
    
    // Open resource file for reading language specific default values
    RPbk2LocalizedResourceFile resFile;
    resFile.OpenLC( KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, 
        Pbk2PresentationUtils::PresentationResourceFile() );
    
    // Read the name order value from the resource file
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( R_QTN_PHOB_NAME_ORDER ) );
    TInt orderInResFile = GetDigitFromDescriptorL( reader );
    CleanupStack::PopAndDestroy(); // reader
    
    CleanupStack::PopAndDestroy(); // resFile
    
    // Converting from TPbk2NameOrderInUiSpecification to TPbk2NameOrderInCenRep
    TInt orderInCenRep = EPbk2CenRepNameOrderUndefined;

    CRepository* sortOrderSettings = CRepository::NewLC(TUid::Uid(KCRUidPhonebook));
    sortOrderSettings->Get(KPhonebookNameOrdering, orderInCenRep);
    
    switch ( orderInResFile )
        {
        case EPbk2UiSpecLastNameFirstName:
            {
            orderInCenRep = EPbk2CenRepLastNameFirstName;
            break;
            }
            
        case EPbk2UiSpecFirstNameLastName: // FALL THROUGH
            
        // If the name order is not defined in the UI Specification,
        // and...
        default:
            {
            // ... also not defined in the CenRep,
            // then use the "First Name" "Last Name" order
            // for most number of languages.
            if ( EPbk2CenRepNameOrderUndefined == orderInCenRep )
                {
                orderInCenRep = EPbk2CenRepFirstNameLastName;
                }
            break;
            }
            
        } // end switch

    if (orderInCenRep == EPbk2CenRepLastNameFirstName)
        {
        SetPriority(CCmsContactFieldItem::ECmsMiddleName, EFalse, 1);
        SetPriority(CCmsContactFieldItem::ECmsFirstName, EFalse, 2);
        SetPriority(CCmsContactFieldItem::ECmsLastName, EFalse, 3);
        }
    else
        {
        SetPriority(CCmsContactFieldItem::ECmsFirstName, EFalse, 3);
        SetPriority(CCmsContactFieldItem::ECmsMiddleName, EFalse, 2);
        SetPriority(CCmsContactFieldItem::ECmsLastName, EFalse, 1);
        }
        
    CleanupStack::PopAndDestroy(sortOrderSettings);
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherHeaderTextOrder::~CCCAppCommLauncherHeaderTextOrder()
    {
    iHeaderRows.ResetAndDestroy();
    iHeaderRowsClipped.ResetAndDestroy();
    delete iCurrentDbName;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::SetPriority
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::SetPriority(CCmsContactFieldItem::TCmsContactField aContactField, TBool aGroup, TInt aPriority)
    {
    if (aGroup)
        {
        iGroupPriorities[aContactField -  KCCAppCommLauncherHeaderTextOrderFirstField] = aPriority;
        }
    else
        {
        iIndividualPriorities[aContactField -  KCCAppCommLauncherHeaderTextOrderFirstField] = aPriority;
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::GetPriority
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherHeaderTextOrder::GetPriority(CCmsContactFieldItem::TCmsContactField aContactField, TBool aGroup)
    {
    if (aGroup)
        {
        return iGroupPriorities[aContactField -  KCCAppCommLauncherHeaderTextOrderFirstField];
        }
    else
        {
        return iIndividualPriorities[aContactField -  KCCAppCommLauncherHeaderTextOrderFirstField];
        }
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::GetTextForRow
// ---------------------------------------------------------------------------
//
const TDesC& CCCAppCommLauncherHeaderTextOrder::GetTextForRow(TInt aRow)
    {
    if (aRow >= 0 && aRow < iHeaderRowsClipped.Count() && aRow < iHeaderRows.Count())
        {
        if (iHeaderRowsClipped[aRow])
            {
            TPtr ptr(iHeaderRowsClipped[aRow]->Des());
            // Replace characters that can not be displayed correctly.
            Pbk2PresentationUtils::ReplaceNonGraphicCharacters(ptr, ' ');
            return *iHeaderRowsClipped[aRow];
            }
        else if (iHeaderRows[aRow])
            {
            TPtr ptr(iHeaderRows[aRow]->Des());
            Pbk2PresentationUtils::ReplaceNonGraphicCharacters(ptr, ' ');
            return *iHeaderRows[aRow];
            }
        }
    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::SelectContactFieldL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::SelectContactFieldL(
    const CCmsContactField& aContactField, 
    TInt aTopGroupPriority)
    {
    CCmsContactFieldItem::TCmsContactField type = aContactField.Type();    
    const RPointerArray<CCmsContactFieldItem>& fields = aContactField.Items();    

    for (TInt i=0; i < fields.Count(); i++)
        {
        for (TInt i2 = 0; i2 < KCCAppCommLauncherHeaderTextOrderLineCount; i2++)
            {
            if ( GetPriority(type, ETrue) == aTopGroupPriority &&
                      (iHeaderTypes[i2] < 0 || GetPriority(type, EFalse) > GetPriority((CCmsContactFieldItem::TCmsContactField)iHeaderTypes[i2], EFalse)) )
                {
                if(StrippedLengthL(*fields[i])) //Skip blank fields 
                    {
                    TInt typeMem = KContactTypeNotSet;
                    for (TInt i3 = i2; i3 < KCCAppCommLauncherHeaderTextOrderLineCount; i3++)
                        {
                        TInt typeTemp = iHeaderTypes[i3];
                        iHeaderTypes[i3] = typeMem;
                        typeMem = typeTemp;
                        }
                    iHeaderTypes[i2] = type;  
                    break;                    
                    }
                }
            }            
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::ProcessContactFieldsL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::ProcessContactFieldsL(
    RArray<CCmsContactField> aCmsContactFields)
    {
    TInt topGroupPriority = 0;
    delete iCurrentDbName;
    iCurrentDbName = NULL;
    TInt count = aCmsContactFields.Count();
    TInt i;
    
    //1. Get the topmost group of fields containing displayable sdata
    for(i=0; i < count ; i++ )
        {
        for(TInt j=0; j < aCmsContactFields[i].Items().Count(); j++ )
            {
            CCmsContactFieldItem::TCmsContactField type = aCmsContactFields[i].Type();
            if(StrippedLengthL(aCmsContactFields[i].ItemL(j)))
                {
                if ( GetPriority(type, ETrue) > topGroupPriority )
                    {
                    topGroupPriority = GetPriority(type, ETrue);  
                    }
                }
            }
        }

    //2. Select preferred fields from top group     
    for(i=0; i < count ; i++ )
        {
        SelectContactFieldL( aCmsContactFields[i], topGroupPriority ); 
        }
    if (KCCAppCommLauncherHeaderTextOrderLineCount > 1 && iHeaderTypes[1] == KContactTypeNotSet)
        {
        iHeadersWrapped = ETrue;
        }

    //3. Process the selected fields
    for(i=0; i < count ; i++ )
        {
        ProcessContactFieldL( aCmsContactFields[i] );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::ProcessContactFieldL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::ProcessContactFieldL(const CCmsContactField& aContactField)
    {
    for (TInt i = 0; i < KCCAppCommLauncherHeaderTextOrderLineCount; i++)
        {
        if (iHeaderRowsClipped[i])
            {
            delete iHeaderRowsClipped[i];
            iHeaderRowsClipped[i] = 0;
            }
        if (aContactField.Type() == iHeaderTypes[i] && !iHeaderRows[i])
            {
            //Check is there anything from current database for this header row[i]
            TInt itemCountForDb = ItemCountForCurrentDbL( aContactField ); 
            if(!itemCountForDb)
                {
                //nothing to show, skip this contact field
                break; 
                }
            
            if (itemCountForDb > 1)
                {
                iHeadersWrapped = EFalse;
                
                HBufC* mem = 0;
                TInt typeMem = KContactTypeNotSet;
                for (TInt i2 = i + itemCountForDb-1; i2 < KCCAppCommLauncherHeaderTextOrderLineCount; i2++)                 
                    {
                    HBufC* temp = iHeaderRows[i2];
                    TInt typeTemp = iHeaderTypes[i2];
                    iHeaderRows[i2] = mem;
                    iHeaderTypes[i2] = typeMem;
                    mem = temp;
                    typeMem = typeTemp;
                    }
                delete mem;
                }
            for (unsigned i2 = i; i2 < KCCAppCommLauncherHeaderTextOrderLineCount && i2-i < itemCountForDb; i2++)             
                {
                //Add only rows from current db
                TPtrC info = aContactField.ItemL(i2-i).Info();
                if( iCurrentDbName && *iCurrentDbName == info ) 
                    {
                    iHeaderRows[i2] = aContactField.ItemL(i2-i).Data().AllocL();
                    iHeaderTypes[i2] = aContactField.Type();
                    }
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::StrippedLengthL
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherHeaderTextOrder::StrippedLengthL(
    const CCmsContactFieldItem& aCmsContactFieldItem)
    {
    HBufC* buf1 = aCmsContactFieldItem.Data().AllocLC();
    TPtr buf1Ptr = buf1->Des();
    buf1Ptr.TrimAll();
    
    DeleteCharL(buf1Ptr, 0x000d); //KCarriageReturn = 0x000d
    DeleteCharL(buf1Ptr, 0x000a); //KLineFeed = 0x000a
    buf1Ptr.TrimAll();            //if any whitespace left after first trim...    

    TInt strippedLength = buf1Ptr.Length();    
    CleanupStack::PopAndDestroy(buf1);    
    return strippedLength;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::ProcessContactFieldsL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::DeleteCharL(TDes& aDest, TChar aChar)
    {
    TBuf<1> str;
    str.Append(aChar);    
    
    TInt delPos = aDest.Find(str);
    while( delPos != KErrNotFound )
        {
        aDest.Delete(delPos, 1);
        delPos = aDest.Find(str);
        }    
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::ItemCountForCurrentDbL
// ---------------------------------------------------------------------------
//
TInt CCCAppCommLauncherHeaderTextOrder::ItemCountForCurrentDbL( 
    const CCmsContactField& aContactField )
    {
    TInt ret(0);
    
    for(TInt i=0; i < aContactField.ItemCount(); i++ )
        {
        if( !iCurrentDbName )
            {
            //From now on accept only data from this database
            iCurrentDbName = aContactField.ItemL(i).Info().AllocL();
            }    

        TPtrC info = aContactField.ItemL(i).Info();
        if( *iCurrentDbName == info ) 
            {
            ret++;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::Reset
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::Reset()
    {
    for (TInt i = 0; i < KCCAppCommLauncherHeaderTextOrderLineCount; i++)
        {
        delete iHeaderRows[i];
        iHeaderRows[i] = NULL;
        delete iHeaderRowsClipped[i];
        iHeaderRowsClipped[i] = NULL;
        iHeaderTypes[i] = KContactTypeNotSet;
        }
    iHeadersWrapped = EFalse;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::ClipL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherHeaderTextOrder::ClipL(const CFont& aFont, TInt aLineWidth)
    {
    iHeaderRowsClipped.ResetAndDestroy();
    if (iHeadersWrapped && iHeaderRows[0])
        {
        HBufC* textTemp = HBufC::NewLC(iHeaderRows[0]->Length() + 1);
        CArrayFixFlat<TInt> * lineWidths = new (ELeave) CArrayFixFlat<TInt>(KCCAppCommLauncherHeaderTextOrderLineCount);
        CleanupStack::PushL(lineWidths);
        lineWidths->AppendL(aLineWidth);
        lineWidths->AppendL(aLineWidth);
        lineWidths->AppendL(aLineWidth);
        CArrayFixFlat<TPtrC>* lines = new (ELeave) CArrayFixFlat<TPtrC>(KCCAppCommLauncherHeaderTextOrderLineCount);
        CleanupStack::PushL(lines);
        TPtr ptr = textTemp->Des();
        ptr.Copy(*iHeaderRows[0]);
        AknTextUtils::WrapToArrayAndClipL(ptr, *lineWidths, aFont, *lines);
        for (TInt i=0; i < KCCAppCommLauncherHeaderTextOrderLineCount; i++)
            {
            if (i < lines->Count())
                {
                iHeaderRowsClipped.AppendL(lines->At(i).AllocL());
                }
            else
                {
                iHeaderRowsClipped.AppendL(KNullDesC().AllocL());
                }
            }
        CleanupStack::PopAndDestroy(3);
        }
    else
        {
        for (TInt i = 0; i < KCCAppCommLauncherHeaderTextOrderLineCount; i++)
            {
            iHeaderRowsClipped.AppendL(0);
            if (iHeaderRows[i])
                {
                AknTextUtils::TClipDirection clipDirection = MapContactFieldToClipDirection((CCmsContactFieldItem::TCmsContactField)iHeaderTypes[i]);
                iHeaderRowsClipped[i] = iHeaderRows[i]->AllocL();
                TPtr ptr = iHeaderRowsClipped[i]->Des();
                AknTextUtils::ClipToFit(ptr, aFont, aLineWidth, clipDirection);
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherHeaderTextOrder::MapStringToContactField
// ---------------------------------------------------------------------------
//
 TInt CCCAppCommLauncherHeaderTextOrder::MapStringToContactField(const TDesC& aDesc, CCmsContactFieldItem::TCmsContactField& aType)
    {
    _LIT(KECmsLastName, "ECmsLastName");
    _LIT(KECmsMiddleName, "ECmsMiddleName");
    _LIT(KECmsFirstName, "ECmsFirstName");
    _LIT(KECmsUndefined, "ECmsUndefined");
    _LIT(KECmsPresenceData, "ECmsPresenceData");
    _LIT(KECmsBrandedAvailability, "ECmsBrandedAvailability");
    _LIT(KECmsThumbnailPic, "ECmsThumbnailPic");
    _LIT(KECmsSipAddress, "ECmsSipAddress");
    _LIT(KECmsLandPhoneHome, "ECmsLandPhoneHome");
    _LIT(KECmsMobilePhoneHome, "ECmsMobilePhoneHome");
    _LIT(KECmsFaxNumberHome, "ECmsFaxNumberHome");
    _LIT(KECmsVideoNumberHome, "ECmsVideoNumberHome");
    _LIT(KECmsVoipNumberHome, "ECmsVoipNumberHome");
    _LIT(KECmsEmailHome, "ECmsEmailHome");
    _LIT(KECmsUrlHome, "ECmsUrlHome");
    _LIT(KECmsLandPhoneWork, "ECmsLandPhoneWork");
    _LIT(KECmsMobilePhoneWork, "ECmsMobilePhoneWork");
    _LIT(KECmsVideoNumberWork, "ECmsVideoNumberWork");
    _LIT(KECmsFaxNumberWork, "ECmsFaxNumberWork");
    _LIT(KECmsVoipNumberWork, "ECmsVoipNumberWork");
    _LIT(KECmsEmailWork, "ECmsEmailWork");
    _LIT(KECmsUrlWork, "ECmsUrlWork");
    _LIT(KECmsEmailGeneric, "ECmsEmailGeneric");
    _LIT(KECmsUrlGeneric, "ECmsUrlGeneric");
    _LIT(KECmsLandPhoneGeneric, "ECmsLandPhoneGeneric");
    _LIT(KECmsMobilePhoneGeneric, "ECmsMobilePhoneGeneric");
    _LIT(KECmsVideoNumberGeneric, "ECmsVideoNumberGeneric");
    _LIT(KECmsAddrLabelGeneric, "ECmsAddrLabelGeneric");
    _LIT(KECmsAddrPOGeneric, "ECmsAddrPOGeneric");
    _LIT(KECmsAddrExtGeneric, "ECmsAddrExtGeneric");
    _LIT(KECmsAddrStreetGeneric, "ECmsAddrStreetGeneric");
    _LIT(KECmsAddrLocalGeneric, "ECmsAddrLocalGeneric");
    _LIT(KECmsAddrRegionGeneric, "ECmsAddrRegionGeneric");
    _LIT(KECmsAddrPostcodeGeneric, "ECmsAddrPostcodeGeneric");
    _LIT(KECmsAddrCountryGeneric, "ECmsAddrCountryGeneric");
    _LIT(KECmsVoipNumberGeneric, "ECmsVoipNumberGeneric");
    _LIT(KECmsAddrLabelHome, "ECmsAddrLabelHome");
    _LIT(KECmsAddrPOHome, "ECmsAddrPOHome");
    _LIT(KECmsAddrExtHome, "ECmsAddrExtHome");
    _LIT(KECmsAddrStreetHome, "ECmsAddrStreetHome");
    _LIT(KECmsAddrLocalHome, "ECmsAddrLocalHome");
    _LIT(KECmsAddrRegionHome, "ECmsAddrRegionHome");
    _LIT(KECmsAddrPostcodeHome, "ECmsAddrPostcodeHome");
    _LIT(KECmsAddrCountryHome, "ECmsAddrCountryHome");
    _LIT(KECmsAddrLabelWork, "ECmsAddrLabelWork");
    _LIT(KECmsAddrPOWork, "ECmsAddrPOWork");
    _LIT(KECmsAddrExtWork, "ECmsAddrExtWork");
    _LIT(KECmsAddrStreetWork, "ECmsAddrStreetWork");
    _LIT(KECmsAddrLocalWork, "ECmsAddrLocalWork");
    _LIT(KECmsAddrRegionWork, "ECmsAddrRegionWork");
    _LIT(KECmsAddrPostcodeWork, "ECmsAddrPostcodeWork");
    _LIT(KECmsAddrCountryWork, "ECmsAddrCountryWork");
    _LIT(KECmsImpp, "ECmsImpp");
    _LIT(KECmsFullName, "ECmsFullName");
    _LIT(KECmsNote, "ECmsNote");
    _LIT(KECmsNickname, "ECmsNickname");
    _LIT(KECmsJobTitle, "ECmsJobTitle");
    _LIT(KECmsDepartment, "ECmsDepartment");
    _LIT(KECmsBirthday, "ECmsBirthday");
    _LIT(KECmsAnniversary, "ECmsAnniversary");
    _LIT(KECmsCompanyName, "ECmsCompanyName");
    _LIT(KECmsAssistantNumber, "ECmsAssistantNumber");
    _LIT(KECmsCarPhone, "ECmsCarPhone");
    _LIT(KECmsPagerNumber, "ECmsPagerNumber");
    _LIT(KECmsPushToTalk, "ECmsPushToTalk");
    _LIT(KECmsShareView, "ECmsShareView");

    
    if (!aDesc.Compare(KECmsLastName))
        aType = CCmsContactFieldItem::ECmsLastName;
    else if (!aDesc.Compare(KECmsMiddleName))
        aType = CCmsContactFieldItem::ECmsMiddleName;
    else if (!aDesc.Compare(KECmsFirstName))
        aType = CCmsContactFieldItem::ECmsFirstName;
    else if (!aDesc.Compare(KECmsUndefined))
        aType = CCmsContactFieldItem::ECmsUndefined;
    else if (!aDesc.Compare(KECmsPresenceData))
        aType = CCmsContactFieldItem::ECmsPresenceData;
    else if (!aDesc.Compare(KECmsBrandedAvailability))
        aType = CCmsContactFieldItem::ECmsBrandedAvailability;
    else if (!aDesc.Compare(KECmsThumbnailPic))
        aType = CCmsContactFieldItem::ECmsThumbnailPic;
    else if (!aDesc.Compare(KECmsSipAddress))
        aType = CCmsContactFieldItem::ECmsSipAddress;
    else if (!aDesc.Compare(KECmsLandPhoneHome))
        aType = CCmsContactFieldItem::ECmsLandPhoneHome;
    else if (!aDesc.Compare(KECmsMobilePhoneHome))
        aType = CCmsContactFieldItem::ECmsMobilePhoneHome;
    else if (!aDesc.Compare(KECmsFaxNumberHome))
        aType = CCmsContactFieldItem::ECmsFaxNumberHome;
    else if (!aDesc.Compare(KECmsVideoNumberHome))
        aType = CCmsContactFieldItem::ECmsVideoNumberHome;
    else if (!aDesc.Compare(KECmsVoipNumberHome))
        aType = CCmsContactFieldItem::ECmsVoipNumberHome;
    else if (!aDesc.Compare(KECmsEmailHome))
        aType = CCmsContactFieldItem::ECmsEmailHome;
    else if (!aDesc.Compare(KECmsUrlHome))
        aType = CCmsContactFieldItem::ECmsUrlHome;
    else if (!aDesc.Compare(KECmsLandPhoneWork))
        aType = CCmsContactFieldItem::ECmsLandPhoneWork;
    else if (!aDesc.Compare(KECmsMobilePhoneWork))
        aType = CCmsContactFieldItem::ECmsMobilePhoneWork;
    else if (!aDesc.Compare(KECmsVideoNumberWork))
        aType = CCmsContactFieldItem::ECmsVideoNumberWork;
    else if (!aDesc.Compare(KECmsFaxNumberWork))
        aType = CCmsContactFieldItem::ECmsFaxNumberWork;
    else if (!aDesc.Compare(KECmsVoipNumberWork))
        aType = CCmsContactFieldItem::ECmsVoipNumberWork;
    else if (!aDesc.Compare(KECmsEmailWork))
        aType = CCmsContactFieldItem::ECmsEmailWork;
    else if (!aDesc.Compare(KECmsUrlWork))
        aType = CCmsContactFieldItem::ECmsUrlWork;
    else if (!aDesc.Compare(KECmsEmailGeneric))
        aType = CCmsContactFieldItem::ECmsEmailGeneric;
    else if (!aDesc.Compare(KECmsUrlGeneric))
        aType = CCmsContactFieldItem::ECmsUrlGeneric;
    else if (!aDesc.Compare(KECmsLandPhoneGeneric))
        aType = CCmsContactFieldItem::ECmsLandPhoneGeneric;
    else if (!aDesc.Compare(KECmsMobilePhoneGeneric))
        aType = CCmsContactFieldItem::ECmsMobilePhoneGeneric;
    else if (!aDesc.Compare(KECmsVideoNumberGeneric))
        aType = CCmsContactFieldItem::ECmsVideoNumberGeneric;
    else if (!aDesc.Compare(KECmsAddrLabelGeneric))
        aType = CCmsContactFieldItem::ECmsAddrLabelGeneric;
    else if (!aDesc.Compare(KECmsAddrPOGeneric))
        aType = CCmsContactFieldItem::ECmsAddrPOGeneric;
    else if (!aDesc.Compare(KECmsAddrExtGeneric))
        aType = CCmsContactFieldItem::ECmsAddrExtGeneric;
    else if (!aDesc.Compare(KECmsAddrStreetGeneric))
        aType = CCmsContactFieldItem::ECmsAddrStreetGeneric;
    else if (!aDesc.Compare(KECmsAddrLocalGeneric))
        aType = CCmsContactFieldItem::ECmsAddrLocalGeneric;
    else if (!aDesc.Compare(KECmsAddrRegionGeneric))
        aType = CCmsContactFieldItem::ECmsAddrRegionGeneric;
    else if (!aDesc.Compare(KECmsAddrPostcodeGeneric))
        aType = CCmsContactFieldItem::ECmsAddrPostcodeGeneric;
    else if (!aDesc.Compare(KECmsAddrCountryGeneric))
        aType = CCmsContactFieldItem::ECmsAddrCountryGeneric;
    else if (!aDesc.Compare(KECmsVoipNumberGeneric))
        aType = CCmsContactFieldItem::ECmsVoipNumberGeneric;
    else if (!aDesc.Compare(KECmsAddrLabelHome))
        aType = CCmsContactFieldItem::ECmsAddrLabelHome;
    else if (!aDesc.Compare(KECmsAddrPOHome))
        aType = CCmsContactFieldItem::ECmsAddrPOHome;
    else if (!aDesc.Compare(KECmsAddrExtHome))
        aType = CCmsContactFieldItem::ECmsAddrExtHome;
    else if (!aDesc.Compare(KECmsAddrStreetHome))
        aType = CCmsContactFieldItem::ECmsAddrStreetHome;
    else if (!aDesc.Compare(KECmsAddrLocalHome))
        aType = CCmsContactFieldItem::ECmsAddrLocalHome;
    else if (!aDesc.Compare(KECmsAddrRegionHome))
        aType = CCmsContactFieldItem::ECmsAddrRegionHome;
    else if (!aDesc.Compare(KECmsAddrPostcodeHome))
        aType = CCmsContactFieldItem::ECmsAddrPostcodeHome;
    else if (!aDesc.Compare(KECmsAddrCountryHome))
        aType = CCmsContactFieldItem::ECmsAddrCountryHome;
    
    else if (!aDesc.Compare(KECmsAddrLabelWork))
        aType = CCmsContactFieldItem::ECmsAddrLabelWork;
    else if (!aDesc.Compare(KECmsAddrPOWork))
        aType = CCmsContactFieldItem::ECmsAddrPOWork;
    else if (!aDesc.Compare(KECmsAddrExtWork))
        aType = CCmsContactFieldItem::ECmsAddrExtWork;
    else if (!aDesc.Compare(KECmsAddrStreetWork))
        aType = CCmsContactFieldItem::ECmsAddrStreetWork;
    else if (!aDesc.Compare(KECmsAddrLocalWork))
        aType = CCmsContactFieldItem::ECmsAddrLocalWork;
    else if (!aDesc.Compare(KECmsAddrRegionWork))
        aType = CCmsContactFieldItem::ECmsAddrRegionWork;
    else if (!aDesc.Compare(KECmsAddrPostcodeWork))
        aType = CCmsContactFieldItem::ECmsAddrPostcodeWork;
    else if (!aDesc.Compare(KECmsAddrCountryWork))
        aType = CCmsContactFieldItem::ECmsAddrCountryWork;
    
    else if (!aDesc.Compare(KECmsImpp))
        aType = CCmsContactFieldItem::ECmsImpp;
    else if (!aDesc.Compare(KECmsFullName))
        aType = CCmsContactFieldItem::ECmsFullName;
    else if (!aDesc.Compare(KECmsNote))
        aType = CCmsContactFieldItem::ECmsNote;
    else if (!aDesc.Compare(KECmsNickname))
        aType = CCmsContactFieldItem::ECmsNickname;
    else if (!aDesc.Compare(KECmsJobTitle))
        aType = CCmsContactFieldItem::ECmsJobTitle;
    else if (!aDesc.Compare(KECmsDepartment))
        aType = CCmsContactFieldItem::ECmsDepartment;
    else if (!aDesc.Compare(KECmsBirthday))
        aType = CCmsContactFieldItem::ECmsBirthday;
    else if (!aDesc.Compare(KECmsAnniversary))
        aType = CCmsContactFieldItem::ECmsAnniversary;
    else if (!aDesc.Compare(KECmsCompanyName))
        aType = CCmsContactFieldItem::ECmsCompanyName;    
    else if (!aDesc.Compare(KECmsAssistantNumber))
        aType = CCmsContactFieldItem::ECmsAssistantNumber;
    else if (!aDesc.Compare(KECmsCarPhone))
        aType = CCmsContactFieldItem::ECmsCarPhone;
    else if (!aDesc.Compare(KECmsPagerNumber))
        aType = CCmsContactFieldItem::ECmsPagerNumber;
    else if (!aDesc.Compare(KECmsPushToTalk))
        aType = CCmsContactFieldItem::ECmsPushToTalk;
    else if (!aDesc.Compare(KECmsShareView))
        aType = CCmsContactFieldItem::ECmsShareView;
    else
        return KErrNotFound;
    return KErrNone;
    }
 
 // ---------------------------------------------------------------------------
 // CCCAppCommLauncherHeaderTextOrder::MapContactFieldToClipDirection
 // ---------------------------------------------------------------------------
 //
 AknTextUtils::TClipDirection CCCAppCommLauncherHeaderTextOrder::MapContactFieldToClipDirection(CCmsContactFieldItem::TCmsContactField aType)
     {
     switch (aType)
         {
         case CCmsContactFieldItem::ECmsMobilePhoneGeneric:
         case CCmsContactFieldItem::ECmsMobilePhoneWork:
         case CCmsContactFieldItem::ECmsMobilePhoneHome:
         case CCmsContactFieldItem::ECmsLandPhoneGeneric:
         case CCmsContactFieldItem::ECmsLandPhoneWork:
         case CCmsContactFieldItem::ECmsLandPhoneHome:
         case CCmsContactFieldItem::ECmsVideoNumberGeneric:
         case CCmsContactFieldItem::ECmsVideoNumberWork:
         case CCmsContactFieldItem::ECmsVideoNumberHome:
         case CCmsContactFieldItem::ECmsFaxNumberHome:
         case CCmsContactFieldItem::ECmsFaxNumberWork:
         case CCmsContactFieldItem::ECmsEmailGeneric:
         case CCmsContactFieldItem::ECmsEmailWork:
         case CCmsContactFieldItem::ECmsEmailHome:
         case CCmsContactFieldItem::ECmsVoipNumberGeneric:
         case CCmsContactFieldItem::ECmsVoipNumberHome:
         case CCmsContactFieldItem::ECmsVoipNumberWork:
         case CCmsContactFieldItem::ECmsSipAddress:
         case CCmsContactFieldItem::ECmsImpp:
         case CCmsContactFieldItem::ECmsAssistantNumber:
         case CCmsContactFieldItem::ECmsCarPhone:
         case CCmsContactFieldItem::ECmsPagerNumber:
         case CCmsContactFieldItem::ECmsPushToTalk:
         case CCmsContactFieldItem::ECmsShareView:
             return AknTextUtils::EClipFromBeginning;
         default:
             return AknTextUtils::EClipFromEnd;
         }
     }
