/******************************************************************************************
** 文件名:   IAIrisClientSDK.h
**
** Copyright 
** 创建人:
** 日  期:   2017-09-21
** 修改人:
** 日  期:
** 描  述:   客户端SDK,提供初始化,虹膜注册,虹膜识别等功能
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once
#if defined _WIN32 || defined _WIN64
    #ifdef IAIRISCLIENTSDK_EXPORTS
    #define __IAIRISCLIENTSDK_API__ extern "C" __declspec(dllexport)
    #else
    #define __IAIRISCLIENTSDK_API__ extern "C" __declspec(dllimport)
    #endif
    #define _STYLE_CALL __stdcall
#else
    #define __IAIRISCLIENTSDK_API__ extern "C"
    #define _STYLE_CALL
#endif

#define IN
#define OUT
#define INOUT

/**************************************
* 常量定义
***************************************/
//版本信息字符串长度
const int IRIS_VERSION_LENGTH = 500;

//设备厂商unit长度
const int IRIS_UNIT_LENGTH = 11;
//设备型号model长度
const int IRIS_MODEL_LENGTH = 11;
//设备序列号serialNum长度
const int IRIS_SERIALNUM_LENGTH = 11;

//灰度图像BMP文件头大小
const int IRIS_BMP_HEADER_SIZE = 1078;

//预留空间长度
const int IRIS_RESERVEDINFO_LEN = 50;
const int IRIS_RESERVEDINFO_DEVICEERROR = 0;		//预留空间第0位与设备状态相关
const int IRIS_RESERVEDINFO_DEVICELIGHTMODE = 1;		//预留空间第1位与设备亮度模式异步频闪相关
const int IRIS_RESERVEDINFO_ENCRYPT_STATUS = 4;		//预留空间第4位返回验证状态

const int IRIS_RESERVEDPARA_LEN = 50;
const int IRIS_RESERVEDPARA_ENROLLNUM = 6;			//预留参数第6位表示注册图像个数，配置值必须是3，6，9之一，推荐9

const int DISPLAY_IMG_HEIGHT = 270;					//回调函数中,所得到图像的高
const int DISPLAY_IMG_WIDTH = 480;					//回调函数中,所得到图像的宽
const int DISPLAY_IMG_SIZE = (DISPLAY_IMG_HEIGHT * DISPLAY_IMG_WIDTH); //回调函数中，所得到图像的大小

//虹膜特征尺寸大小
const int IRIS_ENROLL_FEATURE_SIZE = 512;
const int IRIS_IDEN_FEATURE_SIZE = 1024;

//虹膜图像尺寸大小
const int IRIS_IMG_HEIGHT = 480;
const int IRIS_IMG_WIDTH = 640;
const long IRIS_IMG_SIZE = (IRIS_IMG_HEIGHT * IRIS_IMG_WIDTH);

const int IRIS_ENROLL_NUM = 3;
const int IRIS_GETIRISINFO_NUM = 3;

typedef struct tagSDKIrisIdenFeatureOutStruct
{
    int leftNum;																	//实际左眼特征个数，小于等于IRIS_GETIRISINFO_NUM
    int rightNum;																	//实际右眼特征个数，小于等于IRIS_GETIRISINFO_NUM
    unsigned char idenFeatureLeft[IRIS_IDEN_FEATURE_SIZE * IRIS_GETIRISINFO_NUM];	//左眼识别特征
    unsigned char idenFeatureRight[IRIS_IDEN_FEATURE_SIZE * IRIS_GETIRISINFO_NUM];	//右眼识别特征
    unsigned char imageDataLeft[IRIS_IMG_SIZE * IRIS_GETIRISINFO_NUM];				//左眼图像
    unsigned char imageDataRight[IRIS_IMG_SIZE * IRIS_GETIRISINFO_NUM];				//右眼图像
}SDKIrisIdenFeatureOut, *pSDKIrisIdenFeatureOut;

