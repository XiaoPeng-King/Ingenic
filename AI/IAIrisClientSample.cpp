#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "IAIrisClientSDK.h"
#include "IAIrisServerSDK.h"
#include "globalFun.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
//Camera sensor type
//////////////////////////////////////////////////////////////////////////
#define IR_ALGO_DEV_INV					0		//Invalid camera
#define IR_ALGO_DEV_CAP					1		//Common capture camera
#define IR_ALGO_DEV_SML					2		//Small integrated camera
#define IR_ALGO_DEV_CNT					3		//Contact high-quality camera

enum VeryfydevMode{
    VERYFYDEV_ING,
    VERYFYDEV_ERROR,
    VERYFYDEV_SUCCESS,
    VERYFYDEV_INVALID_LICENSE,
    VERYFYDEV_OTHER_FAILED,
};

const int Iris_Ori_ImH = 1080;/*!< 虹膜图像MAX宽*/
const int Iris_Ori_ImW = 1920;/*!< 虹膜图像MAX高*/
const int Iris_Ori_ImD = 1;/*!< 虹膜图像通道*/
const int Iris_MAX_IM_SIZE	= (Iris_Ori_ImH*Iris_Ori_ImW*Iris_Ori_ImD);/*!< 最大的允许虹膜图像尺寸*/

//注册和识别的默认路径
#define APP_Enr_Image_Path_Left ("./ResultSave/Enroll_Image/Left_Eye/")
#define APP_Enr_Image_Path_Right ("./ResultSave/Enroll_Image/Right_Eye/")
#define APP_Enr_Code_Path_Left ("./ResultSave/Enroll_Template/Left_Eye/")
#define APP_Enr_Code_Path_Right ("./ResultSave/Enroll_Template/Right_Eye/")


#define APP_ImSave_Format (".bmp")
#define APP_Enr_Code_Format (".enr")

// 数字水印
#define iaIRIM_WaterMark_Len		4//33
#define IR_DevInfo_Len				50//20140313 测试设备序列号

const static char iaIRDeviceID_CAP[iaIRIM_WaterMark_Len] = "IAC";	//
const static char iaIRDeviceID_CNT[iaIRIM_WaterMark_Len] = "IAE";	//

unsigned char g_irisImgBuf[Iris_MAX_IM_SIZE]; //注意：实际应用中，该数组为camera出来的每帧图像数据
unsigned char irisTmpBuf[Iris_MAX_IM_SIZE + 1078];

unsigned char *g_enrollCodesL;	//保存左眼注册特征，最大数目为10000;
unsigned char *g_enrollCodesR;	//保存右眼注册特征，最大数目为10000;

int g_enrNumL = 0,g_enrNumR = 0;//虹膜实际读入系统的注册特征数目


enum APPMaxNum{APP_Max_Code_Num=6000};

string g_strNameLeft[APP_Max_Code_Num];
string g_strNameRight[APP_Max_Code_Num];

string g_strUserName = "dd";
Demo_Timer g_eventTimer;

bool g_EnrollManualStop = false;
bool g_IdentManualStop = false;

int g_InitFlag = false;
int g_demo_workMode = IrisWorkModeIdle;
VeryfydevMode g_VERYFYDEV  = VERYFYDEV_ING; //标记设备的校验状态

//////////////////状态回调数据记录//////////////////////
static IrisPos g_irisPos;
static int g_leftNum = 0;
static int g_rightNum = 0;
static int g_curDist =0;
static int g_leftRadius = 0;
static int g_rightRadius = 0;
static int g_pushImage = 0;
static IrisDistFlag g_distEstimation;
static int g_UIUEMsgID = -1;

string demo_getUIUE(int throwAwayFrameStatus);
void demo_getVersion();
int demo_init();
int demo_release();
int demo_enroll();
int demo_stopEnroll();
int demo_ident();
int demo_stopIdent();

std::thread m_image_thread;
std::thread m_enroll_thread;
std::thread m_ident_thread;

