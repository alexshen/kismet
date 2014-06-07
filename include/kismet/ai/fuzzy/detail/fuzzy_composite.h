#ifndef KISMET_DETAIL_FUZZY_COMPOSITE_H
#define KISMET_DETAIL_FUZZY_COMPOSITE_H

#include <memory>
#include <cstddef>
#include "kismet/ai/fuzzy/fuzzy_term.h"

namespace kismet
{
namespace fuzzy
{
namespace detail
{

class fuzzy_composite : public fuzzy_term
{
public:
    enum { max_terms = 4 };

    void aggregate(float dom) override;

    void add(std::unique_ptr<fuzzy_term> term);
    
    std::size_t size() const;
protected:
    fuzzy_composite(fuzzy_composite const& rhs);

    template<typename F>
    void for_each(F f) const
    {
        for (auto& t : m_terms)
        {
            if (!t)
            { 
                break;
            }

            f(*t);
        }
    }
private:
    using fuzzy_term_ptr = std::unique_ptr<fuzzy_term>;

    fuzzy_term_ptr m_terms[max_terms];
};

} // namespace detail
} // namespace fuzzy
} // namespace kismet

#endif // KISMET_DETAIL_FUZZY_COMPOSITE_H
