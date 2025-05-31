/**
 * 0不是正数也不是负数.但浮点数中存在-0.0和0.0两个数值.值相等,可以区分正负.
 * 在amxmodx内部,整数与浮点数的转换规则如下:
 * 整数转浮点数:	浮点数=*((float*)&整数)
 * 浮点数转整数:	整数=*((cell*)&浮点数)
 * 其中cell是int32,amxmodx目前没有让amxxpawn支持其他值类型,
 * 而是使用类标签(Tag),将cell转换为其它值类型.
 * 常量表达式在编译时转换,其它在运行时转换.
*/

#if (defined(_float_util_inc))
	#endinput
#endif
#define _float_util_inc

#include <float>

#pragma semicolon 1;

/**
 * 最常见的无效数字.
 * 运行时对负数开方,或执行0.0 / 0.0,也就是floatdiv(0.0, 0.0)函数,可获得该数值.
 * 其他写法:
 * Float:0b_11111111_11000000_00000000_00000000	// 右边24位只要非0,皆非数字
 * Float:-4194304
 * Float:0xFFC00000
*/
const Float:FLOAT_NAN = Float:0b_11111111_11000000_00000000_00000000;

/**
 * 负无穷.比FLOAT_MAX_NEGATIVE小.
 * 运行时执行-1.0 / 0.0,也就是floatdiv(-1.0, 0.0)函数,可获得该数值.
 * 其他写法:
 * Float:0b_11111111_10000000_00000000_00000000
 * Float:-8388608
 * Float:0xFF800000
*/
const Float:FLOAT_NEGATIVE_INFINITY = Float:0b_11111111_10000000_00000000_00000000;

/**
 * 正规最大值的负数(最小有限负数).比FLOAT_NEGATIVE_INFINITY大.
 * 其他写法:
 * -3.402823466385288598117042e38
 * -3.4028234663e38
 * Float:0b_11111111_01111111_11111111_11111111
 * Float:-8388609
 * Float:0xFF7FFFFF
*/
const Float:FLOAT_MAX_NEGATIVE = -340282346638528859811704000000000000000.0;

/**
 * 正数,次正规数(denormal number)的最小值.即大于0的最小可表示正数.常用于判断浮点数是否约为0.0
 * 其他写法:
 * 1.401298464324817070923730e-45
 * 1.4012984643e-45
 * Float:0b_00000000_00000000_00000000_00000001
 * Float:1
 * Float:0x00000001
*/
const Float:FLOAT_MIN_DENORM = 0.000000000000000000000000000000000000000000001401298464324817070923730;

/**
 * 次正规的最大值.比FLOAT_MIN小.
 * 其他写法:
 * 1.175494210692441075487029e-38
 * 1.1754942106e-38
 * Float:0b_00000000_01111111_11111111_11111111
 * Float:8388607
 * Float:0x007FFFFF
*/
const Float:FLOAT_MAX_DENORM = 0.00000000000000000000000000000000000001175494210692441075487029;

/**
 * 正数,正规的最小值.比FLOAT_MAX_DENORM大.
 * 其他写法:
 * 1.175494350822287507968737e-38
 * 1.1754943508e-38
 * Float:0b_00000000_10000000_00000000_00000000
 * Float:8388608
 * Float:0x00800000
*/
const Float:FLOAT_MIN = 0.0000000000000000000000000000000000000117549435;

/**
 * 机器Epsilon,1.0与比1.0大的下一个可表示数值的差.
 * 运行时执行floatpower(2.0, -23.0)函数,可获得该数值.
 * 其他写法:
 * Float:0b_00110100_00000000_00000000_00000000
 * Float:872415232
 * Float:0x34000000
*/
const Float:FLOAT_MACHINE_EPSILON = 0.00000011920928955078125;

/**
 * 正规的最大值(最大有限正数).比FLOAT_INFINITY小.
 * 其他写法:
 * 3.402823466385288598117042e38
 * 3.4028234663e38
 * Float:0b_01111111_01111111_11111111_11111111
 * Float:2139095039
 * Float:0x7F7FFFFF
*/
const Float:FLOAT_MAX = 340282346638528800000000000000000000000.0;

/**
 * 正无穷.比FLOAT_MAX大.
 * 运行时执行1.0 / 0.0,也就是floatdiv(1.0, 0.0)函数,可获得该数值.
 * 其他写法:
 * Float:0b_01111111_10000000_00000000_00000000
 * Float:2139095040
 * Float:0x7F800000
*/
const Float:FLOAT_INFINITY = Float:0b_01111111_10000000_00000000_00000000;

