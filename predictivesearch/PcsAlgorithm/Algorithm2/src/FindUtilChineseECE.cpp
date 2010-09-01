/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Chinese Find Utilities implementation file.
*
*/

#include <PtiEngine.h>
#include <centralrepository.h>
#include <AknFepInternalCRKeys.h>
#include <aknedsts.h>
#include <coeaui.h>
#include <AvkonInternalCRKeys.h>

// The below code is commented out because current CFindUtilChineseECE is used
// on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
// for the further merging work (merge from FindUtil 5.0)
#if 0
#include <aknenv.h> //phonebook
#include <aknappui.h>
#include <aknapp.h>
#include <eikenv.h>
#include <eikapp.h>
#include <eikappui.h>
#endif

#include "CPcsAlgorithm2.h"
#include "FindUtilChineseECE.h"
#include "chinesefindutils.h"
#include "CPcsKeyMap.h"
#include "CPsQuery.h"
#include "CPsQueryItem.h"

// CONSTANTS 
const TUint16 KStarChar = 0x002a;
const TUint16 KZhuyinstart = 0x3105;
const TInt KZhuyincount = 37;
const TInt KSeperator = 2626;
const TUint16 KMinUnicodeHz = 0x4e00;
const TUint16 KMaxUnicodeHz = 0x9fa5;
const TUint16 KStrokeHorizontalValue = 0x4e00;
const TUint16 KStrokeVerticalValue = 0x4e28;
const TUint16 KStrokeDownToLeftValue = 0x4e3f;
const TUint16 KStrokeDownToRightValue = 0x4e36;
const TUint16 KStrokeBendingValue = 0x4e5b;
const TInt KSysInputMode = -1;
const TInt KLeftToRightFlag =0x200e;
const TInt KRightToLeftFlag =0x200f;
// The below code is commented out because current CFindUtilChineseECE is used
// on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
// for the further merging work (merge from FindUtil 5.0)
#if 0
const TUid KUidPhoneBook = {0x101F4CCE};
const TUid KUidPhoneBookServer = {0x10207277};
#endif


_LIT(KWildChar, "*");
_LIT(KUnderLine, "_");
_LIT(KMiddleLine, "-");
_LIT(KBlank, " ");
_LIT(KTab, "\t");
_LIT(KPanicReason, "Abnormal input parameters!");

const TInt KLitLineFeed(8233);
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// Symbian constructor
// ---------------------------------------------------------
//
CFindUtilChineseECE* CFindUtilChineseECE::NewL(CPcsAlgorithm2* aAlgorithm)
    {
    CFindUtilChineseECE* self = new (ELeave) CFindUtilChineseECE();

    CleanupStack::PushL(self);
    self->ConstructL(aAlgorithm);
    CleanupStack::Pop(self);//self

    return self;
    }

// ---------------------------------------------------------
// Symbian second-phase constructor
// ---------------------------------------------------------
//
void CFindUtilChineseECE::ConstructL(CPcsAlgorithm2* aAlgorithm)
    {
    iRepositoryFind = CRepository::NewL(KCRUidAknFep);
    iRepositoryFindAdaptive = CRepository::NewL(KCRUidAvkon);

    iWatcher = CFindRepositoryWatcher::NewL(KCRUidAknFep, 
        TCallBack(HandleFindRepositoryCallBack, this), 
        iRepositoryFind);

    iWatcherAdaptive = CFindRepositoryWatcher::NewL(KCRUidAvkon,
        TCallBack(HandleFindRepositoryCallBack, this),
        iRepositoryFindAdaptive);

    iAlgorithm = aAlgorithm;
    OpenL();

    }

// ---------------------------------------------------------
// CFindUtilChineseECE utils class
// ---------------------------------------------------------
//
CFindUtilChineseECE::CFindUtilChineseECE() :
iLanguage(ELangTest),
iCurInputMode(KSysInputMode),
iSearchMethod(EAdptSearchPinyin),
iSearchMethodAdaptive(EFalse)
    {
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CFindUtilChineseECE::~CFindUtilChineseECE()
    {
    delete iPtiEngine;
    delete iWatcher;
    delete iRepositoryFind;
    delete iWatcherAdaptive;
    delete iRepositoryFindAdaptive;
    }

// ---------------------------------------------------------
// Open ptiengine and active it by input language
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::OpenT9InterfaceL(TLanguage aLanguage)
    {
    if (!iPtiEngine)
        {
        iPtiEngine = CPtiEngine::NewL();
        }

    if (aLanguage != iLanguage)
        {
        // We only support Chinese languages. If input language is changed
        // to non-Chinese, then first available Chinese language is used
        // instead.
        if ( aLanguage == ELangPrcChinese ||
             aLanguage == ELangTaiwanChinese ||
             aLanguage == ELangHongKongChinese )
            {
            iLanguage = aLanguage;
            }
        else if ( IsSupportLanguage(ELangPrcChinese) )
            {
            iLanguage = ELangPrcChinese;
            }
        else if ( IsSupportLanguage(ELangTaiwanChinese) )
            {
            iLanguage = ELangTaiwanChinese;
            }
        else if ( IsSupportLanguage(ELangHongKongChinese) )
            {
            iLanguage = ELangHongKongChinese;
            }
        else
            {
            iLanguage = aLanguage;
            }
        
        iPtiEngine->ActivateLanguageL(iLanguage);
        iPtiEngine->EnableToneMarks(EFalse);
        }

    return ETrue;
    }

// ---------------------------------------------------------
// Close ptiengine
// ---------------------------------------------------------
//
void CFindUtilChineseECE::CloseT9InterfaceL()
    {
    iLanguage = ELangTest;
    iPtiEngine->CloseCurrentLanguageL();
    }

// ---------------------------------------------------------
// Translate Chinese word to its spelling
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::DoTranslationL(TInt16 aHZUnicode, RPointerArray<HBufC>& aSpellList)
    {
    // Always translate according the adaptive search method setting
    return T9ChineseTranslationAdaptiveL(aHZUnicode, aSpellList);
    
#if 0
    // The below code is commented out because current CFindUtilChineseECE is used
    // on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
    // for the further merging work (merge from FindUtil 5.0)
#if 0
    if(CEikonEnv::Static())
        {
        if (iSearchMethodAdaptive &&(CEikonEnv::Static()->EikAppUi()->Application()->AppDllUid() == KUidPhoneBook ||
            CEikonEnv::Static()->EikAppUi()->Application()->AppDllUid() == KUidPhoneBookServer ))
            {
            if (!T9ChineseTranslationAdaptiveL(aHZUnicode, aSpellList))
                {
                return EFalse;
                }    
            }
        else
            {
#endif
            if (!T9ChineseTranslationL(aHZUnicode, aSpellList))
                {
                return EFalse;
                }
            // The below code is commented out because current CFindUtilChineseECE is used
            // on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
            // for the further merging work (merge from FindUtil 5.0)
#if 0          
            }
        }
    else
        {
        if (iSearchMethodAdaptive)
            {
            if (!T9ChineseTranslationAdaptiveL(aHZUnicode, aSpellList))
                {
                return EFalse;
                }
            }
        else
            {
            if (!T9ChineseTranslationL(aHZUnicode, aSpellList))
                {
                return EFalse;
                }
            }
        }
#endif
    return ETrue;

#endif
    }

// ---------------------------------------------------------
// Find pane text is including stroke symbol
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::IsStrokeSymbol(const TUint16 aFindWord)
    {
    TInt strokeValue = 0;

    switch (aFindWord)
        {
        case KStrokeHorizontalValue:
            strokeValue = 1;
            break;
        case KStrokeVerticalValue:
            strokeValue = 2;
            break;
        case KStrokeDownToLeftValue:
            strokeValue = 3;
            break;
        case KStrokeDownToRightValue:
            strokeValue = 4;
            break;
        case KStrokeBendingValue:
            strokeValue = 5;
            break;
        default:
            return strokeValue;
        }// switch

    return strokeValue;
    }

#if 0
// ---------------------------------------------------------
// Do translate for Chinese word
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::T9ChineseTranslationL(TInt16 aHZUnicode, RPointerArray<HBufC>& aSpellList)
    {
    TBuf<KMaxWordInterpretationLen> wordInterpretationBuf;

    if (iLanguage == ELangPrcChinese)
        {
        if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiPinyin) != KErrNone)
            {
            return EFalse;
            }
        }
    else if (iLanguage == ELangTaiwanChinese)
        {
        if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiZhuyin) != KErrNone)
            {
            return EFalse;
            }
        }
    else if (iLanguage == ELangHongKongChinese)
        {
        if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiStrokes) != KErrNone)
            {
            return EFalse;
            }
        }
    else if (iLanguage == ELangEnglish)
        {
        if (!iSupportPRCChinese)
            {
            return EFalse;
            }

        TInt err = KErrNone;
        if (!iPtiEnginePrc)
            {
            iPtiEnginePrc = CPtiEngine::NewL(EFalse);
            TRAP(err, iPtiEnginePrc->ActivateLanguageL(ELangPrcChinese));
            }

        if (err == KErrNone)
            {
            if (iPtiEnginePrc->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiPinyin) != KErrNone)
                {
                return EFalse;
                }
            }
        }
    else
        {
        return EFalse;
        }

    const TInt len = wordInterpretationBuf.Length();
    TInt start = 0;

    for (TInt i = 0; i < len; i++)
        {
        if (wordInterpretationBuf[i] == KSeperator)
            {
            aSpellList.Append((wordInterpretationBuf.MidTPtr(start, i - start)).Alloc());
            start = i + 1;
            }
        }

    aSpellList.Append((wordInterpretationBuf.MidTPtr(start, len - start)).Alloc());

    return ETrue;
    }
