package de.fruitfly.ovr;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;

public class OculusRift implements IOculusRift {
	
	private boolean initialized = false;

    public final String VERSION = "1.14";

	private HMDInfo hmdInfo = new HMDInfo();

    public float rawYaw;
    public float rawPitch;
    public float rawRoll;

    public float yawAngleDegrees = 0.0f;
    public float pitchAngleDegrees = 0.0f;
    public float rollAngleDegrees = 0.0f;

    public String _initSummary = "Not initialised";

	private static boolean libraryLoaded = false;
	
	public OculusRift()
    {
        resetHMDInfo();
	}

    private void resetHMDInfo()
    {
        // Initialize "fake" default HMD values for testing without HMD plugged in.
        // These default values match those returned by the HMD.
        hmdInfo.HResolution            = 1280;
        hmdInfo.VResolution            = 800;
        hmdInfo.HScreenSize            = 0.14976f;
        hmdInfo.VScreenSize            = hmdInfo.HScreenSize / (1280.0f / 800.0f);
        hmdInfo.VScreenCenter          = hmdInfo.VScreenSize / 2f;
        hmdInfo.InterpupillaryDistance = 0.0635f;
        hmdInfo.LensSeparationDistance = 0.064f;
        hmdInfo.EyeToScreenDistance    = 0.041f;
        hmdInfo.DistortionK[0]         = 1.0f;
        hmdInfo.DistortionK[1]         = 0.22f;
        hmdInfo.DistortionK[2]         = 0.24f;
        hmdInfo.DistortionK[3]         = 0.0f;
        hmdInfo.ChromaticAb[0]         = 1.0f;
        hmdInfo.ChromaticAb[1]         = 0.0f;
        hmdInfo.ChromaticAb[2]         = 1.0f;
        hmdInfo.ChromaticAb[3]         = 0.0f;
        hmdInfo.isFakeData             = true;

        yawAngleDegrees = 0.0f;
        pitchAngleDegrees = 0.0f;
        rollAngleDegrees = 0.0f;
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
		OculusRift.LoadLibrary( nativeDir );
		return init();
	}

	public boolean init() {

        _initSummary = "Last initialisation attempt failed";

        if (!initialized)
		    initialized = _initSubsystem();

        if (initialized)
        {
            hmdInfo.HResolution = _getHResolution();
            hmdInfo.VResolution = _getVResolution();
            hmdInfo.HScreenSize = _getHScreenSize();
            hmdInfo.VScreenSize = _getVScreenSize();
            hmdInfo.VScreenCenter = _getVScreenCenter();
            hmdInfo.EyeToScreenDistance =  _getEyeToScreenDistance();
            hmdInfo.LensSeparationDistance = _getLensSeparationDistance();
            hmdInfo.InterpupillaryDistance = _getInterpupillaryDistance();
            hmdInfo.DistortionK[0] = _getDistortionK0();
            hmdInfo.DistortionK[1] = _getDistortionK1();
            hmdInfo.DistortionK[2] = _getDistortionK2();
            hmdInfo.DistortionK[3] = _getDistortionK3();
            hmdInfo.ChromaticAb[0] = _getChromaAbCorrection0();
            hmdInfo.ChromaticAb[1] = _getChromaAbCorrection1();
            hmdInfo.ChromaticAb[2] = _getChromaAbCorrection2();
            hmdInfo.ChromaticAb[3] = _getChromaAbCorrection3();
            hmdInfo.isFakeData     = false;

            _initSummary = "OK";
        }
        else
        {
            resetHMDInfo();
        }
		
		return initialized;
	}
	
	public boolean isInitialized() {
		return initialized;
	}

    public void setIPD(float ipd)
    {
        _setInterpupillaryDistance(ipd);
        hmdInfo.InterpupillaryDistance = ipd;
    }

    public float getIPD()
    {
        if (initialized)
            return _getInterpupillaryDistance();

        return hmdInfo.InterpupillaryDistance;
    }

