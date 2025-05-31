
#if (defined(_vector_util_inc))
	#endinput
#endif
#define _vector_util_inc

#include <float>
#include <vector>

#pragma semicolon 1;

/** 零向量. */
stock const Float:V3_Zero[3];

/**
 * 8个水平方向的网格偏移向量(Z=0),长度为1或√2(对角线方向).
 * 顺序:从西南方向开始,逆时针旋转.
 * 用途:2D平面遍历,寻路算法的邻居方向检测...
*/
stock const Float:V3_Grid8Offsets[][] =
{
	{ -1.0, -1.0,  0.0 },
	{  0.0, -1.0,  0.0 },
	{  1.0, -1.0,  0.0 },
	{  1.0,  0.0,  0.0 },
	{  1.0,  1.0,  0.0 },
	{  0.0,  1.0,  0.0 },
	{ -1.0,  1.0,  0.0 },
	{ -1.0,  0.0,  0.0 },
};

/**
 * 26个3D方向的网格偏移向量,长度为1,√2(边相邻)或√3(角相邻).
 * 顺序:从下至上,从西南方向开始,逆时针旋转.
 * 用途:3D空间邻居检测,体素遍历...
*/
stock const Float:V3_Grid26Offsets[][] =
{
	{  0.0,  0.0, -1.0 },

	{ -1.0, -1.0, -1.0 },
	{  0.0, -1.0, -1.0 },
	{  1.0, -1.0, -1.0 },
	{  1.0,  0.0, -1.0 },
	{  1.0,  1.0, -1.0 },
	{  0.0,  1.0, -1.0 },
	{ -1.0,  1.0, -1.0 },
	{ -1.0,  0.0, -1.0 },

	{ -1.0, -1.0,  0.0 },
	{  0.0, -1.0,  0.0 },
	{  1.0, -1.0,  0.0 },
	{  1.0,  0.0,  0.0 },
	{  1.0,  1.0,  0.0 },
	{  0.0,  1.0,  0.0 },
	{ -1.0, 1.0,  0.0 },
	{ -1.0, 0.0,  0.0 },

	{ -1.0, -1.0,  1.0 },
	{  0.0, -1.0,  1.0 },
	{  1.0, -1.0,  1.0 },
	{  1.0,  0.0,  1.0 },
	{  1.0,  1.0,  1.0 },
	{  0.0,  1.0,  1.0 },
	{ -1.0,  1.0,  1.0 },
	{ -1.0,  0.0,  1.0 },

	{  0.0,  0.0,  1.0 },
};

/** 定义角度转换为向量时的基准方向(基于世界坐标系). */
enum AngleDirection
{
	/** 将角度转换为前向向量(X轴正方向) */
	ANGLE_FORWARD,
	/** 将角度转换为左向向量(Y轴正方向) */
	ANGLE_LEFT,
	/** 将角度转换为右向向量(Y轴反方向) */
	ANGLE_RIGHT,
	/** 将角度转换为上向向量(Z轴正方向) */
	ANGLE_UP
}

/** 检查两向量是否近似时,所允许的误差范围. */
const Float:VECEQ_TOLERANCE = 0.000005;

/** 用于储存复杂表达式的计算结果,避免该表达式被多次执行. */
stock Float:__tempScalar;