#endif

// ---------------------------------------------------------
// Do translate for Chinese word
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::T9ChineseTranslationAdaptiveL(TInt16 aHZUnicode,  
    RPointerArray<HBufC>& aSpellList)
    {
    TBuf<KMaxWordInterpretationLen> wordInterpretationBuf;

    switch(iSearchMethod)
        {
        case EAdptSearchPinyin:
            if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiPinyin)
                != KErrNone)
                {
                return EFalse;
                }
             break;
        case EAdptSearchStroke:
            if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiStrokes)
                != KErrNone)
                {
                return EFalse;
                }
            break;
        case EAdptSearchZhuyin:
            if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiZhuyin)
                != KErrNone)
                {
                return EFalse;
                }   
            break;
        case EAdptSearchNormalCangjie:
            if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiCangJie)
                != KErrNone)
                {
                return EFalse;
                }
            break;
        case EAdptSearchEasyCangjie:
            if (iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiEasyCangjie) 
                != KErrNone)
                {
                return EFalse;
                }
            break;
        case EAdptSearchAdvCangjie:
            if ((iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiCangJie) != KErrNone)
                &&(iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiEasyCangjie) != KErrNone))
                {
                return EFalse;
                }
            break;
        default:
            return EFalse;
        }


    TInt len = wordInterpretationBuf.Length();
    TInt start = 0;

    if( iSearchMethod != EAdptSearchAdvCangjie )
        {
        for (TInt i =0; i < len; i++)
            {
            if (wordInterpretationBuf[i] == KSeperator) 
                {
                aSpellList.Append((wordInterpretationBuf.MidTPtr(start, i-start)).Alloc());
                start = i + 1;
                }
            }

        aSpellList.Append((wordInterpretationBuf.MidTPtr(start, len-start)).Alloc());
        }
    //Could look advanced cangjie as normal and easy cangjie
    else 
        {
        iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiEasyCangjie);
        len = wordInterpretationBuf.Length();
        start = 0;
        for (TInt i =0; i < len; i++)
            {
            if (wordInterpretationBuf[i] == KSeperator) 
                {
                aSpellList.Append((wordInterpretationBuf.MidTPtr(start, i-start)).Alloc());
                start = i + 1;
                }
            }

        aSpellList.Append((wordInterpretationBuf.MidTPtr(start, len-start)).Alloc()); 


        iPtiEngine->GetSpelling(aHZUnicode, wordInterpretationBuf, EPtiCangJie);
        len = wordInterpretationBuf.Length();
        start = 0;
        for (TInt i =0; i < len; i++)
            {
            if (wordInterpretationBuf[i] == KSeperator) 
                {
                aSpellList.Append((wordInterpretationBuf.MidTPtr(start, i-start)).Alloc());
                start = i + 1;
                }
            }

        aSpellList.Append((wordInterpretationBuf.MidTPtr(start, len-start)).Alloc());  
        }

    return ETrue;
    }

// ---------------------------------------------------------
// Find pane text is including separator
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsFindWordSeparator(TChar aCh)
    {
    return aCh ==' '|| aCh=='-'|| aCh =='\t' || aCh == '_'|| aCh == KLitLineFeed;
    }

// ---------------------------------------------------------
// Create ptiengine and initialize member data 
// ---------------------------------------------------------
//
void CFindUtilChineseECE::OpenL()
    {
    TInt inputLanguage = 0;
    TInt searchMode = 0;

    if (iRepositoryFind != NULL)
        {
        iRepositoryFind->Get(KAknFepInputTxtLang, inputLanguage);
        }

    // The following sets iLangueage to inputLanguage if inputLanguage
    // is Chinese. If inputLanguage is non-Chinese, iLanguage will be
    // set to first available Chinese language.
    OpenT9InterfaceL((TLanguage) inputLanguage);

    if (iRepositoryFindAdaptive != NULL)
        {
        iRepositoryFindAdaptive->Get(KAknAvkonAdaptiveSearchEnabled, iSearchMethodAdaptive);
        }
    if (iLanguage == ELangPrcChinese)
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChinesePRC , searchMode);
        if (searchMode == 0)
            {
            iSearchMethod = EAdptSearchPinyin;
            }
        else if (searchMode == 1)
            {
            iSearchMethod = EAdptSearchStroke;
            }
        }
    else if (iLanguage == ELangTaiwanChinese)
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChineseTW , searchMode);
        if (searchMode == 0)
            {
            iSearchMethod = EAdptSearchZhuyin;
            }
        else if (searchMode == 1)
            {
            iSearchMethod = EAdptSearchStroke;
            }
        }
    else if (iLanguage == ELangHongKongChinese)
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChineseHongkong  , searchMode);
        if (searchMode == 1)
            {
            iRepositoryFind->Get(KAknFepCangJieMode , searchMode);

            switch (searchMode)
                {
                case 0:
                    iSearchMethod = EAdptSearchNormalCangjie;
                    break;
                case 1:
                    iSearchMethod = EAdptSearchEasyCangjie;
                    break;
                case 2:
                    iSearchMethod = EAdptSearchAdvCangjie;
                    break;
                }
            }
        else if (searchMode == 0)
            {
            iSearchMethod = EAdptSearchStroke;
            }
        }
    }

// ---------------------------------------------------------
// Close ptiEngine 
// ---------------------------------------------------------
//
void CFindUtilChineseECE::Close()
    {
    TRAP_IGNORE(CloseT9InterfaceL());    
    }

// ---------------------------------------------------------
// Match arithmetic for general search
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::Match(const TDesC& aContactsField, const TDesC& aWord)
    {
    // Check function parameter
    if (aWord.Length() == 0)
        {
        return ETrue;
        }

    if (aContactsField.Length() == 0)
        {
        return EFalse;
        }

    TBool ret = EFalse;
    // Check Chinese word in find pane
    TBool chineseWord = IsChineseWordIncluded(aWord);

    if (chineseWord)
        {
        ret = IncludeString(aContactsField, aWord);

        if (ret)
            {
            // Included return ETrue
            return ret;
            }
        /*     else// If it do not find, check the symbol
            {
            // if Stroke Symbol in HongKong
            TBool strokeSymHK = (IsStrokeSymbolInString(aWord)) && 
                               (iLanguage == ELangHongKongChinese);

            // it is not include stroke symbol
            if (!strokeSymHK)
                {
                return ret;
                }
            }*/
        }

    // Array for item string
    RPointerArray<STRINGINFO> stringInfoArr;
    // Split item string
    TRAPD(err, SplitItemStringL(stringInfoArr, aContactsField));
    if (err != KErrNone)
        {
        stringInfoArr.ResetAndDestroy();
        stringInfoArr.Close();
        return ret;
        }

    TInt index = 0;
    STRINGINFO* pStringInfo;
    const TInt arrayCount = stringInfoArr.Count();

    if (!chineseWord)
        {
        // Search English word
        for (index = 0; index < arrayCount; index++)
            {
            pStringInfo = stringInfoArr[index];
            if (!pStringInfo->isChinese &&
                pStringInfo->segmentString.MatchC(aWord) != KErrNotFound)
                {
                ret = ETrue;
                break;
                }
            }
        }

    if (!ret)// If not find, search Chinese word 
        {
        TRAP(err, ret = MatchChineseInitialStringL(stringInfoArr, aWord));
        }

    stringInfoArr.ResetAndDestroy();
    stringInfoArr.Close();

    return ret;
    }

// ---------------------------------------------------------
// Initial character search 
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::MatchChineseInitialStringL(RPointerArray<STRINGINFO>& aStringInfoArr, 
    const TDesC& aSearchStr)
    {
    TInt spellCount = 0;
    TInt i = 0;
    TInt chineseWordCount;
    TInt chineseWordIndex;
    STRINGINFO* pStringInfo;
    TBool ret = EFalse;
    const TInt arrayCount = aStringInfoArr.Count();

    for (TInt index = 0; index < arrayCount; index++)
        {
        pStringInfo = aStringInfoArr[index];
        // English word, continue
        if (!pStringInfo->isChinese)
            {
            continue;
            }

        chineseWordCount = pStringInfo->segmentString.Length();

        for (chineseWordIndex = 0; chineseWordIndex < chineseWordCount; chineseWordIndex++)
            {
            RPointerArray<HBufC> spellList;

            if (!DoTranslationL(TInt16(pStringInfo->segmentString[chineseWordIndex]), spellList))
                {
                spellList.ResetAndDestroy();//destroy spellList
                spellList.Close();
                continue;
                }

            spellCount = spellList.Count();
            // Search all spelling
            for (i = 0; i < spellCount; i++)
                {
                if (spellList[i]->MatchC(aSearchStr) != KErrNotFound)
                    {
                    // If find, break
                    ret = ETrue;
                    break;
                    }
                }//end for

            spellList.ResetAndDestroy();//destroy spellList
            spellList.Close();

            if (ret)
                {
                // If find, return
                return ret;
                }
            }
        }

    return ret;
    }

