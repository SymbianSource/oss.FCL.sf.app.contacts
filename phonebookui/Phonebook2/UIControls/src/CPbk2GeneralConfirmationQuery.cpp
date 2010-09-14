/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 general confirmation query.
*
*/


#include "CPbk2GeneralConfirmationQuery.h"

// Phonebook 2
#include <MPbk2AppUi.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2ContactNameFormatter.h>
#include <pbk2uicontrols.rsg>

// Virtual Phonebook
#include <MVPbkBaseContact.h>

// System includes
#include <StringLoader.h>
#include <AknQueryDialog.h>

/// Unnamed namespace for local definitions
namespace {

const TUint32 KNameFormatterFlags = MPbk2ContactNameFormatter::EUseSeparator;
const TInt KDontDelete = 0;

#ifdef _DEBUG

enum TPanicCode
    {
    EExecuteLD_PreCond =1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2GeneralConfirmationQuery");
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

/**
 * Createst prompt text.
 *
 * @param aContact      Contact.
 * @param aResId        Prompt string resource id.
 * @param aFormatFlag    Formatting flag info; default is KNameFormatterFlags
 * @return  Prompt text.
 */
HBufC* CreatePromptLC( const MVPbkBaseContact& aContact, TInt aResId, TInt aFormatFlag = KNameFormatterFlags )
    {
    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();
    HBufC* name = appServices->NameFormatter().GetContactTitleL
        ( aContact.Fields(), aFormatFlag );
    CleanupStack::PushL( name );
    HBufC* prompt = StringLoader::LoadL( aResId, *name );
    CleanupStack::PopAndDestroy( name );
    CleanupStack::PopAndDestroy(); // appServices

    CleanupStack::PushL( prompt );

    return prompt;
    }

/**
 * Createst prompt text.
 *
 * @param aAmountOfContacts     Amount of contacts.
 * @param aResId                Prompt string resource id.
 * @return  Prompt text.
 */
HBufC* CreatePromptLC( TInt aAmountOfContacts, TInt aResId )
    {
    return StringLoader::LoadLC( aResId, aAmountOfContacts );
    }

/**
 * Executes the query.
 *
 * @param aQuery    Confirmation query.
 * @param aParam    Prompt parameter (templated type).
 * @param aResId    Prompt text resource id.
 * @return  Result.
 */
template<typename PromptParam>
TInt ExecuteQueryLD( CPbk2GeneralConfirmationQuery* aQuery,
        CAknQueryDialog*& aQueryDlg,
        const PromptParam& aParam, TInt aResId )
    {
    CleanupStack::PushL( aQuery );
    HBufC* prompt = CreatePromptLC( aParam, aResId );
    aQueryDlg = CAknQueryDialog::NewL();
    TInt result = aQueryDlg->ExecuteLD
        ( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt );
    aQueryDlg = NULL;
    CleanupStack::PopAndDestroy( 2, aQuery );
    return result;
    }

/**
 * Executes the query.
 *
 * @param aQuery    Confirmation query.
 * @param aParam    Prompt parameter (templated type).
 * @param aResId    Prompt text resource id.
 * @param aFormatFlag Formatting flag info
 * @return  Result.
 */
template<typename PromptParam>
TInt ExecuteQueryLD( CPbk2GeneralConfirmationQuery* aQuery,
        CAknQueryDialog*& aQueryDlg,
        const PromptParam& aParam, TInt aResId, TInt aFormatFlag )
    {
    CleanupStack::PushL( aQuery );
    HBufC* prompt = CreatePromptLC( aParam, aResId, aFormatFlag );
    aQueryDlg = CAknQueryDialog::NewL();
    TInt result = aQueryDlg->ExecuteLD
        ( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt );
    aQueryDlg = NULL;
    CleanupStack::PopAndDestroy( 2, aQuery );
    return result;
    }


}  /// namespace

// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::CPbk2GeneralConfirmationQuery
// --------------------------------------------------------------------------
//
inline CPbk2GeneralConfirmationQuery::CPbk2GeneralConfirmationQuery()
    {
    }

// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::~CPbk2GeneralConfirmationQuery
// --------------------------------------------------------------------------
//
CPbk2GeneralConfirmationQuery::~CPbk2GeneralConfirmationQuery()
    {
    delete iQueryDlg;
    }

// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2GeneralConfirmationQuery* CPbk2GeneralConfirmationQuery::NewL()
    {
    CPbk2GeneralConfirmationQuery* self =
        new ( ELeave ) CPbk2GeneralConfirmationQuery;
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2GeneralConfirmationQuery::ExecuteLD
        ( const MVPbkBaseContact& aContact, TInt aResId )
    {
    return ExecuteQueryLD( this, iQueryDlg, aContact, aResId );
    }


// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::ExecuteFormatLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2GeneralConfirmationQuery::ExecuteLD
        ( const MVPbkBaseContact& aContact, TInt aResId, TInt aFormatFlag )
    {
    return ExecuteQueryLD( this, iQueryDlg, aContact, aResId, aFormatFlag );
    }


// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2GeneralConfirmationQuery::ExecuteLD
        ( TInt aAmountOfContacts, TInt aResId )
    {
    __ASSERT_DEBUG( aAmountOfContacts > 1, Panic( EExecuteLD_PreCond ) );

    TInt ret = KDontDelete;

    if ( aAmountOfContacts > 1 )
        {
        ret = ExecuteQueryLD( this, iQueryDlg, aAmountOfContacts, aResId );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2GeneralConfirmationQuery::StopQuery
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2GeneralConfirmationQuery::StopQuery()
    {
    delete iQueryDlg;
    iQueryDlg = NULL;
    }

// End of File
