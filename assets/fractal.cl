#include <colormaps.h>

constant unsigned MAX_ITERS = 256;
constant unsigned NUM_COLORS = 257; // 257 colors are there in spectrum map defined in colormaps.h

constant float2 JULIA_SET[4] = {
    (float2)(-0.618f, 0.000f),
    (float2)(-0.400f, 0.600f),
    (float2)( 0.285f, 0.000f),
    (float2)( 0.285f, 0.010f)
};

int juliaSet(float2 coords, int set)
{
	float2 C = JULIA_SET[set];
    float2 temp;

    for(unsigned i=0; i<MAX_ITERS; i++)
    {
        temp.s0 = (coords.s0 * coords.s0 - coords.s1 * coords.s1) + C.s0;
        temp.s1 = 2 * coords.s0 * coords.s1 + C.s1;
		coords  = temp;

        if(dot(coords, coords) > 2) {
            return i;
        }
    }

    return MAX_ITERS;
}

kernel
void fractal(write_only image2d_t out, int dim0, int dim1, int set)
{
    const int gx = get_global_id(0);
    const int gy = get_global_id(1);

    float h0 = dim0/2.0f;
    float h1 = dim1/2.0f;

    float2 npos = (float2)((h0-gx)/h0, (h1-gy)/gy);
    if (gx<dim0 && gy<dim1) {
        int iteration   = juliaSet(2.0f*npos, set);
        float stepValue = (float)iteration/(float)MAX_ITERS;
        int colorIndex  = stepValue * NUM_COLORS;
        int nextColor   = (colorIndex+1)%NUM_COLORS;
        float4 color    = smoothstep(SPECTRUM[colorIndex], SPECTRUM[nextColor], stepValue);
        write_imagef(out, (int2)(gx,gy), color);
    }
}
