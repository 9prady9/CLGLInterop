#include <colormaps.h>

constant unsigned MAX_ITERS = 512;
constant unsigned NUM_COLORS = 257; // 257 colors are there in spectrum map defined in colormaps.h

constant float2 JULIA_SET[5] = {
    (float2)(-0.700f, 0.270f),
    (float2)(-0.618f, 0.000f),
    (float2)(-0.400f, 0.600f),
    (float2)( 0.285f, 0.000f),
    (float2)( 0.285f, 0.010f)
};

int juliaSet(float2 coords, int set)
{
	float2 C = JULIA_SET[set];
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
             int set, int zoom, int movex, int movey)
{
    const int gx = get_global_id(0);
    const int gy = get_global_id(1);

    float h0 = dim0/2.0f;
    float h1 = dim1/2.0f;

    float2 npos = (float2)(1.5f*(gx-h0)/(zoom*h0), (gy-h1)/(zoom*h1));

    npos += (float2)(movex, movey);

    if (gx<dim0 && gy<dim1) {
        int iteration   = juliaSet(npos, set);
        int colorIndex  = iteration%NUM_COLORS;
        write_imagef(out, (int2)(gx,gy), SPECTRUM[NUM_COLORS-1-colorIndex]);
    }
}
