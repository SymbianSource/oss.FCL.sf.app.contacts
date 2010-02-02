/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TVPBKVCARDFIELDTYPEMAPPING_H
#define TVPBKVCARDFIELDTYPEMAPPING_H

#include <VPbkFieldType.hrh>
#include <TVPbkFieldVersitProperty.h>

class MVPbkFieldType;
class MVPbkFieldTypeList;
class TVPbkFieldTypeParameters;
namespace VPbkEngUtils
    {
    class CTextStore;    
    }

NONSHARABLE_CLASS( TVPbkVCardFieldTypeMapping )
    {
    public:
        void InitializeL
            ( TResourceReader& aResReader,
              VPbkEngUtils::CTextStore& aTextStore );

    public:
        /**
         * Returns a field type that matches this type mapping.
         */
        const MVPbkFieldType* FindMatch
            ( const MVPbkFieldTypeList& aFieldTypeList ) const;

        /**
         * Returns the versit property name.
         */
        inline TVPbkFieldTypeName Name() const;

        /**
         * Returns the versit property subfield.
         */
        inline TVPbkSubFieldType SubField() const;

        /**
         * Returns the versit property parameters (read-only).
         */
        inline const TVPbkFieldTypeParameters& Parameters() const;

        /**
         * Returns the versit property parameters (read-write).
         */
        inline TVPbkFieldTypeParameters& Parameters();

        /**
         * Returns the versit extension name.
         */
        inline TPtrC8 ExtensionName() const;        
        
        inline TVPbkNonVersitFieldType NonVersitType() const;
        
    private:    
        TVPbkFieldVersitProperty iVersitProperty;
        TVPbkNonVersitFieldType iNonVersitType;
    };
    
inline TVPbkFieldTypeName TVPbkVCardFieldTypeMapping::Name() const
    {
    return iVersitProperty.Name();
    }

inline TVPbkSubFieldType TVPbkVCardFieldTypeMapping::SubField() const
    {
    return iVersitProperty.SubField();
    }

inline const TVPbkFieldTypeParameters& TVPbkVCardFieldTypeMapping::Parameters
        ( ) const
    {
    return iVersitProperty.Parameters();
    }

inline TVPbkFieldTypeParameters& TVPbkVCardFieldTypeMapping::Parameters()
    {
    return iVersitProperty.Parameters();
    }

inline TPtrC8 TVPbkVCardFieldTypeMapping::ExtensionName() const
    {
    return iVersitProperty.ExtensionName();
    }    
    
inline TVPbkNonVersitFieldType TVPbkVCardFieldTypeMapping::NonVersitType() const
    {
    return iNonVersitType;
    }
    
#endif // TVPBKVCARDFIELDTYPEMAPPING_H

// End of file
    
