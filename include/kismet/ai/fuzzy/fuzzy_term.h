#ifndef KISMET_FUZZY_TERM_H
#define KISMET_FUZZY_TERM_H

#include <memory>

namespace kismet
{
namespace fuzzy
{

class fuzzy_term;
using fuzzy_term_ptr = std::unique_ptr<fuzzy_term>;

class fuzzy_term
{
public:
    fuzzy_term() = default;

    virtual ~fuzzy_term() = default;

    /**
     * Return the dom of this fuzzy term
     */
    virtual float get_dom() const = 0;

    /**
     * Aggregate dom to form final shape of fuzzy variable
     */
    virtual void aggregate(float dom) = 0;

    virtual fuzzy_term_ptr clone() const = 0;
protected:
    // Allow derived class to implement clone
    fuzzy_term(fuzzy_term const& rhs) {}

    // Disallow assignment
    fuzzy_term& operator =(fuzzy_term&) = delete;
};

} // namespace fuzzy
} // namespace kismet

#endif // !KISMET_FUZZY_TERM_H
