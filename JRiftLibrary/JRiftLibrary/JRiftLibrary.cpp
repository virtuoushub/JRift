#include "de_fruitfly_ovr_OculusRift.h"
#include "OVR.h"	

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

using namespace OVR;

typedef enum AspectCorrectionType
{
	CORRECTION_NONE          = 0,
	CORRECTION_16_9_TO_16_10 = 1,
	CORRECTION_16_10_TO_16_9 = 2,
	CORRECTION_AUTO          = 3,
};

Ptr<DeviceManager>	pManager;
Ptr<HMDDevice>		pHMD;
Ptr<LatencyTestDevice> pLatencyTester;
Ptr<SensorDevice>	pSensor;
Ptr<Profile>        pUserProfile;
Ptr<ProfileManager> pm;
std::auto_ptr<SensorFusion> pFusionResult; // Work-around for 0.2.5 SDK issue
OVR::ProfileType    CurrentProfileType = OVR::Profile_Unknown;
bool                IsDefaultProfile = true;

Util::LatencyTest    LatencyUtil;

HMDInfo			Info;

bool			InfoLoaded;
bool			Initialized = false;
bool            LogInfo = false;
Quatf			quaternion;

float yaw, pitch, roll;
float _ipd;

static jclass eyeRenderParams_Class;
static jmethodID eyeRenderParams_constructor_MethodID;
static jclass magCalibrationData_Class;
static jmethodID magCalibrationData_constructor_MethodID;
static jclass UserProfileData_Class;
static jmethodID UserProfileData_constructor_MethodID;

