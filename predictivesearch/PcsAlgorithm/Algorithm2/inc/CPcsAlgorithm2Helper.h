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

#ifndef C_PCS_ALGORITHM_2_HELPER
#define C_PCS_ALGORITHM_2_HELPER

// INCLUDES
#include <e32cmn.h>
#include <e32math.h>
#include <e32hashtab.h>
#include <MPbkGlobalSetting.h> // For name order setting

#include "CPcsPlugin.h"
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPcsAlgorithm2FilterHelper.h"
#include "mdatastoreobserver.h"

// FORWARD DECLARATION
class CPcsAlgorithm2;
class CPsData;
class CPsDataPluginInterface;

// CLASS DECLARATION
class CPcsAlgorithm2Helper : public CBase, private MPbkGlobalSettingObserver
    {
public:

    /**
     * Two phase construction
     */
    static CPcsAlgorithm2Helper* NewL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Destructor
     */
    virtual ~CPcsAlgorithm2Helper();

private:

    /**
     * Filter subset results for Mixed mode. Some portion of query can be in 
     * ITU-T mode and some portion in Qwerty mode.
     */
    void FilterResultsSingleL(CPcsAlgorithm2FilterHelper* aAlgorithmFilterHelper,
                              RPointerArray<CPcsPoolElement>& aSearchSet,
                              CPsQuery& aSearchQuery,
                              TUint8 aFilteredDataMatch,
                              TBool aIsSearchInGroup,
                              const RArray<TInt>& aContactsInGroup);
    
    /**
     * Create a buffer containing the first name and last name of a contact in the
     * order specified in Phonebook settings.
     */
    HBufC* CreateNameBufLC( const CPsData& aContactData, TInt aFirstNameIndex, TInt aLastNameIndex ) const;
    
    /**
     * Get and store the order of first and last name.
     */
    void UpdateNameOrderL();

private: // from MPbkGlobalSettingObserver
    /**
     * React to change in Phonebook settings.
     */
    void SettingChangedL( MPbkGlobalSetting::TPbkGlobalSetting aKey );
    
public:

    /**
     * Search Function for mixed style
     * Some chars are entered in ITU-T and some in QWERTY
     */
    void SearchSingleL(const CPsSettings& aSettings, 
                       CPsQuery& aQuery,
                       TBool aIsGroupSearch, 
                       const RArray<TInt>& aContactsInGroup,
                       RPointerArray<CPsData>& aSearchResults,
                       RPointerArray<CPsPattern>& aSearchSeqs);

    /**
     * Funciton to search matching sequences in the input text
     */
    void SearchMatchSeqL(CPsQuery& aPsQuery,
                         const TDesC& aData,
                         RPointerArray<TDesC>& aMatchSet,
                         RArray<TPsMatchLocation>& aMatchLocation);

    /**
     * Sort search sequences
     */
    void SortSearchSeqsL(RPointerArray<TDesC>& aMatchSet);

private:

    /**
     * Constructor
     */
    CPcsAlgorithm2Helper();

    /**
     * 2nd phase construtor
     */
    void ConstructL(CPcsAlgorithm2* aAlgorithm);

private:

    /**
     * Pointer to algorithm instance. Not owned.
     */
    CPcsAlgorithm2* iAlgorithm;

    /**
     * Pointer to key map instance. Not owned.
     */
    CPcsKeyMap* iKeyMap;

    /**
     * Array of result sets from different data stores.
     */
    typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;
    RPointerArray<CPSDATA_R_PTR_ARRAY> iSearchResultsArr;
    
    /**
     * Phonebook settings store for reading name order.
     */
    MPbkGlobalSetting* iPbkSettings;
    
    /**
     * Contact name display order 
     */
    enum TNameOrder
        {
        EFirstnameLastname,
        ELastnameFirstname
        };
    TNameOrder iNameOrder;
    
    /**
     * Counter of the matched contacts requested by PCS client
     */
    TInt iMaxCount;
    };

#endif // C_PCS_ALGORITHM_2_HELPER
