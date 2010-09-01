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
*      Phonebook cache entry class.
*
*/


#ifndef __TPbkContactEntry_H__
#define __TPbkContactEntry_H__

// INCLUDES
#include <cntdef.h>         // TContactItemId
#include <PbkIconId.hrh>    // TPbkIconId

/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode_ContactEntryInline
    {
    EPanicPreCond_NameBuf = 1
    };


// LOCAL FUNCTIONS
void Panic(TPanicCode_ContactEntryInline aReason)
    {
    _LIT(KPanicText, "TPbkContactEntry inline");
    User::Panic(KPanicText, aReason);
    }

}  // namespace


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook cache entry.
 * @see CPbkEntryCache
 */
class TPbkContactEntry
    {
    public:  // Constructors and destructors
        /**
         * Default constructor.
         */
        TPbkContactEntry();

        /**
         * Destroys all resources held by this object. After calling Destroy()
         * this object is unusable.
         */
        void Destroy();

    public:  // Getters
        /**
         * Returns the contact id of this entry.
         */
        TContactItemId ContactId() const;

        /**
         * Returns the name.
		 * @precond iName
         */
        const TDesC& Name() const;

        /**
         * Returns the Phonebook icon id, EPbkNullIconId means no icon.
         */
        TPbkIconId IconId() const;

    public:  // Setters
        /**
         * Sets the contact item id.
         */
        void SetContactItemId(TContactItemId aContactId);

        /**
         * Reserves space for the name. Does not change existing name text.
         */
        void ReserveNameMaxLengthL(TInt aMaxLength);

        /**
         * Returns a modifiable descriptor to the name buffer.
         * Call ReserveNameMaxLengthL() first to allocate space for the
         * buffer.
         */
        TPtr NameBuf();

        /**
         * Resets the name of this entry.
         *
         * @postcond Name().Length()==0
         */
        void ResetName();

        /**
         * Sets the icon id.
         */
        void SetIconId(TPbkIconId aIconId);

        /**
         * Resets the contents of this entry.
         */
        void Reset();

    private:  // Implementation
        static const TDesC& ToTDesC(HBufC* aBuf);
        static void ResetHBufC(HBufC* aBuf);
        IMPORT_C static HBufC* ReserveL(HBufC* aBuf, TInt aMaxLength);
        static void Destroy(TAny* aSelf);

    private:  // data
        /// Own: ContactId
        TContactItemId iContactId;
        /// Own: Name
        HBufC* iName;
        /// Own: Icon id
        TPbkIconId iIconId;
    };


// INLINE FUNCTIONS

/**
 * Returns contents of aBuf as a const TDesC& or KNullDesC if !aBuf.
 */
inline const TDesC& TPbkContactEntry::ToTDesC(HBufC* aBuf)
    {
    if (aBuf)
        {
        return *aBuf;
        }
    else
        {
        return KNullDesC;
        }
    }

/**
 * Resets member aBuf to empty text.
 * @postcond ToTDesC(aBuf).Length()==0
 */
inline void TPbkContactEntry::ResetHBufC(HBufC* aBuf)
    {
    if (aBuf)
        {
        *aBuf = KNullDesC;
        }
    }

inline TPbkContactEntry::TPbkContactEntry() :
    iContactId(KNullContactId),
    iName(NULL),
    iIconId(EPbkNullIconId)
    {
    }

inline TContactItemId TPbkContactEntry::ContactId() const
    {
    return iContactId;
    }

inline const TDesC& TPbkContactEntry::Name() const
    {
    return ToTDesC(iName);
    }

inline TPbkIconId TPbkContactEntry::IconId() const
    {
    return iIconId;
    }

inline void TPbkContactEntry::SetContactItemId(TContactItemId aContactId)
    {
    iContactId = aContactId;
    }

inline void TPbkContactEntry::ReserveNameMaxLengthL(TInt aMaxLength)
    {
    iName = ReserveL(iName,aMaxLength);
    }

inline TPtr TPbkContactEntry::NameBuf()
    {
	__ASSERT_DEBUG(iName, Panic(EPanicPreCond_NameBuf));

	return iName->Des();
    }

inline void TPbkContactEntry::ResetName()
    {
    ResetHBufC(iName);
    }

inline void TPbkContactEntry::SetIconId(TPbkIconId aIconId)
    {
    iIconId = aIconId;
    }

inline void TPbkContactEntry::Reset()
    {
    iContactId = KNullContactId;
    ResetHBufC(iName);
    iIconId = EPbkNullIconId;
    }

inline void TPbkContactEntry::Destroy()
    {
    delete iName;
    }

/**
 * Returns ETrue if aLeft == aRight.
 */
inline TBool operator==
        (const TPbkContactEntry& aLhs, const TPbkContactEntry& aRhs)
    {
    return 
        aLhs.ContactId() == aRhs.ContactId() &&
        aLhs.Name() == aRhs.Name() &&
        aLhs.IconId() == aRhs.IconId();
    }

/**
 * Returns ETrue if aLeft != aRight.
 */
inline TBool operator!=
        (const TPbkContactEntry& aLeft, const TPbkContactEntry& aRight)
    {
    return !(aLeft==aRight);
    }


#endif // __TPbkContactEntry_H__

// End of File
