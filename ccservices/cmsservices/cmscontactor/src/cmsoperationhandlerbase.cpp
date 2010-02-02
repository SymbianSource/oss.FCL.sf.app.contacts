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
* Description:  Base class for all CMS contactor's operations.
 *
*/


// INCLUDE FILES
#include "cmscontactorheaders.h"

/// Unnamed namespace for local definitions
namespace
    {

    _LIT( KCMSContactorResourceFileName, "\\resource\\cms\\cmscontactor.rsc");

#ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_LaunchDialogL = 1,
        EPanicPreCond_PrepareDataL,
        EPanicPreCond_GetNameStringLC,
        EPanicPreCond_PrepareHeaderL,
        EPanicPreCond_PrepareHeaderL2
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CCMSOperationHandlerBase");
        User::Panic (KPanicText, aReason);
        }
#endif // _DEBUG


    } /// namespace


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCMSOperationHandlerBase::CCMSOperationHandlerBase
// ----------------------------------------------------------
//
CCMSOperationHandlerBase::CCMSOperationHandlerBase(
    const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
    const RPointerArray<CCmsContactField>& aItemArray,
    const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag ) :
        iItemArray( aItemArray ), iContactFieldTypesFilter( aContactFieldTypesFilter ), 
        iCoeEnv( *CCoeEnv::Static()), iCommLaunchFlag( aCommLaunchFlag )
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::CCMSOperationHandlerBase()"));

    return;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::~CCMSOperationHandlerBase
// ----------------------------------------------------------
//
CCMSOperationHandlerBase::~CCMSOperationHandlerBase()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::~CCMSOperationHandlerBase()"));

    delete iHeading;
    iHeading = NULL;

    delete iFullName;
    iFullName = NULL;

    iQueryArray.Close ();

    iCoeEnv.DeleteResourceFile ( iResourceOffset);

    CMS_DP (
            KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::~CCMSOperationHandlerBase(): Done."));
    return;
    }

