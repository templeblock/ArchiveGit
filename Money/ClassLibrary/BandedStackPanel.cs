// Copyright (C) Microsoft Corporation. All Rights Reserved.
// This code released under the terms of the Microsoft Public License
// (Ms-PL, http://opensource.org/licenses/ms-pl.html).

using System;
using System.Windows;
using System.Windows.Controls;

namespace ClassLibrary
{
    /// <summary>
    /// BandedStackPanel behaves like a StackPanel and adds support for "bands"
    /// which lay elements out side-by-side perpendicular to the usual orientation.
    /// </summary>
    public class BandedStackPanel : Panel
    {
        /// <summary>
        /// Gets or sets the number of bands (columns or rows) to display in the secondary direction.
        /// </summary>
        public int Bands
        {
            get { return (int)GetValue(BandsProperty); }
            set { SetValue(BandsProperty, value); }
        }
        public static readonly DependencyProperty BandsProperty = DependencyProperty.Register(
            "Bands", typeof(int), typeof(BandedStackPanel), new PropertyMetadata(1, OnBandsPropertyChanged));
        private static void OnBandsPropertyChanged(DependencyObject o, DependencyPropertyChangedEventArgs e)
        {
            if ((int)e.NewValue < 1)
            {
                throw new ArgumentException("Value of Bands property must be greater than 0.");
            }
            ((BandedStackPanel)o).InvalidateMeasure();
        }

        /// <summary>
        /// Gets or sets the orientation to use for the primary direction.
        /// </summary>
        public Orientation Orientation
        {
            get { return (Orientation)GetValue(OrientationProperty); }
            set { SetValue(OrientationProperty, value); }
        }
        public static readonly DependencyProperty OrientationProperty = DependencyProperty.Register(
            "Orientation", typeof(Orientation), typeof(BandedStackPanel), new PropertyMetadata(Orientation.Vertical, OnOrientationPropertyChanged));
        private static void OnOrientationPropertyChanged(DependencyObject o, DependencyPropertyChangedEventArgs e)
        {
            var newOrientation = (Orientation)e.NewValue;
            if ((Orientation.Vertical != newOrientation) && (Orientation.Horizontal != newOrientation))
            {
                throw new ArgumentException("Unsupported Orientation value.");
            }
            ((BandedStackPanel)o).InvalidateMeasure();
        }

        /// <summary>
        /// Implements custom measure logic.
        /// </summary>
        /// <param name="constraint">Constraint to measure within.</param>
        /// <returns>Desired size.</returns>
        protected override Size MeasureOverride(Size constraint)
        {
            var bands = Bands;
            var orientation = Orientation;
            // Calculate the Size to Measure children with
            var constrainedLength = new OrientedLength(orientation, constraint);
            constrainedLength.PrimaryLength = double.PositiveInfinity;
            constrainedLength.SecondaryLength /= bands;
            var availableLength = constrainedLength.Size;
            // Measure each child
            var band = 0;
            var levelLength = new OrientedLength(orientation);
            var usedLength = new OrientedLength(orientation);
            foreach (UIElement child in Children)
            {
                child.Measure(availableLength);
                // Update for the band/level of this child
                var desiredLength = new OrientedLength(orientation, child.DesiredSize);
                levelLength.PrimaryLength = Math.Max(levelLength.PrimaryLength, desiredLength.PrimaryLength);
                levelLength.SecondaryLength += desiredLength.SecondaryLength;
                // Move to the next band
                band = (band + 1) % bands;
                if (0 == band)
                {
                    // Update for the complete level; reset for the next one
                    usedLength.PrimaryLength += levelLength.PrimaryLength;
                    usedLength.SecondaryLength = Math.Max(usedLength.SecondaryLength, levelLength.SecondaryLength);
                    levelLength.PrimaryLength = 0;
                    levelLength.SecondaryLength = 0;
                }
            }
            // Update for the partial level at the end
            usedLength.PrimaryLength += levelLength.PrimaryLength;
            usedLength.SecondaryLength = Math.Max(usedLength.SecondaryLength, levelLength.SecondaryLength);
            // Return the used size
            return usedLength.Size;
        }

        /// <summary>
        /// Implements custom arrange logic.
        /// </summary>
        /// <param name="arrangeSize">Size to arrange to.</param>
        /// <returns>Used size.</returns>
        protected override Size ArrangeOverride(Size arrangeSize)
        {
            var bands = Bands;
            var orientation = Orientation;
            var count = Children.Count;
            // Prepare the Rect to arrange children with
            var arrangeLength = new OrientedLength(orientation, arrangeSize);
            arrangeLength.SecondaryLength /= bands;
            // Arrange each child
            for (var i = 0; i < count; i += bands)
            {
                // Determine the length of the current level
                arrangeLength.PrimaryLength = 0;
                arrangeLength.SecondaryOffset = 0;
                for (var band = 0; (band < bands) && (i + band < count); band++)
                {
                    var desiredLength = new OrientedLength(orientation, Children[i + band].DesiredSize);
                    arrangeLength.PrimaryLength = Math.Max(arrangeLength.PrimaryLength, desiredLength.PrimaryLength);
                }
                // Arrange each band within the level
                for (var band = 0; (band < bands) && (i + band < count); band++)
                {
                    Children[i + band].Arrange(arrangeLength.Rect);
                    arrangeLength.SecondaryOffset += arrangeLength.SecondaryLength;
                }
                // Update for the next level
                arrangeLength.PrimaryOffset += arrangeLength.PrimaryLength;
            }
            // Return the arranged size
            return arrangeSize;
        }

        /// <summary>
        /// Helper class to provide an abstraction for Width/Height and enable
        /// the same code to work for both Horizontal and Vertical orientation.
        /// </summary>
        private class OrientedLength
        {
            private readonly Orientation _orientation;

            public OrientedLength(Orientation orientation)
            {
                _orientation = orientation;
            }
            public OrientedLength(Orientation orientation, Size size)
                : this(orientation)
            {
                if (Orientation.Horizontal == _orientation)
                {
                    SecondaryLength = size.Height;
                    PrimaryLength = size.Width;
                }
                else
                {
                    SecondaryLength = size.Width;
                    PrimaryLength = size.Height;
                }
            }

            public double SecondaryOffset { get; set; }
            public double PrimaryOffset { get; set; }
            public double PrimaryLength { get; set; }
            public double SecondaryLength { get; set; }

            public Size Size
            {
                get
                {
                    return (Orientation.Horizontal == _orientation) ?
                        new Size(PrimaryLength, SecondaryLength) :
                        new Size(SecondaryLength, PrimaryLength);
                }
            }

            public Rect Rect
            {
                get
                {
                    return (Orientation.Horizontal == _orientation) ?
                        new Rect(PrimaryOffset, SecondaryOffset, PrimaryLength, SecondaryLength) :
                        new Rect(SecondaryOffset, PrimaryOffset, SecondaryLength, PrimaryLength);
                }
            }
        }
    }
}