typedef struct tagSDKIrisEnrollInfoOutStruct
{
    int leftNum;																	//实际左眼特征个数，小于等于IRIS_ENROLL_NUM
    int rightNum;																	//实际右眼特征个数，小于等于IRIS_ENROLL_NUM
    unsigned char enrollFeatureLeft[IRIS_ENROLL_FEATURE_SIZE * IRIS_ENROLL_NUM];	//左眼注册特征
    unsigned char enrollFeatureRight[IRIS_ENROLL_FEATURE_SIZE * IRIS_ENROLL_NUM];	//右眼注册特征
    unsigned char idenFeatureLeft[IRIS_IDEN_FEATURE_SIZE * IRIS_ENROLL_NUM];        //左眼识别特征
    unsigned char idenFeatureRight[IRIS_IDEN_FEATURE_SIZE * IRIS_ENROLL_NUM];       //右眼识别特征
    unsigned char imageDataLeft[IRIS_IMG_SIZE * IRIS_ENROLL_NUM];					//左眼图像
    unsigned char imageDataRight[IRIS_IMG_SIZE * IRIS_ENROLL_NUM];					//右眼图像
}SDKIrisEnrollInfoOut, *pSDKIrisEnrollInfoOut;


//最大匹配特征数量
const int IRIS_MAX_NUM_OF_MATCH = 100;

//最小注册时间,单位:秒,下同
const int IRIS_MIN_ENR_TIME = 0;
//最大注册时间,超过该时间超时退出
const int IRIS_MAX_ENR_TIME = 120;
//最小识别时间
const int IRIS_MIN_REC_TIME = 0;
//最大识别时间,超过该时间超时退出
const int IRIS_MAX_REC_TIME = 30;

//函数调用错误码
const int E_IRIS_ERROR_BASE = 0;     //基础错误码

//与系统相关的定义，以1***开头
const int E_IRIS_INIT_FAILED = E_IRIS_ERROR_BASE - 1001;          //SDK初始化失败或者尚未初始化
const int E_IRIS_HANDLE_ERROR = E_IRIS_ERROR_BASE - 1002;         //在与Handle相关的函数中失败
const int E_IRIS_INVALID_DEV_STATUS = E_IRIS_ERROR_BASE - 1003;   //设备状态出错,设备初始化失败或停止工作或设备被拔出
const int E_IRIS_INVALID_LICENSE = E_IRIS_ERROR_BASE - 1004;	  //无效的授权,授权过期,需要申请新的授权SDK
const int E_IRIS_READ_DEVICEID_ERROR = E_IRIS_ERROR_BASE - 1005;  //读取设备ID出错
const int E_IRIS_VERYFYDEV_ERROR = E_IRIS_ERROR_BASE - 1006;      //设备校验出错
const int E_IRIS_VERYFYDEV_ING = E_IRIS_ERROR_BASE - 1007;        //正在校验设备
const int E_IRIS_FUNCTION_DISABLE = E_IRIS_ERROR_BASE - 1008;     //不支持的功能，根据具体情况及客户需求，不同的版本所支持的功能存在差异
const int E_IRIS_DEVICE_UPSIDEDOWN = E_IRIS_ERROR_BASE - 1009;	  //设备上下翻转

//与注册或识别算法逻辑相关的以21**开头
const int E_IRIS_ENR_IDEN_FAILED = E_IRIS_ERROR_BASE - 2101;			//注册、识别、获取虹膜特征信息失败
const int E_IRIS_FAIL2STOP = E_IRIS_ERROR_BASE - 2102;					//调用停止注册、停止识别、停止获取虹膜特征信息函数时失败
const int E_IRIS_ALGORITHM_CONFLICT = E_IRIS_ERROR_BASE - 2103;			//算法冲突，注册、识别、获取虹膜特征信息不能同时进行
const int E_IRIS_INVALID_ENR_INPUT = E_IRIS_ERROR_BASE - 2104;			//注册、获取虹膜特征信息时无效的输入,如图像数目，时间，EyeMode等
const int E_IRIS_INVALID_REC_INPUT = E_IRIS_ERROR_BASE - 2105;			//识别时无效的输入,如识别时间，EyeMode等
const int E_IRIS_INVALID_MATCH_INPUT = E_IRIS_ERROR_BASE - 2106;		//比对时无效的输入
const int E_IRIS_IDEN_OVERTIME = E_IRIS_ERROR_BASE - 2107;				//注册识别时overtime
const int E_IRIS_IDEN_UNSUPPORT_FEATURE_NUM = E_IRIS_ERROR_BASE - 2108; //识别时输入特征数目超过授权数目
const int E_IRIS_IDEN_NO_FEATURE = E_IRIS_ERROR_BASE - 2109;			//识别时没有特征

