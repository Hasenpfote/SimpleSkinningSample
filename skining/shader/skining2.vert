#version 120
//#pragma optimize(off)
#pragma debug(on)

uniform vec4 jointRot[2];
uniform vec4 jointPos[2];
attribute vec4 weights;
attribute vec4 matrixIndices;
attribute vec4 numJoints;
varying vec4 color;

void transform(out vec4 tv, const in vec4 v, const in vec4 q, const in vec4 t){
	vec4 r;
	// q^1 * v * q
	r.xyz = q.w * v.xyz - cross(q.xyz, v.xyz);
	r.w   = dot(q.xyz, v.xyz);
	// q^1 * v * q + t
	tv.xyz = r.w * q.xyz + q.w * r.xyz + cross(r.xyz, q.xyz) + t.xyz;
	tv.w = 1.0;
}

void main(void){
    vec4 index  = matrixIndices;
    vec4 weight = weights;
	vec4 pos = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 temp;

	int num = int(numJoints.x);

    for(int i = 0; i < num; i++){
		transform(temp, gl_Vertex, jointRot[int(index.x)], jointPos[int(index.x)]);
		pos += weight.x * temp;
        // shift over the index/weight variables, this moves the index and 
        // weight for the current bone into the .x component of the index 
        // and weight variables
        index  = index.yzwx;
        weight = weight.yzwx;
    }

	gl_Position = gl_ModelViewProjectionMatrix * pos;
	color = gl_Color;
}
