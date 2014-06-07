#ifndef KISMET_FUZZY_VARIABLE_H
#define KISMET_FUZZY_VARIABLE_H

#include <vector>
#include <memory>

namespace kismet
{
namespace fuzzy
{

class fuzzy_set;

/**
 * A fuzzy variable which contains fuzzy sets.
 */
class fuzzy_variable
{
public:
    fuzzy_variable();
    ~fuzzy_variable();

    // Allow moving
    fuzzy_variable(fuzzy_variable&& rhs);
    fuzzy_variable& operator =(fuzzy_variable rhs);

    void add_traiangle_set(float min, float mid, float max);

    void add_trapezoid_set(float m1, float m2, float m3, float m4);

    void add_left_trapezoid_set(float min, float mid, float max);

    void add_right_trapezoid_set(float min, float mid, float max);

    void add_singleton_set(float m);

    void reset_dom();

    // Fuzzify input value
    void fuzzify(float input);

    float defuzzify_mean_max() const;

    float defuzzify_centroid(std::size_t sample_count) const;

    // Get the domain of the fuzzy variable
    float get_min() const
    {
        return m_min;
    }

    float get_max() const
    {
        return m_max;
    }

    void swap(fuzzy_variable& rhs);
private:
    void update_range(float min, float max);

    using fuzzy_set_ptr = std::unique_ptr<fuzzy_set>;

    std::vector<fuzzy_set_ptr> m_sets;

    // Domain of the fuzzy variable
    float m_min;
    float m_max;
};

inline void swap(fuzzy_variable& lhs, fuzzy_variable& rhs)
{
    return lhs.swap(rhs);
}

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_VARIABLE_H
