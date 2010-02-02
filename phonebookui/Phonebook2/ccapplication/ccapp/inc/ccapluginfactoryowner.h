/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class handling ownership of cca ui view factory instantiated 
* from ecom. The factory needs to be kept alive as long as the last
* ui view instantiated from this factory is alive.
*
*/
#ifndef C_CCAPLUGINFACTORYOWNER_H
#define C_CCAPLUGINFACTORYOWNER_H

#include <e32base.h>
#include <mccaparentcleaner.h>

class MCcaPluginFactory;

class CcaPluginFactoryOwner : public CBase, public MCcaParentCleaner
    {
    public:

        /**
         * Two-phased constructor.
         */        
        static CcaPluginFactoryOwner* NewL(
            CImplementationInformation* aImplementationInformation);
        
        /**
         * Destructor.
         */
        ~CcaPluginFactoryOwner();

        /**
         * Increases refrence count.
         * @return Reference count value after addition.
         */        
        TInt AddedChild();     

        /**
         * Returns refence to to factory instance.
         * @return factory instance.
         */
        MCcaPluginFactory& Factory();        
        
    private:
        
        /**
         * Constuctor.
         */
        CcaPluginFactoryOwner();
        
        /**
         * Second phase constuction.
         */        
        void ConstructL(CImplementationInformation* aImplementationInformation);
        
        /**
         * From MCcaParentCleaner
         */        
        virtual TInt DeletedChild();

    private:
        
        TUid iDtor_ID_Key; //iInstance identifier for ecom.        
        TInt iChildCount;
        MCcaPluginFactory* iFactory; //owned        
    };

#endif // C_CCAPLUGINFACTORYOWNER_H
