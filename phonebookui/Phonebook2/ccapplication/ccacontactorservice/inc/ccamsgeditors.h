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
* Description:  Launching Editors. 
 *
*/


#ifndef __CCAMSGEDITORS_H__
#define __CCAMSGEDITORS_H__

#include <e32base.h>                             

class CSendingServiceInfo;

/**
 * Helper class for launching message editors
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCCAMsgEditors) : public CBase
	{
public:

	/**
	 * Launch a msg editor.
	 *
	 * @param TUid Service uid (MTM) 
	 * @param TDesC& Address where to send the msg
	 * @param TDesC& Alias name for msg editor
	 *
	 * @return void 
	 */
	static void ExecuteL( const TUid aServiceUid, const TDesC& aAddress,
			const TDesC& aName);
private:

    static void PrepareServicesList( RPointerArray<CSendingServiceInfo>& aServiceList );
    
	};

#endif // __CCAMSGEDITORS_H__
// End of File
