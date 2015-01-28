using System.Windows.Controls;
using System.Windows.Input;

namespace XamlViewer
{
	public class EditingTextBox : TextBox
	{
		const int TAB_SPACES = 4;

		protected override void OnKeyDown(KeyEventArgs args)
		{
			if (args.Key == Key.Tab)
			{
				int line, col;
				GetPositionFromIndex(SelectionStart, out line, out col);
				SelectedText = new string(' ', TAB_SPACES - col % TAB_SPACES);
				args.Handled = true;
				return;
			}

			base.OnKeyDown(args);
		}

		public void GetPositionFromIndex(int index, out int line, out int col)
		{
			if (index > Text.Length)
			{
				line = col = -1;
				return;
			}

			line = col = 0;
			bool justGottaEol = false;

			for (int i = 0; i < index; i++)
			{
				if (Text[i] == '\r' || Text[i] == '\n')
				{
					if (!justGottaEol)
						line++;

					col = 0;
					justGottaEol = true;
				}
				else
				{
					col++;
					justGottaEol = false;
				}
			}
			return;
		}
	}
}
