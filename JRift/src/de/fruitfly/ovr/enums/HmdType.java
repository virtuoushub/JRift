package de.fruitfly.ovr.enums;

public enum HmdType
{
    ovrHmd_None(0),
    ovrHmd_DK1(3),
    ovrHmd_DKHD(4),
    ovrHmd_CrystalCoveProto(5),
    ovrHmd_DK2(6),
    ovrHmd_Other(999);

        private final int hmdTypeEnum;

        private HmdType(int value)
        {
            this.hmdTypeEnum = value;
        }

    public static HmdType fromInteger(int x) {
        switch(x) {
            case 0:
                return ovrHmd_None;
            case 3:
                return ovrHmd_DK1;
            case 4:
                return ovrHmd_DKHD;
            case 5:
                return ovrHmd_CrystalCoveProto;
            case 6:
                return ovrHmd_DK2;
        }
        return ovrHmd_Other;
    }

    public static String toString(HmdType type)
    {
        switch(type)
        {
            case ovrHmd_None:
                return "None";
            case ovrHmd_DK1:
                return "DK1";
            case ovrHmd_DKHD:
                return "DKHD";
            case ovrHmd_CrystalCoveProto:
                return "CrystalCoveProto";
            case ovrHmd_DK2:
                return "DK2";
        }

        return "Unknown";
    }
}
