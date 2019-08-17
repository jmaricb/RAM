/*!
 * @file minimizer_engine.hpp
 *
 * @brief MinimizerEngine class header file
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <unordered_map>

namespace thread_pool {
    class ThreadPool;
}

namespace ram {

struct Sequence {
    Sequence(const char* name, std::uint32_t name_length,
        const char* data, std::uint32_t data_length);
    Sequence(const char* name, std::uint32_t name_length,
        const char* data, std::uint32_t data_length,
        const char* quality, std::uint32_t quality_length);
    ~Sequence() = default;

    static std::uint64_t num_objects;

    std::uint64_t id;
    std::string name;
    std::string data;
    std::string quality;
};

struct Overlap {
    Overlap(std::uint64_t t_id, std::uint32_t t_begin, std::uint32_t t_end,
        std::uint32_t q_begin, std::uint32_t q_end, std::uint32_t strand);
    ~Overlap() = default;

    std::uint64_t t_id;
    std::uint32_t t_begin;
    std::uint32_t t_end;
    std::uint32_t q_begin;
    std::uint32_t q_end;
    std::uint32_t strand;
};

class MinimizerEngine;
std::unique_ptr<MinimizerEngine> createMinimizerEngine(std::uint8_t k,
    std::uint8_t w, std::shared_ptr<thread_pool::ThreadPool> thread_pool);

using uint128_t = std::pair<std::uint64_t, std::uint64_t>;

class MinimizerEngine {
public:
    ~MinimizerEngine() = default;

    void minimize(const std::vector<std::unique_ptr<Sequence>>& src);
    void minimize(
        std::vector<std::unique_ptr<Sequence>>::const_iterator begin,
        std::vector<std::unique_ptr<Sequence>>::const_iterator end);

    void filter(double f);

    std::vector<Overlap> map(const std::unique_ptr<Sequence>& src,
        bool diagonal, bool triangle, std::uint32_t e = -1) const;

    friend std::unique_ptr<MinimizerEngine> createMinimizerEngine(std::uint8_t k,
        std::uint8_t w, std::shared_ptr<thread_pool::ThreadPool> thread_pool);
private:
    MinimizerEngine(std::uint8_t k, std::uint8_t w,
        std::shared_ptr<thread_pool::ThreadPool> thread_pool);
    MinimizerEngine(const MinimizerEngine&) = delete;
    const MinimizerEngine& operator=(const MinimizerEngine&) = delete;

    std::vector<uint128_t> minimize(const std::unique_ptr<Sequence>& src,
        std::uint32_t e = -1) const;

    struct MinimizerHash {
        MinimizerHash(std::uint16_t num_bints);
        ~MinimizerHash() = default;

        void clear();

        inline std::pair<std::vector<uint128_t>::const_iterator, std::vector<uint128_t>::const_iterator>
            operator[](std::uint64_t minimizer) const;

        std::uint64_t m_;
        std::vector<std::vector<uint128_t>> minimizers;
        std::vector<std::unordered_map<std::uint64_t, uint128_t>> index;
    };

    std::uint8_t k_;
    std::uint8_t w_;
    std::int64_t s_;
    MinimizerHash hash_;
    std::shared_ptr<thread_pool::ThreadPool> thread_pool_;
};

}
