
constant float2 setK[] = {-0.618f,0.0f};

bool isInsideJulia(float2 coords,int set)
{
	float2 C = setK[set];
    float2 temp;
    // Transform params into (-1,1) range // 1.5 is scale factor
    // Julia Set
	float2 transformed = 2*(coords-(float2)0.5f);

    for(unsigned int k=0; k<1000; k++)
    {
        temp.s0 = (transformed.s0*transformed.s0-transformed.s1*transformed.s1) + C.s0;
        temp.s1 = 2*transformed.s0*transformed.s1 + C.s1;
		transformed = temp;
        if( dot(transformed,transformed) > 500 )
            return false;
    }
    return true;
}

__kernel void fractal( __write_only image2d_t out,
                    __global const uchar4* in,
                    int dim0, int dim1, int dim2)
{
	const int iterations = 100;
	float4 orange = (float4)(1.0f,0.8f,0,1.0f);
	float4 white = (float4)(1.0f,0.9f,0.9f,1.0f);
    // dims 0 - width, 1 - height, 2 - channels
    int gx = get_global_id(0);
    int gy = get_global_id(1);
    int2 pos = (int2)(gx,gy);
    if (gx>=dim0 || gy>=dim1) {
        return;
    }
	float2 normalizedPos = (float2)(pos.s0/(float)dim0,pos.s1/(float)dim1);
    float4 opxl = isInsideJulia(normalizedPos,0) ? orange : white;
    write_imagef (out,pos,opxl);
}
