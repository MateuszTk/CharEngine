using System;
using System.Drawing;


namespace Test
{
    class Program
    {
        static char[] shades = { ' ', '.', ',', '!', '*', '#', '%' ,'@' };
        const int yTargetResolution = 68;

        static void Main(string[] args)
        {
            try
            {
                Bitmap image = (Bitmap)Image.FromFile(@"F:\obrazy\" + "test.png", true);//gradient-h.png

                Color pixelColor;
                int imWidth = image.Width;
                int imHeight = image.Height;
                string pixels = "";

                float yStep = (float)imHeight / yTargetResolution;
                int xAdaptiveResolution = (int)((float)imWidth / (float)imHeight * (float)yTargetResolution * 1.94f);
                float xStep = (float)imWidth / xAdaptiveResolution;

                for (int y = 0; y < yTargetResolution - 1; y++)
                {
                    pixels = "";
                    for (int x = 0; x < xAdaptiveResolution - 1; x++)
                    {
                        pixelColor = image.GetPixel((int)(x * xStep), (int)(y * yStep));
                        pixels += shades[(int)MathF.Round(MathF.Max(pixelColor.R, MathF.Max(pixelColor.G, pixelColor.B)) / 255 * (shades.Length - 1))];
                    }
                    Console.WriteLine(pixels);
                }

            }
            catch (System.IO.FileNotFoundException)
            {
                Console.Write("There was an error opening the bitmap. Please check the path.");
            }
        }
    }
}
