// Following is a horrible pseudo-random number generator based upon Perlin noise
// taken from here: http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
// it generates a random number in the interval [-1 ... 1]
float rand(int x)
{
    x = (x << 13) ^ x;
    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

kernel
void random(global float* positions,
        int const dimx, int const dimy,
        uint const seed)
{
    int id = get_global_id(0);
    positions[2*id+0] = rand(seed * id);
    positions[2*id+1] = rand(seed ^ id);
}