//与异步注册、识别函数调用相关
const int E_IRIS_ASYNCSTART_FAILED = E_IRIS_ERROR_BASE - 2121;			//异步调用失败
const int E_IRIS_ASYNCTASK_BUSY = E_IRIS_ERROR_BASE - 2122;				//异步任务正在执行，还没有结果

//特别的，当人为调用停止注册、识别、获取虹膜特征信息函数时，之前调用的注册、识别、获取虹膜特征信息函数将返回1
const int E_IRIS_ENR_IDEN_INTERRUPT = 1;

//与比对算法逻辑相关的以22**开头
const int E_IRIS_MATCH_FAILED = E_IRIS_ERROR_BASE - 2201;				//比对失败

//与采图相关的以23**开头
const int E_IRIS_CAP_IMG_FAILED = E_IRIS_ERROR_BASE - 2301;				//图像采集失败
const int E_IRIS_CAP_IMG_INVALID = E_IRIS_ERROR_BASE - 2302;			//所采集图像不是授权的图像
//与获取图像特征相关的以24**开头
const int E_IRIS_GETFEATURE_FAILD = E_IRIS_ERROR_BASE - 2401;			//从图像获取特征失败

//与算法控制参数相关的以30**开头，
const int E_IRIS_INVALID_IRISMODE = E_IRIS_ERROR_BASE - 3001;			//无效的irismode
const int E_IRIS_INVALID_TIME = E_IRIS_ERROR_BASE - 3002;				//无效的注册时间或识别时间
const int E_IRIS_INVALID_FINDMODE = E_IRIS_ERROR_BASE - 3003;			//无效的FindMode
const int E_IRIS_INVALID_IFSPOOFDETECT = E_IRIS_ERROR_BASE - 3004;		//无效的IfSpoofDetect
const int E_IRIS_INVALID_PARAMETER = E_IRIS_ERROR_BASE - 3005;			//无效的算法控制参数

//与设备相关
const int E_IRIS_CAPTURER_INVALID = E_IRIS_ERROR_BASE - 4001;
const int E_IRIS_OPEN_CAPTURER_FAILED = E_IRIS_ERROR_BASE - 4002;

//函数调用通用返回值
const int E_IRIS_OK = 0;												//函数调用成功，并且完成了函数功能
const int E_IRIS_DEFAULT_ERROR = E_IRIS_ERROR_BASE - 8000;				//函数调用缺省错误，一般为函数调用结果初始值，一般不可能返回该值



//工作模式，表示空闲/注册/识别
enum IrisWorkMode
{
    IrisWorkModeIdle = -1,		//空闲模式，即未处于注册或者识别工作模式
    IrisWorkModeEnroll = 0,		//注册模式
    IrisWorkModeIden = 1,		//识别模式
};

//iris模式,表示虹膜注册/识别时,需要哪只眼睛进行注册/识别,如任意眼,左眼,右眼,双眼,同时也用于表示眼睛类型
enum IrisMode
{
    IrisNone = -1,	//无效
    IrisUndef = 0,	//任意眼
    IrisLeft = 1,	//左眼
    IrisRight = 2,	//右眼
    IrisBoth = 3,	//双眼
};

//距离提示
enum IrisDistFlag
{
    IrisDistTooFar = -1,
    IrisDistSuitable = 0,
    IrisDistTooClose = 1,
    IrisDistUnknown = 100,
};

//注册、识别、获取特征时的成功,失败提示
enum ResultFlag
{
    IrisEnrRecBothFailed = -3,		//注册、识别、获取特征时左右眼均注册或识别失败
    IrisEnrRecRightFailed = -2,		//右眼注册、识别、获取特征失败导致注册、识别、获取特征失败,左眼情况未知
    IrisEnrRecLeftFailed = -1,		//左眼注册、识别、获取特征失败导致注册、识别、获取特征失败,右眼情况未知

    IrisEnrRecUnknown = 0,			//注册、识别、获取特征结果未知

    IrisEnrRecLeftSuccess = 1,		//注册、识别、获取特征时仅左眼注册、识别、获取特征成功
    IrisEnrRecRightSuccess = 2,     //注册、识别、获取特征时仅右眼注册、识别、获取特征成功
    IrisEnrRecBothSuccess = 3,      //注册、识别、获取特征时左右眼均注册、识别、获取特征成功
};

