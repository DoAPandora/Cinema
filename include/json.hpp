#pragma once
#include "rapidjson-macros/shared/macros.hpp"

// Terible namespace name should probably change
namespace Cinema::JSON {

    // Just basics for now

    DECLARE_JSON_CLASS(Vector3,
        NAMED_VALUE_DEFAULT(float, x, 0, "x");
        NAMED_VALUE_DEFAULT(float, y, 0, "y");
        NAMED_VALUE_DEFAULT(float, z, 0, "z");
    );

    DECLARE_JSON_CLASS(CinemaInfo,
        NAMED_VALUE_DEFAULT(std::string, videoId, "", "videoID");
        NAMED_VALUE_DEFAULT(std::string, title, "", "title");
        NAMED_VALUE_DEFAULT(std::string, author, "", "author");
        NAMED_VALUE_DEFAULT(std::string, videoFile, "", "videoFile");
        NAMED_VALUE_DEFAULT(int, duration, 0, "duration");
        NAMED_VALUE_DEFAULT(int, offset, 0, "offset");
        NAMED_VALUE_DEFAULT(Vector3, screenPosition, Vector3(), "screenPosition");
        NAMED_VALUE_DEFAULT(float, screenHeight, 0, "screenHeight");
        NAMED_VALUE_DEFAULT(int, screenCurvature, 0, "screenCurvature");
    );
}