void LOG(std::string s)
{
	if (LogInfo)
		printf("%s\n", s.c_str());
}

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1initSubsystem(JNIEnv *env, jobject jobj) 
{
	Initialized = false;

	System::Init();

	pManager = *DeviceManager::Create();
	pm = *ProfileManager::Create();
	pFusionResult.reset(new SensorFusion); 

	_ipd = 0.0635f; // Default
	Info.InterpupillaryDistance = _ipd;

	pHMD = *pManager->EnumerateDevices<HMDDevice>().CreateDevice();
	pLatencyTester = *pManager->EnumerateDevices<LatencyTestDevice>().CreateDevice();

	if (pHMD && pFusionResult.get() != 0) {
		printf("Oculus Rift Device Interface created.\n");
		pUserProfile = pHMD->GetProfile(); // Get the current default profile
		if (pUserProfile != 0)
			CurrentProfileType = pUserProfile->Type;  // Get the current default profile type
		InfoLoaded = pHMD->GetDeviceInfo(&Info);
		pSensor = *pHMD->GetSensor();
		pFusionResult->AttachToSensor(pSensor);
		Initialized = InfoLoaded && pSensor;
		printf("Oculus Rift Device Interface initialized.\n");
	}
	else {
		printf("Unable to create Oculus Rift device interface.\n");
	}

	if (Initialized && pLatencyTester)
	{
		LatencyUtil.SetDevice(pLatencyTester);
	}

	if (InfoLoaded) {

		_ipd = Info.InterpupillaryDistance;

		printf(" DisplayDeviceName: %s\n", Info.DisplayDeviceName);
		printf(" ProductName: %s\n", Info.ProductName);
		printf(" Manufacturer: %s\n", Info.Manufacturer);
		printf(" Version: %d\n", Info.Version);
		printf(" HResolution: %i\n", Info.HResolution);
		printf(" VResolution: %i\n", Info.VResolution);
		printf(" HScreenSize: %f\n", Info.HScreenSize);
		printf(" VScreenSize: %f\n", Info.VScreenSize);
		printf(" VScreenCenter: %f\n", Info.VScreenCenter);
		printf(" EyeToScreenDistance: %f\n", Info.EyeToScreenDistance);
		printf(" LensSeparationDistance: %f\n", Info.LensSeparationDistance);
        printf(" InterpupillaryDistance: %f\n", Info.InterpupillaryDistance);
        printf(" DistortionK[0]: %f\n", Info.DistortionK[0]);
        printf(" DistortionK[1]: %f\n", Info.DistortionK[1]);
		printf(" DistortionK[2]: %f\n", Info.DistortionK[2]);
		printf(" DistortionK[3]: %f\n", Info.DistortionK[3]);
		printf(" ChromaticAb[0]: %f\n", Info.ChromaAbCorrection[0]);
        printf(" ChromaticAb[1]: %f\n", Info.ChromaAbCorrection[1]);
		printf(" ChromaticAb[2]: %f\n", Info.ChromaAbCorrection[2]);
		printf(" ChromaticAb[3]: %f\n", Info.ChromaAbCorrection[3]);
	}
	if( !Initialized )
	{
		Reset();
	}

	return Initialized;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1destroySubsystem(JNIEnv *env, jobject jobj) 
{
	printf("Destroying Oculus Rift device interface.\n");	
		
	if (Initialized)
	{
		Reset();
	}
}

void Reset()
{
	pSensor.Clear();
	pManager.Clear();
	pHMD.Clear(); // Ensure HMDDevice is also cleared
	pm.Clear();
	if (pUserProfile != 0)
		pUserProfile.Clear();
	if (pFusionResult.get() != 0)
		pFusionResult->AttachToSensor(0); // Workaround for SDK 0.2.4(a) System::Destroy() hang
		
	CurrentProfileType = OVR::Profile_Unknown;
	IsDefaultProfile   = true;

	LatencyUtil.SetDevice(NULL);
    pLatencyTester.Clear();
	pFusionResult.reset();
	
	System::Destroy();

	Initialized = false;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1setPredictionEnabled(JNIEnv *, jobject, jfloat delta, jboolean enable) 
{
	if (Initialized)
	{
		pFusionResult->SetPrediction(delta, enable == 1 ? true : false);
	}
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1pollSubsystem(JNIEnv *, jobject) {
	if (!Initialized) return;
	if (!pSensor) return;

	bool isPredictionEnabled = pFusionResult->IsPredictionEnabled();
	if (isPredictionEnabled)
		quaternion = pFusionResult->GetPredictedOrientation();
	else
		quaternion = pFusionResult->GetOrientation();

	quaternion.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1getHResolution(JNIEnv *, jobject) {
	if (!Initialized) return 0;
	return Info.HResolution;
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1getVResolution(JNIEnv *, jobject){
	if (!Initialized) return 0;
	return Info.VResolution;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getHScreenSize(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.HScreenSize;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getVScreenSize(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.VScreenSize;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getVScreenCenter(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.VScreenCenter;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyeToScreenDistance(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.EyeToScreenDistance;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getLensSeparationDistance(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.LensSeparationDistance;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getInterpupillaryDistance(JNIEnv *, jobject){
	if (Initialized)
	{
		return Info.InterpupillaryDistance;
	}

	return _ipd;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1setInterpupillaryDistance(JNIEnv *, jobject, jfloat ipd) {
	if (Initialized)
	{
		Info.InterpupillaryDistance = ipd;
	}

	_ipd = ipd;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK0(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.DistortionK[0];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK1(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.DistortionK[1];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK2(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.DistortionK[2];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK3(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.DistortionK[3];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection0(JNIEnv *, jobject){
	if (!Initialized) return 1.0f;
	return Info.ChromaAbCorrection[0];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection1(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.ChromaAbCorrection[1];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection2(JNIEnv *, jobject){
	if (!Initialized) return 1.0f;
	return Info.ChromaAbCorrection[2];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection3(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return Info.ChromaAbCorrection[3];
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getYaw(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return yaw;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getPitch(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return pitch;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getRoll(JNIEnv *, jobject){
	if (!Initialized) return 0.0f;
	return roll;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyeRenderParams(
   JNIEnv *env, 
   jobject thisObj,
   jint viewportX,
   jint viewportY,
   jint viewportWidth,
   jint viewportHeight,
   jfloat clipNear,
   jfloat clipFar,
   jfloat eyeToScreenDistanceScaleFactor,
   jfloat lensSeparationScaleFactor,
   jfloat distortionFitX,
   jfloat distortionFitY,
   jint aspectCorrectionMode
)
{
	if (eyeRenderParams_Class == NULL)
	{
		jclass localEyeRenderParamClass = env->FindClass("de/fruitfly/ovr/EyeRenderParams");
		eyeRenderParams_Class = (jclass)env->NewGlobalRef(localEyeRenderParamClass);
		env->DeleteLocalRef(localEyeRenderParamClass);
	}

	if (eyeRenderParams_constructor_MethodID == NULL)
	{
		eyeRenderParams_constructor_MethodID = env->GetMethodID(eyeRenderParams_Class, 
			"<init>", "(FF"
			          "IIII"
					  "FFFFFFFFFFFFFFFF"
					  "FFFFFFFFFFFFFFFF"
					  "IIII"
					  "FFFFFFFFFFFFFFFF"
					  "FFFFFFFFFFFFFFFF)V");
	}
	
	Util::Render::Viewport viewPort;
	viewPort.x = viewportX;
	viewPort.y = viewportY;
	viewPort.w = viewportWidth;
	viewPort.h = viewportHeight;

	Util::Render::StereoConfig stereo;
	HMDInfo hmd = Info;

	// Set lens separation distance
	hmd.LensSeparationDistance *= lensSeparationScaleFactor;

	// Use defaults (Oculus Rift DK1 parameters) if not initialised.
	if (Initialized)
	{
		stereo.SetHMDInfo(hmd);
	}

	stereo.SetIPD(_ipd); // Set IPD manually
	stereo.SetEyeToScreenDistanceScaleFactor(eyeToScreenDistanceScaleFactor); // FOV adjustment
	stereo.SetStereoMode(Util::Render::Stereo_LeftRight_Multipass);
	stereo.SetFullViewport(viewPort);
	stereo.SetDistortionFitPointVP(distortionFitX, distortionFitY); // Defaults to -1.0f, 0.0f. 0.0f, 0.0f is 'No fit'.

	// Calculate aspect ratio multiplier to correct for incorrect display aspect ratio
	switch(aspectCorrectionMode)
	{
		case CORRECTION_AUTO:
		{
			if (Initialized)
			{
				double HmdRequiredAspect = (double)Info.HResolution / (double)Info.VResolution;
				double SuppliedAspect = (double)viewPort.w / (double)viewPort.h;
				float AspectMultiplier = (float)(HmdRequiredAspect / SuppliedAspect);
				stereo.SetAspectMultiplier(AspectMultiplier);
			}
		}
		break;
		case CORRECTION_16_10_TO_16_9:
		{
			double aspect_16_10 = (double)16 / (double)10;
			double aspect_16_9  = (double)16 / (double)9;
			float AspectMultiplier = (float)(aspect_16_9 / aspect_16_10);
			stereo.SetAspectMultiplier(AspectMultiplier);
		}
		break;
		case CORRECTION_16_9_TO_16_10:
		{
			double aspect_16_10 = (double)16 / (double)10;
			double aspect_16_9  = (double)16 / (double)9;
			float AspectMultiplier = (float)(aspect_16_10 / aspect_16_9);
			stereo.SetAspectMultiplier(AspectMultiplier);
		}
		break;
	}

	// Set custom clip plane
	stereo.SetClipNear(clipNear);
	stereo.SetClipFar(clipFar);

	float renderScale = stereo.GetDistortionScale();

	Util::Render::StereoEyeParams leftEye = stereo.GetEyeRenderParams(Util::Render::StereoEye_Left);
	Util::Render::StereoEyeParams rightEye = stereo.GetEyeRenderParams(Util::Render::StereoEye_Right);

	Util::Render::Viewport leftViewPort   = leftEye.VP;
	Matrix4f               leftProjection = leftEye.Projection;
	Matrix4f               leftViewAdjust = leftEye.ViewAdjust;
	
	Util::Render::Viewport rightViewPort   = rightEye.VP;
	Matrix4f               rightProjection = rightEye.Projection;
	Matrix4f               rightViewAdjust = rightEye.ViewAdjust;

	const Util::Render::DistortionConfig DistortionConfig = stereo.GetDistortionConfig();
	float yFovDegrees = stereo.GetYFOVDegrees();
	float aspect = stereo.GetAspect();
	float XCenterOffset = DistortionConfig.XCenterOffset; 

	jobject eyeRenderParams = env->NewObject(eyeRenderParams_Class, eyeRenderParams_constructor_MethodID,
											 renderScale,
											 XCenterOffset,
											 leftViewPort.x, leftViewPort.y, leftViewPort.w, leftViewPort.h,
											 leftProjection.M[0][0], leftProjection.M[0][1], leftProjection.M[0][2], leftProjection.M[0][3],
											 leftProjection.M[1][0], leftProjection.M[1][1], leftProjection.M[1][2], leftProjection.M[1][3],
											 leftProjection.M[2][0], leftProjection.M[2][1], leftProjection.M[2][2], leftProjection.M[2][3],
											 leftProjection.M[3][0], leftProjection.M[3][1], leftProjection.M[3][2], leftProjection.M[3][3],
											 leftViewAdjust.M[0][0], leftViewAdjust.M[0][1], leftViewAdjust.M[0][2], leftViewAdjust.M[0][3],
											 leftViewAdjust.M[1][0], leftViewAdjust.M[1][1], leftViewAdjust.M[1][2], leftViewAdjust.M[1][3],
											 leftViewAdjust.M[2][0], leftViewAdjust.M[2][1], leftViewAdjust.M[2][2], leftViewAdjust.M[2][3],
											 leftViewAdjust.M[3][0], leftViewAdjust.M[3][1], leftViewAdjust.M[3][2], leftViewAdjust.M[3][3],
											 rightViewPort.x, rightViewPort.y, rightViewPort.w, rightViewPort.h,
											 rightProjection.M[0][0], rightProjection.M[0][1], rightProjection.M[0][2], rightProjection.M[0][3],
											 rightProjection.M[1][0], rightProjection.M[1][1], rightProjection.M[1][2], rightProjection.M[1][3],
											 rightProjection.M[2][0], rightProjection.M[2][1], rightProjection.M[2][2], rightProjection.M[2][3],
											 rightProjection.M[3][0], rightProjection.M[3][1], rightProjection.M[3][2], rightProjection.M[3][3],
											 rightViewAdjust.M[0][0], rightViewAdjust.M[0][1], rightViewAdjust.M[0][2], rightViewAdjust.M[0][3],
											 rightViewAdjust.M[1][0], rightViewAdjust.M[1][1], rightViewAdjust.M[1][2], rightViewAdjust.M[1][3],
											 rightViewAdjust.M[2][0], rightViewAdjust.M[2][1], rightViewAdjust.M[2][2], rightViewAdjust.M[2][3],
											 rightViewAdjust.M[3][0], rightViewAdjust.M[3][1], rightViewAdjust.M[3][2], rightViewAdjust.M[3][3]
											);
												
	return eyeRenderParams;
}

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1isCalibrated
  (JNIEnv *, jobject)
{
	if (!Initialized) return false;

	return pFusionResult->HasMagCalibration() && pFusionResult->IsYawCorrectionEnabled();
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1reset
  (JNIEnv *, jobject)
{
	if (!Initialized) return;

	// Reset the sensor data and calibration settings. Also resets Yaw position
	// which will be noticable if yaw drift has occurred.

	pFusionResult->Reset();
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getUserProfileData(
   JNIEnv *env, jobject)
{
	if (!Initialized) return 0;

	if (!pUserProfile) 
		pUserProfile = pm->GetDeviceDefaultProfile(CurrentProfileType);

	if (!pUserProfile) return 0;

	CurrentProfileType = pUserProfile->Type;

	if (UserProfileData_Class == NULL)
	{
		jclass localUserProfileData_Class = env->FindClass("de/fruitfly/ovr/UserProfileData");
		UserProfileData_Class = (jclass)env->NewGlobalRef(localUserProfileData_Class);
		env->DeleteLocalRef(localUserProfileData_Class);
	}

	if (UserProfileData_constructor_MethodID == NULL)
	{
		UserProfileData_constructor_MethodID = env->GetMethodID(UserProfileData_Class, 
			"<init>", "("
			          "F"
					  "F"
					  "F"
					  "I"
					  "Z"
					  "Ljava/lang/String;"
					  ")V");
	}

	float playerHeight = pUserProfile->GetPlayerHeight();
	float eyeHeight = pUserProfile->GetEyeHeight();
	float ipd = pUserProfile->GetIPD();
	int gender = pUserProfile->GetGender();
	std::string name = pUserProfile->Name;
	jstring str = env->NewStringUTF(name.c_str());

	jobject profileData = env->NewObject(UserProfileData_Class, UserProfileData_constructor_MethodID,
		playerHeight,
		eyeHeight,
		ipd,
		gender,
		IsDefaultProfile,
		str
	);

    env->DeleteLocalRef(str);

	return profileData;
}

JNIEXPORT jobjectArray JNICALL Java_de_fruitfly_ovr_OculusRift__1getUserProfiles(
   JNIEnv *env, jobject)
{
	if (!Initialized) return 0;

	if (!pUserProfile) return 0;

	int ProfileCount = pm->GetProfileCount(CurrentProfileType);
	jobjectArray profileList = (jobjectArray)env->NewObjectArray(ProfileCount, 
		                                                         env->FindClass("java/lang/String"), 
																 env->NewStringUTF(""));

	for (int i = 0; i < ProfileCount; i++)
		env->SetObjectArrayElement(profileList, i, env->NewStringUTF(pm->GetProfileName(CurrentProfileType, i)));
  
    return profileList; 
}

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift__1loadUserProfile(
   JNIEnv *env, jobject, jstring profileName)
{
	if (!Initialized) return false;

	jboolean success = true;
	jboolean isCopy = false;
	const char* cname = env->GetStringUTFChars(profileName, &isCopy);
	std::string name = cname;

	if (pm->HasProfile(CurrentProfileType, name.c_str()))
	{
		pUserProfile = pm->LoadProfile(CurrentProfileType, name.c_str());
		std::string defaultname = pm->GetDefaultProfileName(CurrentProfileType);
		if (name.compare(defaultname) == 0)
		{
			IsDefaultProfile = true;
		}
		else
		{
			IsDefaultProfile = false;	
		}
	}
	else
	{
		success = false;
	}

	env->ReleaseStringUTFChars(profileName, cname);
  
    return success; 
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1latencyTesterProcessInputs
  (JNIEnv *, jobject)
{
	if (Initialized && pLatencyTester)
		LatencyUtil.ProcessInputs();
}

JNIEXPORT jfloatArray JNICALL Java_de_fruitfly_ovr_OculusRift__1latencyTesterDisplayScreenColor
  (JNIEnv *env, jobject)
{
	Color colorToDisplay;
	bool displayColor = false;
	if (Initialized && pLatencyTester)
		displayColor = LatencyUtil.DisplayScreenColor(colorToDisplay);

	if (!displayColor)
		return 0;

	jfloatArray result = env->NewFloatArray(4);
	if (result == 0) 
		return 0;

	jfloat rgba[4];
	colorToDisplay.GetRGBA(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	env->SetFloatArrayRegion(result, 0, 4, rgba);
	return result;
}

JNIEXPORT jstring JNICALL Java_de_fruitfly_ovr_OculusRift__1latencyTesterGetResultsString
  (JNIEnv *env, jobject)
{
	if (!Initialized || !pLatencyTester)
		return 0;

	const char* results = LatencyUtil.GetResultsString();
	if (results == 0)
		return 0;

	return env->NewStringUTF(results);
}