/**
 * 判断一个浮点数是否为无效数字.
 *
 * @note	NaN并不是一个具体的值,而是一个范围.
 *		使用==运算符,对于两个相等的NaN浮点数会返回假.
 *		所以才需要使用这个函数代替==运算符.
 *
 * @param value	需要被检验的浮点数.
 *
 * @return false=不在NaN范围内,true=在NaN范围内
*/
stock bool:IsNaN(Float:value)
{
	// 指数部分必须全部为1
	const Exponent = 0b_01111111_10000000_00000000_00000000;
	if (_:value & Exponent != Exponent) return false;
	// 尾数部分必须非0
	const Mantissa = 0b_00000000_01111111_11111111_11111111;
	return _:value & Mantissa ? true : false;
}

/**
 * 浮点数求余运算.
 *
 * @param a	被除数
 * @param b	除数
 *
 * @return	a % b
*/
stock Float:FloatMod(Float:a, Float:b)
{
	return a - b * floatround(a / b, floatround_floor);
}

/** 
 * 将数值文本根据空白字符或逗号隔开,根据需要转换为整数或浮点数数组.
 *
 * @param text			需要被解析的文本
 * @param numberArray		用来接收解析结果的数组
 * @param numberArraySize 	需要解析多少个数值(不足则用0补充)
 * @param needFloatNumber	需要解析为整数还是浮点数:false=整数;true=浮点数
 *
 * @return 实际解析得到的数值数量(不包括补充的0)
*/
stock NumberArrayTextParser(const text[], {_, Float}:numberArray[], numberArraySize, bool:needFloatNumber)
{
	const PRECISION = 8;			// 定义浮点数最大精度,最多15
	static part[1 + 39 + 1 + PRECISION + 1];// 1位记录-,39位记录整数部分,1位记录小数点,PRECISION位记录小数部分,1位记录终止符
	
	new startPos = -1;			// 记录数字起点,-1表示尚未开始
	new endPos;				// 记录数字终点,或表示当前字符位置
	new bool:findSignChar;			// 是否已经找到负号
	new bool:findDotChar;			// 是否已经找到小数点
	new numberCount;			// 记录已转换数量
	
	// 循环条件:且转换数量未达标
	for (; numberCount < numberArraySize; endPos++)
	{
		// 若是负号
		if (text[endPos] == '-')
		{
			// 若已登记起点,或已经登记过负号,或负号下一个字不是数字,直接停止循环
			if (-1 < startPos || findSignChar || !isalnum(text[endPos + 1])) break;
			// 记住起点,负号
			startPos = endPos, findSignChar = true;
			continue;
		}
		
		// 若是数字
		if (isalnum(text[endPos]))
		{
			// 若尚未登记起点,将这个数字的索引作为起点
			if (startPos < 0) startPos = endPos;
			// 若已经登记起点,那么跳过这个数字
			continue;
		}
		
		// 若是小数点
		if (text[endPos] == '.')
		{
			// 若我们需要的是浮点数,需要一些额外解析...
			if (needFloatNumber)
			{
				// 若未登记起点,或已经登记过小数点,或小数点下一个字不是数字,直接停止循环
				if (startPos < 0 || findDotChar || !isalnum(text[endPos + 1])) break;
				// 记住小数点
				findDotChar = true;
				continue;
			}
			// 我们不需要浮点数,停止循环
			break;
		}
		
		// 若是空白字符或逗号
		if (isspace(text[endPos]) || text[endPos] == ',')
		{
			// 若尚未登记起点,跳过
			if (startPos < 0) continue;
			
			// 若已经登记起点,则当前位置是终点.可以进行转换
			copy(part, min(endPos - startPos, charsmax(part)), text[startPos]);
			if (needFloatNumber)	numberArray[numberCount++] = floatstr(part);
			else			numberArray[numberCount++] = str_to_num(part);
			// 重置findSignChar,startPos和findDotChar
			startPos = -1, findSignChar = findDotChar = false;
			continue;
		}
		
		// 若是终止符,或其它意义不明的字符,停止循环
		break;
	}
	// 若已经登记起点
	if (-1 < startPos)
	{
		// 若已经登记起点,则当前位置是终点.可以进行转换
		copy(part, min(endPos - startPos, charsmax(part)), text[startPos]);
		if (needFloatNumber)	numberArray[numberCount++] = floatstr(part);
		else			numberArray[numberCount++] = str_to_num(part);
	}
	
	// 若转换数量不足我们所需,用0填补后续空间
	while (numberCount < numberArraySize) numberArray[numberCount++] = 0;
	
	// 返回已转换数量
	return numberCount;
}