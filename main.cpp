#include <librealsense2/rs.hpp>
#include <iostream>
#include <chrono>
#include <thread>

typedef struct {
    std::string serial;
    rs2::pipeline pipeline;
} Cam;

Cam start_device(rs2::device device) {
    std::string serial_number(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
    std::cout << "Found camera with serial " << serial_number << std::endl;

    rs2::config config;
    config.enable_device(serial_number);
    config.enable_stream(RS2_STREAM_INFRARED, 1, 848, 480, RS2_FORMAT_Y8, 90);

    rs2::pipeline pipeline;
    rs2::pipeline_profile profile = pipeline.start(config);
    profile.get_device().first<rs2::depth_sensor>().set_option(RS2_OPTION_INTER_CAM_SYNC_MODE, 0);

    Cam cam = { serial_number, pipeline };
    return cam;
}

int main(int argc, char * argv[]) try
{
    std::cout << "Start!" << std::endl;

    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    rs2::context ctx;
    std::vector<Cam> cams;

    for (auto&& device : ctx.query_devices()) {
        cams.push_back(start_device(device));
    }

    while (true) {
        for (auto it = cams.begin(); it != cams.end(); ++it) {
            auto cam = *it;
            auto data = cam.pipeline.wait_for_frames();
            auto backend = data.get_frame_metadata(RS2_FRAME_METADATA_BACKEND_TIMESTAMP);
            auto counter = data.get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
            std::cout << cam.serial << " " << counter << " " << backend << std::endl;
        }
    }

    return 0;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return 1;
}
catch (const std::exception& e)
{
    std::cerr << "Exception " << e.what() << std::endl;
    return 1;
}
