/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Listbox model decorator that adds commands to the list.
*
*/


// INCLUDES
#include "CPbk2ListboxModelCmdDecorator.h"

#include <CPbk2IconArray.h>
#include <Pbk2UID.h>
#include <Pbk2Commands.hrh>
#include <Pbk2IconId.hrh>

// LOCAL CONSTANTS AND MACROS
const TText KSeparator = '\t';



// ================= MEMBER FUNCTIONS =======================

CPbk2ListboxModelCmdDecorator* CPbk2ListboxModelCmdDecorator::NewL(
    const CPbk2IconArray& aIconArray,
    TPbk2IconId aEmptyIconId,
    TPbk2IconId aDefaultIconId )
    {
    CPbk2ListboxModelCmdDecorator* self = new (ELeave) CPbk2ListboxModelCmdDecorator(
	    aIconArray,
	    aEmptyIconId,
	    aDefaultIconId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CPbk2ListboxModelCmdDecorator::~CPbk2ListboxModelCmdDecorator()
    {
    }

TInt CPbk2ListboxModelCmdDecorator::MdcaCount() const
    {
    TInt count = 0; // No list items
    if ( iDecoratedModel )
    	{
    	count = iDecoratedModel->MdcaCount() + NumberOfEnabledCommands();
    	}
    return count;
    }

TPtrC CPbk2ListboxModelCmdDecorator::MdcaPoint( TInt aIndex ) const
    {
    TPtrC result;
    const TInt enabledCommandCount = NumberOfEnabledCommands();
    if ( aIndex < enabledCommandCount )
    	{
    	// It is a command that is to be shown.
    	
    	// Need to create a listbox compatible string.
        TInt iconIndex = iIconArray.FindIcon(iEmptyIconId);
        if (iconIndex < 0)
            {
            iconIndex = iIconArray.FindIcon(iDefaultIconId);
            }

        if(EnabledCommand( aIndex ).CommandId() == EPbk2CmdRcl )
            {
            iconIndex = iIconArray.FindIcon(TPbk2IconId(
                TUid::Uid(KPbk2UID3), EPbk2qgn_prop_group_small )); //TODO: use correct icon
            }
        
        iBuffer.Num(iconIndex);
        iBuffer.Append(KSeparator);
        iBuffer.Append( EnabledCommand( aIndex ).NameForUi() );
    	result.Set( iBuffer );
    	}
    else
    	{
    	// It is a contact that is to be shown.
    	result.Set(
    		iDecoratedModel->MdcaPoint( aIndex - enabledCommandCount) );
    	}
    return result;
    }

void CPbk2ListboxModelCmdDecorator::SetDecoratedModel( const MDesCArray& aModel )
    {
    iDecoratedModel = &aModel;
    }

void CPbk2ListboxModelCmdDecorator::SetListCommands(
	const RPointerArray<MPbk2UiControlCmdItem>* aCommands )
	{
	iCommands = aCommands;
	}

TInt CPbk2ListboxModelCmdDecorator::NumberOfEnabledCommands() const
	{
	TInt enabledCommandCount = 0;
	if ( iCommands )
		{
		for ( TInt n = 0; n < iCommands->Count(); ++n )
			{
			const MPbk2UiControlCmdItem& cmd = *(*iCommands)[n];
			if ( cmd.IsEnabled() )
				{
				enabledCommandCount++;
				}
			}
		}
	return enabledCommandCount;
	}

MPbk2UiControlCmdItem& CPbk2ListboxModelCmdDecorator::EnabledCommand( TInt aIndex ) const
	{
	TInt enabledCount = 0;
	TInt indexOfResult = KErrNotFound;
	for ( TInt n = 0; n < iCommands->Count()&& indexOfResult == KErrNotFound; ++n )
		{
		if ( (*iCommands)[ n ]->IsEnabled() )
			{
			enabledCount++;
			if ( enabledCount-1 == aIndex )
				{
				indexOfResult = n;
				}
			}
		}
	return *(*iCommands)[ indexOfResult ];
	}


CPbk2ListboxModelCmdDecorator::CPbk2ListboxModelCmdDecorator(
	const CPbk2IconArray& aIconArray,
	TPbk2IconId aEmptyIconId,
	TPbk2IconId aDefaultIconId ) :
	iIconArray( aIconArray ),
	iEmptyIconId( aEmptyIconId ),
	iDefaultIconId( aDefaultIconId ),
	iBuffer( iFormattingBuffer )
    {
    }

void CPbk2ListboxModelCmdDecorator::ConstructL()
    {
    }

// end of file
