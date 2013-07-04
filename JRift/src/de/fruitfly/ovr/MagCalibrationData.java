package de.fruitfly.ovr;

import java.io.Serializable;

public class MagCalibrationData implements Serializable
{
    public float[] _reference;
    public float[][] _calibrationM;

    public MagCalibrationData(float refX, float refY, float refZ, float refW,
                              float calM00, float calM01, float calM02, float calM03,
                              float calM10, float calM11, float calM12, float calM13,
                              float calM20, float calM21, float calM22, float calM23,
                              float calM30, float calM31, float calM32, float calM33)
    {
        _reference = new float[4];
        _calibrationM = new float[4][4];

        _reference[0] = refX; _reference[1] = refY; _reference[2] = refZ; _reference[3] = refW;
        _calibrationM[0][0] = calM00; _calibrationM[0][1] = calM01; _calibrationM[0][2] = calM02; _calibrationM[0][3] = calM03;
        _calibrationM[1][0] = calM10; _calibrationM[1][1] = calM11; _calibrationM[1][2] = calM12; _calibrationM[1][3] = calM13;
        _calibrationM[2][0] = calM20; _calibrationM[2][1] = calM21; _calibrationM[2][2] = calM22; _calibrationM[2][3] = calM23;
        _calibrationM[3][0] = calM30; _calibrationM[3][1] = calM31; _calibrationM[3][2] = calM32; _calibrationM[3][3] = calM33;
    }
}
