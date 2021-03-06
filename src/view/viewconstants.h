#ifndef _NALCG_VIEW_CONSTANTS_H_
#define _NALCG_VIEW_CONSTANTS_H_

#include <Ogre.h>

using namespace Ogre;

namespace ViewConstants
{
    static const ColourValue AMBIENT_COLOUR(ColourValue(0.2, 0.2, 0.2));
    static const ColourValue YELLOW_COLOUR(ColourValue(1.0, 1.0, 0.8));
    static const ColourValue BLUE_COLOUR(ColourValue(1.0, 0.8, 0.9));
    static const int SQUARE_SIDE_LENGTH = 200;
    static const Vector3 WHITE_CAMERA_POSITION(0, 1800, 1900);
    static const Vector3 BLACK_CAMERA_POSITION(0, 1800, -1900);

    static const std::string SHOW_ADDITIONAL("Show additional options");
    static const std::string HIDE_GUI(       "Hide graphical interface");
    static const std::string SHOW_BASIC(     "Show basic options");
};

#endif // _NALCG_VIEW_CONSTANTS_H_
