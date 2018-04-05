#ifndef TIMINGH
#define TIMINGH

#include <chrono>

class rt_timer
{
public:
    // Elapsed time in seconds
    double time;

    rt_timer( const bool start_now = false ) : time(0) { if (start_now) play(); }

    void reset() { time = 0; }

    void play()
    {
        t_start = std::chrono::system_clock::now();
    }

    void pause()
    {
        std::chrono::system_clock::time_point t_end = std::chrono::system_clock::now();
        std::chrono::duration<double> len = t_end - t_start;
        time += len.count();
    }

private:
    std::chrono::system_clock::time_point t_start;
};

#endif
