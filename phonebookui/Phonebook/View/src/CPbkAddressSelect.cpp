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
*       Provides methods for Generic Phonebook address selection dialog.
*
*/


// INCLUDE FILES

#include    "CPbkAddressSelect.h"
#include    <avkon.hrh> // AVKON softkey codes
#include    <aknnotedialog.h>
#include    <StringLoader.h>

#include    "CPbkSelectFieldDlg.h"
#include    "MPbkControlKeyObserver.h"

#include    <CPbkFieldInfo.h>
#include    <CPbkContactItem.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ExecuteLD
    };

static void Panic(TPanicCode aReason)   
    {
    _LIT(KPanicText, "CPbkAddressSelect");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// MODULE DATA STRUCTURES

/**
 * Special field selection dialog class for CPbkAddressSelect.
 * The main purpose of this class is to get #include of MPbkControlKeyObserver
 * away from public header CPbkAddressSelect.h.
 */
NONSHARABLE_CLASS(CPbkAddressSelect::CSelectFieldDlg) : 
        public CPbkSelectFieldDlg, 
        private MPbkControlKeyObserver
    {
    public:  // Interface
		/**
		 * Constructor.
		 * @param aParent parent
		 */
        CSelectFieldDlg(CPbkAddressSelect& aParent) :
            iParent(aParent)
            {
            SetObserver(this);
            }

    private:  // from MPbkControlKeyObserver
        TKeyResponse PbkControlKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType);

    private:  // Data
		/// Ref: parent
        CPbkAddressSelect& iParent;
    };


// ================= MEMBER FUNCTIONS =======================

TKeyResponse CPbkAddressSelect::CSelectFieldDlg::PbkControlKeyEventL
        (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    // Forward call to virtual function in CPbkAddressSelect interface
    return iParent.PbkControlKeyEventL(aKeyEvent,aType);
    }

EXPORT_C CPbkAddressSelect::TBaseParams::TBaseParams
        (const CPbkContactItem& aContact, 
        const TPbkContactItemField* aDefaultField) :
    iContact(aContact), 
    iDefaultField(aDefaultField),
    iFocusedField(NULL),
    iUseDefaultDirectly(EFalse),
    iSelectedField(NULL)
    {
    }

EXPORT_C const CPbkContactItem& CPbkAddressSelect::TBaseParams::ContactItem()
    {
    return iContact;
    }

EXPORT_C void CPbkAddressSelect::TBaseParams::SetFocusedField
        (const TPbkContactItemField* aFocusedField)
    {
    iFocusedField = aFocusedField;
    }

EXPORT_C void CPbkAddressSelect::TBaseParams::SetUseDefaultDirectly
        (TBool aUseDefaultDirectly)
    {
    iUseDefaultDirectly = aUseDefaultDirectly;
    }

EXPORT_C const TPbkContactItemField* CPbkAddressSelect::TBaseParams::SelectedField() const
    {
    return iSelectedField;
    }

EXPORT_C const TPbkContactItemField* CPbkAddressSelect::TBaseParams::FocusedField() const
    {
    return iFocusedField;
    }


EXPORT_C CPbkAddressSelect::CPbkAddressSelect()
    {
    __ASSERT_DEBUG(!iFieldDlg && !iDestroyedPtr, Panic(EPanicPostCond_Constructor));
    }

EXPORT_C CPbkAddressSelect::~CPbkAddressSelect()
	{
    // Tell ExecuteLD this object is already destroyed
    if (iDestroyedPtr) *iDestroyedPtr = ETrue;
    delete iFieldDlg;
    delete iFields;
	}

EXPORT_C TInt CPbkAddressSelect::ExecuteLD(TBaseParams& aParams)
    {
    __ASSERT_DEBUG(!iFieldDlg && !iFields, Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    // Ensure that thisDestroyed will be ETrue if this object is destroyed.
    // See in destructor how this is done.
    iDestroyedPtr = &thisDestroyed;

    // Set ContactItem member
    iContactItem = &aParams.iContact;

    // If currently focused field is a non-empty applicable field
    if (aParams.iFocusedField && AddressField(*aParams.iFocusedField))
        {
        // Non-empty applicable field was focused -> return field text
        aParams.iSelectedField = aParams.iFocusedField;
        }
    else 
        {
        // Focus is in on some other field, first check default field
        if (aParams.iDefaultField && 
            aParams.iUseDefaultDirectly && 
            AddressField(*aParams.iDefaultField))
            {
            // Non-empty default number field was found and direct use of
            // default was on -> return default field text
            aParams.iSelectedField = aParams.iDefaultField;
            }
        else
            {
            // No direct call to focused or default number, build an array
            // of applicable fields
            iFields = new(ELeave) CPbkFieldArray(8 /*granularity*/);
            TInt defaultNumberIndex = -1;
            CPbkFieldArray& fieldSet = aParams.iContact.CardFields();
            const TInt fieldCount = fieldSet.Count();
            for (TInt i=0; i < fieldCount; ++i)
                {
                const TPbkContactItemField& field = fieldSet[i];
                if (AddressField(field))
                    {
                    // Append all accepted address fields
                    iFields->AppendL(field);
                    if (aParams.iDefaultField && field.IsSame(*aParams.iDefaultField))
                        {
                        // Last appended field was the default, store its
                        // index so it can be focused later
                        defaultNumberIndex = iFields->Count()-1;
                        }
                    }
                }

            // Different actions for different number of applicable fields
            // found
            if (iFields->Count() == 0)
                {
                // No applicable addresses found
                NoAddressesL();
                }
            else if (iFields->Count() == 1)
                {
                // Exactly one applicable address found, just return it
                aParams.iSelectedField = aParams.iContact.FindSameField((*iFields)[0]);
                }
            else if (iFields->Count() > 1)
                {
                // Run the address selection dialog.
                const TDesC& title = QueryTitleL();
                iFieldDlg = new(ELeave) CSelectFieldDlg(*this);
                iFieldDlg->ResetWhenDestroyed(&iFieldDlg);
                TPbkContactItemField* field = NULL;
                
                field = iFieldDlg->ExecuteLD(
                    *iFields, 
                    QuerySoftkeysResource(), 
                    title, 
                    defaultNumberIndex);

                if (field)
                    {
                    // Field was selected, return it
                    aParams.iSelectedField = aParams.iContact.FindSameField(*field);
                    }
                }
            }
        }

    if (thisDestroyed)
        {
        // This object has already been destroyed
        CleanupStack::Pop(this);
        }    
    else
        {
        CleanupStack::PopAndDestroy(this);
        }

    // Return result
    return (aParams.iSelectedField != NULL);
    }
    
EXPORT_C const CPbkContactItem& CPbkAddressSelect::ContactItem() const
    {
    return *iContactItem;
    }

EXPORT_C void CPbkAddressSelect::AttemptExitL(TBool aAccept)
    {
    if (iFieldDlg)
        {
        iFieldDlg->AttemptExitL(aAccept);
        }
    }

EXPORT_C TKeyResponse CPbkAddressSelect::PbkControlKeyEventL
        (const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// Spare virtual function
EXPORT_C void CPbkAddressSelect::Reserved_1()
    {
    }

// Spare virtual function
EXPORT_C void CPbkAddressSelect::Reserved_2()
    {
    }


//  End of File  
