#ifndef	SKIN_H
#define SKIN_H

#include "common.h"
//RoH & SM+
struct 	v_model_skinned_0
{
	float4 	P	: POSITION;		// (float,float,float,1) - quantized	// short4
	float3	N	: NORMAL;		// normal				// DWORD
	float3	T	: TANGENT;		// tangent				// DWORD
	float3	B	: BINORMAL;		// binormal				// DWORD
	float2	tc	: TEXCOORD0;	// (u,v)				// short2
};
struct 	v_model_skinned_1   		// 24 bytes
{
	float4 	P	: POSITION;	// (float,float,float,1) - quantized	// short4
	float4	N	: NORMAL;	// (nx,ny,nz,index)			// DWORD
	float3	T	: TANGENT;	// tangent				// DWORD
	float3	B	: BINORMAL;	// binormal				// DWORD
	float2	tc	: TEXCOORD0;	// (u,v)				// short2
};
struct 	v_model_skinned_2		// 28 bytes
{
	float4 	P	: POSITION;	// (float,float,float,1) - quantized	// short4
	float4 	N	: NORMAL;	// (nx,ny,nz,weight)			// DWORD
	float3	T	: TANGENT;	// tangent				// DWORD
	float3	B	: BINORMAL;	// binormal				// DWORD
	float4 	tc	: TEXCOORD0;	// (u,v, w=m-index0, z=m-index1)  	// short4
};

struct 	v_model_skinned_3		// 28 bytes
{
	float4 	P	: POSITION;	// (float,float,float,1) - quantized	// short4
	float4 	N	: NORMAL;	// (nx,ny,nz,weight0)			// DWORD
	float4	T	: TANGENT;	// (tx,ty,tz,weight1)				// DWORD
	float4	B	: BINORMAL;	// (bx,by,bz,m-index2)				// DWORD
	float4 	tc	: TEXCOORD0;	// (u,v, w=m-index0, z=m-index1)  	// short4
};

struct 	v_model_skinned_4		// 28 bytes
{
	float4 	P	: POSITION;	// (float,float,float,1) - quantized	// short4
	float4 	N	: NORMAL;	// (nx,ny,nz,weight0)			// DWORD
	float4	T	: TANGENT;	// (tx,ty,tz,weight1)				// DWORD
	float4	B	: BINORMAL;	// (bx,by,bz,weight2)				// DWORD
	float2 	tc	: TEXCOORD0;	// (u,v)  					// short2
	float4 	ind: TEXCOORD1;	// (x=m-index0, y=m-index1, z=m-index2, w=m-index3)  	// DWORD
};

//////////////////////////////////////////////////////////////////////////////////////////
float4 	u_position	(float4 v)	{ return float4(v.xyz, 1.f);	}	// -12..+12
//////////////////////////////////////////////////////////////////////////////////////////
uniform float4 	sbones_array	[256-10] : register(vs,c11); 