// ---------------------------------------------------------
// Remove separator from search text
// ---------------------------------------------------------
//
void CFindUtilChineseECE::RemoveSeparator(TDes& aSearchText)
    {
    TInt index = -1;
    TBuf<1> temp;

    temp.Copy(KUnderLine);
    while ((index = aSearchText.FindC(temp)) > 0)
        {
        aSearchText.Replace(index, 1, KNullDesC);
        }

    temp.Copy(KMiddleLine);
    while ((index = aSearchText.FindC(temp)) > 0)
        {
        aSearchText.Replace(index, 1, KNullDesC);
        }

    temp.Copy(KBlank);
    while ((index = aSearchText.FindC(temp)) > 0)
        {
        aSearchText.Replace(index, 1, KNullDesC);
        }

    temp.Copy(KTab);
    while ((index = aSearchText.FindC(temp)) > 0)
        {
        aSearchText.Replace(index, 1, KNullDesC);
        }
    }

// ---------------------------------------------------------
// Match arithmetic for accurate search 
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::MatchRefineL(const TDesC& aItemString, CPsQuery& aPsQuery)
    {
    RArray<TInt> ignore1;
    CleanupClosePushL( ignore1 );
    RArray<TInt> ignore2;
    CleanupClosePushL( ignore2 );
    
    TBool match = MatchRefineL( aItemString, aPsQuery, ignore1, ignore2, EFalse );
    
    CleanupStack::PopAndDestroy( &ignore2 );
    CleanupStack::PopAndDestroy( &ignore1 );
    
    return match;
    }

TBool CFindUtilChineseECE::MatchRefineL(const TDesC& aItemString, CPsQuery& aPsQuery, 
        RArray<TInt>& aMatchPos, RArray<TInt>& aMatchLength, TBool aHighLight )
    {
    TBuf<KMaxWordLength> tempBuf;
    TBuf<KMaxWordLength> itemString;
    TBool haschineseword = EFalse; 

    itemString.Zero();
    if ( KMaxWordLength > aItemString.Length() )
        {
        itemString.Copy( aItemString );
        }
    else
        {
        itemString.Copy( aItemString.Left( KMaxWordLength ) );
        }
    
    //trim the special char
    if(itemString.Length()>0)
        {
        if(itemString[0]== KLeftToRightFlag || itemString[0]== KRightToLeftFlag)
            {
            itemString.Delete(0,1);
            itemString.Delete((itemString.Length()-1),1);
            }
        }

    // Check function parameter
    if (aPsQuery.Count() == 0)
        {
        return ETrue;
        }

    if (aItemString.Length() == 0)
        {
        return EFalse;
        }

    TBool ret = EFalse;
    CPsQuery* query = CPsQuery::NewL();
    CleanupStack::PushL( query );
    GetPartOfQueryL(aPsQuery, 0, aPsQuery.Count()-1, *query);
    RemoveSeparatorL(*query);
    TPtrC queryStrPtr = query->QueryAsStringLC();

    tempBuf.Zero();
    tempBuf.Copy(queryStrPtr);

    CleanupStack::PopAndDestroy(); // queryStrPtr

    if (iLanguage == ELangHongKongChinese)
        {
        UpdateCurrentInputMode();
        if (iCurInputMode == KSysInputMode)
            {
            iCurInputMode = 0x0020;
            }

        }
    else
        {
        iCurInputMode = KSysInputMode;
        }
    
    TBool inputMethodStroke = EFalse;
    if(iLanguage == ELangHongKongChinese && iCurInputMode == 0x0020)
        {
        inputMethodStroke = ETrue;
        }
      
    // Check if any Chinese word in find pane
    if ( IsChineseWordIncluded(tempBuf) )
        {
        haschineseword = ETrue;
        ret = IncludeString(aItemString, tempBuf);
      
        if( IsAllChineseWord( tempBuf ))
            {
            CleanupStack::PopAndDestroy( query );
            return ret;
            }
        }

   
    // There are two steps to do when searchstring includes Chinese character 
    //
    // step 1:  check whether itemstring includes the chinese charactor in the searchstring
    //              If not, return EFalse. Otherwise, go to step2
    //
    // step2:  If itemstring includes the chinese charactor in the searchstring, translate 
    //             the Chinese character in the searchstring to spellList and reconstruct the query.
    if (haschineseword)
        {
        TInt ii=0;
        
        // Step 1: 
        // E.g: itemstring = "0x4E00,0x4E8C,0x4E09,0x56DB,0x4E94" 
        // (Unicode of Chinese number "1 2 3 4 5")
        // searchstring1 = "0x4E00,x,y,z,0x4E09"
        // searchstring2 = "0x4E8C,0x4E00"
        // searchstring3 = "0x4E00,0x4E5D" 
        // In this case, searchstring2 and searchstring3 will return EFalse, 
        // only searchstring1 will be translated to spellList in step 2
        while (ii< tempBuf.Length())
            {
            TInt lastFindCursor = -1;
            if( (TUint16)tempBuf[ii] <= KMaxUnicodeHz 
                && (TUint16)tempBuf[ii]>= KMinUnicodeHz
                && ( !inputMethodStroke ||(inputMethodStroke &&
                    !IsStrokeSymbol(tempBuf[ii])) ) )
                {
                TInt Findcursor = itemString.Locate(tempBuf[ii]);
                if( KErrNotFound == Findcursor || Findcursor < lastFindCursor )
                    {
                    CleanupStack::PopAndDestroy( query );
                    return EFalse;
                    }
                else
                    {
                    lastFindCursor = Findcursor;
                    ii++;
                    }
                }
            else
                {
                ii++;
                }
            }
            
        ii = 0;
        // Counter of queryItem
        TInt queryItemCount = 0;
        
        //Step 2: 
        // Translating searchstring to spellList and reconstruct query
         while ( ii< tempBuf.Length() )
             {            
             // If it is a valid chinese character
             if ( (TUint16)tempBuf[ii] <= KMaxUnicodeHz 
                 && (TUint16)tempBuf[ii]>= KMinUnicodeHz
                 && ( !inputMethodStroke ||(inputMethodStroke &&
                     !IsStrokeSymbol(tempBuf[ii])) ) )
                 {
                 RPointerArray<HBufC> spellList;
                 
                 // If successfully translating the chinese character to spellList( Pinyin,Zhuyin or stroke )
                 // then reconstruct the query, replace the Chinese character with its spellList in the query.
                 // Otherwise, just increase the counter of queryItem.
                 if( DoTranslationL(TInt16(tempBuf[ii]), spellList) )
                     {                    
                     // Replace the queryItem by the translated spelling
                     CPsQueryItem& tempItem = query->GetItemAtL(queryItemCount);
                     const TKeyboardModes tempMode = tempItem.Mode();
                     
                     // Remove the Chinese character queryItem
                     query->Remove(queryItemCount);
                     
                     // Reconstruct the query, replace Chinese character with its spellList
                     for(TInt cnt = 0; cnt < spellList[0]->Length(); cnt++)
                         {
                         // Add a new query item to query
                         CPsQueryItem* newItem = CPsQueryItem::NewL();   
                         CleanupStack::PushL(newItem);
                         newItem->SetCharacter(  (*spellList[0])[cnt] );
                         newItem->SetMode(tempMode);
   
                         query->InsertL(*newItem, queryItemCount + cnt );
                         CleanupStack::Pop(newItem);  
                         }
                    
                     queryItemCount += spellList[0]->Length();
                     }
                 else
                     {
                     queryItemCount++;
                     }
                     
                 ii++;
                 spellList.ResetAndDestroy();
                 spellList.Close();

                 }
             //if not, just pass it by     
             else
                 {
                 ii++;
                 queryItemCount++;
                 }
             }

        }
    
    // Array for item string
    RPointerArray<STRINGINFO> stringInfoArr;
    // Split item string
    SplitItemStringL(stringInfoArr, itemString);

    if (stringInfoArr.Count() > 0)
        {
        ret = MatchSegmentL(stringInfoArr, *query, aMatchPos, aMatchLength, aHighLight);
        }

    stringInfoArr.ResetAndDestroy();
    stringInfoArr.Close();

    CleanupStack::PopAndDestroy( query );

    return ret;
    }

