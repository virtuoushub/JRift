package de.fruitfly.ovr.structs;

public class Posef
{
    public Posef(float quatx,
                 float quaty,
                 float quatz,
                 float quatw,
                 float posx,
                 float posy,
                 float posz
                 )
    {
        Orientation.x = quatx;
        Orientation.y = quaty;
        Orientation.z = quatz;
        Orientation.w = quatw;
        Position.x = posx;
        Position.y = posy;
        Position.z = posz;
    }

    public Quatf     Orientation;
    public Vector3f  Position;
}
