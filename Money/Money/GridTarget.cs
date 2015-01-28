using System;
using System.Windows;
using ClassLibrary;

namespace Money
{
	public class GridTarget : GridPanel, IDropTarget
	{
		public GridTarget()
		{
		}

		public void OnDropTargetEvent(DragDrop dragDrop, DropTargetEventType type)
		{
			if (dragDrop.Target != this)
				return;
			Package package = null;
			if (dragDrop.Payload is Package)
				package = (Package)dragDrop.Payload;
			else
			if (dragDrop.Payload is Int64)
				package = PackagesPanel.Current.Tree.FindPackageById((Int64)dragDrop.Payload);
			if (package == null)
				return;
			//Point mousePoint = dragDrop.GetPosition(dragDrop.Target);
			if (type == DropTargetEventType.Enter)
				dragDrop.DefaultEnterEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Over)
				dragDrop.DefaultOverEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Leave)
				dragDrop.DefaultLeaveEventHandler(dragDrop.Target);
			else
			if (type == DropTargetEventType.Drop)
				PackagesPanel.Current.CreatePackageDialog(package.Id, true/*bForUseInDocument*/);
		}
	}
}
