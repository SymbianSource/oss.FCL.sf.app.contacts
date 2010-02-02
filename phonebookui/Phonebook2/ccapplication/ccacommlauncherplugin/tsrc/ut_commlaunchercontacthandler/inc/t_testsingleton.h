/*
* Copyright (c) 2006, 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface for identity object.
*
*/

#ifndef T_CTESTSINGLETON_H
#define T_CTESTSINGLETON_H

#include <e32base.h>



class T_CTestSingleton : public CBase

      {

  public: // constructor and destructor

      static T_CTestSingleton* InstanceL();

      void Release();
    
      void SetValue( TInt aValue );
      
      TInt GetValue( );

  private: // constructors

      T_CTestSingleton(); 

      void ConstructL();

      virtual ~T_CTestSingleton();

  private: // data

    TInt iValue;
    TInt iRefCount;

    };


#endif // T_CTESTSINGLETON_H