/** 函数宏:两个3D向量是否相等. */
#define V3_Equal(%0,%1)			(%0[0] == %1[0] && %0[1] == %1[1] && %0[2] == %1[2])
/** 函数宏:两个3D向量是否近似. */
#define V3_NearlyEqual(%0,%1)		(floatabs(%1[0] - %0[0]) <= VECEQ_TOLERANCE && floatabs(%1[1] - %0[1]) <= VECEQ_TOLERANCE && floatabs(%1[2] - %0[2]) <= VECEQ_TOLERANCE)
/** 函数宏:计算3D向量长度的平方. */
#define V3_LengthSquare(%0)		(%0[0] * %0[0] + %0[1] * %0[1] + %0[2] * %0[2])
/** 函数宏:计算3D向量长度. */
#define V3_Length(%0)			floatsqroot(V3_LengthSquare(%0))
/** 函数宏:计算两个3D向量距离的平方. */
#define V3_DistanceSquare(%0,%1)	((%0[0]-%1[0])*(%0[0]-%1[0]) + (%0[1]-%1[1])*(%0[1]-%1[1]) + (%0[2]-%1[2])*(%0[2]-%1[2]))
/** 函数宏:计算两个3D向量距离. */
#define V3_Distance(%0,%1)		floatsqroot(V3_DistanceSquare(%0,%1))
/** 函数宏:设置3D向量的3个分量. */
#define V3_Set(%0,%1,%2,%3)		(%0[0] = (%1), %0[1] = (%2), %0[2] = (%3))
/** 函数宏:输出向量的3D副本. */
#define V3_Copy(%0,%1)			(%1[0] = %0[0], %1[1] = %0[1], %1[2] = %0[2])
/** 函数宏:输出3D向量的相反向量. */
#define V3_Neg(%0,%1)			(%1[0] = -%0[0], %1[1] = -%0[1], %1[2] = -%0[2])
/** 函数宏:返回两个3D向量点乘的结果. */
#define V3_Dot(%0,%1)			((%0[0] * %1[0]) + (%0[1] * %1[1]) + (%0[2] * %1[2]))
/** 函数宏:输出两个3D向量叉乘的结果.注意,参数3不能引用参数1或参数2. */
#define V3_Cross(%0,%1,%2)		(%2[0] = %0[1]*%1[2] - %0[2]*%1[1], %2[1] = %0[2]*%1[0] - %0[0]*%1[2], %2[2] = %0[0]*%1[1] - %0[1]*%1[0])
/** 函数宏:输出3D向量在平面3D法向量上的反射向量. */
#define V3_Reflect(%0,%1,%2)		V3_SubScaled(%0, %1, 2.0 * V3_Dot(%0, %1), %2)
/** 函数宏:输出两个3D向量相加的结果. */
#define V3_Add(%0,%1,%2)		(%2[0] = %0[0] + %1[0], %2[1] = %0[1] + %1[1], %2[2] = %0[2] + %1[2])
/** 函数宏:输出两个3D向量相减的结果. */
#define V3_Sub(%0,%1,%2)		(%2[0] = %0[0] - %1[0], %2[1] = %0[1] - %1[1], %2[2] = %0[2] - %1[2])
/** 函数宏:输出3D向量乘以标量的结果. */
#define V3_MulScalar(%0,%1,%2)		(__tempScalar = (%1), %2[0] = %0[0] * __tempScalar, %2[1] = %0[1] * __tempScalar, %2[2] = %0[2] * __tempScalar)
/** 函数宏:输出3D向量1+3D向量2*标量的结果. */
#define V3_AddScaled(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] + %1[0] * __tempScalar, %3[1] = %0[1] + %1[1] * __tempScalar, %3[2] = %0[2] + %1[2] * __tempScalar)
/** 函数宏:输出3D向量1-3D向量2*标量的结果. */
#define V3_SubScaled(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] - %1[0] * __tempScalar, %3[1] = %0[1] - %1[1] * __tempScalar, %3[2] = %0[2] - %1[2] * __tempScalar)

