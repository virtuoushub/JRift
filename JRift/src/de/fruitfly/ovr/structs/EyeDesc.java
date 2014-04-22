package de.fruitfly.ovr.structs;

import de.fruitfly.ovr.enums.EyeType;

public class EyeDesc
{
    public EyeType  Eye;
    public Sizei    TextureSize;     // Absolute size of render texture.
    public Recti    RenderViewport;  // Viewport within texture where eye rendering takes place.
                                     // If specified as (0,0,0,0), it will be initialized to TextureSize.
    public FovPort  Fov;
}
