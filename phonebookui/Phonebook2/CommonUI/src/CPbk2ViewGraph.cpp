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
* Description:  Phonebook 2 view nodes and graphs.
*
*/


// INCLUDE FILES
#include <CPbk2ViewGraph.h>

// System includes
#include <barsc.h>      // RResourceFile
#include <barsread.h>   // TResourceReader

/// Unnamed namespace for local definitions
namespace {

/**
 * Comparison operator for TPbk2ViewTransition objects.
 *
 * @param aLeft     Left hand side to compare.
 * @param aRight    Right hand side to compare.
 * @return  ETrue if the objects are equal, EFalse otherwise.
 */
inline TBool operator==
        ( const TPbk2ViewTransition& aLeft,
          const TPbk2ViewTransition& aRight )
    {
    return ( aLeft.iEvent == aRight.iEvent && aLeft.iNode == aRight.iNode );
    }

/**
 * Compares tab group orderings.
 *
 * @param aLhs  Left hand side (LHS).
 * @param aRhs  Right hand side (RHS).
 * @return  Positive integer if LHS is greater than RHS.
 *          Zero if LHS is equal to RHS.
 *          Negative integer if RHS is greater than LHS.
 */
TInt TabGroupOrdering
        ( const CPbk2ViewNode& aLhs, const CPbk2ViewNode& aRhs )
    {
    return ( aLhs.TabGroupOrdering() - aRhs.TabGroupOrdering() );
    }

#ifdef _DEBUG

enum TPaniCode
    {
    EPanic_NodeAlreadyExists = 1,
    EPanic_FindViewsInTabGroupL_OOB
    };

void Panic(TPaniCode aReason)
    {
    _LIT( KPanicText, "CPbk2ViewGraph" );
    User::Panic( KPanicText, aReason );
    }

#endif // _DEBUG

} /// namespace



