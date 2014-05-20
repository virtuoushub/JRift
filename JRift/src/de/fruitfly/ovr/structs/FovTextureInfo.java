package de.fruitfly.ovr.structs;

public class FovTextureInfo
{
    public FovTextureInfo() {}

    public FovTextureInfo(int resW, int resH, float scaleW, float scaleH)
    {
        Res.w = resW;
        Res.h = resH;
        ScaleW = scaleW;
        ScaleH = scaleH;
    }

    public Sizei Res = new Sizei();
    public float ScaleW = 0f;
    public float ScaleH = 0f;
}
