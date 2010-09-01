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


#ifndef C_CMSOPERATIONHANDLERBASE_H
#define C_CMSOPERATIONHANDLERBASE_H

#include <e32base.h>

#include "cmscontactfield.h"

/**
 *  CCMSOperationHandlerBaseBase
 *  Base class for all CMS contactor's operations.
 *
 *  @code
 *
 *  @endcode
 *
 *  @since S60 v5.0
 */
class CCMSOperationHandlerBase : public CBase
    {

public:

    /**
     * Constructor.
     */
    CCMSOperationHandlerBase(
            const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
            const RPointerArray<CCmsContactField>& aItemArray,
            const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag );

    /**
     * Destructor.
     */
    virtual ~CCMSOperationHandlerBase();

    /**
     * Interface method.
     * Every operation must have a ExecuteLD to get started execute of operation.
     *
     * @since S60 v5.0
     */
    virtual void ExecuteLD() = 0;

protected:
    // Interface

    /**
     * Second phase constructor
     */
    void BaseConstructL();

    /**
     * Initiaties operation.
     *
     * @since S60 v5.0
     * @param aContactFieldItem: Selected field item to complete operation.
     */
    virtual void
            StartOperationL( const CCmsContactFieldItem& aContactFieldItem) = 0;

    /**
     * Method for preparing given data for operation.
     * Must run before calling LaunchDialogL.
     *
     * @since S60 v5.0
     * @param aCbaResourceId: Resource Id of softkey buttons.
     * @param aHeaderResourceId: Resource Id of header string.
     */
    virtual void PrepareDataL( TInt aCbaResourceId, TInt aHeaderResourceId, CCmsContactFieldItem::TCmsDefaultAttributeTypes aDefaultType);

    /**
     * Launches actual dialog if needed. (more than 1 items to select from)
     *
     * @since S60 v5.0
     * @return reference of selected field item.
     */
    const CCmsContactFieldItem& LaunchDialogL();

    /**
     * Returns the full name string of contact. (firstname + lastname)
     * Ownership transferred to caller.
     *
     * @since S60 v5.0
     * @return the name string.
     */
    TDesC* GetNameStringLC();

private:
    // Own implement

    /**
     * Helper function for preparing data.
     * Leaves with KErrArgument if there is no data at all.
     *
     * @since S60 v5.0
     */
    void PrepareDialogL();

    /**
     * Helper function for filtering correct contactfields.
     *
     * @since S60 v5.0
     * @param aType: type of required contact field type.
     */
    TBool IsNeededType( CCmsContactFieldItem::TCmsContactField aType);

    /**
     * Helper function for preparing header data.
     *
     * @since S60 v5.0
     * @param aHeaderResourceId: Resource Id of header string.
     */
    void PrepareHeaderL( TInt aHeaderResourceId);

    /**
     * Helper function for allocating name strings.
     *
     * @since S60 v5.0
     */
    void SetNameStringL();

    /**
     * Helper function for solving default number
     * Leaves with KErrNotFound
     *
     * @since S60 v5.0
     * @return CCmsContactFieldItem
     */
    const CCmsContactFieldItem* GetDefaultNumberItemL();

protected:
    // data

    /**
     * Contact fields given.
     * Not Own.
     */
    const RPointerArray<CCmsContactField>& iItemArray;

private:
    // data

    /**
     * For filtering correct contact field types.
     * Not Own.
     */
    const RArray<CCmsContactFieldItem::TCmsContactField>
            & iContactFieldTypesFilter;

    /**
     * Modified array for making listbox query.
     * Own.
     */
    RPointerArray<CCmsContactField> iQueryArray;

    /**
     * Environment.
     * Not Own.
     */
    CCoeEnv& iCoeEnv;

    /**
     * Resource id of used softkeys.
     * Own.
     */
    TInt iCbaResourceId;

    /**
     * Heading text of dialog.
     * Own.
     */
    HBufC* iHeading;

    /**
     * Fullname
     * Own.
     */
    TDesC* iFullName;

    /**
     * Offset of loaded resource file.
     * Own.
     */
    TInt iResourceOffset;

    /**
     * Found items.
     * Own.
     */
    TInt iContactFieldItemsFound;

    /**
     * Communication launching related information
     * Own.
     */
    TCmsContactor::TCMSCommLaunchFlag iCommLaunchFlag;

    /**
     * Default type
     * Own.
     */
    CCmsContactFieldItem::TCmsDefaultAttributeTypes iDefaultType;
    };

#endif // C_CMSOPERATIONHANDLERBASE_H

// End of File
