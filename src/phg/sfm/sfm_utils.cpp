#include "sfm_utils.h"

#include <algorithm>
#include <stdexcept>
#include <iostream>


// pseudorandom number generator
uint64_t xorshift64(uint64_t *state)
{
    if (*state == 0) {
        *state = 1;
    }

    uint64_t x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return *state = x;
}

void phg::randomSample(std::vector<int> &dst, int max_id, int sample_size, uint64_t *state)
{
    dst.clear();

    const int max_attempts = 1000;

    for (int i = 0; i < sample_size; ++i) {
        for (int k = 0; k < max_attempts; ++k) {
            int v = xorshift64(state) % max_id;
            if (dst.empty() || std::find(dst.begin(), dst.end(), v) == dst.end()) {
                dst.push_back(v);
                break;
            }
        }
        if (dst.size() < i + 1) {
            throw std::runtime_error("Failed to sample ids");
        }
    }
}

// проверяет, что расстояние от точки до линии меньше порога
bool phg::epipolarTest(const cv::Vec2d &pt0, const cv::Vec2d &pt1, const cv::Matx33d &F, double t)
{
    double d0[] = {pt0[0], pt0[1], 1};
    cv::Mat pt0t(3, 1, CV_64F, d0);
    cv::Mat line = F * pt0t;

    double dist = abs(pt1[0] * line.at<double>(0, 0) + pt1[1] * line.at<double>(0, 1) + line.at<double>(0, 2)) /
            sqrt(line.at<double>(0, 0) * line.at<double>(0, 0) + line.at<double>(0, 1) * line.at<double>(0, 1));

    return dist < t;
}
