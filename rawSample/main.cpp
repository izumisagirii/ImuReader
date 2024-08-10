#include "reader/reader.h"
#include <cassert>
#include <chrono> // For time measurement
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <iostream>

bmi160_sensor_data bmi160_accel[256]; // assume max 32 data packets
bmi160_sensor_data bmi160_gyro[256];  // assume max 32 data packets
bool keep_running = true;
ULONG frame_count = 0;
std::chrono::time_point<std::chrono::high_resolution_clock> start_time, end_time;

void signalHandler(int signum)
{
    keep_running = false;
    printf("Interrupt signal (%d) received. Exiting...\n", signum);
}

int main()
{
    std::cout << "CommandLine IMU Test" << std::endl;
    ImuReader imuReader{};
    std::cout << "ImuReader Cmdline Tester by CYH" << std::endl;

#ifdef WINDOWS
    std::cout << "Running on Windows" << std::endl;
#elif defined(LINUX)
    std::cout << "Running on Linux" << std::endl;
#endif

    auto ret = imuReader.init(0);

    if (ret != BMI160_OK)
    {
        std::cout << "Failed to initialize IMU, code " << ret << std::endl;
        return -1;
    }

    imuReader.setSensorConfig();

    // Register signal handler for Ctrl+C
    signal(SIGINT, signalHandler);

    // Ask user if they want to continue
    char continue_loop;
    std::cout << "Do you want to start data fetching? Ctrl-C to stop. (y/n): ";
    std::cin >> continue_loop;
    if (continue_loop != 'y' && continue_loop != 'Y')
    {
        return 0;
    }

    // Ask user if they want to print data
    char print_data;
    std::cout << "Do you want to print the data? (y/n): ";
    std::cin >> print_data;
    bool should_print = (print_data == 'y' || print_data == 'Y');

    // Ask user if they want to use FIFO
    char use_fifo;
    std::cout << "Do you want to use FIFO or just fetch one sample? (y/n): ";
    std::cin >> use_fifo;
    bool use_fifo_mode = (use_fifo == 'y' || use_fifo == 'Y');

    // Record start time
    start_time = std::chrono::high_resolution_clock::now();

    while (keep_running)
    {
        if (use_fifo_mode)
        {
            uint8_t accel_len = 255;
            uint8_t gyro_len = 255;
            uint32_t sensor_time = 0;
            ret = imuReader.getFIFOData(bmi160_accel, &accel_len, bmi160_gyro, &gyro_len, &sensor_time);
            if (ret == BMI160_OK)
            {
                if (should_print)
                {
                    for (uint8_t i = 0; i < accel_len; ++i)
                    {
                        printf("Accel[%d] - ax:%d\tay:%d\taz:%d\n", i, bmi160_accel[i].x, bmi160_accel[i].y,
                               bmi160_accel[i].z);
                    }
                    for (uint8_t i = 0; i < gyro_len; ++i)
                    {
                        printf("Gyro[%d] - gx:%d\tgy:%d\tgz:%d\n", i, bmi160_gyro[i].x, bmi160_gyro[i].y,
                               bmi160_gyro[i].z);
                    }
                    printf("FIFO packet length: %d\t Sensor Time: %u\n", accel_len + gyro_len, sensor_time);
                }
                else
                {
                    Sleep(2);
                }
                frame_count += accel_len + gyro_len;
            }
            else
            {
                printf("BMI160 get FIFO data failure !\n");
            }
        }
        else
        {
            ret = imuReader.getSensorData(&bmi160_accel[0], &bmi160_gyro[0]);
            if (ret == BMI160_OK)
            {
                if (should_print)
                {
                    printf("ax:%d\tay:%d\taz:%d\tt:%u\t", bmi160_accel[0].x, bmi160_accel[0].y, bmi160_accel[0].z,
                           bmi160_accel[0].sensortime);
                    printf("gx:%d\tgy:%d\tgz:%d\tt:%u\n", bmi160_gyro[0].x, bmi160_gyro[0].y, bmi160_gyro[0].z,
                           bmi160_gyro[0].sensortime);
                }
                else
                {
                    // Sleep(0);
                }
                frame_count++;
            }
            else
            {
                printf("BMI160 get sensor data failure !\n");
            }
        }
    }

    // Record end time
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    double fps = frame_count / elapsed_seconds.count();

    std::cout << "Total frames: " << frame_count << std::endl;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << " seconds" << std::endl;
    std::cout << "Frames per second (FPS): " << fps << std::endl;

    return 0;
}