	public void poll()
    {
		if (initialized)
        {
            _latencyTesterProcessInputs();
            _pollSubsystem();

            // Yaw
            rawYaw = _getYaw();   // -PI to PI, RH
            yawAngleDegrees = (float)Math.toDegrees(-rawYaw);

            // Pitch
            rawPitch = _getPitch();  // -PI/2 to PI/2, RH
            pitchAngleDegrees = (float)Math.toDegrees(-rawPitch);
            if (pitchAngleDegrees > MAXPITCH)
                pitchAngleDegrees = MAXPITCH;
            if (pitchAngleDegrees < -MAXPITCH)
                pitchAngleDegrees = -MAXPITCH;

            // Roll
            rawRoll = _getRoll();  // -PI to PI, RH
            rollAngleDegrees = (float)Math.toDegrees(-rawRoll);
            if (rollAngleDegrees > MAXROLL)
                rollAngleDegrees = MAXROLL;
            if (rollAngleDegrees < -MAXROLL)
                rollAngleDegrees = -MAXROLL;
        }

        //System.out.println("Yaw: " + yawAngleDegrees + ", Pitch: " + pitchAngleDegrees + ", Roll: " + rollAngleDegrees);
	}
	
	public HMDInfo getHMDInfo() {
		return hmdInfo;
	}
	
	public SensorInfo getSensorInfo() {
		return null; // TODO: Support get sensor info
	}
	
	public float getYawDegrees_LH()
    {
        return yawAngleDegrees;
	}
	
	public float getPitchDegrees_LH()
    {
        return pitchAngleDegrees;
    }

	public float getRollDegrees_LH()
    {
        return rollAngleDegrees;
	}

    public void setPrediction(float delta, boolean enable)
    {
        if (initialized)
        {
            _setPredictionEnabled(delta, enable);
        }
    }

	public void destroy() {

        if (initialized)
        {
            _destroySubsystem();
        }

        _initSummary = "Not initialised";
        initialized = false;
	}

    public EyeRenderParams getEyeRenderParams(int viewPortWidth, int viewPortHeight)
    {
        return getEyeRenderParams(0, 0, viewPortWidth, viewPortHeight, 0.05f, 1000.0f);
    }

    public EyeRenderParams getEyeRenderParams(int viewPortX, int viewPortY, int viewPortWidth, int viewPortHeight, float clipNear, float clipFar)
    {
        return getEyeRenderParams(viewPortX, viewPortY, viewPortWidth, viewPortHeight, clipNear, clipFar, 1.0f, 1.0f);
    }

    public EyeRenderParams getEyeRenderParams(int viewPortX,
                                              int viewPortY,
                                              int viewPortWidth,
                                              int viewPortHeight,
                                              float clipNear,
                                              float clipFar,
                                              float eyeToScreenDistanceScaleFactor,
                                              float lensSeparationScaleFactor)
    {
        return getEyeRenderParams(
                viewPortX,
                viewPortY,
                viewPortWidth,
                viewPortHeight,
                clipNear,
                clipFar,
                eyeToScreenDistanceScaleFactor,
                lensSeparationScaleFactor,
                -1.0f,
                0.0f,
                AspectCorrectionType.CORRECTION_AUTO);
    }

    public EyeRenderParams getEyeRenderParams(int viewPortX,
                                              int viewPortY,
                                              int viewPortWidth,
                                              int viewPortHeight,
                                              float clipNear,
                                              float clipFar,
                                              float eyeToScreenDistanceScaleFactor,
                                              float lensSeparationScaleFactor,
                                              float distortionFitX,
                                              float distortionFitY,
                                              AspectCorrectionType aspectCorrectionType)
    {
        return _getEyeRenderParams(
                viewPortX,
                viewPortY,
                viewPortWidth,
                viewPortHeight,
                clipNear,
                clipFar,
                eyeToScreenDistanceScaleFactor,
                lensSeparationScaleFactor,
                distortionFitX,
                distortionFitY,
                aspectCorrectionType.getValue());
    }

    public UserProfileData getUserProfile()
    {
        if (!isInitialized())
            return null;

        return _getUserProfileData();
    }

    public boolean loadUserProfile(String profileName)
    {
        if (!isInitialized())
            return false;

        return _loadUserProfile(profileName);
    }

    public String[] getUserProfiles()
    {
        if (!isInitialized())
            return null;

        return _getUserProfiles();
    }

    public float[] latencyTesterDisplayScreenColor()
    {
        return _latencyTesterDisplayScreenColor();
    }

    public String latencyTesterGetResultsString()
    {
        return _latencyTesterGetResultsString();
    }

	protected native boolean _initSubsystem();
    protected native void _pollSubsystem();
    protected native void _destroySubsystem();
    protected native void _reset(); // Resets sensor data, including Yaw

    protected native boolean _isCalibrated();

