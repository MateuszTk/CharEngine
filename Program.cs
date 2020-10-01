using System;
using System.Drawing;


namespace Test
{
    class Program
    {
        static char[] chars = { '@', '%', '#', '*', '!', '.' ,' ' };

        static void Main(string[] args)
        {
            try
            {
                Bitmap image = (Bitmap)Image.FromFile(@"F:\obrazy\" + "test.png", true);//gradient-h.png

                Color pixelColor;
                int imWidth = image.Width;
                int imHeight = image.Height;
                string pixels = "";

                for (int y = 0; y < imHeight - 1; y++)
                {
                    pixels = "";
                    for (int x = 0; x < imWidth - 1; x++)
                    {
                        pixelColor = image.GetPixel(x, y);
                        pixels += chars[(int)MathF.Round(MathF.Max(pixelColor.R, MathF.Max(pixelColor.G, pixelColor.B)) / 255 * (chars.Length - 1))];
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
