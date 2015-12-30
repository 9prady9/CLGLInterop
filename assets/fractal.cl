#include <colormaps.h>

constant unsigned MAX_ITERS = 512;
constant unsigned NUM_COLORS = 257; // 257 colors are there in spectrum map defined in colormaps.h

int isInside(float2 coords, float2 C)
{
    float2 temp;

    unsigned i;
    for(i=0; i<MAX_ITERS; i++)
    {
        temp.s0 = (coords.s0 * coords.s0 - coords.s1 * coords.s1) + C.s0;
        temp.s1 = 2 * coords.s0 * coords.s1 + C.s1;
		coords  = temp;

        if(dot(coords, coords) > 4) {
            break;
        }
    }

    return i;
}

kernel
void fractal(write_only image2d_t out, int dim0, int dim1,
             float scalex, float scaley, float movex, float movey,
             float centerx, float centery)
{
    const int gx = get_global_id(0);
    const int gy = get_global_id(1);

    float h0 = dim0/2.0f;
    float h1 = dim1/2.0f;

    float2 scale     = (float2)(scalex, scaley);
    float2 translate = (float2)(movex, movey);
    float2 npos      = (float2)((gx-h0)/h0, (gy-h1)/h1);
    float2 center    = (float2)(centerx, centery);

    npos = npos * scale + translate;

    if (gx<dim0 && gy<dim1) {
        int iteration   = isInside(npos, center);
        int colorIndex  = iteration%NUM_COLORS;
        write_imagef(out, (int2)(gx,gy), SPECTRUM[NUM_COLORS-1-colorIndex]);
    }
}
