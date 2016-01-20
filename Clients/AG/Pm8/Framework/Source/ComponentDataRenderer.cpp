// ****************************************************************************
//
//  File Name:			ComponentDataRenderer.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of RComponentDataRenderer
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/ComponentDataRenderer.cpp                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "ComponentDataRenderer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "CompositeSelection.h"
#include "DragDropDataTransfer.h"
#include "View.h"
#include "ComponentDocument.h"

// ****************************************************************************
//
//  Function Name:	RComponentDataRenderer::RenderData( )
//
//  Description:		Called to render data in the specified format
//
//  Returns:			TRUE if successful
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentDataRenderer::RenderData( RDataTransferSource* pDataSource, RDataTransferTarget* pDataTarget, YDataFormat dataFormat )
	{
	BOOLEAN fResult = TRUE;

	// If we are doing drag&drop, we might be looking for the component data
	if( dataFormat == kComponentFormat )
		{
		try
			{
			// Get the drag & drop info
			RDragDropInfo dragDropInfo;
			dragDropInfo.Paste( *pDataSource );

			// Render the component data
			RComponentCollection componentCollection( *dragDropInfo.m_Data.m_pSourceView->GetSelection( ) );
			componentCollection.Copy( *pDataTarget, kComponentFormat );
			}

		catch( YException exception )
			{
			::ReportException ( exception );
			fResult = FALSE;
			}

		catch( ... )
			{
			fResult = FALSE;
			}
		}

	// Otherwise, we need to pull down the component data and convert it to something else
	else
		{
		RComponentCollection componentCollection;

		try
			{
			// Ask for component data
			componentCollection.Paste( *pDataSource, NULL );

			// If there arent any components, we cant do anything
			if( componentCollection.Count( ) == 0 )
				throw "No components";
				
			// If there is only one component, let it render itself; it might know how to do a better job.
			else if( componentCollection.Count( ) == 1 )
				( *componentCollection.Start( ) )->Copy( dataFormat, *pDataTarget );

			// If there are multiple components, do it ourself
			else
				componentCollection.Copy( *pDataTarget, dataFormat );
			}

		catch( YException exception )
			{
			::ReportException( exception );
			fResult = FALSE;
			}

		catch( ... )
			{
			fResult = FALSE;
			}

		componentCollection.DeleteAllComponents( );
		}

	return fResult;
	}