int itSetImageWaterMark(unsigned char *pImage, int imageH, int imageW, int deviceType)
{
	if (deviceType == IR_ALGO_DEV_CNT)
	{
		memcpy(pImage + 3 * imageW, iaIRDeviceID_CNT, iaIRIM_WaterMark_Len);// iaIRIM_WaterMark_Len);
		memcpy(pImage + 4 * imageW - 11, iaIRDeviceID_CNT, iaIRIM_WaterMark_Len);//iaIRIM_WaterMark_Len);
	}
	else
	{
		memcpy(pImage + 3 * imageW, iaIRDeviceID_CAP, iaIRIM_WaterMark_Len);// iaIRIM_WaterMark_Len);
		memcpy(pImage + 4 * imageW - 11, iaIRDeviceID_CAP, iaIRIM_WaterMark_Len);//iaIRIM_WaterMark_Len);
	}

	return 0;
}

static int s_irisFrameRate = 0;
void DemoIris_StatusInfoCallback(EnrIdenStatus* enrIdenStatus)
{
    static int qtnum=0;

    static struct timeval qtbegintv;
    static struct timeval qtendtv;
    if(qtnum==0) gettimeofday(&qtbegintv,NULL);
    qtnum = qtnum+1;
    if(qtnum >=20)
    {
        qtnum = 0;
        gettimeofday(&qtendtv,NULL);
        int qttimediff =  (qtendtv.tv_sec-qtbegintv.tv_sec)*1000000+qtendtv.tv_usec-qtbegintv.tv_usec;
        qttimediff = qttimediff/20;
        s_irisFrameRate = 1000000.0/qttimediff;
    }

    if (VERYFYDEV_SUCCESS != g_VERYFYDEV)
    {
        if(enrIdenStatus->reservedInfo[IRIS_RESERVEDINFO_ENCRYPT_STATUS] == E_IRIS_VERYFYDEV_ING)
        {
            //正在校验
            g_VERYFYDEV = VERYFYDEV_ING;
            DEBUGLOG("加密芯片验证 正在进行中请等待...\n");
        }
        else if(enrIdenStatus->reservedInfo[IRIS_RESERVEDINFO_ENCRYPT_STATUS] == E_IRIS_VERYFYDEV_ERROR)
        {
            //设备校验出错
            g_VERYFYDEV = VERYFYDEV_ERROR;
            DEBUGLOG("加密芯片验证 出错...\n");
        }
        else if(enrIdenStatus->reservedInfo[IRIS_RESERVEDINFO_ENCRYPT_STATUS] == E_IRIS_INVALID_LICENSE)
        {
            //无效的算法证书
            g_VERYFYDEV = VERYFYDEV_INVALID_LICENSE;
            DEBUGLOG("加密芯片验证 无效...\n");
        }
        else if(enrIdenStatus->reservedInfo[IRIS_RESERVEDINFO_ENCRYPT_STATUS] == E_IRIS_OK)
        {
            //校验成功
            g_VERYFYDEV = VERYFYDEV_SUCCESS;
            DEBUGLOG("加密芯片验证 成功...\n");
        }
        else
        {
            //其他错误
            g_VERYFYDEV = VERYFYDEV_OTHER_FAILED;
        }
    }

    //memcpy(g_irisImgBuf, enrIdenStatus->imageData, DISPLAY_IMG_SIZE);
    g_irisPos.leftIrisPos.xPixel = enrIdenStatus->irisPos.leftIrisPos.xPixel;
    g_irisPos.leftIrisPos.yPixel = enrIdenStatus->irisPos.leftIrisPos.yPixel;
    g_irisPos.rightIrisPos.xPixel = enrIdenStatus->irisPos.rightIrisPos.xPixel;
    g_irisPos.rightIrisPos.yPixel = enrIdenStatus->irisPos.rightIrisPos.yPixel;
    g_curDist = enrIdenStatus->irisPos.dist;
    g_leftRadius = enrIdenStatus->irisPos.leftIrisPos.radius;
    g_rightRadius = enrIdenStatus->irisPos.rightIrisPos.radius;
    g_leftNum = enrIdenStatus->leftIrisProgress;
    g_rightNum = enrIdenStatus->rightIrisProgress;
    g_distEstimation = enrIdenStatus->distEstimation;
    g_UIUEMsgID = enrIdenStatus->uiueID;

    int sdkWorkMode = enrIdenStatus->workMode;

    string irisUIUE = demo_getUIUE(g_UIUEMsgID);
    //DEBUGLOG("demo g_workMode: %d\n",g_workMode);

    if((sdkWorkMode != IrisWorkModeIdle) && (g_VERYFYDEV == VERYFYDEV_SUCCESS))
    {
        DEBUGLOG("demo============>> IRIS UIUE: %s\n",irisUIUE.c_str());
        DEBUGLOG("demo============>> 虹膜工作模式: %d 左眼进度: %d 右眼进度: %d 距离: %d \n",\
                 sdkWorkMode,g_leftNum,g_rightNum,g_curDist);
    }

}

