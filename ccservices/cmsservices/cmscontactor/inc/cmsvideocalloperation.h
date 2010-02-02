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
* Description:  Video call operation.
 *
*/


#ifndef C_CMSVIDEOCALLOPERATION_H
#define C_CMSVIDEOCALLOPERATION_H

#include <e32base.h>

#include "cmsoperationhandlerbase.h"
#include "cmscontactfield.h"


/**
 *  CCMSCallOperation
 *  Call operation. 
 *
 *  @code
 *   
 *  @endcode
 *
 *  @since S60 v5.0
 */
class CCMSVideoCallOperation : public CCMSOperationHandlerBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CCMSVideoCallOperation* NewL(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag );

public:

    // from base class CCMSOperationHandlerBase

    /**
     * Operation for starting executing the operation.
     *
     * @since S60 v5.0
     */
    void ExecuteLD();

private:

    // from base class CCMSOperationHandlerBase

    /**
     * Initiaties operation.
     *
     * @since S60 v5.0
     * @param aContactFieldItem: Selected field item to complete operation.
     */
    void StartOperationL( const CCmsContactFieldItem& aContactFieldItem);

private:

    void ConstructL();
    CCMSVideoCallOperation(
        const RArray<CCmsContactFieldItem::TCmsContactField>& aContactFieldTypesFilter,
        const RPointerArray<CCmsContactField>& aItemArray,
        const TCmsContactor::TCMSCommLaunchFlag aCommLaunchFlag );

    };

#endif // C_CMSVIDEOCALLOPERATION_H

// End of File
