#include <librealsense2/rs.hpp>
#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char * argv[]) try
{
    std::cout << "Start!" << std::endl;

    rs2::log_to_console(RS2_LOG_SEVERITY_ERROR);
    rs2::colorizer color_map;
    rs2::pipeline pipe;
    pipe.start();

    while(true) {
        rs2::frameset data = pipe.wait_for_frames();
	rs2_metadata_type backend = data.get_frame_metadata(RS2_FRAME_METADATA_BACKEND_TIMESTAMP);
	rs2_metadata_type counter = data.get_frame_metadata(RS2_FRAME_METADATA_FRAME_COUNTER);
	std::cout << counter << " " << backend << std::endl;
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
    std::cerr << e.what() << std::endl;
    return 1;
}
