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
    public String _name;
    public boolean _isDefault;

    UserProfileData(float playerHeight, float eyeHeight, float ipd,
                    String gender, boolean isDefault, String name)
    {
         _name = name;
        _playerHeight = playerHeight;
        _eyeHeight = eyeHeight;
        _ipd = ipd;
        _isDefault = isDefault;

        if (gender.equalsIgnoreCase("male")) {
            _gender = GenderType.Gender_Male;
        }
        else if (gender.equalsIgnoreCase("female")) {
            _gender = GenderType.Gender_Female;
        }
        else
        {
           _gender = GenderType.Gender_Unspecified;
        }
    }

    public String getGenderString()
    {
        String gender = "Unspecified";

        switch(_gender)
        {
            case Gender_Male:
                gender = "Male";
                break;
            case Gender_Female:
                gender = "Female";
        }

        return gender;
    }
}
