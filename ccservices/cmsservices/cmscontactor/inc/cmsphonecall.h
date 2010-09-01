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
* Description:  Launching Call. 
 *
*/


#ifndef __CMSPHONECALL_H__
#define __CMSPHONECALL_H__

#include <e32base.h>                             
#include <aiwdialdataext.h>       

class CCmsPhoneCall : public CBase
	{
public:
    
    /**
     * Type of call
     */
    enum TCmsPhoneCallType { ECmsCallTypeVoice, ECmsCallTypeVoIP, ECmsCallTypeVideo };

	/**
	 * Establish a call by using a phone number
	 *
	 * @param TDesC8& Phone number
	 * @param aCallType Is this a cellular Voice, VoIP, or Video call
	 *
	 * @return void 
	 */
	static void ExecuteL( const TDesC& aPhoneNumber,
	        TCmsPhoneCallType aCallType = ECmsCallTypeVoice);

private:

	/**
	 * Establish a call by using a phone number
	 *
	 * @param TDesC8& Phone number
	 * @param TBool Is this a VoIP call
	 *
	 * @return void 
	 */
	static void DoAIWCallL( CAiwDialDataExt& aDialData);
	};

#endif // __CMSPHONECALL_H__
