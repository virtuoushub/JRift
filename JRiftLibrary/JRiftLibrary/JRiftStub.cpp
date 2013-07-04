#include "de_fruitfly_ovr_OculusRift.h"

float _ipd;

static jclass eyeRenderParams_Class;
static jmethodID eyeRenderParams_constructor_MethodID;

JNIEXPORT jboolean JNICALL Java_de_fruitfly_ovr_OculusRift_initSubsystem(JNIEnv *env, jobject jobj) 
{
	return false;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift_destroySubsystem(JNIEnv *env, jobject jobj) 
{
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1setPredictionEnabled(JNIEnv *, jobject, jfloat delta, jboolean enable) 
{
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift_pollSubsystem(JNIEnv *, jobject) {
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1getHResolution(JNIEnv *, jobject) {
	return 0;
}

JNIEXPORT jint JNICALL Java_de_fruitfly_ovr_OculusRift__1getVResolution(JNIEnv *, jobject){
	return 0;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getHScreenSize(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getVScreenSize(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getVScreenCenter(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyeToScreenDistance(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getLensSeparationDistance(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getInterpupillaryDistance(JNIEnv *, jobject){
	return _ipd;
}

JNIEXPORT void JNICALL Java_de_fruitfly_ovr_OculusRift__1setInterpupillaryDistance(JNIEnv *, jobject, jfloat ipd) {
	_ipd = ipd;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK0(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK1(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK2(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getDistortionK3(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection0(JNIEnv *, jobject){
	return 1.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection1(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection2(JNIEnv *, jobject){
	return 1.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getChromaAbCorrection3(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getYaw(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getPitch(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jfloat JNICALL Java_de_fruitfly_ovr_OculusRift__1getRoll(JNIEnv *, jobject){
	return 0.0f;
}

JNIEXPORT jobject JNICALL Java_de_fruitfly_ovr_OculusRift__1getEyeRenderParams(
   JNIEnv *env, 
   jobject thisObj,
   jint viewportX,
   jint viewportY,
   jint viewportWidth,
   jint viewportHeight,
   jfloat clipNear,
   jfloat clipFar
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
	
	return NULL;
}