// --------------------------------------------------------------------------
// CCMSOperationHandlerBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CCMSOperationHandlerBase::BaseConstructL()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::BaseConstructL()"));

    // preparing resources for use        
    TFileName fileName;
    fileName.Append (KCMSContactorResourceFileName);

    BaflUtils::NearestLanguageFile ( iCoeEnv.FsSession (), fileName);
    if ( ConeUtils::FileExists ( fileName))
        {
        iResourceOffset = iCoeEnv.AddResourceFileL (fileName);
        }
    else // not found from user drive, leave badly. 
        {
        CMS_DP(KCMSContactorLoggerFile, CMS_L( "CCMSSelectionDialog::BaseConstructL(): Resource file not exists."));        
        User::Leave (KErrGeneral);
        }

    //These are allocated later on when going through given data. 
    iFullName = HBufC::NewL (0);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::BaseConstructL(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::PrepareDataL
// ----------------------------------------------------------
//
void CCMSOperationHandlerBase::PrepareDataL( TInt aCbaResourceId,
        TInt aHeaderResourceId, CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareDataL()"));

    __ASSERT_DEBUG (0 != aCbaResourceId && 0 != aHeaderResourceId,
                        Panic (EPanicPreCond_PrepareDataL));

    //Prepare data for dialog.     
    PrepareDialogL ();

    //Listbox heading
    PrepareHeaderL ( aHeaderResourceId);

    //Buttons
    iCbaResourceId = aCbaResourceId;

    //Default type
    iDefaultType = aDefaultType; 

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareDataL(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::LaunchDialogL
// ----------------------------------------------------------
//
const CCmsContactFieldItem& CCMSOperationHandlerBase::LaunchDialogL()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::LaunchDialogL()"));

    const CCmsContactFieldItem* result= NULL;

    //USE CASE: ONLY ONE NUMBER
    if( 1 == iContactFieldItemsFound )
        {
        result = &iQueryArray[0]->ItemL(0);
        }
    //USE CASE: DEFAULT CAN BE USED
    else
    if ( iCommLaunchFlag & TCmsContactor::ECMSContactorUseDefaultAddress )
        {
        result = GetDefaultNumberItemL();
        }        

    //USE CASE: NO DEFAULT AND NOT ONLY NUMBER
    if( NULL == result )
        {
        CCMSSelectionDialog* dialog = CCMSSelectionDialog::NewL ( iQueryArray);
        result = dialog->ExecuteLD (iCbaResourceId, iHeading->Des(), 0);
        }

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::LaunchDialogL(): Done."));
    return *result;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::GetNameStringLC
// ----------------------------------------------------------
//
TDesC* CCMSOperationHandlerBase::GetNameStringLC()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::GetNameStringLC()"));

    __ASSERT_DEBUG (NULL != iFullName,
                        Panic (EPanicPreCond_GetNameStringLC));

    HBufC* returnstring = HBufC::NewLC (iFullName->Size());
    returnstring->Des().Append (*iFullName);

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::GetNameStringLC(): Done."));
    return returnstring;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::PrepareDialogL
// ----------------------------------------------------------
//
void CCMSOperationHandlerBase::PrepareDialogL()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareDialogL()"));

    iContactFieldItemsFound = 0;

    for (TInt i = 0; i < iItemArray.Count (); i++)
        {
        CCmsContactField* contactfield = iItemArray[i];

        if ( IsNeededType (contactfield->Type ()))
            {
            iQueryArray.Append (contactfield);
            iContactFieldItemsFound += contactfield->ItemCount ();
            }
        }

    __ASSERT_ALWAYS ( 0 != iContactFieldItemsFound, User::Leave (KErrArgument));

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareDialogL(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::IsNeededType
// ----------------------------------------------------------
//
TBool CCMSOperationHandlerBase::IsNeededType(
        CCmsContactFieldItem::TCmsContactField aType)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::IsNeededType()"));
    TBool found = EFalse;

    for (TInt i = 0; i < iContactFieldTypesFilter.Count (); i++)
        {
        if ( aType==iContactFieldTypesFilter[i])
            {
            found = ETrue;          
            break;
            }
        }

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::IsNeededType(): Done."));
    return found;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::PrepareHeaderL
// ----------------------------------------------------------
//
void CCMSOperationHandlerBase::PrepareHeaderL( TInt aHeaderResourceId)
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareHeaderL()"));

    __ASSERT_DEBUG (0 != aHeaderResourceId,
                        Panic (EPanicPreCond_PrepareHeaderL));

    HBufC* resourceheading = StringLoader::LoadLC ( aHeaderResourceId );

    //Get name data from the given contact data. 
    SetNameStringL ();

    //Everything should be ready to create actual header text.
    __ASSERT_DEBUG (NULL != &iFullName,
                        Panic (EPanicPreCond_PrepareHeaderL2));

    iHeading = HBufC::NewL (resourceheading->Size());
    iHeading->Des().Append (resourceheading->Des ());

    CleanupStack::PopAndDestroy (resourceheading);
    
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::PrepareHeaderL(): Done."));
    return;
    }

// ----------------------------------------------------------
// CCMSOperationHandlerBase::SetNameStringL
// ----------------------------------------------------------
//
void CCMSOperationHandlerBase::SetNameStringL()
    {
    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::GetNameString()"));
    TBool namefound = EFalse;

    for (TInt i = 0; !namefound && i < iItemArray.Count (); i++)
        {
        CCmsContactField* contactfield = iItemArray[i];

        if ( CCmsContactFieldItem::ECmsFullName == contactfield->Type () )
            {
            namefound = ETrue;
            
            //Take the first item.
            const CCmsContactFieldItem* item = &contactfield->ItemL(0);            
            if ( iFullName)
                {
                delete iFullName;
                iFullName = NULL;
                }
            iFullName = item->Data().AllocL ();            
            }
        }

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::GetNameString(): Done."));
    return;
    }


// ----------------------------------------------------------
// CCMSOperationHandlerBase::GetDefaultNumberItemL
// ----------------------------------------------------------
//
const CCmsContactFieldItem* CCMSOperationHandlerBase::GetDefaultNumberItemL()
    {
    const CCmsContactFieldItem* ret = NULL; 

    CMS_DP (KCMSContactorLoggerFile,
            CMS_L ( "CCMSOperationHandlerBase::GetDefaultNumberItemL()"));

    TInt count = iQueryArray.Count(); 
    for(TInt a = 0; a < count; a++)
        {   
        CCmsContactField* field = iQueryArray[a];
        
        if(field->HasDefaultAttribute(iDefaultType))
            {
            ret = &field->ItemL(iDefaultType);
            CMS_DP (KCMSContactorLoggerFile,
                    CMS_L ( "CCMSOperationHandlerBase::GetDefaultNumberItemL(): Default found."));
            break;                
            }                    
        }    
    return ret; 
    }

// End of file
