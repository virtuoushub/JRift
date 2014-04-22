package de.fruitfly.ovr.structs;

public class EyeRenderDesc
{
    public EyeDesc  Desc;
    public Recti	DistortedViewport; 	           // Distortion viewport
    public Vector2f PixelsPerTanAngleAtCenter;     // How many display pixels will fit in tan(angle) = 1.
    public Vector3f ViewAdjust;  		           // Translation to be applied to view matrix.
}
