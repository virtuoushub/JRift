package de.fruitfly.ovr.structs;

public class SensorState
{
    public SensorState(float PredictedPoseStatefPosefOrientationx,
                       float PredictedPoseStatefPosefOrientationy,
                       float PredictedPoseStatefPosefOrientationz,
                       float PredictedPoseStatefPosefOrientationw,
                       float PredictedPoseStatefPosefPositionx,
                       float PredictedPoseStatefPosefPositiony,
                       float PredictedPoseStatefPosefPositionz,
                       float PredictedVector3fAngularVelocityx,
                       float PredictedVector3fAngularVelocityy,
                       float PredictedVector3fAngularVelocityz,
                       float PredictedVector3fLinearVelocityx,
                       float PredictedVector3fLinearVelocityy,
                       float PredictedVector3fLinearVelocityz,
                       float PredictedVector3fAngularAccelerationx,
                       float PredictedVector3fAngularAccelerationy,
                       float PredictedVector3fAngularAccelerationz,
                       float PredictedVector3fLinearAccelerationx,
                       float PredictedVector3fLinearAccelerationy,
                       float PredictedVector3fLinearAccelerationz,
                       double PredictedTimeInSeconds,
                       float RecordedPoseStatefPosefOrientationx,
                       float RecordedPoseStatefPosefOrientationy,
                       float RecordedPoseStatefPosefOrientationz,
                       float RecordedPoseStatefPosefOrientationw,
                       float RecordedPoseStatefPosefPositionx,
                       float RecordedPoseStatefPosefPositiony,
                       float RecordedPoseStatefPosefPositionz,
                       float RecordedVector3fAngularVelocityx,
                       float RecordedVector3fAngularVelocityy,
                       float RecordedVector3fAngularVelocityz,
                       float RecordedVector3fLinearVelocityx,
                       float RecordedVector3fLinearVelocityy,
                       float RecordedVector3fLinearVelocityz,
                       float RecordedVector3fAngularAccelerationx,
                       float RecordedVector3fAngularAccelerationy,
                       float RecordedVector3fAngularAccelerationz,
                       float RecordedVector3fLinearAccelerationx,
                       float RecordedVector3fLinearAccelerationy,
                       float RecordedVector3fLinearAccelerationz,
                       double RecordedTimeInSeconds,
                       float temperature,
                       int statusFlags
                       )
    {
        Predicted.Pose.Orientation.x    = PredictedPoseStatefPosefOrientationx;
        Predicted.Pose.Orientation.y    = PredictedPoseStatefPosefOrientationy;
        Predicted.Pose.Orientation.z    = PredictedPoseStatefPosefOrientationz;
        Predicted.Pose.Orientation.w    = PredictedPoseStatefPosefOrientationw;
        Predicted.Pose.Position.x       = PredictedPoseStatefPosefPositionx;
        Predicted.Pose.Position.y       = PredictedPoseStatefPosefPositiony;
        Predicted.Pose.Position.z       = PredictedPoseStatefPosefPositionz;
        Predicted.AngularVelocity.x     = PredictedVector3fAngularVelocityx;
        Predicted.AngularVelocity.y     = PredictedVector3fAngularVelocityy;
        Predicted.AngularVelocity.z     = PredictedVector3fAngularVelocityz;
        Predicted.LinearVelocity.x      = PredictedVector3fLinearVelocityx;
        Predicted.LinearVelocity.y      = PredictedVector3fLinearVelocityy;
        Predicted.LinearVelocity.z      = PredictedVector3fLinearVelocityz;
        Predicted.AngularAcceleration.x = PredictedVector3fAngularAccelerationx;
        Predicted.AngularAcceleration.y = PredictedVector3fAngularAccelerationy;
        Predicted.AngularAcceleration.z = PredictedVector3fAngularAccelerationz;
        Predicted.LinearAcceleration.x  = PredictedVector3fLinearAccelerationx;
        Predicted.LinearAcceleration.y  = PredictedVector3fLinearAccelerationy;
        Predicted.LinearAcceleration.z  = PredictedVector3fLinearAccelerationz;
        Predicted.TimeInSeconds         = PredictedTimeInSeconds;
        Recorded.Pose.Orientation.x    = RecordedPoseStatefPosefOrientationx;
        Recorded.Pose.Orientation.y    = RecordedPoseStatefPosefOrientationy;
        Recorded.Pose.Orientation.z    = RecordedPoseStatefPosefOrientationz;
        Recorded.Pose.Orientation.w    = RecordedPoseStatefPosefOrientationw;
        Recorded.Pose.Position.x       = RecordedPoseStatefPosefPositionx;
        Recorded.Pose.Position.y       = RecordedPoseStatefPosefPositiony;
        Recorded.Pose.Position.z       = RecordedPoseStatefPosefPositionz;
        Recorded.AngularVelocity.x     = RecordedVector3fAngularVelocityx;
        Recorded.AngularVelocity.y     = RecordedVector3fAngularVelocityy;
        Recorded.AngularVelocity.z     = RecordedVector3fAngularVelocityz;
        Recorded.LinearVelocity.x      = RecordedVector3fLinearVelocityx;
        Recorded.LinearVelocity.y      = RecordedVector3fLinearVelocityy;
        Recorded.LinearVelocity.z      = RecordedVector3fLinearVelocityz;
        Recorded.AngularAcceleration.x = RecordedVector3fAngularAccelerationx;
        Recorded.AngularAcceleration.y = RecordedVector3fAngularAccelerationy;
        Recorded.AngularAcceleration.z = RecordedVector3fAngularAccelerationz;
        Recorded.LinearAcceleration.x  = RecordedVector3fLinearAccelerationx;
        Recorded.LinearAcceleration.y  = RecordedVector3fLinearAccelerationy;
        Recorded.LinearAcceleration.z  = RecordedVector3fLinearAccelerationz;
        Recorded.TimeInSeconds         = RecordedTimeInSeconds;        
        Temperature = temperature;
        StatusFlags = statusFlags;
    }
    
    // Predicted pose configuration at requested absolute time.
    // One can determine the time difference between predicted and actual
    // readings by comparing ovrPoseState.TimeInSeconds.
    public PoseStatef  Predicted;

    // Actual recorded pose configuration based on the sensor sample at a
    // moment closest to the requested time.
    public PoseStatef  Recorded;

    // Sensor temperature reading, in degrees Celsius, as sample time.
    public float       Temperature;

    // Sensor status described by ovrStatusBits.
    public int   StatusFlags;
}