string demo_getUIUE(int throwAwayFrameStatus)
{
    string strUIUEMsg = std::to_string(throwAwayFrameStatus);

    switch(throwAwayFrameStatus)
    {
    case Iris_UIUE_MsgID_SUITABLE:
        strUIUEMsg.append(" Iris:当前状态合适");
        break;
        break;
    case Iris_UIUE_MsgID_ATTACK:
        strUIUEMsg.append(" Iris:请先注册");
        break;
    case Iris_UIUE_MsgID_EYE_NOT_FOUND:
        strUIUEMsg.append(" Iris:请将双眼对准取景框");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_CLOSE:
        strUIUEMsg.append(" Iris:请远离");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_FAR:
        strUIUEMsg.append(" Iris:请靠近");
        break;
    case Iris_UIUE_MsgID_WITH_GLASS:
        strUIUEMsg.append(" Iris:尝试调整下角度或摘掉眼镜");
        break;
    case Iris_UIUE_MsgID_BAD_EYE_OPENNESS:
        strUIUEMsg.append(" Iris:请睁大眼睛或调整下角度");
        break;
    case Iris_UIUE_MsgID_MOTION_BLUR:
    case Iris_UIUE_MsgID_FOCUS_BLUR:
        strUIUEMsg.append(" Iris:正在扫描,请保持稳定");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_DOWN:
        strUIUEMsg.append(" Iris:请向上移动");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_UP:
        strUIUEMsg.append(" Iris:请向下移动");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_RIGHT:
        strUIUEMsg.append(" Iris:请向左移动");
        break;
    case Iris_UIUE_MsgID_EYE_TOO_LEFT:
        strUIUEMsg.append(" Iris:请向右移动");
        break;
    default:
        strUIUEMsg.append(" ");
        break;
    }

    return strUIUEMsg;
}

void demo_getVersion()
{
    if(g_InitFlag)
    {
        int typeInof = 0;
        char unitInfo[MAX_PATH];
        char modelInfo[MAX_PATH];
        char serialNumInfo[MAX_PATH];

        Iris_GetDevInfo(&typeInof, unitInfo, modelInfo, serialNumInfo);

        DEBUGLOG("demo=============>> typeInof: %d\n",typeInof);
        DEBUGLOG("demo=============>> unitInfo: %s\n",unitInfo);
        DEBUGLOG("demo=============>> modelInfo: %s\n",modelInfo);
        DEBUGLOG("demo=============>> serialNumInfo: %s\n",serialNumInfo);
    }
    else
    {
        DEBUGLOG("Please Init SDK\n");
    }

    return;
}

int demo_init()
{

    int res = Iris_Init(DemoIris_StatusInfoCallback);

    if(res == E_IRIS_OK)
    {
        DEBUGLOG("SDK Init 正在进行中请等待...\n");
    }
    else
    {
        DEBUGLOG("SDK初始化失败,%d,详情请对照头文件\n",res);
    }

    while(g_VERYFYDEV == VERYFYDEV_ING)
    {
        Demo_Delay(1);
    }

    g_InitFlag = 1;
    return 0;
}

