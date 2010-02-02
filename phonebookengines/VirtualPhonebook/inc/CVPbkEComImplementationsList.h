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
* Description:  Virtual Phonebook utility for listing ECOM implementations
*
*/


#ifndef CVPBKECOMIMPLEMENTATIONSLIST_H
#define CVPBKECOMIMPLEMENTATIONSLIST_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATIONS

/**
 * Virtual Phonebook utility for listing ECOM implementations
 *
 * Lists all ecom implementations according to interface UID.
 */
class CVPbkEComImplementationsListBase : public CBase
    {
    public: // Construction and destruction
        IMPORT_C ~CVPbkEComImplementationsListBase();

    public: // Interface

        /**
         * Create ECOM implementations for aInterfaceUid and give aParam
         * for them.
         *
         * @param aInterfaceUid An ECOM interface UID
         * @param aParam The parameter for the implementations.
         */
        IMPORT_C void LoadImplementationsL( TUid aInterfaceUid, TAny* aParam );

        /**
         * Create ECOM implementations for aInterfaceUid and give aParam
         * for them.
         *
         * @param aInterfaceUid An ECOM interface UID
         * @param aResolverParams parameters for ECOM resolver
         * @param aParam The parameter for the implementations.
         */
        IMPORT_C void LoadImplementationsL( TUid aInterfaceUid,
                const TEComResolverParams& aResolverParams, TAny* aParam );

        /**
         * Returns the number of implementations in this list.
         *
         * @return the number of implementations in this list.
         */
        IMPORT_C TInt Count() const;

        /**
         * Returns the implementation at position aIndex
         *
         * @param aIndex the position of the implemetation.
         * @return The implementation at position aIndex
         */
        IMPORT_C CBase& At( TInt aIndex ) const;

        /**
         * Returns the ECOM implementation information for implementation
         * aImpl or NULL.
         *
         * @param aImpl The implementation retrieved using At() in this
         *              list.
         * @return The ECOM implementation information for implementation
         *         aImpl or NULL.
         */
        IMPORT_C const CImplementationInformation* FindInfo(
                CBase& aImpl ) const;

    protected: // Interface
        IMPORT_C CVPbkEComImplementationsListBase();

    private: // Implementation
        virtual CBase* CreateImplementationL( TUid aImplementationUid,
                TAny* aParam ) = 0;
        void DoLoadImplementationsL( RImplInfoPtrArray& aImplInfoPtrArray,
                TAny* aParam );

    private: // Data
        class CEComImpl : public CBase
            {
            public:
                CEComImpl( CImplementationInformation* aInfo,
                    CBase* aImpl );
                ~CEComImpl();
                CImplementationInformation* iInfo;
                CBase* iImpl;
            };
        RPointerArray<CEComImpl> iImplementations;
    };

/**
 * A template for a list of type T ECOM implementations.
 *
 * An implementation class must be CBase derived class that has a
 * NewL( TUid aImplementationUid, <ParamT>& aParam )
 */
template<typename T, typename ParamT>
NONSHARABLE_CLASS( CVPbkEComImplementationsList )
        :   public CVPbkEComImplementationsListBase
    {
    public: // See documentation from CVPbkEComImplementationsListBase
        inline void LoadImplementationsL( TUid aInterfaceUid,
                ParamT& aParam );
        inline void LoadImplementationsL( TUid aInterfaceUid,
                const TEComResolverParams& aResolverParams, ParamT& aParam );
        inline TInt Count() const;
        inline T& At( TInt aIndex ) const;
        inline const CImplementationInformation* FindInfo( T& aImpl ) const;

        // From CVPbkEComImplementationsListBase
        inline CBase* CreateImplementationL( TUid aImplementationUid,
                TAny* aParam );
    };

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::LoadImplementationsL
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
inline void CVPbkEComImplementationsList<T,ParamT>::LoadImplementationsL(
        TUid aInterfaceUid, ParamT& aParam )
    {
    CVPbkEComImplementationsListBase::LoadImplementationsL( aInterfaceUid,
            &aParam );
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::LoadImplementationsL
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
inline void CVPbkEComImplementationsList<T,ParamT>::LoadImplementationsL(
        TUid aInterfaceUid, const TEComResolverParams& aResolverParams,
        ParamT& aParam )
    {
    CVPbkEComImplementationsListBase::LoadImplementationsL( aInterfaceUid,
            aResolverParams, &aParam );
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::Count
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
inline TInt CVPbkEComImplementationsList<T,ParamT>::Count() const
    {
    return CVPbkEComImplementationsListBase::Count();
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::At
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
inline T& CVPbkEComImplementationsList<T,ParamT>::At( TInt aIndex ) const
    {
    return static_cast<T&>( CVPbkEComImplementationsListBase::At( aIndex ));
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::FindInfo
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
const CImplementationInformation*
        CVPbkEComImplementationsList<T,ParamT>::FindInfo(
            T& aImpl ) const
    {
    return CVPbkEComImplementationsListBase::FindInfo( aImpl );
    }

// --------------------------------------------------------------------------
// CVPbkEComImplementationsList<T,ParamT>::CreateImplementationL
// --------------------------------------------------------------------------
//
template<typename T, typename ParamT>
inline CBase* CVPbkEComImplementationsList<T,ParamT>::CreateImplementationL(
        TUid aImplementationUid, TAny* aParam )
    {
    return T::NewL( aImplementationUid,
            *reinterpret_cast<ParamT*>( aParam ) );
    }

#endif // CVPBKECOMIMPLEMENTATIONSLIST_H
//End of file