    protected native void _setPredictionEnabled(float delta, boolean enable);

    protected native int _getHResolution();
    protected native int _getVResolution();
    protected native float _getHScreenSize();
    protected native float _getVScreenSize();
    protected native float _getVScreenCenter();
    protected native float _getEyeToScreenDistance();
    protected native float _getLensSeparationDistance();
    protected native float _getInterpupillaryDistance();
    protected native void  _setInterpupillaryDistance(float ipd);
    protected native float _getDistortionK0();
    protected native float _getDistortionK1();
    protected native float _getDistortionK2();
    protected native float _getDistortionK3();
    protected native float _getChromaAbCorrection0();
    protected native float _getChromaAbCorrection1();
    protected native float _getChromaAbCorrection2();
    protected native float _getChromaAbCorrection3();

    protected native float _getYaw();
    protected native float _getPitch();
    protected native float _getRoll();

    protected native EyeRenderParams _getEyeRenderParams(int currentViewportX,
                                                         int currentViewportY,
                                                         int currentViewportWidth,
                                                         int currentViewportHeight,
                                                         float clipNear,
                                                         float clipFar,
                                                         float eyeToScreenDistanceScaleFactor,
                                                         float lensSeparationScaleFactor,
                                                         float distortionFitX,
                                                         float distortionFitY,
                                                         int aspectCorrectionMode);

    protected native UserProfileData _getUserProfileData();
    protected native boolean _loadUserProfile(String profileName);
    protected native String[] _getUserProfiles();

    protected native void    _latencyTesterProcessInputs();
    protected native float[] _latencyTesterDisplayScreenColor();
    protected native String  _latencyTesterGetResultsString();

	public static void LoadLibrary( File nativeDir )
	{
		if( libraryLoaded ) return;
		String os = System.getProperty("os.name");
		boolean is64bit = System.getProperty("sun.arch.data.model").equalsIgnoreCase("64"); 
		/*
		String libName = "FILE_DOESN'T_EXIST";

        if ( os.contains("Win") )
        {
			if( is64bit )
				libName = "JRiftLibrary64.dll";
			else
				libName = "JRiftLibrary.dll";
		}
		else if (os.contains("Mac") )
		{
			if( is64bit )
				libName = "libJRiftLibrary64.jnilib";
			else
				libName = "libJRiftLibrary.jnilib";
        }
        else if (os.contains("Linux") )
        {
			if( is64bit )
				libName = "libJRiftLibrary64.so";
			else
				libName = "libJRiftLibrary.so";
        }
		else
		{
            System.out.println("Operating System not supported: " + os );
		}
		try 
		{
			System.out.println("Loading jar:/"+libName+" ... ");
			InputStream libStream = OculusRift.class.getResourceAsStream( "/" + libName );

			byte[] buffer = new byte[1024];
			int len = libStream.read(buffer); //Try to read before opening FileOutputStream

			File outFile = new File( nativeDir, libName );
			OutputStream out = new FileOutputStream( outFile );

			while (len != -1) 
			{
				out.write(buffer, 0, len);
				len = libStream.read(buffer);
			}
			out.close();
			System.load( outFile.getAbsolutePath() );
			libraryLoaded = true;

			System.out.println( libName + " loaded");
		} 
		catch( Exception e ) 
		{ 
			System.out.println( e.toString() );
			System.out.println( "Couldn't load "+libName+"... trying alternative way" );
			if( is64bit )
				System.loadLibrary( "JRiftLibrary64" );
			else
				System.loadLibrary( "JRiftLibrary" );
			libraryLoaded = true;
		}
		*/

		//Launcher takes care of extracting natives
		if( is64bit )
			System.loadLibrary( "JRiftLibrary64" );
		else
			System.loadLibrary( "JRiftLibrary" );
		System.out.println("Loaded JRift native library");
		libraryLoaded = true;
    }
	
	public static void main(String[] args) {
		OculusRift.LoadLibrary(new File(System.getProperty("java.io.tmpdir")) );
		OculusRift or = new OculusRift();
		or.init();
		
		HMDInfo hmdInfo = or.getHMDInfo();
		System.out.println(hmdInfo);
		
		while (or.isInitialized()) {
			or.poll();
			
			System.out.println("Yaw: " + or.getYawDegrees_LH() + " Pitch: " + or.getPitchDegrees_LH() + " Roll: " + or.getRollDegrees_LH());
			
			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		or.destroy();
	}
}