//UIUE提示信息，注释中为建议提示用语
enum IrisUIUEMsgID {
    Iris_UIUE_MsgID_SUITABLE = 0,								//正在扫描，请保持稳定
    Iris_UIUE_MsgID_BAD_IMAGE_QUALITY = 1 << 0,					//请将双眼对准取景框
    Iris_UIUE_MsgID_EYE_NOT_FOUND = 1 << 1,						//请将双眼对准取景框
    Iris_UIUE_MsgID_EYE_TOO_CLOSE = 1 << 2,						//请离远一点
    Iris_UIUE_MsgID_EYE_TOO_FAR = 1 << 3,						//请靠近一点
    Iris_UIUE_MsgID_EYE_TOO_UP = 1 << 4,						//请将双眼对准取景框
    Iris_UIUE_MsgID_EYE_TOO_DOWN = 1 << 5,						//请将双眼对准取景框
    Iris_UIUE_MsgID_EYE_TOO_LEFT = 1 << 6,						//请将双眼对准取景框
    Iris_UIUE_MsgID_EYE_TOO_RIGHT = 1 << 7,						//请将双眼对准取景框
    Iris_UIUE_MsgID_MOTION_BLUR = 1 << 8,						//正在扫描，请保持稳定
    Iris_UIUE_MsgID_FOCUS_BLUR = 1 << 9,						//正在扫描，请保持稳定
    Iris_UIUE_MsgID_BAD_EYE_OPENNESS = 1 << 10,					//请睁大眼睛或调整下角度
    Iris_UIUE_MsgID_WITH_GLASS = 1 << 14,						//尝试调整下角度或摘掉眼镜
    Iris_UIUE_MsgID_OUTDOOR = 1 << 15,							//请离开室外强光环境或者请在室内环境中使用
    Iris_UIUE_MsgID_ATTACK = 1 << 16,							//请停止恶意攻击(建议不提示)
    Iris_UIUE_MsgID_BLINK = 1 << 17,							//请眨眼
    Iris_UIUE_MsgID_UNAUTHORIZED_ATTACK = 1 << 18,				//如果您是非注册用户，请停止使用(建议不提示)
    Iris_UIUE_MsgID_Device_Disconnected = 1 << 31,				//设备断开
};

//虹膜位置信息
typedef struct tagIrisPosStruct
{
    struct{
        int xPixel;
        int yPixel;
        int radius;
    }leftIrisPos, rightIrisPos;
    int dist;  //虹膜与摄像头之间的估算距离
}IrisPos;

//虹膜算法运行过程中的状态信息
typedef struct tagEnrIdenStatusStruct
{
    int workMode;					//工作模式,识别还是注册
    int irisMode;					//虹膜的图像类型：左眼、右眼、左右眼、任意眼、None
    int leftIrisProgress;			//左眼进度
    int rightIrisProgress;			//右眼进度

    IrisUIUEMsgID uiueID;			//UIUE状态ID
    IrisPos irisPos;				//虹膜位置信息
    IrisDistFlag distEstimation;				//表示当前距离是否合适，远、近、合适、未知
    unsigned char imageData[DISPLAY_IMG_SIZE];	//算法返回的图像(注：并不一定是原始采集的图像)
    int reservedInfo[IRIS_RESERVEDINFO_LEN];	//预留信息
}EnrIdenStatus, *pEnrIdenStatus;

//算法控制参数设置
typedef struct tagAlgConfigStruct
{
    int minEnrIdenTime;							//最小注册、获取特征时间,识别不需要设置,即使设置也不会生效
    int maxEnrIdenTime;							//最大注册、识别、获取特征时间,超过该时间还没有注册、识别、获取特征成功,注册、识别、获取特征超时退出
    int reservedPara[IRIS_RESERVEDPARA_LEN];	//预留控制参数
}AlgConfigStruct, *pAlgConfigStruct;

