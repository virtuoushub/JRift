package de.fruitfly.ovr;

import de.fruitfly.ovr.enums.Axis;
import de.fruitfly.ovr.enums.EyeType;
import de.fruitfly.ovr.enums.HandedSystem;
import de.fruitfly.ovr.enums.RotateDirection;
import de.fruitfly.ovr.structs.*;

import java.io.File;

public class OculusRift //implements IOculusRift
{
	private boolean initialized = false;

    public final String VERSION = "0.3.2.1";

	private HmdDesc hmdDesc = new HmdDesc();
    private SensorState sensorState = new SensorState();

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

    public void resetSensor()
    {
        _resetSensor();
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
                VSyncEnabled,
                0,            // TODO: Support multisample?
                useChromaticAbCorrection,
                useTimewarp,
                useVignette);
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

    public static EulerOrient getEulerAnglesDeg(Quatf quat,
                                                float scale,
                                                Axis rotationAxis1,
                                                Axis rotationAxis2,
                                                Axis rotationAxis3,
                                                HandedSystem hand,
                                                RotateDirection rotationDir)
    {
        if( !libraryLoaded )
            return null;

        EulerOrient eulerAngles = _convertQuatToEuler(quat.x, quat.y, quat.z, quat.w, scale,
                rotationAxis1.value(),
                rotationAxis2.value(),
                rotationAxis3.value(),
                hand.value(),
                rotationDir.value());

        eulerAngles.yaw = (float)Math.toDegrees(eulerAngles.yaw);
        eulerAngles.pitch = (float)Math.toDegrees(eulerAngles.pitch);
        eulerAngles.roll = (float)Math.toDegrees(eulerAngles.roll);

        return eulerAngles;
    }

    // Native declarations

	protected native boolean         _initSubsystem();
    protected native void            _destroySubsystem();

    protected native boolean         _getNextHmd();
    protected native HmdDesc         _getHmdDesc();

    protected native SensorState     _getSensorState(double timeFromNow);
    protected native void            _resetSensor();

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

    protected native static EulerOrient _convertQuatToEuler(float quatx,
                                                            float quaty,
                                                            float quatz,
                                                            float quatw,
                                                            float scale,
                                                            int rot1,
                                                            int rot2,
                                                            int rot3,
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

        while (or.isInitialized())
        {
            or.poll(0.0d);
            SensorState state = or.getLastSensorState();
            EulerOrient euler = or.getEulerAnglesDeg(state.Predicted.Pose.Orientation,
                                               1.0f,
                                               Axis.Axis_Y,
                                               Axis.Axis_X,
                                               Axis.Axis_Z,
                                               HandedSystem.Handed_L,
                                               RotateDirection.Rotate_CCW);

            Vector3f pos = state.Predicted.Pose.Position;

            System.out.println("Yaw: " + euler.yaw + " Pitch: " + euler.pitch + " Roll: " + euler.roll + " PosX: " + pos.x+ " PosY: " + pos.y + " PosZ: " + pos.z);

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
