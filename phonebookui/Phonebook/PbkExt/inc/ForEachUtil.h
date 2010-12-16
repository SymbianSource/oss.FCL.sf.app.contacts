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
*
*
*/


/**
 * Template class for functor with one parameter.
 * @param _ReturnType Return type of the member function
 * @param _Class Class that has the member function to be called.
 * @param _Argument Type of member function's argument.
 */
template <class _ReturnType, class _Class, class _Argument>
class MemberFunction1  
    {
    public:
        explicit MemberFunction1
                (_ReturnType (_Class::* aMemberFunction)(_Argument)) : 
            iMemberFunction(aMemberFunction) 
            {
            }
            
        _ReturnType operator()
                (_Class* aClassInstance, _Argument aArgument) const 
            { 
            return (aClassInstance->*iMemberFunction)(aArgument); 
            }
    private:
        _ReturnType (_Class::*iMemberFunction)(_Argument);
    };

/** 
 * Function for creating functor with one parameter.
 */
template <class _ReturnType, class _Class, class _Argument>
inline MemberFunction1<_ReturnType, _Class, _Argument> 
MemberFunction(_ReturnType (_Class::* aMemberFunction)(_Argument))
    { 
    return MemberFunction1<_ReturnType, _Class, _Argument>(aMemberFunction); 
    }

/**
 * Template class for functor with two parameters.
 */
template<class _ReturnType, class _Class, class _Argument, class _Argument2>
class MemberFunction2
    {
    public:
        explicit MemberFunction2
            (_ReturnType (_Class::* aMemberFunction)(_Argument, _Argument2)) : 
            iMemberFunction(aMemberFunction)
            {
            }

        _ReturnType operator()
                (_Class* aClassInstance, _Argument aArgument1, 
                 _Argument2 aArgument2) const
            {
            return ((aClassInstance->*iMemberFunction)
                (aArgument1, aArgument2));
            }
    private:
        _ReturnType (_Class::* iMemberFunction)(_Argument, _Argument2);
    };

/**
 * Function for creating functor with two parameters.
 */
template <class _ReturnType, class _Class, class _Argument, class _Argument2>
inline MemberFunction2<_ReturnType, _Class, _Argument, _Argument2> 
MemberFunction(_ReturnType (_Class::* aMemberFunction)(_Argument, _Argument2))
    { 
    return MemberFunction2<_ReturnType, _Class, _Argument, _Argument2>
        (aMemberFunction); 
    }

/**
 * Template class for functor with one parameter and 
 * void return type.
 *
 * NOTE: The void versions have to be done this way because
 * VC 6.0 does not support partial specialization of templates.
 */
template<class _Class, class _Argument>
class VoidMemberFunction1 
    {
    public:
        explicit VoidMemberFunction1
                (void (_Class::* aMemberFunction)(_Argument)) :
            iMemberFunction(aMemberFunction) 
            {
            }

        void operator()(_Class* aClassInstance, _Argument aArgument) const 
            { 
            (aClassInstance->*iMemberFunction)(aArgument); 
            }
    private:
        void (_Class::* iMemberFunction)(_Argument);
    };

/**
 * Functon for creating functor with one parameter and void
 * return type.
 */
template<class _Class, class _Argument>
inline VoidMemberFunction1<_Class, _Argument> 
VoidMemberFunction(void (_Class::* aMemberFunction)(_Argument))
    { 
    return VoidMemberFunction1<_Class, _Argument>(aMemberFunction); 
    }

/**
 * Template class for functor with two parameters and 
 * void return type.
 */
template<class _Class, class _Argument, class _Argument2>
class VoidMemberFunction2
    {
    public:
        explicit VoidMemberFunction2
                (void (_Class::* aMemberFunction)(_Argument, _Argument2)) :
            iMemberFunction(aMemberFunction)
            {}
        void operator()(_Class* aClassInstance, _Argument aArgument, 
                        _Argument2 aArgument2) const
            {
            ((aClassInstance->*iMemberFunction)(aArgument,aArgument2));
            }
    private:
        void (_Class::* iMemberFunction)(_Argument, _Argument2);
    };

/**
 * Functon for creating functor with two parameters and void
 * return type.
 */
template<class _Class, class _Argument, class _Argument2>
inline VoidMemberFunction2<_Class, _Argument, _Argument2> 
VoidMemberFunction(void (_Class::* aMemberFunction)(_Argument, _Argument2))
    { 
    return VoidMemberFunction2<_Class, _Argument, _Argument2>(aMemberFunction); 
    }

/**
 * Executes given member function on all array elements.
 * Version with no arguments.
 */
template<class Array, class Functor>
void ForEachL(Array& aArray, Functor aFuncL)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i<count; ++i)
        {
        aFuncL(aArray[i]);
        }
    }

/**
 * Executes given member function on all array elements.
 * Version with one argument.
 */
template<class Array, class Functor, class Arg>
void ForEachL(Array& aArray, Functor aFuncL, Arg& aArgument)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i<count; ++i)
        {
        aFuncL(aArray[i], aArgument);
        }
    }

/**
 * Executes given member function on all array elements.
 * Version with two arguments.
 */
template<class Array, class Functor, class Arg, class Arg2>
void ForEachL(Array& aArray, Functor aFuncL, Arg& aArgument, Arg2& aArgument2)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i<count; ++i)
        {
        aFuncL(aArray[i], aArgument, aArgument2);
        }
    }

/**
 * Executes given member function on all array elements
 * until first returns ETrue. When first returns ETrue
 * loop is terminated and ETrue is returned. If
 * no function returns ETrue, EFalse is returned.
 *
 * If aInverted template parameter is set to ETrue,
 * logic is reversed, ie. if function returns EFalse, loop
 * is terminated and EFalse is returned. If 
 * no funtion returns EFalse, ETrue is returned.
 * 
 */
template<class Array, class Functor, class Arg>
TBool TryEachL(Array& aArray, Functor aFuncL, 
             Arg& aArgument, TBool aInverted = EFalse)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i<count; ++i)
        {
        TBool exitCode = aFuncL(aArray[i], aArgument);
        if( !aInverted )
            {
            if(exitCode)
                {
                return exitCode;
                }
            }
        else
            {
            if(!exitCode)
                {
                return exitCode;
                }
            }
        }
    return aInverted;
    }

/**
 * Two-argument version of the previous.
 */
template<class Array, class Functor, class Arg, class Arg2>
TBool TryEachL(Array& aArray, Functor aFuncL, 
             Arg& aArgument, Arg2& aArgument2, 
             TBool aInverted = EFalse)
    {
    const TInt count = aArray.Count();
    for (TInt i=0; i<count; ++i)
        {
        TBool exitCode = aFuncL(aArray[i], aArgument, aArgument2);
        if( !aInverted )
            {
            if(exitCode)
                {
                return exitCode;
                }
            }
        else
            {
            if(!exitCode)
                {
                return exitCode;
                }
            }
        }
    return aInverted;
    }

// End of File