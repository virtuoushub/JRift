package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.structs.*;

import java.io.File;

public class OculusRift //implements IOculusRift
{
	private boolean initialized = false;

    public final String VERSION = "0.3.1.1";

	private HmdDesc hmdDesc = new HmdDesc();
    private SensorState sensorState = new SensorState();

    public static final int ROTATE_CCW = 1;
    public static final int ROTATE_CW  = -1;
    public static final int HANDED_R = 1;
    public static final int HANDED_L = -1;

    public String _initSummary = "Not initialised";

	private static boolean libraryLoaded = false;
	
	public OculusRift()
    {
        resetHMDInfo();
	}

    private void resetHMDInfo()
    {
        hmdDesc = new HmdDesc();
        sensorState = new SensorState();
    }

	public String getInitializationStatus()
	{
		return _initSummary;
	}

    public String getVersion()
    {
        return VERSION;
    }

	public boolean init( File nativeDir )
	{
		OculusRift.LoadLibrary();
		return init();
	}

	public boolean init() {

        _initSummary = "Last initialisation attempt failed";

        if (!initialized)
		    initialized = _initSubsystem();

        if (initialized)
        {
            hmdDesc = _getHmdDesc();
            _initSummary = "OK";
        }
        else
        {
            resetHMDInfo();
        }
		
		return initialized;
	}
	
	public boolean isInitialized()
    {
		return initialized;
	}

    public void destroy()
    {
        if (initialized)
        {
            _destroySubsystem();
        }

        _initSummary = "Not initialised";
        initialized = false;
    }

    public boolean getNextHmd()
    {
        return _getNextHmd();
    }

    public HmdDesc getHmdDesc()
    {
        return hmdDesc;
    }

    public SensorState poll(double futureDelta)
    {
        if (initialized)
        {
            // Get sensor state as of now
            sensorState = _getSensorState(futureDelta);
        }

        return sensorState;
    }

    public SensorState getLastSensorState()
    {
        return sensorState;
    }

    public FovTextureInfo getFovTextureSize(float renderScaleFactor)
    {
        return _getFovTextureSize(renderScaleFactor);
    }

    public EyeRenderParams configureRendering(Sizei InTextureSize,
                                              Sizei OutTextureSize,
                                              GLConfig glConfig,
                                              boolean VSyncEnabled,
                                              boolean useChromaticAbCorrection,
                                              boolean useTimewarp,
                                              boolean useVignette)
    {
        return _configureRendering(InTextureSize.w,
                InTextureSize.h,
                OutTextureSize.w,
                OutTextureSize.h,
                glConfig.TexId,
                glConfig.Window,
                glConfig.Display,
                true,//TODO: VSyncEnabled
                0,            // TODO: Support multisample?
                true,//useChromaticAbCorrection,
                true,//useTimewarp,
                true);//useVignette);   // TODO: Reenable when these settings aren't broken in the Oculus preview SDK
    }

    public FrameTiming beginFrameGetTiming()
    {
        return _beginFrame(0);
    }

    public Posef beginEyeRender(EyeType eye)
    {
        return _beginEyeRender(eye.value());
    }

    public Matrix4f getMatrix4fProjection(FovPort fov,
                                          float nearClip,
                                          float farClip)
    {
        return _getMatrix4fProjection(fov.UpTan,
                                      fov.DownTan,
                                      fov.LeftTan,
                                      fov.RightTan,
                                      nearClip,
                                      farClip);
    }

    public void endEyeRender(EyeType eye)
    {
        _endEyeRender(eye.value());
    }

    public void endFrame()
    {
        _endFrame();
    }

    public static Vector3f getEulerAngles(Quatf quat, float scale, int hand, int rotationDir)
    {
        return getEulerAngles(quat.x, quat.y, quat.z, quat.w, scale, hand, rotationDir);
    }

    public static Vector3f getEulerAngles(float x, float y, float z, float w, float scale, int hand, int rotationDir)
    {
        if( !libraryLoaded )
            return null;

        Vector3f eulerAngles = _convertQuatToEuler(x, y, z, w, scale, hand, rotationDir);
        eulerAngles.x = (float)Math.toDegrees(eulerAngles.x);
        eulerAngles.y = (float)Math.toDegrees(eulerAngles.y);
        eulerAngles.z = (float)Math.toDegrees(eulerAngles.z);
        return eulerAngles;
    }