// ---------------------------------------------------------
// Splite the input text to sgement by language
// ---------------------------------------------------------
//
void CFindUtilChineseECE::SplitItemStringL(RPointerArray<STRINGINFO>& aStringInfoArr, 
    const TDesC& aItemString)
    {
    TBuf<KMaxWordLength> tempBuf;
    TBuf<KMaxWordLength> englishBuf;
    TBuf<KMaxWordLength> chineseBuf;

    tempBuf.Zero();
    englishBuf.Zero();
    chineseBuf.Zero();

    TInt index = 0;
    TBool isChinese = EFalse;
    const TInt strLength = aItemString.Length();

    for (; index < strLength; index++)
        {
        if (IsFindWordSeparator(aItemString[index]))
            {
            // Check Chinese and English Buf. If it is not empty, 
            // add buf to Array
            InsertStrInforArrayL(aStringInfoArr, chineseBuf, ETrue, index);
            InsertStrInforArrayL(aStringInfoArr, englishBuf, EFalse, index);
            continue;
            }

        isChinese = ((TUint16)aItemString[index] >= KMinUnicodeHz && 
                               (TUint16)aItemString[index] <= KMaxUnicodeHz);
        
        if ( !isChinese )// English word
            {
            // Chinese word is end and add to array
            InsertStrInforArrayL(aStringInfoArr, chineseBuf, ETrue, index);

            // Add English word to array
            englishBuf.Append(aItemString[index]);
            }
        else // Chinese word
            {
            // English word is end and add to array
            InsertStrInforArrayL(aStringInfoArr, englishBuf, EFalse, index);

            // Add Chinese word to array
            chineseBuf.Append(aItemString[index]);
            }
        }

    // Finish loop check buffer is empty
    InsertStrInforArrayL(aStringInfoArr, chineseBuf, ETrue, index);
    InsertStrInforArrayL(aStringInfoArr, englishBuf, EFalse, index);
    }

// ---------------------------------------------------------
// Insert segment to list
// ---------------------------------------------------------
//
void CFindUtilChineseECE::InsertStrInforArrayL(RPointerArray<STRINGINFO>& aStringInfoArr, 
    TDes &aSegmentStr, TBool aChinese, TInt aIndexAfterStr)
    {
    if (aSegmentStr.Length() <= 0)
        {
        return;
        }

    STRINGINFO* strInfo = new (ELeave) STRINGINFO;

    strInfo->segmentString.Zero();
    strInfo->segmentString.Copy(aSegmentStr);
    strInfo->segmentString.LowerCase();
    strInfo->isChinese = aChinese;
    strInfo->segmentPos = aIndexAfterStr - aSegmentStr.Length();

    aStringInfoArr.AppendL(strInfo);
    aSegmentStr.Zero();
    }

// ---------------------------------------------------------
// This segment is matched by search text
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::MatchSegmentL(RPointerArray<STRINGINFO>& aStringInfoArr, 
    CPsQuery& aQuery, RArray<TInt>& aMatchPos, RArray<TInt>& aMatchLength, TBool aHighLight )
    {
    const TInt arrayLength = aStringInfoArr.Count();
    const TInt searchStrLength = aQuery.Count();
    STRINGINFO* pStringInfo;
    TInt index = 0;
    TBool ret = EFalse;
    TBool hasChinese = EFalse;

    // First check if there is a full match in non-Chinese in any one segment
    for (; index < arrayLength; index++)
        {
        pStringInfo = aStringInfoArr[index];
        TPtrC currentSeg( pStringInfo->segmentString );
        TInt matchLen( 0 );

        // Compare word
        if (pStringInfo->isChinese)
            {
            hasChinese = ETrue;
            }
        else
            {
            matchLen = MatchStringL( currentSeg, aQuery );
            }
        
        // If full match is found
        if ( matchLen == searchStrLength  )
            {
            //Only append postion of non-Chinese character
            if(  !pStringInfo->isChinese )
                {
                aMatchPos.Append( pStringInfo->segmentPos );
                aMatchLength.Append( matchLen );
                }
            ret =   ETrue;
            
            // If there is no need to HighLight the matched part, return immediately
            // Otherwise, continue to find all the matched positions
            if( !aHighLight ) 
                return ret;
            }
        }
    
    // Then search through segments again, this time considering
    // also cases where matching part continues over several segments.
    // This algorithm is greedy and heuristic and can't necesarily find
    // all the matches in the segments after the first one.
    TPtrC segCurrent;
    TInt searchStrIndextemp = 0;
    TInt searchStrIndexMax = 0;
    TInt searchStrIndex = 0;
    index = 0;

    // Loop through seqments until whole query is matched
    for (; index < arrayLength && !ret ; index++)
        {
        // Create an empty CPsQuery
        CPsQuery* tempQuery = CPsQuery::NewL();
        CleanupStack::PushL( tempQuery );

        GetPartOfQueryL( aQuery, searchStrIndex, searchStrLength - 1, *tempQuery );

        pStringInfo = aStringInfoArr[index];
        segCurrent.Set( pStringInfo->segmentString );

        if (pStringInfo->isChinese)
            {
            // find the longest substring matching the query
            searchStrIndexMax = 0;
            for (TInt i = 0; i< segCurrent.Length(); i++)
                {
                TPtrC tempSeg( segCurrent.Mid(i) );
                TInt startIdx( KErrNotFound );
                TInt endIdx( KErrNotFound );
                searchStrIndextemp = MatchChineseStringL(tempSeg, *tempQuery, startIdx, endIdx);
                if (searchStrIndextemp > searchStrIndexMax)
                    {
                    searchStrIndex -= searchStrIndexMax;
                    searchStrIndexMax = searchStrIndextemp;
                    searchStrIndex += searchStrIndexMax;
                    }
                
                if (searchStrIndex >= searchStrLength)
                    {
                    // found substring which consumes whole query, no need to find any longer
                    break;
                    }
                }
            }
        else // not chinese
            {
            TInt matchLen = MatchStringL(segCurrent, *tempQuery);
            searchStrIndex += matchLen;
            if ( matchLen )
                {
                aMatchPos.Append( pStringInfo->segmentPos );
                aMatchLength.Append( matchLen );
                }
            }

        if (searchStrIndex >= searchStrLength)
            {
             ret = ETrue; // also breaks us out from the loop
            }
        else if ( !pStringInfo->isChinese && !hasChinese )
            {
           CleanupStack::PopAndDestroy( tempQuery );
            break;
            }
        
        CleanupStack::PopAndDestroy( tempQuery );
        }

    return ret;
    }

