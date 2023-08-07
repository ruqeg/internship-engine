RWBuffer<uint> g_outputRangeBuffer : register(u1);

static const uint MAX_PARTICLES = 100u;


//0 number;
//1 offset;
//2 expired;
//3 vertexCountPerInstance;
//4 instanceCount;
//5 startVertexLocation;
//6 startInstanceLocation;

[numthreads(1, 1, 1)]
void main()
{
    uint number = g_outputRangeBuffer[0];
    uint offset = g_outputRangeBuffer[1];
    uint expired = g_outputRangeBuffer[2];
    
    g_outputRangeBuffer[0] = number - (int) expired;
    g_outputRangeBuffer[1] = (offset + (int) expired) % MAX_PARTICLES;
    g_outputRangeBuffer[2] = 0u;
    
    number = g_outputRangeBuffer[0];
    offset = g_outputRangeBuffer[1];
    expired = g_outputRangeBuffer[2];
    
    g_outputRangeBuffer[3] = 4u;
    g_outputRangeBuffer[4] = number;
    g_outputRangeBuffer[5] = 0u;
    g_outputRangeBuffer[6] = 0u;
}