/*****************************************************************************
*                         回调函数
*  函 数 名：Iris_StatusInfoCallback
*  功    能：回调函数传递给上层应用
*  说    明：用户在应用程序端实现本回调函数,SDK将相关信息通过该回调函数传递给上层应用
*  参    数：EnrIdenStatus：输出参数，注册、识别、获取特征过程中的状态信息
*
*  返 回 值：
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
typedef void ( *Iris_StatusInfoCallback)(EnrIdenStatus* enrIdenStatus);

/*****************************************************************************
*                         设备资源初始化
*  函 数 名：Iris_Init
*  功    能：初始化设备及使用的相关资源
*  说    明：
*  参    数：statusInfoCB：输入参数，工作状态信息的回调
*
*  返 回 值：0：初始化成功
*			<0：初始化失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：razer
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_Init(IN Iris_StatusInfoCallback statusInfoCB);

/*****************************************************************************
*                         释放虹膜算法库资源
*  函 数 名：Iris_Release
*  功    能：释放设备及相关资源
*  说    明：
*  参    数：
*
*  返 回 值：0：释放成功
*			<0：释放失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_Release();

/*****************************************************************************
*                         给SDK送入虹膜图像
*  函 数 Iris_PushCameraImage
*  功    能：给SDK送入虹膜图像（注册和识别）
*  说    明：
*  参    数：image：输入参数，图像数据
*			 imageH：输入参数，图像数据的高
*			 imageW：输入参数，图像数据的宽
*
*
*  返 回 值：0：成功
*    		<0：失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_PushCameraImage(IN unsigned char *image, IN int imageH, IN int imageW);

/*****************************************************************************
*                         获取虹膜设备信息
*  函 数 名：Iris_GetDevInfo
*  功    能：获取虹膜设备信息
*  说    明：初始化设备成功后才能调用
*  参    数：type：输出参数，设备认证方式(1：指纹，2：人脸，3：指静脉，4：虹膜)，正常输出为：4
*			unit：输出参数，设备厂商，正常输出应为：IA
*			model：输出参数，设备型号
*			serialNum：输出参数，设备序列号
*
*  返 回 值：0：获取成功
*			<0：获取失败，根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_GetDevInfo(OUT int *type, OUT char* unit, OUT char* model, OUT char* serialNum);

/*****************************************************************************
*                         开始采集虹膜
*  函 数 名：Iris_StartEnroll
*  功    能：虹膜采集（注册）
*  说    明：
*  参    数：enrollInfo：输出参数，注册得到的左眼、右眼虹膜图像个数及注册特征、图像
*			irisMode：输入参数，指定需要指定哪只眼睛进行注册
*			resultFlag：输出参数，左眼、右眼虹膜注册成功标志
*			algConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：注册成功
*			1：人为停止注册
*    		<0：注册失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_StartEnroll(OUT SDKIrisEnrollInfoOut *enrollInfo, IN IrisMode irisMode, OUT ResultFlag &resultFlag, IN AlgConfigStruct *algConfig);

/*****************************************************************************
*                         停止采集虹膜
*  函 数 名：Iris_StopEnroll
*  功    能：停止采集虹膜
*  说    明：
*  参    数：
*
*  返 回 值：0：停止采集成功
*    		<0：停止采集失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_StopEnroll();

/*****************************************************************************
*                         获取虹膜识别特征信息
*  函 数 名：Iris_StartGetIrisInfo
*  功    能：获取虹膜识别特征信息，用于后续比对
*  说    明：
*  参    数：idenInfo：输出参数，获取到的左眼、右眼虹膜图像个数及识别特征、图像
*			irisMode：输入参数，指定需要获取哪只眼睛的虹膜信息
*			resultFlag：输出参数，左眼、右眼虹膜获取成功标志
*			algConfig：输入参数，算法控制参数
*
*
*  返 回 值：0：获取虹膜识别特征信息成功
*			1：人为停止获取虹膜识别特征信息
*    		<0：获取虹膜识别特征信息失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：razer
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_StartGetIrisInfo(OUT SDKIrisIdenFeatureOut *idenInfo, IN IrisMode irisMode, OUT ResultFlag &resultFlag, IN AlgConfigStruct *algConfig);

/*****************************************************************************
*                         停止获取虹膜识别特征信息
*  函 数 名：Iris_StopGetIdenInfo
*  功    能：停止获取虹膜识别特征信息
*  说    明：
*  参    数：
*
*  返 回 值：0：停止获取虹膜识别特征信息成功
*           <0：停止获取虹膜识别特征信息失败,根据返回值可以查询失败原因,详见函数调用错误码
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ int _STYLE_CALL Iris_StopGetIrisInfo();

/*****************************************************************************
*                         获取SDK及算法版本信息
*  函 数 名：Iris_GetVersion
*  功    能：获取SDK及算法版本信息
*  说    明：用户在应用程序端获取SDK及算法版本信息，如：授权时间等
*  参    数：version：输出参数，SDK及算法版本信息
*
*  返 回 值：
*  创 建 人：
*  创建时间：2017-09-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
__IAIRISCLIENTSDK_API__ void _STYLE_CALL Iris_GetVersion(char* version);
