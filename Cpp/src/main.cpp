//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
    int w = 200;
    int h = 100;

    char* img = new char[ w * h * 3 ];
    int k = 0;

    for ( int j = h-1; j >= 0; j-- )
    {
        for ( int i = 0; i < w; i++ )
        {
            float r = float(i) / float(w);
            float g = float(j) / float(h);
            float b = 0.2f;

            img[k++] = char( 255.99f*r );
            img[k++] = char( 255.99f*g );
            img[k++] = char( 255.99f*b );
        }
    }

    stbi_write_png( "out.png", w, h, 3, img, 0 );
}