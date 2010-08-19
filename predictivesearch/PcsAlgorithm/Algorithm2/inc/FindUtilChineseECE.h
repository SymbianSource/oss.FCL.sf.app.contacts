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
* Description:  Identifies a contact in cache pools
*
*/

#ifndef __FINDUTILCHINESE_ECE__
#define __FINDUTILCHINESE_ECE__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class CPtiEngine;
class CPcsAlgorithm2;
class CFindRepositoryWatcher;
class CRepository;
class CPsQuery;

// CONSTANTS
const TInt KMaxWordLength = 128;
const TInt KMaxWordInterpretationLen = 255;
/**

Defines flags that can be used to indicate what is the default search method for each language.
*/

enum TChineseSearchMethod
    {
    EAdptSearchPinyin =0,
    EAdptSearchStroke,
    EAdptSearchZhuyin,
    EAdptSearchNormalCangjie,
    EAdptSearchEasyCangjie,
    EAdptSearchAdvCangjie
    };
/**
 *  STRINGINFO struct
 *
 *  This data struct is for splited text in find pane. 
 *  It includes two type word, which is Chinese or no-Chinese 
 *
 *  @lib FindUtil.dll
 *  @since S60 v3.2
 */
struct STRINGINFO
    {
    TBuf<KMaxWordLength> segmentString;
    TBool isChinese;
    TInt segmentPos;
    };

/**
 *  CFindUtilChineseECE utils class
 *
 *  This class is Chinese find utils class. It handle Chinese variant find
 *  in PhoneBook, LandMark, Clock and so on.
 *
 *  @lib FindUtil.dll
 *  @since S60 v3.2
 */
