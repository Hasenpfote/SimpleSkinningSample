#version 120

uniform mat4 skinningMatrices[2];
attribute vec4 weights;
attribute vec4 matrixIndices;
attribute vec4 numJoints;
varying vec4 color;

void main(void){
    vec4 index  = matrixIndices;
    vec4 weight = weights;
//	vec4 normal      = vec4( gl_Normal.xyz, 0.0 );
    vec4 position    = vec4( 0.0, 0.0, 0.0, 0.0 );
//	vec4 tempNormal  = vec4( 0.0, 0.0, 0.0, 0.0 );
	
	int num = int(numJoints.x);
    for(int i = 0; i < num; i++){
		// apply influence of joint i
        position = position + weight.x * (skinningMatrices[int(index.x)] * gl_Vertex);
        // transform normal by joint i
//		tempNormal = tempNormal + weight.x * (skinningMatrices[int(index.x)] * normal);
        // shift over the index/weight variables, this moves the index and 
        // weight for the current bone into the .x component of the index 
        // and weight variables
        index  = index.yzwx;
        weight = weight.yzwx;
	}

    gl_Position = gl_ModelViewProjectionMatrix * position;
	color = gl_Color;
}