int demo_release()
{
    if(g_demo_workMode == IrisWorkModeEnroll)
    {
        //stop Enroll
        Iris_StopEnroll();
        g_demo_workMode = IrisWorkModeIdle;
    }
    if(g_demo_workMode == IrisWorkModeIden)
    {
        //stop Ident
        Iris_StopGetIrisInfo();
        g_demo_workMode = IrisWorkModeIdle;
    }
    if(g_InitFlag)
    {
        Iris_Release();
        g_InitFlag = false;
        DEBUGLOG("SDK Release OK\n");
    }
    else
    {
        DEBUGLOG("Please Init SDK\n");
    }
    if(g_pushImage)
    {
	g_pushImage = 0;
    }    
    return 0;
}

void Thread_Push_Image()
{
	int deviceType = 1;		//IR_ALGO_DEV_SML
	string file_paths = "./Image_2M/";
    	std::vector<string> files;
    	getFiles(file_paths.c_str(), files);
	std::vector<string>::iterator  i_files = files.begin();
	int files_num = files.size();
	printf("there has %d images in fold %s!\n", files_num, file_paths.c_str());
	string filePath="./iris_2M.bmp";
	while(g_pushImage)
	//while (i_files != files.end())
	{
		//string filePath = file_paths + *i_files;
		FILE* srcFile = fopen(filePath.c_str(), "rb+");
		fread(irisTmpBuf, 1, Iris_Ori_ImW * Iris_Ori_ImH + 1078, srcFile);
		fclose(srcFile);

		Demo_DipImInvert(g_irisImgBuf, irisTmpBuf + 1078, Iris_Ori_ImH, Iris_Ori_ImW);

		// 添加水印
		itSetImageWaterMark(g_irisImgBuf, Iris_Ori_ImH, Iris_Ori_ImW, deviceType);

		Iris_PushCameraImage(g_irisImgBuf, Iris_Ori_ImH, Iris_Ori_ImW);
		Demo_Delay(40);
		i_files++;
	}

}


int demo_pushImage()
{
	g_pushImage = 1;
	m_image_thread = std::thread(Thread_Push_Image);
}