class CFindUtilChineseECE : public CBase
    {
public:

    /**
     * Match arithmetic for general search
     *
     * @since S60 v3.2
     * @param aContactsField Reference to the searching target
     * @param aWord Reference to text in find pane
     * @return ETrue for the text are matched; otherwise EFalse
     */
    virtual TBool Match(const TDesC& aContactsField, const TDesC& aWord);

    /**
     * Match arithmetic for accurate search 
     *
     * @since S60 v3.2
     * @param aItemString Reference to the searching target
     * @param aPsQuery Reference to the search query
     * @return ETrue for the text are matched; otherwise EFalse
     */
    virtual TBool MatchRefineL(const TDesC& aItemString, CPsQuery& aPsQuery);

    /**
     * Match arithmetic for accurate search 
     *
     * @since S60 v5.2
     * @param aItemString Reference to the searching target
     * @param aPsQuery Reference to the search query
     * @param aMatchPos On return, contain positions of matching parts within aItemString
     * @param aMatchLength On return, contains lenghts of matching parts within aItemString
     * @param aHighLight  If highlight or underline position of matched contact is enabled
     * @return ETrue for the text are matched; otherwise EFalse
     */
    TBool MatchRefineL(const TDesC& aItemString, CPsQuery& aPsQuery, 
        RArray<TInt>& aMatchPos, RArray<TInt>& aMatchLength, TBool aHighLight );

    /**
     * Current input text is handled by this model 
     *
     * @since S60 v3.2
     * @param aWord Reference to text in find pane
     * @return ETrue for handling by this model; otherwise EFalse
     */
    virtual TBool IsWordValidForMatching(const TDesC& aWord);

    // The below code is commented out because current CFindUtilChineseECE is used
    // on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
    // for the further merging work (merge from FindUtil 5.0)
#if 0
        /**
         * From MFindUtil It tests a partial matching.
         * @since 5.0
         * @param aItemString is a field string
         * @param aSearchText is a search string
         * @param aNextChars is a reference to buffer where next possible characters are saved. 
         *        The HBufC buffer may be re-allocated by this method. 
         *        In that case the pointer reference is modified to point to the re-allocated object.                              
         * @return If aSearchText has been found in aItemString, 
         *         ETrue is returned.
         */
        TBool MatchAdaptiveRefineL( const TDesC& aItemString, 
        							const TDesC& aSearchText, 
        							HBufC*& aNextChars);
#endif
        
    /**
     * Create ptiengine and initialize member data 
     *
     * @since S60 v3.2
     * @return none
     */
    virtual void OpenL();

    /**
     * From CFindUtilBase
     * Close ptiEngine 
     *
     * @since S60 v3.2
     * @return none
     */
    virtual void Close();

    /**
     * Symbian constructor
     *
     * @since S60 v3.2
     * @return Pointer to created CFindUtilChineseECE object
     */
    static CFindUtilChineseECE* NewL(CPcsAlgorithm2* aAlgorithm);

  

public:

    /**
     * C++ default constructor
     *
     * @since S60 v3.2
     * @return None
     */
    CFindUtilChineseECE();

    /**
     * Destructor
     *
     * @since S60 v3.2
     * @return None
     */
    ~CFindUtilChineseECE();

public:

    /**
     * Translate Chinese word to its spelling
     *
     * @since S60 v3.2
     * @param aHZUnicode For translate word
     * @param aSpellList Reference to spelling list for polyphonic word
     * @return ETrue for this word can be translated; otherwise EFalse
     */
    TBool DoTranslationL(TInt16 aHZUnicode, RPointerArray<HBufC>& aSpellList);

    /**
     * Find pane text is including Chinese word
     *
     * @since S60 v3.2
     * @param aWord Reference to text for analyse
     * @return ETrue it includes Chinese; otherwise EFalse
     */
    TBool IsChineseWordIncluded(const TDesC& aWord);

    /*
     * 
     */
    inline TLanguage CurrentInputLanguage()
        {
        return iLanguage;
        }

    inline TChineseSearchMethod CurrentSearchMethod()
        {
        return iSearchMethod;
        }
    
private:

    /**
     * Symbian second-phase constructor
     *
     * @since S60 v3.2
     * @return None
     */
    void ConstructL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Open ptiengine and active it by input language
     *
     * @since S60 v3.2
     * @param aLanguage Actived language
     * @return ETrue for opening ptiengine successfully; otherwise EFalse
     */
    TBool OpenT9InterfaceL(TLanguage aLanguage);

    /**
     * Close ptiengine
     *
     * @since S60 v3.2
     * @return none
     */
    void CloseT9InterfaceL();

    /**
     * This letter is stroke symbol
     *
     * @since S60 v3.2
     * @param aFindWord For analyse
     * @return ETrue for it is stroke symbol; otherwise EFalse
     */
    TInt IsStrokeSymbol(const TUint16 aFindWord);
    
    /**
     * Do translate for Chinese word
     *
     * @since S60 v3.2
     * @param aHZUnicode For translate word
     * @param aSpellList Reference to spelling list for polyphonic word
     * @return ETrue for it can be translated; otherwise EFalse
     */
    TBool T9ChineseTranslationL(TInt16 aHZUnicode, RPointerArray<HBufC>& aSpellList);

    /**
     * Do translate for Chinese word
     *
     * @since S60 v5.0.1
     * @param aHZUnicode For translate word
     * @param aSpellList Reference to spelling list for polyphonic word
     * @return ETrue for it can be translated; otherwise EFalse
     */
    TBool T9ChineseTranslationAdaptiveL(TInt16 aHZUnicode, RPointerArray<HBufC>& aSpellList);
    
    /**
     * Find pane text is including separator
     *
     * @since S60 v3.2
     * @param aCh Analyze word
     * @return ETrue it is separator; otherwise EFalse
     */
    TBool IsFindWordSeparator(TChar aCh);

    /**
     * Find pane text is including stroke symbol
     *
     * @since S60 v3.2
     * @param aWord Reference to text for analyse
     * @return ETrue it is stroke symbol; otherwise EFalse
     */
    TBool IsStrokeSymbolInString(const TDesC& aWord);

    /**
     * Find pane text is just Chinese word
     * 
     * @param aWord Reference to text for analyse
     * @return ETrue it is all Chinese word(s); otherwise EFalse
     */
    TBool IsAllChineseWord(const TDesC& aWord);
    
    /**
     * Find pane text is including zhuyin symbol
     *
     * @since S60 v3.2
     * @param aWord Reference to text for analyse
     * @return ETrue it includes zhuyin symbol; otherwise EFalse
     */
    TBool IsZhuyinSymbolInString(const TDesC& aWord);

    /**
     * This language is support by this model
     *
     * @since S60 v3.2
     * @param aLang Language for support
     * @return ETrue it supports; otherwise EFalse
     */
    TBool IsSupportLanguage(TLanguage aLang);

    /**
     * Callback method to notify client about language change.
     *
     * @since S60 v3.2
     * @param aPtr Pointer to the callback data
     * @return Error code
     */
    static TInt HandleFindRepositoryCallBack(TAny* aPtr);

    /**
     * Splite the input text to sgement by language
     *
     * @since S60 v3.2
     * @param aStringInfoArr Reference to segment list
     * @param aItemString Reference to input text
     * @return none
     */
    void SplitItemStringL(RPointerArray<STRINGINFO>& aStringInfoArr, const TDesC &aItemString);

    /**
     * Insert segment to list
     *
     * @since S60 v3.2
     * @param aStringInfoArr Reference to segment list
     * @param aSegmentStr Reference to segment text
     * @param aChinese The segment is Chinese
     * @param aIndexAfterStr The first index in the source string after the aSegmentStr.
     *                       The indices of characters of aSegmentStr are 
     *                       [aIndexAfterStr-aSegmementStr.Length(), aIndexAfterStr-1]
     * @return none
     */
    void InsertStrInforArrayL(RPointerArray<STRINGINFO>& aStringInfoArr,
                              TDes &aSegmentStr, TBool aChinese, TInt aIndexAfterStr);

    /**
     * This segment is matched by search text
     *
     * @since S60 v3.2
     * @param aStringInfoArr Reference to segment list
     * @param aSearchText Reference to searching text
     * @param aMatchPos On return, contain positions of matching parts within the original searcg target text
     * @param aMatchLength On return, contains lenghts of matching parts within the original search target text
     * @param aHighLight  If highlight or underline position of matched contact is enabled
     * @return ETrue it is matched; otherwise EFalse
     */
    TBool MatchSegmentL(RPointerArray<STRINGINFO>& aStringInfoArr, 
        CPsQuery& aQuery, RArray<TInt>& aMatchPos, RArray<TInt>& aMatchLength, TBool aHighLight );

    /**
     * Search text by reverse
     *
     * @since S60 v3.2
     * @param aFirst Reference to matched text
     * @param aSecond Reference to matched text
	 * @param aFullQuery Reference to the FullQuery
     * @return Matched count
     */
    TInt ReverseMatchStringL(const TDesC& aFirst, const TDesC& aSecond, CPsQuery& aFullQuery );

    /**
     * Search text in other text 
     *
     * @since S60 v3.2
     * @param aFirst Reference to matched text
     * @param aSecond Reference to matched text
	 * @param aFullQuery Reference to the FullQuery
     * @return Matched count
     */
    TInt MatchStringL(const TDesC& aFirst, const TDesC& aSecond, CPsQuery& aFullQuery);
    
    /**
     * Search text in other text 
     *
     * @since S60 v3.2
     * @param aSearchTargetString Data to be searched through
     * @param aQuery Text to be searched from aSearchTargetString
     * @return Number of matched characters from the begining of aQuery
     */
    TInt MatchStringL(const TDesC& aSearhTargetString, CPsQuery& aQuery);

    /**
     * Search Chinese word in input text 
     *
     * @since S60 v3.2
     * @param aSearchTargetString Data to be searched through
     * @param aQuery Text to be searched from aSearchTargetString
     * @param aMatchStrtIdx On return, will contain the start index 
     *                      of the matching part within aSearchTargetString
     * @param aMatchEndIdx On return, will contain the end index 
     *                     of the matching part within aSearchTargetString
     * @return Number of matched characters from the begining of aQuery
     */
    TInt MatchChineseStringL(const TDesC& aSearchTargetString, CPsQuery& aQuery, TInt& aMatchStartIdx, TInt& aMatchEndIdx);

    /**
     * Initial character search 
     *
     * @since S60 v3.2
     * @param aStringInfoArr Reference to spelling list
     * @param aSearchStr Reference to search text
     * @return ETrue it is matched; otherwise EFalse
     */
    TBool MatchChineseInitialStringL(RPointerArray<STRINGINFO>& aStringInfoArr,
                                     const TDesC& aSearchStr);

    /**
     * Maximal matched count in spelling list  
     *
     * @since S60 v3.2
     * @param aSpellList Reference to spelling list
     * @param aSearchStr Reference to search text
     * @param aPreviouStr Reference to previous matched word
     * @param aMatchIndex Reference to match index in Search text
     * @param aFullMatched Previous word is fully matched
     * @param aMatchedCount Previous word matched count
     * @param aAgain Search again
	 * @param aFullQuery Reference to the FullQuery
     * @return Matched count
     */
    TInt MaxMatchInListL(RPointerArray<HBufC>& aSpellList, CPsQuery& aQuery,
                        TDesC& aPreviouStr, TInt& aMatchIndex, const TBool aFullMatched,
                        const TInt aMatchedCount, TBool& aAgain, TBool& aPrevReplaced, CPsQuery& aFullQuery);

    /**
     * Search the taxt is include the input text 
     *
     * @since S60 v3.2
     * @param aFirst Reference to matched text
     * @param aSecond Reference to matched text
     * @return Matched count
     */
    TBool IncludeString(const TDesC& aFirst, const TDesC& aSecond);

    /**
     * Update input method in ptiengine from find pane
     *
     * @since S60 v3.2
     * @return none
     */
    void UpdateCurrentInputMode();

    /**
     * Remove separator from search text
     *
     * @since S60 v3.2
     * @param aSearchText Reference to search text
     * @return none
     */
    void RemoveSeparator(TDes& aSearchText);
    
    // The below code is commented out because current CFindUtilChineseECE is used
    // on 3.2.3 which is not supported adaptive search. It will be easy to keep these code 
    // for the further merging work (merge from FindUtil 5.0)
#if 0
     /**
     * From MFindUtil It return next char.
     * @since 5.0
     * @param aNextChars is a buffer where next possible characters are saved
     * @param aCh is a char need to be input to aNextChars
     * 
     */
    void UpdateNextCharsL(HBufC*& aNextChars, TChar aCh);
     /**
     * From MFindUtil It return next chars.
     * @since 5.0
     * @param aNextChars is a buffer where next possible characters are saved
     * @param aItemString is a Contact field string
     */
    void UpdateNextCharsFromStringL(HBufC*& aNextChars, const TDesC& aItemString);   
    
     /**
     * From MFindUtil It return next chars.
     * @since 5.0
     * @param aNextChars is a buffer where next possible characters are saved
     * @param aItemString is a Contact field string
     * @param aSearchText Reference to search text
     * @param aInputLang for the input language 
     */
    TBool IsAdaptiveFindMatchL( const TDesC& aItemString, const TDesC& aSearchText,
                               HBufC*& aNextChars);
                               
    
    /**
    * Trim chinese charater if it is exist in search text
    * @since 5.0
    * @Param aItemString is a contact field string
    * @param aSearchText is a search  text   
    * @param aNextChars is a buffer to store the charcaters needed to display.    
    */ 
    TBool TrimChineseCharacterL(TDes& aItemString, TDes& aSearchText,HBufC*& aNextChars);

    /**
       * Trim chinese charater if it is exist in search text
       * @since 5.0
       * @Param aarray is a contact field string which had to segments
       * @param aSearchText is a search  text
       * @param aNextChars is a buffer to store the charcaters needed to display.    
       */ 
    TBool CheckEnglishFirstL( RPointerArray<STRINGINFO>& aarray, TDes& aSearchText,HBufC*& aNextChars);
    
    /**
     * Trim chinese charater if it is exist in search text
     * @since 5.0
     * @Param AstringInfoArr is a contact field string which had to segments
     * @param aindex is the current find cursor
     * @param aNextChars is a buffer to store the charcaters needed to display.    
     */ 
    void AppendCharInNextSegmentL(const RPointerArray<STRINGINFO>& astringInfoArr ,
            const TInt aindex,HBufC*& aNextChars);
    
    /**
     * Trim chinese charater if it is exist in search text
     * @since 5.0
     * @Param AstringInfoArr is a contact field string which had to segments
     * @param aindex is the current find cursor
     * @paran aindexforfind is the temp cursor for find
     * @param aCurrentBuf the data in this segment
     * @param aTranSearchBuf is the left search data
     * @param aNextChars is a buffer to store the charcaters needed to display.    
     */ 
    
    TBool AppendNextSegmentFirstCharL(const RPointerArray<STRINGINFO>& astringInfoArr ,
            const TInt aindex ,const TInt aindexforfind ,
            const TDes& aCurrentBuf ,const TDes& aTranSearchBuf ,
            HBufC*& aNextChars);
    
    /**
         * Check and add next char if existed overlap problem (tang ang)
         * @since 5.0
         * @Param AstringInfoArr is a contact field string which had to segments
         * @param aindex is the current find cursor
         * @paran aindexforfind is the temp cursor for find
         * @param aCurrentBuf is the search string
         * @param aNextChars is a buffer to store the charcaters needed to display.    
         */ 
    void AddNextIfOverlapL(const RPointerArray<STRINGINFO>& astringInfoArr ,
                const TInt aindex,const TInt aindexforfind ,
                const TDes& aCurrentBuf,HBufC*& aNextChars );
#endif

    // help function for using CPsQuery
public:
    void GetPartOfQueryL(CPsQuery& aSrcQuery, TInt aStartIndex, 
                         TInt aEndIndex, CPsQuery& aDestQuery);
private:
    void RemoveSeparatorL(CPsQuery& aQuery);
    

private:

    /**
     * Ptiengine for Chinese translation (Own)
     */
    CPtiEngine* iPtiEngine;

    /**
     * Current text input language 
     */
    TLanguage iLanguage;

    /**
     * Current input mode
     */
    TInt iCurInputMode;

    /**
     * Repository for input mode (Own)
     */
    CRepository* iRepositoryFind;

    /**
     * The repository watcher(Own);
     */
    CFindRepositoryWatcher* iWatcher;
   
    /**
     * Repository for input mode (Own)
     */     
    CRepository* iRepositoryFindAdaptive;

    /**
     * The repository watcher(Own);
     */
    CFindRepositoryWatcher* iWatcherAdaptive;
    
   
    /**
     * The default adaptive search method;
     */
    TChineseSearchMethod iSearchMethod;
    
    /**
     * The search method;
     */
    TBool iSearchMethodAdaptive;

    CPcsAlgorithm2* iAlgorithm;

    };

#endif // __FINDUTILCHINESE_ECE__
// End of File
