#include "vec3.h"

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
            vec3 col( float(i) / float(w), float(j) / float(h), 0.2f );

            img[k++] = char( 255.99f*col[0] );
            img[k++] = char( 255.99f*col[1] );
            img[k++] = char( 255.99f*col[2] );
        }
    }

    stbi_write_png( "out.png", w, h, 3, img, 0 );
}