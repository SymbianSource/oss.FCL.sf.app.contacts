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
*     Contact model ECOM sort plugin.
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif
#include "CCntSortPlugin.h"

#include <SortUtil.h>
#include <cntdef.h>

#include "CSortKeyArray.h"

// Phonebook custom Field value.
// Top Contact field type.
const TInt KUidContactFieldTopContactValue  = 0x200100E3;

const TInt KCompareGreater = 2;	// >0, left item is greater
const TInt KCompareLess = -2;	// <0, left item is smaller

// Greater than any practical top contact number string 
// which is in form like '0000000001'. 
_LIT(KBigNumStr, "9999999999"); 

namespace {

#ifdef _DEBUG
enum TPanicType 
    {
    EPanicPreCond_CompareViewContactsL = 1,
    EPanicSortUtilFactoryReturnedNULL,
    EPanicInvalidViewParameters,
    EPanicDefaultCompareFunctionNULL,
    EPanicDefaultIsSortableFunctionNULL
    };

void Panic(TPanicType aPanicType)
    {
    _LIT(KPanicTxt, "CCntSortPlugin");
    User::Panic(KPanicTxt, aPanicType);
    }
#endif
} // namespace

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CCntSortPlugin::NewL
// ---------------------------------------------------------------------------
//
CCntSortPlugin* CCntSortPlugin::NewL(TAny* aParams)
	{
	CCntSortPlugin* self = new(ELeave) CCntSortPlugin;
    CleanupStack::PushL( self );
    self->ConstructL(static_cast<TSortPluginParams*>(aParams));
    CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------------------------
// CCntSortPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CCntSortPlugin::ConstructL(TSortPluginParams* aParams)
    {
	// Validate parameters
	if (!aParams || 
        (aParams->iParametersRevision != KCntSortPluginViewParamsRev1Uid))
		User::Leave(KErrArgument);

    TSortPluginViewParamsRev1* viewParams = 
        static_cast<TSortPluginViewParamsRev1*>(aParams->iViewSortParams);

    __ASSERT_DEBUG(viewParams, 
                   Panic(EPanicInvalidViewParameters));
    __ASSERT_DEBUG(viewParams->iCompareViewContactsL, 
                   Panic(EPanicDefaultCompareFunctionNULL));
    __ASSERT_DEBUG(viewParams->iIsSortable, 
                   Panic(EPanicDefaultIsSortableFunctionNULL));

    iIsSortable = viewParams->iIsSortable;
    
    iSortUtil = CSortUtil::NewL();
    // Sort Util factory has to return valid pointer
    // Factory should default to european sorting if nothing else is
    // applicable
    __ASSERT_DEBUG(iSortUtil, Panic(EPanicSortUtilFactoryReturnedNULL));

    iLeftSortKeyArray = CSortKeyArray::NewL();
    iRightSortKeyArray = CSortKeyArray::NewL();
    }

// ---------------------------------------------------------------------------
// CCntSortPlugin::CCntSortPlugin
// ---------------------------------------------------------------------------
//
CCntSortPlugin::CCntSortPlugin()
	{
	}

// ---------------------------------------------------------------------------
// CCntSortPlugin::~CCntSortPlugin
// ---------------------------------------------------------------------------
//
CCntSortPlugin::~CCntSortPlugin()
	{
    delete iSortUtil;
    iSortOrder.Close();
    delete iLeftSortKeyArray;
    delete iRightSortKeyArray;
	}

void CCntSortPlugin::SetSortOrderL
        (const RContactViewSortOrder& aViewSortOrder)
    {
    iSortOrder.Close();
    iSortOrder.CopyL(aViewSortOrder);
    
    iTopContactId = KErrNotFound;
    for ( TInt i=0; i<iSortOrder.Count(); i++ )
        {
        if ( TUid::Uid(KUidContactFieldTopContactValue) == iSortOrder[i] )
            {
            iTopContactId = i;
            break;
            }
        }
    }

TInt CCntSortPlugin::SortStart(TSortStartTypes aSortStartType, TInt aCount)
    {
    TRAPD(ret, DoSortStartL(aSortStartType, aCount));
    return ret;
    }

void CCntSortPlugin::DoSortStartL
        (TSortStartTypes /* aSortStartType */, TInt /* aCount */)
    {
    iLeftSortKeyArray->Reset();
    iRightSortKeyArray->Reset();

    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        TFieldType fieldType = iSortOrder[i];
        if (fieldType == KUidContactFieldGivenNamePronunciation ||
            fieldType == KUidContactFieldFamilyNamePronunciation ||
            fieldType == KUidContactFieldCompanyNamePronunciation)
            {
            iLeftSortKeyArray->AppendL
                (TSortKey(KNullDesC, ESortKeyPronounciation));
            iRightSortKeyArray->AppendL
                (TSortKey(KNullDesC, ESortKeyPronounciation));
            }
        else
            {
            iLeftSortKeyArray->AppendL(TSortKey(KNullDesC, ESortKeyBasic));
            iRightSortKeyArray->AppendL(TSortKey(KNullDesC, ESortKeyBasic));
            }
        }
    }

void CCntSortPlugin::SortCompleted()
    {
    iLeftSortKeyArray->Reset();
    iRightSortKeyArray->Reset();
    }

TInt CCntSortPlugin::SortCompareViewContactsL
        (const CViewContact& aLhs, const CViewContact& aRhs)
    {
    __ASSERT_DEBUG(aLhs.FieldCount() == iSortOrder.Count(),
                   Panic(EPanicPreCond_CompareViewContactsL));
    __ASSERT_DEBUG(aRhs.FieldCount() == iSortOrder.Count(),
                   Panic(EPanicPreCond_CompareViewContactsL));
    __ASSERT_DEBUG(iLeftSortKeyArray && 
                   iLeftSortKeyArray->SortKeyCount() == iSortOrder.Count(),
                   Panic(EPanicPreCond_CompareViewContactsL));
    __ASSERT_DEBUG(iRightSortKeyArray && 
                   iRightSortKeyArray->SortKeyCount() == iSortOrder.Count(),
                   Panic(EPanicPreCond_CompareViewContactsL));

    // Change the text in the sortkeys to correspond fields in the contacts
    const TInt count = iSortOrder.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iLeftSortKeyArray->SetText(aLhs.Field(i), i);
        iRightSortKeyArray->SetText(aRhs.Field(i), i);
        
        if ( i == iTopContactId )
            {
            TInt leftTopContactLength = aLhs.Field(i).Length();
            TInt rightTopContactLength = aRhs.Field(i).Length();
            if ( leftTopContactLength == 0 && rightTopContactLength > 0 )
                {
                if ( i == 0 )   // If it's the first field, return result directly.
                    {
                    return KCompareGreater;
                    }
                else
                    {
                    iLeftSortKeyArray->SetText(KBigNumStr, i);
                    }
                }
            else if ( leftTopContactLength > 0 && rightTopContactLength == 0 )
                {
                if ( i == 0 )
                    {
                    return KCompareLess;
                    }
                else
                    {
                    iRightSortKeyArray->SetText(KBigNumStr, i);
                    }
                }
            }
        }

    return iSortUtil->Interface()->CompareItems
        (*iLeftSortKeyArray, *iRightSortKeyArray);
    }

TInt CCntSortPlugin::ApiCompareViewContactsL
    (const CViewContact& aLhs, const CViewContact& aRhs)
    {
    return SortCompareViewContactsL(aLhs, aRhs);
    }

TBool CCntSortPlugin::ViewContactIsSortable
        (const CViewContact& aViewContact)
    {
    return iIsSortable(aViewContact);
    }

// End of File
