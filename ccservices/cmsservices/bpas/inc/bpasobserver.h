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
* Description:  Interface for handling PresenceInfos
*
*/



#ifndef M_BPASOBSERVER_H
#define M_BPASOBSERVER_H


class CBPASInfo;


/**
 *  ?one_line_short_description
 *
 *  ?more_complete_description
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 v5.0
 */
class MBPASObserver 
    {

public:


    /**
     * BPAS calls this method as a response to
     * asynchronous MBPAS::GiveInfoL
     *
     * @since S60 ?S60_version
     * @param aInfos                [in] result array
     *                              Ownership is not transferred
     */
    virtual void HandleInfosL( TInt aErrorCode, RPointerArray<CBPASInfo>& aInfos ) = 0;
    
    /**
     * BPAS calls this method when new presence info is available for
     * a previously subscribed  contact. aInfo must be pushed to cleanup
     * before calling any leaving methods.
     *
     * @param aInfo resulted info, ownership is not transfered
     */
    virtual void HandleSubscribedInfoL(CBPASInfo* aInfo) = 0;
    
    };


#endif // M_BPASOBSERVER_H