SDKIrisEnrollInfoOut enrollInfo;
void Thread_Iris_Enroll()
{
    int s_irisNumEnr = 3;
    ResultFlag flag;
    AlgConfigStruct algConfig;
    algConfig.minEnrIdenTime = IRIS_MIN_ENR_TIME;
    algConfig.maxEnrIdenTime = IRIS_MAX_ENR_TIME;
    algConfig.reservedPara[IRIS_RESERVEDPARA_ENROLLNUM] = s_irisNumEnr;
    int res = Iris_StartEnroll(&enrollInfo, IrisBoth,flag, &algConfig);

    if(res == E_IRIS_OK)
    {
        int left_save_num = 0;
        int right_save_num = 0;
        for (int num = 0; num < enrollInfo.leftNum; num++)
        {
            int enrCodePosLeft=0;
            int enrCodePosRight=0;
            string index;
            string curName;

            //left iris
            {
                stringstream ss;
                ss << left_save_num;
                ss >> index;
                curName = g_strUserName + "_Left_" + index;
                left_save_num = left_save_num + 1;

                bool leftDupCheck = false;
                int indL;
                for (indL = 0; indL < g_enrNumL; indL++)
                {
                    if (g_strNameLeft[indL] == curName)
                    {
                        leftDupCheck = true;
                        break;
                    }
                }
                if(leftDupCheck)
                {
                    enrCodePosLeft = IRIS_ENROLL_FEATURE_SIZE * indL;
                    memcpy(g_enrollCodesL + enrCodePosLeft, enrollInfo.enrollFeatureLeft + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
                    g_strNameLeft[indL] = curName;
                }
                else
                {
                    enrCodePosLeft = IRIS_ENROLL_FEATURE_SIZE * g_enrNumL;
                    memcpy(g_enrollCodesL + enrCodePosLeft, enrollInfo.enrollFeatureLeft + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
                    g_enrNumL++;
                    g_strNameLeft[indL] = curName;
                }
               // save image
                string saveImageName = curName + APP_ImSave_Format;
                Demo_SaveImage(APP_Enr_Image_Path_Left,saveImageName,enrollInfo.imageDataLeft + num*IRIS_IMG_SIZE,IRIS_IMG_WIDTH,IRIS_IMG_HEIGHT);
                // save feature
                string curFileName = APP_Enr_Code_Path_Left + curName + APP_Enr_Code_Format;
                Demo_ItFeatSave(APP_Enr_Code_Path_Left,curFileName.c_str(), enrollInfo.enrollFeatureLeft + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
            }

            // right iris
            {
                stringstream ss;
                ss << right_save_num;
                ss >> index;
                curName = g_strUserName + "_Right_" + index;
                right_save_num = right_save_num + 1;

                bool rightDupCheck = false;
                int indR;
                for (indR = 0; indR < g_enrNumR; indR++)
                {

                    if (g_strNameRight[indR] == curName)
                    {
                        rightDupCheck = true;
                        break;
                    }
                }
                if(rightDupCheck)
                {
                    enrCodePosRight = IRIS_ENROLL_FEATURE_SIZE * indR;
                    memcpy(g_enrollCodesR + enrCodePosRight, enrollInfo.enrollFeatureRight + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
                    g_strNameRight[indR] = curName;
                }
                else
                {
                    enrCodePosRight = IRIS_ENROLL_FEATURE_SIZE * g_enrNumR;
                    memcpy(g_enrollCodesR + enrCodePosRight, enrollInfo.enrollFeatureRight + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
                    g_enrNumR++;
                    g_strNameRight[indR] = curName;
                }
                // save image
                string saveImageName = curName + APP_ImSave_Format;
                Demo_SaveImage(APP_Enr_Image_Path_Right,saveImageName,enrollInfo.imageDataRight + num*IRIS_IMG_SIZE,IRIS_IMG_WIDTH,IRIS_IMG_HEIGHT);
                // save feature
                string curFileName = APP_Enr_Code_Path_Right + curName + APP_Enr_Code_Format;
                Demo_ItFeatSave(APP_Enr_Code_Path_Right,curFileName.c_str(), enrollInfo.enrollFeatureRight + num*IRIS_ENROLL_FEATURE_SIZE, IRIS_ENROLL_FEATURE_SIZE);
            }
        }

        DEBUGLOG("注册成功: %d\n",res);
    }
    else if(res == E_IRIS_IDEN_OVERTIME)
    {
        DEBUGLOG("注册超时: %d\n",res);
    }
    else if(res == E_IRIS_ENR_IDEN_INTERRUPT)
    {
        DEBUGLOG("注册停止: %d\n",res);
    }
    else
    {
        DEBUGLOG("注册失败: %d\n",res);
    }
}

int demo_enroll()
{
    if(!g_InitFlag)
    {
        DEBUGLOG("Please Init SDK\n");
        return 0;
    }
    if(g_demo_workMode != IrisWorkModeIdle)
    {
        DEBUGLOG("Working please wait...\n");
    }
    else
    {

        if(m_enroll_thread.joinable())
        {
            m_enroll_thread.join();
        }

        g_demo_workMode = IrisWorkModeEnroll;
        m_enroll_thread = std::thread(Thread_Iris_Enroll);

    }

}

int demo_stopEnroll()
{
    if(g_demo_workMode == IrisWorkModeEnroll)
    {
        Iris_StopEnroll();
        if(m_enroll_thread.joinable())
        {
            m_enroll_thread.join();
        }
        g_demo_workMode = IrisWorkModeIdle;
        DEBUGLOG("Enroll stopped\n");
    }
    else
    {
        DEBUGLOG("Enroll has been stopped\n");
    }
    return 0;
}

SDKIrisIdenFeatureOut idenInfo;
void Thread_Iris_Ident()
{
    int nFunFlag = E_IRIS_ERROR_BASE;

    int matchIndexL = -1;
    int matchResultL = -1;
    bool leftMatched = false;
    int matchIndexR = -1;
    int matchResultR = -1;
    bool rightMatched = false;

    ResultFlag flag;
    AlgConfigStruct algConfig;
    algConfig.minEnrIdenTime = IRIS_MIN_REC_TIME;
    algConfig.maxEnrIdenTime = 60;
    nFunFlag = Iris_StartGetIrisInfo(&idenInfo,IrisUndef,flag, &algConfig);

    if(g_demo_workMode == IrisWorkModeIdle)
    {
        DEBUGLOG("结果：手动停止识别\n");
        return ;
    }

    if(nFunFlag == E_IRIS_OK)
    {
        string IdentifyName;
        //判断左眼
        if (flag == IrisEnrRecLeftSuccess || flag == IrisEnrRecBothSuccess)
        {
            for (int i = 0; i < IRIS_GETIRISINFO_NUM; i++)
            {                
                matchResultL = Iris_Ver1N(idenInfo.idenFeatureLeft + i * IRIS_IDEN_FEATURE_SIZE, g_enrollCodesL, g_enrNumL, &matchIndexL);
                if(matchResultL == E_IRIS_SERVER_OK)
                {
                    IdentifyName = g_strNameLeft[matchIndexL];
                    vector<string> parts = Demo_split(IdentifyName,"_");
                    IdentifyName = parts[0];
                    DEBUGLOG("结果：识别成功 (L:%d of %d); 名字ID: %s\n", matchIndexL, g_enrNumL, IdentifyName.c_str());
                    leftMatched = true;

                    break;
                }
            }
        }
        //判断右眼
        if (!leftMatched && (flag == IrisEnrRecRightSuccess || flag == IrisEnrRecBothSuccess))
        {
            for (int i = 0; i < IRIS_GETIRISINFO_NUM; i++)
            {
                matchResultR = Iris_Ver1N(idenInfo.idenFeatureRight + i * IRIS_IDEN_FEATURE_SIZE, g_enrollCodesR, g_enrNumR, &matchIndexR);
                if(matchResultR == E_IRIS_SERVER_OK)
                {
                    IdentifyName = g_strNameRight[matchIndexR];
                    vector<string> parts = Demo_split(IdentifyName,"_");
                    IdentifyName = parts[0];
                    DEBUGLOG("结果：识别成功 (R:%d of %d); 名字ID: %s\n", matchIndexR, g_enrollCodesR, IdentifyName.c_str());
                    rightMatched = true;

                    break;
                }
            }

        }
        if (matchResultL != E_IRIS_SERVER_OK && matchResultR != E_IRIS_SERVER_OK)
        {
            DEBUGLOG("结果：识别失败: %d, %d\n",matchResultL, matchResultR);
        }
    }
    else if(nFunFlag == E_IRIS_IDEN_OVERTIME)
    {
        DEBUGLOG("结果：识别超时\n");
    }
    else
    {
        DEBUGLOG("结果：识别失败\n");
    }

    return ;
}

int demo_ident()
{
    if(!g_InitFlag)
    {
        DEBUGLOG("Please Init SDK\n");
        return 0;
    }
    if(g_demo_workMode != IrisWorkModeIdle)
    {
        DEBUGLOG("Working please wait...\n");
    }
    else
    {
        g_demo_workMode = IrisWorkModeIden;
        if(m_ident_thread.joinable())
        {
            m_ident_thread.join();
        }
        m_ident_thread = std::thread(Thread_Iris_Ident);
    }
}

int demo_stopIdent()
{
    if(g_demo_workMode == IrisWorkModeIden)
    {
        Iris_StopGetIrisInfo();
        if(m_ident_thread.joinable())
        {
            m_ident_thread.join();
        }
        g_demo_workMode = IrisWorkModeIdle;
        DEBUGLOG("Ident stop\n");
    }
    else
    {
        DEBUGLOG("Ident stopped\n");
    }
    return 0;
}

void demo_LoadLeftTemplate(const string path)
{
    int enrPosLenLeft = 0;
    int EnrCodeLen = IRIS_ENROLL_FEATURE_SIZE;
    bool  nReadTpFlag = false;

    vector<string> files;
    int fileNum = Demo_GetIrisFiles(path,files,APP_Enr_Code_Format);
    if(fileNum == 0)
    {
        DEBUGLOG("=====================================================demo_LoadLeftTemplate fileNum null\n");
        return ;
    }
    DEBUGLOG("=====================================================demo_LoadLeftTemplate fileNum\n");
    for(const auto &x: files)
    {
        string bmpFile = x;

        nReadTpFlag = Demo_ItFeatLoad((const char*)bmpFile.c_str(),
                                      g_enrollCodesL + enrPosLenLeft, EnrCodeLen);
        if (nReadTpFlag)
        {
            vector<string> splitStr =  Demo_split(bmpFile, "\\");
            string str = splitStr.back();
            splitStr =  Demo_split(str, ".");
            str = splitStr[0];
            g_strNameLeft[g_enrNumL] = str;
            enrPosLenLeft += EnrCodeLen;
            g_enrNumL++;
            DEBUGLOG(" %03d: %s\n",g_enrNumL, str.c_str());
        }

        if (g_enrNumL >= APP_Max_Code_Num)
        {
            break;
        }
    }
    return;
}

void demo_LoadRightTemplate(const string path)
{
    int enrPosLenRight = 0;
    int EnrCodeLen = IRIS_ENROLL_FEATURE_SIZE;
    bool  nReadTpFlag = false;

    vector<string> files;
    int fileNum = Demo_GetIrisFiles(path,files,APP_Enr_Code_Format);
    if(fileNum == 0)
    {
        DEBUGLOG("=====================================================demo_LoadRightTemplate fileNum null\n");
        return ;
    }
    DEBUGLOG("=====================================================demo_LoadRightTemplate fileNum\n");
    for(const auto &x: files)
    {
        string bmpFile = x;

        nReadTpFlag = Demo_ItFeatLoad((const char*)bmpFile.c_str(),
                                      g_enrollCodesR + enrPosLenRight, EnrCodeLen);
        if (nReadTpFlag)
        {
            vector<string> splitStr =  Demo_split(bmpFile, "\\");
            string str = splitStr.back();
            splitStr =  Demo_split(str, ".");
            str = splitStr[0];
            g_strNameRight[g_enrNumR] = str;
            enrPosLenRight += EnrCodeLen;
            g_enrNumR++;
            DEBUGLOG(" %03d: %s\n",g_enrNumR, str.c_str());
        }

        if (g_enrNumR >= APP_Max_Code_Num)
        {
            break;
        }
    }
    return;
}

int main()
{
#if defined _WIN32 || defined _WIN64
    system("chcp 65001");
    system("cls");
#endif
    //应用程序用到的系统空间
    g_enrollCodesL = new unsigned char[APP_Max_Code_Num*IRIS_ENROLL_FEATURE_SIZE];
    g_enrollCodesR = new unsigned char[APP_Max_Code_Num*IRIS_ENROLL_FEATURE_SIZE];


    memset(g_enrollCodesL, 0, APP_Max_Code_Num*IRIS_ENROLL_FEATURE_SIZE);
    memset(g_enrollCodesR, 0, APP_Max_Code_Num*IRIS_ENROLL_FEATURE_SIZE);

    demo_LoadLeftTemplate(APP_Enr_Code_Path_Left);
    demo_LoadRightTemplate(APP_Enr_Code_Path_Right);

    int  input;
    bool while_flag = true;
    while(while_flag)
    {
        input = -1;
        printf("======================Please select an action==========================\n");
        printf("0--------------------Init SDK\n");
        printf("1--------------------Get SDK Version\n");
        printf("2--------------------Push Camera Image\n");
        printf("3--------------------Start Enroll\n");
        printf("4--------------------Stop Enroll\n");
        printf("5--------------------Start Ident\n");
        printf("6--------------------Stop Ident\n");
        printf("7--------------------Exit\n");
        printf("===========================================================\n");
        scanf("%d", &input);
        switch (input)
        {
        case 0://init
            demo_init();
            break;
        case 1://get version
	    demo_getVersion();           
            break;
        case 2://push image
            demo_pushImage();
            break;
        case 3://Enroll
            demo_enroll();
            break;
        case 4://stop Enroll
            demo_stopEnroll();
            break;
        case 5://Ident
            demo_ident();
            break;
        case 6://stop Ident
            demo_stopIdent();
            break;
        case 7://release
            demo_release();
            while_flag = false;
            break;
        default:
            DEBUGLOG("Please enter the correct function number\n");
            scanf("%*[^\n]%*c");
            break;
        }
    }

    return 0;
}