float3 quat_rot(float3 v, float4 q)
{
	return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

float3 	skinning_dir 	(float3 dir, float4 q)
{
	return 	quat_rot(unpack_normal	(dir), q);
}

float4 	skinning_pos 	(float4 pos, float4 m1, float4 m2)
{
	float3 	P	= pos.xyz;
	float3 o = m1 + quat_rot(P, m2);
	return float4(o.xyz, 1);
}

v_model skinning_0	(v_model_skinned_0	v)
{
	//	Swizzle for D3DCOLOUR format
	v.N.xyz		= v.N.zyx;
	v.T.xyz		= v.T.zyx;
	v.B.xyz		= v.B.zyx;

	// skinning
	v_model 	o;
	o.P 		= u_position	(v.P);
	o.N 		= unpack_normal(v.N);
	o.T 		= unpack_normal(v.T);
	o.B 		= unpack_normal(v.B);
	o.tc 		= v.tc;
	return o;
}

v_model skinning_1 	(v_model_skinned_1	v)
{
	//	Swizzle for D3DCOLOUR format
	v.N.xyz		= v.N.zyx;
	v.T.xyz		= v.T.zyx;
	v.B.xyz		= v.B.zyx;
	
	// matrices
	int 	mid 	= (int)round(v.N.w * 170);
	float4  rot 	= sbones_array[mid+0];
	float4  pos 	= sbones_array[mid+1];

	// skinning
	v_model 	o;
	o.P 		= skinning_pos(v.P, pos, rot );
	o.N 		= skinning_dir(v.N, rot );
	o.T 		= skinning_dir(v.T, rot );
	o.B 		= skinning_dir(v.B, rot );
	o.tc 		= v.tc;
	return o;
}

float4 lerp_4(float3 vec1, float3 vec2, float3 vec3, float3 vec4, float3 weights)
{
	float3 ret = float3(0.f, 0.f, 0.f);
	ret += vec1*weights[0];
	ret += vec2*weights[1];
	ret += vec3*weights[2];
	ret += vec4*(1-weights[0]-weights[1]-weights[2]);
	return float4(ret.xyz, 1.f);
}

v_model skinning_2 	(v_model_skinned_2	v)
{
	//	Swizzle for D3DCOLOUR format
	v.N.xyz		= v.N.zyx;
	v.T.xyz		= v.T.zyx;
	v.B.xyz		= v.B.zyx;

	// matrices
	int 	id_0 	= (v.tc.z);
	float4  rot_0 	= sbones_array[id_0+0];
	float4  pos_0 	= sbones_array[id_0+1];

	int 	id_1 	= (v.tc.w);
	float4 rot_1 	= sbones_array[id_1+0];
	float4 pos_1 	= sbones_array[id_1+1];

	// lerp
	float 	wLerp 	= v.N.w;

	// skinning
	v_model 	o;
	o.P 		= lerp(skinning_pos(v.P, pos_0, rot_0 ), skinning_pos(v.P, pos_1, rot_1 ), wLerp);
	o.N 		= lerp(skinning_dir(v.N, rot_0 ), skinning_dir(v.N, rot_1 ), wLerp);
	o.T 		= lerp(skinning_dir(v.T, rot_0 ), skinning_dir(v.T, rot_1 ), wLerp);
	o.B 		= lerp(skinning_dir(v.B, rot_0 ), skinning_dir(v.B, rot_1 ), wLerp);
	o.tc 		= v.tc;
	return o;

}

v_model skinning_3 	(v_model_skinned_3	v)
{
	//	Swizzle for D3DCOLOUR format
	v.N.xyz		= v.N.zyx;
	v.T.xyz		= v.T.zyx;
	v.B.xyz		= v.B.zyx;

	// matrices
	int 	id_0 	= (int)round(v.tc.z * 0.666666666);
	float4  rot_0 	= sbones_array[id_0+0];
	float4  pos_0 	= sbones_array[id_0+1];

	int 	id_1 	= (int)round(v.tc.w * 0.666666666);
	float4 rot_1 	= sbones_array[id_1+0];
	float4 pos_1 	= sbones_array[id_1+1];
	
	int 	id_2 	= (int)round(v.B.w * 0.666666666);
	float4 rot_2 	= sbones_array[id_2+0];
	float4 pos_2 	= sbones_array[id_2+1];
	
	// lerp
	float3 	w	 	= float3(v.N.w, v.T.w, 0.f);

	// skinning
	v_model 	o;
	
	o.P 		= lerp_4(skinning_pos(v.P, pos_0, rot_0 ), 	skinning_pos(v.P, pos_1, rot_1 ), 	skinning_pos(v.P, pos_2, rot_2 ), 	float3(0.f, 0.f, 0.f), w);
	o.N 		= lerp_4(skinning_dir(v.N, rot_0 ), 		skinning_dir(v.N, rot_1 ), 			skinning_dir(v.N, rot_2 ), 			float3(0.f, 0.f, 0.f), w);
	o.T 		= lerp_4(skinning_dir(v.T, rot_0 ), 		skinning_dir(v.T, rot_1 ), 			skinning_dir(v.T, rot_2 ), 			float3(0.f, 0.f, 0.f), w);
	o.B 		= lerp_4(skinning_dir(v.B, rot_0 ), 		skinning_dir(v.B, rot_1 ), 			skinning_dir(v.B, rot_2 ), 			float3(0.f, 0.f, 0.f), w);
	o.tc 		= v.tc;

#ifdef SKIN_COLOR
	o.rgb_tint	= float3	(2,0,0)	;
	if (id_0==id_1)	o.rgb_tint	= float3(1,2,0);
#endif

	return o;
}
v_model skinning_4 	(v_model_skinned_4	v)
{
	//	Swizzle for D3DCOLOUR format
	v.N.xyz		= v.N.zyx;
	v.T.xyz		= v.T.zyx;
	v.B.xyz		= v.B.zyx;
	v.ind.xyz	= v.ind.zyx;

	// matrices
	int 	id_0 	= (int)round(v.ind.x * 170 + 0.3);
	float4  rot_0 	= sbones_array[id_0+0];
	float4  pos_0 	= sbones_array[id_0+1];

	int 	id_1 	= (int)round(v.ind.y * 170 + 0.3);
	float4 rot_1 	= sbones_array[id_1+0];
	float4 pos_1 	= sbones_array[id_1+1];
	
	int 	id_2 	= (int)round(v.ind.z * 170 + 0.3);
	float4 rot_2 	= sbones_array[id_2+0];
	float4 pos_2 	= sbones_array[id_2+1];
	
	int 	id_3 	= (int)round(v.ind.w * 170 + 0.3);
	float4 rot_3 	= sbones_array[id_3+0];
	float4 pos_3 	= sbones_array[id_3+1];
	
	// lerp
	float3 	w	 	= float3(v.N.w, v.T.w, v.B.w);

	// skinning
	v_model 	o;
	
	o.P 		= lerp_4(skinning_pos(v.P, pos_0, rot_0 ), 	skinning_pos(v.P, pos_1, rot_1 ), 	skinning_pos(v.P, pos_2, rot_2 ), 	skinning_pos(v.P, pos_3, rot_3 ), 	w);
	o.N 		= lerp_4(skinning_dir(v.N, rot_0 ), 		skinning_dir(v.N, rot_1 ), 			skinning_dir(v.N, rot_2 ), 			skinning_dir(v.N, rot_3 ), 			w);
	o.T 		= lerp_4(skinning_dir(v.T, rot_0 ), 		skinning_dir(v.T, rot_1 ), 			skinning_dir(v.T, rot_2 ), 			skinning_dir(v.T, rot_3 ), 			w);
	o.B 		= lerp_4(skinning_dir(v.B, rot_0 ), 		skinning_dir(v.B, rot_1 ), 			skinning_dir(v.B, rot_2 ), 			skinning_dir(v.B, rot_3 ), 			w);
	o.tc 		= v.tc;
	return o;

}

#endif