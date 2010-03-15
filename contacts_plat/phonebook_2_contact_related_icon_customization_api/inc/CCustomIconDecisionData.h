/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for icon choice decision data.
*/

#ifndef CCUSTOMICONDECISIONDATA_H
#define CCUSTOMICONDECISIONDATA_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;

/**
 * Container for icon choice decision data.
 *
 * When an application supporting Contact Related Icon Customization API allows
 * customization for a certain icon, it instantiates this class and passes it to
 * the icon customization plug-in for getting a reference to the custom icon.
 *
 * The plug-in uses the data members of this class for its decision making. For
 * example, it could use the iPhoneNumber to resolve a certain custom icon
 * which is related to that phone number.
 *
 * Note that member values and semantics are undefined by the API. The
 * application decides how the values are filled. Application should
 * at least fill one of the members so that the plug-in can make meaningful
 * icon choice decision.
 *
 * @see CContactIconCustomPluginBase
 */
class CCustomIconDecisionData : public CBase
    {
    public: // Construction and destruction

        /**
         * Constructor.
         */
        CCustomIconDecisionData();

        /**
         * Destructor.
         */
        ~CCustomIconDecisionData();

    public: // Interface

    /**
     * Returns the context.
     *
     * @return  UID of the context.
     * @see iContext
     */
    TUid Context() const;

    /**
     * Returns the time.
     *
     * @return  Time.
     * @see iTime
     */
    const TTime* Time() const;

    /**
     * Returns the phone number. KNullDesC if not set.
     *
     * @return  Phone number.
     */
    TPtrC PhoneNumber() const;

    /**
     * Returns the contact link.
     *
     * @return  Contact link.
     */
    const MVPbkContactLink* ContactLinkRef() const;

    /**
     * Sets context.
     *
     * @param aUid  Context UID.
     * @see iContext
     */
    void SetContext(
            TUid aUid );

    /**
     * Sets the time.
     *
     * @param aTime     The time to set. Ownership taken.
     * @see iTime
     */
    void SetTime(
            TTime* aTime );

    /**
     * Sets the phone number.
     *
     * @param aPhoneNumber      The phone number to set.
     */
    void SetPhoneNumberL(
            const TDesC& aPhoneNumber );

    /**
     * Sets contact link.
     *
     * @param aLink     The link to set. Ownership is not taken.
     */
    void SetContactLinkRef(
            const MVPbkContactLink* aLink );

    private: // Data

        /// Own: Context property for customization.
        /// It is up to caller how it sets this. At minimum caller should set this
        /// value to its application UID or similar. Or, if there are several
        /// logical application contexts, application may select different
        /// UID for each use. Caller is allowed to set this to KNullUid.
        TUid iContext;

        /// Own: Time property for customization, optional.
        /// For example in Logs application this could be time of the Logs event.
        /// The time value should be Universal time.
        TTime* iTime;

        /// Own: Phonenumber property for customization, optional.
        HBufC* iPhoneNumber;

        /// Ref: Contact link
        const MVPbkContactLink* iContactLinkRef;
    };

#include <CCustomIconDecisionData.inl>

#endif // CCUSTOMICONDECISIONDATA_H

// End of File
