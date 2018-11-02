#include <librealsense2/rs.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <future>

typedef struct {
    std::string serial;
    rs2::pipeline pipeline;
} Cam;

Cam start_device(rs2::device device, int camSyncMode) {
    std::string serial_number(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
    std::cout << "Found camera with serial " << serial_number << std::endl;

    rs2::config config;
    config.enable_device(serial_number);
    //config.enable_stream(RS2_STREAM_INFRARED, 1, 848, 480, RS2_FORMAT_Y8, 90);
    //config.enable_stream(RS2_STREAM_DEPTH, 848, 480, RS2_FORMAT_Z16, 90);
    config.enable_stream(RS2_STREAM_INFRARED, 1, 640, 480, RS2_FORMAT_Y8, 30);
    config.enable_stream(RS2_STREAM_DEPTH, 640, 480, RS2_FORMAT_Z16, 30);
    config.enable_stream(RS2_STREAM_COLOR, 1920, 1080, RS2_FORMAT_BGR8, 30);

    rs2::pipeline pipeline;
    rs2::pipeline_profile profile = pipeline.start(config);
    profile.get_device().first<rs2::depth_sensor>().set_option(RS2_OPTION_INTER_CAM_SYNC_MODE, camSyncMode);

    Cam cam = { serial_number, pipeline };
    return cam;
}

void grabFrames(Cam cam) {
    for (int i = 0; i < 250; ++i) {
        auto data = cam.pipeline.wait_for_frames();
        auto backend = data.get_frame_metadata(RS2_FRAME_METADATA_BACKEND_TIMESTAMP);
        auto counter = data.get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
        std::stringstream str;
        str << cam.serial << "," << counter << "," << backend << "\n";
        std::cout << str.str();
    }
}

int main(int argc, char * argv[]) try
{
    std::cout << "Start!" << std::endl;

    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    rs2::context ctx;
    std::vector<Cam> cams;
    int camSyncMode = 1;

    for (auto&& device : ctx.query_devices()) {
        auto cam = start_device(device, camSyncMode);
        cams.push_back(std::move(cam));
        camSyncMode = 2;
    }

    std::vector<std::thread> threads;

    for (auto it = cams.begin(); it != cams.end(); ++it) {
        auto cam = *it;
        std::thread thread(grabFrames, cam);
        threads.push_back(std::move(thread));
    }

    for (auto it = threads.begin(); it != threads.end(); ++it) {
        (*it).join();
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
