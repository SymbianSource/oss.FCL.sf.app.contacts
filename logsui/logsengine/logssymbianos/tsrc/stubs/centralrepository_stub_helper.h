/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __CENTRALREPOSITORYHELPER_H__
#define __CENTRALREPOSITORYHELPER_H__

class CentralRepositoryStubHelper{

public: // Stub data
    
    static void reset();
    static void setFailCode(TInt err);
    static void setCurrentVal(TInt val);  
    static TInt currentVal();  
        
};

#endif // __CENTRALREPOSITORYHELPER_H__