    // Native declarations

	protected native boolean         _initSubsystem();
    protected native void            _destroySubsystem();

    protected native boolean         _getNextHmd();
    protected native HmdDesc         _getHmdDesc();

    protected native SensorState     _getSensorState(double timeFromNow);

    protected native FovTextureInfo  _getFovTextureSize(float RenderScaleFactor);
    protected native EyeRenderParams _configureRendering(int InTextureWidth,
                                                         int InTextureHeight,
                                                         int OutTextureWidth,
                                                         int OutTextureHeight,
                                                         int InTextureGLId,
                                                         long pWindow,
                                                         long pDisplay,
                                                         boolean VSyncEnabled,
                                                         int MultiSample,
                                                         boolean useChromaticAbCorrection,
                                                         boolean useTimewarp,
                                                         boolean useVignette);

    protected native FrameTiming     _beginFrame(int frameIndex);
    protected native Posef           _beginEyeRender(int eye);
    protected native Matrix4f        _getMatrix4fProjection(float EyeFovPortUpTan,
                                                            float EyeFovPortDownTan,
                                                            float EyeFovPortLeftTan,
                                                            float EyeFovPortRightTan,
                                                            float nearClip,
                                                            float farClip);
    protected native void            _endEyeRender(int eye);
    protected native void            _endFrame();

    protected native static Vector3f _convertQuatToEuler(float quatx,
                                                         float quaty,
                                                         float quatz,
                                                         float quatw,
                                                         float scale,
                                                         int hand,
                                                         int rotationDir);

    public static void LoadLibrary()
    {
        if( libraryLoaded ) return;
        String os = System.getProperty("os.name");
        boolean is64bit = System.getProperty("sun.arch.data.model").equalsIgnoreCase("64");

        //Launcher takes care of extracting natives
        if( is64bit )
        {
            System.loadLibrary("JRiftLibrary64");
            System.out.println("Loaded JRift native library (64bit)");
        }
        else
        {
            System.loadLibrary("JRiftLibrary");
            System.out.println("Loaded JRift native library (32bit)");
        }

        libraryLoaded = true;
    }

    public static void main(String[] args)
    {
        OculusRift.LoadLibrary();
        OculusRift or = new OculusRift();

        if (!or.init())
        {
            System.out.println("Failed to initialise OR lib");
            return;
        }

        HmdDesc hmdDesc = or.getHmdDesc();
        System.out.println(hmdDesc.toString());

        FovTextureInfo recommendedFovTextureSize = or.getFovTextureSize(1.0f);
        System.out.println("Render target size: " + recommendedFovTextureSize.Res.w + "x" + recommendedFovTextureSize.Res.h);

        while (or.isInitialized()) {
            or.poll(0.0d);
            SensorState state = or.getLastSensorState();
            Vector3f euler = or.getEulerAngles(state.Predicted.Pose.Orientation,
                                               1.0f,
                                               OculusRift.HANDED_L,
                                               OculusRift.ROTATE_CCW);
            Vector3f pos = state.Predicted.Pose.Position;

            System.out.println("Yaw: " + euler.y + " Pitch: " + euler.x + " Roll: " + euler.z + " PosX: " + pos.x+ " PosY: " + pos.y + " PosZ: " + pos.z);

            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        or.destroy();
    }

//    public UserProfileData getUserProfile()
//    {
//        if (!isInitialized())
//            return null;
//
//        return _getUserProfileData();
//    }
//
//    public boolean loadUserProfile(String profileName)
//    {
//        if (!isInitialized())
//            return false;
//
//        return _loadUserProfile(profileName);
//    }
//
//    public String[] getUserProfiles()
//    {
//        if (!isInitialized())
//            return null;
//
//        return _getUserProfiles();
//    }
//
//    public float[] latencyTesterDisplayScreenColor()
//    {
//        return _latencyTesterDisplayScreenColor();
//    }
//
//    public String latencyTesterGetResultsString()
//    {
//        return _latencyTesterGetResultsString();
//    }

//    protected native UserProfileData _getUserProfileData();
//    protected native boolean _loadUserProfile(String profileName);
//    protected native String[] _getUserProfiles();
//
//    protected native void    _latencyTesterProcessInputs();
//    protected native float[] _latencyTesterDisplayScreenColor();
//    protected native String  _latencyTesterGetResultsString();
}
