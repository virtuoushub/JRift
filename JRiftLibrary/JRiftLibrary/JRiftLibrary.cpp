#include "de_fruitfly_ovr_OculusRift.h"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "OVR_CAPI_GL.h"
#include "CAPI/CAPI_HMDState.h"

using namespace OVR;

ovrHmd                      _pHmd = 0;
std::auto_ptr<ovrHmdDesc>   _pHmdDesc(0);

int				    _hmdIndex    = -1;
bool			    _initialised = false;
bool                _renderConfigured = false;
ovrPosef            _eyeRenderPose[2];
ovrGLTexture        _EyeTexture[2];

const bool          LogDebug = true;

// JNI class / method caching
static jclass       eyeRenderParams_Class                = 0;
static jmethodID    eyeRenderParams_constructor_MethodID = 0;
static jclass       frameTiming_Class                    = 0;
static jmethodID    frameTiming_constructor_MethodID     = 0;
static jclass       posef_Class                          = 0;
static jmethodID    posef_constructor_MethodID           = 0;
static jclass       sensorState_Class                    = 0;
static jmethodID    sensorState_constructor_MethodID     = 0;
static jclass       sizei_Class                          = 0;
static jmethodID    sizei_constructor_MethodID           = 0;
static jclass       hmdDesc_Class                        = 0;
static jmethodID    hmdDesc_constructor_MethodID         = 0;
static jclass       vector3f_Class                       = 0;
static jmethodID    vector3f_constructor_MethodID        = 0;
static jclass       matrix4f_Class                       = 0;
static jmethodID    matrix4f_constructor_MethodID        = 0;

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1initSubsystem(JNIEnv *env, jobject jobj) 
{
    DEBUGLOG("Initialising Oculus Rift subsystem...");

	Reset();

    if (!CacheJNIGlobals(env))
    {
        return false;
    }

	// Initialise LibOVR
	ovr_Initialize();

    // Try first HMD index [TODO: Store / use last used index]
    _hmdIndex = 0;

    if (CreateHmdAndStartSensor(_hmdIndex))
        _initialised = true;
	
	if (!_initialised)
	{
		printf("Unable to create Oculus Rift device interface.\n");
	}

	return _initialised;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySubsystem(JNIEnv *env, jobject jobj) 
{
	printf("Destroying Oculus Rift device interface.\n");	
		
	if (_initialised)
	{
		Reset();
	}
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getHmdDesc(JNIEnv *env, jobject) 
{
	if (!_initialised) 
        return 0;

    jstring productName = env->NewStringUTF( _pHmdDesc->ProductName == NULL ? "" : _pHmdDesc->ProductName );
    jstring manufacturer = env->NewStringUTF( _pHmdDesc->Manufacturer == NULL ? "" : _pHmdDesc->Manufacturer );
    jstring displayDeviceName = env->NewStringUTF( _pHmdDesc->DisplayDeviceName == NULL ? "" : _pHmdDesc->DisplayDeviceName );
    
    ClearException(env);

    jobject jHmdDesc = env->NewObject(hmdDesc_Class, hmdDesc_constructor_MethodID,
                                      (int)_pHmdDesc->Type,
                                      productName,
                                      manufacturer,
                                      (int)_pHmdDesc->Caps,
                                      (int)_pHmdDesc->DistortionCaps,
                                      _pHmdDesc->Resolution.w,
                                      _pHmdDesc->Resolution.h,
                                      _pHmdDesc->WindowsPos.x,
                                      _pHmdDesc->WindowsPos.y,
                                      _pHmdDesc->DefaultEyeFov[0].UpTan,
                                      _pHmdDesc->DefaultEyeFov[0].DownTan,
                                      _pHmdDesc->DefaultEyeFov[0].LeftTan,
                                      _pHmdDesc->DefaultEyeFov[0].RightTan,
                                      _pHmdDesc->DefaultEyeFov[1].UpTan,
                                      _pHmdDesc->DefaultEyeFov[1].DownTan,
                                      _pHmdDesc->DefaultEyeFov[1].LeftTan,
                                      _pHmdDesc->DefaultEyeFov[1].RightTan,
                                      _pHmdDesc->MaxEyeFov[0].UpTan,
                                      _pHmdDesc->MaxEyeFov[0].DownTan,
                                      _pHmdDesc->MaxEyeFov[0].LeftTan,
                                      _pHmdDesc->MaxEyeFov[0].RightTan,
                                      _pHmdDesc->MaxEyeFov[1].UpTan,
                                      _pHmdDesc->MaxEyeFov[1].DownTan,
                                      _pHmdDesc->MaxEyeFov[1].LeftTan,
                                      _pHmdDesc->MaxEyeFov[1].RightTan,
                                      (int)_pHmdDesc->EyeRenderOrder[0],
                                      (int)_pHmdDesc->EyeRenderOrder[1],
                                      displayDeviceName,
                                      _pHmdDesc->DisplayId
            );

    env->DeleteLocalRef( productName );
    env->DeleteLocalRef( manufacturer );
    env->DeleteLocalRef( displayDeviceName );

    if (jHmdDesc == 0) PrintNewObjectException(env, "HmdDesc");

    return jHmdDesc;
}

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1getNextHmd(JNIEnv *env, jobject jobj) 
{
    if (!_initialised)
        return false;

    // Reset render config
    ResetRenderConfig();

	// Stop sensor
	if (_pHmd)
		ovrHmd_StopSensor(_pHmd);

	// Cleanup HMD
	if (_pHmd)
		ovrHmd_Destroy(_pHmd);

    _pHmd = 0;
    _pHmdDesc.reset();

    const int hmdCount = ovrHmd_Detect();
    _hmdIndex++;
    if (_hmdIndex >= hmdCount)
        _hmdIndex = 0;

    return CreateHmdAndStartSensor(_hmdIndex);
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getSensorState(JNIEnv *env, jobject, jdouble time) 
{
	if (!_initialised) 
        return 0;

	// Get sensorstate at the specified time in the future from now (0.0 means 'now')
	ovrSensorState ss = ovrHmd_GetSensorState(_pHmd, time);

    ClearException(env);

    jobject jss = env->NewObject(sensorState_Class, sensorState_constructor_MethodID, 
                                 ss.Predicted.Pose.Orientation.x,   
                                 ss.Predicted.Pose.Orientation.y,  
                                 ss.Predicted.Pose.Orientation.z,   
                                 ss.Predicted.Pose.Orientation.w,   
                                 ss.Predicted.Pose.Position.x,      
                                 ss.Predicted.Pose.Position.y,      
                                 ss.Predicted.Pose.Position.z,      
                                 ss.Predicted.AngularVelocity.x,    
                                 ss.Predicted.AngularVelocity.y,    
                                 ss.Predicted.AngularVelocity.z,    
                                 ss.Predicted.LinearVelocity.x,     
                                 ss.Predicted.LinearVelocity.y,     
                                 ss.Predicted.LinearVelocity.z,     
                                 ss.Predicted.AngularAcceleration.x,
                                 ss.Predicted.AngularAcceleration.y,
                                 ss.Predicted.AngularAcceleration.z,
                                 ss.Predicted.LinearAcceleration.x, 
                                 ss.Predicted.LinearAcceleration.y, 
                                 ss.Predicted.LinearAcceleration.z, 
                                 ss.Predicted.TimeInSeconds,        
                                 ss.Recorded.Pose.Orientation.x,    
                                 ss.Recorded.Pose.Orientation.y,    
                                 ss.Recorded.Pose.Orientation.z,    
                                 ss.Recorded.Pose.Orientation.w,    
                                 ss.Recorded.Pose.Position.x,       
                                 ss.Recorded.Pose.Position.y,       
                                 ss.Recorded.Pose.Position.z,       
                                 ss.Recorded.AngularVelocity.x,     
                                 ss.Recorded.AngularVelocity.y,     
                                 ss.Recorded.AngularVelocity.z,     
                                 ss.Recorded.LinearVelocity.x,      
                                 ss.Recorded.LinearVelocity.y,      
                                 ss.Recorded.LinearVelocity.z,      
                                 ss.Recorded.AngularAcceleration.x, 
                                 ss.Recorded.AngularAcceleration.y, 
                                 ss.Recorded.AngularAcceleration.z, 
                                 ss.Recorded.LinearAcceleration.x,  
                                 ss.Recorded.LinearAcceleration.y,  
                                 ss.Recorded.LinearAcceleration.z,  
                                 ss.Recorded.TimeInSeconds,         
                                 ss.Temperature,
                                 ss.StatusFlags
                                 );

    if (jss == 0) PrintNewObjectException(env, "SensorState");

    return jss;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getFovTextureSize(JNIEnv *env, jobject, jfloat RenderScaleFactor)
{
	if (!_initialised)
		return 0;

	// A RenderScaleFactor of 1.0f signifies default (non-scaled) operation
    Sizei recommendedTex0Size = ovrHmd_GetFovTextureSize(_pHmd, ovrEye_Left,  _pHmdDesc->DefaultEyeFov[0], RenderScaleFactor);
    Sizei recommendedTex1Size = ovrHmd_GetFovTextureSize(_pHmd, ovrEye_Right, _pHmdDesc->DefaultEyeFov[1], RenderScaleFactor);
    Sizei RenderTargetSize;
    RenderTargetSize.w = recommendedTex0Size.w + recommendedTex1Size.w;
    RenderTargetSize.h = max ( recommendedTex0Size.h, recommendedTex1Size.h );

    ClearException(env);

    jobject jsizei = env->NewObject(sizei_Class, sizei_constructor_MethodID,
                                    RenderTargetSize.w,
                                    RenderTargetSize.h
                                    );

    if (jsizei == 0) PrintNewObjectException(env, "Sizei");

    return jsizei;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1configureRendering(
	JNIEnv *env, 
	jobject, 
	jint InTextureWidth,
	jint InTextureHeight,
	jint InTextureGLId,
	jlong WglContext, 
	jlong Window, 
	jlong GdiDc,
	jboolean VSyncEnabled)
{
	if (!_initialised)
		return 0;

    ResetRenderConfig();

    // Initialize eye rendering information for ovrHmd_Configure.
    // The viewport sizes are re-computed in case RenderTargetSize changed due to HW limitations.
    ovrEyeDesc eyes[2];
    eyes[0].Eye                 = ovrEye_Left;
    eyes[1].Eye                 = ovrEye_Right;
    eyes[0].Fov                 = _pHmdDesc->DefaultEyeFov[0];
    eyes[1].Fov                 = _pHmdDesc->DefaultEyeFov[1];
	eyes[0].TextureSize.w       = InTextureWidth;
	eyes[0].TextureSize.h       = InTextureHeight;
	eyes[1].TextureSize.w       = InTextureWidth;
	eyes[1].TextureSize.h       = InTextureHeight;
    eyes[0].RenderViewport.Pos  = Vector2i(0,0);
    eyes[0].RenderViewport.Size = Sizei(InTextureWidth / 2, InTextureHeight);
    eyes[1].RenderViewport.Pos  = Vector2i((InTextureWidth + 1) / 2, 0);
    eyes[1].RenderViewport.Size = eyes[0].RenderViewport.Size;

	// Setup GL texture data.
	_EyeTexture[0].OGL.Header.API            = ovrRenderAPI_OpenGL;
    _EyeTexture[0].OGL.Header.TextureSize.w  = InTextureWidth;
	_EyeTexture[0].OGL.Header.TextureSize.h  = InTextureHeight;
    _EyeTexture[0].OGL.Header.RenderViewport = eyes[0].RenderViewport;
	_EyeTexture[0].OGL.TexId                 = (GLuint)InTextureGLId;
    
    // Right eye uses the same texture, but different rendering viewport.
    _EyeTexture[1] = _EyeTexture[0];
    _EyeTexture[1].OGL.Header.RenderViewport = eyes[1].RenderViewport;

	// Configure OpenGL. 
	ovrGLConfig cfg; 
	cfg.OGL.Header.API         = ovrRenderAPI_OpenGL; 
	cfg.OGL.Header.RTSize      = Sizei(_pHmdDesc->Resolution.w, _pHmdDesc->Resolution.h); 
	cfg.OGL.Header.Multisample = 0;  // TODO: Use multisampling?

//#ifdef _WIN
	// Cast context pointers to 32 / 64 bit as appropriate
	cfg.OGL.WglContext = (HGLRC)(intptr_t)WglContext; 
	cfg.OGL.Window     = (HWND)(intptr_t)Window; 
	cfg.OGL.GdiDc      = (HDC)(intptr_t)GdiDc;
//#else etc...

	const unsigned DistortionCaps = ovrDistortion_Chromatic | ovrDistortion_TimeWarp | ovrDistortion_Vignette;
	ovrEyeRenderDesc EyeRenderDesc[2];

	ovrBool result = ovrHmd_ConfigureRendering(_pHmd, &cfg.Config, (VSyncEnabled ? 0 : ovrHmdCap_NoVSync), DistortionCaps, eyes, EyeRenderDesc);
	if (!result)
	{
		printf("ovrHmd_ConfigureRendering() - ERROR: failure");
        ResetRenderConfig();
		return 0;
	}

    _renderConfigured = true;

	jobject eyeRenderDesc = env->NewObject(eyeRenderParams_Class, eyeRenderParams_constructor_MethodID,
                                           EyeRenderDesc[0].Desc.Eye,
                                           EyeRenderDesc[0].Desc.TextureSize.w, 
                                           EyeRenderDesc[0].Desc.TextureSize.h,
                                           EyeRenderDesc[0].Desc.RenderViewport.Pos.x, 
                                           EyeRenderDesc[0].Desc.RenderViewport.Pos.y,
                                           EyeRenderDesc[0].Desc.RenderViewport.Size.w,
                                           EyeRenderDesc[0].Desc.RenderViewport.Size.h,
                                           EyeRenderDesc[0].Desc.Fov.UpTan,
                                           EyeRenderDesc[0].Desc.Fov.DownTan,
                                           EyeRenderDesc[0].Desc.Fov.LeftTan,
                                           EyeRenderDesc[0].Desc.Fov.RightTan,
                                           EyeRenderDesc[0].DistortedViewport.Pos.x, 
                                           EyeRenderDesc[0].DistortedViewport.Pos.y,
                                           EyeRenderDesc[0].DistortedViewport.Size.w,
                                           EyeRenderDesc[0].DistortedViewport.Size.h,
                                           EyeRenderDesc[0].PixelsPerTanAngleAtCenter.x,
                                           EyeRenderDesc[0].PixelsPerTanAngleAtCenter.y,
                                           EyeRenderDesc[0].ViewAdjust.x,
                                           EyeRenderDesc[0].ViewAdjust.y,
                                           EyeRenderDesc[0].ViewAdjust.z,
                                           EyeRenderDesc[1].Desc.Eye,
                                           EyeRenderDesc[1].Desc.TextureSize.w, 
                                           EyeRenderDesc[1].Desc.TextureSize.h,
                                           EyeRenderDesc[1].Desc.RenderViewport.Pos.x, 
                                           EyeRenderDesc[1].Desc.RenderViewport.Pos.y,
                                           EyeRenderDesc[1].Desc.RenderViewport.Size.w,
                                           EyeRenderDesc[1].Desc.RenderViewport.Size.h,
                                           EyeRenderDesc[1].Desc.Fov.UpTan,
                                           EyeRenderDesc[1].Desc.Fov.DownTan,
                                           EyeRenderDesc[1].Desc.Fov.LeftTan,
                                           EyeRenderDesc[1].Desc.Fov.RightTan,
                                           EyeRenderDesc[1].DistortedViewport.Pos.x, 
                                           EyeRenderDesc[1].DistortedViewport.Pos.y,
                                           EyeRenderDesc[1].DistortedViewport.Size.w,
                                           EyeRenderDesc[1].DistortedViewport.Size.h,
                                           EyeRenderDesc[1].PixelsPerTanAngleAtCenter.x,
                                           EyeRenderDesc[1].PixelsPerTanAngleAtCenter.y,
                                           EyeRenderDesc[1].ViewAdjust.x,
                                           EyeRenderDesc[1].ViewAdjust.y,
                                           EyeRenderDesc[1].ViewAdjust.z
										);

    return eyeRenderDesc;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1beginFrame(JNIEnv *env, jobject, jint FrameIndex)
{
    if (!_initialised)
        return 0;

    if (!_renderConfigured)
    {
        printf("beginFrame() - ERROR: Render config not set!");
        return 0;
    }

    // A FrameIndex of 0 should be used if GetFrameTiming was not used.
    // TODO: Support GetFrameTiming!
    ovrFrameTiming frameTiming = ovrHmd_BeginFrame(_pHmd, FrameIndex);

	jobject jframeTiming = env->NewObject(frameTiming_Class, frameTiming_constructor_MethodID,
                                          frameTiming.DeltaSeconds,
                                          frameTiming.ThisFrameSeconds,
                                          frameTiming.TimewarpPointSeconds,
                                          frameTiming.NextFrameSeconds,
                                          frameTiming.ScanoutMidpointSeconds,
                                          frameTiming.EyeScanoutSeconds[0],
                                          frameTiming.EyeScanoutSeconds[1]
										 );

    return jframeTiming;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1beginEyeRender(JNIEnv *env, jobject, jint Eye)
{
    if (!_initialised)
        return 0;

    if (!_renderConfigured)
    {
        printf("beginEyeRender() - ERROR: Render config not set!");
        return 0;
    }

    ovrEyeType eye = ovrEye_Left;
    if (Eye > 0)
        eye = ovrEye_Right;

    _eyeRenderPose[eye] = ovrHmd_BeginEyeRender(_pHmd, eye);

	jobject jposef = env->NewObject(posef_Class, posef_constructor_MethodID,
                                    _eyeRenderPose[eye].Orientation.x,
                                    _eyeRenderPose[eye].Orientation.y,
                                    _eyeRenderPose[eye].Orientation.z,
                                    _eyeRenderPose[eye].Orientation.w,
                                    _eyeRenderPose[eye].Position.x,
                                    _eyeRenderPose[eye].Position.y,
                                    _eyeRenderPose[eye].Position.z);

    return jposef;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getMatrix4fProjection(
    JNIEnv *env, 
    jobject, 
    jfloat EyeFovPortUpTan,
    jfloat EyeFovPortDownTan,
    jfloat EyeFovPortLeftTan,
    jfloat EyeFovPortRightTan,
    jfloat nearClip,
    jfloat farClip)
{
    if (!_initialised)
        return 0;

    if (!_renderConfigured)
    {
        printf("getMatrix4fProjection() - ERROR: Render config not set!");
        return 0;
    }

    FovPort fov;
    fov.UpTan = EyeFovPortUpTan;
    fov.DownTan = EyeFovPortDownTan;
    fov.LeftTan = EyeFovPortLeftTan;
    fov.RightTan = EyeFovPortRightTan;

    Matrix4f proj = ovrMatrix4f_Projection(fov, nearClip, farClip, true); // true = RH for OGL

    jobject jproj = env->NewObject(matrix4f_Class, matrix4f_constructor_MethodID,
                                   proj.M[0][0], proj.M[0][1], proj.M[0][2], proj.M[0][3],
                                   proj.M[1][0], proj.M[1][1], proj.M[1][2], proj.M[1][3],
                                   proj.M[2][0], proj.M[2][1], proj.M[2][2], proj.M[2][3],
                                   proj.M[3][0], proj.M[3][1], proj.M[3][2], proj.M[3][3]
                                   );

    return jproj;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1endEyeRender(JNIEnv *env, jobject, jint Eye)
{
    if (!_initialised)
        return;

    if (!_renderConfigured)
    {
        printf("endEyeRender() - ERROR: Render config not set!");
        return;
    }

    ovrEyeType eye = ovrEye_Left;
    if (Eye > 0)
        eye = ovrEye_Right;

    ovrHmd_EndEyeRender(_pHmd, eye, _eyeRenderPose[eye], &_EyeTexture[eye].Texture);
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1endFrame(JNIEnv *env, jobject)
{
    if (!_initialised)
        return;

    if (!_renderConfigured)
    {
        printf("endFrame() - ERROR: Render config not set!");
        return;
    }

    // Let OVR do distortion rendering, Present and flush/sync
    ovrHmd_EndFrame(_pHmd);
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1convertQuatToEuler
  (JNIEnv *env, jobject, jfloat quaternion_x, jfloat quaternion_y, jfloat quaternion_z, jfloat quaternion_w, jfloat scale, jint hand, jint rotationDir)
{
	Quatf quat(quaternion_x, quaternion_y, quaternion_z, quaternion_w);
    ovrVector3f euler;
	
	if (scale != 1.0f)
		quat = quat.PowNormalized(scale);

	if (hand == 1)
	{
		if (rotationDir == 1)
            quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CCW, Handed_R>(&euler.y, &euler.x, &euler.z);
		else
			quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CW, Handed_R>(&euler.y, &euler.x, &euler.z);
	}
	else
	{
		if (rotationDir == 1)
			quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CCW, Handed_L>(&euler.y, &euler.x, &euler.z);
		else
			quat.GetEulerAngles<Axis_Y, Axis_X, Axis_Z, Rotate_CW, Handed_L>(&euler.y, &euler.x, &euler.z);
	}

    // Cache JNI objects here to prevent the need for initialisation
    if (sizei_Class == NULL)
	{
		jclass localClass = env->FindClass("de/fruitfly/ovr/struct/Sizei");
		sizei_Class = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);
	}

	if (sizei_constructor_MethodID == NULL)
	{
		sizei_constructor_MethodID = env->GetMethodID(sizei_Class, 
			"<init>", "("
                      "II"
                      ")V");
	}

    jobject jvector3f = env->NewObject(vector3f_Class, vector3f_constructor_MethodID,
                                       euler.x,
                                       euler.y,
                                       euler.z
                                       );

	return jvector3f;
}

void ResetRenderConfig()
{
    if (_initialised)
    {
        ovrHmd_ConfigureRendering(_pHmd, 0, 0, 0, 0, 0);
    }

    // Reset texture data
	_EyeTexture[0].OGL.Header.API                   = ovrRenderAPI_None;
    _EyeTexture[0].OGL.Header.TextureSize.w         = 0;
	_EyeTexture[0].OGL.Header.TextureSize.h         = 0;
    _EyeTexture[0].OGL.Header.RenderViewport.Pos.x  = 0;
    _EyeTexture[0].OGL.Header.RenderViewport.Pos.y  = 0;
    _EyeTexture[0].OGL.Header.RenderViewport.Size.w = 0;
    _EyeTexture[0].OGL.Header.RenderViewport.Size.h = 0;
	_EyeTexture[0].OGL.TexId                        = 0;
    _EyeTexture[1] = _EyeTexture[0];

    _renderConfigured = false;
}

bool CreateHmdAndStartSensor(int hmdIndex)
{
    _pHmd = 0;
    _pHmdDesc.reset();

    bool result = false;

	// Get HMD
	_pHmd = ovrHmd_Create(_hmdIndex);

	if (_pHmd == 0)
	{
		// Create debug DK2(!)
        _hmdIndex = -1;
		_pHmd = ovrHmd_CreateDebug(ovrHmd_DK2);
		printf("No Oculus Rift devices found, creating dummy DK2 device...\n");
	}
	else
	{
		printf("Oculus Rift device(s) found!\n");
	}

	if (_pHmd)
	{
		_pHmdDesc.reset(new ovrHmdDesc);
		ovrHmd_GetDesc(_pHmd, _pHmdDesc.get());

		// Log description
		LogHmdDesc(_pHmdDesc);

		// Start sensor
		ovrBool sensorResult = ovrHmd_StartSensor(_pHmd,
			ovrHmdCap_Orientation | ovrHmdCap_YawCorrection | ovrHmdCap_Position | ovrHmdCap_LowPersistence | ovrHmdCap_LatencyTest,
			0);

		if (sensorResult)
		{
			// Initialised successfully
			printf("Oculus Rift Device Interface initialized.\n");
            result = true;
		}
	}

    return result;
}

void DEBUGLOG(std::string s)
{
	if (LogDebug)
		printf("DEBUG: %s\n", s.c_str());
}

void ClearException(JNIEnv *env)
{
    env->ExceptionClear();
}

void PrintNewObjectException(JNIEnv *env, std::string objectName)
{
    printf("Failed to create object '%s'", objectName.c_str());
    env->ExceptionDescribe();
    env->ExceptionClear();
}

void LogHmdDesc(std::auto_ptr<ovrHmdDesc>& pHmdDesc)
{
	// Chuck out some basic HMD info
	printf(" ProductName: %s\n", pHmdDesc->ProductName);
	printf(" Manufacturer: %s\n", pHmdDesc->Manufacturer);
	printf(" Resolution: %d X %d\n", pHmdDesc->Resolution.w, pHmdDesc->Resolution.h);
}

void Reset()
{
	if (_initialised)
	{
        // Reset render config
        ResetRenderConfig();

		// Stop sensor
		if (_pHmd)
			ovrHmd_StopSensor(_pHmd);

		// Cleanup HMD
		if (_pHmd)
			ovrHmd_Destroy(_pHmd);

		// Shutdown LibOVR
		ovr_Shutdown();
	}

	_pHmd = 0;
	_pHmdDesc.reset();
	_hmdIndex = -1;

    _eyeRenderPose[0].Orientation.x = 0.0;
    _eyeRenderPose[0].Orientation.y = 0.0;
    _eyeRenderPose[0].Orientation.z = 0.0;
    _eyeRenderPose[0].Orientation.w = 0.0;
    _eyeRenderPose[0].Position.x = 0.0;
    _eyeRenderPose[0].Position.y = 0.0;
    _eyeRenderPose[0].Position.z = 0.0;
    _eyeRenderPose[1] = _eyeRenderPose[0];

	_initialised = false;
}

bool CacheJNIGlobals(JNIEnv *env)
{
    if (!LookupJNIGlobal(env,
                         sizei_Class,
                         "de/fruitfly/ovr/structs/Sizei",
                         sizei_constructor_MethodID,
                         "(II)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         hmdDesc_Class,
                         "de/fruitfly/ovr/structs/HmdDesc",
                         hmdDesc_constructor_MethodID,
                         "(ILjava/lang/String;Ljava/lang/String;IIIIIIFFFFFFFFFFFFFFFFIILjava/lang/String;J)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         sensorState_Class,
                         "de/fruitfly/ovr/structs/SensorState",
                         sensorState_constructor_MethodID,
                         "(FFFFFFFFFFFFFFFFFFFDFFFFFFFFFFFFFFFFFFFDFI)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         vector3f_Class,
                         "de/fruitfly/ovr/structs/Vector3f",
                         vector3f_constructor_MethodID,
                         "(FFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         eyeRenderParams_Class,
                         "de/fruitfly/ovr/EyeRenderParams",
                         eyeRenderParams_constructor_MethodID,
                         "(IIIIIIIFFFFIIIIFFFFFIIIIIIIFFFFIIIIFFFFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         posef_Class,
                         "de/fruitfly/ovr/structs/Posef",
                         posef_constructor_MethodID,
                         "(FFFFFFF)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         frameTiming_Class,
                         "de/fruitfly/ovr/structs/FrameTiming",
                         frameTiming_constructor_MethodID,
                         "(FDDDDDD)V"))
    {
        return false;
    }

    if (!LookupJNIGlobal(env,
                         matrix4f_Class,
                         "de/fruitfly/ovr/structs/Matrix4f",
                         matrix4f_constructor_MethodID,
                         "(FFFFFFFFFFFFFFFF)V"))
    {
        return false;
    }

    return true;
}

bool LookupJNIGlobal(JNIEnv *env,
                     jclass& clazz,
                     std::string className,
                     jmethodID& method,
                     std::string constructorSignature)
{
    if (clazz == NULL)
	{
		jclass localClass = env->FindClass(className.c_str());
        if (localClass == 0)
        {
            printf("Failed to find class '%s'", className.c_str());
            return false;
        }

		clazz = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);
	}

	if (method == NULL)
	{
		method = env->GetMethodID(clazz, "<init>", constructorSignature.c_str());
        if (method == 0)
        {
            printf("Failed to find constuctor method for class '%s' with signature: %s", 
                className.c_str(), constructorSignature.c_str());
            return false;
        }
	}

    return true;
}
