#ifndef KISMET_FUZZY_SET_H
#define KISMET_FUZZY_SET_H

#include <memory>
#include "kismet/core/assert.h"
#include "kismet/utility.h"

namespace kismet
{
namespace fuzzy
{

/**
 * Base class for all kind of fuzzy sets.
 */
class fuzzy_set
{
public:
    fuzzy_set(float mean);

    virtual ~fuzzy_set();

    // Disallow copying
    fuzzy_set(fuzzy_set&) = delete;
    fuzzy_set& operator =(fuzzy_set&) = delete;

    /**
     * Calculate the degree of membership of the given input.
     * @return degree of membership in [0..1]
     */
    float get_dom(float input) const
    {
        float dom = do_get_dom(input);
        KISMET_ASSERT(dom >= 0.0f && dom <= 1.0f);
        return dom;
    }

    float get_dom() const
    {
        return m_dom;
    }

    /**
     * Reset dom to 0
     */
    void reset_dom()
    {
        set_dom(0.0f);
    }

    /**
     * Set the dom to the given value, used by fuzzy_system for inference
     */
    void set_dom(float dom)
    {
        KISMET_ASSERT(dom >= 0.0f && dom <= 1.0f);
        m_dom = dom;
    }

    /**
     * Get the mean of maximum, used for defuzzification
     */
    float get_mean_max() const
    {
        return m_mean_max;
    }
private:
    virtual float do_get_dom(float input) const = 0;

    /// Current level of confidence used for rule inference
    float m_dom;

    /// The mean of all elements whose dom are 1.0
    float m_mean_max;
};

} // namespace fuzzy
} // namespace kismet

#endif // KISMET_FUZZY_SET_H