// ---------------------------------------------------------
// Search Chinese word in input text 
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::MatchChineseStringL(const TDesC& aSearhTargetString, CPsQuery& aQuery, 
        TInt& aMatchStartIdx, TInt& aMatchEndIdx)
    {
    const TInt stringLength = aSearhTargetString.Length();
    const TInt searchLength = aQuery.Count();
    TInt index = 0;
    TBuf<KMaxWordLength> previousBuf;
    TInt curMatchCount = 0;
    TInt preMatchCount = 0;
    TInt matchIndex = 0;
    TInt curSearchIndex = 0;
    TBool bFullMatched = EFalse;
    TBool bContinue = EFalse;
    TBool bPrevReplaced = EFalse;
    aMatchStartIdx = KErrNotFound;
    aMatchEndIdx = KErrNotFound;
    
    CPsQuery* tempFullQuery = CPsQuery::NewL();
    CleanupStack::PushL( tempFullQuery );
    GetPartOfQueryL( aQuery, 0, aQuery.Count() - 1, *tempFullQuery );

    for (; index < stringLength; index++)
        {
        RPointerArray<HBufC> spellList;

        if (!DoTranslationL(TInt16(aSearhTargetString[index]), spellList))
            {
            spellList.ResetAndDestroy();//destroy spellList
            spellList.Close();

            continue;
            }

        matchIndex = 0;
        bContinue = EFalse;

        // Get left string
        CPsQuery* tempQuery = CPsQuery::NewL();
        CleanupStack::PushL( tempQuery );
        GetPartOfQueryL( aQuery, curSearchIndex, searchLength - 1, *tempQuery );

        // Get Matched count and the matched index in list
        curMatchCount = MaxMatchInListL(spellList, *tempQuery, previousBuf, matchIndex, 
            bFullMatched, preMatchCount, bContinue, bPrevReplaced, *tempFullQuery);

        // There is a string matched in list 
        if (curMatchCount != 0)
            {
            previousBuf.Copy(*spellList[matchIndex]);

            if (curMatchCount == previousBuf.Length())
                {
                bFullMatched = ETrue;
                }
            else
                {
                bFullMatched = EFalse;
                }

            // If it is repeat match, continue
            if (!bContinue)
                {
                curSearchIndex += curMatchCount;
                }
            else
                {
                bFullMatched = EFalse;
                previousBuf.Zero();
                curMatchCount = 0;
                }

            // Store the index of first matched Han character
            if ( bPrevReplaced || aMatchStartIdx == KErrNotFound )
                {
                aMatchStartIdx = index;
                }
            // Store the index of last matched Han character
            aMatchEndIdx = index;
            }
        else
            {
            bFullMatched = EFalse;
            previousBuf.Zero();
            if (curSearchIndex != 0)
                {
                index--;
                }

            curSearchIndex = 0;
            
            // Any earlier partial match is discarded, reset the matching part indices
            aMatchStartIdx = KErrNotFound;
            aMatchEndIdx = KErrNotFound;
            }

        spellList.ResetAndDestroy();//destroy spellList
        spellList.Close();
        preMatchCount = curMatchCount;

        CleanupStack::PopAndDestroy( tempQuery );
        tempQuery = NULL;

        // Search to End and return
        if (curSearchIndex >= searchLength)
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy( tempFullQuery );
    tempFullQuery = NULL;

    return curSearchIndex;
    }

// ---------------------------------------------------------
// Maximal matched count in spelling list  
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::MaxMatchInListL(RPointerArray<HBufC> &aSpellList, CPsQuery& aQuery,
    TDesC& aPreviouStr, TInt& aMatchIndex, const TBool aFullMatched, 
    const TInt aMatchedCount, TBool& aAgain, TBool& aPrevReplaced, CPsQuery& aFullQuery)
    {
    const TInt itemCount = aSpellList.Count();
    TInt matchCount = 0;
    TBool selfMatch = EFalse;
    aMatchIndex = 0;

    // If list is empty, then return
    if (itemCount == 0)
        {
        return matchCount;
        }

    TInt index = 0;
    TInt temp = 0;
    TBuf<KMaxWordLength> tempBuf;
    TInt repeatCount = 0;
    TBool loopFlag = EFalse;

    for (; index < itemCount; index++)
        {
        temp = 0;
        repeatCount = 0;
        loopFlag = EFalse;
        tempBuf.Zero();
        tempBuf.Copy( *aSpellList[index] );

        temp = MatchStringL(tempBuf, aQuery);
        if(temp != 0)
            {
            selfMatch = ETrue;
            }
        else
            {
            selfMatch = EFalse;
            }
        // Not find and previous word is not empty
        if (temp == 0 && aPreviouStr.Length() != 0)
            {
            // Previous word is fully matched
            if (aFullMatched)
                {
                repeatCount = ReverseMatchStringL(aPreviouStr, tempBuf, aFullQuery );
                // Find repeat and remove it, search again
                if (repeatCount != 0)
                    {
                    temp = MatchStringL(tempBuf.Right(tempBuf.Length() - repeatCount), aQuery);

                    if (temp == 0)
                        {
                        loopFlag = ETrue;
                        temp = repeatCount;
                        }
                    }
                }
            else
                {
                repeatCount = MatchStringL(aPreviouStr, tempBuf, aFullQuery);

                // Find repeat and remove it, search again
                if (repeatCount != 0)
                    {
                    if (aMatchedCount <= repeatCount)
                        {
                        temp = MatchStringL(tempBuf.Right(tempBuf.Length() - aMatchedCount), aQuery);

                        if (temp == 0)
                            {
                            loopFlag = ETrue;
                            temp = aMatchedCount;
                            }
                        }
                    }

                if (temp == 0)
                    {
                    repeatCount = ReverseMatchStringL(aPreviouStr.Left(aMatchedCount), tempBuf, aFullQuery);

                    if (repeatCount != 0)
                        {
                        temp = MatchStringL(tempBuf.Right(tempBuf.Length() - repeatCount), aQuery);

                        if (temp == 0)
                            {
                            loopFlag = ETrue;
                            temp = repeatCount;
                            }
                        }
                    }
                }
            }

        // Find count is longer than before
        // Record new data
        if ((loopFlag == aAgain) &&
            (temp > matchCount))
            {
            matchCount = temp;
            aMatchIndex = index;
            aAgain = loopFlag;
            }

        if (loopFlag != aAgain &&
            loopFlag &&
            (temp > matchCount))
            {
            matchCount = temp;
            aMatchIndex = index;
            aAgain = loopFlag;
            }

        if (loopFlag != aAgain &&
            !loopFlag &&
            temp != 0)
            {
            matchCount = temp;
            aMatchIndex = index;
            aAgain = loopFlag;
            }
        if (matchCount == aQuery.Count() && selfMatch)
            {
            break;
            }
        }
    
    // Was the previous match replaced with a longer match from this spell list
    aPrevReplaced = ( matchCount && repeatCount );
    
    return matchCount;
    }

// ---------------------------------------------------------
// Search the text is include the input text  
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IncludeString(const TDesC& aFirst, const TDesC& aSecond)
    {
    TBuf<KMaxWordLength> tempBuf;
    tempBuf.Zero();

    if (TInt16(aSecond[0]) != KStarChar)
        {
        tempBuf.Append(KWildChar);
        }

    tempBuf.Append(aSecond);

    if (TInt16(aSecond[aSecond.Length() - 1]) != KStarChar)
        {
        tempBuf.Append(KWildChar);
        }

    if (aFirst.MatchC(tempBuf) != KErrNotFound)
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------
// Search text in other text  
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::MatchStringL(const TDesC& aFirst, const TDesC& aSecond, CPsQuery& aFullQuery)
    {
    const TInt secondStrLength = aSecond.Length();
    const TInt firstStrLength = aFirst.Length();

    const TInt compareCount = Min( firstStrLength, secondStrLength );
    TInt index = 0;
    TBuf<KMaxWordInterpretationLen> firstNumInterpretationBuf;
    TBuf<KMaxWordInterpretationLen> secondNumInterpretationBuf;
    
    iAlgorithm->GetKeyMap()->GetMixedKeyStringForDataL(aFullQuery, aFirst, firstNumInterpretationBuf);
    iAlgorithm->GetKeyMap()->GetMixedKeyStringForDataL(aFullQuery, aSecond, secondNumInterpretationBuf);

    for (; index < compareCount; index++)
        {
        if (firstNumInterpretationBuf.Left(index + 1).FindC(secondNumInterpretationBuf.Left(index + 1)) == KErrNotFound)
            {
            break;
            }
        }
    // Match count
    return index;
    }

// ---------------------------------------------------------
// Search CPsQuery in aSearhTargetStr
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::MatchStringL(const TDesC& aSearhTargetStr, CPsQuery& aQuery)
    {
    const TInt secondStrLength = aQuery.Count();
    const TInt firstStrLength = aSearhTargetStr.Length();
    TBuf<KMaxWordInterpretationLen> numInterpretationBuf;
    TBuf<KMaxWordInterpretationLen> queryStr;

    const TInt compareCount = Min( firstStrLength, secondStrLength );

    TInt index = 0;

    iAlgorithm->GetKeyMap()->GetMixedKeyStringForDataL(aQuery, aSearhTargetStr, numInterpretationBuf);
    iAlgorithm->GetKeyMap()->GetMixedKeyStringForQueryL(aQuery, queryStr);

    for (; index < compareCount; index++)
        {
        if (numInterpretationBuf.Left(index + 1).FindC(queryStr.Left(index + 1)) == KErrNotFound)
            {
            break;
            }
        }

    // Match count
    return index;
    }

// ---------------------------------------------------------
// Search text by reverse  
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::ReverseMatchStringL(const TDesC& aFirst, const TDesC& aSecond, CPsQuery& aFullQuery)
    {
    const TInt secondStrLength = aSecond.Length();
    const TInt firstStrLength = aFirst.Length();
    TInt compareCount = firstStrLength > secondStrLength ? secondStrLength : firstStrLength;
    TInt index = 0;
    TInt matchCount = 0;
    TBuf<KMaxWordInterpretationLen> firstNumInterpretationBuf;
    TBuf<KMaxWordInterpretationLen> secondNumInterpretationBuf;
    
    iAlgorithm->GetKeyMap()->GetMixedKeyStringForDataL(aFullQuery, aFirst, firstNumInterpretationBuf);
    iAlgorithm->GetKeyMap()->GetMixedKeyStringForDataL(aFullQuery, aSecond, secondNumInterpretationBuf);
    
    for (; index < compareCount; index++)
        {
        if( firstNumInterpretationBuf.Right( index + 1 ).FindC( secondNumInterpretationBuf.Left( index + 1 ) ) != KErrNotFound )
            {
            matchCount = index + 1;
            }
        }

    return matchCount;
    }

// ---------------------------------------------------------
// Current input text is handled by this model  
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsWordValidForMatching(const TDesC& /*aWord*/)
    {
    return ETrue;
    }

// ---------------------------------------------------------
// Find pane text is including Chinese word  
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsChineseWordIncluded(const TDesC& aWord)
    {
    TBool IsChineseSearchStr = EFalse;
    const TInt len = aWord.Length();

    for (TInt ii = 0; ii < len; ii++)
        {

        if ( ((TUint16) aWord[ii] >= KMinUnicodeHz) &&
            ((TUint16) aWord[ii] <= KMaxUnicodeHz) )
            {
            IsChineseSearchStr = ETrue;
            break;
            }
        }

    return IsChineseSearchStr;
    }

// ---------------------------------------------------------
// Find pane text is including stroke symbol  
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsStrokeSymbolInString(const TDesC& aWord)
    {
    const TInt len = aWord.Length();

    for (TInt index = 0; index < len; index++)
        {
        if (IsStrokeSymbol(TUint16(aWord[index])) != 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------
// Find pane text is all Chinese word(s)
// --------------------------------------------------------
//
TBool CFindUtilChineseECE::IsAllChineseWord(const TDesC& aWord)
    {
    TBool isChineseWord = ETrue;
    const TInt len = aWord.Length();
    
    TBool InputMethodStroke = EFalse;
    if(iLanguage == ELangHongKongChinese && iCurInputMode == 0x0020)
        {
        InputMethodStroke = ETrue;
        }
    
    for( TInt i = 0; i < len; i++ )
        {
        if( (( TUint16 )aWord[i] > KMaxUnicodeHz ) ||  (( TUint16 )aWord[i] < KMinUnicodeHz )
            || ( InputMethodStroke && IsStrokeSymbol(aWord[i]) ) )
            {
            isChineseWord = EFalse;
            break;
            }
        }
    return isChineseWord;
    }

// ---------------------------------------------------------
// Find pane text is including zhuyin symbol  
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsZhuyinSymbolInString(const TDesC& aWord)
    {
    const TInt len = aWord.Length();

    for (TInt index = 0; index < len; index++)
        {
        if ((aWord[index] >= KZhuyinstart) && 
            (aWord[index] < KZhuyinstart + KZhuyincount))
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------
// This language is support by this model 
// ---------------------------------------------------------
//
TBool CFindUtilChineseECE::IsSupportLanguage(TLanguage aLang)
    {
    if (iPtiEngine->GetLanguage(aLang))
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------
// Callback method to notify client about language change.
// ---------------------------------------------------------
//
TInt CFindUtilChineseECE::HandleFindRepositoryCallBack(TAny* aPtr)
    {
    TInt ret = KErrNone;
    CFindUtilChineseECE* self = static_cast<CFindUtilChineseECE*> (aPtr);

    // Get changed key either from language repository (AknFepRepository)
    // or from adaptive repository (AvkonRepository).
    TUint32 changedKey = self->iWatcher->ChangedKey();
    if ( changedKey == NCentralRepositoryConstants::KInvalidNotificationId )
        {
        changedKey = self->iWatcherAdaptive->ChangedKey();
        }


    // Update the search method if input language or any search method variable
    // changed
    if ( changedKey == KAknFepInputTxtLang ||
         changedKey == KAknAdaptiveSearchChinesePRC ||
         changedKey == KAknAdaptiveSearchChineseTW ||
         changedKey == KAknAdaptiveSearchChineseHongkong ||
         changedKey == KAknFepCangJieMode ||
         changedKey == KAknAvkonAdaptiveSearchEnabled )
        {
        TChineseSearchMethod prevSearchMethods = self->iSearchMethod;

        TRAP( ret, self->OpenL() );
        
        // Reconstruct cache if search method got changed
        if ( ret == KErrNone && prevSearchMethods != self->iSearchMethod )
            {
            TRAP(ret, self->iAlgorithm->ReconstructCacheDataL());
            }
        }
    
    return ret;
    }

// ---------------------------------------------------------
// Update input method in ptiengine from find pane
// ---------------------------------------------------------
//
void CFindUtilChineseECE::UpdateCurrentInputMode()
    {
    CCoeEnv* coeEnv = NULL;
    coeEnv = CCoeEnv::Static();
    iCurInputMode = KSysInputMode;
    if (coeEnv)
        {
        CAknEdwinState* pState = NULL;

        if (coeEnv->AppUi() == NULL)
            return;

        TCoeInputCapabilities inputCapabilities = static_cast<const CCoeAppUi*>(coeEnv->AppUi())->InputCapabilities();
        MCoeFepAwareTextEditor* fepAvareTextEditor = inputCapabilities.FepAwareTextEditor();
        if (fepAvareTextEditor)
            {
            pState = static_cast<CAknEdwinState*> (fepAvareTextEditor->Extension1()->State(KNullUid));
            if (pState)
                {
                iCurInputMode = pState->CurrentInputMode();
                }
            }
        }
    }

// The below code is commented out because current CFindUtilChineseECE is used
// on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
// for the further merging work (merge from FindUtil 5.0)
#if 0  
// ---------------------------------------------------------------------------
// UpdateNextChar
// ---------------------------------------------------------------------------
//
void CFindUtilChineseECE::UpdateNextCharsL(HBufC*& aNextChars, TChar aCh)
    {
    if( aNextChars->Locate(aCh) == KErrNotFound )
        {			
        if( aNextChars->Des().Length() == aNextChars->Des().MaxLength())
            {
            aNextChars = aNextChars->ReAllocL( aNextChars->Des().MaxLength()+10 );
            }		
        aNextChars->Des().Append(aCh);
        }
    }    

// ---------------------------------------------------------------------------
// UpdateNextCharFromString
// ---------------------------------------------------------------------------
//
void CFindUtilChineseECE::UpdateNextCharsFromStringL(HBufC*& aNextChars, const TDesC& aItemString)
    {

    // Array for item string
    RPointerArray<STRINGINFO> stringInfoArr;
    // Split item string
    TRAPD(err, SplitItemStringL(stringInfoArr, aItemString));

    if (err != KErrNone)
        {
        stringInfoArr.ResetAndDestroy();
        stringInfoArr.Close();
        }

    TInt index = 0;
    STRINGINFO* pStringInfo;
    const TInt arrayCount = stringInfoArr.Count();


    for (index = 0; index < arrayCount; index ++)
        {
        pStringInfo = stringInfoArr[index];
        if (!pStringInfo->isChinese )
            {
            UpdateNextCharsL( aNextChars, pStringInfo->segmentString[0]);	
            }
        else
            {
            TInt chineseworkcount =  pStringInfo->segmentString.Length();

            for(TInt i =0;i<chineseworkcount;i++) 
                {
                RPointerArray<HBufC> spellList;
                TBuf<KMaxWordLength> tempBuf;
                if (!DoTranslationL(TInt16(pStringInfo->segmentString[i]), spellList))
                    {
                    UpdateNextCharsL( aNextChars, pStringInfo->segmentString[i]);
                    }
                else
                    {
                    //for multiphnetic spell
                    TInt spellCount = spellList.Count();
                    // Search all spelling
                    for (TInt j = 0; j < spellCount; j++)
                        {
                        tempBuf.Copy(spellList[j]->Des());
                        UpdateNextCharsL( aNextChars, tempBuf[0]);                                        
                        }//end for
                    }
                spellList.ResetAndDestroy();//destroy spellList
                spellList.Close();       	
                }
            }
        }
    stringInfoArr.ResetAndDestroy();
    stringInfoArr.Close();

    }

// ---------------------------------------------------------------------------
// TrimChineseCharacter
// ---------------------------------------------------------------------------
//
TBool CFindUtilChineseECE::TrimChineseCharacterL(TDes& aItemString, TDes& aSearchText,HBufC*& aNextChars)
    {
    TBuf<KMaxWordLength> tempBuf;    
    TBuf<KMaxWordLength> itemString;

    tempBuf.Zero();

    tempBuf.Copy(aSearchText);

    itemString.Zero();

    itemString.Copy(aItemString);

    TInt searchMode;
    TBool InputMethodStroke= EFalse;

    if ( iLanguage == ELangPrcChinese )
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChinesePRC, searchMode);
        if(searchMode == 1)
            {
            InputMethodStroke = ETrue;
            }
        }
    else if ( iLanguage == ELangHongKongChinese)
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChineseHongkong, searchMode);
        if(searchMode == 1)
            {
            InputMethodStroke = ETrue;
            }
        }
    else if( iLanguage == ELangTaiwanChinese)
        {
        iRepositoryFindAdaptive->Get(KAknAdaptiveSearchChineseTW, searchMode);
        if(searchMode == 0)
            {
            InputMethodStroke = ETrue;
            }
        }
    TInt ii =0;
    while (ii< tempBuf.Length())
        {
        if ((TInt)tempBuf[ii]>= KMinUnicodeHz && (!InputMethodStroke
            ||(InputMethodStroke && !IsStrokeSymbol(tempBuf[ii]))))
            {
            TInt Findcursor = itemString.Locate(tempBuf[ii]);
            if (Findcursor != KErrNotFound)
                {
                if ((Findcursor == itemString.Length()-1) && (ii
                    == tempBuf.Length()-1))
                    {
                    return ETrue;
                    }
                itemString.Delete(0, Findcursor+1);
                tempBuf.Delete(0, ii+1);
                if (tempBuf.Length()==0)
                    {
                    TBuf<KMaxWordLength> usefortrim;
                    usefortrim.Append(itemString[0]);
                    UpdateNextCharsFromStringL(aNextChars, usefortrim);
                    return ETrue;
                    }
                ii=0;
                }
            else
                {
                ii++;
                }
            }
        else
            {
            ii++;
            }

        }
    aSearchText.Zero();

    aSearchText.Copy(tempBuf);

    aItemString.Zero();

    aItemString.Copy(itemString);

    return EFalse;

    }
// ---------------------------------------------------------------------------
// CheckEnglishFirst
// ---------------------------------------------------------------------------
//
TBool CFindUtilChineseECE::CheckEnglishFirstL( RPointerArray<STRINGINFO>& aarray, TDes& aSearchText,HBufC*& aNextChars)
    {
    TBool ret = EFalse ;
    if (aarray.Count() > 0)
        {
        const TInt arrayLength = aarray.Count();
        const TInt searchStrLength = aSearchText.Length();
        TBuf<KMaxWordLength> tempBuf;
        TBuf<KMaxWordLength> temp;

        TInt searchStrIndex = 0;
        STRINGINFO* pStringInfo= NULL;
        TBuf<KMaxWordLength> transSearchBuf;
        TInt index = 0;

        tempBuf.Zero();

        tempBuf.Copy(aSearchText);
        // First only check English
        for (; index < arrayLength; index ++)
            {
            transSearchBuf.Zero();
            pStringInfo = aarray[index];

            transSearchBuf = tempBuf.Mid(searchStrIndex, searchStrLength
                - searchStrIndex);

            // Compare word
            if (!pStringInfo->isChinese)
                {
                searchStrIndex += MatchStringL(pStringInfo->segmentString,
                    transSearchBuf);
                }

            if (searchStrIndex >= searchStrLength)
                {
                ret = ETrue;

                //append to next char
                if (pStringInfo->segmentString.Length() > searchStrIndex)
                    {
                    UpdateNextCharsL(aNextChars,
                        pStringInfo->segmentString[searchStrIndex]);
                    }
                break;
                }
            }
        //for english string add next character directly    
        if (ret)
            {
            if (arrayLength <= index+1)
                {
                return ret;
                }

            AppendCharInNextSegmentL(aarray,index+1,aNextChars);

            }
        }
    return ret;
    }
// ---------------------------------------------------------------------------
// AddNextIfOverlapL to check if overlap problem 
// ---------------------------------------------------------------------------
//
void CFindUtilChineseECE::AddNextIfOverlapL(const RPointerArray<STRINGINFO>& astringInfoArr ,
    const TInt aindex, const TInt aindexforfind, const TDes& aCurrentBuf,HBufC*& aNextChars )
    {
    TBuf<KMaxWordLength> temp;
    TBuf<KMaxWordLength> tempNext;
    RPointerArray<HBufC> tempSpellList;
    STRINGINFO* pStringInfo;
    STRINGINFO* pStringInfoNext;

    if(astringInfoArr.Count()>aindex)
        {
        pStringInfo= astringInfoArr[aindex];
        }

    if (pStringInfo->isChinese)
        {
        if((pStringInfo->segmentString).Length()>aindexforfind+1)//in same segment
            {
            if (DoTranslationL(TInt16((pStringInfo->segmentString)[aindexforfind+1]), tempSpellList))
                {
                temp.Copy(tempSpellList[0]->Des());
                }
            }
        else if(astringInfoArr.Count()>aindex+1)//next segment should be english
            {
            temp.Copy(astringInfoArr[aindex+1]->segmentString);
            }
        }
    else
        {
        if(astringInfoArr.Count()>aindex+1)
            {
            pStringInfo= astringInfoArr[aindex+1]; //next segment should be chinese
            if (!DoTranslationL(TInt16((pStringInfo->segmentString)[0]), tempSpellList))
                {
                temp.Copy(pStringInfo->segmentString);
                }
            else
                {
                temp.Copy(tempSpellList[0]->Des());
                }
            }

        }
    tempSpellList.ResetAndDestroy();
    if(ReverseMatchStringL(aCurrentBuf,temp)>0)
        {
        if (pStringInfo->isChinese)
            {
            if((pStringInfo->segmentString).Length()>aindexforfind+2)//in same segment
                {
                if (!DoTranslationL(TInt16((pStringInfo->segmentString)[aindexforfind+2]), tempSpellList))
                    {
                    tempNext.Append((pStringInfo->segmentString)[aindexforfind+2]);
                    }
                else
                    {
                    tempNext.Copy(tempSpellList[0]->Des());
                    }
                }
            else if(astringInfoArr.Count()>aindex+2)//next segment should be english
                {
                pStringInfoNext = astringInfoArr[aindex+2];
                if(pStringInfoNext->isChinese)
                    {
                    if (!DoTranslationL(TInt16((pStringInfoNext->segmentString)[0]),
                        tempSpellList))
                        {
                        tempNext.Append((pStringInfoNext->segmentString)[0]);
                        }
                    else
                        {
                        tempNext.Copy(tempSpellList[0]->Des());
                        }
                    }
                else
                    {
                    tempNext.Copy(pStringInfoNext->segmentString);
                    }
                }
            }
        else
            {
            if(astringInfoArr.Count()>aindex+2)
                {
                pStringInfo= astringInfoArr[aindex+2]; //next segment should be chinese
                if (!DoTranslationL(TInt16((pStringInfo->segmentString)[0]), tempSpellList))
                    {
                    tempNext.Copy(pStringInfo->segmentString);
                    }
                else
                    {
                    tempNext.Copy(tempSpellList[0]->Des());
                    }
                }        
            }
        }

    if(tempNext.Length()>0)
        {
        UpdateNextCharsL(aNextChars,tempNext[0]);
        }  
    tempSpellList.ResetAndDestroy();
    tempSpellList.Close();
    }
// ---------------------------------------------------------------------------
// AppendCharInNextSegmentL this segment is english
// ---------------------------------------------------------------------------
//
void CFindUtilChineseECE::AppendCharInNextSegmentL(const RPointerArray<STRINGINFO>& astringInfoArr ,
    const TInt aindex,HBufC*& aNextChars)
    {
    TBuf<KMaxWordLength> temp;

    RPointerArray<HBufC> tempSpellList;

    if ( !astringInfoArr[aindex]->isChinese)
        {
        UpdateNextCharsL(aNextChars,
            astringInfoArr[aindex]->segmentString[0]);
        }
    else
        {
        if (!DoTranslationL(
            TInt16(astringInfoArr[aindex]->segmentString[0]),
            tempSpellList))
            {
            UpdateNextCharsL(aNextChars,
                astringInfoArr[aindex]->segmentString[0]);
            }
        else
            {
            //for multiphnetic spell
            TInt spellCount = tempSpellList.Count();
            // Search all spelling
            for (TInt j = 0; j < spellCount; j++)
                {
                temp = tempSpellList[j]->Des();
                UpdateNextCharsL(aNextChars, temp[0]);
                }//end for
            }
        }
    tempSpellList.ResetAndDestroy();
    tempSpellList.Close();
    }

// ---------------------------------------------------------------------------
// AppendNextSegmentFirstChar this segment is chinese
// ---------------------------------------------------------------------------
//
TBool CFindUtilChineseECE::AppendNextSegmentFirstCharL(const RPointerArray<STRINGINFO>& astringInfoArr ,
    const TInt aindex ,const TInt aindexforfind,const TDes& aCurrentBuf ,const TDes& aTranSearchBuf,HBufC*& aNextChars)
    {
    STRINGINFO* pStringInfo= astringInfoArr[aindex];
    TBuf<KMaxWordLength> temp;
    TInt ret;
    if (pStringInfo->isChinese)
        {
        if (aCurrentBuf.Length() > aindexforfind + 1)
            {
            RPointerArray<HBufC> tempSpellList;
            if (!DoTranslationL(
                TInt16(aCurrentBuf[aindexforfind + 1]),
                tempSpellList))
                {
                UpdateNextCharsL(aNextChars,
                    aCurrentBuf[aindexforfind + 1 ]);
                }
            else
                {
                //for multiphnetic spell
                TInt spellCount = tempSpellList.Count();
                // Search all spelling
                for (TInt j = 0; j < spellCount; j++)
                    {
                    temp = tempSpellList[j]->Des();
                    UpdateNextCharsL(aNextChars, temp[0]);
                    if (temp.Length()>1)
                        {
                        TBuf<KMaxWordLength>
                        useforminan(aTranSearchBuf);
                        useforminan.Append(temp[1]);
                        if (MatchRefineL(aCurrentBuf, useforminan))
                            {
                            UpdateNextCharsL(aNextChars,temp[1]);
                            }
                        }
                    }//end for                               
                }
            tempSpellList.ResetAndDestroy();
            tempSpellList.Close();
            }
        else
            {
            //if space exist in chinese words, it will split them into differenct segments.
            if (aindex < astringInfoArr.Count() - 1)
                {
                STRINGINFO* pStringInfo1 =
                astringInfoArr[aindex + 1];
                if (pStringInfo1->isChinese)
                    {
                    RPointerArray<HBufC> tempSpellList;
                    if (!DoTranslationL(TInt16((pStringInfo1->segmentString)[0]),
                        tempSpellList))
                        {
                        UpdateNextCharsL(aNextChars,(pStringInfo1->segmentString)[0]);
                        }
                    else
                        {
                        //for multiphnetic spell
                        TInt spellCount = tempSpellList.Count();
                        // Search all spelling
                        for (TInt j = 0; j < spellCount; j++)
                            {
                            temp = tempSpellList[j]->Des();
                            UpdateNextCharsL(aNextChars,temp[0]);
                            }//end for
                        }
                    tempSpellList.ResetAndDestroy();
                    tempSpellList.Close();
                    }
                else
                    {
                    UpdateNextCharsL(aNextChars,
                        pStringInfo1->segmentString[0]);
                    }
                }
            }

        }
    else
        {
        //have no next element in array
        if (astringInfoArr.Count() <= aindex+1)
            {
            ret = ETrue;
            return ret;
            }

        AppendCharInNextSegmentL(astringInfoArr,aindex+1,aNextChars);
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// IsAdaptiveFindMatchL
// ---------------------------------------------------------------------------
//
TBool CFindUtilChineseECE::IsAdaptiveFindMatchL( const TDesC& aItemString, const TDesC& aSearchText,
    HBufC*& aNextChars )
    {

    TBuf<KMaxWordLength> tempBuf;    
    TBuf<KMaxWordLength> itemString;

    TBool ret = EFalse;

    tempBuf.Zero();
    if ( KMaxWordLength > aSearchText.Length() )
        {
        tempBuf.Copy( aSearchText );
        }
    else
        {
        tempBuf.Copy( aSearchText.Left( KMaxWordLength ) );
        }
    
    itemString.Zero();
     
    if ( KMaxWordLength > aItemString.Length() )
        {
        itemString.Copy( aItemString );
        }
    else
        {
        itemString.Copy( aItemString.Left( KMaxWordLength ) );
        }
    
    RemoveSeparator(tempBuf);

    RPointerArray<HBufC> spellList;

    //trim the sepcial char
    if(itemString.Length()>0)
        {
        if(itemString[0]== KLeftToRightFlag || itemString[0]== KRightToLeftFlag)
            {
            itemString.Delete(0,1);
            itemString.Delete((itemString.Length()-1),1);     
            } 
        }                

    //Find if search string has chinese word. 
    //Compare with item string if find chinese word in item string then delete it from search and item string.

    if(IsChineseWordIncluded(tempBuf))
        {
        if(TrimChineseCharacterL(itemString,tempBuf,aNextChars))
            {
            spellList.ResetAndDestroy();//destroy spellList
            spellList.Close();
            return ETrue;
            }
        }

    // Array for item string
    RPointerArray<STRINGINFO> stringInfoArr;
    // Split item string
    SplitItemStringL(stringInfoArr, itemString);

    // For store temp data 
    TBuf<KMaxWordInterpretationLen> temp;

    //First only check English

    if(CheckEnglishFirstL(stringInfoArr,tempBuf,aNextChars))
        {
        spellList.ResetAndDestroy();//destroy spellList
        spellList.Close();
        stringInfoArr.ResetAndDestroy();
        stringInfoArr.Close();
        return ETrue;
        }

    const TInt arrayLength = stringInfoArr.Count();
    const TInt searchStrLength = tempBuf.Length();
    TInt searchStrIndex = 0;
    STRINGINFO* pStringInfo= NULL;
    TBuf<KMaxWordLength> transSearchBuf;
    TInt index = 0; 
    TInt indexforfind =0;
    TInt indexlast=0;

    TBuf<KMaxWordLength> bufcurrent;
    TBuf<KMaxWordLength> tempbufcurrent;

    // Find Chinese and English
    for (; index < arrayLength; index ++)
        {
        TInt matchLength = 0;
        transSearchBuf.Zero();
        pStringInfo = stringInfoArr[index];

        transSearchBuf = tempBuf.Mid(searchStrIndex, searchStrLength
            - searchStrIndex);
        if (transSearchBuf.Length()==0)
            {
            break;
            }

        bufcurrent = pStringInfo->segmentString;

        for (TInt j =0; j < pStringInfo->segmentString.Length(); j++)
            {
            TInt indexforfindtemp = 0;
            TInt searchStrIndextemp = 0;
            if (pStringInfo->isChinese)
                {
                tempbufcurrent = bufcurrent.Mid(j);
                matchLength = MatchChineseStringL(tempbufcurrent,
                    transSearchBuf, indexforfindtemp);
                searchStrIndextemp +=matchLength;
                if (indexforfindtemp>=indexforfind)
                    {
                    if (indexlast == index && searchStrIndextemp
                        >= searchStrIndex)
                        {
                        searchStrIndex = searchStrIndextemp;
                        }
                    else
                        if (indexlast != index)
                            {
                            searchStrIndex += searchStrIndextemp;
                            }
                    indexforfind = indexforfindtemp;
                    indexlast = index;
                    }
                }
            else
                {//english words and chinese words don't coexist in same segment
                matchLength = MatchStringL(bufcurrent, transSearchBuf);
                searchStrIndex +=matchLength;
                if (searchStrIndex >= searchStrLength)
                    {
                    //append next char in the same segment
                    ret = ETrue;

                    if (bufcurrent.Length() > matchLength)
                        {
                        UpdateNextCharsL(aNextChars, bufcurrent[matchLength]);
                        }
                    if (arrayLength <= index+1)
                        {
                        spellList.ResetAndDestroy();//destroy spellList
                        spellList.Close();
                        stringInfoArr.ResetAndDestroy();
                        stringInfoArr.Close();
                        return ret;
                        }

                    AppendCharInNextSegmentL(stringInfoArr,index+1,aNextChars);

                    }
                break;
                }
            // Append next character in same segment this segment is chinese
            if (searchStrIndex >= searchStrLength)
                {
                ret = ETrue;
                RPointerArray<HBufC> tempSpellList;
                if (tempbufcurrent.Length()>indexforfind)
                    {
                    if (!DoTranslationL(TInt16(tempbufcurrent[indexforfind]),
                        tempSpellList))
                        {
                        UpdateNextCharsL(aNextChars,
                            tempbufcurrent[indexforfind]);
                        }
                    else
                        {
                        //for multiphnetic spell
                        TInt spellCount = tempSpellList.Count();
                        TInt matchMaxIndex = 0;
                        TInt matchMax = 0;
                        TInt matchMaxPre = 0;
                        // Search all spelling
                        for (TInt j = 0; j < spellCount; j++)
                            {
                            //for multiphnetic spell
                            TInt spellCount = tempSpellList.Count();
                            TInt matchMaxIndex = 0;
                            TInt matchMax = 0;
                            TInt matchMaxPre = 0;
                            TBool fullmatch = EFalse;
                            TBool offset = KErrNotFound;
                            // Search all spelling
                            for (TInt j = 0; j < spellCount; j++)
                                {
                                temp = tempSpellList[j]->Des();
                                for (TInt k = 1; k <= searchStrLength; k++)
                                    {
                                    TBuf<KMaxWordLength> tmpBuf = tempBuf.Mid(
                                        searchStrLength - k, k);
                                    offset = temp.Find(tmpBuf);
                                    if (offset != KErrNotFound)
                                        {
                                        TBuf<KMaxWordLength> buf = temp.Mid(0,k);
                                        if (buf == tmpBuf)
                                            {
                                            fullmatch = ETrue;
                                            }
                                        }
                                    if (offset != KErrNotFound && fullmatch)
                                        {
                                        matchMax = k;
                                        }
                                    fullmatch = EFalse;
                                    }
                                if (matchMax > matchMaxPre)
                                    {
                                    matchMaxPre = matchMax;
                                    matchMaxIndex = j;
                                    }
                                if (matchMax == searchStrLength && fullmatch)
                                    {
                                    if (matchMaxPre <temp.Length())
                                        {
                                        UpdateNextCharsL(aNextChars, temp[matchMax]);
                                        }
                                    }
                                fullmatch = EFalse;
                                offset = KErrNotFound;
                                }
                            temp = tempSpellList[matchMaxIndex]->Des();
                            if (matchMaxPre <temp.Length())
                                {
                                UpdateNextCharsL(aNextChars,temp[matchMaxPre]);
                                }
                            }
                        tempSpellList.ResetAndDestroy();
                        tempSpellList.Close();
                        }
                    }
                //Append first translated character or english letter in next segment.
                if (ret)
                    { 
                    AppendNextSegmentFirstCharL(stringInfoArr,index,
                        indexforfind,tempbufcurrent,transSearchBuf,aNextChars);

                    AddNextIfOverlapL(stringInfoArr,index,indexforfind+j,transSearchBuf,aNextChars);
                    }
                searchStrIndex = 0;
                }
            indexforfind = 0;
            }
        }

    spellList.ResetAndDestroy();//destroy spellList
    spellList.Close(); 		
    stringInfoArr.ResetAndDestroy();
    stringInfoArr.Close();

    return ret;
    }

// -----------------------------------------------------------------------------
// CFindUtilChineseECE::MatchAdaptiveRefineL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TBool CFindUtilChineseECE::MatchAdaptiveRefineL( const TDesC& aItemString, 
    const TDesC &aSearchText, HBufC*& aNextChars )
    {
    //Get default adaptive search method.
    if ( aItemString.Length() == 0 )
        {
        return EFalse;
        }
    if ( aSearchText.Length() == 0 )
        {
        UpdateNextCharsFromStringL( aNextChars, aItemString );
        return ETrue;
        } 

    return IsAdaptiveFindMatchL( aItemString, aSearchText, aNextChars);		    

    }
#endif

void CFindUtilChineseECE::GetPartOfQueryL(CPsQuery& aSrcQuery, TInt aStartIndex, 
    TInt aEndIndex, CPsQuery& aDestQuery)
    {
    TInt index = aStartIndex;

    if (aStartIndex <= aEndIndex && aStartIndex >= 0 && aSrcQuery.Count()>0 )
        {
        for (; index <= aEndIndex; index++)
            {
            // Add a query item
            CPsQueryItem* newItem = CPsQueryItem::NewL();
            CPsQueryItem& tempItem = aSrcQuery.GetItemAtL(index);

            newItem->SetCharacter(tempItem.Character());
            newItem->SetMode(tempItem.Mode());

            aDestQuery.AppendL(*newItem);
            }
        }
    else 
        {
        User::Panic(KPanicReason, 0);
        }
    }

void CFindUtilChineseECE::RemoveSeparatorL(CPsQuery& aQuery)
    {
    TInt index = 0;

    for ( ; index<aQuery.Count(); index++ )
        {
        CPsQueryItem& queryItem = aQuery.GetItemAtL( index );
        switch( queryItem.Character() )
            {
            case '_': 
            case '-':
            case ' ':
            case '\t':
                {
                // remove the charator of index
                aQuery.Remove( index );
                // ajust index after remove
                index--; 
                }
                break;
            default:
                continue;
            } // switch
        } // for
    }
// end of file
