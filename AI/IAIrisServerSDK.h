/******************************************************************************************
** 文件名:   IAIrisServerSDK.h
**
** Copyright 
** 创建人:
** 日  期:   2017-09-23
** 修改人:
** 日  期:
** 描  述:   虹膜服务端SDK,提供从虹膜图像获取特征,虹膜1:1，1:N比对等功能
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once
#if defined _WIN32 || defined _WIN64
    #ifdef IAIRISSERVERSDK_EXPORTS
    #define __IAIRISSERVERSDK_API__ extern "C" __declspec(dllexport)
    #else
    #define __IAIRISSERVERSDK_API__ extern "C" __declspec(dllimport)
    #endif
    #define _STYLE_CALL __stdcall
#else
    #define __IAIRISSERVERSDK_API__ extern "C"
    #define _STYLE_CALL
#endif
#define IN
#define OUT
#define INOUT

/************************************************************************/
/*                         错误码定义                                    */
/************************************************************************/
//虹膜特征尺寸大小
const int IRIS_ENROLL_FEATURE_LENGTH = 512;
const int IRIS_IDEN_FEATURE_LENGTH = 1024;

//虹膜图像尺寸大小
const int IRIS_IMAGE_HEIGHT = 480;
const int IRIS_IMAGE_WIDTH = 640;
const long IRIS_IMAGE_LENGTH = (IRIS_IMAGE_HEIGHT * IRIS_IMAGE_WIDTH);

//版本信息字符串长度
const int IRIS_SERVER_VERSION_LENGTH = 260;

//函数调用通用返回值
const int E_IRIS_SERVER_OK = 0;
//函数调用错误码
const int E_IRIS_SERVER_ERROR_BASE = 0;												//基础错误码
const int E_IRIS_SERVER_DEFAULT_ERROR = E_IRIS_SERVER_ERROR_BASE - 8000;			//函数调用缺省错误，一般为函数调用结果初始值，一般不可能返回该值
const int E_IRIS_SERVER_INVALID_LICENSE = E_IRIS_SERVER_ERROR_BASE - 1004;			//无效的授权,授权过期,需要申请新的授权SDK
const int E_IRIS_SERVER_INVALID_MATCH_INPUT = E_IRIS_SERVER_ERROR_BASE - 2106;		//比对时无效的输入
//与比对算法逻辑相关的以22**开头
const int E_IRIS_SERVER_MATCH_FAILED = E_IRIS_SERVER_ERROR_BASE - 2201;				//比对失败
//与获取图像特征相关的以24**开头
const int E_IRIS_SERVER_GETFEATURE_FAILD = E_IRIS_SERVER_ERROR_BASE - 2401;			//从图像获取特征失败

//与算法控制参数相关的以30**开头，
const int E_IRIS_SERVER_INVALID_PARAMETER = E_IRIS_SERVER_ERROR_BASE - 3005;		//无效的算法控制参数

//获取虹膜特征模式
enum IrisFeatureMode
{
    IrisFeatureModeEnroll = 0,		//注册模式
    IrisFeatureModeIden = 1,		//识别模式
};

/*****************************************************************************
*                         从图像获取虹膜特征信息
*  函 数 名：Iris_CodeFromImage
*  功    能：从图像获取虹膜注册或识别特征信息
*  说    明：
*  参    数：irisImageData：输入参数，虹膜图像
*			IrisFeatureMode：输入参数，虹膜特征模式，获取注册特征还是识别特征
*			feature：输出参数，从图像获取到的注册或识别特征
*			featureLength：输出参数，特征长度
*
*  返 回 值：0：从图像获取虹膜特征信息成功
*           <0：从图像获取虹膜特征信息失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISSERVERSDK_API__ int _STYLE_CALL Iris_CodeFromImage(IN unsigned char *irisImageData, OUT unsigned char *Feature, IN int irisFeatureMode, OUT int* featureLength);

/*****************************************************************************
*                         虹膜比对，1:1
*  函 数 名：Iris_Ver11
*  功    能：以1:1方式进行虹膜比对
*  说    明：
*  参    数：idenFeature：输入参数，识别特征
*			enrollFeature：输入参数，注册特征
*
*  返 回 值：0：比对成功
*			<0：比对失败，根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISSERVERSDK_API__ int _STYLE_CALL Iris_Ver11(IN unsigned char *idenFeature, IN unsigned char* enrollFeature);

/*****************************************************************************
*                         虹膜比对，1:N
*  函 数 名：Iris_Ver1N
*  功    能：以1:N方式进行虹膜比对
*  说    明：
*  参    数：idenFeature：输入参数，识别特征
*			enrollFeatureList：输入参数，注册特征
*			enrollFeatureCount：输入参数，注册特征个数
*			index：输出参数，比对成功的用户索引
*
*  返 回 值：0：比对成功
*			<0：比对失败，根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISSERVERSDK_API__ int _STYLE_CALL Iris_Ver1N(IN unsigned char *idenFeature, IN unsigned char* enrollFeatureList, IN int enrollFeatureCount, OUT int *index);