#if (debug == 2)
/** 输出3D向量除以标量的结果. */
stock V3_DivScalar(const Float:vec[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = vec[0] / scalar, out[1] = vec[1] / scalar, out[2] = vec[2] / scalar;
}
/** 输出3D向量1+3D向量2/标量的结果. */
stock V3_AddDivided(const Float:in1[], const Float:in2[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = in1[0] + in2[0] / scalar, out[1] = in1[1] + in2[1] / scalar, out[2] = in1[2] + in2[2] / scalar;
}
/** 输出3D向量1-3D向量2/标量的结果. */
stock V3_SubDivided(const Float:in1[], const Float:in2[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = in1[0] - in2[0] / scalar, out[1] = in1[1] - in2[1] / scalar, out[2] = in1[2] - in2[2] / scalar;
}
/** 输出3D向量的标量化结果. */
stock V3_Normalize(const Float:in[], Float:out[])
{
	__tempScalar = 1.0 / V3_Length(in);
	if (__tempScalar == 0.0) abort(AMX_ERR_DIVIDE, "zero vector cannot be normalized.");
	out[0] = in[0] * __tempScalar, out[1] = in[1] * __tempScalar, out[2] = in[2] * __tempScalar;
}
#else
/** 函数宏:输出3D向量除以标量的结果. */
#define V3_DivScalar(%0,%1,%2)		(__tempScalar = (%1), %2[0] = %0[0] / __tempScalar, %2[1] = %0[1] / __tempScalar, %2[2] = %0[2] / __tempScalar)
/** 函数宏:输出3D向量1+3D向量2/标量的结果. */
#define V3_AddDivided(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] + %1[0] / __tempScalar, %3[1] = %0[1] + %1[1] / __tempScalar, %3[2] = %0[2] + %1[2] / __tempScalar)
/** 函数宏:输出3D向量1-3D向量2/标量的结果. */
#define V3_SubDivided(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] - %1[0] / __tempScalar, %3[1] = %0[1] - %1[1] / __tempScalar, %3[2] = %0[2] - %1[2] / __tempScalar)
/** 函数宏:输出3D向量的标量化结果. */
#define V3_Normalize(%0,%1)		V3_MulScalar(%0, 1.0 / V3_Length(%0), %1)
#endif

/**
 * 将欧拉角转换为指定方向的单位向量(前向,左向,右向或上向).
 *
 * @note	金源引擎中,模型与第一人称的角度有差异.
 *		模型角度的pitch加法为抬头,减法为低头.
 *		第一人称的角度的pitch减法为抬头,加法为低头.
 *		这里要求填写第一人称角度.
 *
 * @param angles	需要被转换的角度,单位为度,顺序:[pitch,yaw,roll]
 * @param dirType	转换的目标方向,详情查阅AngleDirection枚举的成员
 * @param outVec	输出转换所得向量(长度为1)
 *
 * @noreturn
*/
stock V3_AngleToDirection(const Float:angles[], AngleDirection:dirType, Float:outVec[])
{
	new Float:cp = floatcos(M_PI * angles[0] / 180.0), Float:sp = floatsin(M_PI * angles[0] / 180.0);
	new Float:cy = floatcos(M_PI * angles[1] / 180.0), Float:sy = floatsin(M_PI * angles[1] / 180.0);
	switch (dirType)
	{
		case ANGLE_UP:
		{
			new Float:sr = floatsin(M_PI * angles[2] / 180.0), Float:cr = floatcos(M_PI * angles[2] / 180.0);
			outVec[0] = cr * sp * cy + -sr * -sy;
			outVec[1] = cr * sp * sy + -sr * cy;
			outVec[2] = cr * cp;
		}
		case ANGLE_RIGHT:
		{
			new Float:sr = -floatsin(M_PI * angles[2] / 180.0), Float:cr = -floatcos(M_PI * angles[2] / 180.0);
			outVec[0] = sr * sp * cy + cr * -sy;
			outVec[1] = sr * sp * sy + cr * cy;
			outVec[2] = sr * cp;
		}
		case ANGLE_LEFT:
		{
			new Float:sr = floatsin(M_PI * angles[2] / 180.0), Float:cr = floatcos(M_PI * angles[2] / 180.0);
			outVec[0] = sr * sp * cy + cr * -sy;
			outVec[1] = sr * sp * sy + cr * cy;
			outVec[2] = sr * cp;
		}
		default:
		{
			outVec[0] = cp * cy;
			outVec[1] = cp * sy;
			outVec[2] = -sp;
		}
	}
}

/**
 * 将欧拉角转换为前向,右向/左向和上向的单位向量.
 *
 * @note	金源引擎中,模型与第一人称的角度有差异.
 *		模型角度的pitch加法为抬头,减法为低头.
 *		第一人称的角度的pitch减法为抬头,加法为低头.
 *		这里要求填写第一人称角度.
 *
 * @param angles	需要被转换的角度,顺序:[pitch,yaw,roll]
 * @param useRight	false=Y轴正方向为左,true=Y轴正方向为右
 * @param basisX	输出X轴在世界参考系中的方向,代表前方
 * @param basisY	输出Y轴在世界参考系中的方向,代表左或右
 * @param basisZ	输出Z轴在世界参考系中的方向,代表上方
 *
 * @noreturn
*/
stock V3_AngleToBasisVectors(const Float:angles[], bool:useRight, Float:basisX[], Float:basisY[], Float:basisZ[])
{
	new Float:cp = floatcos(M_PI * angles[0] / 180.0), Float:sp = floatsin(M_PI * angles[0] / 180.0);
	new Float:cy = floatcos(M_PI * angles[1] / 180.0), Float:sy = floatsin(M_PI * angles[1] / 180.0);
	new Float:sr = floatsin(M_PI * angles[2] / 180.0), Float:cr = floatcos(M_PI * angles[2] / 180.0);
	
	basisX[0] = cp * cy;
	basisX[1] = cp * sy;
	basisX[2] = -sp;

	basisY[0] = sr * sp * cy + cr * -sy;
	basisY[1] = sr * sp * sy + cr * cy;
	basisY[2] = sr * cp;
	if (useRight) V3_Neg(basisY, basisY);

	basisZ[0] = cr * sp * cy + -sr * -sy;
	basisZ[1] = cr * sp * sy + -sr * cy;
	basisZ[2] = cr * cp;
}

/**
 * 将局部坐标系的向量转换到世界坐标系.
 * 代表局部坐标系XYZ轴的向量,可从V3_AngleToDirection或V3_AngleToBasisVectors函数获取.
 *
 * @note	金源引擎的世界坐标系:X轴=前,Y轴=左,Z轴=上.
 *
 * @param basisX	局部坐标系X轴的世界方向
 * @param basisY	局部坐标系Y轴的世界方向
 * @param basisZ	局部坐标系Z轴的世界方向
 * @param localVec	待转换的局部向量
 * @param worldVec	输出的世界向量,注意:禁止与其它参数引用同一向量
 *
 * @noreturn
*/
stock V3_LocalToWorld(const Float:basisX[], const Float:basisY[], const Float:basisZ[], const Float:localVec[], Float:worldVec[])
{
	for (new axis = 3; 0 < axis--;) worldVec[axis] = basisX[axis] * localVec[0] + basisY[axis] * localVec[1] + basisZ[axis] * localVec[2];
}

/**
 * 将世界坐标系的向量转换到局部坐标系.
 * 代表局部坐标系XYZ轴的向量,可从V3_AngleToDirection或V3_AngleToBasisVectors函数获取.
 *
 * @note	金源引擎的世界坐标系:X轴=前,Y轴=左,Z轴=上.
 *
 * @param basisX	局部坐标系X轴的世界方向
 * @param basisY	局部坐标系Y轴的世界方向
 * @param basisZ	局部坐标系Z轴的世界方向
 * @param worldVec	待转换的世界向量
 * @param localVec	输出的局部向量,注意:禁止与其它参数引用同一向量
 *
 * @noreturn
*/
stock V3_WorldToLocal(const Float:basisX[], const Float:basisY[], const Float:basisZ[], const Float:worldVec[], Float:localVec[])
{
	localVec[0] = V3_Dot(worldVec, basisX);
	localVec[1] = V3_Dot(worldVec, basisY);
	localVec[2] = V3_Dot(worldVec, basisZ);
}

/**
 * 将向量转换为欧拉角.
 *
 * @note	金源引擎中,模型与第一人称的角度有差异.
 *		模型角度的pitch加法为抬头,减法为低头.
 *		第一人称的角度的pitch减法为抬头,加法为低头.
 *		这里输出的是模型角度.对angles.x取反,可使其变为第一人称角度.
 *
 * @param vector	需要被转换的向量
 * @param angles	输出转换所得角度,顺序:[pitch,yaw,roll]
 *
 * @noreturn
*/
stock V3_VectorToAngle(const Float:vector[], Float:angles[])
{
	#if (debug == 2)
	if (V3_Equal(vector, V3_Zero)) abort(AMX_ERR_PARAMS, "zero vector cannot be converted to angle.");
	#endif
	new Float:pitch = floatatan2(vector[2], V2_Length(vector), degrees);
	new Float:yaw = floatatan2(vector[1], vector[0], degrees);
	angles[0] = pitch < 0.0 ? pitch + 360.0 : pitch;
	angles[1] = yaw < 0.0 ? yaw + 360.0 : yaw;
	angles[2] = 0.0;
}

/** 函数宏:两个2D向量是否相等. */
#define V2_Equal(%0,%1)			(%0[0] == %1[0] && %0[1] == %1[1])
/** 函数宏:两个2D向量是否近似. */
#define V2_NearlyEqual(%0,%1)		(floatabs(%1[0] - %0[0]) <= VECEQ_TOLERANCE && floatabs(%1[1] - %0[1]) <= VECEQ_TOLERANCE)
/** 函数宏:计算2D向量长度的平方. */
#define V2_LengthSquare(%0)		(%0[0] * %0[0] + %0[1] * %0[1])
/** 函数宏:计算2D向量长度. */
#define V2_Length(%0)			floatsqroot(V2_LengthSquare(%0))
/** 函数宏:计算两个2D向量距离的平方. */
#define V2_DistanceSquare(%0,%1)	((%0[0]-%1[0])*(%0[0]-%1[0]) + (%0[1]-%1[1])*(%0[1]-%1[1]))
/** 函数宏:计算两个2D向量距离. */
#define V2_Distance(%0,%1)		floatsqroot(V2_DistanceSquare(%0,%1))
/** 函数宏:设置2D向量的2个分量. */
#define V2_Set(%0,%1,%2)		(%0[0] = (%1), %0[1] = (%2))
/** 函数宏:输出向量的2D副本. */
#define V2_Copy(%0,%1)			(%1[0] = %0[0], %1[1] = %0[1])
/** 函数宏:输出2D向量的相反向量. */
#define V2_Neg(%0,%1)			(%1[0] = -%0[0], %1[1] = -%0[1])
/** 函数宏:返回两个2D向量点乘的结果. */
#define V2_Dot(%0,%1)			((%0[0] * %1[0]) + (%0[1] * %1[1]))
/** 函数宏:返回两个2D向量叉乘的结果. */
#define V2_Cross(%0,%1,%2)		(%0[0] * %1[1] - %0[1] * %1[0])
/** 函数宏:输出2D向量在平面2D法向量上的反射向量. */
#define V2_Reflect(%0,%1,%2)		V2_SubScaled(%0, %1, 2.0 * V2_Dot(%0, %1), %2)
/** 函数宏:输出两个2D向量相加的结果. */
#define V2_Add(%0,%1,%2)		(%2[0] = %0[0] + %1[0], %2[1] = %0[1] + %1[1])
/** 函数宏:输出两个2D向量相减的结果. */
#define V2_Sub(%0,%1,%2)		(%2[0] = %0[0] - %1[0], %2[1] = %0[1] - %1[1])
/** 函数宏:输出2D向量乘以标量的结果. */
#define V2_MulScalar(%0,%1,%2)		(__tempScalar = (%1), %2[0] = %0[0] * __tempScalar, %2[1] = %0[1] * __tempScalar)
/** 函数宏:输出2D向量1+2D向量2*标量的结果. */
#define V2_AddScaled(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] + %1[0] * __tempScalar, %3[1] = %0[1] + %1[1] * __tempScalar)
/** 函数宏:输出2D向量1-2D向量2*标量的结果. */
#define V2_SubScaled(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] - %1[0] * __tempScalar, %3[1] = %0[1] - %1[1] * __tempScalar)

#if (debug == 2)
/** 输出2D向量除以标量的结果. */
stock V2_DivScalar(const Float:vec[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = vec[0] / scalar, out[1] = vec[1] / scalar;
}
/** 输出2D向量1+2D向量2/标量的结果. */
stock V2_AddDivided(const Float:in1[], const Float:in2[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = in1[0] + in2[0] / scalar, out[1] = in1[1] + in2[1] / scalar;
}
/** 输出2D向量1-2D向量2/标量的结果. */
stock V2_SubDivided(const Float:in1[], const Float:in2[], Float:scalar, Float:out[])
{
	if (scalar == 0.0) abort(AMX_ERR_DIVIDE, "zero cannot be used as a divisor.");
	out[0] = in1[0] - in2[0] / scalar, out[1] = in1[1] - in2[1] / scalar;
}
/** 输出2D向量的标量化结果. */
stock V2_Normalize(const Float:in[], Float:out[])
{
	__tempScalar = 1.0 / V2_Length(in);
	if (__tempScalar == 0.0) abort(AMX_ERR_DIVIDE, "zero vector cannot be normalized.");
	out[0] = in[0] * __tempScalar, out[1] = in[1] * __tempScalar;
}
#else
/** 函数宏:输出2D向量除以标量的结果. */
#define V2_DivScalar(%0,%1,%2)		(__tempScalar = (%1), %2[0] = %0[0] / __tempScalar, %2[1] = %0[1] / __tempScalar)
/** 函数宏:输出2D向量1+2D向量2/标量的结果. */
#define V2_AddDivided(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] + %1[0] / __tempScalar, %3[1] = %0[1] + %1[1] / __tempScalar)
/** 函数宏:输出2D向量1-2D向量2/标量的结果. */
#define V2_SubDivided(%0,%1,%2,%3)	(__tempScalar = (%2), %3[0] = %0[0] - %1[0] / __tempScalar, %3[1] = %0[1] - %1[1] / __tempScalar)
/** 函数宏:输出2D向量的标量化结果. */
#define V2_Normalize(%0,%1)		V2_MulScalar(%0, 1.0 / V2_Length(%0), %1)
#endif