// --------------------------------------------------------------------------
// CPbk2ViewNode::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewNode* CPbk2ViewNode::NewL(
        TUid aViewId, 
        TUid aDefaultPrevViewId, 
        TBool aExitNode,
        TPbk2TabGroupId aTabGroupId,
        TPbk2TabGroupOrdering aTabGroupOrdering,
        TInt aTabResourceId,
        TResourceReader& aTransitionReader)
    {
    CPbk2ViewNode* self = new(ELeave) CPbk2ViewNode(aViewId, 
        aDefaultPrevViewId, aExitNode, aTabGroupId, aTabGroupOrdering,
        aTabResourceId);
    CleanupStack::PushL(self);
    self->ReadTransitionsL(aTransitionReader);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::CPbk2ViewNode
// --------------------------------------------------------------------------
//
CPbk2ViewNode::CPbk2ViewNode() :
    iTransitions( 1 )  // Reallocation granularity of 1
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::CPbk2ViewNode
// --------------------------------------------------------------------------
//
CPbk2ViewNode::CPbk2ViewNode(
        TUid aViewId, 
        TUid aDefaultPrevViewId, 
        TBool aExitNode,
        TPbk2TabGroupId aTabGroupId,
        TPbk2TabGroupOrdering aTabGroupOrdering,
        TInt aTabResourceId) :
    iViewId ( aViewId ), 
    iDefaultPrevViewId ( aDefaultPrevViewId ), 
    iExitNode ( aExitNode ),
    iTransitions( 1 ),  // Reallocation granularity of 1    
    iTabGroupId ( aTabGroupId ),
    iTabGroupOrdering ( aTabGroupOrdering ),
    iTabResourceId ( aTabResourceId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::~CPbk2ViewNode
// --------------------------------------------------------------------------
//
CPbk2ViewNode::~CPbk2ViewNode()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::FindTransition
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ViewNode::FindTransition
        ( const TPbk2ViewTransition& aTransition ) const
    {
    for (TInt i=0; i < iTransitions.Count(); ++i)
        {
        if (iTransitions[i] == aTransition)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::FindTransition
// --------------------------------------------------------------------------
//
TPbk2ViewTransition* CPbk2ViewNode::FindTransition
        ( TPbk2ViewTransitionEvent aEvent )
    {
    TPbk2ViewTransition* transition = NULL;

    for ( TInt i=0; i < iTransitions.Count(); ++i )
        {
        if ( iTransitions[i].iEvent == aEvent )
            {
            transition = &iTransitions[i];
            break;
            }
        }
    return transition;
    }

// --------------------------------------------------------------------------
// CPbk2ViewNode::ReadTransitionsL
// --------------------------------------------------------------------------
//
void CPbk2ViewNode::ReadTransitionsL( TResourceReader& aResReader )
    {
    // read view transitions
    TInt transCount = aResReader.ReadInt16(); // transitions[]
    iTransitions.SetReserveL(transCount);
    while (transCount-- > 0)
        {
        // read PBK2_VIEW_TRANSITION resource struct
        aResReader.ReadInt8();  // read version number
        TPbk2ViewTransition trans;
        trans.iEvent = TPbk2ViewTransitionEvent(aResReader.ReadInt32());
        trans.iTargetViewId.iUid = aResReader.ReadInt32();
        trans.iNode = NULL;
        iTransitions.AppendL(trans);
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::~CPbk2ViewGraph
// --------------------------------------------------------------------------
//
CPbk2ViewGraph::~CPbk2ViewGraph()
    {
    if (iNodes)
        {
        iNodes->ResetAndDestroy();
        }
    delete iNodes;
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewGraph* CPbk2ViewGraph::NewL( TResourceReader& aResReader )
    {
    CPbk2ViewGraph* self = new(ELeave) CPbk2ViewGraph;
    CleanupStack::PushL(self);
    self->ConstructL(aResReader);
    CleanupStack::Pop(self);  //self
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ViewGraph::ConstructL( TResourceReader& aResReader )
    {
    iNodes = new ( ELeave ) CPbk2ViewNodeArray( 1 );
    AppendNodesFromResourceL( aResReader );
    // unravel the view id's to pointers to view graph nodes
    LinkNodesWithTransitions();
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::FindNodeWithViewId
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewNode* CPbk2ViewGraph::FindNodeWithViewId
        ( TUid aViewId ) const
    {
    if (aViewId != KNullUid)
        {
        TInt count = iNodes->Count();
        while (count-- > 0)
            {
            CPbk2ViewNode* node = iNodes->At(count);
            if (node->iViewId == aViewId)
                {
                return node;
                }
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::ModifyViewGraphL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewGraph::ModifyViewGraphL( 
        TResourceReader& aResReader )
    {
    AppendNodesFromResourceL( aResReader );
    // unravel the view id's to pointers to view graph nodes
    LinkNodesWithTransitions();
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::AddViewNode
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ViewGraph::AddViewNodeL(
        CPbk2ViewNode* aNode)
    {
    __ASSERT_DEBUG(!FindNodeWithViewId(aNode->iViewId),
                   Panic(EPanic_NodeAlreadyExists));

    iNodes->AppendL(aNode);
    // unravel the view id's to pointers to view graph nodes
    LinkNodesWithTransitions();
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::FindViewsInTabGroupL
// returns the nodes in the tab group in the tab group sort order
// --------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CPbk2ViewNode> CPbk2ViewGraph::FindViewsInTabGroupL( 
        TPbk2TabGroupId aTabGroupId ) const
    {
    RPointerArray<CPbk2ViewNode> result;
    CleanupClosePushL(result);

    TLinearOrder<CPbk2ViewNode> ordering(TabGroupOrdering);
    const TInt count = Count();
    
    __ASSERT_DEBUG( iNodes->Count() >= count, 
        Panic( EPanic_FindViewsInTabGroupL_OOB ) );
    
    // loop through all the view graph nodes    
    for (TInt i = 0; i < count; ++i)
        {
        // if the tab group id is the one were looking for
        // insert it into the result array
        if (iNodes->At(i)->iTabGroupId == aTabGroupId)
            {
            result.InsertInOrder(iNodes->At(i), ordering);
            }
        }
    CleanupStack::Pop(); // result
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::AppendNodesFromResourceL
// Appends the nodes from resource to iNodes.
// --------------------------------------------------------------------------
//
void CPbk2ViewGraph::AppendNodesFromResourceL( TResourceReader& aResReader )
    {
    // reads an array of PBK2_VIEW_NODE resource structures
    TInt count = aResReader.ReadInt16();
    iNodes->SetReserveL(iNodes->Count() + count);
    while (count-- > 0)
        {
        // create view node for each element in resource structure
        CPbk2ViewNode* node = new(ELeave) CPbk2ViewNode;
        CleanupStack::PushL(node);

        // read PBK2_VIEW_NODE resource struct
        aResReader.ReadInt8();  // read version number
        node->iViewId.iUid = aResReader.ReadInt32();
        node->iDefaultPrevViewId.iUid = aResReader.ReadInt32();
        node->iDefaultPreviousNode = NULL;
        node->iPreviousNode = NULL;
        node->iExitNode = aResReader.ReadInt8();

        // read view transitions
        node->ReadTransitionsL(aResReader);

        // read tab group id's and resource id's
        node->iTabGroupId = TPbk2TabGroupId(aResReader.ReadInt32());
        node->iTabGroupOrdering = TPbk2TabGroupOrdering( 
                aResReader.ReadInt32() );
        // reads the link id of a PBK2_VIEW_NODE_TAB structure
        node->iTabResourceId = aResReader.ReadInt32();

        __ASSERT_DEBUG(!FindNodeWithViewId(node->iViewId),
                       Panic(EPanic_NodeAlreadyExists));
        iNodes->AppendL(node);
        CleanupStack::Pop();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ViewGraph::LinkNodesWithTransitions
// --------------------------------------------------------------------------
//
void CPbk2ViewGraph::LinkNodesWithTransitions()
    {
    // loop through all the view graph nodes
    const TInt count( iNodes->Count() );
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2ViewNode* node = iNodes->At(i);
        // Find the nodes previous views node from the graph
        // and set this nodes previous node pointer
        node->iDefaultPreviousNode = FindNodeWithViewId
            ( node->iDefaultPrevViewId );

        // loop through the transitions of this node
        TInt transCount = node->iTransitions.Count();
        while (transCount-- > 0)
            {
            TPbk2ViewTransition& trans = node->iTransitions.At(transCount);
            switch (trans.iEvent)
                {
                // ignore left and right transitions
                case EPbk2ViewTransitionLeft:
                    {
                    break;
                    }
                case EPbk2ViewTransitionRight:
                    {
                    break;
                    }
                default:
                    {
                    // find the transitions node pointer and set it to the
                    // transition node
                    trans.iNode = FindNodeWithViewId(trans.iTargetViewId);
                    break;
                    }
                }
            }
        }
    }

//  End of File
