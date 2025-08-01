//get a scalar random value from a 3d value
float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719)){
    //make value smaller to avoid artefacts
    float32_t3 smallValue = sin(value);
    //get scalar value from 3d vector
    float32_t random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking the factional part
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t rand2dTo1d(float32_t2 value, float32_t2 dotDir = float32_t2(12.9898, 78.233)){
    float32_t2 smallValue = sin(value);
    float32_t random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t rand1dTo1d(float32_t3 value, float32_t mutator = 0.546){
    float32_t3 st = value + mutator;
    float32_t random = frac(sin(dot(st, float3(12.989, 78.233, 37.719))) * 143758.5453);
    return random;
}

//to 2d functions

float32_t2 rand3dTo2d(float32_t3 value){
    return float32_t2(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155))
    );
}

float32_t2 rand2dTo2d(float32_t2 value){
    return float32_t2(
        rand2dTo1d(value, float32_t2(12.989, 78.233)),
        rand2dTo1d(value, float32_t2(39.346, 11.135))
    );
}

float32_t2 rand1dTo2d(float32_t value){
    return float32_t2(
        rand2dTo1d(value, 3.9812),
        rand2dTo1d(value, 7.1536)
    );
}

//to 3d functions

float32_t3 rand3dTo3d(float32_t3 value){
    return float32_t3(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float32_t3(73.156, 52.235, 09.151))
    );
}

float32_t3 rand2dTo3d(float32_t2 value){
    return float32_t3(
        rand2dTo1d(value, float32_t2(12.989, 78.233)),
        rand2dTo1d(value, float32_t2(39.346, 11.135)),
        rand2dTo1d(value, float32_t2(73.156, 52.235))
    );
}

float32_t3 rand1dTo3d(float32_t value){
    return float32_t3(
        rand1dTo1d(value, 3.9812),
        rand1dTo1d(value, 7.1536),
        rand1dTo1d(value, 5.7241)
    );
}
class RandomGenerator{
    float32_t3 seed;
    float32_t3 Generate3d(){
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t Generate1d(){
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};