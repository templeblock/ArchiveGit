using System.ServiceModel.DomainServices.Client;

namespace Money
{
	internal static class RiaServicesHelper
	{
		// Extension for OperationBase
		internal static string CheckErrorStatus(this OperationBase operation)
		{
			if (!operation.HasError)
				return null;
			
			if (!operation.IsErrorHandled)
				operation.MarkErrorAsHandled(); // must be called
			string errorStatus = (operation.Error != null ? operation.Error.Message : null);
			return errorStatus;
		}
	}
}
