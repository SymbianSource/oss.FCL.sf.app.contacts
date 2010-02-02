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
*     Command handler wrapper for Phonebook application.
*
*/


#ifndef __PbkCommandHandler_H__
#define __PbkCommandHandler_H__

// CLASS DECLARATION

/**
 * Abstract Phonebook command handler interface. Instances of this class
 * encapsulate an operation that can be executed by calling ExecuteL().
 */
class MPbkCommandHandler
    {
    public:
        /**
         * Excutes the encapsulated command.
         */
        virtual void ExecuteL() const = 0;
    };


/**
 * Phonebook command handler class.
 * Wraps a function call. Used for checking FFS status before
 * executing a user command.
 * 0(TPbkCommandHandler) and 1(TPbkCommandHandler1) parameter versions are provided.
 *
 * @param Cls   member function of this class is encapsulated by an object 
 *              of this class.
 * @see TPbkCommandHandler1
 * @see MPbkCommandHandler
 */
template<class Cls>
class TPbkCommandHandler : public MPbkCommandHandler
    {
    public:  // Interface
        /// typedef for a member function of class Cls
        typedef void (Cls::*TMemFunPtr)();

        /**
         * Constructor.
         * @param   aObject object to encapsulate a member function call for.
         * @param   aMemFun member function to encapsulate.
         */
        inline TPbkCommandHandler(Cls* aObject, TMemFunPtr aMemFun) 
            : iObject(aObject), iMemFun(aMemFun) 
            {
            }

    public:  // from MPbkCommandHandler
        void ExecuteL() const 
            { 
            (iObject->*iMemFun)(); 
            }

    private:  // Data
        Cls* iObject;
        TMemFunPtr iMemFun;
    };

/**
 * Wraps a member function call.
 * Use: PbkCommandHandler(this, &CClassName::CmdFunction)
 * 
 * @param   aObject object to encapsulate a member function call for.
 * @param   aMemFun member function to encapsulate.
 * @return  IS-A MPbkCommandHandler class where ExecuteL can be called.
 *          MPbkCommandHandler can easily be passed as a function parameter, 
 *          not requiring any template parameters.
 */
template<class Cls> inline
TPbkCommandHandler<Cls> PbkCommandHandler(Cls* aObject, void (Cls::*aMemFun)())
    {
    return TPbkCommandHandler<Cls>(aObject, aMemFun);
    }


/**
 * Same as TPbkCommandHandler but enables the member function to have one
 * parameter.
 *
 * @param Cls   member function of this class is encapsulated by an object 
 *              of this class.
 * @param Param Type of the member function parameter.
 * @see TPbkCommandHandler
 * @see MPbkCommandHandler
 */
template<class Cls, class Param>
class TPbkCommandHandler1 : public MPbkCommandHandler
    {
    public: // Interface
        /// typedef for a member function of class Cls
        typedef void (Cls::*TMemFunPtr)(Param);

        /**
         * Constructor.
         *
         * @param   aObject object to encapsulate a member function call for.
         * @param   aMemFun member function to encapsulate.
         * @param   aParam  parameter to pass to aMemFun when ExecuteL() is 
         *                  called.
         */
        inline TPbkCommandHandler1
            (Cls* aObject, TMemFunPtr aMemFun, Param aParam) 
            : iObject(aObject), iMemFun(aMemFun), iParam(aParam)
            { 
            }

    public:  // from MPbkCommandHandler
        void ExecuteL() const 
            { 
            (iObject->*iMemFun)(iParam); 
            }

    private:  // Data
        Cls* iObject;
        TMemFunPtr iMemFun;
        Param iParam;
    };

/**
 * Wraps a member function call.
 * Usage: PbkCommandHandler(this, &CClassName::CmdFunction, param)
 * 
 * @param   aObject object to encapsulate a member function call for.
 * @param   aMemFun member function to encapsulate.
 * @param   aParam  parameter to pass to aMemFun when ExecuteL() is 
 *                  called.
 * @return  IS-A MPbkCommandHandler class where ExecuteL can be called.
 *          MPbkCommandHandler can easily be passed as a function parameter, 
 *          not requiring any template parameters.
 */
template<class Cls, class Param> inline
TPbkCommandHandler1<Cls, Param> PbkCommandHandler
        (Cls* aObject, void (Cls::*aMemFun)(Param), Param aParam)
    {
    return TPbkCommandHandler1<Cls,Param>(aObject, aMemFun, aParam);
    }


#endif // __PbkCommandHandler_H__

// End of File
