package de.fruitfly.ovr;

/**
 * Created with IntelliJ IDEA.
 * User: StellaArtois
 * Date: 7/4/13
 * Time: 11:27 PM
 * To change this template use File | Settings | File Templates.
 */
public class UserProfileData
{
    public static enum GenderType
    {
        Gender_Unspecified,
        Gender_Male,
        Gender_Female
    };

    public GenderType _gender;
    public float _playerHeight;
    public float _eyeHeight;
    public float _ipd;
    String _name;

    UserProfileData(float playerHeight, float eyeHeight, float ipd,
                    int gender, String name)
    {
         _name = name;
        _playerHeight = playerHeight;
        _eyeHeight = eyeHeight;
        _ipd = ipd;
        switch (gender)
        {
            case 1:
                _gender = GenderType.Gender_Male;
                break;
            case 2:
                _gender = GenderType.Gender_Female;
                break;
            default:
                _gender = GenderType.Gender_Unspecified;
                break;
        }
    }
}
